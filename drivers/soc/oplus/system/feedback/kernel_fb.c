// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2018-2020 Oplus. All rights reserved.
 */
/***************************************************************
** File : kerbel_fb.c
** Description : BSP kevent fb data
** Version : 1.0
******************************************************************/

#include <linux/err.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#ifdef CONFIG_OPLUS_KEVENT_UPLOAD
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0))
#include <../../../arch/arm64/kernel/secureguard/rootguard/oplus_kevent.h>
#else
#include <linux/oplus_kevent.h>
#endif
#endif
#include <soc/oplus/system/kernel_fb.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MAX_LOG (31)
#define MAX_ID (19)
#define LIST_MAX (50)
#define DELAY_REPORT_US (30*1000*1000)
#define MAX_BUF_LEN (2048)

struct packets_pool {
	struct list_head packets;
	struct task_struct *flush_task;
	spinlock_t wlock;
	bool wlock_init;
};

struct packet {
	struct kernel_packet_info *pkt;
	struct list_head list;
	int retry;
};

static struct packets_pool *g_pkts = NULL;

static char *const _tag[FB_MAX_TYPE + 1] = {
	"fb_stability",
	"fb_fs",
	"fb_storage",
	"PSW_BSP_SENSOR",
	"fb_boot",
};

#ifdef CONFIG_OPLUS_KEVENT_UPLOAD
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)) && !IS_ENABLED(CONFIG_OPLUS_KERNEL_SECURE_GUARD)
int kevent_send_to_user(struct kernel_packet_info *userinfo) {return 0;}
#endif
#endif

static struct packet * package_alloc(
	fb_tag tag_id, const char *event_id, unsigned char *payload)
{
	struct packet *packet;
	struct kernel_packet_info *pkt;
	char *tmp = payload;

	if (tag_id > FB_MAX_TYPE || !event_id || !payload || !_tag[tag_id]) {
		return NULL;
        }

	packet = (struct packet *)kmalloc(sizeof(struct packet), GFP_ATOMIC);
	if (!packet) {
		return NULL;
	}

	/*presplit payload, '\n' is not allowed*/
	for(;*tmp != '\0'; tmp++) {
		if (*tmp == '\n') {
			*tmp = '\0';
			break;
		}
	}

	pkt = (struct kernel_packet_info *)
		kzalloc(sizeof(struct kernel_packet_info) + strlen(payload) + 1, GFP_ATOMIC);

	if (!pkt) {
		kfree(packet);
		return NULL;
	}

	packet->pkt = pkt;
	packet->retry = 4; /* retry 4 times at most*/
	pkt->type = 1; /*means only string is available*/

	memcpy(pkt->log_tag, _tag[tag_id], strlen(_tag[tag_id]) > MAX_LOG?MAX_LOG:strlen(_tag[tag_id]));
	memcpy(pkt->event_id, event_id, strlen(event_id) > MAX_ID?MAX_ID:strlen(event_id));
	pkt->payload_length = strlen(payload) + 1;
	memcpy(pkt->payload, payload, strlen(payload));

	return packet;
}

static void package_release(struct packet *packet)
{
	if (packet) {
		kfree(packet->pkt);
		kfree(packet);
	}
}

int oplus_kevent_fb(fb_tag tag_id, const char *event_id, unsigned char *payload)
{
	struct packet *packet;
	unsigned long flags;
	int ret = 0;

	/*ignore before wlock init*/
	if (!g_pkts->wlock_init) {
		return -ENODEV;
        }

	packet = package_alloc(tag_id, event_id, payload);
	if (!packet) {
		return -ENODEV;
        }

	ret = kevent_send_to_user(packet->pkt);
	if (!ret) {
		goto exit;
        }

	if (ret > 0) {
		spin_lock_irqsave(&g_pkts->wlock, flags);
		list_add(&packet->list, &g_pkts->packets);
		spin_unlock_irqrestore(&g_pkts->wlock, flags);

		wake_up_process(g_pkts->flush_task);
		return 0;
	}

exit:
	package_release(packet);

	return 0;
}


#define CAUSENAME_SIZE 128

static unsigned int BKDRHash(char *str, unsigned int len)
{
	unsigned int seed = 131;
        /* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	unsigned int i    = 0;

	if (str == NULL) {
		return 0;
	}

	for(i = 0; i < len; str++, i++) {
		hash = (hash * seed) + (*str);
	}

	return hash;
}

int oplus_kevent_fb_str(fb_tag tag_id, const char *event_id, unsigned char *str)
{
	unsigned char payload[1024] = {0x00};
	unsigned int hashid = 0;
	int ret = 0;
	char strHashSource[CAUSENAME_SIZE] = {0x00};

	snprintf(strHashSource, CAUSENAME_SIZE, "%s", str);
	hashid = BKDRHash(strHashSource, strlen(strHashSource));
	ret = snprintf(payload, sizeof(payload), "NULL$$EventField@@%d$$FieldData@@%s$$detailData@@%s",  hashid, str, _tag[tag_id]);
	pr_err("%s, payload= %s, ret=%d\n", __func__, payload, ret);
	return oplus_kevent_fb(tag_id, event_id, payload);
}

/*thread to deal with the buffer list*/
static int fb_flush_thread(void *arg)
{
	struct packets_pool *pkts_pool = (struct packets_pool *)arg;
	struct packet *tmp, *s;
	unsigned long flags;
	struct list_head list_tmp;

	while(!kthread_should_stop()) {
		if (list_empty(&pkts_pool->packets)) {
			set_current_state(TASK_INTERRUPTIBLE);
			schedule();
		}

		set_current_state(TASK_RUNNING);

		spin_lock_irqsave(&pkts_pool->wlock, flags);
		INIT_LIST_HEAD(&list_tmp);
		list_splice_init(&pkts_pool->packets, &list_tmp);
		spin_unlock_irqrestore(&pkts_pool->wlock, flags);

		list_for_each_entry_safe(s, tmp, &list_tmp, list) {
			if (s->pkt) {
				if (kevent_send_to_user(s->pkt) && s->retry) {
					pr_debug("failed to send feedback %s\n", s->pkt->log_tag);
					s->retry--;
					spin_lock_irqsave(&pkts_pool->wlock, flags);
					list_add(&s->list, &pkts_pool->packets);
					spin_unlock_irqrestore(&pkts_pool->wlock, flags);
				} else {
					package_release(s);
				}

				msleep(20);
			}
		}
	}

	return 0;
}

/*
* @format: tag_id:event_id:payload
*/
static ssize_t kernel_fb_write(struct file *file,
				const char __user *buf,
				size_t count,
				loff_t *lo)
{
	char *r_buf;
	int tag_id = 0;
	char event_id[MAX_ID] = {0};
	int idx1 = 0, idx2 = 0;
	int len;

	r_buf = (char *)kzalloc(MAX_BUF_LEN, GFP_KERNEL);
	if (!r_buf) {
		return count;
        }

	if (copy_from_user(r_buf, buf, MAX_BUF_LEN > count?count:MAX_BUF_LEN)) {
		goto exit;
	}

	r_buf[MAX_BUF_LEN - 1] ='\0'; /*make sure last bype is eof*/
	len = strlen(r_buf);

	tag_id = r_buf[0] - '0';
	if (tag_id > FB_MAX_TYPE || tag_id < 0) {
		goto exit;
	}

	while(idx1 < len) {
		if (r_buf[idx1++] == ':') {
			idx2 = idx1;
			while (idx2 < len) {
				if (r_buf[idx2++] == ':') {
					break;
                                }
			}
			break;
		}
	}

	if (idx1 == len || idx2 == len) {
		goto exit;
        }

	memcpy(event_id, &r_buf[idx1], idx2-idx1-1 > MAX_ID?MAX_ID: idx2-idx1-1);
	event_id[MAX_ID - 1] = '\0';

	oplus_kevent_fb(tag_id, event_id, r_buf + idx2);

exit:

	kfree(r_buf);
	return count;
}

static ssize_t kernel_fb_read(struct file *file,
				char __user *buf,
				size_t count,
				loff_t *ppos)
{
	return count;
}

static const struct file_operations kern_fb_fops = {
	.write = kernel_fb_write,
	.read  = kernel_fb_read,
	.open  = simple_open,
	.owner = THIS_MODULE,
};

static int __init kernel_fb_init(void)
{
	struct proc_dir_entry *d_entry = NULL;

	pr_err("%s\n", __func__);

	g_pkts = (struct packets_pool *)kzalloc(sizeof(struct packets_pool), GFP_KERNEL);
	if (!g_pkts) {
		return -ENOMEM;
        }

	INIT_LIST_HEAD(&g_pkts->packets);
	spin_lock_init(&g_pkts->wlock);

	g_pkts->flush_task = kthread_create(fb_flush_thread, g_pkts, "fb_flush");
	if (!g_pkts->flush_task) {
		return -ENODEV;
	}

	g_pkts->wlock_init = true;

	d_entry = proc_create_data("kern_fb", 0664, NULL, &kern_fb_fops, NULL);
	if (!d_entry) {
		pr_err("failed to create node\n");
		return -ENODEV;
	}

	return 0;
}

core_initcall(kernel_fb_init);

MODULE_LICENSE("GPL v2");

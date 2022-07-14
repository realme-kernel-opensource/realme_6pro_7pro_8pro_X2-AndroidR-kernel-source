/***************************************************************
** Copyright (C),  2018,  OPPO Mobile Comm Corp.,  Ltd
** VENDOR_EDIT
** File : oppo_mm_kevent_fb.h
** Description : MM kevent fb data
** Version : 1.0
** Date : 2018/12/03
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**   Guo.Ling          2018/12/03        1.0           Build this moudle
**   LiPing-M          2019/01/29        1.1           Add SMMU for QCOM
******************************************************************/
#ifndef _OPPO_MM_KEVENT_FB_
#define _OPPO_MM_KEVENT_FB_

#define MM_KEVENT_MAX_PAYLOAD_SIZE	150

enum {
	MM_KEY_RATELIMIT_NONE = 0,
	MM_KEY_RATELIMIT_30MIN = 60 * 30 * 1000,
	MM_KEY_RATELIMIT_1H = MM_KEY_RATELIMIT_30MIN * 2,
	MM_KEY_RATELIMIT_1DAY = MM_KEY_RATELIMIT_1H * 24,
};

enum OPPO_MM_DIRVER_FB_EVENT_ID {
	OPPO_MM_DIRVER_FB_EVENT_ID_ESD = 401,
	OPPO_MM_DIRVER_FB_EVENT_ID_VSYNC,
	OPPO_MM_DIRVER_FB_EVENT_ID_HBM,
	OPPO_MM_DIRVER_FB_EVENT_ID_FFLSET,
	OPPO_MM_DIRVER_FB_EVENT_ID_MTK_CMDQ,
	OPPO_MM_DIRVER_FB_EVENT_ID_MTK_UNDERFLOW,
	OPPO_MM_DIRVER_FB_EVENT_ID_MTK_FENCE,
	OPPO_MM_DIRVER_FB_EVENT_ID_MTK_JS,
	OPPO_MM_DIRVER_FB_EVENT_ID_SMMU,
	OPPO_MM_DIRVER_FB_EVENT_ID_GPU_FAULT,
	OPPO_MM_DIRVER_FB_EVENT_ID_PANEL_MATCH_FAULT,
	OPPO_MM_DIRVER_FB_EVENT_ID_ERROR = 420,
	OPPO_MM_DIRVER_FB_EVENT_ID_INFO = 421,
	OPPO_MM_DIRVER_FB_EVENT_ID_AUDIO = 801,
};

enum OPPO_MM_DIRVER_FB_EVENT_MODULE {
	OPPO_MM_DIRVER_FB_EVENT_MODULE_DISPLAY = 0,
	OPPO_MM_DIRVER_FB_EVENT_MODULE_AUDIO
};

int upload_mm_kevent_fb_data(enum OPPO_MM_DIRVER_FB_EVENT_MODULE module, unsigned char *payload);

int mm_kevent_upload(enum OPPO_MM_DIRVER_FB_EVENT_MODULE module, const char *name,
			 int rate_limit_ms, char *payload);
#define mm_kevent(m, name, rate_limit_ms, fmt, ...) \
	do { \
		char kv_data[MM_KEVENT_MAX_PAYLOAD_SIZE] = ""; \
		scnprintf(kv_data, sizeof(kv_data), fmt, ##__VA_ARGS__); \
		mm_kevent_upload(m, name, rate_limit_ms, kv_data); \
	} while (0)

#define mm_display_kevent(name, rate_limit_ms, fmt, ...) \
		mm_kevent(OPPO_MM_DIRVER_FB_EVENT_MODULE_DISPLAY, name, rate_limit_ms, fmt, ##__VA_ARGS__)

#define mm_display_kevent_named(rate_limit_ms, fmt, ...) \
	do { \
		char name[MM_KEVENT_MAX_PAYLOAD_SIZE]; \
		scnprintf(name, sizeof(name), "%s:%d", __func__, __LINE__); \
		mm_display_kevent(name, rate_limit_ms, fmt, ##__VA_ARGS__); \
	} while (0)

int mm_kevent_init(void);
void mm_kevent_deinit(void);

#endif /* _OPPO_MM_KEVENT_FB_ */


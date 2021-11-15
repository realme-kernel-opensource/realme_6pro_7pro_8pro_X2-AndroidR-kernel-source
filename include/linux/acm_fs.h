#ifndef __ACM_FS_H__
#define __ACM_FS_H__

#define ACM_DELETE_ERR  999

#define ACM_FLAG_LOGGING (0x01)
#define ACM_FLAG_DEL (0x01 << 1)
#define ACM_FLAG_CRT (0x01 << 2)

int acm_search(struct dentry *dentry, int file_type,
               int op);
int acm_opstat(int flag);

#endif /* __ACM_FS_H__ */

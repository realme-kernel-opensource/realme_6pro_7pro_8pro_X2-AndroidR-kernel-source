/***************************************************************
** Copyright (C),  2018,  OPPO Mobile Comm Corp.,  Ltd
** VENDOR_EDIT
** File : oppo_kevent_feedback.h
** Description : oppo kevent feedback data
** Version : 1.2
** Date : 2019/03/09
**
** ------------------------------- Revision History: -----------
**  <author>        <data>        <version >        <desc>
**   Guo.Ling          2018/12/03        1.0           Build this moudle
**   LiPing-M          2019/01/29        1.1           Add SMMU for QCOM
**   GaoTing.Gan       2019/03/08        1.2           Add venus dump feedback and public the interface
******************************************************************/
#ifndef _OPPO_MM_KEVENT_FB_
#define _OPPO_MM_KEVENT_FB_

enum OPPO_MM_DIRVER_FB_EVENT_ID {
	OPPO_MM_DIRVER_FB_EVENT_ID_VIDEO_DUMP = 351,
	OPPO_MM_DIRVER_FB_EVENT_ID_ESD = 401,
	OPPO_MM_DIRVER_FB_EVENT_ID_VSYNC,
	OPPO_MM_DIRVER_FB_EVENT_ID_HBM,
	OPPO_MM_DIRVER_FB_EVENT_ID_FFLSET,
	OPPO_MM_DIRVER_FB_EVENT_ID_MTK_CMDQ,
	OPPO_MM_DIRVER_FB_EVENT_ID_MTK_UNDERFLOW,
	OPPO_MM_DIRVER_FB_EVENT_ID_MTK_FENCE,
	OPPO_MM_DIRVER_FB_EVENT_ID_MTK_JS,
	OPPO_MM_DIRVER_FB_EVENT_ID_SMMU,
	OPPO_MM_DIRVER_FB_EVENT_ID_GPU_FAULT,/*410*/
	OPPO_MM_DIRVER_FB_EVENT_ID_PANEL_MATCH_FAULT,
	OPPO_MM_DIRVER_FB_EVENT_ID_GPU_FENCE_TIMEOUT,
	OPPO_MM_DIRVER_FB_EVENT_ID_ADSP_RESET = 801,
};

enum OPPO_MM_DIRVER_FB_EVENT_MODULE {
	OPPO_MM_DIRVER_FB_EVENT_MODULE_DISPLAY = 0,
	OPPO_MM_DIRVER_FB_EVENT_MODULE_AUDIO,
	OPPO_MM_DIRVER_FB_EVENT_MODULE_VIDEO
};

int upload_mm_kevent_feedback_data(enum OPPO_MM_DIRVER_FB_EVENT_MODULE module, unsigned char *payload);

#endif /* _OPPO_MM_KEVENT_FB_ */

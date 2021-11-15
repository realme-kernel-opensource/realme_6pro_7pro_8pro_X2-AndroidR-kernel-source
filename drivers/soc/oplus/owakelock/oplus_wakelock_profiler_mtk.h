/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2018-2020 Oplus. All rights reserved.
 */

#ifndef __OPLUS_WAKELOCK_PROFILER_H__
#define __OPLUS_WAKELOCK_PROFILER_H__


#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/wakeup_reason.h>
#include <linux/alarmtimer.h>
#include <linux/fb.h>


#define WS_CNT_MASK 0xffffffff
#define WS_CNT_POWERKEY (1<<0)
#define WS_CNT_RTCALARM (1<<1)
/* Not a wakeup source interrupt, but a non-net alarm wakeup */
#define WS_CNT_ALARM (1<<2)
#define WS_CNT_MODEM (1<<3)
#define WS_CNT_WLAN (1<<4)
#define WS_CNT_ADSP (1<<5)
#define WS_CNT_CDSP (1<<6)
#define WS_CNT_SLPI (1<<7)
#define WS_CNT_SENSOR (1<<8)
#define WS_CNT_OTHER (1<<9)

#define WS_CNT_GLINK (1<<13)
#define WS_CNT_ABORT (1<<14)
#define WS_CNT_SUM (1<<15)
#define WS_PMIC_IQR (1<<16)

#define WS_CNT_ALL WS_CNT_MASK

#ifdef CONFIG_OPPO_POWER_MTK

#define IRQ_NAME_WLAN_IPCC_DATA "wlan"
#define IRQ_NAME_WLAN_CONN2AP_DATA " R12_CONN2AP_SPM_WAKEUP_B"
#define IRQ_NAME_WLAN_CONNWDT_DATA " R12_CONN_WDT_IRQ_B"
#define IRQ_NAME_WLAN_MSI "msi_wlan_irq"


#define IRQ_NAME_MODEM_CCIF0 " R12_CCIF0_EVENT_B"
#define IRQ_NAME_MODEM_CCIF1 " R12_CCIF1_EVENT_B"
#define IRQ_NAME_MODEM_MD2AP " R12_MD2AP_PEER_WAKEUP_EVENT"
#define IRQ_NAME_MODEM_AP2AP " R12_AP2AP_PEER_WAKEUP_EVENT"
#define IRQ_NAME_MODEM_MD1 " R12_MD1_WDT_B"
#define IRQ_NAME_GLINK "glink" /* dummy irq name */



#define IRQ_NAME_ABORT "abort" /* dummy irq name */
#define IRQ_NAME_OTHER "other"


#define IRQ_NAME_RTCALARM "pm8xxx_rtc_alarm"
#define IRQ_NAME_ALARM    "alarm" /* dummy irq name */

#define IRQ_NAME_SENSOR_SCP " R12_SC_SCP2SPM_WAKEUP"
#define IRQ_NAME_SENSOR_MBOX "MBOX"

#define IRQ_NAME_ADSP "ADSP"
#define IRQ_NAME_ADSP_GLINK "glink-native-adsp"
#define IRQ_NAME_ADSP2SPM " R12_SC_ADSP2SPM_WAKEUP"
#define IRQ_NAME_AFE_IRQ_MCU " R12_AFE_IRQ_MCU_B"

#define IRQ_NAME_CDSP "cdsp"
#define IRQ_NAME_CDSP_GLINK "glink-native-cdsp"

#define IRQ_NAME_SLPI "slpi"
#define IRQ_NAME_SLPI_GLINK "glink-native-slpi"


#define IRQ_NAME_POWERKEY "MT6358_IRQ_PWRKEY"

#define IRQ_NAME_WAKE_SUM "wakeup_sum" /* dummy irq name */


/*define R13 clk info,according pcm_def.h*/
#define R13_SRCCLKENI0                        (1U << 0)
#define R13_SRCCLKENI1                        (1U << 1)
#define R13_MD_SRCCLKENA_0                    (1U << 2)
#define R13_MD_APSRC_REQ_0                    (1U << 3)
#define R13_CONN_DDR_EN                       (1U << 4)
#define R13_MD_SRCCLKENA_1                    (1U << 5)
#define R13_SSPM_SRCCLKENA                    (1U << 6)
#define R13_SSPM_APSRC_REQ                    (1U << 7)
#define R13_MD1_STATE                         (1U << 8)
#define R13_EMI_CLK_OFF_2_ACK                 (1U << 9)
#define R13_MM_STATE                          (1U << 10)
#define R13_SSPM_STATE                        (1U << 11)
#define R13_MD_DDR_EN_0                       (1U << 12)
#define R13_CONN_STATE                        (1U << 13)
#define R13_CONN_SRCCLKENA                    (1U << 14)
#define R13_CONN_APSRC_REQ                    (1U << 15)
#define R13_BIT16                             (1U << 16)
#define R13_BIT17                             (1U << 17)
#define R13_SCP_STATE                         (1U << 18)
#define R13_CSYSPWRUPREQ                      (1U << 19)
#define R13_PWRAP_SLEEP_ACK                   (1U << 20)
#define R13_EMI_CLK_OFF_ACK_ALL               (1U << 21)
#define R13_AUDIO_DSP_STATE                   (1U << 22)
#define R13_SW_DMDRAMCSHU_ACK_ALL             (1U << 23)
#define R13_CONN_SRCCLKENB                    (1U << 24)
#define R13_SC_DR_SRAM_LOAD_ACK_ALL           (1U << 25)
#define R13_INFRA_AUX_IDLE                    (1U << 26)
#define R13_DVFS_STATE                        (1U << 27)
#define R13_SC_DR_SRAM_PLL_LOAD_ACK_ALL       (1U << 28)
#define R13_SC_DR_SRAM_RESTORE_ACK_ALL        (1U << 29)
#define R13_MD_VRF18_REQ_0                    (1U << 30)
#define R13_DDR_EN_STATE                      (1U << 31)
#define OTHER_CLK                             (1U << 26)
#define all_no_sleep_clk_sum                  0xffffffff
#define all_sleep_clk_sum					  (1U << 26)
#define CLK_CNT_CLEAR						  0xffffffff


/*define R13 clk info,according pcm_def.h*/


/*irq name define in kernel-x.xx/include/linux/mfd/mt6358/core.h -> mt6358_irq_numbers*/
static const char *pmic_irq_name[82]={
	[0] =  "MT6358_IRQ_VPROC11_OC"		,
	[1] =  "MT6358_IRQ_VPROC12_OC"		,
	[2] =  "MT6358_IRQ_VCORE_OC"		,
	[3] =  "MT6358_IRQ_VGPU_OC" 		,
	[4] =  "MT6358_IRQ_VMODEM_OC"		,
	[5] =  "MT6358_IRQ_VDRAM1_OC"		,
	[6] =  "MT6358_IRQ_VS1_OC"			,
	[7] =  "MT6358_IRQ_VS2_OC"			,
	[8] =  "MT6358_IRQ_VPA_OC"			,
	[9] =  "MT6358_IRQ_VCORE_PREOC" 	,
	[10] =  "MT6358_IRQ_VFE28_OC"		,
	[11] =  "MT6358_IRQ_VXO22_OC"		,
	[12] =  "MT6358_IRQ_VRF18_OC"		,
	[13] =  "MT6358_IRQ_VRF12_OC"		,
	[14] =  "MT6358_IRQ_VEFUSE_OC"		,
	[15] =  "MT6358_IRQ_VCN33_OC"		,
	[16] =  "MT6358_IRQ_VCN28_OC"		,
	[17] =  "MT6358_IRQ_VCN18_OC"		,
	[18] =  "MT6358_IRQ_VCAMA1_OC"		,
	[19] =  "MT6358_IRQ_VCAMA2_OC"		,
	[20] =  "MT6358_IRQ_VCAMD_OC"		,
	[21] =  "MT6358_IRQ_VCAMIO_OC"		,
	[22] =  "MT6358_IRQ_VLDO28_OC"		,
	[23] =  "MT6358_IRQ_VA12_OC" 		,
	[24] =  "MT6358_IRQ_VAUX18_OC"		,
	[25] =  "MT6358_IRQ_VAUD28_OC"		,
	[26] =  "MT6358_IRQ_VIO28_OC"		,
	[27] =  "MT6358_IRQ_VIO18_OC"		,
	[28] =  "MT6358_IRQ_VSRAM_PROC11_OC" ,
	[29] =  "MT6358_IRQ_VSRAM_PROC12_OC" ,
	[30] =  "MT6358_IRQ_VSRAM_OTHERS_OC" ,
	[31] =  "MT6358_IRQ_VSRAM_GPU_OC"	,
	[32] =  "MT6358_IRQ_VDRAM2_OC"		,
	[33] =  "MT6358_IRQ_VMC_OC"			,
	[34] =  "MT6358_IRQ_VMCH_OC" 		,
	[35] =  "MT6358_IRQ_VEMC_OC" 		,
	[36] =  "MT6358_IRQ_VSIM1_OC"		,
	[37] =  "MT6358_IRQ_VSIM2_OC"		,
	[38] =  "MT6358_IRQ_VIBR_OC" 		,
	[39] =  "MT6358_IRQ_VUSB_OC" 		,
	[40] =  "MT6358_IRQ_VBIF28_OC"		,
	[41] =  "MT6358_IRQ_PWRKEY"			,
	[42] =  "MT6358_IRQ_HOMEKEY" 		,
	[43] =  "MT6358_IRQ_PWRKEY_R"		,
	[44] =  "MT6358_IRQ_HOMEKEY_R"		,
	[45] =  "MT6358_IRQ_NI_LBAT_INT" 	,
	[46] =  "MT6358_IRQ_CHRDET"			,
	[47] =  "MT6358_IRQ_CHRDET_EDGE" 	,
	[48] =  "MT6358_IRQ_VCDT_HV_DET" 	,
	[49] =  "MT6358_IRQ_RTC" 			,
	[50] =  "MT6358_IRQ_FG_BAT0_H"		,
	[51] =  "MT6358_IRQ_FG_BAT0_L"		,
	[52] =  "MT6358_IRQ_FG_CUR_H"		,
	[53] =  "MT6358_IRQ_FG_CUR_L"		,
	[54] =  "MT6358_IRQ_FG_ZCV"			,
	[55] =  "MT6358_IRQ_FG_BAT1_H"		,
	[56] =  "MT6358_IRQ_FG_BAT1_L"		,
	[57] =  "MT6358_IRQ_FG_N_CHARGE_L"	,
	[58] =  "MT6358_IRQ_FG_IAVG_H"		,
	[59] =  "MT6358_IRQ_FG_IAVG_L"		,
	[60] =  "MT6358_IRQ_FG_TIME_H"		,
	[61] =  "MT6358_IRQ_FG_DISCHARGE"	,
	[62] =  "MT6358_IRQ_FG_CHARGE"		,
	[63] =  "MT6358_IRQ_BATON_LV"		,
	[64] =  "MT6358_IRQ_BATON_HT"		,
	[65] =  "MT6358_IRQ_BATON_BAT_IN"	,
	[66] =  "MT6358_IRQ_BATON_BAT_OUT"	,
	[67] =  "MT6358_IRQ_BIF" 			,
	[68] =  "MT6358_IRQ_BAT_H"			,
	[69] =  "MT6358_IRQ_BAT_L"			,
	[70] =  "MT6358_IRQ_BAT2_H"			,
	[71] =  "MT6358_IRQ_BAT2_L"			,
	[72] =  "MT6358_IRQ_BAT_TEMP_H"		,
	[73] =  "MT6358_IRQ_BAT_TEMP_L"		,
	[74] =  "MT6358_IRQ_AUXADC_IMP"		,
	[75] =  "MT6358_IRQ_NAG_C_DLTV"		,
	[76] =  "MT6358_IRQ_AUDIO"			,
	[77] =  "MT6358_IRQ_ACCDET"			,
	[78] =  "MT6358_IRQ_ACCDET_EINT0"	,
	[79] =  "MT6358_IRQ_ACCDET_EINT1"	,
	[80] =  "MT6358_IRQ_SPI_CMD_ALERT"	,
	[81] =  "MT6358_IRQ_NR"				,

};
/*irq name define in kernel-x.xx/include/linux/mfd/mt6358/core.h -> mt6358_irq_numbers*/

#else
#define IRQ_NAME_POWERKEY "pon_kpdpwr_status"
#define IRQ_NAME_RTCALARM "pm8xxx_rtc_alarm"
#define IRQ_NAME_ALARM    "alarm" /* dummy irq name */
#define IRQ_NAME_MODEM_QSI "msi_modem_irq"
#define IRQ_NAME_MODEM_GLINK "glink-native-modem"
#define IRQ_NAME_MODEM_IPA "ipa"
#define IRQ_NAME_MODEM_QMI "qmi" /* dummy irq name */
#define IRQ_NAME_WLAN_MSI "msi_wlan_irq"
#define IRQ_NAME_WLAN_IPCC_DATA "WLAN"
#define IRQ_NAME_ADSP "adsp"
#define IRQ_NAME_ADSP_GLINK "glink-native-adsp"
#define IRQ_NAME_CDSP "cdsp"
#define IRQ_NAME_CDSP_GLINK "glink-native-cdsp"
#define IRQ_NAME_SLPI "slpi"
#define IRQ_NAME_SLPI_GLINK "glink-native-slpi"
#define IRQ_NAME_GLINK "glink" /* dummy irq name */
#define IRQ_NAME_ABORT "abort" /* dummy irq name */
#define IRQ_NAME_WAKE_SUM "wakeup_sum" /* dummy irq name */
#endif

#define MODULE_DETAIL_PRINT 0
#define MODULE_STATIS_PRINT 1

#define IRQ_PROP_MASK 0xff
/* real wakeup source irq name */
#define IRQ_PROP_REAL (1<<0)
/* exchanged wakeup source irq name */
#define IRQ_PROP_EXCHANGE (1<<1)
/* not wakeup source irq name, just used for statics*/
#define IRQ_PROP_DUMMY_STATICS (1<<2)

#if defined(CONFIG_OPPO_WAKELOCK_PROFILER) || defined(CONFIG_OPLUS_WAKELOCK_PROFILER)

int wakeup_reasons_statics(const char *irq_name, u64 choose_flag);
void wakeup_reasons_clear(u64 choose_flag);
void wakeup_reasons_print(u64 choose_flag, bool detail);
struct timespec64 print_utc_time(char *annotation);
void oplus_rpmh_stats_statics(const char *rpm_name,u64 sleep_count,u64 sleep_time);



void alarmtimer_suspend_flag_set(void);
void alarmtimer_suspend_flag_clear(void);
void alarmtimer_busy_flag_set(void);
void alarmtimer_busy_flag_clear(void);
void alarmtimer_wakeup_count(struct alarm *alarm);

void wakeup_get_start_time(void);
void wakeup_get_end_hold_time(void);
void clk_state_statics(const u32 clk_state,const u64 ap_sleep_time,const u64 deep_sleep_time,const char*static_type);


#else

static inline int wakeup_reasons_statics(const char *irq_name, u64 choose_flag) {return true;}
static inline void wakeup_reasons_clear(int choose_flag) {}
static inline void wakeup_reasons_print(int choose_flag, bool detail) {}

static inline void alarmtimer_suspend_flag_set(void) {}
static inline void alarmtimer_suspend_flag_clear(void) {}
static inline void alarmtimer_busy_flag_set(void) {}
static inline void alarmtimer_busy_flag_clear(void) {}
static inline void alarmtimer_wakeup_count(struct alarm *alarm) {}

static inline void wakeup_get_start_time(void) {}
static inline void wakeup_get_end_hold_time(void) {}

#endif

#endif  /* __OPLUS_WAKELOCK_PROFILER_H__ */

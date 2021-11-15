#ifndef _IPT_IMQ_H
#define _IPT_IMQ_H
/************************************************************************************
** File: - include/linux/netfilter_ipv4/ipt_IMQ.h
** VENDOR_EDIT
** Copyright (C), 2008-2020, OPLUS Mobile Comm Corp., Ltd
**
** Description:
**      1. Add for limit speed function
**
** Version: 1.0
** Date :   2020-03-20
** Author:  HuangJunyuan@CONNECTIVITY.WIFI.INTERNET
** TAG  :   OPLUS_FEATURE_WIFI_LIMMITBGSPEED
**
** ---------------------Revision History: ---------------------
**  <author>                      <data>     <version >   <desc>
** ---------------------------------------------------------------
**  HuangJunyuan@CONNECTIVITY.WIFI  2020/03/20  1.0          build this module
**
************************************************************************************/


/* Backwards compatibility for old userspace */
#include <linux/netfilter/xt_IMQ.h>

#define ipt_imq_info xt_imq_info

#endif /* _IPT_IMQ_H */


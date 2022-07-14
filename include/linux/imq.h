
#ifndef _IMQ_H
#define _IMQ_H

/************************************************************************************
** File: - include/linux/imq.h
** VENDOR_EDIT
** Copyright (C), 2008-2020, OPPO Mobile Comm Corp., Ltd
**
** Description:
**      1. Add for limit speed function
**
** Version: 1.0
** Date :   2020-03-20
** TAG  :   OPLUS_FEATURE_WIFI_LIMMITBGSPEED
**
** ---------------------Revision History: ---------------------
**  <author>                      <data>     <version >   <desc>
** ---------------------------------------------------------------
**
************************************************************************************/

/* IFMASK (16 device indexes, 0 to 15) and flag(s) fit in 5 bits */
#define IMQ_F_BITS	5

#define IMQ_F_IFMASK	0x0f
#define IMQ_F_ENQUEUE	0x10

#define IMQ_MAX_DEVS	(IMQ_F_IFMASK + 1)

#endif /* _IMQ_H */


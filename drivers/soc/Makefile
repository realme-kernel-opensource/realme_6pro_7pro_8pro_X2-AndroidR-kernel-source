# SPDX-License-Identifier: GPL-2.0
#
# Makefile for the Linux Kernel SOC specific device drivers.
#

obj-$(CONFIG_ARCH_ACTIONS)	+= actions/
obj-$(CONFIG_ARCH_AT91)		+= atmel/
obj-y				+= bcm/
obj-$(CONFIG_ARCH_DOVE)		+= dove/
obj-$(CONFIG_MACH_DOVE)		+= dove/
obj-y				+= fsl/
obj-$(CONFIG_ARCH_MXC)		+= imx/
obj-$(CONFIG_SOC_XWAY)		+= lantiq/
obj-$(CONFIG_ARCH_MEDIATEK)	+= mediatek/
obj-$(CONFIG_ARCH_MESON)	+= amlogic/
obj-$(CONFIG_ARCH_QCOM)		+= qcom/
obj-y				+= renesas/
obj-$(CONFIG_ARCH_ROCKCHIP)	+= rockchip/
obj-$(CONFIG_SOC_SAMSUNG)	+= samsung/
obj-$(CONFIG_ARCH_SUNXI)	+= sunxi/
obj-$(CONFIG_ARCH_TEGRA)	+= tegra/
obj-$(CONFIG_SOC_TI)		+= ti/
obj-$(CONFIG_ARCH_U8500)	+= ux500/
obj-$(CONFIG_PLAT_VERSATILE)	+= versatile/
obj-$(CONFIG_ARCH_ZX)		+= zte/
obj-y                           += oplus/

#ifdef OPLUS_FEATURE_IOMONITOR
obj-$(CONFIG_IOMONITOR)    +=  oplus/iomonitor/
#endif /*OPLUS_FEATURE_IOMONITOR*/

#ifdef VENDOR_EDIT
#/*ye.zhang@BSP.Bootloader.Bootflow, 2020-6-18, Add for oppo project*/
obj-y				+= oppo/
#endif
#ifdef OPLUS_FEATURE_POWERINFO_STANDBY
#SunFaliang@BSP.Power.Basic, 2020/07/09, add for oplus_wakelock_profile.
obj-$(CONFIG_OPLUS_WAKELOCK_PROFILER) += oplus/owakelock/
#endif /* OPLUS_FEATURE_POWERINFO_STANDBY */
#ifdef OPLUS_FEATURE_HEALTHINFO
#//wenbin.liu@PSW.Kernel.MM, 2018/05/23, Add for Healthinfo monitor
obj-y     +=  oplus/oppo_healthinfo/
#endif /* OPLUS_FEATURE_HEALTHINFO */
#ifdef OPLUS_FEATURE_SENSOR_SMEM
#ChenYajie@BSP.SENSOR, 2020/06/16, Add for sensor info
obj-y                     += oplus/sensor/
#endif
#Add for NandSwap
obj-$(CONFIG_NANDSWAP)		+= oplus/oplus_nandswap/

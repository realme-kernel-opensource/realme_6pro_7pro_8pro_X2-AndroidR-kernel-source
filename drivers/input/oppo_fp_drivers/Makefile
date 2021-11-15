obj-$(CONFIG_OPPO_FINGERPRINT) += oppo_fp_common/

### Capacitive fingerprint sensor
obj-$(CONFIG_OPPO_FINGERPRINT_FPC) += fpc/

obj-$(CONFIG_OPPO_FINGERPRINT_EGIS) += egis/

obj-$(CONFIG_OPPO_FINGERPRINT_CHIPONE) +=chipone/

### Optical fingerprint sensor
ifneq ($(CONFIG_OPPO_FINGERPRINT_GOODIX)_$(CONFIG_OPPO_FINGERPRINT_GOODIX_OPTICAL), _)
obj-y += goodix_optical_fp/
endif
obj-$(CONFIG_OPPO_FINGERPRINT_SILEAD_OPTICAL) += silead_optical_fp/
obj-$(CONFIG_OPPO_FINGERPRINT_EGIS_OPTICAL)   += egis_optical_fp/
obj-$(CONFIG_OPPO_FINGERPRINT_SILEAD) += silead/
obj-$(CONFIG_OPPO_FINGERPRINT_JIIOV_OPTICAL)  += anc_driver/

### FEATURE MACRO
ccflags-$(CONFIG_OPPO_FINGERPRINT_NAVIGATION_ENABLE) += -DFP_NAVIGATION_ENABLE

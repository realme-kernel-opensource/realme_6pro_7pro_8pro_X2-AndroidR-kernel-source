#
# Makefile for kernel Egis Fingerprint drivers.
#
ifeq ($(CONFIG_OPPO_FINGERPRINT_QCOM),y)
obj-y += egis_qcom.o
obj-y +=ets_navi_input_qcom.o
else ifeq ($(CONFIG_OPPO_FINGERPRINT_MTK),y)
obj-y += egis.o
obj-y +=ets_navi_input.o
endif
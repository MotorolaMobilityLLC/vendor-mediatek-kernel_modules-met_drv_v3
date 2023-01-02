LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := met_sspm_api.ko
LOCAL_STRIP_MODULE := true

ifneq ("$(wildcard $(LOCAL_PATH)/../../../met_drv_secure_v3)","")
	LOCAL_REQUIRED_MODULES := met.ko met_plf.ko
else
	LOCAL_REQUIRED_MODULES := met.ko
endif

include $(MTK_KERNEL_MODULE)
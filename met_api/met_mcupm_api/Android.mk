LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := met_mcupm_api.ko
LOCAL_STRIP_MODULE := true

LOCAL_REQUIRED_MODULES := met.ko

include $(MTK_KERNEL_MODULE)
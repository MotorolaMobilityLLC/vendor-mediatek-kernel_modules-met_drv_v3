LOCAL_PATH := $(call my-dir)

# kernel version check
KERNEL_VERSION := $(word 2,$(subst -, ,$(LINUX_KERNEL_VERSION)))
MAJOR_VERSION := $(shell echo $(KERNEL_VERSION) | cut -f1 -d.)
MINOR_VERSION := $(shell echo $(KERNEL_VERSION) | cut -f2 -d.)
VERSION_CHECK := $(shell test $(MAJOR_VERSION) -ge 5 -a $(MINOR_VERSION) -ge 10 && echo true)

ifeq ($(VERSION_CHECK), true)

include $(CLEAR_VARS)
LOCAL_MODULE := met.ko
LOCAL_STRIP_MODULE := true

include $(MTK_KERNEL_MODULE)

endif # Kernel version >= 4.19

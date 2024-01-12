ORIGIN_SRC := $(src)
override src := $(src)/../..

include $(src)/Makefile

override src := $(ORIGIN_SRC)

ifneq ("$(wildcard $(src)/../../../met_drv_secure_v3)","")
	ccflags-y += -DMET_PLF_EXIST
endif

obj-$(CONFIG_MTK_MET) := met_sspm_api.o
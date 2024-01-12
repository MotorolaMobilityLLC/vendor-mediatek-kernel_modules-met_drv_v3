ORIGIN_SRC := $(src)
override src := $(src)/../..

include $(src)/Makefile

override src := $(ORIGIN_SRC)

obj-$(CONFIG_MTK_MET) := met_backlight_api.o
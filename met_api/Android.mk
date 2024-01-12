MET_API_ROOT_DIR := $(call my-dir)

include $(MET_API_ROOT_DIR)/met_gpu_api/Android.mk
include $(MET_API_ROOT_DIR)/met_gpu_adv_api/Android.mk
include $(MET_API_ROOT_DIR)/met_vcore_api/Android.mk
include $(MET_API_ROOT_DIR)/met_backlight_api/Android.mk
include $(MET_API_ROOT_DIR)/met_emi_api/Android.mk
include $(MET_API_ROOT_DIR)/met_sspm_api/Android.mk
include $(MET_API_ROOT_DIR)/met_mcupm_api/Android.mk
include $(MET_API_ROOT_DIR)/met_ipi_api/Android.mk
include $(MET_API_ROOT_DIR)/met_scmi_api/Android.mk

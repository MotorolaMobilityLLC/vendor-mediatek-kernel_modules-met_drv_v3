/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __CORE_PLF_INIT_H__
#define __CORE_PLF_INIT_H__

extern struct miscdevice met_device;

/*
 *   MET External Symbol
 */

#ifdef MET_GPU
/*
 *   GPU
 */
#include <mtk_gpu_utility.h>


extern bool mtk_get_gpu_loading(unsigned int *pLoading);
extern bool mtk_get_gpu_block(unsigned int *pBlock);
extern bool mtk_get_gpu_idle(unsigned int *pIdle);
extern bool mtk_get_gpu_memory_usage(unsigned int *pMemUsage);
extern bool mtk_get_gpu_pmu_init(struct GPU_PMU *pmus, int pmu_size, int *ret_size);
extern bool mtk_get_gpu_pmu_swapnreset(struct GPU_PMU *pmus, int pmu_size);
extern bool mtk_get_gpu_pmu_deinit(void);
extern bool mtk_get_gpu_pmu_swapnreset_stop(void);

extern bool (*mtk_get_gpu_loading_symbol)(unsigned int *pLoading);
extern bool (*mtk_get_gpu_block_symbol)(unsigned int *pBlock);
extern bool (*mtk_get_gpu_idle_symbol)(unsigned int *pIdle);
extern bool (*mtk_get_gpu_memory_usage_symbol)(unsigned int *pMemUsage);
extern bool (*mtk_get_gpu_pmu_init_symbol)(struct GPU_PMU *pmus, int pmu_size, int *ret_size);
extern bool (*mtk_get_gpu_pmu_swapnreset_symbol)(struct GPU_PMU *pmus, int pmu_size);
extern bool (*mtk_get_gpu_pmu_deinit_symbol)(void);
extern bool (*mtk_get_gpu_pmu_swapnreset_stop_symbol)(void);

extern bool mtk_register_gpu_power_change(const char *name, void (*callback)(int power_on));
extern bool mtk_unregister_gpu_power_change(const char *name);
extern bool (*mtk_register_gpu_power_change_symbol)(const char *name,
					void (*callback)(int power_on));
extern bool (*mtk_unregister_gpu_power_change_symbol)(const char *name);


extern bool mtk_get_gpu_cur_freq(unsigned int *puiFreq);
extern bool mtk_get_gpu_cur_oppidx(int *piIndex);
extern bool mtk_get_gpu_floor_index(int *piIndex);
extern bool mtk_get_gpu_ceiling_index(int *piIndex);
extern bool mtk_get_gpu_floor_limiter(unsigned int *puiLimiter);
extern bool mtk_get_gpu_ceiling_limiter(unsigned int *puiLimiter);

extern bool (*mtk_get_gpu_cur_freq_symbol)(unsigned int *puiFreq);
extern bool (*mtk_get_gpu_cur_oppidx_symbol)(int *piIndex);
extern bool (*mtk_get_gpu_floor_index_symbol)(int *piIndex);
extern bool (*mtk_get_gpu_ceiling_index_symbol)(int *piIndex);
extern bool (*mtk_get_gpu_floor_limiter_symbol)(unsigned int *puiLimiter);
extern bool (*mtk_get_gpu_ceiling_limiter_symbol)(unsigned int *puiLimiter);


extern struct metdevice met_gpu;
extern struct metdevice met_gpudvfs;
extern struct metdevice met_gpumem;
extern struct metdevice met_gpupwr;
extern struct metdevice met_gpu_pmu;
#ifdef MET_GPU_STALL_MONITOR
extern struct metdevice met_gpu_stall;
#endif
#endif /* MET_GPU */


#ifdef MET_VCOREDVFS
/*
 *   VCORE DVFS
 */
extern int vcorefs_get_num_opp(void);
extern int  vcorefs_get_opp_info_num(void);
extern char ** vcorefs_get_opp_info_name(void);
extern unsigned int * vcorefs_get_opp_info(void);
extern int  vcorefs_get_src_req_num(void);
extern char ** vcorefs_get_src_req_name(void);
extern unsigned int * vcorefs_get_src_req(void);

extern int (*vcorefs_get_num_opp_symbol)(void);
extern int  (*vcorefs_get_opp_info_num_symbol)(void);
extern char ** (*vcorefs_get_opp_info_name_symbol)(void);
extern unsigned int * (*vcorefs_get_opp_info_symbol)(void);
extern int  (*vcorefs_get_src_req_num_symbol)(void);
extern char ** (*vcorefs_get_src_req_name_symbol)(void);
extern unsigned int * (*vcorefs_get_src_req_symbol)(void);


#ifdef VCOREDVFS_OLD_VER

#include <mtk_spm.h>
#include <mtk_vcorefs_manager.h>

extern char *governor_get_kicker_name(int id);
extern int vcorefs_enable_debug_isr(bool);

extern u32 (*spm_vcorefs_get_MD_status_symbol)(void);
extern void (*spm_vcorefs_register_handler_symbol)(vcorefs_handler_t handler);
extern void (*vcorefs_register_req_notify_symbol)(vcorefs_req_handler_t handler);
extern char *(*governor_get_kicker_name_symbol)(int id);
extern int (*vcorefs_enable_debug_isr_symbol)(bool);
extern int (*vcorefs_get_hw_opp_symbol)(void);
extern int (*vcorefs_get_curr_vcore_symbol)(void);
extern int (*vcorefs_get_curr_ddr_symbol)(void);
extern int *kicker_table_symbol;

#endif /* VCOREDVFS_OLD_VER */

extern struct metdevice met_vcoredvfs;

#endif /* MET_VCOREDVFS */


#ifdef MET_EMI
extern unsigned int mtk_dramc_get_data_rate(void);      /* in Mhz */
extern unsigned int mtk_dramc_get_ddr_type(void);
extern int get_cur_ddr_ratio(void);

extern unsigned int (*mtk_dramc_get_data_rate_symbol)(void); /* in Mhz */
extern unsigned int (*mtk_dramc_get_ddr_type_symbol)(void);
extern int (*get_cur_ddr_ratio_symbol)(void);

extern struct metdevice met_sspm_emi;
#endif /* MET_EMI */

#ifdef MET_SMI
extern struct metdevice met_sspm_smi;
#endif

#ifdef MET_PTPOD
#include <mtk_gpufreq.h>
#include <mach/mtk_cpufreq_api.h>
#include <mtk_cpufreq_config.h>

extern unsigned int mt_gpufreq_get_cur_volt(void);
extern unsigned int mt_cpufreq_get_cur_volt(unsigned int cluster_id);
extern unsigned int (*mt_gpufreq_get_cur_volt_symbol)(void);
extern unsigned int (*mt_cpufreq_get_cur_volt_symbol)(unsigned int cluster_id);

extern struct metdevice met_ptpod;
#endif /* MET_PTPOD */

#ifdef MET_WALLTIME
extern struct metdevice met_wall_time;
#endif

#ifdef MTK_TINYSYS_SSPM_SUPPORT
#if defined(ONDIEMET_SUPPORT) || defined(TINYSYS_SSPM_SUPPORT)
extern struct metdevice met_sspm_common;
#endif
#endif /* MTK_TINYSYS_SSPM_SUPPORT */

#if IS_ENABLED(CONFIG_MTK_TINYSYS_SSPM_SUPPORT)
#if defined(ONDIEMET_SUPPORT) || defined(TINYSYS_SSPM_SUPPORT)
#ifdef MET_SSPM_WALLTIME
extern struct metdevice met_sspm_walltime;
#endif
#endif
#endif /* CONFIG_MTK_TINYSYS_SSPM_SUPPORT */

#ifdef MET_CPUDSU
extern struct metdevice met_cpudsu;
#endif

#ifdef MET_SPM_TWAM
#include "mtk_spm.h"

/* ap side used */
#ifdef SPMTWAM_AP
extern void spm_twam_enable_monitor(const struct twam_sig *twamsig, bool speed_mode);
extern void spm_twam_register_handler(twam_handler_t handler);
extern void spm_twam_disable_monitor(void);
extern void spm_twam_set_idle_select(unsigned int sel);
extern void spm_twam_set_window_length(unsigned int len);
extern void spm_twam_set_mon_type(struct twam_sig *mon);

extern void (*spm_twam_enable_monitor_symbol)(const struct twam_sig *twamsig, bool speed_mode);
extern void (*spm_twam_register_handler_symbol)(twam_handler_t handler);
extern void (*spm_twam_disable_monitor_symbol)(void);
extern void (*spm_twam_set_idle_select_symbol)(unsigned int sel);
extern void (*spm_twam_set_window_length_symbol)(unsigned int len);
extern void (*spm_twam_set_mon_type_symbol)(struct twam_sig *mon);
#endif

/* sspm side used */
#ifdef SPMTWAM_SSPM
extern void spm_twam_enable_monitor(bool en_monitor, bool debug_signal, twam_handler_t cb_handler);
extern bool spm_twam_met_enable(void);
extern void spm_twam_config_channel(struct twam_cfg *cfg, bool speed_mode, unsigned int window_len_hz);

extern void (*spm_twam_enable_monitor_symbol)(bool en_monitor, bool debug_signal, twam_handler_t cb_handler);
extern bool (*spm_twam_met_enable_symbol)(void);
extern void (*spm_twam_config_channel_symbol)(struct twam_cfg *cfg, bool speed_mode, unsigned int window_len_hz);
#endif

extern struct metdevice met_spmtwam;
#endif


#endif /*__CORE_PLF_INIT_H__*/

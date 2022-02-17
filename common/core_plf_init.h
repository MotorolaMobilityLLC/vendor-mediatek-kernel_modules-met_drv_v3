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


#ifdef MET_GPU_LOAD_MONITOR
extern bool mtk_get_gpu_loading(unsigned int *pLoading);
extern bool mtk_get_gpu_block(unsigned int *pBlock);
extern bool mtk_get_gpu_idle(unsigned int *pIdle);

extern bool (*mtk_get_gpu_loading_symbol)(unsigned int *pLoading);
extern bool (*mtk_get_gpu_block_symbol)(unsigned int *pBlock);
extern bool (*mtk_get_gpu_idle_symbol)(unsigned int *pIdle);

extern struct metdevice met_gpu;
#endif

#ifdef MET_GPU_MEM_MONITOR
extern bool mtk_get_gpu_memory_usage(unsigned int *pMemUsage);

extern bool (*mtk_get_gpu_memory_usage_symbol)(unsigned int *pMemUsage);

extern struct metdevice met_gpumem;
#endif

#ifdef MET_GPU_PMU_MONITOR
extern bool mtk_get_gpu_pmu_init(struct GPU_PMU *pmus, int pmu_size, int *ret_size);
extern bool mtk_get_gpu_pmu_swapnreset(struct GPU_PMU *pmus, int pmu_size);
extern bool mtk_get_gpu_pmu_deinit(void);
extern bool mtk_get_gpu_pmu_swapnreset_stop(void);

extern bool (*mtk_get_gpu_pmu_init_symbol)(struct GPU_PMU *pmus, int pmu_size, int *ret_size);
extern bool (*mtk_get_gpu_pmu_swapnreset_symbol)(struct GPU_PMU *pmus, int pmu_size);
extern bool (*mtk_get_gpu_pmu_deinit_symbol)(void);
extern bool (*mtk_get_gpu_pmu_swapnreset_stop_symbol)(void);

extern struct metdevice met_gpu_pmu;
#endif

#ifdef MET_GPU_DVFS_MONITOR
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

extern struct metdevice met_gpudvfs;
#endif

#ifdef MET_GPU_PWR_MONITOR
extern struct metdevice met_gpupwr;
#endif

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

extern struct metdevice met_vcoredvfs;

#endif /* MET_VCOREDVFS */


#ifdef MET_EMI
#if IS_ENABLED(CONFIG_MTK_DRAMC)
extern unsigned int mtk_dramc_get_data_rate(void);      /* in Mhz */
extern unsigned int mtk_dramc_get_ddr_type(void);

extern unsigned int (*mtk_dramc_get_data_rate_symbol)(void); /* in Mhz */
extern unsigned int (*mtk_dramc_get_ddr_type_symbol)(void);
#endif
#if IS_ENABLED(CONFIG_MTK_DVFSRC_MET)
extern int get_cur_ddr_ratio(void);

extern int (*get_cur_ddr_ratio_symbol)(void);
#endif

extern struct metdevice met_sspm_emi;
#endif /* MET_EMI */

#ifdef MET_SMI
extern struct metdevice met_sspm_smi;
#endif

#ifdef MET_WALLTIME
extern struct metdevice met_wall_time;
#endif

#ifdef MET_SSPM
extern struct metdevice met_sspm_common;
#endif

#ifdef MET_CPUDSU
extern struct metdevice met_cpudsu;
#endif

#ifdef MET_THERMAL
extern struct metdevice met_thermal;
#endif

#ifdef MET_BACKLIGHT
extern int mtk_leds_register_notifier(struct notifier_block *nb);
extern int mtk_leds_unregister_notifier(struct notifier_block *nb);

extern int (*mtk_leds_register_notifier_symbol)(struct notifier_block *nb);
extern int (*mtk_leds_unregister_notifier_symbol)(struct notifier_block *nb);
extern struct metdevice met_backlight;
#endif

#endif /*__CORE_PLF_INIT_H__*/

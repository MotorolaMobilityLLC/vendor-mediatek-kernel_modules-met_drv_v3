// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/kallsyms.h>
#include "met_drv.h"
#include "interface.h"
#include "core_plf_init.h"
#include "met_kernel_symbol.h"

#undef	DEBUG

#ifdef MET_GPU
/*
 *   GPU
 */
#ifdef MET_GPU_LOAD_MONITOR
bool (*mtk_get_gpu_loading_symbol)(unsigned int *pLoading);
bool (*mtk_get_gpu_block_symbol)(unsigned int *pBlock);
bool (*mtk_get_gpu_idle_symbol)(unsigned int *pIdle);
#endif
#ifdef MET_GPU_MEM_MONITOR
bool (*mtk_get_gpu_memory_usage_symbol)(unsigned int *pMemUsage);
#endif
#ifdef MET_GPU_PMU_MONITOR
bool (*mtk_get_gpu_pmu_init_symbol)(struct GPU_PMU *pmus, int pmu_size, int *ret_size);
bool (*mtk_get_gpu_pmu_swapnreset_symbol)(struct GPU_PMU *pmus, int pmu_size);
bool (*mtk_get_gpu_pmu_deinit_symbol)(void);
bool (*mtk_get_gpu_pmu_swapnreset_stop_symbol)(void);
#endif
#ifdef MET_GPU_DVFS_MONITOR
bool (*mtk_get_gpu_cur_freq_symbol)(unsigned int *puiFreq);
bool (*mtk_get_gpu_cur_oppidx_symbol)(int *piIndex);
bool (*mtk_get_gpu_floor_index_symbol)(int *piIndex);
bool (*mtk_get_gpu_ceiling_index_symbol)(int *piIndex);
bool (*mtk_get_gpu_floor_limiter_symbol)(unsigned int *puiLimiter);
bool (*mtk_get_gpu_ceiling_limiter_symbol)(unsigned int *puiLimiter);
#endif
#endif /* MET_GPU */

#ifdef MET_VCOREDVFS
/*
 *   VCORE DVFS
 */
#include <dvfsrc-exp.h>
int  (*vcorefs_get_opp_info_num_symbol)(void);
char ** (*vcorefs_get_opp_info_name_symbol)(void);
unsigned int * (*vcorefs_get_opp_info_symbol)(void);
int  (*vcorefs_get_src_req_num_symbol)(void);
char ** (*vcorefs_get_src_req_name_symbol)(void);
unsigned int * (*vcorefs_get_src_req_symbol)(void);
int (*vcorefs_get_num_opp_symbol)(void);
#endif /* MET_VCOREDVFS */

#ifdef MET_EMI
#if IS_ENABLED(CONFIG_MTK_DRAMC)
unsigned int (*mtk_dramc_get_data_rate_symbol)(void);
unsigned int (*mtk_dramc_get_ddr_type_symbol)(void);
#endif
#if IS_ENABLED(CONFIG_MTK_DVFSRC_MET)
int (*get_cur_ddr_ratio_symbol)(void);
#endif
#endif /* MET_EMI */

#ifdef MET_BACKLIGHT
int (*mtk_leds_register_notifier_symbol)(struct notifier_block *nb);
int (*mtk_leds_unregister_notifier_symbol)(struct notifier_block *nb);
#endif

static int met_symbol_get(void)
{
#ifdef MET_GPU
#ifdef MET_GPU_LOAD_MONITOR
	_MET_SYMBOL_GET(mtk_get_gpu_loading);
	_MET_SYMBOL_GET(mtk_get_gpu_block);
	_MET_SYMBOL_GET(mtk_get_gpu_idle);
#endif
#ifdef MET_GPU_MEM_MONITOR
	_MET_SYMBOL_GET(mtk_get_gpu_memory_usage);
#endif
#ifdef MET_GPU_PMU_MONITOR
	_MET_SYMBOL_GET(mtk_get_gpu_pmu_init);
	_MET_SYMBOL_GET(mtk_get_gpu_pmu_swapnreset);
	_MET_SYMBOL_GET(mtk_get_gpu_pmu_deinit);
	_MET_SYMBOL_GET(mtk_get_gpu_pmu_swapnreset_stop);
#endif
#ifdef MET_GPU_DVFS_MONITOR
	_MET_SYMBOL_GET(mtk_get_gpu_cur_freq);
	_MET_SYMBOL_GET(mtk_get_gpu_cur_oppidx);
	_MET_SYMBOL_GET(mtk_get_gpu_floor_index);
	_MET_SYMBOL_GET(mtk_get_gpu_ceiling_index);
	_MET_SYMBOL_GET(mtk_get_gpu_floor_limiter);
	_MET_SYMBOL_GET(mtk_get_gpu_ceiling_limiter);
#endif
#endif /* MET_GPU */

#ifdef MET_VCOREDVFS
	_MET_SYMBOL_GET(vcorefs_get_num_opp);
	_MET_SYMBOL_GET(vcorefs_get_opp_info_num);
	_MET_SYMBOL_GET(vcorefs_get_opp_info_name);
	_MET_SYMBOL_GET(vcorefs_get_opp_info);
	_MET_SYMBOL_GET(vcorefs_get_src_req_num);
	_MET_SYMBOL_GET(vcorefs_get_src_req_name);
	_MET_SYMBOL_GET(vcorefs_get_src_req);
#endif

#ifdef MET_EMI
#if IS_ENABLED(CONFIG_MTK_DRAMC)
	_MET_SYMBOL_GET(mtk_dramc_get_data_rate);
	_MET_SYMBOL_GET(mtk_dramc_get_ddr_type);
#endif
#if IS_ENABLED(CONFIG_MTK_DVFSRC_MET)
	_MET_SYMBOL_GET(get_cur_ddr_ratio);
#endif
#endif

#ifdef MET_BACKLIGHT
	_MET_SYMBOL_GET(mtk_leds_register_notifier);
	_MET_SYMBOL_GET(mtk_leds_unregister_notifier);
#endif

	return 0;
}

static int met_symbol_put(void)
{
#ifdef MET_GPU
#ifdef MET_GPU_LOAD_MONITOR
	_MET_SYMBOL_PUT(mtk_get_gpu_loading);
	_MET_SYMBOL_PUT(mtk_get_gpu_block);
	_MET_SYMBOL_PUT(mtk_get_gpu_idle);
#endif
#ifdef MET_GPU_MEM_MONITOR
	_MET_SYMBOL_PUT(mtk_get_gpu_memory_usage);
#endif
#ifdef MET_GPU_PMU_MONITOR
	_MET_SYMBOL_PUT(mtk_get_gpu_pmu_init);
	_MET_SYMBOL_PUT(mtk_get_gpu_pmu_swapnreset);
	_MET_SYMBOL_PUT(mtk_get_gpu_pmu_deinit);
	_MET_SYMBOL_PUT(mtk_get_gpu_pmu_swapnreset_stop);
#endif
#ifdef MET_GPU_DVFS_MONITOR
	_MET_SYMBOL_PUT(mtk_get_gpu_cur_freq);
	_MET_SYMBOL_PUT(mtk_get_gpu_cur_oppidx);
	_MET_SYMBOL_PUT(mtk_get_gpu_floor_index);
	_MET_SYMBOL_PUT(mtk_get_gpu_ceiling_index);
	_MET_SYMBOL_PUT(mtk_get_gpu_floor_limiter);
	_MET_SYMBOL_PUT(mtk_get_gpu_ceiling_limiter);
#endif
#endif /* MET_GPU */

#ifdef MET_VCOREDVFS
	_MET_SYMBOL_PUT(vcorefs_get_num_opp);
	_MET_SYMBOL_PUT(vcorefs_get_opp_info_num);
	_MET_SYMBOL_PUT(vcorefs_get_opp_info_name);
	_MET_SYMBOL_PUT(vcorefs_get_opp_info);
	_MET_SYMBOL_PUT(vcorefs_get_src_req_num);
	_MET_SYMBOL_PUT(vcorefs_get_src_req_name);
	_MET_SYMBOL_PUT(vcorefs_get_src_req);
#endif

#ifdef MET_EMI
#if IS_ENABLED(CONFIG_MTK_DRAMC)
	_MET_SYMBOL_PUT(mtk_dramc_get_data_rate);
	_MET_SYMBOL_PUT(mtk_dramc_get_ddr_type);
#endif
#if IS_ENABLED(CONFIG_MTK_DVFSRC_MET)
	_MET_SYMBOL_PUT(get_cur_ddr_ratio);
#endif
#endif

#ifdef MET_BACKLIGHT
	_MET_SYMBOL_PUT(mtk_leds_register_notifier);
	_MET_SYMBOL_PUT(mtk_leds_unregister_notifier);
#endif

	return 0;
}

int core_plf_init(void)
{
	/*initial met external symbol*/
	met_symbol_get();

#ifdef MET_GPU
#ifdef MET_GPU_LOAD_MONITOR
	met_register(&met_gpu);
#endif
#ifdef MET_GPU_DVFS_MONITOR
    met_register(&met_gpudvfs);
#endif
#ifdef MET_GPU_MEM_MONITOR
	met_register(&met_gpumem);
#endif
#ifdef MET_GPU_PWR_MONITOR
	met_register(&met_gpupwr);
#endif
#ifdef MET_GPU_PMU_MONITOR
	met_register(&met_gpu_pmu);
#endif
#ifdef MET_GPU_STALL_MONITOR
	met_register(&met_gpu_stall);
#endif
#ifdef MET_GPU_STALL_MONITOR
	met_register(&met_gpu_stall);
#endif
#endif

#ifdef MET_VCOREDVFS
	met_register(&met_vcoredvfs);
#endif

#ifdef MET_EMI
	met_register(&met_sspm_emi);
#endif

#ifdef MET_SMI
	met_register(&met_sspm_smi);
#endif

#ifdef MET_WALLTIME
	met_register(&met_wall_time);
#endif

#ifdef MET_SSPM
	met_register(&met_sspm_common);
#endif

#ifdef MET_CPUDSU
	met_register(&met_cpudsu);
#endif

#ifdef MET_BACKLIGHT
	met_register(&met_backlight);
#endif

#ifdef MET_THERMAL
	met_register(&met_thermal);
#endif

#ifdef MET_PTPOD
	met_register(&met_ptpod);
#endif

	return 0;
}

void core_plf_exit(void)
{
	/*release met external symbol*/
	met_symbol_put();

#ifdef MET_GPU
#ifdef MET_GPU_LOAD_MONITOR
	met_deregister(&met_gpu);
#endif
#ifdef MET_GPU_DVFS_MONITOR
    met_deregister(&met_gpudvfs);
#endif
#ifdef MET_GPU_MEM_MONITOR
	met_deregister(&met_gpumem);
#endif
#ifdef MET_GPU_PWR_MONITOR
	met_deregister(&met_gpupwr);
#endif
#ifdef MET_GPU_PMU_MONITOR
	met_deregister(&met_gpu_pmu);
#endif
#ifdef MET_GPU_STALL_MONITOR
	met_deregister(&met_gpu_stall);
#endif
#endif

#ifdef MET_VCOREDVFS
	met_deregister(&met_vcoredvfs);
#endif

#ifdef MET_EMI
	met_deregister(&met_sspm_emi);
#endif

#ifdef MET_SMI
	met_deregister(&met_sspm_smi);
#endif

#ifdef MET_WALLTIME
	met_deregister(&met_wall_time);
#endif

#ifdef MET_SSPM
	met_deregister(&met_sspm_common);
#endif

#ifdef MET_CPUDSU
	met_deregister(&met_cpudsu);
#endif

#ifdef MET_BACKLIGHT
	met_deregister(&met_backlight);
#endif

#ifdef MET_THERMAL
	met_deregister(&met_thermal);
#endif

#ifdef MET_PTPOD
	met_deregister(&met_ptpod);
#endif
}

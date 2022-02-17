// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 */
/*****************************************************************************
 * headers
 *****************************************************************************/
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/signal.h>
#include <linux/semaphore.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <asm/io.h>  /* for ioremap and iounmap */

#include "sspm_met_log.h" /* for sspm_ipidev_symbol */
#include "sspm_met_ipi_handle.h"
#include "interface.h"
#include "core_plf_init.h"
#include "tinysys_sspm.h"
#include "tinysys_mgr.h" /* for ondiemet_module */
#include "met_kernel_symbol.h"

#include <linux/scmi_protocol.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/device.h>

#include "tinysys-scmi.h"

/*****************************************************************************
 * define declaration
 *****************************************************************************/


/*****************************************************************************
 * struct & enum declaration
 *****************************************************************************/


/*****************************************************************************
 * external function declaration
 *****************************************************************************/
extern unsigned int met_get_chip_id(void);
extern char *met_get_platform(void);


/*****************************************************************************
 * internal function declaration
 *****************************************************************************/
static void _log_done_cb(const void *p);

static struct completion SSPM_ACK_comp;
static struct completion SSPM_CMD_comp;
struct scmi_tinysys_info_st *tinfo = NULL;
int feature_id = 3; // scmi_met = <3>, feature_id will be reloaded from .dts

static int _sspm_recv_thread(void *data);


/*****************************************************************************
 * external variable declaration
 *****************************************************************************/
int sspm_buf_available;
EXPORT_SYMBOL(sspm_buf_available);


/*****************************************************************************
 * internal variable declaration
 *****************************************************************************/
static unsigned int ridx, widx, wlen;
static unsigned int recv_buf[4], recv_buf_copy[4];
static unsigned int log_size;
static struct task_struct *_sspm_recv_task;
static int sspm_ipi_thread_started;
static int sspm_buffer_dumping;
static int sspm_recv_thread_comp;
static int sspm_run_mode = SSPM_RUN_NORMAL;



/*****************************************************************************
 * internal function ipmlement
 *****************************************************************************/
void MET_handler(u32 r_feature_id, scmi_tinysys_report* report)
{
    u32 cmd = 0;
	int ret = 0;

    MET_PRINTF_D("\x1b[1;33m ==> p1: 0x%x \033[0m\n", report->p1);
    MET_PRINTF_D("\x1b[1;33m ==> p2: 0x%x \033[0m\n", report->p2);
    MET_PRINTF_D("\x1b[1;33m ==> p3: 0x%x \033[0m\n", report->p3);
    MET_PRINTF_D("\x1b[1;33m ==> p4: 0x%x \033[0m\n", report->p4);

    recv_buf[0] = report->p1;
    recv_buf[1] = report->p2;
    recv_buf[2] = report->p3;
    recv_buf[3] = report->p4;
    memcpy(recv_buf_copy, recv_buf, sizeof(recv_buf_copy));

    cmd = report->p1 & MET_SUB_ID_MASK;
	MET_PRINTF_D("\x1b[1;33m ==> cmd: 0x%x \033[0m\n". cmd);
	switch (cmd) {
	case MET_DUMP_BUFFER:	/* mbox 1: start index; 2: size */
		sspm_buffer_dumping = 1;
		ridx = report->p2;
		widx = report->p3;
		log_size = report->p4;
		break;

	case MET_CLOSE_FILE:	/* no argument */
		/* do close file */
		ridx = report->p2;
		widx = report->p3;
		if (widx < ridx) {	/* wrapping occurs */
			wlen = log_size - ridx;
			sspm_log_req_enq((char *)(sspm_log_virt_addr) + (ridx << 2),
					wlen * 4, _log_done_cb, (void *)1);
			sspm_log_req_enq((char *)(sspm_log_virt_addr),
					widx * 4, _log_done_cb, (void *)1);
		} else {
			wlen = widx - ridx;
			sspm_log_req_enq((char *)(sspm_log_virt_addr) + (ridx << 2),
					wlen * 4, _log_done_cb, (void *)1);
		}
		ret = sspm_log_stop();
		break;

	case MET_RESP_MD2AP:
	    sspm_buffer_dumping = 0;
		break;

    case MET_SSPM_ACK:
        complete(&SSPM_ACK_comp);
        return;

	default:
		break;
	}

	MET_PRINTF_D("\x1b[1;33m ==> MET_handler done, while(1) wake up \033[0m\n");
	complete(&SSPM_CMD_comp);

}

int scmi_tinysys_common_set_compl( u32 feature_id, unsigned int *buffer, int timeout)
{
    int ret = 0;
    if (tinfo == NULL)
    {
        PR_BOOTMSG("scmi protocol handle is NULL!!\n");
        return 1; // FAIL
    }

    MET_PRINTF_D("\x1b[1;34m ==> buffer[0~4]: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x \033[0m \n",
								buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
    ret = scmi_tinysys_common_set(tinfo->ph, feature_id,
                                  buffer[0], buffer[1], buffer[2], buffer[3], buffer[4]);
    if (ret != 0) {
		MET_PRINTF_D("scmi_tinysys_common_set_compl error(%d)\n", ret);
    }

    MET_PRINTF_D("\x1b[1;34m ==> wait for (0x%x) completion \033[0m \n", buffer[0]);
    ret = wait_for_completion_killable_timeout(&SSPM_ACK_comp, timeout);
    MET_PRINTF_D("\x1b[1;34m ==> (0x%x) SSPM ACK \033[0m\n", buffer[0]);

    return ret;
}

void start_sspm_ipi_recv_thread()
{
    init_completion(&SSPM_ACK_comp);
    init_completion(&SSPM_CMD_comp);

    tinfo = get_scmi_tinysys_info();
    of_property_read_u32(tinfo->sdev->dev.of_node, "scmi_met", &feature_id);

    scmi_tinysys_event_notify(feature_id, 1 /*1: ENABLE*/);
    scmi_tinysys_register_event_notifier(feature_id, MET_handler);
    MET_PRINTF_D(" ==> feature_id: %d \n", feature_id);

	if (sspm_ipi_thread_started != 1) {
		sspm_recv_thread_comp = 0;
		_sspm_recv_task = kthread_run(_sspm_recv_thread,
						NULL, "sspmsspm_recv");
		if (IS_ERR(_sspm_recv_task)) {
			pr_debug("MET: Can not create sspmsspm_recv\n");
		} else {
			sspm_ipi_thread_started = 1;
		}
	}
}


void stop_sspm_ipi_recv_thread()
{
	if (_sspm_recv_task) {
		sspm_recv_thread_comp = 1;


		kthread_stop(_sspm_recv_task);
		_sspm_recv_task = NULL;
		sspm_ipi_thread_started = 0;
	}
}


void sspm_start(void)
{
	int ret = 0;
	unsigned int rdata = 0;
	unsigned int ipi_buf[4];
	const char* platform_name = NULL;
	unsigned int platform_id = 0;

	MET_PRINTF_D("\x1b[1;31m ==> sspm_start \033[0m\n");

	/* clear DRAM buffer */
	if (sspm_log_virt_addr != NULL) {
		memset_io((void *)sspm_log_virt_addr, 0, sspm_buffer_size);
	} else {
		return;
	}

	platform_name = met_get_platform();
	if (platform_name) {
		char buf[5];

		memset(buf, 0x0, 5);
		memcpy(buf, &platform_name[2], 4);

		ret = kstrtouint(buf, 10, &platform_id);
	}

	/* send DRAM physical address */
	ipi_buf[0] = MET_MAIN_ID | MET_BUFFER_INFO;
	ipi_buf[1] = (unsigned int)sspm_log_phy_addr; /* address */
	if (ret == 0)
		ipi_buf[2] = platform_id;
	else
		ipi_buf[2] = 0;

	ipi_buf[3] = 0;
	ret = met_ipi_to_sspm_command(ipi_buf, 0, &rdata, 1);

	/* start ondiemet now */
	ipi_buf[0] = MET_MAIN_ID | MET_OP | MET_OP_START;
	ipi_buf[1] = ondiemet_module[ONDIEMET_SSPM] ;
	ipi_buf[2] = SSPM_LOG_FILE;
	ipi_buf[3] = SSPM_RUN_NORMAL;
	ret = met_ipi_to_sspm_command(ipi_buf, 0, &rdata, 1);

}


void sspm_stop(void)
{
	int ret = 0;
	unsigned int rdata = 0;
	unsigned int ipi_buf[4];
	unsigned int chip_id = 0;

	MET_PRINTF_D("\x1b[1;31m ==> sspm_stop \033[0m\n");

	chip_id = met_get_chip_id();
	if (sspm_buf_available == 1) {
		ipi_buf[0] = MET_MAIN_ID | MET_OP | MET_OP_STOP;
		ipi_buf[1] = chip_id;
		ipi_buf[2] = 0;
		ipi_buf[3] = 0;
		ret = met_ipi_to_sspm_command(ipi_buf, 0, &rdata, 1);
	}
}


void sspm_extract(void)
{
	int ret;
	unsigned int rdata;
	unsigned int ipi_buf[4];
	int count;

	MET_PRINTF_D("\x1b[1;31m ==> sspm_extract \033[0m\n");

	count = 20;
	if (sspm_buf_available == 1) {
		while ((sspm_buffer_dumping == 1) && (count != 0)) {
			msleep(50);
			count--;
		}
		ipi_buf[0] = MET_MAIN_ID | MET_OP | MET_OP_EXTRACT;
		ipi_buf[1] = 0;
		ipi_buf[2] = 0;
		ipi_buf[3] = 0;
		ret = met_ipi_to_sspm_command(ipi_buf, 0, &rdata, 1);
	}

	if (sspm_run_mode == SSPM_RUN_NORMAL) {
		ondiemet_module[ONDIEMET_SSPM]  = 0;
	}
}


void sspm_flush(void)
{
	int ret;
	unsigned int rdata;
	unsigned int ipi_buf[4];

	MET_PRINTF_D("\x1b[1;31m ==> sspm_flush \033[0m\n");

	if (sspm_buf_available == 1) {
		ipi_buf[0] = MET_MAIN_ID | MET_OP | MET_OP_FLUSH;
		ipi_buf[1] = 0;
		ipi_buf[2] = 0;
		ipi_buf[3] = 0;
		ret = met_ipi_to_sspm_command((void *)ipi_buf, 0, &rdata, 1);
	}

	if (sspm_run_mode == SSPM_RUN_NORMAL) {
		ondiemet_module[ONDIEMET_SSPM] = 0;
	}
}


int met_ipi_to_sspm_command(
	unsigned int *buffer,
	int slot,
	unsigned int *retbuf,
	int retslot)
{
	int ret = 0;

	ret = scmi_tinysys_common_set_compl(feature_id, buffer, 500 /*unit:jiffies (2s time out)*/);

	if (ret != 0) {
		PR_BOOTMSG("%s 0x%X, 0x%X, 0x%X, 0x%X\n", __FUNCTION__,
			buffer[0], buffer[1], buffer[2], buffer[3]);
		pr_debug("met_AP_to_sspm_command error(%d)\n", ret);
	}

	return ret;
}
EXPORT_SYMBOL(met_ipi_to_sspm_command);


int met_ipi_to_sspm_command_async(
	unsigned int *buffer,
	int slot,
	unsigned int *retbuf,
	int retslot)
{
	int ret = 0;

	return ret;
}
EXPORT_SYMBOL(met_ipi_to_sspm_command_async);


/*****************************************************************************
 * internal function ipmlement
 *****************************************************************************/
static void _log_done_cb(const void *p)
{
	unsigned int ret;
	unsigned int rdata = 0;
	unsigned int ipi_buf[4];
	unsigned int opt = (p != NULL);

	MET_PRINTF_D("\x1b[1;31m ==> _log_done_cb ++\033[0m\n");

	if (opt == 0) {
		MET_PRINTF_D("\x1b[1;31m ==> Send MET_RESP_AP2MD --> SSPM \033[0m\n");
		ipi_buf[0] = MET_MAIN_ID | MET_RESP_AP2MD;
		ipi_buf[1] = MET_DUMP_BUFFER;
		ipi_buf[2] = 0;
		ipi_buf[3] = 0;
		ret = met_ipi_to_sspm_command((void *)ipi_buf, 0, &rdata, 1);
	}

	MET_PRINTF_D("\x1b[1;31m ==> _log_done_cb --\033[0m\n");
}

static int _sspm_recv_thread(void *data)
{
    u32 cmd = 0;
	int ret = 0;
	unsigned int ridx, widx, wlen;

	do {
		MET_PRINTF_D("\x1b[1;34m ==> (0x%x) command done, then to wait receiving next SSPM CMD\033[0m\n", recv_buf_copy[0]);
		ret = wait_for_completion_killable(&SSPM_CMD_comp);
		MET_PRINTF_D("\x1b[1;34m ==> release to do SSPM CMD (0x%x) \033[0m\n", recv_buf_copy[0]);

		if (ret) {
			// skip cmd handling if receive fail
			continue;
		}

		if (sspm_recv_thread_comp == 1) {
			while (!kthread_should_stop()) {
				;
			}
			return 0;
		}

		/* heavyweight cmd handling (not support reply_data) */
		cmd = recv_buf_copy[0] & MET_SUB_ID_MASK;
		MET_PRINTF_D("\x1b[1;33m ==> cmd: 0x%x \033[0m\n". cmd);
		switch (cmd) {
		case MET_DUMP_BUFFER:	/* mbox 1: start index; 2: size */
			sspm_buffer_dumping = 1;
			ridx = recv_buf_copy[1];
			widx = recv_buf_copy[2];
			log_size = recv_buf_copy[3];

			MET_PRINTF_D("\x1b[1;34m ==> ridx: %d (0x%x) \033[0m\n", ridx, ridx);
			MET_PRINTF_D("\x1b[1;34m ==> widx: %d (0x%x) \033[0m\n", widx, widx);
			MET_PRINTF_D("\x1b[1;34m ==> log_size: %d \033[0m\n", log_size);
			MET_PRINTF_D("\x1b[1;34m ==> sspm_log_virt_addr: 0x%x \033[0m\n", (char *)sspm_log_virt_addr);
			MET_PRINTF_D("==> ==================================\n");

			if (widx < ridx) {	/* wrapping occurs */
				wlen = log_size - ridx;

                MET_PRINTF_D("\x1b[1;36m ==> #1 base: addr + ridx << 2 = 0x%x \033[0m\n", (char *)(sspm_log_virt_addr) + (ridx << 2));
                MET_PRINTF_D("\x1b[1;36m ==> #1 len: wlen * 4 = 0x%x \033[0m\n", wlen * 4);
                MET_PRINTF_D("\x1b[1;36m ==> #1 end: 0x%x \033[0m\n", (char *)(sspm_log_virt_addr) + (ridx << 2) + (wlen * 4));

				sspm_log_req_enq((char *)(sspm_log_virt_addr) + (ridx << 2),
						wlen * 4, _log_done_cb, (void *)1);

                MET_PRINTF_D("\x1b[1;36m ==> #2 base: addr = 0x%x \033[0m\n", (char *)(sspm_log_virt_addr));
                MET_PRINTF_D("\x1b[1;36m ==> #2 len: widx * 4 = 0x%x \033[0m\n", widx * 4);
                MET_PRINTF_D("\x1b[1;36m ==> #2 end: 0x%x \033[0m\n", (char *)(sspm_log_virt_addr) + (widx * 4));

				sspm_log_req_enq((char *)(sspm_log_virt_addr),
						widx * 4, _log_done_cb, (void *)0);
			} else {
				wlen = widx - ridx;

                MET_PRINTF_D("\x1b[1;36m ==> #3 base: addr + ridx << 2 = 0x%x \033[0m\n", (char *)(sspm_log_virt_addr) + (ridx << 2));
                MET_PRINTF_D("\x1b[1;36m ==> #3 len: wlen * 4 = 0x%x \033[0m\n", wlen * 4);
                MET_PRINTF_D("\x1b[1;36m ==> #3 end: 0x%x \033[0m\n", (char *)(sspm_log_virt_addr) + (ridx << 2) + (wlen * 4));

				sspm_log_req_enq((char *)(sspm_log_virt_addr) + (ridx << 2),
						wlen * 4, _log_done_cb, (void *)0);
			}
			break;

		case MET_CLOSE_FILE:	/* no argument */
			/* do close file */
			ridx = recv_buf_copy[1];
			widx = recv_buf_copy[2];
			if (widx < ridx) {	/* wrapping occurs */
				wlen = log_size - ridx;
				sspm_log_req_enq((char *)(sspm_log_virt_addr) + (ridx << 2),
						wlen * 4, _log_done_cb, (void *)1);
				sspm_log_req_enq((char *)(sspm_log_virt_addr),
						widx * 4, _log_done_cb, (void *)1);
			} else {
				wlen = widx - ridx;
				sspm_log_req_enq((char *)(sspm_log_virt_addr) + (ridx << 2),
						wlen * 4, _log_done_cb, (void *)1);
			}
			ret = sspm_log_stop();

			/* continuous mode handling */
			if (sspm_run_mode == SSPM_RUN_CONTINUOUS) {
				/* clear the memory */
				memset_io((void *)sspm_log_virt_addr, 0,
					  sspm_buffer_size);
				/* re-start ondiemet again */
				sspm_start();
			}
			break;

		default:
			break;
		}

		MET_PRINTF_D("\x1b[1;34m ==> In while(1) AP done CMD(0x%x) ACK --> SSPM \033[0m\n", recv_buf_copy[0]);
		ret = scmi_tinysys_common_set(tinfo->ph, feature_id, (MET_MAIN_ID | MET_AP_ACK), 0, 0, 0, 0);
	} while (!kthread_should_stop());

	return 0;
}


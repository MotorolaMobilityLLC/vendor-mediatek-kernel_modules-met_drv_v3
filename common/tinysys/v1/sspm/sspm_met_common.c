// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2019 MediaTek Inc.
 */
/*****************************************************************************
 * headers
 *****************************************************************************/
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/of.h>

#include "met_drv.h"
#include "tinysys_sspm.h"
#include "tinysys_mgr.h"

#include "sspm_met_log.h"
#include "sspm_met_ipi_handle.h" /* for met_ipi_to_sspm_command */


/*****************************************************************************
 * struct & enum declaration
 *****************************************************************************/
struct sspm_met_event_header {
	unsigned int rts_event_id;
	const char *rts_event_name;
	const char *chart_line_name;
	const char *key_list;
};


/*****************************************************************************
 * internal function declaration
 *****************************************************************************/
static void ondiemet_sspm_start(void);
static void ondiemet_sspm_stop(void);
static int ondiemet_sspm_print_help(char *buf, int len);
static int ondiemet_sspm_process_argument(const char *arg, int len);
static int ondiemet_sspm_print_header(char *buf, int len);


/*****************************************************************************
 * external function declaration
 *****************************************************************************/


/*****************************************************************************
 * internal variable
 *****************************************************************************/
#define NR_RTS_STR_ARRAY 2
#define MXNR_NODE_NAME 32
#define MXNR_EVENT_NAME 64
#define MAX_MET_RTS_EVENT_NUM 128
static unsigned int event_id_flag[MAX_MET_RTS_EVENT_NUM / 32];
static struct sspm_met_event_header met_event_header[MAX_MET_RTS_EVENT_NUM];
static char sspm_help[] = "  --sspm_common=rts_event_name\n";
static char header[] = 	"met-info [000] 0.0: sspm_common_header: ";
static bool met_event_header_updated = false;
static int nr_dts_header = 0;

struct metdevice met_sspm_common = {
	.name = "sspm_common",
	.owner = THIS_MODULE,
	.type = MET_TYPE_BUS,
	.cpu_related = 0,
	.ondiemet_mode = 1,
	.ondiemet_start = ondiemet_sspm_start,
	.ondiemet_stop = ondiemet_sspm_stop,
	.ondiemet_process_argument = ondiemet_sspm_process_argument,
	.ondiemet_print_help = ondiemet_sspm_print_help,
	.ondiemet_print_header = ondiemet_sspm_print_header,
};


/*****************************************************************************
 * internal function implement
 *****************************************************************************/
static int ondiemet_sspm_print_help(char *buf, int len)
{
	return snprintf(buf, PAGE_SIZE, sspm_help);
}


static int get_rts_header_from_dts_table(struct sspm_met_event_header* __met_event_header)
{
	int idx;
	struct device_node *np;
	const char *rts_string[NR_RTS_STR_ARRAY];
	int nr_str = 0;

	/*get rts root node*/
	np = of_find_node_by_name(NULL, "sspm-rts-header");
	if (!np) {
		pr_debug("unable to find sspm-rts-header\n");
		return 0;
	}

	/*get string array*/
	for (idx = 0; idx < MAX_MET_RTS_EVENT_NUM; idx++) {
		char node_name[MXNR_NODE_NAME];
		sprintf(node_name, "node_%d", idx);
		nr_str = of_property_read_string_array(np,
				node_name, &rts_string[0], NR_RTS_STR_ARRAY);
		if (nr_str != NR_RTS_STR_ARRAY) {
			pr_debug("%s: nr_str != %d\n", node_name, NR_RTS_STR_ARRAY);
			break;
		} else {
			__met_event_header[idx].rts_event_id = idx;
			__met_event_header[idx].rts_event_name = rts_string[0];
			__met_event_header[idx].chart_line_name = rts_string[0];
			__met_event_header[idx].key_list = rts_string[1];
		}
	}

	return idx;
}


static int ondiemet_sspm_print_header(char *buf, int len)
{
	int i;
	int write_len;
	int flag = 0;
	int mask = 0;
	int group;
	static int is_dump_header = 0;
	static int read_idx = 0;

	len = 0;
	met_sspm_common.header_read_again = 0;
	if (is_dump_header == 0) {
		len = snprintf(buf, PAGE_SIZE, "%s", header);
		is_dump_header = 1;
	}

	for (i = read_idx; i < nr_dts_header; i++) {
		if (met_event_header[i].chart_line_name) {
			group = i / 32;
			mask = 1 << (i - group * 32);
			flag = event_id_flag[group] & mask;
			if (flag == 0) {
				continue;
			}

			write_len = strlen(met_event_header[i].chart_line_name) + strlen(met_event_header[i].key_list) + 3;
			if ((len + write_len) < PAGE_SIZE) {
				len += snprintf(buf+len, PAGE_SIZE-len, "%u,%s,%s;",
					met_event_header[i].rts_event_id,
					met_event_header[i].chart_line_name,
					met_event_header[i].key_list);
			} else {
				met_sspm_common.header_read_again = 1;
				read_idx = i;
				return len;
			}
		}
	}

	if (i == nr_dts_header) {
		is_dump_header = 0;
		read_idx = 0;
		buf[len - 1] = '\n';
		met_sspm_common.mode = 0;
		for (i = 0 ; i < MAX_MET_RTS_EVENT_NUM / 32; i++) {
			event_id_flag[i] = 0;
		}
	}

	return len;
}


static void ondiemet_sspm_start(void)
{
	if (sspm_buffer_size == 0) {
		return;
	}

	ondiemet_module[ONDIEMET_SSPM] |= ID_COMMON;
}


static void ondiemet_sspm_stop(void)
{
	return;
}


static void update_event_id_flag(int event_id)
{
	unsigned int ipi_buf[4] = {0};
	unsigned int rdata = 0;
	unsigned int res = 0;
	unsigned int group = 0;

	if (sspm_buffer_size == 0)
		return ;

	group = event_id / 32;
	event_id_flag[group] |= 1 << (event_id - group * 32);
	ipi_buf[0] = MET_MAIN_ID | MET_ARGU | MID_COMMON<<MID_BIT_SHIFT | 1;
	ipi_buf[1] = group;
	ipi_buf[2] = event_id_flag[group];
	ipi_buf[3] = 0;
	res = met_ipi_to_sspm_command((void *)ipi_buf, 0, &rdata, 1);

	met_sspm_common.mode = 1;
}


static char *strdup(const char *s)
{
	char *p = kmalloc(strlen(s) + 1, GFP_KERNEL);

	if (p) {
		strcpy(p, s);
	}

	return p;
}


static int ondiemet_sspm_process_argument(const char *arg, int len)
{
	int i = 0;
	int rts_event_id = -1;
	int res = 0;
	char *line = NULL;
	char *token = NULL;

	if (!met_event_header_updated) {
		nr_dts_header = get_rts_header_from_dts_table(met_event_header);
		met_event_header_updated = true;
	}

	for (i = 0; met_event_header[i].rts_event_name && i < nr_dts_header; i++) {
		if (strncmp(met_event_header[i].rts_event_name, arg, MXNR_EVENT_NAME) == 0) {
			rts_event_id = i;
			break;
		}
	}

	if (strstarts(arg, "update_rts_event_tbl")) {
		char *ptr = NULL;

		/* update_rts_event_tbl=rts_event_id;rts_event_name;chart_line_name;key_list*/
		line = strdup(arg);
		if (line == NULL) {
			return -1;
		}
		ptr = line;
		token = strsep(&line, "=");

		/* rts_event_id, */
		token = strsep(&line, ";");
		res = kstrtoint(token, 10, &rts_event_id);
		met_event_header[rts_event_id].rts_event_id = rts_event_id;

		/* rts_event_name */
		token = strsep(&line, ";");
		met_event_header[rts_event_id].rts_event_name = token;

		/* chart_line_name */
		token = strsep(&line, ";");
		met_event_header[rts_event_id].chart_line_name = token;

		/* key_list */
		token = strsep(&line, ";\n");
		met_event_header[rts_event_id].key_list = token;
	}

	if (rts_event_id >= 0) {
		update_event_id_flag(rts_event_id);
	}

	return 0;
}
EXPORT_SYMBOL(met_sspm_common);
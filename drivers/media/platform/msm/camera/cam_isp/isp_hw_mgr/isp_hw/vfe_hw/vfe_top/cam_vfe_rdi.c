/* Copyright (c) 2017-2019 The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/slab.h>
#include "cam_vfe_rdi.h"
#include "cam_isp_hw_mgr_intf.h"
#include "cam_isp_hw.h"
#include "cam_vfe_hw_intf.h"
#include "cam_io_util.h"
#include "cam_debug_util.h"
#include "cam_cdm_util.h"
#include "cam_vfe_soc.h"
#include "cam_cpas_api.h"

struct cam_vfe_mux_rdi_data {
	void __iomem                                *mem_base;
	struct cam_hw_intf                          *hw_intf;
	struct cam_vfe_top_ver2_reg_offset_common   *common_reg;
	struct cam_vfe_rdi_ver2_reg                 *rdi_reg;
	struct cam_vfe_rdi_overflow_status          *rdi_irq_status;
	struct cam_vfe_rdi_reg_data                 *reg_data;
	struct cam_hw_soc_info                      *soc_info;
	struct timeval                     sof_ts;
	struct timeval                     rup_ts;
	struct timeval                     error_ts;

	enum cam_isp_hw_sync_mode          sync_mode;
};

static int cam_vfe_rdi_get_reg_update(
	struct cam_isp_resource_node  *rdi_res,
	void *cmd_args, uint32_t arg_size)
{
	uint32_t                          size = 0;
	uint32_t                          reg_val_pair[2];
	struct cam_isp_hw_get_cmd_update *cdm_args = cmd_args;
	struct cam_cdm_utils_ops         *cdm_util_ops = NULL;
	struct cam_vfe_mux_rdi_data      *rsrc_data = NULL;

	if (arg_size != sizeof(struct cam_isp_hw_get_cmd_update)) {
		CAM_ERR(CAM_ISP, "Error - Invalid cmd size");
		return -EINVAL;
	}

	if (!cdm_args || !cdm_args->res) {
		CAM_ERR(CAM_ISP, "Error - Invalid args");
		return -EINVAL;
	}

	cdm_util_ops = (struct cam_cdm_utils_ops *)cdm_args->res->cdm_ops;
	if (!cdm_util_ops) {
		CAM_ERR(CAM_ISP, "Error - Invalid CDM ops");
		return -EINVAL;
	}

	size = cdm_util_ops->cdm_required_size_reg_random(1);
	/* since cdm returns dwords, we need to convert it into bytes */
	if ((size * 4) > cdm_args->cmd.size) {
		CAM_ERR(CAM_ISP,
			"Error - buf size:%d is not sufficient, expected: %d",
			cdm_args->cmd.size, size * 4);
		return -EINVAL;
	}

	if (cdm_args->rup_data->is_fe_enable &&
		(cdm_args->rup_data->res_bitmap &
			(1 << CAM_IFE_REG_UPD_CMD_PIX_BIT))) {
		cdm_args->cmd.used_bytes = 0;
		CAM_DBG(CAM_ISP, "Avoiding reg_upd for fe for rdi");
		return 0;
	}

	rsrc_data = rdi_res->res_priv;
	reg_val_pair[0] = rsrc_data->rdi_reg->reg_update_cmd;
	reg_val_pair[1] = rsrc_data->reg_data->reg_update_cmd_data;
	CAM_DBG(CAM_ISP, "RDI%d reg_update_cmd %x",
		rdi_res->res_id - CAM_ISP_HW_VFE_IN_RDI0, reg_val_pair[1]);

	cdm_util_ops->cdm_write_regrandom(cdm_args->cmd.cmd_buf_addr,
		1, reg_val_pair);
	cdm_args->cmd.used_bytes = size * 4;

	return 0;
}

static void cam_vfe_rdi_get_irq(struct cam_isp_resource_node  *rdi_res,
		void *cmd_args, uint32_t arg_size)
{
	struct cam_isp_hw_get_cmd_update *irq_reg = NULL;
	struct cam_vfe_mux_rdi_data      *rsrc_data = NULL;

	rsrc_data = rdi_res->res_priv;

	irq_reg = (struct cam_isp_hw_get_cmd_update *)cmd_args;

	*(uint32_t *)irq_reg->data =
		(rsrc_data->reg_data->reg_update_irq_mask |
			rsrc_data->reg_data->sof_irq_mask);

	CAM_DBG(CAM_ISP, "RDI%d irq_mask 0x%x",
		rdi_res->res_id - CAM_ISP_HW_VFE_IN_RDI0,
		*(uint32_t *)irq_reg->data);
}

int cam_vfe_rdi_ver2_acquire_resource(
	struct cam_isp_resource_node  *rdi_res,
	void                          *acquire_param)
{
	struct cam_vfe_mux_rdi_data   *rdi_data;
	struct cam_vfe_acquire_args   *acquire_data;

	rdi_data     = (struct cam_vfe_mux_rdi_data *)rdi_res->res_priv;
	acquire_data = (struct cam_vfe_acquire_args *)acquire_param;

	rdi_data->sync_mode   = acquire_data->vfe_in.sync_mode;
	rdi_res->rdi_only_ctx = 0;

	return 0;
}

static int cam_vfe_rdi_resource_start(
	struct cam_isp_resource_node  *rdi_res)
{
	struct cam_vfe_mux_rdi_data   *rsrc_data;
	int                            rc = 0;

	if (!rdi_res) {
		CAM_ERR(CAM_ISP, "Error! Invalid input arguments");
		return -EINVAL;
	}

	if (rdi_res->res_state != CAM_ISP_RESOURCE_STATE_RESERVED) {
		CAM_ERR(CAM_ISP, "Error! Invalid rdi res res_state:%d",
			rdi_res->res_state);
		return -EINVAL;
	}

	rsrc_data = (struct cam_vfe_mux_rdi_data  *)rdi_res->res_priv;
	rdi_res->res_state = CAM_ISP_RESOURCE_STATE_STREAMING;

	/* Reg Update */
	cam_io_w_mb(rsrc_data->reg_data->reg_update_cmd_data,
		rsrc_data->mem_base + rsrc_data->rdi_reg->reg_update_cmd);

	CAM_DBG(CAM_ISP, "Start RDI %d",
		rdi_res->res_id - CAM_ISP_HW_VFE_IN_RDI0);

	return rc;
}


static int cam_vfe_rdi_resource_stop(
	struct cam_isp_resource_node        *rdi_res)
{
	struct cam_vfe_mux_rdi_data           *rdi_priv;
	int rc = 0;

	if (!rdi_res) {
		CAM_ERR(CAM_ISP, "Error! Invalid input arguments");
		return -EINVAL;
	}

	if (rdi_res->res_state == CAM_ISP_RESOURCE_STATE_RESERVED ||
		rdi_res->res_state == CAM_ISP_RESOURCE_STATE_AVAILABLE)
		return 0;

	rdi_priv = (struct cam_vfe_mux_rdi_data *)rdi_res->res_priv;

	if (rdi_res->res_state == CAM_ISP_RESOURCE_STATE_STREAMING)
		rdi_res->res_state = CAM_ISP_RESOURCE_STATE_RESERVED;


	return rc;
}

static int cam_vfe_rdi_process_cmd(struct cam_isp_resource_node *rsrc_node,
	uint32_t cmd_type, void *cmd_args, uint32_t arg_size)
{
	int rc = -EINVAL;

	if (!rsrc_node || !cmd_args) {
		CAM_ERR(CAM_ISP, "Error! Invalid input arguments");
		return -EINVAL;
	}

	switch (cmd_type) {
	case CAM_ISP_HW_CMD_GET_REG_UPDATE:
		rc = cam_vfe_rdi_get_reg_update(rsrc_node, cmd_args,
			arg_size);
		break;
	case CAM_ISP_HW_CMD_GET_RDI_IRQ_MASK:
		cam_vfe_rdi_get_irq(rsrc_node, cmd_args,
			arg_size);
		break;
	default:
		CAM_ERR(CAM_ISP,
			"unsupported RDI process command:%d", cmd_type);
		break;
	}

	return rc;
}

static int cam_vfe_rdi_cpas_reg_dump(
struct cam_vfe_mux_rdi_data *rdi_priv)
{
	struct cam_vfe_soc_private *soc_private =
		rdi_priv->soc_info->soc_private;
	uint32_t  val;

	if (soc_private->cpas_version == CAM_CPAS_TITAN_175_V120) {
		cam_cpas_reg_read(soc_private->cpas_handle[0],
			CAM_CPAS_REG_CAMNOC, 0x3A20, true, &val);
		CAM_INFO(CAM_ISP, "IFE0_nRDI_MAXWR_LOW offset 0x3A20 val 0x%x",
			val);

		cam_cpas_reg_read(soc_private->cpas_handle[0],
			CAM_CPAS_REG_CAMNOC, 0x5420, true, &val);
		CAM_INFO(CAM_ISP, "IFE1_nRDI_MAXWR_LOW offset 0x5420 val 0x%x",
			val);

		cam_cpas_reg_read(soc_private->cpas_handle[0],
			CAM_CPAS_REG_CAMNOC, 0x3620, true, &val);
		CAM_INFO(CAM_ISP,
			"IFE0123_RDI_WR_MAXWR_LOW offset 0x3620 val 0x%x", val);

	} else if (soc_private->cpas_version < CAM_CPAS_TITAN_175_V120) {
		cam_cpas_reg_read(soc_private->cpas_handle[0],
			CAM_CPAS_REG_CAMNOC, 0x420, true, &val);
		CAM_INFO(CAM_ISP, "IFE02_MAXWR_LOW offset 0x420 val 0x%x", val);

		cam_cpas_reg_read(soc_private->cpas_handle[0],
			CAM_CPAS_REG_CAMNOC, 0x820, true, &val);
		CAM_INFO(CAM_ISP, "IFE13_MAXWR_LOW offset 0x820 val 0x%x", val);
	}

	return 0;

}

static int cam_vfe_rdi_handle_irq_top_half(uint32_t evt_id,
	struct cam_irq_th_payload *th_payload)
{
	return -EPERM;
}

static int cam_vfe_rdi_handle_irq_bottom_half(void *handler_priv,
	void *evt_payload_priv)
{
	int                                  ret = CAM_VFE_IRQ_STATUS_ERR;
	struct cam_isp_resource_node        *rdi_node;
	struct cam_vfe_mux_rdi_data         *rdi_priv;
	struct cam_vfe_top_irq_evt_payload  *payload;
	uint32_t                             irq_status0;
	uint32_t                             irq_status1;
	uint32_t                             irq_rdi_status;
	struct cam_hw_soc_info              *soc_info = NULL;
	struct cam_vfe_soc_private          *soc_private = NULL;
	struct timespec64                    ts;

	if (!handler_priv || !evt_payload_priv) {
		CAM_ERR(CAM_ISP, "Invalid params");
		return ret;
	}

	rdi_node = handler_priv;
	rdi_priv = rdi_node->res_priv;
	payload = evt_payload_priv;
	soc_info = rdi_priv->soc_info;
	soc_private =
		(struct cam_vfe_soc_private *)soc_info->soc_private;

	irq_status0 = payload->irq_reg_val[CAM_IFE_IRQ_CAMIF_REG_STATUS0];
	irq_status1 = payload->irq_reg_val[CAM_IFE_IRQ_CAMIF_REG_STATUS1];

	CAM_DBG(CAM_ISP, "event ID:%d", payload->evt_id);
	CAM_DBG(CAM_ISP, "irq_status_0 = %x", irq_status0);
	CAM_DBG(CAM_ISP, "irq_status_1 = %x", irq_status1);

	switch (payload->evt_id) {
	case CAM_ISP_HW_EVENT_SOF:
		if (irq_status0 & rdi_priv->reg_data->sof_irq_mask) {
			CAM_DBG(CAM_ISP, "Received SOF");
			rdi_priv->sof_ts.tv_sec =
				payload->ts.mono_time.tv_sec;
			rdi_priv->sof_ts.tv_usec =
				payload->ts.mono_time.tv_usec;
			ret = CAM_VFE_IRQ_STATUS_SUCCESS;
		}
		break;
	case CAM_ISP_HW_EVENT_REG_UPDATE:
		if (irq_status0 & rdi_priv->reg_data->reg_update_irq_mask) {
			CAM_DBG(CAM_ISP, "Received REG UPDATE");
			rdi_priv->rup_ts.tv_sec =
				payload->ts.mono_time.tv_sec;
			rdi_priv->rup_ts.tv_usec =
				payload->ts.mono_time.tv_usec;
			ret = CAM_VFE_IRQ_STATUS_SUCCESS;
		}
		break;
	default:
		break;
	}


	if (!rdi_priv->rdi_irq_status)
		goto end;

	irq_rdi_status =
		(irq_status1 &
		rdi_priv->rdi_irq_status->rdi_overflow_mask);
	if (irq_rdi_status) {
		get_monotonic_boottime64(&ts);
		CAM_INFO(CAM_ISP,
			"current monotonic time stamp seconds %lld:%lld",
			ts.tv_sec, ts.tv_nsec/1000);

		CAM_INFO(CAM_ISP,
			"SOF %lld:%lld EPOCH %lld.%lld EOF %lld.%lld RUP %lld.%lld",
			rdi_priv->sof_ts.tv_sec,
			rdi_priv->sof_ts.tv_usec,
			rdi_priv->rup_ts.tv_sec,
			rdi_priv->rup_ts.tv_usec);

		cam_vfe_rdi_cpas_reg_dump(rdi_priv);

		CAM_INFO(CAM_ISP, "ife_clk_src:%lld",
			soc_private->ife_clk_src);

		if (irq_rdi_status &
			rdi_priv->rdi_irq_status->rdi0_overflow_mask) {
			CAM_INFO(CAM_ISP, "RDI %d overflow",
				CAM_ISP_IFE_OUT_RES_RDI_0);
		} else if (irq_rdi_status &
			rdi_priv->rdi_irq_status->rdi1_overflow_mask) {
			CAM_INFO(CAM_ISP, "RDI %d overflow",
				CAM_ISP_IFE_OUT_RES_RDI_1);
		} else if (irq_rdi_status &
			rdi_priv->rdi_irq_status->rdi2_overflow_mask) {
			CAM_INFO(CAM_ISP, "RDI %d overflow",
				CAM_ISP_IFE_OUT_RES_RDI_2);
		} else if (irq_rdi_status &
			rdi_priv->rdi_irq_status->rdi3_overflow_mask) {
			CAM_INFO(CAM_ISP, "RDI %d overflow",
				CAM_ISP_IFE_OUT_RES_RDI_3);
		}
	}
end:
	CAM_DBG(CAM_ISP, "returing status = %d", ret);
	return ret;
}

int cam_vfe_rdi_ver2_init(
	struct cam_hw_intf            *hw_intf,
	struct cam_hw_soc_info        *soc_info,
	void                          *rdi_hw_info,
	struct cam_isp_resource_node  *rdi_node)
{
	struct cam_vfe_mux_rdi_data     *rdi_priv = NULL;
	struct cam_vfe_rdi_ver2_hw_info *rdi_info = rdi_hw_info;

	rdi_priv = kzalloc(sizeof(struct cam_vfe_mux_rdi_data),
			GFP_KERNEL);
	if (!rdi_priv) {
		CAM_DBG(CAM_ISP, "Error! Failed to alloc for rdi_priv");
		return -ENOMEM;
	}

	rdi_node->res_priv = rdi_priv;

	rdi_priv->mem_base   = soc_info->reg_map[VFE_CORE_BASE_IDX].mem_base;
	rdi_priv->hw_intf    = hw_intf;
	rdi_priv->common_reg = rdi_info->common_reg;
	rdi_priv->rdi_reg    = rdi_info->rdi_reg;
	rdi_priv->soc_info = soc_info;
	rdi_priv->rdi_irq_status = rdi_info->rdi_irq_status;

	switch (rdi_node->res_id) {
	case CAM_ISP_HW_VFE_IN_RDI0:
		rdi_priv->reg_data = rdi_info->reg_data[0];
		break;
	case CAM_ISP_HW_VFE_IN_RDI1:
		rdi_priv->reg_data = rdi_info->reg_data[1];
		break;
	case CAM_ISP_HW_VFE_IN_RDI2:
		rdi_priv->reg_data = rdi_info->reg_data[2];
		break;
	case CAM_ISP_HW_VFE_IN_RDI3:
		if (rdi_info->reg_data[3]) {
			rdi_priv->reg_data = rdi_info->reg_data[3];
		} else {
			CAM_ERR(CAM_ISP, "Error! RDI3 is not supported");
			goto err_init;
		}
		break;
	default:
		CAM_DBG(CAM_ISP, "invalid Resource id:%d", rdi_node->res_id);
		goto err_init;
	}

	rdi_node->start = cam_vfe_rdi_resource_start;
	rdi_node->stop  = cam_vfe_rdi_resource_stop;
	rdi_node->process_cmd = cam_vfe_rdi_process_cmd;
	rdi_node->top_half_handler = cam_vfe_rdi_handle_irq_top_half;
	rdi_node->bottom_half_handler = cam_vfe_rdi_handle_irq_bottom_half;

	rdi_priv->sof_ts.tv_sec = 0;
	rdi_priv->sof_ts.tv_usec = 0;
	rdi_priv->rup_ts.tv_sec = 0;
	rdi_priv->rup_ts.tv_usec = 0;
	rdi_priv->error_ts.tv_sec = 0;
	rdi_priv->error_ts.tv_usec = 0;

	return 0;
err_init:
	kfree(rdi_priv);
	return -EINVAL;
}

int cam_vfe_rdi_ver2_deinit(
	struct cam_isp_resource_node  *rdi_node)
{
	struct cam_vfe_mux_rdi_data *rdi_priv = rdi_node->res_priv;

	rdi_node->start = NULL;
	rdi_node->stop  = NULL;
	rdi_node->process_cmd = NULL;
	rdi_node->top_half_handler = NULL;
	rdi_node->bottom_half_handler = NULL;

	rdi_node->res_priv = NULL;

	if (!rdi_priv) {
		CAM_ERR(CAM_ISP, "Error! rdi_priv NULL");
		return -ENODEV;
	}
	kfree(rdi_priv);

	return 0;
}

/*
 * Copyright (c) 2014, Mentor Graphics Corporation
 * All rights reserved.
 * Copyright (c) 2015 Xilinx, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Mentor Graphics Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**************************************************************************
 * FILE NAME
 *
 *       platform_info.c
 *
 * DESCRIPTION
 *
 *       This file implements APIs to get platform specific
 *       information for OpenAMP.
 *
 **************************************************************************/

#include "openamp/hil.h"
#include "metal/atomic.h"
#include "platform_info.h"

#define IPI_BASE_ADDR        XPAR_XIPIPSU_0_BASE_ADDRESS /* IPI base address*/
#define IPI_CHN_BITMASK      0x01000000 /* IPI channel bit mask APU<->RPU0 */

#define APU_CPU_ID           0

/* -- FIX ME: ipi info is to be defined -- */
struct ipi_info {
	const char *name;
	const char *bus_name;
	struct meta_device *dev;
	struct metal_io_region *io;
	metal_phys_addr_t paddr;
	uint32_t ipi_chn_mask;
	int registered;
	atomic_int sync;
};

extern struct hil_platform_ops zynqmp_r5_a53_proc_ops;

static struct ipi_info chn_ipi_info[] = {
	{NULL, NULL, NULL, NULL, IPI_BASE_ADDR, IPI_CHN_BITMASK, 0, 0},
};

const struct firmware_info fw_table[] =
{
	{"unknown",
	 0,
	 0}
};

const int fw_table_size = sizeof(fw_table)/sizeof(struct firmware_info);

struct hil_proc *platform_create_proc(int proc_index)
{
	(void) proc_index;
	struct hil_proc *proc;
	proc = hil_create_proc(&zynqmp_r5_a53_proc_ops, APU_CPU_ID, NULL);
	if (!proc)
		return NULL;

	/* Setup IPI info */
	hil_set_vdev_ipi(proc, 0,
		IPI_IRQ_VECT_ID, (void *)&chn_ipi_info[0]);
	hil_set_vring_ipi(proc, 0,
		IPI_IRQ_VECT_ID, (void *)&chn_ipi_info[0]);
	hil_set_vring_ipi(proc, 1,
		IPI_IRQ_VECT_ID, (void *)&chn_ipi_info[0]);

	hil_set_rpmsg_channel(proc, 0, RPMSG_CHAN_NAME);
	return proc;
}

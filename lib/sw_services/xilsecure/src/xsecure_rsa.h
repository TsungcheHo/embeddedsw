/******************************************************************************
*
* (c) Copyright 2012 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
*******************************************************************************/
/*****************************************************************************/
/**
*
* @file xsecure_rsa.h
*
* This file contains hardware interface related information for RSA device
*
* This driver supports the following features:
*
* - RSA 4096 based decryption
* - verification/authentication of decrypted data
*
* <b>Initialization & Configuration</b>
*
* The Rsa driver instance can be initialized
* in the following way:
*
*   - XSecure_RsaInitialize(XSecure_Rsa *InstancePtr, u8* EncText,
*									u8 *Mod, u8 *ModExt, u8 *ModExpo)
*
* The method used for RSA decryption needs precalculated value off R^2 mod N
* which is generated by bootgen and is present in the signature along with
* modulus and exponent.
*
* @note
*		-The format of the public key( modulus, exponent and precalculated
*		 R^2 mod N should be same as specified by the bootgen
*
*		-For matching, PKCS paddding scheme has to be applied in the manner
*		 specified by the bootgen.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  ba   10/10/14 Initial release
*
* </pre>
*
*
******************************************************************************/

#ifndef XSECURE_RSA_H_
#define XSECURE_RSA_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xsecure_hw.h"
#include "xil_assert.h"
#include "xil_io.h"
#include "xstatus.h"
/************************** Constant Definitions ****************************/

/** @name Control Register
 *
 * The Control register (CR) controls the major functions of the device.
 * It is used to set the function to be implemented by the RSA device in
 * the next iteration.
 *
 * Control Register Bit Definition
 */
#define XSECURE_CSU_RSA_CONTROL_2048	(0xA0U) /**< RSA 2048 Length Code */
#define XSECURE_CSU_RSA_CONTROL_4096	(0xC0U) /**< RSA 4096 Length Code */
#define XSECURE_CSU_RSA_CONTROL_DCA     (0x08U) /**< Abort Operation */
#define XSECURE_CSU_RSA_CONTROL_NOP     (0x00U) /**< No Operation */
#define XSECURE_CSU_RSA_CONTROL_EXP     (0x01U) /**< Exponentiation Opcode */
#define XSECURE_CSU_RSA_CONTROL_EXP_PRE (0x05U) /**< Expo. using R*R mod M */
#define XSECURE_CSU_RSA_CONTROL_MASK    (XSECURE_CSU_RSA_CONTROL_4096 + \
                                          XSECURE_CSU_RSA_CONTROL_EXP_PRE)
/* @} */

/** @name RSA status Register
 *
 * The Status Register(SR) indicates the current state of RSA device.
 *
 * Status Register Bit Definition
 */
#define XSECURE_CSU_RSA_STATUS_DONE           (0x1U) /**< Operation Done */
#define XSECURE_CSU_RSA_STATUS_BUSY           (0x2U) /**< RSA busy */
#define XSECURE_CSU_RSA_STATUS_ERROR          (0x4U) /**< Error */
#define XSECURE_CSU_RSA_STATUS_PROG_CNT       (0xF8U)  /**< Progress Counter */
/* @}*/

#define XSECURE_CSU_RSA_RAM_EXPO		 (0U) /**< bit for RSA RAM Exponent */
#define XSECURE_CSU_RSA_RAM_MOD			 (1U) /**< bit for RSA RAM modulus */
#define XSECURE_CSU_RSA_RAM_DIGEST		 (2U) /**< bit for RSA RAM Digest */
#define XSECURE_CSU_RSA_RAM_SPAD		 (3U) /**< bit for RSA RAM SPAD */
#define XSECURE_CSU_RSA_RAM_RES_Y		 (4U) /**< bit for RSA RAM Result(Y) */
#define XSECURE_CSU_RSA_RAM_RES_Q		 (5U) /**< bit for RSA RAM Result(Q) */

#define XSECURE_CSU_RSA_RAM_WORDS		 (6U) /**< Total Locations in RSA RAM */

#define XSECURE_RSA_FAILED                0x1U /**< RSA Failed Error Code */

#define XSECURE_HASH_TYPE_SHA3				 (48U) /**< SHA-3 hash size */
#define XSECURE_HASH_TYPE_SHA2				 (32U)/**< SHA-2 hash size */
#define XSECURE_FSBL_SIG_SIZE				 (512U) /**< FSBL signature size */

/***************************** Type Definitions ******************************/
/**
 * The RSA driver instance data structure. A pointer to an instance data
 * structure is passed around by functions to refer to a specific driver
 * instance.
 */
typedef struct {
	u32 BaseAddress; /**< Device Base Address */
	u8* EncText; /**< Data to be Encrypted */
	u8* Mod; /**< Modulus */
	u8* ModExt; /**< Precalc. R sq. mod N */
	u8* ModExpo; /**< Exponent */
} XSecure_Rsa;

/***************************** Function Prototypes ***************************/

/*
 *  Initialization
 */
s32  XSecure_RsaInitialize(XSecure_Rsa *InstancePtr, u8* EncText,
									u8 *Mod, u8 *ModExt, u8 *ModExpo);

/*
 * RSA Decryption
 */
s32 XSecure_RsaDecrypt(XSecure_Rsa *InstancePtr, u8* Result);

/*
 * RSA Signature Validation, assuming PKCS padding
 */
u32 XSecure_RsaCheckPadding(u8 *Signature, u8 *Hash, u32 HashLen);

#ifdef __cplusplus
extern "C" }
#endif

#endif /* XSECURE_RSA_H_ */

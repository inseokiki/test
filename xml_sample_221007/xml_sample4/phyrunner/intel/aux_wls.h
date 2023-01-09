/**********************************************************************
*
* INTEL CONFIDENTIAL
* Copyright 2009-2019 Intel Corporation All Rights Reserved.
* 
* The source code contained or described herein and all documents related to the
* source code ("Material") are owned by Intel Corporation or its suppliers or
* licensors. Title to the Material remains with Intel Corporation or its
* suppliers and licensors. The Material may contain trade secrets and proprietary
* and confidential information of Intel Corporation and its suppliers and
* licensors, and is protected by worldwide copyright and trade secret laws and
* treaty provisions. No part of the Material may be used, copied, reproduced,
* modified, published, uploaded, posted, transmitted, distributed, or disclosed
* in any way without Intel's prior express written permission.
* 
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or delivery
* of the Materials, either expressly, by implication, inducement, estoppel or
* otherwise. Any license under such intellectual property rights must be
* express and approved by Intel in writing.
* 
* Unless otherwise agreed by Intel in writing, you may not remove or alter this
* notice or any other notice embedded in Materials by Intel or Intel's suppliers
* or licensors in any way.
* 
*  version: RefPHY-20.04
*
**********************************************************************/

/**
 * @brief Wireless Subsystem Interface Functions. This provides shared memory between MAC and PHY to exchange APIs
 * @file aux_wls.h
 * @ingroup group_source_auxlib_wls
 * @author Intel Corporation
 **/


#ifndef _AUX_WLS_H_
#define _AUX_WLS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common_typedef.h"

#define     aux_wls_virt_to_phys     wls_va_to_pa
#define     aux_wls_phys_to_virt     wls_pa_to_va

#define     WLS_STATE_NON_RT         (1)
#define     WLS_STATE_TTI_STARTED    (2)

uint32_t wls_layer_init(char * wls_device_name, uint64_t nWlsMemorySize);
uint32_t  wls_layer_close(void);
void wls_print_thread_info(void);

void *wls_va_to_pa(void *data);
void *wls_pa_to_va(void *data);

void *wls_alloc_buffer(uint32_t phyInstance, uint32_t size);
int  wls_free_buffer(void *ptr);
int  wls_send(unsigned long data, uint32_t size);

#ifdef NR5G
int  wls_send_list(unsigned long pList, uint32_t nIsUrllc);
#else
int  wls_send_list(unsigned long pList);
#endif

void wls_rx_process(void);
void wls_set_state(uint32_t nState);

int wls_is_native_ptr(void *data);

#ifdef NR5G
typedef uint32_t   (*wls_urllc_call_back_fn) (void *);
uint32_t wls_set_urllc_callback(wls_urllc_call_back_fn pUrllcCallBack);
#endif

#ifdef __cplusplus
}
#endif

#ifdef INSNODE_U50FH_INTERFACE_LOG_DEBUG
void wls_set_sfidx(uint32_t frameIdx,uint32_t slotIdx);
#endif

#endif /* #ifndef _AUX_WLS_H_*/


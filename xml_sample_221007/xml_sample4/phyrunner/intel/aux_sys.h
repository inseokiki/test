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
 * @brief This file contains all system level utilities which interfaces the operating system
 *        and the L1 userspace application
 * @file aux_sys.h
 * @ingroup group_source_auxlib_sys
 * @author Intel Corporation
 **/


#ifndef _AUX_SYS_H_
#define _AUX_SYS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>

#define SYS_IRQINFO_NAME                    "/proc/interrupts"

#define SYS_SIG_TYPE_SIGUSR1    0
#define SYS_SIG_TYPE_SIGUSR2    1
#define SYS_SIG_TYPE_SIGTERM    3

struct sys_rt_irq_thread
{
    pthread_t      thread;
    pthread_attr_t attr;
    int            priority;
    int            policy;
    volatile int   started;
    int            dev;
};


typedef void (*SYS_SIG_H) (void);

int sys_init(void);
void sys_termios_echo_off(void);
void sys_termios_echo_on(void);
void sys_assert_handler(uint32_t rc, const char *filename, uint32_t nRow);
int sys_pthread_set_prio(int prio, int policy);
int sys_pthread_remove(pthread_t * p_thread);
int sys_pthread_create(struct sys_rt_irq_thread *rt, void *thread_fn(void*),
            void *thread_fn_param, int policy, int priority);
pid_t sys_get_pid(void);
int sys_check_daemon_mode(uint32_t appRunMode);
void sys_load_app_in_ram(char *appname);
int sys_reg_signals(void);
void sys_reg_signal_handler(SYS_SIG_H hSysSig, uint32_t type);
int sys_affinity_bind(int coreNum);
int sys_kernel_irq_info(void);

void sys_save_buf_to_file(char *filename, char *bufname, unsigned char *pBuffer, uint32_t size, uint32_t buffers_num);
int sys_load_file_to_buff(char *filename, char *bufname, unsigned char *pBuffer, uint32_t size, uint32_t buffers_num);
void sys_save_buf_to_file_8bit_txt(char *filename, char *bufname, unsigned char *pBuffer, uint32_t size, uint32_t buffers_num);
void sys_save_buf_to_file_txt(char *filename, char *bufname, unsigned char *pBuffer, uint32_t size, uint32_t buffers_num);
void sys_save_buf_to_file_txt_one_column(char *filename, char *bufname, unsigned char *pBuffer, uint32_t size, uint32_t buffers_num);
void sys_save_bufS64_to_file_txt(char *filename, char *bufname, unsigned char *pBuffer, uint32_t size, uint32_t buffers_num);
int sys_strcmp (const char * pSrc, const char * pDst);


#define _assert(x) if (((unsigned long)x) == 0) { sys_assert_handler (((unsigned long)x), __FILE__, __LINE__);exit (1);}
#define _assert_rc(x) if (((unsigned long)x) != SUCCESS) { sys_assert_handler (((unsigned long)x), __FILE__, __LINE__);exit (1);}

#ifdef __cplusplus
}
#endif


#endif /*_AUX_SYS_H_*/


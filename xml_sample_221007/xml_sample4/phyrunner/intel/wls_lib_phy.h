/**********************************************************************
*
* This file is provided under a dual BSD/GPLv2 license.  When using or
*   redistributing this file, you may do so under either license.
* 
*   GPL LICENSE SUMMARY
* 
*   Copyright(c) 2007-2019 Intel Corporation. All rights reserved.
* 
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of version 2 of the GNU General Public License as
*   published by the Free Software Foundation.
* 
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
* 
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
*   The full GNU General Public License is included in this distribution
*   in the file called LICENSE.GPL.
* 
*   Contact Information:
*   Intel Corporation
* 
*   BSD LICENSE
* 
*   Copyright(c) 2007-2019 Intel Corporation. All rights reserved.
*   All rights reserved.
* 
*   Redistribution and use in source and binary forms, with or without
*   modification, are permitted provided that the following conditions
*   are met:
* 
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided with the
*       distribution.
*     * Neither the name of Intel Corporation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
* 
*   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* 
*  version: BSP-20.04
*
**********************************************************************/

#ifndef _WLS_LIB_H_
#define _WLS_LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

/** WLS driver client operates as slave in terms of management of shared memory */
#define WLS_SLAVE_CLIENT   0
/** WLS driver client operates as master in terms of management of shared memory */
#define WLS_MASTER_CLIENT  1

/* definitions PUT/GET Flags */
#define WLS_TF_SCATTER_GATHER  (1 << 15)
#define WLS_TF_URLLC           (1 << 10)
#define WLS_TF_SYN             (1 << 9)
#define WLS_TF_FIN             (1 << 8)
#define WLS_FLAGS_MASK         (0xFF00)

/** First block in Scatter/Gather sequence of blocks */
#define WLS_SG_FIRST               (WLS_TF_SCATTER_GATHER | WLS_TF_SYN)
/** Next block in Scatter/Gather sequence of blocks */
#define WLS_SG_NEXT                (WLS_TF_SCATTER_GATHER)
/** Last block in Scatter/Gather sequence of blocks */
#define WLS_SG_LAST                (WLS_TF_SCATTER_GATHER | WLS_TF_FIN)

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
 *
 *  @param[in]   ifacename - pointer to string with device driver name (/dev/wls)
 *  @param[in]   modef     - mode of operation (Master or Slave)
 *
 *  @return  pointer to WLS handle
 *
 *  @description
 *  Function opens the WLS interface and registers as instance in the kernel space driver.
 *  Control section of shared memory is mapped to application memory.
 *  pointer (handle) of WLS interface is returned for future use by WLS functions
 *
**/
//-------------------------------------------------------------------------------------------
void* WLS_Open(const char *ifacename, unsigned int mode, unsigned long long nWlsMemorySize);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
 *
 *  @param[in]   h - handle of WLS interface to close
 *
 *  @return  0 - in case of success
 *
 *  @description
 *  Function closes the WLS interface and deregisters as instance in the kernel space driver.
 *  Control section of shared memory is unmapped form user space application
 *
**/
//-------------------------------------------------------------------------------------------
int WLS_Close(void* h);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
 *
 *  @param[in]   h - handle of WLS interface to check status
 *
 *  @return  1 - in case of success
 *
 *  @description
 *  Function checks state of remote peer of WLS interface and returns 1 if remote peer is available
 *  (one to one connection is established)
 *
**/
//-------------------------------------------------------------------------------------------
int WLS_Ready(void* h);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
 *
 *  @param[in]   h    - handle of WLS interface
 *  @param[in]   size - size of memory block to allocate
 *
 *  @return  void*    - pointer to allocated memory block or NULL if no memory available
 *
 *  @description
 *  Function allocates memory block for data exchange shared memory. Memory block is backed
 *  by huge pages.
 *
**/
//-------------------------------------------------------------------------------------------
void* WLS_Alloc(void* h, unsigned int size);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*  @param[in]   pMsg - pointer to WLS memory
*
*  @return  0 - if operation is successful
*
*  @description
*  Function frees memory block for data exchange shared memory. Memory block is backed
*  by huge pages
*
**/
//-------------------------------------------------------------------------------------------
int WLS_Free(void* h, void* pMsg);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
 *
 *  @param[in]   h    - handle of WLS interface
 *  @param[in]   pMsg - pointer to memory block (physical address) with data to be transfered to remote peer.
 *                      pointer should belong to WLS memory allocated via WLS_Alloc()
 *  @param[in]   MsgSize - size of memory block to send (should be less than 2 MB)
 *  @param[in]   MsgTypeID - application specific identifier of message type
 *  @param[in]   Flags - Scatter/Gather flag if memory block has multiple chunks
 *
 *  @return  0 - if successful
 *          -1 - if error
 *
 *  @description
 *  Function puts memory block (or group of blocks) allocated from WLS memory into interface
 *  for transfer to remote peer.
 *
**/
//-------------------------------------------------------------------------------------------
int WLS_Put(void* h, unsigned long long pMsg, unsigned int MsgSize, unsigned short MsgTypeID, unsigned short Flags);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
 *
 *  @param[in]   h    - handle of WLS interface
 *
 *  @return  number of blocks available
 *
 *  @description
 *  Function checks if there are memory blocks with data from remote peer and returns number of blocks
 *  available for "get" operation
 *
**/
//-------------------------------------------------------------------------------------------
int WLS_Check(void* h);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*  @param[in]   *MsgSize - pointer to set size of memory block
*  @param[in]   *MsgTypeID - pointer to application specific identifier of message type
*  @param[in]   *Flags - pointer to Scatter/Gather flag if memory block has multiple chunks
*
*  @return  pointer to memory block (physical address) with data received from remote peer
*           NULL -  if error
*
*  @description
*  Function gets memory block from interface received from remote peer. Function is non-blocking
*  operation and returns NULL if no blocks available
*
**/
//-------------------------------------------------------------------------------------------
unsigned long long WLS_Get(void* h, unsigned int *MsgSize, unsigned short *MsgTypeID, unsigned short *Flags);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*
*  @return  number of blocks available for get
*
*  @description
*  Function waits for new memory block from remote peer. Function is blocking call and returns number
*  of blocks received.
*
**/
//-------------------------------------------------------------------------------------------
int WLS_Wait(void* h);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*
*  @return  0 - if successful
*
*  @description
*  Function performs "wakeup" notification to remote peer to unblock "wait" operations pending
*
**/
//-------------------------------------------------------------------------------------------
int WLS_WakeUp(void* h);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*  @param[in]   *MsgSize - pointer to set size of memory block
*  @param[in]   *MsgTypeID - pointer to application specific identifier of message type
*  @param[in]   *Flags - pointer to Scatter/Gather flag if memory block has multiple chunks
*
*  @return  pointer to memory block (physical address) with data received from remote peer
*           NULL -  if error
*
*  @description
*  Function gets memory block from interface received from remote peer. Function is blocking
*  operation and waits till next memory block from remote peer.
*
**/
//-------------------------------------------------------------------------------------------
unsigned long long WLS_WGet(void* h, unsigned int *MsgSize, unsigned short *MsgTypeID, unsigned short *Flags);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*  @param[in]   pMsg - virtual address of WLS memory block.
*
*  @return  physical address of WLS memory block
*           NULL - if error
*
*  @description
*  Function converts virtual address (VA) to physical address (PA)
*
**/
//-------------------------------------------------------------------------------------------
unsigned long long WLS_VA2PA(void* h, void* pMsg);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*  @param[in]   pMsg - physical address of WLS memory block.
*
*  @return  virtual address of WLS memory block
*           NULL - if error
*
*  @description
*  Function converts physical address (PA) to virtual address (VA)
*
**/
//-------------------------------------------------------------------------------------------
void* WLS_PA2VA(void* h, unsigned long long pMsg);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*  @param[in]   pMsg - physical address of WLS memory block.
*
*  @return  0 - if successful
*          -1 - if error
*
*  @description
*  Function is used by master to provide memory blocks to slave for next slave to master transfer
*  of data.
*
**/
//-------------------------------------------------------------------------------------------
int WLS_EnqueueBlock(void* h, unsigned long long pMsg);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*
*  @return  0   - pointer (physical address) of WLS memory block
*          NULL - if error
*
*  @description
*  Function is used by master and slave to get block from master to slave queue of available memory
*  blocks.
*
**/
//-------------------------------------------------------------------------------------------
unsigned long long WLS_DequeueBlock(void* h);

//-------------------------------------------------------------------------------------------
/** @ingroup wls_mod
*
*  @param[in]   h    - handle of WLS interface
*
*  @return  number of blocks in slave to master queue
*
*  @description
*  Function returns number of current available block provided by master for new transfer
*  of data form slave.
*
**/
//-------------------------------------------------------------------------------------------
int WLS_NumBlocks(void* h);


//////////////////////////////////////////////////////////////////////////////////////////
// Definitions for PHY side

#define print_log(fmt, args...) printf("%s:" fmt "\n", __FUNCTION__, ## args)
#define print_dbg(fmt, args...) // printf("%s:[dbg] " fmt "\n", __FUNCTION__, ## args)
#define print_err(fmt, args...) printf("%s:[err] " fmt "\n", __FUNCTION__, ## args)

#define WLS_MAX_PHY_INSTANCES       ( 13 )
#define WLS_MAX_NUM_BLOCKS          ( 48 )

#define IncWlsAllocBlock(a) ((a < (WLS_MAX_NUM_BLOCKS-1)) ? (a+1) : 0)

#define MAX_PHY_INSTANCES           ( 12 )

typedef uint32_t   (*wls_urllc_call_back_fn) (void *);

typedef struct _PHY_WLS_LAYER_CTX_
{
    pthread_t           *p_pthread_wls_phy;
    sem_t               *p_sem_wls_phy;
    volatile int        f_runthread_wls_phy;
    volatile int        f_thread_wls_phy_started;

    int                 dlZbcEnable;
    int                 wlsState;
    volatile int        wlsPrintThreadInfo;

    void *              hWls;
    void*               wls_mem_base;
    uint32_t            wls_mem_size;

    uint32_t            nWlsFirst;
    uint32_t            nPush[WLS_MAX_PHY_INSTANCES];
    uint32_t            nPop[WLS_MAX_PHY_INSTANCES];
    void*               pWlsBuffer[WLS_MAX_PHY_INSTANCES][WLS_MAX_NUM_BLOCKS];

    int                 nZbc;
    int                 nFirst;

    int                 nZbcUrllc;
    int                 nFirstUrllc;
    wls_urllc_call_back_fn pUrllcCallBack;
} PHY_WLS_LAYER_CTX, *PPHY_WLS_LAYER_CTX;

extern pthread_mutex_t wlsSendLock;
extern pthread_mutex_t wlsUlLock;

extern void setWlsRxProcess( void *pFunc );
extern void wls_allocate_buffer_to_queue(uint32_t nFirst);
extern PPHY_WLS_LAYER_CTX wls_get_layer_ctx(void);




#ifdef __cplusplus
}
#endif
#endif //_WLS_LIB_H_

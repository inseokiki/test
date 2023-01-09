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
 * @brief This file has Shared Memory interface functions between MAC and PHY
 * @file testmac_wls.c
 * @ingroup group_testmac
 * @author Intel Corporation
 **/


#include <sys/ioctl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>

#include "wls_lib_mac.h"

#define MSG_MAXSIZE                         ( 16384 * 16 )

#define ALLOC_TRACK_SIZE                    ( 16384 )

#ifndef SUCCESS
/** SUCCESS = 0 */
#define SUCCESS     0
#endif /* #ifndef SUCCESS */

#ifndef FAILURE
/** FAILURE = 1 */
#define FAILURE     1
#endif /* #ifndef FAILURE */

#define print_err(...)
#define print_dbg(...) 

int gwls_mac_ready = 0;

static WLS_MAC_CTX wls_mac_iface;
static uint8_t alloc_track[ALLOC_TRACK_SIZE];

//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param void
 *
 *  @return  Pointer to WLS_MAC_CTX stucture
 *
 *  @description
 *  This function returns the WLS Local structure which has WLS related parameters
 *
**/
//-------------------------------------------------------------------------------------------
PWLS_MAC_CTX wls_mac_get_ctx(void)
{
    return &wls_mac_iface;
}

void wls_mac_print_stats(void)
{
    PWLS_MAC_CTX pWls = wls_mac_get_ctx();

    printf("wls_mac_free_list_all:\n");
    printf("        nTotalBlocks[%d] nAllocBlocks[%d] nFreeBlocks[%d]\n", pWls->nTotalBlocks, pWls->nAllocBlocks, (pWls->nTotalBlocks- pWls->nAllocBlocks));
    printf("        nTotalAllocCnt[%d] nTotalFreeCnt[%d] Diff[%d]\n", pWls->nTotalAllocCnt, pWls->nTotalFreeCnt, (pWls->nTotalAllocCnt- pWls->nTotalFreeCnt));
    printf("        nDlBufAllocCnt[%d] nDlBufFreeCnt[%d] Diff[%d]\n", pWls->nTotalDlBufAllocCnt, pWls->nTotalDlBufFreeCnt, (pWls->nTotalDlBufAllocCnt- pWls->nTotalDlBufFreeCnt));
    printf("        nUlBufAllocCnt[%d] nUlBufFreeCnt[%d] Diff[%d]\n\n", pWls->nTotalUlBufAllocCnt, pWls->nTotalUlBufFreeCnt, (pWls->nTotalUlBufAllocCnt- pWls->nTotalUlBufFreeCnt));
}



//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[in]   ptr Address to convert
 *
 *  @return  Converted address
 *
 *  @description
 *  This function converts Virtual Address to Physical Address
 *
**/
//-------------------------------------------------------------------------------------------
uint64_t wls_mac_va_to_pa(void *ptr)
{
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();
    uint64_t ret = (uint64_t)WLS_VA2PA(pWls->hWls, ptr);

    //printf("wls_mac_va_to_pa: %p ->%p\n", ptr, (void*)ret);

    return ret;
}



//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[in]   ptr Address to convert
 *
 *  @return  Converted address
 *
 *  @description
 *  This function converts Physical Address to Virtual Address
 *
**/
//-------------------------------------------------------------------------------------------
void *wls_mac_pa_to_va(uint64_t ptr)
{
    PWLS_MAC_CTX pWls = wls_mac_get_ctx();
    void *ret = WLS_PA2VA(pWls->hWls, ptr);

    //printf("wls_mac_pa_to_va: %p -> %p\n", (void*)ptr, ret);

    return ret;
}



//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[in]   pMemArray Pointer to WLS Memory management structure
 *  @param[in]   pMemArrayMemory Pointer to flat buffer that was allocated
 *  @param[in]   totalSize Total Size of flat buffer allocated
 *  @param[in]   nBlockSize Size of each block that needs to be partitioned by memory manager
 *
 *  @return  0 if SUCCESS
 *
 *  @description
 *  This function creates memory blocks from a flat buffer which will be used for communciation
 *  between MAC and PHY
 *
**/
//-------------------------------------------------------------------------------------------
uint32_t wls_mac_create_mem_array(PWLS_MAC_MEM_SRUCT pMemArray,
                              void *pMemArrayMemory,
                              uint32_t totalSize, uint32_t nBlockSize)
{
    int numBlocks = totalSize / nBlockSize;
    void **ptr;
    uint32_t i;

    printf("wls_mac_create_mem_array: pMemArray[%p] pMemArrayMemory[%p] totalSize[%d] nBlockSize[%d] numBlocks[%d]\n",
        pMemArray, pMemArrayMemory, totalSize, nBlockSize, numBlocks);

    // Can't be less than pointer size
    if (nBlockSize < sizeof(void *))
    {
        return FAILURE;
    }

    // Can't be less than one block
    if (totalSize < sizeof(void *))
    {
        return FAILURE;
    }

    pMemArray->ppFreeBlock = (void **)pMemArrayMemory;
    pMemArray->pStorage = pMemArrayMemory;
    pMemArray->pEndOfStorage = ((unsigned long*)pMemArrayMemory) + numBlocks * nBlockSize / sizeof(unsigned long);
    pMemArray->nBlockSize = nBlockSize;
    pMemArray->nBlockCount = numBlocks;

    // Initialize single-linked list of free blocks;
    ptr = (void **)pMemArrayMemory;
    for (i = 0; i < pMemArray->nBlockCount; i++)
    {
#ifdef MEMORY_CORRUPTION_DETECT
        // Fill with some pattern
        uint8_t *p = (uint8_t *)ptr;
        uint32_t j;

        p += (nBlockSize - 16);
        for (j = 0; j < 16; j++)
        {
            p[j] = MEMORY_CORRUPTION_DETECT_FLAG;
        }
#endif

        if (i == pMemArray->nBlockCount - 1)
        {
            *ptr = NULL;      // End of list
        }
        else
        {
            // Points to the next block
            *ptr = (void **)(((uint8_t*)ptr) + nBlockSize);
            ptr += nBlockSize / sizeof(unsigned long);
        }
    }

    memset(alloc_track, 0, sizeof(uint8_t) * ALLOC_TRACK_SIZE);

    return SUCCESS;
}




//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[in]    pMemArray Pointer to WLS Memory management structure
 *  @param[out]   ppBlock Pointer where allocated memory block is stored
 *
 *  @return  0 if SUCCESS
 *
 *  @description
 *  This function allocated a memory block from pool
 *
**/
//-------------------------------------------------------------------------------------------
uint32_t wls_mac_alloc_mem_array(PWLS_MAC_MEM_SRUCT pMemArray, void **ppBlock)
{
    int idx;

    if (pMemArray->ppFreeBlock == NULL)
    {
        printf("wls_mac_alloc_mem_array pMemArray->ppFreeBlock = NULL\n");
        return FAILURE;
    }

    // FIXME: Remove after debugging
    if (((void *) pMemArray->ppFreeBlock < pMemArray->pStorage) ||
        ((void *) pMemArray->ppFreeBlock >= pMemArray->pEndOfStorage))
    {
        printf("wls_mac_alloc_mem_array ERROR: Corrupted MemArray;Arr=%p,Stor=%p,Free=%p\n",
                pMemArray, pMemArray->pStorage, pMemArray->ppFreeBlock);
        return FAILURE;
    }

    pMemArray->ppFreeBlock = (void **)((unsigned long)pMemArray->ppFreeBlock & 0xFFFFFFFFFFFFFFF0);
    *pMemArray->ppFreeBlock = (void **)((unsigned long)*pMemArray->ppFreeBlock & 0xFFFFFFFFFFFFFFF0);

    if ((*pMemArray->ppFreeBlock != NULL) &&
        (((*pMemArray->ppFreeBlock) < pMemArray->pStorage) ||
        ((*pMemArray->ppFreeBlock) >= pMemArray->pEndOfStorage)))
    {
        fprintf(stderr, "ERROR: Corrupted MemArray;Arr=%p,Stor=%p,Free=%p,Curr=%p\n",
                pMemArray, pMemArray->pStorage, pMemArray->ppFreeBlock,
                *pMemArray->ppFreeBlock);
        return FAILURE;
    }

    *ppBlock = (void *) pMemArray->ppFreeBlock;
    pMemArray->ppFreeBlock = (void **) (*pMemArray->ppFreeBlock);

    idx = (((uint64_t)*ppBlock - (uint64_t)pMemArray->pStorage)) / pMemArray->nBlockSize;
    if (alloc_track[idx])
    {
        printf("wls_mac_alloc_mem_array Double alloc Arr=%p,Stor=%p,Free=%p,Curr=%p\n",
            pMemArray, pMemArray->pStorage, pMemArray->ppFreeBlock,
            *pMemArray->ppFreeBlock);
    }
    else
    {
#ifdef MEMORY_CORRUPTION_DETECT
        uint32_t nBlockSize = pMemArray->nBlockSize, i;
        uint8_t *p = (uint8_t *)*ppBlock;

        p += (nBlockSize - 16);
        for (i = 0; i < 16; i++)
        {
            p[i] = MEMORY_CORRUPTION_DETECT_FLAG;
        }
#endif
        alloc_track[idx] = 1;
    }

    //printf("Block allocd [%p,%p]\n", pMemArray, *ppBlock);

    return SUCCESS;
}




//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[in]   pMemArray Pointer to WLS Memory management structure
 *  @param[in]   pBlock Pointer to block that needs to be added back to pool
 *
 *  @return  0 if SUCCESS
 *
 *  @description
 *  This function frees a WLS block of memory and adds it back to the pool
 *
**/
//-------------------------------------------------------------------------------------------
uint32_t wls_mac_free_mem_array(PWLS_MAC_MEM_SRUCT pMemArray, void *pBlock)
{
    int idx;
    unsigned long mask = (((unsigned long)pMemArray->nBlockSize) - 1);

    pBlock = (void *)((unsigned long)pBlock & ~mask);

    if ((pBlock < pMemArray->pStorage) || (pBlock >= pMemArray->pEndOfStorage))
    {
        printf("wls_mac_free_mem_array WARNING: Trying to free foreign block;Arr=%p,Blk=%p pStorage [%p .. %p]\n",
               pMemArray, pBlock, pMemArray->pStorage, pMemArray->pEndOfStorage);

        return FAILURE;
    }

    idx = (int)(((uint64_t)pBlock - (uint64_t)pMemArray->pStorage)) / pMemArray->nBlockSize;

    if (alloc_track[idx] == 0)
    {
        printf("wls_mac_free_mem_array ERROR: Double free Arr=%p,Stor=%p,Free=%p,Curr=%p\n",
            pMemArray, pMemArray->pStorage, pMemArray->ppFreeBlock,
            pBlock);

        return SUCCESS;

    }
    else
    {
#ifdef MEMORY_CORRUPTION_DETECT
        uint32_t nBlockSize = pMemArray->nBlockSize, i;
        uint8_t *p = (uint8_t *)pBlock;

        p += (nBlockSize - 16);
        for (i = 0; i < 16; i++)
        {
            if (p[i] != MEMORY_CORRUPTION_DETECT_FLAG)
            {
                printf("ERROR: Corruption\n");
                wls_mac_print_stats();
                exit(-1);
            }
        }
#endif
        alloc_track[idx] = 0;
    }

    if (((void *) pMemArray->ppFreeBlock) == pBlock)
    {
        // Simple protection against freeing of already freed block
        return SUCCESS;
    }

    // FIXME: Remove after debugging
    if ((pMemArray->ppFreeBlock != NULL)
        && (((void *) pMemArray->ppFreeBlock < pMemArray->pStorage)
        || ((void *) pMemArray->ppFreeBlock >= pMemArray->pEndOfStorage)))
    {
        printf("wls_mac_free_mem_array ERROR: Corrupted MemArray;Arr=%p,Stor=%p,Free=%p\n",
                pMemArray, pMemArray->pStorage, pMemArray->ppFreeBlock);
        return FAILURE;
    }

    // FIXME: Remove after debugging
    if ((pBlock < pMemArray->pStorage) ||
        (pBlock >= pMemArray->pEndOfStorage))
    {
        printf("wls_mac_free_mem_array ERROR: Invalid block;Arr=%p,Blk=%p\n",
                pMemArray, pBlock);
        return FAILURE;
    }

    *((void **)pBlock) = (void **)((unsigned long)pMemArray->ppFreeBlock & 0xFFFFFFFFFFFFFFF0);
    pMemArray->ppFreeBlock = (void **) ((unsigned long)pBlock & 0xFFFFFFFFFFFFFFF0);

    //printf("Block freed [%p,%p]\n", pMemArray, pBlock);

    return SUCCESS;
}






//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param   void
 *
 *  @return  Pointer to the memory block
 *
 *  @description
 *  This function allocates a block of memory from the pool
 *
**/
//-------------------------------------------------------------------------------------------
void *wls_mac_alloc_buffer(uint32_t size, uint32_t loc)
{
    void *pBlock = NULL;
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();

    pthread_mutex_lock((pthread_mutex_t *)&pWls->lock_alloc);

    if (wls_mac_alloc_mem_array(&pWls->sWlsStruct, &pBlock) != SUCCESS)
    {
        printf("wls_mac_alloc_buffer alloc error size[%d] loc[%d]\n", size, loc);
        wls_mac_print_stats();
        exit(-1);
    }
    else
    {
        pWls->nAllocBlocks++;
    }

    //printf("----------------wls_mac_alloc_buffer: size[%d] loc[%d] buf[%p] nAllocBlocks[%d]\n", size, loc, pBlock, pWls->nAllocBlocks);

    //printf("[%p]\n", pBlock);

    pWls->nTotalAllocCnt++;
    if (loc < MAX_DL_BUF_LOCATIONS)
        pWls->nTotalDlBufAllocCnt++;
    else if (loc < MAX_UL_BUF_LOCATIONS)
        pWls->nTotalUlBufAllocCnt++;

    pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock_alloc);

    return pBlock;
}



//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[in]   *pMsg Pointer to free
 *
 *  @return  void
 *
 *  @description
 *  This function frees a block of memory and adds it back to the pool
 *
**/
//-------------------------------------------------------------------------------------------
void wls_mac_free_buffer(void *pMsg, uint32_t loc)
{
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();

    pthread_mutex_lock((pthread_mutex_t *)&pWls->lock_alloc);

    //printf("----------------wls_mac_free_buffer: buf[%p] loc[%d]\n", pMsg, loc);
    if (wls_mac_free_mem_array(&pWls->sWlsStruct, (void *)pMsg) == SUCCESS)
    {
        pWls->nAllocBlocks--;
    }
    else
    {
        printf("wls_mac_free_buffer Free error\n");
        wls_mac_print_stats();
        exit(-1);
    }

    pWls->nTotalFreeCnt++;
    if (loc < MAX_DL_BUF_LOCATIONS)
        pWls->nTotalDlBufFreeCnt++;
    else if (loc < MAX_UL_BUF_LOCATIONS)
        pWls->nTotalUlBufFreeCnt++;

    pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock_alloc);
}



//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param   void
 *
 *  @return  Number of free blocks
 *
 *  @description
 *  This function queries the number of free blocks in the system
 *
**/
//-------------------------------------------------------------------------------------------
int wls_mac_num_free_blocks(void)
{
    PWLS_MAC_CTX pWls = wls_mac_get_ctx();

    return (pWls->nTotalBlocks- pWls->nAllocBlocks);
}

//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param   void
 *
 *  @return  Number of blocks added
 *
 *  @description
 *  This function add WLS blocks to the L1 Array which will be used by L1 in every TTI to
 *  populate and send back APIs to the MAC
 *
**/
//-------------------------------------------------------------------------------------------
int wls_mac_add_blocks_to_ul(void)
{
    int ret = 0;
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();

    void *pMsg = wls_mac_alloc_buffer(0, MIN_UL_BUF_LOCATIONS+0);

    if(pMsg)
    {
        /* allocate blocks for UL transmittion */
        while(WLS_EnqueueBlock(pWls->hWls,(uint64_t)wls_mac_va_to_pa(pMsg)))
        {
            ret++;
            pMsg = wls_mac_alloc_buffer(0, MIN_UL_BUF_LOCATIONS+1);
            if(!pMsg)
                break;
        }

        // free not enqueued block
        if(pMsg)
        {
            wls_mac_free_buffer(pMsg, MIN_UL_BUF_LOCATIONS+3);
        }
    }

    return ret;
}



//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[in]   pWls Pointer to the WLS_MAC_CTX structure
 *
 *  @return  0 if SUCCESS
 *
 *  @description
 *  This function created a partition and blocks of WLS memory for API exchange between MAC and PHY
 *
**/
//-------------------------------------------------------------------------------------------
int wls_mac_create_partition(PWLS_MAC_CTX pWls)
{
    memset(pWls->pWlsMemBase, 0xCC, pWls->nTotalMemorySize);

    pWls->nTotalBlocks = pWls->nTotalMemorySize / MSG_MAXSIZE;

    return wls_mac_create_mem_array(&pWls->sWlsStruct, pWls->pWlsMemBase, pWls->nTotalMemorySize, MSG_MAXSIZE);
}



//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param void
 *
 *  @return  0 if SUCCESS
 *
 *  @description
 *  This function is called at WLS init and waits in an infinite for L1 to respond back with some information
 *  needed by the L2
 *
**/
//-------------------------------------------------------------------------------------------
int wls_mac_ready(void)
{
    int ret = 0;
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();
    ret = WLS_Ready(pWls->hWls);

    return ret;
}




//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param   void
 *
 *  @return  Number of blocks of APIs received
 *
 *  @description
 *  This functions waits in a infinite loop for L1 to send a list of APIs to MAC. This is called
 *  during runtime when L2 sends a API to L1 and then waits for response back.
 *
**/
//-------------------------------------------------------------------------------------------
int wls_mac_wait(void)
{
    int ret = 0;
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();

    ret = WLS_Wait(pWls->hWls);

    return ret;
}



//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[out]   data Location where First API from L1 is stored
 *
 *  @return  Size of Message sent from L1
 *
 *  @description
 *  This function queries the APIs sent from L1 to L2 and gets the first pointer to the linked list
 *
**/
//-------------------------------------------------------------------------------------------
uint32_t wls_mac_recv(uint64_t *data, uint16_t *nFlags)
{
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();
    uint32_t msgSize = 0;
    uint16_t msgType = 0;

    *data = WLS_Get(pWls->hWls, &msgSize, &msgType, nFlags);

    return msgSize;
}

//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param[in]   pMsg Pointer to API block that needs to be sent to L1
 *  @param[in]   MsgSize Size of Message
 *  @param[in]   MsgTypeID Message Id
 *  @param[in]   Flags Special Flags needed for WLS
 *
 *  @return  0 if SUCCESS
 *
 *  @description
 *  This function adds a block of API from L2 to L1 which will be sent later
 *
**/
//-------------------------------------------------------------------------------------------
int wls_mac_put(uint64_t pMsg, uint32_t MsgSize, uint16_t MsgTypeID, uint16_t Flags)
{
    int ret = 0;
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();
    //uint64_t t1 = MLogTick();

    //printf("wls_mac_put: %p size: %d type: %d nFlags: %d\n", (void*)pMsg, MsgSize, MsgTypeID, Flags);
    //  wls_mac_show_data((void*)wls_alloc_buffer(pMsg), MsgSize);
    ret = WLS_Put(pWls->hWls, pMsg, MsgSize, MsgTypeID, Flags);
    return ret;
}


//-------------------------------------------------------------------------------------------
/** @ingroup group_testmac
 *
 *  @param   void
 *
 *  @return  0 if SUCCESS
 *
 *  @description
 *  This function initialized the WLS threads for the Testmac and allocates memory needed to
 *  exchange APIs between MAC and PHY
 *
**/
//-------------------------------------------------------------------------------------------
uint32_t wls_mac_init(char * wls_device_name, uint64_t nTotalMemorySize)
{
    uint32_t ret = FAILURE;
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();

    sleep(1);

    pthread_mutex_init((pthread_mutex_t *)&pWls->lock, NULL);
    pthread_mutex_init((pthread_mutex_t *)&pWls->lock_alloc, NULL);

    pWls->nTotalAllocCnt = 0;
    pWls->nTotalFreeCnt = 0;
    pWls->nTotalUlBufAllocCnt = 0;
    pWls->nTotalUlBufFreeCnt = 0;
    pWls->nTotalDlBufAllocCnt = 0;
    pWls->nTotalDlBufFreeCnt = 0;

    pWls->hWls = WLS_Open(wls_device_name, WLS_MASTER_CLIENT, nTotalMemorySize);
    if (pWls->hWls)
    {
        /* allocate chuck of memory */
        pWls->pWlsMemBase = WLS_Alloc(pWls->hWls, nTotalMemorySize);
        if (pWls->pWlsMemBase)
        {
            pWls->nTotalMemorySize = (uint32_t) nTotalMemorySize;

            ret = wls_mac_create_partition(pWls);

            if (ret == SUCCESS)
            {
                int nBlocks = 0;
                gwls_mac_ready = 1;

                nBlocks = WLS_EnqueueBlock(pWls->hWls, wls_mac_va_to_pa(wls_mac_alloc_buffer(0, MIN_UL_BUF_LOCATIONS+2)));

                printf("WLS_EnqueueBlock [%d]\n", nBlocks);


                /* allocate blocks for UL transmittion */
                while(WLS_EnqueueBlock(pWls->hWls, wls_mac_va_to_pa(wls_mac_alloc_buffer(0, MIN_UL_BUF_LOCATIONS+3))))
                {
                    nBlocks++;
                }

                printf("WLS inited ok [%d]\n\n", nBlocks);
            }
            else
            {
                print_err("can't create WLS Partiton");
                return FAILURE;
            }

        }
        else
        {
            print_err("can't allocate WLS memory");
            return FAILURE;
        }
    }
    else
    {
        print_err("can't open WLS instance");
        return FAILURE;
    }

    return SUCCESS;
}




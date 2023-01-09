#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <time.h>
#include <memory.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/syscall.h>

#include "wls_lib_phy.h"
#include "aux_wls.h"

static ALIGN64 PHY_WLS_LAYER_CTX wls_layer_ctx;

pthread_mutex_t wlsSendLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wlsUlLock = PTHREAD_MUTEX_INITIALIZER;

void (*g_wlsRxProcess) (void) = NULL;

void setWlsRxProcess( void *pFunc )
{
    g_wlsRxProcess = pFunc;
}

PPHY_WLS_LAYER_CTX wls_get_layer_ctx(void)
{
    return &wls_layer_ctx;
}

pid_t sys_get_pid(void)
{
    return syscall(SYS_gettid);
}

int sys_pthread_remove(pthread_t * p_thread)
{
    int rc;

    rc = pthread_cancel(*p_thread);

    if (rc)
    {
        print_err("thread cancellation wasn't queued (shouldn't happen!)");
        return rc;
    }

    return SUCCESS;
}

int wls_is_native_ptr(void *data)
{
    PPHY_WLS_LAYER_CTX pCtx = (PPHY_WLS_LAYER_CTX) wls_get_layer_ctx();

    if((unsigned long)data >= (unsigned long)pCtx->wls_mem_base && (unsigned long)data < ((unsigned long)pCtx->wls_mem_base + pCtx->wls_mem_size))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void wls_allocate_buffer_to_queue(uint32_t nFirst)
{
    PPHY_WLS_LAYER_CTX pCtx = (PPHY_WLS_LAYER_CTX) wls_get_layer_ctx();
    uint32_t i, j;
    uint32_t nPop, nPush, nAllDone = 1;

    pthread_mutex_lock(&wlsUlLock);

    //memset(nVars, 0, sizeof(uint32_t)*40);
    //nVars[0] = 0x00001111;

    if ((nFirst == 0) || (nFirst == 2))
    {
        for (i = 0; i < WLS_MAX_PHY_INSTANCES; i++)
        {
            nPush = pCtx->nPush[i];
            nPop = pCtx->nPop[i];
            //nVars[i+20] = (nPush << 16) | nPop;

            if (nPush != nPop)
            {
                while (nPush != nPop)
                {
                    if (pCtx->pWlsBuffer[i][nPush] == NULL)
                    {
                        pCtx->pWlsBuffer[i][nPush] = (void *)WLS_DequeueBlock((void*)pCtx->hWls);
                        //printf("   Push[%d][%d][%d][%p]\n", i, nPop, nPush, pCtx->pWlsBuffer[i][nPush]);
                    }
                    else
                    {
                        printf("wls_allocate_buffer_to_queue[phyInstance %d][nPop %d][nPush %d] is not NULL %p\n", i, nPop, nPush, pCtx->pWlsBuffer[i][nPush]);
                    }

                    if (pCtx->pWlsBuffer[i][nPush] == NULL)
                    {
                        //printf("   Push is null[%d][%d][%d][%p]\n", i, nPop, nPush, pCtx->pWlsBuffer[i][nPush]);
                        break;
                    }
                    else
                    {
                        nPush = IncWlsAllocBlock(nPush);
                        //nVars[i+1]++;
                    }
                }
                pCtx->nPush[i] = nPush;
            }
        }
    }

    if (nFirst)
    {
        // printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!wls_allocate_buffer_to_queue[%d]!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", nFirst);
        for (j = 0; j < WLS_MAX_NUM_BLOCKS; j++)
        {
            for (i = 0; i < WLS_MAX_PHY_INSTANCES; i++)
            {
                if (pCtx->pWlsBuffer[i][j] == NULL)
                {
                    pCtx->pWlsBuffer[i][j] = (void *)WLS_DequeueBlock((void*)pCtx->hWls);
                    //printf("   %2d %2d: %p\n", i, j, pCtx->pWlsBuffer[i][j]);
                }

                if (pCtx->pWlsBuffer[i][j] == NULL)
                {
                    nAllDone = 0;
                }
                else
                {
                    //nVars[i+1]++;
                }
            }
        }

        if (nFirst == 1)
        {
            for (i = 0; i < WLS_MAX_PHY_INSTANCES; i++)
            {
                pCtx->nPush[i]  = 0;
                pCtx->nPop[i]   = 0;
            }
        }

        if (nAllDone)
            pCtx->nWlsFirst = 0;
        else
            pCtx->nWlsFirst = 2;
    }

    pthread_mutex_unlock(&wlsUlLock);
}

void *wls_pa_to_va(void *data)
{
    PPHY_WLS_LAYER_CTX pCtx = (PPHY_WLS_LAYER_CTX) wls_get_layer_ctx();
    return (void *)WLS_PA2VA((void*)pCtx->hWls, (uint64_t)data);
}

void *wls_va_to_pa(void *data)
{
     PPHY_WLS_LAYER_CTX pCtx = (PPHY_WLS_LAYER_CTX) wls_get_layer_ctx();
     return (void *)WLS_VA2PA((void*)pCtx->hWls, (void*)data);
}

void *wls_alloc_buffer(uint32_t phyInstance, uint32_t size)
{
    uint8_t * ptr = NULL;
    PPHY_WLS_LAYER_CTX pCtx = (PPHY_WLS_LAYER_CTX) wls_get_layer_ctx();
    uint32_t nPop, nPush;

    // For NR, use new lockless scheme with 1 pool per cellId being used
    nPush = pCtx->nPush[phyInstance];
    nPop = pCtx->nPop[phyInstance];

    ptr = (void *) pCtx->pWlsBuffer[phyInstance][nPop];
    pCtx->pWlsBuffer[phyInstance][nPop] = NULL;
    if (ptr == NULL)
    {
        printf("wls_alloc_buffer[phyInstance %d][nPop %d][nPush %d]is NULL\n", phyInstance, nPop, nPush);
        uint32_t i, j;
        for (j = 0; j < WLS_MAX_NUM_BLOCKS; j++)
        {
            printf("   %2d: ", j);
            for (i = 0; i < WLS_MAX_PHY_INSTANCES; i++)
            {
                printf("%p ", pCtx->pWlsBuffer[i][j]);
            }
            printf("\n");
        }

        exit(-1);
    }

    nPop = IncWlsAllocBlock(nPop);
    pCtx->nPop[phyInstance] = nPop;

    print_dbg("wls_alloc_buffer %x\n", ptr);

    return (void *)wls_pa_to_va(ptr);
}

static void *wls_rx_handler(void *pParam)
{
    PPHY_WLS_LAYER_CTX pCtx = wls_get_layer_ctx();
    // PPHYCFG_VARS pPhyCfgVars = phycfg_get_ctx();

    pCtx->f_thread_wls_phy_started = TRUE;

    // If this thread is not to be used in realtime, then just create a dummy thread to get woken up for the non-rt APIs from L2
    // sys_affinity_bind(pPhyCfgVars->systemThread.coreMask);


    while (pCtx->f_runthread_wls_phy)
    {
        // If the WLS Thread option is enabled or if WLS Thread option is disabled and State is non-real time,
        // then call wls_wait to be woken up by Mac

        WLS_Wait(pCtx->hWls);

        if (g_wlsRxProcess != NULL)
            g_wlsRxProcess();
    }

    pCtx->f_thread_wls_phy_started = FALSE;
    usleep(10000000);

    return NULL;
}

static uint32_t wls_start_rx_thread(void)
{
    //uint64_t          tt = MLogPhyTick();
    uint32_t          rc = FAILURE;
    //uint16_t          StartTick;
    //uint16_t          delta = 0;
    PPHY_WLS_LAYER_CTX pCtx = wls_get_layer_ctx();

    print_dbg("Create WLS Receiver thread...");
    pCtx->f_runthread_wls_phy      = TRUE;
    pCtx->f_thread_wls_phy_started = FALSE;

    pCtx->p_pthread_wls_phy = (pthread_t *) malloc(sizeof(pthread_t));

    if (!pCtx->p_pthread_wls_phy)
    {
        print_err("can't allocate space for PHY WLS thread\n");
        WLS_Close(pCtx->hWls);
        return rc;
    }

    pCtx->p_sem_wls_phy = (sem_t*) malloc(sizeof(sem_t));
    if (!pCtx->p_sem_wls_phy)
    {
        print_err("cannot allocate space for PHY semaphore\n");
        WLS_Close(pCtx->hWls);
        free(pCtx->p_pthread_wls_phy);
        return rc;
    }

    memset(pCtx->p_pthread_wls_phy, 0, sizeof(pthread_t));
    memset(pCtx->p_sem_wls_phy, 0, sizeof(sem_t));

    rc = pthread_create(pCtx->p_pthread_wls_phy, NULL, wls_rx_handler, (void *) pCtx);
    // pthread_setname_np(*pCtx->p_pthread_wls_phy, "l1app_wlsnrt");

    if (rc != SUCCESS)
    {
        print_err(" PHY WLS thread wasn't created: rc (0x%X) ", rc);
        WLS_Close(pCtx->hWls);
        free(pCtx->p_pthread_wls_phy);
        free(pCtx->p_sem_wls_phy);
        return rc;
    }

    print_dbg("WLS Receiver thread started");

    return rc;
}

static uint32_t wls_stop_rx_thread(void)
{
    PPHY_WLS_LAYER_CTX pCtx = wls_get_layer_ctx();
    uint32_t rc = SUCCESS;

    print_log("Termination WLS Receiver thread...");
    if(pCtx->p_pthread_wls_phy)
    {
        pCtx->f_runthread_wls_phy = FALSE;
        if(pCtx->hWls)
        {
            WLS_WakeUp(pCtx->hWls);
            while(pCtx->f_thread_wls_phy_started == TRUE)
            {
                usleep(10000);
            }
        }

        sys_pthread_remove(pCtx->p_pthread_wls_phy);
    }

    if(pCtx->p_pthread_wls_phy)
        free(pCtx->p_pthread_wls_phy);

    if(pCtx->p_sem_wls_phy)
        free(pCtx->p_sem_wls_phy);

    pCtx->p_pthread_wls_phy = NULL;
    pCtx->p_sem_wls_phy = NULL;

    return rc;
}

uint32_t wls_layer_init(char * wls_device_name, uint64_t nWlsMemorySize)
{
    uint32_t rc = 0;
    PPHY_WLS_LAYER_CTX pCtx = wls_get_layer_ctx();

    memset(pCtx, 0, sizeof(PHY_WLS_LAYER_CTX));
    printf("wls_layer_init[%s] nWlsMemorySize[%ld]\n", wls_device_name, nWlsMemorySize);

    pCtx->hWls = WLS_Open(wls_device_name, WLS_SLAVE_CLIENT, nWlsMemorySize);
    if (pCtx->hWls)
    {
        /* connect memory preallocated by mac */
        pCtx->wls_mem_base = WLS_Alloc(pCtx->hWls, nWlsMemorySize);
        if (pCtx->wls_mem_base)
        {
            pCtx->wls_mem_size = (uint32_t)nWlsMemorySize;
        }
        else
        {
            printf("WLS_Mmap failed\n");
            return -1;
        }
    }
    else
    {
        printf("WLS_Open failed\n");
        return -1;
    }

    if (rc == 0)
    {
        pCtx->dlZbcEnable = 1;
        pCtx->wlsState = WLS_STATE_NON_RT;
        pCtx->nWlsFirst = 1;

        rc = wls_start_rx_thread();

        if (rc != SUCCESS)
        {
            print_err("    PHY SHM Rx thread wasn't started: rc (0x%X) ", rc);
            return rc;
        }

        return rc;
    }

    return rc;
}

uint32_t wls_layer_close(void)
{
    //uint64_t          tt = MLogPhyTick();
    uint32_t          rc = SUCCESS;
    PPHY_WLS_LAYER_CTX pCtx = wls_get_layer_ctx();
    print_log("WLS interface termination...");

    rc = wls_stop_rx_thread();

    if (rc != SUCCESS)
    {
        print_err(" PHY WLS Rx thread stoped with error: rc (0x%X) ", rc);
    }

    if(pCtx->wls_mem_base)
    {
        print_log("WLS Free ....");
        WLS_Free(pCtx->hWls, (void*)pCtx->wls_mem_base);
    }

    if(pCtx->hWls)
    {
        print_log("WLS Close ....");
        WLS_Close(pCtx->hWls);
        pCtx->hWls = NULL;
    }
    print_log("Done");

    return SUCCESS;
}



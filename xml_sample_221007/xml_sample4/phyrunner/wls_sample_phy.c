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

#include <rte_common.h>
#include <rte_eal.h>
#include <rte_debug.h>

#include "common.h"
#include "gSM_l1_l2_api.h"
#include "wls_lib_phy.h"
#include "aux_wls.h"

#define MAX_MSG_BUFF_SIZE            10000
#define MAX_TX_DATA_COUNT_IN_SLOT    20
#define MAX_SLOT_INFO_COUNT          16
#define UL_IND_MESSAGES_SLOT_OFFSET  3

Fapi_API_Header *g_FapiTxListFirst = NULL;
Fapi_API_Header *g_FapiTxListPrev = NULL;

int g_isCellActive[MAX_CELL_COUNT] = {0};
FapiConfigInfo g_FapiConfigInfo[MAX_CELL_COUNT];
TestConfigReq g_TestConfigReq[MAX_CELL_COUNT];

typedef struct _slotInfo_st
{
    uint8_t  ulTtiReq[MAX_MSG_BUFF_SIZE];
    uint32_t ulTtiReqSize;
} slotInfo_st;

slotInfo_st g_SlotInfo[MAX_CELL_COUNT][MAX_SLOT_INFO_COUNT];
uint32_t    g_CurSlotInfoIndex[MAX_CELL_COUNT] = { 0 };

uint32_t g_DumpCount_DLTTIReq[]  = {1,1,1,1,1,1};
uint32_t g_DumpCount_TxDataReq[] = {1,1,1,1,1,1};
uint32_t g_DumpCount_ULTTIReq[]  = {1,1,1,1,1,1};

int txPhyFapiMsg(Fapi_API_Header *pHeadMsg)
{
    PPHY_WLS_LAYER_CTX pCtx = (PPHY_WLS_LAYER_CTX) wls_get_layer_ctx();
    uint32_t ret = SUCCESS;
    Fapi_API_Header *pCurMsg = NULL;
    uint16_t flags;

    print_dbg("wls_send_list\n");

    pthread_mutex_lock(&wlsSendLock);

    if (pCtx->hWls)
    {
        pCurMsg = pHeadMsg;

        if(pCurMsg->pNext)
        {
            flags = WLS_SG_FIRST;
            while (pCurMsg)
            {
                if (pCurMsg->pNext)
                { 
                    ret = WLS_Put(pCtx->hWls, (uint64_t) wls_va_to_pa(pCurMsg), pCurMsg->msgSize, 0, flags);
                    if (ret != 0)
                    {
                        printf("WLS_Put failed\n");
                        pthread_mutex_unlock(&wlsSendLock);
                        return FAILURE;
                    }

                    pCurMsg = pCurMsg->pNext;
                }
                else
                { 
                    flags = WLS_SG_LAST;
                    ret = WLS_Put(pCtx->hWls, (uint64_t)wls_va_to_pa(pCurMsg), pCurMsg->msgSize, 0, flags );

                    if (ret != 0)
                    {
                        printf("WLS_Put failed\n");
                        pthread_mutex_unlock(&wlsSendLock);
                        return FAILURE;
                    }

                    pCurMsg = NULL;

                }

                flags = WLS_SG_NEXT;
            }
        }
        else 
        {
            ret = WLS_Put(pCtx->hWls, (uint64_t)wls_va_to_pa(pCurMsg), pCurMsg->msgSize, 0, 0);
            if (ret != 0)
            {
                printf("WLS_Put failed\n");
                pthread_mutex_unlock(&wlsSendLock);
                return FAILURE;
            }
        }
    }
    else
    {
        printf("WLS is not ready\n");
    }
    wls_allocate_buffer_to_queue(0);

    pthread_mutex_unlock(&wlsSendLock);

    return ret;
}

void txConfigResp( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;
    uint32_t phyInstance = 0;

    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    CONFIG_RESP *p_ConfigResp;

    pMsgBuff = (uint8_t *) wls_alloc_buffer(phyInstance, MAX_MSG_BUFF_SIZE);
    if (pMsgBuff == NULL)
        return;

    memset( pMsgBuff, 0, MAX_MSG_BUFF_SIZE );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) pMsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    msgSize = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_PHY_CONFIG_RESP;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_ConfigResp = (CONFIG_RESP *) (p_FapiGenericHeader+1);
    p_ConfigResp->errorCode = MSG_OK;
    p_ConfigResp->numInvalidOrUnsupportedTLV = 0;
    p_ConfigResp->numInvalidTLVForIdleOnly = 0;
    p_ConfigResp->numInvalidTLVForRunningOnly = 0;
    p_ConfigResp->numMissingTLV = 0;

    fapiBodyLength += sizeof(CONFIG_RESP);

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    msgSize += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = msgSize;

    printf( "Tx ConfigResp. msgSize:%u\n", msgSize );
    txPhyFapiMsg( (Fapi_API_Header *) pMsgBuff );
}

void txTestConfigResp( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;
    uint32_t phyInstance = 0;

    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;

    pMsgBuff = (uint8_t *) wls_alloc_buffer(phyInstance, MAX_MSG_BUFF_SIZE);
    if (pMsgBuff == NULL)
        return;

    memset( pMsgBuff, 0, MAX_MSG_BUFF_SIZE );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) pMsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    msgSize = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_TEST_CONFIG_RESP;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    msgSize += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = msgSize;

    printf( "Tx TestConfigResp. msgSize:%u\n", msgSize );
    txPhyFapiMsg( (Fapi_API_Header *) pMsgBuff );
}

void txStartResp( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;
    uint32_t phyInstance = 0;

    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    START_RESP *p_StartResp;

    pMsgBuff = (uint8_t *) wls_alloc_buffer(phyInstance, MAX_MSG_BUFF_SIZE);
    if (pMsgBuff == NULL)
        return;

    memset( pMsgBuff, 0, MAX_MSG_BUFF_SIZE );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) pMsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    msgSize = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_PHY_START_RESP;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_StartResp = (START_RESP *) (p_FapiGenericHeader+1);
    p_StartResp->errorCode = MSG_OK;

    fapiBodyLength += sizeof(START_RESP);

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    msgSize += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = msgSize;

    printf( "Tx StartResp. msgSize:%u\n", msgSize );
    txPhyFapiMsg( (Fapi_API_Header *) pMsgBuff );
}

void copyFile( uint8_t *pBasePath, uint8_t *pSrcFilename, uint8_t *pDestFilename )
{
    char fullPathSrc[MAX_TEST_CONFIG_FILENAME_LEN];
    char fullPathDest[MAX_TEST_CONFIG_FILENAME_LEN];
    char fileLineBuff[4096];
    FILE *pFpSrc, *pFpDest;

    sprintf( fullPathSrc, "%s/%s", (char *) pBasePath, (char *) pSrcFilename );
    sprintf( fullPathDest, "%s/%s", (char *) pBasePath, (char *) pDestFilename );

    if ((pFpSrc = fopen(fullPathSrc, "rt")) == NULL)
        return;

    if ((pFpDest = fopen(fullPathDest, "wt")) == NULL)
    {
        fclose( pFpSrc );
        return;
    }

    while (fgets( fileLineBuff, 4096, pFpSrc ) != NULL)
    {
        fputs( fileLineBuff, pFpDest ); 
    }

    fclose( pFpSrc );
    fclose( pFpDest );
}

void handleTestConfig( uint8_t cellIndex )
{
    if (g_TestConfigReq[cellIndex].slotCountToRun == 0)    // exceptional case
        return;

    if (strlen((char *) g_TestConfigReq[cellIndex].filenameDlIqOutput) > 0)
    {
        copyFile( g_TestConfigReq[cellIndex].basePath, g_TestConfigReq[cellIndex].filenameDlIqRef, g_TestConfigReq[cellIndex].filenameDlIqOutput );
    }

    if (strlen((char *) g_TestConfigReq[cellIndex].filenameUlResultOutput) > 0)
    {
        copyFile( g_TestConfigReq[cellIndex].basePath, g_TestConfigReq[cellIndex].filenameUlResultRef, g_TestConfigReq[cellIndex].filenameUlResultOutput );
    }
}

void txStopInd( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;
    uint32_t phyInstance = 0;

    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;

    pMsgBuff = (uint8_t *) wls_alloc_buffer(phyInstance, MAX_MSG_BUFF_SIZE);
    if (pMsgBuff == NULL)
        return;

    memset( pMsgBuff, 0, MAX_MSG_BUFF_SIZE );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) pMsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    msgSize = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_PHY_STOP_IND;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    msgSize += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = msgSize;

    printf( "Tx StopInd. msgSize:%u\n", msgSize );
    txPhyFapiMsg( (Fapi_API_Header *) pMsgBuff );
}

void txSlotInd( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;
    uint32_t phyInstance = 0;
    static int sfn = 0;
    static int slot = 0;

    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    Slot_IND *p_SlotInd;

    pMsgBuff = (uint8_t *) wls_alloc_buffer(phyInstance, MAX_MSG_BUFF_SIZE);
    if (pMsgBuff == NULL)
        return;

    memset( pMsgBuff, 0, MAX_MSG_BUFF_SIZE );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) pMsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    msgSize = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_PHY_SLOT_IND;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_SlotInd = (Slot_IND *) (p_FapiGenericHeader+1);
    p_SlotInd->sfn = sfn;
    p_SlotInd->slot = slot;

    fapiBodyLength += sizeof(Slot_IND);

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    msgSize += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = msgSize;

    printf( "\nTx SlotInd. msgSize:%u Cell %u sfn %4u slot %2u\n", msgSize, cellIndex, sfn, slot );

    slot++;
    if (slot == 20)
    {
        slot = 0;
        sfn++;
        if (sfn == 1024)
            sfn = 0;
    }

    txPhyFapiMsg( (Fapi_API_Header *) pMsgBuff);
}

void txUlIndMessages( uint8_t cellIndex )
{
    uint32_t phyInstance = 0;
    uint32_t slotInfoIndex, pduIndex, pduCount, tbDataIndex;
    Fapi_API_Header *p_FapiApiHeader, *p_FapiApiHeader_CrcInd, *p_FapiApiHeader_RxDataInd;
    Fapi_Generic_Header *p_FapiGenericHeader, *p_FapiGenericHeader_CrcInd, *p_FapiGenericHeader_RxDataInd;
    UL_TTI_REQ *p_UlTtiReq;
    PDU_Struct *p_pduHeader;
    PUSCH_PDU *p_PuschPdu;
    PuschDataInfo *p_PuschDataInfo;
    uint32_t msgSize_CrcInd, msgSize_RxDataInd, fapiBodyLength_CrcInd, fapiBodyLength_RxDataInd;
    uint8_t sendBuff_CrcInd[MAX_MSG_BUFF_SIZE];
    uint8_t sendBuff_RxDataInd[MAX_MSG_BUFF_SIZE];
    CRC_IND *p_CrcInd;
    CRC_IND_PDU *p_CrcIndPdu;
    Rx_Data_IND *p_RxDataInd;
    Rx_Data_IND_PDU *p_RxDataIndPdu;
    uint8_t *pMsgBuff;
    uint64_t *p_Value_U64;
    uint8_t *pRxBuff;

    g_CurSlotInfoIndex[cellIndex]++;
    if (g_CurSlotInfoIndex[cellIndex] >= MAX_SLOT_INFO_COUNT)
        g_CurSlotInfoIndex[cellIndex] = 0;

    if (g_CurSlotInfoIndex[cellIndex] >= UL_IND_MESSAGES_SLOT_OFFSET)
        slotInfoIndex = g_CurSlotInfoIndex[cellIndex]-UL_IND_MESSAGES_SLOT_OFFSET;
    else
        slotInfoIndex = g_CurSlotInfoIndex[cellIndex]+MAX_SLOT_INFO_COUNT-UL_IND_MESSAGES_SLOT_OFFSET;

    if (g_SlotInfo[cellIndex][slotInfoIndex].ulTtiReqSize == 0)
        return;

    p_FapiApiHeader = (Fapi_API_Header *) g_SlotInfo[cellIndex][slotInfoIndex].ulTtiReq;
    if (p_FapiApiHeader->msgSize == 0)
        return;

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);

    if (p_FapiGenericHeader->length == 0)
        return;

    p_UlTtiReq = (UL_TTI_REQ *) (p_FapiGenericHeader+1);

    pduCount = p_UlTtiReq->numPDUs;
    if (pduCount == 0)
        return;

    // Prepare for CrcInd

    memset( sendBuff_CrcInd, 0, sizeof(sendBuff_CrcInd) );
    p_FapiApiHeader_CrcInd = (Fapi_API_Header *) sendBuff_CrcInd;
    p_FapiApiHeader_CrcInd->numOfMessages = 1;
    p_FapiApiHeader_CrcInd->opaqueHandle = 0;
    p_FapiApiHeader_CrcInd->pNext = NULL;
    p_FapiApiHeader_CrcInd->cellIndex = cellIndex;
    p_FapiApiHeader_CrcInd->flags = 0;
    msgSize_CrcInd = sizeof(Fapi_API_Header);

    p_FapiGenericHeader_CrcInd = (Fapi_Generic_Header *) (p_FapiApiHeader_CrcInd+1);
    p_FapiGenericHeader_CrcInd->type = MSG_PHY_CRC_IND;
    p_FapiGenericHeader_CrcInd->length = 0;    // Filled at the end
    msgSize_CrcInd += sizeof(Fapi_Generic_Header);

    p_CrcInd = (CRC_IND *) (p_FapiGenericHeader_CrcInd+1);
    fapiBodyLength_CrcInd = sizeof(CRC_IND);

    p_CrcInd->sfn = p_UlTtiReq->sfn;
    p_CrcInd->slot = p_UlTtiReq->slot;
    p_CrcInd->numCRCs = 0;
    p_CrcIndPdu = (CRC_IND_PDU *) p_CrcInd->variablePart;

    // Prepare for RxDataInd

    memset( sendBuff_RxDataInd, 0, sizeof(sendBuff_RxDataInd) );
    p_FapiApiHeader_RxDataInd = (Fapi_API_Header *) sendBuff_RxDataInd;
    p_FapiApiHeader_RxDataInd->numOfMessages = 1;
    p_FapiApiHeader_RxDataInd->opaqueHandle = 0;
    p_FapiApiHeader_RxDataInd->pNext = NULL;
    p_FapiApiHeader_RxDataInd->cellIndex = cellIndex;
    p_FapiApiHeader_RxDataInd->flags = 0;
    msgSize_RxDataInd = sizeof(Fapi_API_Header);

    p_FapiGenericHeader_RxDataInd = (Fapi_Generic_Header *) (p_FapiApiHeader_RxDataInd+1);
    p_FapiGenericHeader_RxDataInd->type = MSG_PHY_RX_DATA_IND;
    p_FapiGenericHeader_RxDataInd->length = 0;    // Filled at the end
    msgSize_RxDataInd += sizeof(Fapi_Generic_Header);

    p_RxDataInd = (Rx_Data_IND *) (p_FapiGenericHeader_RxDataInd+1);
    fapiBodyLength_RxDataInd = sizeof(Rx_Data_IND);

    p_RxDataInd->sfn = p_UlTtiReq->sfn;
    p_RxDataInd->slot = p_UlTtiReq->slot;
    p_RxDataInd->numPDUs = 0;
    p_RxDataIndPdu = (Rx_Data_IND_PDU *) p_RxDataInd->variablePart;

    // Handle prev UlTtiReq

    p_pduHeader = (PDU_Struct *) p_UlTtiReq->variablePart;
    for (pduIndex = 0; pduIndex < pduCount; pduIndex++)
    {
        if (((uint8_t *)p_pduHeader - (uint8_t *)p_UlTtiReq)+p_pduHeader->pduSize > p_FapiGenericHeader->length)
            break;

        switch (p_pduHeader->pduType)
        {
            case UL_PDU_TYPE_PUSCH :
                p_PuschPdu = (PUSCH_PDU *)p_pduHeader;
                if (!(p_PuschPdu->pduBitmap & BITMASK_PUSCH_PDU_BITMAP_DATA_INCLUDED))
                    break;
                p_PuschDataInfo = (PuschDataInfo *) p_PuschPdu->variablePart;
                if (p_PuschDataInfo->tbSize == 0)
                    break;

                pRxBuff = (uint8_t *) wls_alloc_buffer(phyInstance, p_PuschDataInfo->tbSize);
                if (pRxBuff == NULL)
                    break;

                for (tbDataIndex = 0; tbDataIndex < p_PuschDataInfo->tbSize; tbDataIndex++)
                    pRxBuff[tbDataIndex] = (uint8_t) tbDataIndex;

                // Add CrcInd info
                p_CrcInd->numCRCs++;
                p_CrcIndPdu->handle = p_PuschPdu->handle;
                p_CrcIndPdu->rnti = p_PuschPdu->rnti;
                p_CrcIndPdu->harqID = p_PuschDataInfo->harqProcessID;
                p_CrcIndPdu->tbCrcStatus = CRC_STATUS_PASS;
                fapiBodyLength_CrcInd += sizeof(CRC_IND_PDU);
                p_CrcIndPdu++;

                // Add RxDataInd info
                p_RxDataInd->numPDUs++;
                p_RxDataIndPdu->handle = p_PuschPdu->handle;
                p_RxDataIndPdu->rnti = p_PuschPdu->rnti;
                p_RxDataIndPdu->harqID = p_PuschDataInfo->harqProcessID;
                p_RxDataIndPdu->pduLength = p_PuschDataInfo->tbSize;
                p_RxDataIndPdu->pduTag = RX_DATA_VALUE_FIELD_POINTER_TO_PAYLOAD;    // custom definition
                p_Value_U64 = (uint64_t *) p_RxDataIndPdu->variablePart;
                *p_Value_U64 = (uint64_t) pRxBuff;
                fapiBodyLength_RxDataInd += sizeof(Rx_Data_IND_PDU)+sizeof(uint64_t);
                p_RxDataIndPdu = (Rx_Data_IND_PDU *) ((uint8_t *)(p_RxDataIndPdu+1) + sizeof(uint64_t));

                break;

            default :
                break;
        }

        p_pduHeader = (PDU_Struct *) ((uint8_t *)p_pduHeader+p_pduHeader->pduSize);
    }

    // Finalize CrcInd message
    p_FapiGenericHeader_CrcInd->length = fapiBodyLength_CrcInd;
    msgSize_CrcInd += fapiBodyLength_CrcInd;
    p_FapiApiHeader_CrcInd->msgSize = msgSize_CrcInd;

    // Finalize RxDataInd message
    p_FapiGenericHeader_RxDataInd->length = fapiBodyLength_RxDataInd;
    msgSize_RxDataInd += fapiBodyLength_RxDataInd;
    p_FapiApiHeader_RxDataInd->msgSize = msgSize_RxDataInd;

    // Tx CrcInd message
    if (p_RxDataInd->numPDUs > 0)
    {
        pMsgBuff = (uint8_t *) wls_alloc_buffer(phyInstance, MAX_MSG_BUFF_SIZE);
        if (pMsgBuff == NULL)
            return;

        memcpy( pMsgBuff, p_FapiApiHeader_CrcInd, msgSize_CrcInd );
        txPhyFapiMsg( (Fapi_API_Header *) pMsgBuff);
    }

    // Tx RxDataInd message
    if (p_RxDataInd->numPDUs > 0)
    {
        pMsgBuff = (uint8_t *) wls_alloc_buffer(phyInstance, MAX_MSG_BUFF_SIZE);
        if (pMsgBuff == NULL)
            return;

        memcpy( pMsgBuff, p_FapiApiHeader_RxDataInd, msgSize_RxDataInd );
        txPhyFapiMsg( (Fapi_API_Header *) pMsgBuff);
    }
}

uint32_t fapiHandleRxMsg(Fapi_API_Header *pMsg, uint32_t msgSize)
{
    uint32_t fapiMsgCount, fapiMsgIndex, msgOffset;
    uint8_t cellIndex;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    DL_TTI_REQ *p_DlTtiReq;
    Tx_DATA_REQ *p_TxDataReq;
    UL_DCI_REQ *p_ULDciReq;
    UL_TTI_REQ *p_UlTtiReq;

    p_FapiApiHeader = (Fapi_API_Header *) pMsg;
    fapiMsgCount = p_FapiApiHeader->numOfMessages;
    cellIndex = p_FapiApiHeader->cellIndex;
    msgOffset = sizeof(Fapi_API_Header);

    if (msgOffset > msgSize)
    {
        printf( "  Size error : invalid Fapi_API_Header\n" );
        return 0;
    }

    for (fapiMsgIndex = 0; fapiMsgIndex < fapiMsgCount; fapiMsgIndex++)
    {
        p_FapiGenericHeader = (Fapi_Generic_Header *) (((uint8_t *)pMsg)+msgOffset);

        if (msgOffset+sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length > msgSize)
        {
            printf( "  Size error : invalid Fapi_Generic_Header\n" );
            return 0;
        }

        switch (p_FapiGenericHeader->type)
        {
            case MSG_PHY_CONFIG_REQ :
                printf( "MSG_PHY_CONFIG_REQ received\n" );
                phy_ParseMsg_ConfigReq( p_FapiGenericHeader, &g_FapiConfigInfo[cellIndex] );
                txConfigResp(cellIndex);
                break;

            case MSG_TEST_CONFIG_REQ :
                printf( "MSG_TEST_CONFIG_REQ received\n" );
                phy_ParseMsg_TestConfigReq( p_FapiGenericHeader, &g_TestConfigReq[cellIndex] );
                txTestConfigResp(cellIndex);
                break;

            case MSG_PHY_START_REQ :
                printf( "MSG_PHY_START_REQ received\n" );
                txStartResp(cellIndex);
                g_isCellActive[cellIndex] = TRUE;
                break;

            case MSG_PHY_DL_TTI_REQ :
                p_DlTtiReq = (DL_TTI_REQ *) (p_FapiGenericHeader+1);
             
                printf( "  MSG_PHY_DL_TTI_REQ  received [sfn %u slot %u]\n", p_DlTtiReq->sfn, p_DlTtiReq->slot );
                if (g_DumpCount_DLTTIReq[cellIndex] > 0 && p_FapiGenericHeader->length > sizeof(DL_TTI_REQ))
                {
                    phy_ParseMsg_DLTTIReq(p_FapiGenericHeader);
                    g_DumpCount_DLTTIReq[cellIndex]--;
                }
                break;

            case MSG_PHY_TX_DATA_REQ :
                p_TxDataReq = (Tx_DATA_REQ *) (p_FapiGenericHeader+1);

                printf( "  MSG_PHY_TX_DATA_REQ received [sfn %u slot %u]\n", p_TxDataReq->sfn, p_TxDataReq->slot );
                if (g_DumpCount_TxDataReq[cellIndex] > 0 && p_FapiGenericHeader->length > sizeof(Tx_DATA_REQ))
                {
                    phy_ParseMsg_TxDataReq(p_FapiGenericHeader);
                    g_DumpCount_TxDataReq[cellIndex]--;
                }
                break;

            case MSG_PHY_UL_DCI_REQ :
                p_ULDciReq = (UL_DCI_REQ *) (p_FapiGenericHeader+1);

                printf( "  MSG_PHY_UL_DCI_REQ  received [sfn %u slot %u]\n", p_ULDciReq->sfn, p_ULDciReq->slot );
                break;

            case MSG_PHY_UL_TTI_REQ :
                p_UlTtiReq = (UL_TTI_REQ *) (p_FapiGenericHeader+1);

                printf( "  MSG_PHY_UL_TTI_REQ  received [sfn %u slot %u]\n", p_UlTtiReq->sfn, p_UlTtiReq->slot );
                if (g_DumpCount_ULTTIReq[cellIndex] > 0 && p_FapiGenericHeader->length > sizeof(UL_TTI_REQ))
                {
                    phy_ParseMsg_ULTTIReq(p_FapiGenericHeader);
                    g_DumpCount_ULTTIReq[cellIndex]--;
                }

                // Keep ULConfig in SlotInfo to send Ind msgs
                memcpy( g_SlotInfo[cellIndex][g_CurSlotInfoIndex[cellIndex]].ulTtiReq, pMsg, msgSize );
                g_SlotInfo[cellIndex][g_CurSlotInfoIndex[cellIndex]].ulTtiReqSize = msgSize;
                break;

            case MSG_PHY_STOP_REQ :
                g_isCellActive[cellIndex] = FALSE;
                printf( "MSG_PHY_STOP_REQ received\n" );
                handleTestConfig(cellIndex);
                txStopInd(cellIndex);
                break;

            default :
                break;
        }

        msgOffset += sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length;
    }

    return 0;
}

void wls_rx_process(void)
{
    PPHY_WLS_LAYER_CTX pCtx = wls_get_layer_ctx();

    uint64_t *pPaMsg;
    uint32_t msgSize;
    uint16_t msgType;
    uint16_t flags = 0;
    uint32_t enter = 0;
    Fapi_API_Header *pMsg = NULL;
    Fapi_API_Header *pFirstMsg = NULL;

    while(enter == 0)
    {
        pPaMsg = (uint64_t*)WLS_Get(pCtx->hWls, &msgSize, &msgType, &flags);
        if (pPaMsg)
        {
            if ((flags & WLS_TF_FIN) || (flags == 0))
            {
                enter = 1;
            }

            pMsg = (Fapi_API_Header *) WLS_PA2VA(pCtx->hWls, (uint64_t)pPaMsg);

            if (msgType == MSG_PHY_TX_DATA_BLOCK0 || msgType == MSG_PHY_TX_DATA_BLOCK1)
                continue;

            if (g_FapiTxListFirst == NULL)
            {
                g_FapiTxListFirst = pMsg;
            }

            if(g_FapiTxListPrev)
                g_FapiTxListPrev->pNext = pMsg;

            g_FapiTxListPrev = pMsg;
        }
        else
        {
            break;
        }
    }

    if (enter)
    {
        pFirstMsg = g_FapiTxListFirst;

        if (g_FapiTxListPrev)
            g_FapiTxListPrev->pNext = NULL;

        g_FapiTxListFirst = NULL;
        g_FapiTxListPrev = NULL;

        if (pCtx->nWlsFirst)
        {
            wls_allocate_buffer_to_queue(pCtx->nWlsFirst);
        }

        while (pFirstMsg != NULL)
        {
            fapiHandleRxMsg(pFirstMsg, pFirstMsg->msgSize);
            pFirstMsg = pFirstMsg->pNext;
        }

        wls_allocate_buffer_to_queue(0);
    }
}

int main( void )
{
    uint8_t i, cellIndex;
    char *my_argv[] = { "l1", "-c 4000000", "-n2", "--file-prefix=wls", "--socket-mem=6144", "-w 0000:20:00.0", "--iova-mode=pa" };
    
    printf("init wls ");
    for (i = 0; i < RTE_DIM(my_argv); i++)
    {
        printf("%s ", my_argv[i]);
    }
    printf("\n");

    if (rte_eal_init(RTE_DIM(my_argv), my_argv) < 0)
        rte_panic("Cannot init EAL\n");

    setWlsRxProcess( wls_rx_process );

    if (wls_layer_init("wls", 1063256064) != 0)
    {
        printf("wls_layer_init failed\n" );
        return -1;
    }

    printf( "init done\n" );

    memset( &g_FapiConfigInfo, 0, sizeof(g_FapiConfigInfo) );
    while (1)
    {
        usleep(10000);

        for (cellIndex = 0; cellIndex < MAX_CELL_COUNT; cellIndex++)
        {
            if (!g_isCellActive[cellIndex])
                continue;

            txSlotInd(cellIndex);
            txUlIndMessages(cellIndex);
        }
    }

    wls_layer_close();

    return 0;
}


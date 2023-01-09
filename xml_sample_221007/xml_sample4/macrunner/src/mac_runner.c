#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#include <rte_common.h>
#include <rte_eal.h>
#include <rte_debug.h>

#include "common.h"
#include "wls_lib_mac.h"

#include "gSM_l1_l2_api.h"
#include "mac_runner.h"
#include "main.h"

#define MAX_MSG_BUFF_SIZE            10000
#define MAX_CELL_COUNT               6
#define MAX_FAPI_FREE_LIST_COUNT     10

Fapi_API_Header *g_FapiTxListHead[MAX_CELL_COUNT] = {NULL};
Fapi_API_Header *g_FapiTxListTail[MAX_CELL_COUNT] = {NULL};

uint32_t g_FapiFreeListCnt[MAX_CELL_COUNT][MAX_FAPI_FREE_LIST_COUNT];
Fapi_API_Header *g_FapiFreeList[MAX_CELL_COUNT][MAX_FAPI_FREE_LIST_COUNT][TOTAL_FREE_BLOCKS] = {{{0L}}};

///////////////////////////////////////////////////////////////////////////////////////////////////

int initWls( void )
{
    uint8_t i;
    char *my_argv[] = { "gnb", "-c3", "--proc-type=auto", "--file-prefix", WLS_DEVICE_NAME, "--iova-mode=pa" };

    printf("init wls ");
    for (i = 0; i < RTE_DIM(my_argv); i++)
    {
        printf("%s ", my_argv[i]);
    }
    printf("\n");

    if (rte_eal_init(RTE_DIM(my_argv), my_argv) < 0)
        rte_panic("Cannot init EAL\n");

    if (wls_mac_init(WLS_DEVICE_NAME, WLS_MEMORY_SIZE) != 0)
    {
        printf("wls_mac_init failed\n");
        return FAILURE;
    }

    printf("wls_mac_init success\n");

    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void fapiAddToTxList(Fapi_API_Header *pMsg, uint8_t cellIndex)
{
    if (g_FapiTxListHead[cellIndex] == NULL)
    {
        g_FapiTxListHead[cellIndex] = pMsg;
        g_FapiTxListTail[cellIndex] = pMsg;
    }
    else
    {
        g_FapiTxListTail[cellIndex]->pNext = pMsg;
        g_FapiTxListTail[cellIndex] = pMsg;
    }
}

int fapiAddMsgToFapiFreeList(Fapi_API_Header *pMsg, uint32_t listIndex, uint32_t cellIndex)
{
    int listCount;

    listCount = g_FapiFreeListCnt[cellIndex][listIndex];

    while (pMsg)
    {
        if (listCount < TOTAL_FREE_BLOCKS-1)
        {
            g_FapiFreeList[cellIndex][listIndex][listCount] = pMsg;
        }
        else
        {
            printf("fapiAddMsgToFapiFreeList: queue full\n" );
            return listCount;
        }

        listCount++;

        pMsg = pMsg->pNext;
    }

    g_FapiFreeList[cellIndex][listIndex][listCount] = NULL;
    g_FapiFreeListCnt[cellIndex][listIndex] = listCount;

    return listCount;
}

int fapiAddTxDataBuffListToFapiFreeList(uint64_t *p_txDataBuffList, uint32_t txDataCount, uint32_t listIndex, uint32_t cellIndex)
{
    uint32_t listCount, txDataIndex;
    
    listCount = g_FapiFreeListCnt[cellIndex][listIndex];

    for (txDataIndex = 0; txDataIndex < txDataCount; txDataIndex++)
    {
        if (listCount < TOTAL_FREE_BLOCKS-1)
        {
            g_FapiFreeList[cellIndex][listIndex][listCount] = (Fapi_API_Header *) p_txDataBuffList[txDataIndex];
        }
        else
        {
            printf("fapiAddTxDataBuffListToFapiFreeList : queue full\n" );
            return listCount;
        }
        listCount++;
    }

    g_FapiFreeList[cellIndex][listIndex][listCount] = NULL;
    g_FapiFreeListCnt[cellIndex][listIndex] = listCount;

    return listCount;
}

int fapiFreeMsgsInFapiFreeList(uint32_t listIndex, uint32_t cellIndex)
{
    int msgIndex, msgCount;

    if(listIndex >= MAX_FAPI_FREE_LIST_COUNT)
    {
        printf("fapiFreeMsgsInFapiFreeList : invalid listIndex %u\n", listIndex);
        return FAILURE;
    }

    msgCount = g_FapiFreeListCnt[cellIndex][listIndex];
    for (msgIndex = 0; msgIndex < msgCount; msgIndex++)
    {
        if (g_FapiFreeList[cellIndex][listIndex][msgIndex] != NULL)
        {
            wls_mac_free_buffer(g_FapiFreeList[cellIndex][listIndex][msgIndex], MIN_DL_BUF_LOCATIONS);
            g_FapiFreeList[cellIndex][listIndex][msgIndex] = NULL;
        }
    }

    g_FapiFreeListCnt[cellIndex][listIndex] = 0;

    return SUCCESS;
}

int fapiCheckAndGetTxDataList( Fapi_API_Header *pMsg, uint32_t *p_txDataSizeList, uint64_t *p_txDataList, uint32_t *p_txDataCodewordIndexList, uint32_t *p_txDataCount )
{
    uint32_t fapiMsgCount, fapiMsgIndex, msgOffset, txDataIndex;
    uint32_t pduCount, pduIndex, pduLength, tlvCount, tlvIndex, valueLength;
    Fapi_Generic_Header *p_FapiGenericHeader;
    Tx_DATA_REQ *p_TxDataReq;
    TxDataPduInfo *p_TxDataPduInfo;
    TBS_TLV_Struct *p_TBS_TLV_Struct;
    uint64_t *p_Value_U64;

    txDataIndex = 0;

    fapiMsgCount = pMsg->numOfMessages;
    msgOffset = sizeof(Fapi_API_Header);

    for (fapiMsgIndex = 0; fapiMsgIndex < fapiMsgCount; fapiMsgIndex++)
    {
        p_FapiGenericHeader = (Fapi_Generic_Header *) (((uint8_t *)pMsg)+msgOffset);

        if (p_FapiGenericHeader->type == MSG_PHY_TX_DATA_REQ)
        {
            p_TxDataReq = (Tx_DATA_REQ *) (p_FapiGenericHeader+1);

            pduCount = p_TxDataReq->numPDUs;
            p_TxDataPduInfo = (TxDataPduInfo *) p_TxDataReq->variablePart;
            for (pduIndex = 0; pduIndex < pduCount; pduIndex++)
            {
                pduLength = p_TxDataPduInfo->pduLength;
                tlvCount = p_TxDataPduInfo->numTLV;
                p_TBS_TLV_Struct = (TBS_TLV_Struct *) p_TxDataPduInfo->variablePart;
                for (tlvIndex = 0; tlvIndex < tlvCount; tlvIndex++)
                {
                    if (p_TBS_TLV_Struct->tag == VALUE_FIELD_PAYLOAD_VALUES)
                    {
                        valueLength = p_TBS_TLV_Struct->length;
                        if (valueLength%4 != 0)
                            valueLength += 4-(valueLength%4);
                    }
                    else    // VALUE_FIELD_POINTER_TO_PAYLOAD
                    {
                        p_Value_U64 = (uint64_t *) p_TBS_TLV_Struct->variablePart;
                        p_txDataSizeList[txDataIndex] = p_TBS_TLV_Struct->length;
                        p_txDataList[txDataIndex] = *p_Value_U64;
                        p_txDataCodewordIndexList[txDataIndex] = p_TxDataPduInfo->cwIndex;

                        txDataIndex++;
                        valueLength = sizeof(uint64_t);
                    }

                    p_TBS_TLV_Struct = (TBS_TLV_Struct *) ((uint8_t *)(p_TBS_TLV_Struct+1) + valueLength);
                }

                p_TxDataPduInfo = (TxDataPduInfo *) ((uint8_t *)p_TxDataPduInfo + pduLength);
            }
        }
       
        msgOffset += sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length;
    }

    *p_txDataCount = txDataIndex;

    return SUCCESS;
}

int fapiSendTxDataList( uint32_t *p_txDataSizeList, uint64_t *p_txDataBuffList, uint32_t *p_txDataCodewordIndexList, uint32_t txDataCount, uint16_t nFlags )
{
    uint32_t txDataIndex, msgType, flagValue;

    for (txDataIndex = 0; txDataIndex < txDataCount; txDataIndex++)
    {
        msgType = (p_txDataCodewordIndexList[txDataIndex] == 0) ? MSG_PHY_TX_DATA_BLOCK0 : MSG_PHY_TX_DATA_BLOCK1;
        flagValue = (nFlags == WLS_SG_LAST && txDataIndex == txDataCount-1) ? WLS_SG_LAST : WLS_SG_NEXT;

        if (wls_mac_put(wls_mac_va_to_pa((uint8_t *)p_txDataBuffList[txDataIndex]), p_txDataSizeList[txDataIndex], msgType, flagValue) != 0)
        {
            printf("wls_mac_put Error in fapiSendTxDataList\n" );
            return FAILURE;
        }
    }

    return SUCCESS;
}

int fapiSendTxList(uint8_t cellIndex)
{
    Fapi_API_Header *pHeadMsg;
    uint32_t  ret = SUCCESS;
    PWLS_MAC_CTX pWls =  wls_mac_get_ctx();
    Fapi_API_Header *pCurMsg = NULL;
    uint16_t nFlags = 0;
    int count = 0;
    uint32_t txDataSizeList[MAX_TX_DATA_COUNT_IN_SLOT];
    uint64_t txDataBuffList[MAX_TX_DATA_COUNT_IN_SLOT];
    uint32_t txDataCodewordIndexList[MAX_TX_DATA_COUNT_IN_SLOT];
    uint32_t txDataCount;

    static uint32_t idx[MAX_CELL_COUNT] = {0,0,0,0};

    if (g_FapiTxListHead[cellIndex] == NULL)
        return FAILURE;
    
    pHeadMsg = g_FapiTxListHead[cellIndex];

    g_FapiTxListHead[cellIndex] = NULL;
    g_FapiTxListHead[cellIndex] = NULL;

    pthread_mutex_lock((pthread_mutex_t *)&pWls->lock);

    if (gwls_mac_ready)
    {
        pCurMsg = pHeadMsg;
        fapiAddMsgToFapiFreeList(pCurMsg, idx[cellIndex], cellIndex);
        count++;

        if(pCurMsg->pNext)
        {
            nFlags = WLS_SG_FIRST;
            while (pCurMsg)
            {
                count++;

                fapiCheckAndGetTxDataList( pCurMsg, txDataSizeList, txDataBuffList, txDataCodewordIndexList, &txDataCount );
                fapiAddTxDataBuffListToFapiFreeList( txDataBuffList, txDataCount, idx[cellIndex], cellIndex );

                if (pCurMsg->pNext)
                { 
                    ret = wls_mac_put(wls_mac_va_to_pa(pCurMsg), pCurMsg->msgSize, 0, nFlags );
                    if (ret != 0)
                    {
                        printf("wls_mac_put failed\n");
                        pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock);
                        return FAILURE;
                    }

                    if (fapiSendTxDataList( txDataSizeList, txDataBuffList, txDataCodewordIndexList, txDataCount, nFlags ) != SUCCESS)
                    { 
                        printf("fapiSendTxDataList failed\n");
                        pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock);
                        return FAILURE;
                    }
                }
                else if (txDataCount > 0)
                {
                    nFlags = WLS_SG_NEXT;

                    ret = wls_mac_put(wls_mac_va_to_pa(pCurMsg), pCurMsg->msgSize, 0, nFlags);
                    if (ret != 0)
                    {
                        printf("wls_mac_put failed. txDataCount %d\n", txDataCount);
                        pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock);
                        return FAILURE;
                    }

                    nFlags = WLS_SG_LAST;

                    if (fapiSendTxDataList( txDataSizeList, txDataBuffList, txDataCodewordIndexList, txDataCount, nFlags ) != SUCCESS)
                    { 
                        printf("fapiSendTxDataList failed\n");
                        pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock);
                        return FAILURE;
                    }
                }
                else
                { 
                    nFlags = WLS_SG_LAST;

                    ret = wls_mac_put(wls_mac_va_to_pa(pCurMsg), pCurMsg->msgSize, 0, nFlags);
                    if (ret != 0)
                    {
                        printf("wls_mac_put failed. txDataCount %d\n", txDataCount);
                        pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock);
                        return FAILURE;
                    }
                } 

                pCurMsg = pCurMsg->pNext;
                nFlags = WLS_SG_NEXT;
            }
        }
        else // one block
        {
            ret = wls_mac_put(wls_mac_va_to_pa(pCurMsg), pCurMsg->msgSize, 0, 0);
            if (ret != 0)
            {
                printf("Error\n");
                pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock);
                return FAILURE;
            }
        }

        if (count > 1)
        {
            idx[cellIndex]++;
            if(idx[cellIndex] >= MAX_FAPI_FREE_LIST_COUNT)
                idx[cellIndex] = 0;

            fapiFreeMsgsInFapiFreeList(idx[cellIndex], cellIndex);
        }
    }
    else
    {
        printf("WLS is not ready. Cannot tx msg\n");
    }

    pthread_mutex_unlock((pthread_mutex_t *)&pWls->lock);
    return ret;
}

int txFapiMsg( uint8_t *pMsg, uint32_t msgSize, uint8_t isFinal, uint8_t cellIndex )
{
    if (pMsg == NULL || msgSize <= sizeof(Fapi_API_Header))
        return FAILURE;

    fapiAddToTxList((Fapi_API_Header *) pMsg, cellIndex);

    if (isFinal)
        fapiSendTxList(cellIndex);

    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void txPhyConfigReq( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;

    pMsgBuff = (uint8_t *) wls_mac_alloc_buffer(MAX_MSG_BUFF_SIZE, MIN_DL_BUF_LOCATIONS);
    mac_CreateConfigReq( pMsgBuff, MAX_MSG_BUFF_SIZE, &msgSize, &g_macRunnerConfig.cellConfig[cellIndex].fapiConfigInfo, cellIndex );

    txFapiMsg( pMsgBuff, msgSize, TRUE, cellIndex );
    printf( "<<< Tx ConfigReq for Cell %u\n", cellIndex );
}

void txTestConfigReq( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;

    pMsgBuff = (uint8_t *) wls_mac_alloc_buffer(MAX_MSG_BUFF_SIZE, MIN_DL_BUF_LOCATIONS);
    mac_CreateTestConfigReq( pMsgBuff, MAX_MSG_BUFF_SIZE, &msgSize, &g_macRunnerConfig.cellConfig[cellIndex].testConfigReq, cellIndex );

    txFapiMsg( pMsgBuff, msgSize, TRUE, cellIndex );
    printf( "<<< Tx TestConfigReq for Cell %u\n", cellIndex );
}

void txPhyStartReq( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;

    pMsgBuff = (uint8_t *) wls_mac_alloc_buffer(MAX_MSG_BUFF_SIZE, MIN_DL_BUF_LOCATIONS);
    mac_CreateStartReq( pMsgBuff, MAX_MSG_BUFF_SIZE, &msgSize, cellIndex );

    txFapiMsg( pMsgBuff, msgSize, TRUE, cellIndex );
    printf( "<<< Tx StartReq for Cell %u\n", cellIndex );
}

void txPhyStopReq( uint8_t cellIndex )
{
    uint8_t *pMsgBuff;
    uint32_t msgSize;

    pMsgBuff = (uint8_t *) wls_mac_alloc_buffer(MAX_MSG_BUFF_SIZE, MIN_DL_BUF_LOCATIONS);
    mac_CreateStopReq( pMsgBuff, MAX_MSG_BUFF_SIZE, &msgSize, cellIndex );

    txFapiMsg( pMsgBuff, msgSize, TRUE, cellIndex );
    printf( "<<< Tx StopReq for Cell %u\n", cellIndex );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void slotIndHandler( uint16_t sfn, uint16_t slot, uint8_t cellIndex )
{
    uint32_t scs, slotIndex, fapiMsgIndex, fapiMsgCount, msgSize, pduCount, pduLength, pduIndex, tlvCount, tlvIndex, valueLength, txDataIndex;
    uint8_t *pMsgBuff, *pTxDataBuff;
    slot_config_t *pSlotConfig;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    DL_TTI_REQ *p_DlTtiReq;
    Tx_DATA_REQ *p_TxDataReq;
    TxDataPduInfo *p_TxDataPduInfo;
    TBS_TLV_Struct *p_TBS_TLV_Struct;
    UL_DCI_REQ *p_ULDciReq;
    UL_TTI_REQ *p_UlTtiReq;
    uint64_t *p_Value_U64;
  
    scs = g_macRunnerConfig.cellConfig[cellIndex].fapiConfigInfo.phyConfig.slotTimeDomainInfo.subcarrierSpacing;
    slotIndex = sfn*(10 << scs)+slot;
    pSlotConfig = &(g_macRunnerConfig.cellConfig[cellIndex].slotConfig[slotIndex%g_macRunnerConfig.cellConfig[cellIndex].slotConfigCount]);

    fapiMsgCount = pSlotConfig->fapiMsgCount;
    if (fapiMsgCount == 0)
        return;

    for (fapiMsgIndex = 0; fapiMsgIndex < fapiMsgCount; fapiMsgIndex++)
    {
        msgSize = pSlotConfig->fapiMsgSize[fapiMsgIndex];
        if (msgSize < sizeof(Fapi_API_Header))
            continue;
    
        pMsgBuff = (uint8_t *) wls_mac_alloc_buffer(MAX_MSG_BUFF_SIZE, MIN_DL_BUF_LOCATIONS);
        memcpy( pMsgBuff, pSlotConfig->fapiMsg[fapiMsgIndex], msgSize );

        p_FapiApiHeader = (Fapi_API_Header *) pMsgBuff;
        p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);

        switch (p_FapiGenericHeader->type)
        {
            case MSG_PHY_DL_TTI_REQ :
                p_DlTtiReq = (DL_TTI_REQ *) (p_FapiGenericHeader+1);
                p_DlTtiReq->sfn = sfn;
                p_DlTtiReq->slot = slot;

                printf( "  <<< Tx MSG_PHY_DL_TTI_REQ  [sfn %4u slot %2u]\n", sfn, slot );
                break;

            case MSG_PHY_TX_DATA_REQ :
                p_TxDataReq = (Tx_DATA_REQ *) (p_FapiGenericHeader+1);
                p_TxDataReq->sfn = sfn;
                p_TxDataReq->slot = slot;

                pduCount = p_TxDataReq->numPDUs;
                p_TxDataPduInfo = (TxDataPduInfo *) p_TxDataReq->variablePart;
                for (pduIndex = 0; pduIndex < pduCount; pduIndex++)
                {
                    pduLength = p_TxDataPduInfo->pduLength;
                    tlvCount = p_TxDataPduInfo->numTLV;
                    p_TBS_TLV_Struct = (TBS_TLV_Struct *) p_TxDataPduInfo->variablePart;
                    for (tlvIndex = 0; tlvIndex < tlvCount; tlvIndex++)
                    {
                        if (p_TBS_TLV_Struct->tag == VALUE_FIELD_PAYLOAD_VALUES)
                        {
                            valueLength = p_TBS_TLV_Struct->length;
                            if (valueLength%4 != 0)
                                valueLength += 4-(valueLength%4);
                        }
                        else    // VALUE_FIELD_POINTER_TO_PAYLOAD
                        {
                            p_Value_U64 = (uint64_t *) p_TBS_TLV_Struct->variablePart;
                            txDataIndex = (uint32_t) *p_Value_U64;
                            valueLength = sizeof(uint64_t);

                            pTxDataBuff = (uint8_t *) wls_mac_alloc_buffer(MAX_FAPI_TB_SIZE_IN_SLOT, MIN_DL_BUF_LOCATIONS);
                            memcpy( pTxDataBuff, pSlotConfig->txDataBuff[txDataIndex], pSlotConfig->txDataSize[txDataIndex] );
                            *p_Value_U64 = (uint64_t) pTxDataBuff;
                        }

                        p_TBS_TLV_Struct = (TBS_TLV_Struct *) ((uint8_t *)(p_TBS_TLV_Struct+1) + valueLength);
                    }

                    p_TxDataPduInfo = (TxDataPduInfo *) ((uint8_t *)p_TxDataPduInfo + pduLength);
                }

                printf( "  <<< Tx MSG_PHY_TX_DATA_REQ [sfn %4u slot %2u]\n", sfn, slot );
                break;

            case MSG_PHY_UL_DCI_REQ :
                p_ULDciReq = (UL_DCI_REQ *) (p_FapiGenericHeader+1);
                p_ULDciReq->sfn = sfn;
                p_ULDciReq->slot = slot;

                printf( "  <<< Tx MSG_PHY_UL_DCI_REQ  [sfn %4u slot %2u]\n", sfn, slot );
                break;

            case MSG_PHY_UL_TTI_REQ :
                p_UlTtiReq = (UL_TTI_REQ *) (p_FapiGenericHeader+1);
                p_UlTtiReq->sfn = sfn;
                p_UlTtiReq->slot = slot;

                printf( "  <<< Tx MSG_PHY_UL_TTI_REQ  [sfn %4u slot %2u]\n", sfn, slot );
                break;

            default :
                break;
        }

        txFapiMsg( pMsgBuff, msgSize, ((fapiMsgIndex == fapiMsgCount-1) ? TRUE : FALSE), cellIndex );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void handleTestConfig( uint8_t cellIndex )
{
    TestConfigReq *pTestConfigReq;
    char fullPathRef[MAX_TEST_CONFIG_FILENAME_LEN];
    char fullPathOutput[MAX_TEST_CONFIG_FILENAME_LEN];

    pTestConfigReq = &(g_macRunnerConfig.cellConfig[cellIndex].testConfigReq);

    if (pTestConfigReq->slotCountToRun == 0)    // exceptional case
        return;

    if (strlen((char *) pTestConfigReq->filenameDlIqOutput) > 0)
    {
        sprintf( fullPathRef, "%s/%s", pTestConfigReq->basePath, pTestConfigReq->filenameDlIqRef );
        sprintf( fullPathOutput, "%s/%s", pTestConfigReq->basePath, pTestConfigReq->filenameDlIqOutput );

        printf( "--------- TODO : compare ref file %s with output file %s\n", fullPathRef, fullPathOutput );
    }

    if (strlen((char *) pTestConfigReq->filenameUlResultOutput) > 0)
    {
        sprintf( fullPathRef, "%s/%s", pTestConfigReq->basePath, pTestConfigReq->filenameUlResultRef );
        sprintf( fullPathOutput, "%s/%s", pTestConfigReq->basePath, pTestConfigReq->filenameUlResultOutput );

        printf( "--------- TODO : compare ref file %s with output file %s\n", fullPathRef, fullPathOutput );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void fapiHandleRxMsg(Fapi_API_Header *pMsg, uint32_t msgSize)
{
    uint32_t fapiMsgCount, fapiMsgIndex, msgOffset, freeListIndex, rxBuffListCount, rxBuffListIndex;
    uint8_t cellIndex;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    CONFIG_RESP *p_ConfigResp;
    START_RESP *p_StartResp;
    Slot_IND *p_SlotInd;
    uint8_t *p_RxBuffList[MAX_RX_DATA_COUNT_IN_SLOT];

    p_FapiApiHeader = (Fapi_API_Header *) pMsg;
    fapiMsgCount = p_FapiApiHeader->numOfMessages;
    cellIndex = p_FapiApiHeader->cellIndex;
    msgOffset = sizeof(Fapi_API_Header);

    if (msgOffset > msgSize)
    {
        printf( "  Size error : invalid Fapi_API_Header\n" );
        return;
    }

    for (fapiMsgIndex = 0; fapiMsgIndex < fapiMsgCount; fapiMsgIndex++)
    {
        p_FapiGenericHeader = (Fapi_Generic_Header *) (((uint8_t *)pMsg)+msgOffset);
        if (msgOffset+sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length > msgSize)
        {
            printf( "  Size error : invalid Fapi_Generic_Header\n" );
            return;
        }

        switch (p_FapiGenericHeader->type)
        {
            case MSG_PHY_CONFIG_RESP :
                if (p_FapiGenericHeader->length >= sizeof(CONFIG_RESP))
                {
                    p_ConfigResp = (CONFIG_RESP *) (p_FapiGenericHeader+1);
                    if (p_ConfigResp->errorCode == MSG_OK)
                    {
                        printf( ">>> MSG_PHY_CONFIG_RESP with MSG_OK received. Cell %u\n", cellIndex );

                        if (g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.slotCountToRun > 0)
                            txTestConfigReq(cellIndex);
                        else
                            txPhyStartReq(cellIndex);
                    }
                    else
                    {
                        printf( ">>> MSG_PHY_CONFIG_RESP with errorCode %u received. Cell %u\n", p_ConfigResp->errorCode, cellIndex );
                    }
                }
                else
                {
                    printf( ">>> MSG_PHY_CONFIG_RESP : invalid length %u. Cell %u\n", p_FapiGenericHeader->length, cellIndex );
                }
                break;

            case MSG_TEST_CONFIG_RESP :
                printf( ">>> MSG_TEST_CONFIG_RESP received. Cell %u\n", cellIndex );
                txPhyStartReq(cellIndex);
                break;

            case MSG_PHY_START_RESP :
                if (p_FapiGenericHeader->length >= sizeof(START_RESP))
                {
                    p_StartResp = (START_RESP *) (p_FapiGenericHeader+1);
                    if (p_StartResp->errorCode == MSG_OK)
                    {
                        printf( ">>> MSG_PHY_START_RESP with MSG_OK received. Cell %u\n", cellIndex );
                        g_macRunnerConfig.cellConfig[cellIndex].cellStatus = CELL_STATUS_ACTIVE;
                    }
                    else
                    {
                        printf( ">>> MSG_PHY_START_RESP with errorCode %u received. Cell %u\n", p_StartResp->errorCode, cellIndex );
                    }
                }
                else
                {
                    printf( ">>> MSG_PHY_START_RESP : invalid length %u. Cell %u\n", p_FapiGenericHeader->length, cellIndex );
                }
                break;

            case MSG_PHY_SLOT_IND :
                if (p_FapiGenericHeader->length >= sizeof(Slot_IND))
                {
                    p_SlotInd = (Slot_IND *) (p_FapiGenericHeader+1);
                    printf( "\n>>> MSG_PHY_SLOT_IND : Cell %u Sfn %4u Slot %2u\n", cellIndex, p_SlotInd->sfn, p_SlotInd->slot );

                    slotIndHandler( p_SlotInd->sfn, p_SlotInd->slot, cellIndex );
                }
                else
                {
                    printf( "\n>>> MSG_PHY_SLOT_IND : invalid length %u. Cell %u\n", p_FapiGenericHeader->length, cellIndex );
                }

                if (g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.slotCountToRun > 0)
                {
                    g_macRunnerConfig.cellConfig[cellIndex].slotCountRemaining--;
                    if (g_macRunnerConfig.cellConfig[cellIndex].slotCountRemaining == 0)
                    {
                        txPhyStopReq(cellIndex);
                        g_macRunnerConfig.cellConfig[cellIndex].cellStatus = CELL_STATUS_STOP;
                    }
                }
                break;

            case MSG_PHY_CRC_IND :
                printf( "  >>> MSG_PHY_CRC_IND : Cell %u\n", cellIndex );
                mac_ParseMsg_CrcInd(p_FapiGenericHeader);
                break;

            case MSG_PHY_RX_DATA_IND :
                printf( "  >>> MSG_PHY_RX_DATA_IND : Cell %u\n", cellIndex );
                mac_ParseMsg_RxDataInd(p_FapiGenericHeader, p_RxBuffList, &rxBuffListCount);

                // Release Rx Buff
                for (rxBuffListIndex = 0; rxBuffListIndex < rxBuffListCount; rxBuffListIndex++)
                    wls_mac_free_buffer(p_RxBuffList[rxBuffListIndex], MIN_UL_BUF_LOCATIONS);
                break;

            case MSG_PHY_STOP_IND :
                printf( ">>> MSG_PHY_STOP_IND received. Cell %u\n", cellIndex );
                g_macRunnerConfig.cellConfig[cellIndex].cellStatus = CELL_STATUS_CONFIGURED;

                for (freeListIndex = 0; freeListIndex < MAX_FAPI_FREE_LIST_COUNT; freeListIndex++)
                    fapiFreeMsgsInFapiFreeList( freeListIndex, cellIndex );

                handleTestConfig( cellIndex );
                break;

            default :
                break;
        }

        msgOffset += sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length;
    }
}

void *threadMain_FapiRx(void *data)
{
    int msg_count;
    uint16_t flags;
    uint64_t data_pa;
    void *p_data_va;
    Fapi_API_Header *pMsgNew;
    Fapi_API_Header *pMsgHead;
    Fapi_API_Header *pMsgPrev;
    Fapi_API_Header *pMsgNext;

    while (1)
    {
        msg_count = wls_mac_wait();
        if (msg_count <= 0)
        {
            usleep(10);
            continue;
        }

        flags = 0;
        pMsgNew = NULL;
        pMsgHead = NULL;
        pMsgPrev = NULL;
        pMsgNext = NULL;

        while (msg_count)
        {
            flags = 0;
            wls_mac_recv(&data_pa, &flags);
            p_data_va = wls_mac_pa_to_va(data_pa);

            pMsgNew = (Fapi_API_Header *) p_data_va;
            pMsgNew->pNext = NULL;

            if (pMsgHead == NULL)
                pMsgHead = pMsgNew;

            if (pMsgPrev != NULL)
                pMsgPrev->pNext = pMsgNew;

            pMsgPrev = pMsgNew;

            msg_count--;

            wls_mac_add_blocks_to_ul();
        }

        pMsgNew = pMsgHead;
        while (pMsgNew != NULL)
        {
            pMsgNext = pMsgNew->pNext;
            pMsgNew->pNext = NULL;

            fapiHandleRxMsg(pMsgNew, pMsgNew->msgSize);

            wls_mac_free_buffer(pMsgNew, MIN_UL_BUF_LOCATIONS);
            pMsgNew = pMsgNext;
        }
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int macRunner( void )
{
    pthread_t threadInfo;
    uint8_t cellIndex;

    if (initWls() != SUCCESS)
        return FAILURE;

    pthread_create(&threadInfo, NULL, threadMain_FapiRx, (void *) NULL);

    for (cellIndex = 0; cellIndex < MAX_CELL_COUNT; cellIndex++)
    {
        if (g_macRunnerConfig.cellConfig[cellIndex].cellStatus != CELL_STATUS_CONFIGURED)
            continue;

        txPhyConfigReq(cellIndex);

        if (g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.slotCountToRun > 0)
            g_macRunnerConfig.cellConfig[cellIndex].slotCountRemaining = g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.slotCountToRun;
    }

    while(TRUE)
        sleep(1);

    return SUCCESS;
}


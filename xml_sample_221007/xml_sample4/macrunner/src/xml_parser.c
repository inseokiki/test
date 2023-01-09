#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libgen.h>
#include <unistd.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "common.h"
#include "gSM_l1_l2_api.h"
#include "xml_lib.h"
#include "xml_parser.h"
#include "main.h"

#define XML_LOAD_DEBUG(fmt,args...)  { printf(fmt,##args); }
#define XML_LOAD_TRACE(fmt,args...)  { printf(fmt,##args); }
#define XML_LOAD_WARNING(fmt,args...)  { printf(fmt,##args); }

uint32_t g_phyRel16Support = TRUE;
uint32_t g_phyFapiV4MuMimoGroupSupport = FALSE;
uint32_t g_phySupportL2CbgSeg = FALSE;
uint32_t g_phySupportL1CbgSeg = FALSE;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int loadDlTtiReqMsg( xmlNode *pNode_FapiMsg, uint32_t msgBuffSize, uint8_t *p_MsgBuff, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength, pduLength, pduIndex, value_U32;
    uint32_t pduCountSsb, pduCountCsiRs, pduCountPdcch, pduCountPdsch, pduCountDlDci, pduCountTotal, pduNodeCount;
    uint32_t value_U32Array[MAX_MULTIPLE_XML_NODE_COUNT], arrayCount, arrayIndex;
    uint32_t bchPayloadFlag, numPRGs, prgIndex, digBFInterfaces, digBFInterfacesIndex;
    uint32_t trpScheme, trpIndex, numTrp, nodeIndexOfBeamIdx, numMuMimoGroups, muMimoGroupIndex;
    uint32_t numDlDci, dlDciIndex, pduBitmap, nrOfCodewords, codewordIndex, szRm, numPrbSymRmPatternsByValue, prbSymRmPatternsByValueIndex;
    uint32_t numCoresetRmPatterns, coresetRmPatternIndex, szCrs, szCsiRs, szCrsMbsfn;
    uint32_t digBFInterfacesForTrp[MAX_NUM_TRP];
    int32_t value_S32;
    xmlNode *pNode_PduList[MAX_MULTIPLE_XML_NODE_COUNT], *pNode_Pdu, *pNode_Structure, *pNode_SubStructure;
    xmlChar *pProperty;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    DL_TTI_REQ *p_DlTtiReq;
    PDU_Struct *p_pduHeader;
    SSBPBCH_PDU *p_SsbPdu;
    CSI_RS_PDU *p_CsirsPdu;
    PDCCH_PDU *p_PdcchPdu;
    PDSCH_PDU *p_PdschPdu;
    MAC_GenMIB_PDU *p_MacGenMIBPdu;
    PHY_GenMIB_PDU *p_PHY_GenMIB_PDU;
    TxPrecodingAndBeamforming_PDU *p_TxPrecodingAndBeamforming_PDU;
    Rel16mTRP_TxPrecodingAndBeamforming_PDU *p_Rel16mTRP_TxPrecodingAndBeamforming_PDU;
    Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo;
    SSBPBCH_PDU_Maintenance_FAPIv3 *p_SSBPBCH_PDU_Maintenance_FAPIv3;
    SSBPBCH_PDU_FAPIv4 *p_SSBPBCH_PDU_FAPIv4;
    CSI_RS_PDU_MaintenanceParam_FAPIv3 *p_CSI_RS_PDU_MaintenanceParam_FAPIv3;
    CSI_RS_PDU_FAPIv4 *p_CSI_RS_PDU_FAPIv4;
    Tx_Generalized_Precoding_PDU *p_Tx_Generalized_Precoding_PDU;
    Tx_Generalized_Precoding_MuMimoGroupInfo *p_Tx_Generalized_Precoding_MuMimoGroupInfo;
    PDCCH_PDU_Maintenance_FAPIv3 *p_PDCCH_PDU_Maintenance_FAPIv3;
    PDCCH_PDU_Maintenance_FAPIv3_DCIInfo *p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo;
    PDCCH_PDU_FAPIv4_DCIInfo *p_PDCCH_PDU_FAPIv4_DCIInfo;
    PDCCH_PDU_FAPIv4_MuMimo *p_PDCCH_PDU_FAPIv4_MuMimo;
    PDCCH_PDU_FAPIv4_MuMimo_DCIInfo *p_PDCCH_PDU_FAPIv4_MuMimo_DCIInfo;
    DL_DCI_PDU_part1 *p_DL_DCI_PDU_part1;
    DL_DCI_PDU_part2 *p_DL_DCI_PDU_part2;
    PDSCH_CodewordInfo *p_PDSCH_CodewordInfo;
    PDSCH_PTRS *p_PDSCH_PTRS;
    PDSCH_TxPowerInfo *p_PDSCH_TxPowerInfo;
    PDSCH_CBGReTxCtlInfo *p_PDSCH_CBGReTxCtlInfo;
    PDSCH_Maintenance_FAPIv3 *p_PDSCH_Maintenance_FAPIv3;
    PDSCH_PrbSymRmPatternsInfo *p_PDSCH_PrbSymRmPatternsInfo;
    PDSCH_CoresetRmPatternsInfo *p_PDSCH_CoresetRmPatternsInfo;
    PDSCH_CBGTxInfo *p_PDSCH_CBGTxInfo;
    PDSCH_PTRS_Maintenance_FAPIv3 *p_PDSCH_PTRS_Maintenance_FAPIv3;
    Rel16_PDSCH_FAPIv3 *p_Rel16_PDSCH_FAPIv3;
    PDSCH_PDU_FAPIv4 *p_PDSCH_PDU_FAPIv4;
    LayerMappingcwInfo *p_LayerMappingcwInfo;
    uint16_t *p_Value_U16;

    XML_LOAD_DEBUG( "    MSG_PHY_DL_TTI_REQ\n" );

    memset( p_MsgBuff, 0, msgBuffSize );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) p_MsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    *p_MsgLength = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_PHY_DL_TTI_REQ;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_DlTtiReq = (DL_TTI_REQ *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(DL_TTI_REQ);

    p_DlTtiReq->sfn = 0;  
    p_DlTtiReq->slot = 0; 
    p_DlTtiReq->numPDUs = 0;
    p_DlTtiReq->nDlTypes = NUM_OF_DLPDU_TYPE;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDCCH] = 0;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDSCH] = 0;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_CSI_RS] = 0;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_SSB] = 0;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_DLDCI] = 0;
    p_DlTtiReq->nGroup = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Load PDUs

    pduCountSsb = pduCountCsiRs = pduCountPdcch = pduCountPdsch = pduCountTotal = 0;
    pduCountDlDci = 0;

    pduNodeCount = 0;

    p_pduHeader = (PDU_Struct *) p_DlTtiReq->variablePart;

    searchXmlMultipleNodes( pNode_FapiMsg, pNode_PduList, &pduNodeCount, "PDU" );
    for (pduIndex = 0; pduIndex < pduNodeCount; pduIndex++)
    {
        pNode_Pdu = pNode_PduList[pduIndex];
        pProperty = xmlGetProp(pNode_Pdu, (const xmlChar *) "type");
        if (pProperty == NULL)
        {
            XML_LOAD_DEBUG( "    PDU %u : no type info\n", pduIndex );
            continue;
        }

        pduLength = 0;

        if (strcasecmp( (char *) pProperty, "SSB" ) == 0)
        {
            XML_LOAD_DEBUG( "      SSB PDU\n" );

            p_pduHeader->pduType = DL_PDU_TYPE_SSB;
            pduCountSsb++;

            p_SsbPdu = (SSBPBCH_PDU *) p_pduHeader;
            pduLength += sizeof(SSBPBCH_PDU);

            bchPayloadFlag = 0;
            numPRGs = 0;
            digBFInterfaces = 0;
            trpScheme = 0;
            memset( digBFInterfacesForTrp, 0, sizeof(digBFInterfacesForTrp) );

            if (getXmlNodeValueU32(pNode_Pdu, "physCellId", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        physCellId : %u\n", value_U32 );

                p_SsbPdu->physCellId = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "betaPssProfileNR", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        betaPssProfileNR : %u\n", value_U32 );

                p_SsbPdu->betaPssProfileNR = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "ssbBlockIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        ssbBlockIndex : %u\n", value_U32 );

                p_SsbPdu->ssbBlockIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "ssbSubcarrierOffset", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        ssbSubcarrierOffset: %u\n", value_U32 );

                p_SsbPdu->ssbSubcarrierOffset = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "ssbOffsetPointA", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        ssbOffsetPointA : %u\n", value_U32 );

                p_SsbPdu->ssbOffsetPointA = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bchPayloadFlag", &value_U32 ) == SUCCESS)
            {
                bchPayloadFlag = value_U32;
                XML_LOAD_DEBUG( "        bchPayloadFlag : %u\n", value_U32 );

                p_SsbPdu->bchPayloadFlag = (uint8_t) value_U32;
            }
   
            if (bchPayloadFlag == BCH_PAYLOAD_MAC_GENERATES_FULL || bchPayloadFlag == BCH_PAYLOAD_PHY_GENERATES_TIMING)
            {
                p_MacGenMIBPdu = (MAC_GenMIB_PDU *) ((uint8_t *)p_pduHeader + pduLength);

                pduLength += sizeof(MAC_GenMIB_PDU);

                if (getXmlNodeValueU32(pNode_Pdu, "bchPayload", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        bchPayload : 0x%08x\n", value_U32 );

                    p_MacGenMIBPdu->bchPayload = value_U32;
                }
            }
            else    // BCH_PAYLOAD_PHY_GENERATES_FULL
            {
                p_PHY_GenMIB_PDU = (PHY_GenMIB_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PHY_GenMIB_PDU);

                if (getXmlNodeValueU32(pNode_Pdu, "dmrsTypeAPosition", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        dmrsTypeAPosition : %u\n", value_U32 );

                    p_PHY_GenMIB_PDU->dmrsTypeAPosition = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Pdu, "pdcchConfigSib1", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        pdcchConfigSib1 : %u\n", value_U32 );

                    p_PHY_GenMIB_PDU->pdcchConfigSib1 = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Pdu, "cellBarred", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        cellBarred : %u\n", value_U32 );

                    p_PHY_GenMIB_PDU->cellBarred = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Pdu, "intraFreqReselection", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        intraFreqReselection : %u\n", value_U32 );

                    p_PHY_GenMIB_PDU->intraFreqReselection = (uint8_t) value_U32;
                }
            }

            if (g_phyRel16Support == FALSE && searchXmlNode( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamforming") != 0)
            {
                XML_LOAD_DEBUG( "        PrecodingAndBeamforming :\n" );

                p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                {
                    numPRGs = value_U32;
                    XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                {
                    digBFInterfaces = value_U32;
                    XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                }

                nodeIndexOfBeamIdx = 0;
                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                    p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(uint16_t);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                        *p_Value_U16 = (uint16_t) value_U32;
                    }

                    for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                    {
                        XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }
                        nodeIndexOfBeamIdx++;
                    }
                }
            }
            else if (g_phyRel16Support && searchXmlNode( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamformingRel16") != 0)
            {
                XML_LOAD_DEBUG( "        PrecodingAndBeamformingRel16 :\n" );

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    trpScheme = value_U32;
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );
                }

                if (trpScheme == TRP_SCHEME_BASE)    // use Table 3-84
                {
                    p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                    p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;
 
                    if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                    {
                        numPRGs = value_U32;
                        XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                    {
                        digBFInterfaces = value_U32;
                        XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                    }

                    nodeIndexOfBeamIdx = 0;
                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }

                        for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                        {
                            XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                            p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(uint16_t);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                                *p_Value_U16 = (uint16_t) value_U32;
                            }
                            nodeIndexOfBeamIdx++;
                        }
                    }
                }
                else
                {
                    if (trpScheme == TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY)
                        numTrp = 2;
                    else    // TRP_SCHEME_STRP/TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY/TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY
                        numTrp = 1;

                    p_Rel16mTRP_TxPrecodingAndBeamforming_PDU = (Rel16mTRP_TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PDU);

                    p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                    if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                    {
                        numPRGs = value_U32;
                        XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array(pNode_Structure, "digBFInterfaces", value_U32Array, &arrayCount ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          digBFInterfaces : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                        {
                            digBFInterfacesForTrp[arrayIndex] = value_U32Array[arrayIndex];

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->digBFInterfaces[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    nodeIndexOfBeamIdx = 0;
                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo = (Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "layers", &value_U32, prgIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            layers : %u\n", value_U32 );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->layers = (uint8_t) value_U32;
                        }

                        if (getXmlNodeValueU32Array(pNode_Structure, "pmIdx", value_U32Array, &arrayCount ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            pmIdx : count:%u values:", arrayCount );
                            for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                            XML_LOAD_DEBUG( "\n" );

                            for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                            {
                                p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->pmIdx[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                            }
                        }

                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            XML_LOAD_DEBUG( "            ForEach numTrp[%u] :\n", trpIndex );

                            digBFInterfaces = digBFInterfacesForTrp[trpIndex];
                            for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                            {
                                XML_LOAD_DEBUG( "              ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                                pduLength += sizeof(uint16_t);

                                if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "                beamIdx : %u\n", value_U32 );

                                    *p_Value_U16 = (uint16_t) value_U32;
                                }
                                nodeIndexOfBeamIdx++;
                            }
                        }
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(TxPrecodingAndBeamforming_PDU);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "ssbMaintenanceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        ssbMaintenanceFapiV3 :\n" );

                p_SSBPBCH_PDU_Maintenance_FAPIv3 = (SSBPBCH_PDU_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(SSBPBCH_PDU_Maintenance_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "ssbPduIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ssbPduIndex : %u\n", value_U32 );

                    p_SSBPBCH_PDU_Maintenance_FAPIv3->ssbPduIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ssbFirstSymbolMappingCase", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ssbFirstSymbolMappingCase : %u\n", value_U32 );

                    p_SSBPBCH_PDU_Maintenance_FAPIv3->ssbFirstSymbolMappingCase = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "subcarrierSpacing", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          subcarrierSpacing : %u\n", value_U32 );

                    p_SSBPBCH_PDU_Maintenance_FAPIv3->subcarrierSpacing = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "lMax", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          lMax : %u\n", value_U32 );

                    p_SSBPBCH_PDU_Maintenance_FAPIv3->lMax = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ssPbchBlockPowerScaling", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ssPbchBlockPowerScaling : %u\n", value_U32 );

                    p_SSBPBCH_PDU_Maintenance_FAPIv3->ssPbchBlockPowerScaling = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "betaPSSProfileSSS", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          betaPSSProfileSSS : %u\n", value_U32 );

                    p_SSBPBCH_PDU_Maintenance_FAPIv3->betaPSSProfileSSS = (uint16_t) value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(SSBPBCH_PDU_Maintenance_FAPIv3);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "ssbParametersFapiV4") != 0)
            {
                XML_LOAD_DEBUG( "        ssbParametersFapiV4 :\n" );

                p_SSBPBCH_PDU_FAPIv4 = (SSBPBCH_PDU_FAPIv4 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(SSBPBCH_PDU_FAPIv4);

                if (getXmlNodeValueU32(pNode_Structure, "spatialStreamIndexPresent", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          spatialStreamIndexPresent : %u\n", value_U32 );

                    p_SSBPBCH_PDU_FAPIv4->spatialStreamIndexPresent = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "spatialStreamIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          spatialStreamIndex : %u\n", value_U32 );

                    p_SSBPBCH_PDU_FAPIv4->spatialStreamIndex = (uint8_t) value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(SSBPBCH_PDU_FAPIv4);
            }

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
        else if (strcasecmp( (char *) pProperty, "CSIRS" ) == 0)
        {
            XML_LOAD_DEBUG( "      CSIRS PDU\n" );

            p_pduHeader->pduType = DL_PDU_TYPE_CSI_RS;
            pduCountCsiRs++;

            p_CsirsPdu = (CSI_RS_PDU *) p_pduHeader;
            pduLength += sizeof(CSI_RS_PDU);

            numPRGs = 0;
            digBFInterfaces = 0;
            trpScheme = 0;
            memset( digBFInterfacesForTrp, 0, sizeof(digBFInterfacesForTrp) );
            numMuMimoGroups = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "subcarrierSpacing", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                p_CsirsPdu->subcarrierSpacing = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicPrefix", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicPrefix : %u\n", value_U32 );

                p_CsirsPdu->cyclicPrefix = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "startRB", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        startRB : %u\n", value_U32 );

                p_CsirsPdu->startRB = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nrOfRBs", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nrOfRBs : %u\n", value_U32 );

                p_CsirsPdu->nrOfRBs = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "csiType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        csiType : %u\n", value_U32 );

                p_CsirsPdu->csiType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "row", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        row : %u\n", value_U32 );

                p_CsirsPdu->row = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "freqDomain", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        freqDomain : %u\n", value_U32 );

                p_CsirsPdu->freqDomain = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "symbL0", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        symbL0 : %u\n", value_U32 );

                p_CsirsPdu->symbL0 = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "symbL1", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        symbL1 : %u\n", value_U32 );

                p_CsirsPdu->symbL1 = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cdmType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cdmType : %u\n", value_U32 );

                p_CsirsPdu->cdmType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "freqDensity", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        freqDensity : %u\n", value_U32 );

                p_CsirsPdu->freqDensity = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "scrambId", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        scrambId : %u\n", value_U32 );

                p_CsirsPdu->scrambId = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "powerControlOffsetProfileNR", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        powerControlOffsetProfileNR : %u\n", value_U32 );

                p_CsirsPdu->powerControlOffsetProfileNR = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "powerControlOffsetSSProfileNR", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        powerControlOffsetSSProfileNR : %u\n", value_U32 );

                p_CsirsPdu->powerControlOffsetSSProfileNR = (uint8_t) value_U32;
            }

            if (g_phyRel16Support == FALSE && searchXmlNode( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamforming") != 0)
            {
                XML_LOAD_DEBUG( "        PrecodingAndBeamforming :\n" );

                p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                {
                    numPRGs = value_U32;
                    XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                {
                    digBFInterfaces = value_U32;
                    XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                }

                nodeIndexOfBeamIdx = 0;
                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                    p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(uint16_t);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                        *p_Value_U16 = (uint16_t) value_U32;
                    }

                    for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                    {
                        XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }
                        nodeIndexOfBeamIdx++;
                    }
                }
            }
            else if (g_phyRel16Support && searchXmlNode( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamformingRel16") != 0)
            {
                XML_LOAD_DEBUG( "        PrecodingAndBeamformingRel16 :\n" );

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    trpScheme = value_U32;
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );
                }

                if (trpScheme == TRP_SCHEME_BASE)    // use Table 3-84
                {
                    p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                    p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                    if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                    {
                        numPRGs = value_U32;
                        XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                    {
                        digBFInterfaces = value_U32;
                        XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                    }

                    nodeIndexOfBeamIdx = 0;
                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }

                        for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                        {
                            XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                            p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(uint16_t);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                                *p_Value_U16 = (uint16_t) value_U32;
                            }
                            nodeIndexOfBeamIdx++;
                        }
                    }
                }
                else
                {
                    if (trpScheme == TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY)
                        numTrp = 2;
                    else    // TRP_SCHEME_STRP/TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY/TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY
                        numTrp = 1;

                    p_Rel16mTRP_TxPrecodingAndBeamforming_PDU = (Rel16mTRP_TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PDU);

                    p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                    if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                    {
                        numPRGs = value_U32;
                        XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array(pNode_Structure, "digBFInterfaces", value_U32Array, &arrayCount ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          digBFInterfaces : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                        {
                            digBFInterfacesForTrp[arrayIndex] = value_U32Array[arrayIndex];

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->digBFInterfaces[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    nodeIndexOfBeamIdx = 0;
                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo = (Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "layers", &value_U32, prgIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            layers : %u\n", value_U32 );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->layers = (uint8_t) value_U32;
                        }

                        if (getXmlNodeValueU32Array(pNode_Structure, "pmIdx", value_U32Array, &arrayCount ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            pmIdx : count:%u values:", arrayCount );
                            for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                            XML_LOAD_DEBUG( "\n" );

                            for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                            {
                                p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->pmIdx[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                            }
                        }

                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            XML_LOAD_DEBUG( "            ForEach numTrp[%u] :\n", trpIndex );

                            digBFInterfaces = digBFInterfacesForTrp[trpIndex];
                            for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                            {
                                XML_LOAD_DEBUG( "              ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                                pduLength += sizeof(uint16_t);

                                if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "                beamIdx : %u\n", value_U32 );

                                    *p_Value_U16 = (uint16_t) value_U32;
                                }
                                nodeIndexOfBeamIdx++;
                            }
                        }
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(TxPrecodingAndBeamforming_PDU);
            }

            if (g_phyFapiV4MuMimoGroupSupport && searchXmlNode( pNode_Pdu, &pNode_Structure, "TxGenPrecodingPdu") != 0)
            {
                XML_LOAD_DEBUG( "        TxGenPrecodingPdu :\n" );

                p_Tx_Generalized_Precoding_PDU = (Tx_Generalized_Precoding_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(Tx_Generalized_Precoding_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_Tx_Generalized_Precoding_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numMuMimoGroups", &value_U32 ) == SUCCESS)
                {
                    numMuMimoGroups = value_U32;
                    XML_LOAD_DEBUG( "          numMuMimoGroups : %u\n", value_U32 );

                    p_Tx_Generalized_Precoding_PDU->numMuMimoGroups = (uint8_t) value_U32;
                }

                for (muMimoGroupIndex = 0; muMimoGroupIndex < numMuMimoGroups; muMimoGroupIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numMuMimoGroups[%u] :\n", muMimoGroupIndex );

                    p_Tx_Generalized_Precoding_MuMimoGroupInfo = (Tx_Generalized_Precoding_MuMimoGroupInfo *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(Tx_Generalized_Precoding_MuMimoGroupInfo);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "muMimoGroupIdx", &value_U32, muMimoGroupIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            prachRootSequenceIndex : %u\n", value_U32 );

                        p_Tx_Generalized_Precoding_MuMimoGroupInfo->muMimoGroupIdx = (uint16_t) value_U32;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                if (g_phyFapiV4MuMimoGroupSupport)
                    pduLength += sizeof(Tx_Generalized_Precoding_PDU);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "csiRsMaintenanceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        csiRsMaintenanceFapiV3 :\n" );

                p_CSI_RS_PDU_MaintenanceParam_FAPIv3 = (CSI_RS_PDU_MaintenanceParam_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(CSI_RS_PDU_MaintenanceParam_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "csiRsPduIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          csiRsPduIndex : %u\n", value_U32 );

                    p_CSI_RS_PDU_MaintenanceParam_FAPIv3->csiRsPduIndex = (uint16_t) value_U32;
                }

                if (getXmlNodeValueS32(pNode_Structure, "csiRsPowerOffsetProfileSSS", &value_S32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          csiRsPowerOffsetProfileSSS : %d\n", value_S32 );

                    p_CSI_RS_PDU_MaintenanceParam_FAPIv3->csiRsPowerOffsetProfileSSS = (int16_t) value_S32;
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(CSI_RS_PDU_MaintenanceParam_FAPIv3);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "csiRsParametersFapiV4") != 0)
            {
                XML_LOAD_DEBUG( "        csiRsParametersFapiV4 :\n" );

                p_CSI_RS_PDU_FAPIv4 = (CSI_RS_PDU_FAPIv4 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(CSI_RS_PDU_FAPIv4);

                if (getXmlNodeValueU32(pNode_Structure, "spatialStreamIndicesPresent", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          spatialStreamIndicesPresent : %u\n", value_U32 );

                    p_CSI_RS_PDU_FAPIv4->spatialStreamIndicesPresent = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "spatialStreamIndices", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          spatialStreamIndices : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_CDM_GROUP_COUNT; arrayIndex++)
                    {
                        p_CSI_RS_PDU_FAPIv4->spatialStreamIndices[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(CSI_RS_PDU_FAPIv4);
            }

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
        else if (strcasecmp( (char *) pProperty, "PDCCH" ) == 0)
        {
            XML_LOAD_DEBUG( "      PDCCH PDU\n" );

            p_pduHeader->pduType = DL_PDU_TYPE_PDCCH;
            pduCountPdcch++;

            p_PdcchPdu = (PDCCH_PDU *) p_pduHeader;
            pduLength += sizeof(PDCCH_PDU);

            numDlDci = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "coresetBWPSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        coresetBWPSize : %u\n", value_U32 );

                p_PdcchPdu->coresetBWPSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "coresetBWPStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        coresetBWPStart : %u\n", value_U32 );

                p_PdcchPdu->coresetBWPStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "subcarrierSpacing", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                p_PdcchPdu->subcarrierSpacing = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicPrefix", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicPrefix : %u\n", value_U32 );

                p_PdcchPdu->cyclicPrefix = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "startSymbolIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        startSymbolIndex : %u\n", value_U32 );

                p_PdcchPdu->startSymbolIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "durationSymbols", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        durationSymbols : %u\n", value_U32 );

                p_PdcchPdu->durationSymbols = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32Array(pNode_Pdu, "freqDomainResource", value_U32Array, &arrayCount ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        freqDomainResource : count:%u values:", arrayCount );
                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                XML_LOAD_DEBUG( "\n" );

                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE; arrayIndex++)
                {
                    p_PdcchPdu->freqDomainResource[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                }
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cceRegMappingType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cceRegMappingType : %u\n", value_U32 );

                p_PdcchPdu->cceRegMappingType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "regBundleSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        regBundleSize : %u\n", value_U32 );

                p_PdcchPdu->regBundleSize = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "interleaverSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        interleaverSize : %u\n", value_U32 );

                p_PdcchPdu->interleaverSize = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "coreSetType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        coreSetType : %u\n", value_U32 );

                p_PdcchPdu->coreSetType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "shiftIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        shiftIndex : %u\n", value_U32 );

                p_PdcchPdu->shiftIndex = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "precoderGranularity", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        precoderGranularity: %u\n", value_U32 );

                p_PdcchPdu->precoderGranularity = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numDlDci", &value_U32 ) == SUCCESS)
            {
                numDlDci = value_U32;
                XML_LOAD_DEBUG( "        numDlDci : %u\n", value_U32 );

                p_PdcchPdu->numDlDci = (uint16_t) value_U32;
            }

            for (dlDciIndex = 0; dlDciIndex < numDlDci; dlDciIndex++)
            {
                XML_LOAD_DEBUG( "        ForEach numDlDci[%u] :\n", dlDciIndex );

                p_DL_DCI_PDU_part1 = (DL_DCI_PDU_part1 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(DL_DCI_PDU_part1);

                numPRGs = 0;
                digBFInterfaces = 0;
                trpScheme = 0;
                memset( digBFInterfacesForTrp, 0, sizeof(digBFInterfacesForTrp) );
                numMuMimoGroups = 0;

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "rnti", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          rnti : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->rnti = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "nIdPdcchData", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          nIdPdcchData : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->nIdPdcchData = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "nRntiPdcchData", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          nRntiPdcchData : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->nRntiPdcchData = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "cceIndex", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          cceIndex : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->cceIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "aggregationLevel", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          aggregationLevel : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->aggregationLevel = (uint8_t) value_U32;
                }

                if (g_phyRel16Support == FALSE && searchXmlNode_Nth( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamforming", dlDciIndex ) != 0)
                {
                    XML_LOAD_DEBUG( "        PrecodingAndBeamforming :\n" );

                    p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                    if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                    {
                        numPRGs = value_U32;
                        XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                    {
                        digBFInterfaces = value_U32;
                        XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                    }

                    nodeIndexOfBeamIdx = 0;
                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }

                        for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                        {
                            XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                            p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(uint16_t);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                                *p_Value_U16 = (uint16_t) value_U32;
                            }
                            nodeIndexOfBeamIdx++;
                        }
                    }
                }
                else if (g_phyRel16Support && searchXmlNode_Nth( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamformingRel16", dlDciIndex ) != 0)
                {
                    XML_LOAD_DEBUG( "        PrecodingAndBeamformingRel16 :\n" );

                    if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                    {
                        trpScheme = value_U32;
                        XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );
                    }

                    if (trpScheme == TRP_SCHEME_BASE)    // use Table 3-84
                    {
                        p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                        p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                        if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                        {
                            numPRGs = value_U32;
                            XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                            p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                            p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                        {
                            digBFInterfaces = value_U32;
                            XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                            p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                        }

                        nodeIndexOfBeamIdx = 0;
                        for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                        {
                            XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                            p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(uint16_t);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                                *p_Value_U16 = (uint16_t) value_U32;
                            }

                            for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                            {
                                XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                                pduLength += sizeof(uint16_t);

                                if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                                    *p_Value_U16 = (uint16_t) value_U32;
                                }
                                nodeIndexOfBeamIdx++;
                            }
                        }
                    }
                    else
                    {
                        if (trpScheme == TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY)
                            numTrp = 2;
                        else    // TRP_SCHEME_STRP/TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY/TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY
                            numTrp = 1;

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU = (Rel16mTRP_TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PDU);

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                        if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                        {
                            numPRGs = value_U32;
                            XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32Array(pNode_Structure, "digBFInterfaces", value_U32Array, &arrayCount ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          digBFInterfaces : count:%u values:", arrayCount );
                            for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                            XML_LOAD_DEBUG( "\n" );

                            for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                            {
                                digBFInterfacesForTrp[arrayIndex] = value_U32Array[arrayIndex];

                                p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->digBFInterfaces[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                            }
                        }
                        nodeIndexOfBeamIdx = 0;
                        for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                        {
                            XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo = (Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "layers", &value_U32, prgIndex) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "            layers : %u\n", value_U32 );

                                p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->layers = (uint8_t) value_U32;
                            }

                            if (getXmlNodeValueU32Array(pNode_Structure, "pmIdx", value_U32Array, &arrayCount ) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "            pmIdx : count:%u values:", arrayCount );
                                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                                XML_LOAD_DEBUG( "\n" );

                                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                                {
                                    p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->pmIdx[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                                }
                            }

                            for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                            {
                                XML_LOAD_DEBUG( "            ForEach numTrp[%u] :\n", trpIndex );

                                digBFInterfaces = digBFInterfacesForTrp[trpIndex];
                                for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                                {
                                    XML_LOAD_DEBUG( "              ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                    p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                                    pduLength += sizeof(uint16_t);

                                    if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                                    {
                                        XML_LOAD_DEBUG( "                beamIdx : %u\n", value_U32 );

                                        *p_Value_U16 = (uint16_t) value_U32;
                                    }
                                    nodeIndexOfBeamIdx++;
                                }
                            }
                        }
                    }
                }
                else
                {
                    // add length for Mandatory fields
                    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);
                }

                if (g_phyFapiV4MuMimoGroupSupport && searchXmlNode_Nth( pNode_Pdu, &pNode_Structure, "TxGenPrecodingPdu", dlDciIndex ) != 0)
                {
                    XML_LOAD_DEBUG( "        TxGenPrecodingPdu :\n" );

                    p_Tx_Generalized_Precoding_PDU = (Tx_Generalized_Precoding_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(Tx_Generalized_Precoding_PDU);

                    if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                        p_Tx_Generalized_Precoding_PDU->trpScheme = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "numMuMimoGroups", &value_U32 ) == SUCCESS)
                    {
                        numMuMimoGroups = value_U32;
                        XML_LOAD_DEBUG( "          numMuMimoGroups : %u\n", value_U32 );

                        p_Tx_Generalized_Precoding_PDU->numMuMimoGroups = (uint8_t) value_U32;
                    }

                    for (muMimoGroupIndex = 0; muMimoGroupIndex < numMuMimoGroups; muMimoGroupIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numMuMimoGroups[%u] :\n", muMimoGroupIndex );

                        p_Tx_Generalized_Precoding_MuMimoGroupInfo = (Tx_Generalized_Precoding_MuMimoGroupInfo *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(Tx_Generalized_Precoding_MuMimoGroupInfo);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "muMimoGroupIdx", &value_U32, muMimoGroupIndex ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            prachRootSequenceIndex : %u\n", value_U32 );

                            p_Tx_Generalized_Precoding_MuMimoGroupInfo->muMimoGroupIdx = (uint16_t) value_U32;
                        }
                    }
                }
                else
                {
                    // add length for Mandatory fields
                    if (g_phyFapiV4MuMimoGroupSupport)
                        pduLength += sizeof(Tx_Generalized_Precoding_PDU);
                }

                p_DL_DCI_PDU_part2 = (DL_DCI_PDU_part2 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(DL_DCI_PDU_part2);

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "beta_PDCCH_1_0", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        beta_PDCCH_1_0 : %u\n", value_U32 );

                    p_DL_DCI_PDU_part2->beta_PDCCH_1_0 = (uint8_t) value_U32;
                }

                if (getXmlNodeValueS32_Nth(pNode_Pdu, "powerControlOffsetSSProfileNR", &value_S32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        powerControlOffsetSSProfileNR : %d\n", value_S32 );

                    p_DL_DCI_PDU_part2->powerControlOffsetSSProfileNR = (int8_t) value_S32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "payloadSizeBits", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        payloadSizeBits : %u\n", value_U32 );

                    p_DL_DCI_PDU_part2->payloadSizeBits = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32Array_Nth(pNode_Pdu, "payload", value_U32Array, &arrayCount, dlDciIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        payload : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < DCI_PAYLOAD_BTYE_LEN; arrayIndex++)
                    {
                        p_DL_DCI_PDU_part2->payload[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                pduCountDlDci++;
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "pdcchMaintenanceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        pdcchMaintenanceFapiV3 :\n" );

                p_PDCCH_PDU_Maintenance_FAPIv3 = (PDCCH_PDU_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PDCCH_PDU_Maintenance_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "pdcchPduIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          pdcchPduIndex : %u\n", value_U32 );

                    p_PDCCH_PDU_Maintenance_FAPIv3->pdcchPduIndex = (uint16_t) value_U32;
                }

                for (dlDciIndex = 0; dlDciIndex < numDlDci; dlDciIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numDlDci[%u] :\n", dlDciIndex );

                    p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo = (PDCCH_PDU_Maintenance_FAPIv3_DCIInfo *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDCCH_PDU_Maintenance_FAPIv3_DCIInfo);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "dciIndex", &value_U32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            dciIndex : %u\n", value_U32 );

                        p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo->dciIndex = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "collocatedAl16Candidate", &value_U32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            collocatedAl16Candidate : %u\n", value_U32 );

                        p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo->collocatedAl16Candidate = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueS32_Nth(pNode_Structure, "pdcchDmrsPowerOffsetProfileSSS", &value_S32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            pdcchDmrsPowerOffsetProfileSSS : %d\n", value_S32 );

                        p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo->pdcchDmrsPowerOffsetProfileSSS = (int16_t) value_S32;
                    }

                    if (getXmlNodeValueS32_Nth(pNode_Structure, "pdcchDataPowerOffsetProfileSSS", &value_S32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            pdcchDataPowerOffsetProfileSSS : %d\n", value_S32 );

                        p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo->pdcchDataPowerOffsetProfileSSS = (int16_t) value_S32;
                    }
                }
            } 
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PDCCH_PDU_Maintenance_FAPIv3) + numDlDci*sizeof(PDCCH_PDU_Maintenance_FAPIv3_DCIInfo);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "pdcchParametersFapiV4") != 0)
            {
                XML_LOAD_DEBUG( "        pdcchParametersFapiV4 :\n" );

                for (dlDciIndex = 0; dlDciIndex < numDlDci; dlDciIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numDlDci[%u] :\n", dlDciIndex );

                    p_PDCCH_PDU_FAPIv4_DCIInfo = (PDCCH_PDU_FAPIv4_DCIInfo *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDCCH_PDU_FAPIv4_DCIInfo);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "nIdPdcchDmrs", &value_U32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            nIdPdcchDmrs : %u\n", value_U32 );

                        p_PDCCH_PDU_FAPIv4_DCIInfo->nIdPdcchDmrs = (uint16_t) value_U32;
                    }
                }

                p_PDCCH_PDU_FAPIv4_MuMimo = (PDCCH_PDU_FAPIv4_MuMimo *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PDCCH_PDU_FAPIv4_MuMimo);

                if (getXmlNodeValueU32(pNode_Structure, "spatialStreamIndicesPresent", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          spatialStreamIndicesPresent : %u\n", value_U32 );

                    p_PDCCH_PDU_FAPIv4_MuMimo->spatialStreamIndicesPresent = (uint8_t) value_U32;
                }

                if (p_PDCCH_PDU_FAPIv4_MuMimo->spatialStreamIndicesPresent)
                {
                    for (dlDciIndex = 0; dlDciIndex < numDlDci; dlDciIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numDlDci[%u] :\n", dlDciIndex );

                        p_PDCCH_PDU_FAPIv4_MuMimo_DCIInfo = (PDCCH_PDU_FAPIv4_MuMimo_DCIInfo *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(PDCCH_PDU_FAPIv4_MuMimo_DCIInfo);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "dciIndex", &value_U32, dlDciIndex ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            dciIndex : %u\n", value_U32 );

                            p_PDCCH_PDU_FAPIv4_MuMimo_DCIInfo->dciIndex = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "spatialStreamIndex", &value_U32, dlDciIndex ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            spatialStreamIndex : %u\n", value_U32 );

                            p_PDCCH_PDU_FAPIv4_MuMimo_DCIInfo->spatialStreamIndex = (uint16_t) value_U32;
                        }
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += numDlDci*sizeof(PDCCH_PDU_FAPIv4_DCIInfo) + sizeof(PDCCH_PDU_FAPIv4_MuMimo) + numDlDci*sizeof(PDCCH_PDU_Maintenance_FAPIv3_DCIInfo);
            }

            XML_LOAD_DEBUG( "        pdu Length : %u\n", pduLength );
 
            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
        else if (strcasecmp( (char *) pProperty, "PDSCH" ) == 0)
        {
            XML_LOAD_DEBUG( "      PDSCH PDU\n" );

            p_pduHeader->pduType = DL_PDU_TYPE_PDSCH;
            pduCountPdsch++;

            p_PdschPdu = (PDSCH_PDU *) p_pduHeader;
            pduLength += sizeof(PDSCH_PDU);

            pduBitmap = 0;
            nrOfCodewords = 0;
            numPRGs = 0;
            digBFInterfaces = 0;
            trpScheme = 0;
            memset( digBFInterfacesForTrp, 0, sizeof(digBFInterfacesForTrp) );
            szRm = 0;
            numPrbSymRmPatternsByValue = 0;
            numCoresetRmPatterns = 0;
            szCrs = 0;
            szCsiRs = 0;
            szCrsMbsfn = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "pduBitmap", &value_U32 ) == SUCCESS)
            {
                pduBitmap = value_U32;
                XML_LOAD_DEBUG( "        pduBitmap : %u\n", value_U32 );

                p_PdschPdu->pduBitmap = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "rnti", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rnti : %u\n", value_U32 );

                p_PdschPdu->rnti = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "pduIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        pduIndex : %u\n", value_U32 );

                p_PdschPdu->pduIndex = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpSize : %u\n", value_U32 );

                p_PdschPdu->bwpSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpStart : %u\n", value_U32 );

                p_PdschPdu->bwpStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "subcarrierSpacing", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                p_PdschPdu->subcarrierSpacing = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicPrefix", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicPrefix : %u\n", value_U32 );

                p_PdschPdu->cyclicPrefix = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nrOfCodewords", &value_U32 ) == SUCCESS)
            {
                nrOfCodewords = value_U32;
                XML_LOAD_DEBUG( "        nrOfCodewords : %u\n", value_U32 );

                p_PdschPdu->nrOfCodewords = (uint16_t) value_U32;
            }

            for (codewordIndex = 0; codewordIndex < nrOfCodewords; codewordIndex++)
            {
                XML_LOAD_DEBUG( "        ForEach nrOfCodewords[%u] :\n", codewordIndex );

                p_PDSCH_CodewordInfo = (PDSCH_CodewordInfo *) &p_PdschPdu->codewordInfo[codewordIndex];

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "targetCodeRate", &value_U32, codewordIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          targetCodeRate : %u\n", value_U32 );

                    p_PDSCH_CodewordInfo->targetCodeRate = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "qamModOrder", &value_U32, codewordIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          qamModOrder : %u\n", value_U32 );

                    p_PDSCH_CodewordInfo->qamModOrder = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "mcsIndex", &value_U32, codewordIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          mcsIndex : %u\n", value_U32 );

                    p_PDSCH_CodewordInfo->mcsIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "mcsTable", &value_U32, codewordIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          mcsTable : %u\n", value_U32 );

                    p_PDSCH_CodewordInfo->mcsTable = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "rvIndex", &value_U32, codewordIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          rvIndex : %u\n", value_U32 );

                    p_PDSCH_CodewordInfo->rvIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "tbSize", &value_U32, codewordIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          tbSize : %u\n", value_U32 );

                    p_PDSCH_CodewordInfo->tbSize = value_U32;
                }
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nIdPdsch", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nIdPdsch : %u\n", value_U32 );

                p_PdschPdu->nIdPdsch = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nrOfLayers", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nrOfLayers : %u\n", value_U32 );

                p_PdschPdu->nrOfLayers = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "transmissionScheme", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        transmissionScheme : %u\n", value_U32 );

                p_PdschPdu->transmissionScheme = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "refPoint", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        refPoint : %u\n", value_U32 );

                p_PdschPdu->refPoint = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "dlDmrsSymbPos", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        dlDmrsSymbPos : %u\n", value_U32 );

                p_PdschPdu->dlDmrsSymbPos = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "dmrsConfigType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        dmrsConfigType : %u\n", value_U32 );

                p_PdschPdu->dmrsConfigType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "pdschDmrsScramblingId", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        pdschDmrsScramblingId : %u\n", value_U32 );

                p_PdschPdu->pdschDmrsScramblingId = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "pdschDmrsScramblingIdComplement", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        pdschDmrsScramblingIdComplement : %u\n", value_U32 );

                p_PdschPdu->pdschDmrsScramblingIdComplement = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "lowPaprDmrs", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        lowPaprDmrs : %u\n", value_U32 );

                p_PdschPdu->lowPaprDmrs = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nSCID", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nSCID : %u\n", value_U32 );

                p_PdschPdu->nSCID = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numDmrsCdmGrpsNoData", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        numDmrsCdmGrpsNoData : %u\n", value_U32 );

                p_PdschPdu->numDmrsCdmGrpsNoData = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "dmrsPorts", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        dmrsPorts : %u\n", value_U32 );

                p_PdschPdu->dmrsPorts = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "resourceAlloc", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        resourceAlloc : %u\n", value_U32 );

                p_PdschPdu->resourceAlloc = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32Array(pNode_Pdu, "rbBitmap", value_U32Array, &arrayCount) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rbBitmap : count:%u values:", arrayCount );
                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                XML_LOAD_DEBUG( "\n" );

                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_RB_BITMAP_SIZE; arrayIndex++)
                {
                    p_PdschPdu->rbBitmap[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                }
            }

            if (getXmlNodeValueU32(pNode_Pdu, "rbStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rbStart : %u\n", value_U32 );

                p_PdschPdu->rbStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "rbSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rbSize : %u\n", value_U32 );

                p_PdschPdu->rbSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "vrbToPrbMapping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        vrbToPrbMapping : %u\n", value_U32 );

                p_PdschPdu->vrbToPrbMapping = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "startSymbolIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        startSymbolIndex : %u\n", value_U32 );

                p_PdschPdu->startSymbolIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nrOfSymbols", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nrOfSymbols : %u\n", value_U32 );

                p_PdschPdu->nrOfSymbols = (uint8_t) value_U32;
            }

            if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED) && searchXmlNode( pNode_Pdu, &pNode_Structure, "pdschPtrsParameters" ) != 0)
            {
                XML_LOAD_DEBUG( "        pdschPtrsParameters :\n" );

                p_PDSCH_PTRS = (PDSCH_PTRS *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PDSCH_PTRS);

                if (getXmlNodeValueU32(pNode_Structure, "ptrsPortIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ptrsPortIndex : %u\n", value_U32 );

                    p_PDSCH_PTRS->ptrsPortIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ptrsTimeDensity", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ptrsTimeDensity : %u\n", value_U32 );

                    p_PDSCH_PTRS->ptrsTimeDensity = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ptrsFreqDensity", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ptrsFreqDensity : %u\n", value_U32 );

                    p_PDSCH_PTRS->ptrsFreqDensity = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ptrsReOffset", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ptrsReOffset : %u\n", value_U32 );

                    p_PDSCH_PTRS->ptrsReOffset = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "nEpreRatioOfPDSCHToPTRSProfileNR", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          nEpreRatioOfPDSCHToPTRSProfileNR : %u\n", value_U32 );

                    p_PDSCH_PTRS->nEpreRatioOfPDSCHToPTRSProfileNR = (uint8_t) value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED))
                    pduLength += sizeof(PDSCH_PTRS);
            }

            if (g_phyRel16Support == FALSE && searchXmlNode( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamforming") != 0)
            {
                XML_LOAD_DEBUG( "        PrecodingAndBeamforming :\n" );

                p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                {
                    numPRGs = value_U32;
                    XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                {
                    digBFInterfaces = value_U32;
                    XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                    p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                }

                nodeIndexOfBeamIdx = 0;
                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                    p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(uint16_t);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                        *p_Value_U16 = (uint16_t) value_U32;
                    }

                    for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                    {
                        XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }
                        nodeIndexOfBeamIdx++;
                    }
                }
            }
            else if (g_phyRel16Support && searchXmlNode( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamformingRel16") != 0)
            {
                XML_LOAD_DEBUG( "        PrecodingAndBeamformingRel16 :\n" );

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    trpScheme = value_U32;
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );
                }

                if (trpScheme == TRP_SCHEME_BASE)    // use Table 3-84
                {
                    p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                    p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                    if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                    {
                        numPRGs = value_U32;
                        XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                    {
                        digBFInterfaces = value_U32;
                        XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                    }

                    nodeIndexOfBeamIdx = 0;
                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }

                        for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                        {
                            XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                            p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(uint16_t);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                                *p_Value_U16 = (uint16_t) value_U32;
                            }
                            nodeIndexOfBeamIdx++;
                        }
                    }
                }
                else
                {
                    if (trpScheme == TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY)
                        numTrp = 2;
                    else    // TRP_SCHEME_STRP/TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY/TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY
                        numTrp = 1;

                    p_Rel16mTRP_TxPrecodingAndBeamforming_PDU = (Rel16mTRP_TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PDU);

                    p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                    if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                    {
                        numPRGs = value_U32;
                        XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array(pNode_Structure, "digBFInterfaces", value_U32Array, &arrayCount ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          digBFInterfaces : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                        {
                            digBFInterfacesForTrp[arrayIndex] = value_U32Array[arrayIndex];

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->digBFInterfaces[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    nodeIndexOfBeamIdx = 0;
                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo = (Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "layers", &value_U32, prgIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            layers : %u\n", value_U32 );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->layers = (uint8_t) value_U32;
                        }

                        if (getXmlNodeValueU32Array(pNode_Structure, "pmIdx", value_U32Array, &arrayCount ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            pmIdx : count:%u values:", arrayCount );
                            for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                            XML_LOAD_DEBUG( "\n" );

                            for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                            {
                                p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->pmIdx[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                            }
                        }

                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            XML_LOAD_DEBUG( "            ForEach numTrp[%u] :\n", trpIndex );

                            digBFInterfaces = digBFInterfacesForTrp[trpIndex];
                            for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                            {
                                XML_LOAD_DEBUG( "              ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                                pduLength += sizeof(uint16_t);

                                if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "                beamIdx : %u\n", value_U32 );

                                    *p_Value_U16 = (uint16_t) value_U32;
                                }
                                nodeIndexOfBeamIdx++;
                            }
                        }
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(TxPrecodingAndBeamforming_PDU);
            }

            p_PDSCH_TxPowerInfo = (PDSCH_TxPowerInfo *) ((uint8_t *) p_pduHeader + pduLength);
            pduLength += sizeof(PDSCH_TxPowerInfo);

            if (getXmlNodeValueU32(pNode_Pdu, "powerControlOffsetProfileNR", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        powerControlOffsetProfileNR : %u\n", value_U32 );

                p_PDSCH_TxPowerInfo->powerControlOffsetProfileNR = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "powerControlOffsetSSProfileNR", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        powerControlOffsetSSProfileNR : %u\n", value_U32 );

                p_PDSCH_TxPowerInfo->powerControlOffsetSSProfileNR = (uint8_t) value_U32;
            }

            if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_CBG_RETX_CTRL_INCLUDED) && searchXmlNode( pNode_Pdu, &pNode_Structure, "cbgReTxCtrl" ) != 0)
            {
                XML_LOAD_DEBUG( "        cbgReTxCtrl :\n" );

                p_PDSCH_CBGReTxCtlInfo = (PDSCH_CBGReTxCtlInfo *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PDSCH_CBGReTxCtlInfo);

                if (getXmlNodeValueU32(pNode_Structure, "isLastCbPresent", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          isLastCbPresent : %u\n", value_U32 );

                    p_PDSCH_CBGReTxCtlInfo->isLastCbPresent = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "isInlineTbCrcl", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          isInlineTbCrcl : %u\n", value_U32 );

                    p_PDSCH_CBGReTxCtlInfo->isInlineTbCrcl = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "dlTbCrcCW", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          dlTbCrcCW : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_OF_CORDWORD; arrayIndex++)
                    {
                        p_PDSCH_CBGReTxCtlInfo->dlTbCrcCW[arrayIndex] = value_U32Array[arrayIndex];
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_CBG_RETX_CTRL_INCLUDED))
                    pduLength += sizeof(PDSCH_CBGReTxCtlInfo);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "pdschMaintenanceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        pdschMaintenanceFapiV3 :\n" );

                p_PDSCH_Maintenance_FAPIv3 = (PDSCH_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PDSCH_Maintenance_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "pdschTransType", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          pdschTransType : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->pdschTransType = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "coresetStartPoint", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          coresetStartPoint : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->coresetStartPoint = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "initialDlBwpSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          initialDlBwpSize : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->initialDlBwpSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ldpcBaseGraph", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ldpcBaseGraph : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->ldpcBaseGraph = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "tbSizeLbrmBytes", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          tbSizeLbrmBytes : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->tbSizeLbrmBytes = value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "tbCrcRequired", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          tbCrcRequired : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->tbCrcRequired = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "ssbPdusForRateMatching", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ssbPdusForRateMatching : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < SSB_PDUS_FOR_RATE_MATCHING_SIZE; arrayIndex++)
                    {
                        p_PDSCH_Maintenance_FAPIv3->ssbPdusForRateMatching[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "ssbConfigForRateMatching", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ssbConfigForRateMatching : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->ssbConfigForRateMatching = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prbSymRmPatternBitmapSizeByReference", &value_U32 ) == SUCCESS)
                {
                    szRm = (value_U32+7)/8;
                    XML_LOAD_DEBUG( "          prbSymRmPatternBitmapSizeByReference : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->prbSymRmPatternBitmapSizeByReference = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "prbSymRateMatchPatternBitmapByReference", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prbSymRateMatchPatternBitmapByReference : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < szRm && arrayIndex < PDSCH_RATE_MATCHING_BITMAP_SIZE; arrayIndex++)
                    {
                        p_PDSCH_Maintenance_FAPIv3->prbSymRateMatchPatternBitmapByReference[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPrbSymRmPatternsByValue", &value_U32 ) == SUCCESS)
                {
                    numPrbSymRmPatternsByValue = value_U32;
                    XML_LOAD_DEBUG( "          numPrbSymRmPatternsByValue: %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->numPrbSymRmPatternsByValue = (uint8_t) value_U32;
                }
 
                for (prbSymRmPatternsByValueIndex = 0; prbSymRmPatternsByValueIndex < numPrbSymRmPatternsByValue; prbSymRmPatternsByValueIndex++)
                {
                    if (prbSymRmPatternsByValueIndex >= MAX_PDSCH_PRB_SYM_RM_PATTERNS_INFO_COUNT)
                        break;

                    XML_LOAD_DEBUG( "          ForEach numPrbSymRmPatternsByValue[%u] :\n", prbSymRmPatternsByValueIndex );

                    p_PDSCH_PrbSymRmPatternsInfo = &(p_PDSCH_Maintenance_FAPIv3->prbSymRmPatternsInfo[prbSymRmPatternsByValueIndex]);

                    if (getXmlNodeValueU32Array_Nth(pNode_Pdu, "freqDomainRB", value_U32Array, &arrayCount, prbSymRmPatternsByValueIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            freqDomainRB : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_RATEMATCH_BITMAP_SIZE; arrayIndex++)
                        {
                            p_PDSCH_PrbSymRmPatternsInfo->freqDomainRB[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "symbolsInRB", &value_U32, prbSymRmPatternsByValueIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            symbolsInRB : %u\n", value_U32 );

                        p_PDSCH_PrbSymRmPatternsInfo->symbolsInRB = (uint16_t) value_U32;
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "numCoresetRmPatterns", &value_U32 ) == SUCCESS)
                {
                    numCoresetRmPatterns = value_U32;
                    XML_LOAD_DEBUG( "          numCoresetRmPatterns : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->numCoresetRmPatterns = (uint8_t) value_U32;
                }

                for (coresetRmPatternIndex = 0; coresetRmPatternIndex < numCoresetRmPatterns; coresetRmPatternIndex++)
                {
                    if (coresetRmPatternIndex >= MAX_PDSCH_CORESET_RM_PATTERNS_INFO_COUNT)
                        break;

                    XML_LOAD_DEBUG( "          ForEach numCoresetRmPatterns[%u] :\n", coresetRmPatternIndex );

                    p_PDSCH_CoresetRmPatternsInfo = &(p_PDSCH_Maintenance_FAPIv3->coresetRmPatternsInfo[coresetRmPatternIndex]);

                    if (getXmlNodeValueU32Array_Nth(pNode_Pdu, "freqDomainResources", value_U32Array, &arrayCount, coresetRmPatternIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            freqDomainResources : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE; arrayIndex++)
                        {
                            p_PDSCH_CoresetRmPatternsInfo->freqDomainResources[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "symbolsPattern", &value_U32, coresetRmPatternIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            symbolsPattern : %u\n", value_U32 );

                        p_PDSCH_CoresetRmPatternsInfo->symbolsPattern = (uint16_t) value_U32;
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "pdcchPduIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          pdcchPduIndex : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->pdcchPduIndex = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "dciIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          dciIndex : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->dciIndex = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "lteCrsRateMatchPatternBitmapSize", &value_U32 ) == SUCCESS)
                {
                    szCrs = (value_U32 + 7) / 8;
                    XML_LOAD_DEBUG( "          lteCrsRateMatchPatternBitmapSize : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->lteCrsRateMatchPatternBitmapSize = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "lteCrsRateMatchPattern", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          lteCrsRateMatchPattern : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < szCrs && arrayIndex < MAX_LTE_CRS_RATEMATCH_PATTERN_BITMAP_SIZE; arrayIndex++)
                    {
                        p_PDSCH_Maintenance_FAPIv3->lteCrsRateMatchPattern[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "numCsiRsForRateMatching", &value_U32 ) == SUCCESS)
                {
                    szCsiRs = (value_U32 + 7) / 8;
                    XML_LOAD_DEBUG( "          numCsiRsForRateMatching : %u\n", value_U32 );

                    p_PDSCH_Maintenance_FAPIv3->numCsiRsForRateMatching = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "csiRsForRateMatching", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          csiRsForRateMatching : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < szCsiRs && arrayIndex < MAX_CSI_RS_RATEMATCH_SIZE; arrayIndex++)
                    {
                        p_PDSCH_Maintenance_FAPIv3->csiRsForRateMatching[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueS32(pNode_Structure, "pdschDmrsPowerOffsetProfileSSS", &value_S32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          pdschDmrsPowerOffsetProfileSSS : %d\n", value_S32 );

                    p_PDSCH_Maintenance_FAPIv3->pdschDmrsPowerOffsetProfileSSS = (int16_t) value_S32;
                }

                if (getXmlNodeValueS32(pNode_Structure, "pdschDataPowerOffsetProfileSSS", &value_S32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          pdschDataPowerOffsetProfileSSS : %d\n", value_S32 );

                    p_PDSCH_Maintenance_FAPIv3->pdschDataPowerOffsetProfileSSS = (int16_t) value_S32;
                }

                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_CBG_RETX_CTRL_INCLUDED)  
                  && g_phySupportL2CbgSeg 
                  && searchXmlNode( pNode_Structure, &pNode_SubStructure, "cbgReTxCtrlL2Seg" ) != 0)
                {
                    XML_LOAD_DEBUG( "        cbgReTxCtrlL2Seg :\n" );

                    p_PDSCH_CBGTxInfo = (PDSCH_CBGTxInfo *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDSCH_CBGTxInfo);

                    if (getXmlNodeValueU32(pNode_SubStructure, "maxNumCbgPerTb", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          maxNumCbgPerTb : %u\n", value_U32 );

                        p_PDSCH_CBGTxInfo->maxNumCbgPerTb = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array(pNode_SubStructure, "cbgTxInformation", value_U32Array, &arrayCount ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          cbgTxInformation : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < nrOfCodewords && arrayIndex < MAX_NUM_OF_CORDWORD; arrayIndex++)
                        {
                            p_PDSCH_CBGTxInfo->cbgTxInformation[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }
                }
                else if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_CBG_RETX_CTRL_INCLUDED)
                  && g_phySupportL1CbgSeg
                  && searchXmlNode( pNode_Structure, &pNode_SubStructure, "cbgReTxCtrlL1Seg" ) != 0)
                {
                    XML_LOAD_DEBUG( "        cbgReTxCtrlL1Seg :\n" );

                    p_PDSCH_CBGTxInfo = (PDSCH_CBGTxInfo *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDSCH_CBGTxInfo);

                    if (getXmlNodeValueU32(pNode_SubStructure, "maxNumCbgPerTb", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          maxNumCbgPerTb : %u\n", value_U32 );

                        p_PDSCH_CBGTxInfo->maxNumCbgPerTb = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array(pNode_SubStructure, "cbgTxInformation", value_U32Array, &arrayCount ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          cbgTxInformation : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < nrOfCodewords && arrayIndex < MAX_NUM_OF_CORDWORD; arrayIndex++)
                        {
                            p_PDSCH_CBGTxInfo->cbgTxInformation[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }
                }
                else
                {
                    // add length for Mandatory fields
                    if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_CBG_RETX_CTRL_INCLUDED) && (g_phySupportL2CbgSeg || g_phySupportL1CbgSeg))
                        pduLength += sizeof(PDSCH_CBGTxInfo);
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PDSCH_Maintenance_FAPIv3);

                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_CBG_RETX_CTRL_INCLUDED) && (g_phySupportL2CbgSeg || g_phySupportL1CbgSeg))
                    pduLength += sizeof(PDSCH_CBGTxInfo);
            }

            if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED) && searchXmlNode( pNode_Pdu, &pNode_Structure, "pdschPtrsMaintenanceFapiV3" ) != 0)
            {
                XML_LOAD_DEBUG( "        pdschPtrsMaintenanceFapiV3 :\n" );

                p_PDSCH_PTRS_Maintenance_FAPIv3 = (PDSCH_PTRS_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PDSCH_PTRS_Maintenance_FAPIv3);

                if (getXmlNodeValueS32(pNode_Structure, "pdschPtrsPowerOffsetProfileSSS", &value_S32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          pdschPtrsPowerOffsetProfileSSS : %d\n", value_S32 );

                    p_PDSCH_PTRS_Maintenance_FAPIv3->pdschPtrsPowerOffsetProfileSSS = (int16_t) value_S32;
                }
            }
            else
            {
                // add length for Mandatory fields
                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED))
                    pduLength += sizeof(PDSCH_PTRS_Maintenance_FAPIv3);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "pdschParametersRel16FapiV3" ) != 0)
            {
                XML_LOAD_DEBUG( "        pdschParametersRel16FapiV3 :\n" );

                p_Rel16_PDSCH_FAPIv3 = (Rel16_PDSCH_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(Rel16_PDSCH_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "repetitionScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          repetitionScheme : %u\n", value_U32 );

                    p_Rel16_PDSCH_FAPIv3->repetitionScheme = (uint8_t) value_U32;
                }

                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED) && searchXmlNode( pNode_Structure, &pNode_SubStructure, "pdschPtrs2" ) != 0)
                {
                    XML_LOAD_DEBUG( "          pdschPtrs2 :\n" );

                    p_PDSCH_PTRS_Maintenance_FAPIv3 = (PDSCH_PTRS_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDSCH_PTRS_Maintenance_FAPIv3);

                    if (getXmlNodeValueS32(pNode_Structure, "pdschPtrsPowerOffsetProfileSSS", &value_S32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          pdschPtrsPowerOffsetProfileSSS : %d\n", value_S32 );

                        p_PDSCH_PTRS_Maintenance_FAPIv3->pdschPtrsPowerOffsetProfileSSS = (int16_t) value_S32;
                    }
                }
                else
                {
                    // add length for Mandatory fields
                    if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED))
                        pduLength += sizeof(PDSCH_PTRS_Maintenance_FAPIv3);
                }

                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED) && searchXmlNode( pNode_Structure, &pNode_SubStructure, "pdschPtrs2" ) != 0)
                {
                    XML_LOAD_DEBUG( "        pdschPtrs2 :\n" );

                    p_PDSCH_PTRS = (PDSCH_PTRS *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDSCH_PTRS);

                    if (getXmlNodeValueU32(pNode_SubStructure, "ptrsPortIndex", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          ptrsPortIndex : %u\n", value_U32 );

                        p_PDSCH_PTRS->ptrsPortIndex = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_SubStructure, "ptrsTimeDensity", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          ptrsTimeDensity : %u\n", value_U32 );

                        p_PDSCH_PTRS->ptrsTimeDensity = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_SubStructure, "ptrsFreqDensity", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          ptrsFreqDensity : %u\n", value_U32 );

                        p_PDSCH_PTRS->ptrsFreqDensity = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_SubStructure, "ptrsReOffset", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          ptrsReOffset : %u\n", value_U32 );

                        p_PDSCH_PTRS->ptrsReOffset = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_SubStructure, "nEpreRatioOfPDSCHToPTRSProfileNR", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          nEpreRatioOfPDSCHToPTRSProfileNR : %u\n", value_U32 );

                        p_PDSCH_PTRS->nEpreRatioOfPDSCHToPTRSProfileNR = (uint8_t) value_U32;
                    }
                }
                else
                {
                    // add length for Mandatory fields
                    if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED))
                        pduLength += sizeof(PDSCH_PTRS);
                }

                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED) && searchXmlNode( pNode_Structure, &pNode_SubStructure, "pdschPtrsV3" ) != 0)
                {
                    XML_LOAD_DEBUG( "        pdschPtrsV3 :\n" );

                    p_PDSCH_PTRS_Maintenance_FAPIv3 = (PDSCH_PTRS_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDSCH_PTRS_Maintenance_FAPIv3);

                    if (getXmlNodeValueS32(pNode_SubStructure, "pdschPtrsPowerOffsetProfileSSS", &value_S32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          pdschPtrsPowerOffsetProfileSSS : %d\n", value_S32 );

                        p_PDSCH_PTRS_Maintenance_FAPIv3->pdschPtrsPowerOffsetProfileSSS = (int16_t) value_S32;
                    }
                }
                else
                {
                    // add length for Mandatory fields
                    if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED))
                        pduLength += sizeof(PDSCH_PTRS_Maintenance_FAPIv3);
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(Rel16_PDSCH_FAPIv3);
                if ((pduBitmap & BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED))
                    pduLength += sizeof(PDSCH_PTRS_Maintenance_FAPIv3) + sizeof(PDSCH_PTRS) + sizeof(PDSCH_PTRS_Maintenance_FAPIv3);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "pdschParametersFapiV4" ) != 0)
            {
                XML_LOAD_DEBUG( "        pdschParametersFapiV4 :\n" );

                p_PDSCH_PDU_FAPIv4 = (PDSCH_PDU_FAPIv4 *) ((uint8_t *) p_pduHeader + pduLength);
                pduLength += sizeof(PDSCH_PDU_FAPIv4);

                if (getXmlNodeValueU32(pNode_Structure, "coresetRmPatternBitmapSizeByReference", &value_U32 ) == SUCCESS)
                {
                    szRm = (value_U32 +7)/8;
                    XML_LOAD_DEBUG( "          coresetRmPatternBitmapSizeByReference : %u\n", value_U32 );

                    p_PDSCH_PDU_FAPIv4->coresetRmPatternBitmapSizeByReference = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "coresetRmPatternBitmapByReference", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          coresetRmPatternBitmapByReference : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < szRm && arrayIndex < MAX_SIZE_OF_RATE_MATCH_PATTERN; arrayIndex++)
                    {
                        p_PDSCH_PDU_FAPIv4->coresetRmPatternBitmapByReference[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "lteCrsMbsfnDerivationMethod", &value_U32 ) == SUCCESS)
                {
                    szCrsMbsfn = (value_U32 == 1) ? szCrs : 0;
                    XML_LOAD_DEBUG( "          lteCrsMbsfnDerivationMethod : %u\n", value_U32 );

                    p_PDSCH_PDU_FAPIv4->lteCrsMbsfnDerivationMethod = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "lteCrsMbsfnPattern", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          lteCrsMbsfnPattern : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < szCrsMbsfn && arrayIndex < MAX_SIZE_OF_CRS_RATE_MATCH_PATTERN_MBSFN; arrayIndex++)
                    {
                        p_PDSCH_PDU_FAPIv4->lteCrsMbsfnPattern[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "spatialStreamIndicesPresent", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          spatialStreamIndicesPresent: %u\n", value_U32 );

                    p_PDSCH_PDU_FAPIv4->spatialStreamIndicesPresent = (uint8_t) value_U32;
                }

                if (p_PDSCH_PDU_FAPIv4->spatialStreamIndicesPresent)
                {
                    for (codewordIndex = 0; codewordIndex < nrOfCodewords; codewordIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach nrOfCodewords[%u] :\n", codewordIndex );

                        p_LayerMappingcwInfo = (LayerMappingcwInfo *) &p_PDSCH_PDU_FAPIv4->layerMappiongcwInfo[codewordIndex];

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "numLayersForCW", &value_U32, codewordIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          numLayersForCW : %u\n", value_U32 );

                            p_LayerMappingcwInfo->numLayersForCW = (uint8_t) value_U32;
                        }

                        if (getXmlNodeValueU32Array_Nth(pNode_Structure, "spatialStreamIndicesForCW", value_U32Array, &arrayCount, codewordIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          spatialStreamIndicesForCW : count:%u values:", arrayCount );
                            for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                            XML_LOAD_DEBUG( "\n" );

                            for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_LAYERS_FOR_CW; arrayIndex++)
                            {
                                p_LayerMappingcwInfo->spatialStreamIndicesForCW[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                            }
                        }
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PDSCH_PDU_FAPIv4);
            }

            XML_LOAD_DEBUG( "        pdu Length : %u\n", pduLength );

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
    }

    pduCountTotal = pduCountSsb + pduCountCsiRs + pduCountPdcch + pduCountPdsch;

    p_DlTtiReq->numPDUs = pduCountTotal;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDCCH] = pduCountPdcch;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDSCH] = pduCountPdsch;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_CSI_RS] = pduCountCsiRs;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_SSB] = pduCountSsb;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_DLDCI] = pduCountDlDci;
    p_DlTtiReq->nGroup = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Finalize message

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    XML_LOAD_DEBUG( "    === Final msgLength %u\n", *p_MsgLength );

    return SUCCESS;
}

int loadTxDataReqMsg( xmlNode *pNode_FapiMsg, uint32_t msgBuffSize, uint8_t *p_MsgBuff, uint32_t *p_MsgLength, slot_config_t *pSlotConfig, uint8_t cellIndex )
{
    uint32_t fapiBodyLength, pduLength, pduIndex, value_U32;
    uint32_t pduCountTxData, pduNodeCount, numTLV, tlvIndex, lengthWithPadding, valueIndex, txDataIndex, txDataBuffIndex;
    uint32_t value_U32Array[MAX_MULTIPLE_XML_NODE_COUNT], arrayCount, arrayIndex;
    xmlNode *pNode_PduList[MAX_MULTIPLE_XML_NODE_COUNT], *pNode_Pdu;
    xmlChar *pProperty;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    Tx_DATA_REQ *p_TxDataReq;
    TxDataPduInfo *p_TxDataPduInfo;
    TBS_TLV_Struct *p_TBS_TLV_Struct;
    uint8_t *p_Value_U8;
    uint64_t *p_Value_U64;
    char *p_FileName, subFileName[MAX_FILEPATH_SIZE*2], fileLineBuff[MAX_TX_DATA_FILE_LINE_SIZE], *pToken;
    FILE *p_Fp;

    memset( p_MsgBuff, 0, msgBuffSize );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) p_MsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    *p_MsgLength = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_PHY_TX_DATA_REQ;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_TxDataReq = (Tx_DATA_REQ *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(Tx_DATA_REQ);

    p_TxDataReq->sfn = 0;
    p_TxDataReq->slot = 0;
    p_TxDataReq->numPDUs = 0;
    p_TxDataReq->controlLength = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Load PDUs

    pduCountTxData = 0;
    
    pduNodeCount = 0;

    p_TxDataPduInfo = (TxDataPduInfo *) p_TxDataReq->variablePart;

    searchXmlMultipleNodes( pNode_FapiMsg, pNode_PduList, &pduNodeCount, "PDU" );
    for (pduIndex = 0; pduIndex < pduNodeCount; pduIndex++)
    {
        pNode_Pdu = pNode_PduList[pduIndex];
        pProperty = xmlGetProp(pNode_Pdu, (const xmlChar *) "type");
        if (pProperty == NULL)
        {
            XML_LOAD_DEBUG( "    PDU %u : no type info\n", pduIndex );
            continue;
        }

        pduLength = 0;

        if (strcasecmp( (char *) pProperty, "TXDATA" ) == 0)
        {
            XML_LOAD_DEBUG( "      TXDATA PDU\n" );

            pduCountTxData++;

            pduLength += sizeof(TxDataPduInfo);

            numTLV = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "pduIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        pduIndex : %u\n", value_U32 );

                p_TxDataPduInfo->pduIndex = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cwIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cwIndex : %u\n", value_U32 );

                p_TxDataPduInfo->cwIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numTLV", &value_U32 ) == SUCCESS)
            {
                numTLV = value_U32;
                XML_LOAD_DEBUG( "        numTLV : %u\n", value_U32 );

                p_TxDataPduInfo->numTLV = value_U32;
            }

            for (tlvIndex = 0; tlvIndex < numTLV; tlvIndex++)
            {
                XML_LOAD_DEBUG( "        ForEach numTLV[%u] :\n", tlvIndex );

                p_TBS_TLV_Struct = (TBS_TLV_Struct *) ((uint8_t *)p_TxDataPduInfo + pduLength);
                pduLength += sizeof(TBS_TLV_Struct);
                p_Value_U8 = (uint8_t *) p_TBS_TLV_Struct->variablePart;

                lengthWithPadding = 0;
                valueIndex = 0;

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "tag", &value_U32, tlvIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          tag : %u\n", value_U32 );

                    p_TBS_TLV_Struct->tag = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "length", &value_U32, tlvIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          length : %u\n", value_U32 );

                    lengthWithPadding = value_U32;
                    if (value_U32 % 4 != 0)
                        lengthWithPadding += 4-(value_U32%4);
            
                    p_TBS_TLV_Struct->length = value_U32;
                }

                if (p_TBS_TLV_Struct->tag == VALUE_FIELD_PAYLOAD_VALUES)    // contains uint8_t values
                {
                    if (getXmlNodeValueU32Array_Nth(pNode_Pdu, "value", value_U32Array, &arrayCount, tlvIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          value : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        {
                            *p_Value_U8 = (uint8_t) value_U32Array[arrayIndex];
                            p_Value_U8++;
                            pduLength++;
                            valueIndex++;
                        }
                    }

                    if (valueIndex < lengthWithPadding)
                    {
                        p_Value_U8 += lengthWithPadding-valueIndex;
                        pduLength += lengthWithPadding-valueIndex;
                    }
                }
                else if (p_TBS_TLV_Struct->tag == VALUE_FIELD_POINTER_TO_PAYLOAD)    // contains filename 
                {
                    txDataIndex = pSlotConfig->txDataCount;
                    pSlotConfig->txDataCount++;

                    pSlotConfig->txDataSize[txDataIndex] = p_TBS_TLV_Struct->length;

                    if ((p_FileName = getXmlNodeValueStr_Nth(pNode_Pdu, "value", tlvIndex)) != NULL)
                    {
                        sprintf( subFileName, "%s/%s", g_macRunnerConfig.basePath, p_FileName );
                        XML_LOAD_DEBUG( "          value : %s\n", subFileName );

                        p_Value_U64 = (uint64_t *) p_Value_U8;
                        *p_Value_U64 = (uint64_t) txDataIndex;
                        p_Value_U8 += sizeof(uint64_t);
                        pduLength += sizeof(uint64_t);

                        if ((p_Fp = fopen( subFileName, "rt")) != NULL)
                        {
                            XML_LOAD_DEBUG( "          data :" );

                            // load text file contents to buff
                            txDataBuffIndex = 0;
                            while (txDataBuffIndex < p_TBS_TLV_Struct->length && fgets( fileLineBuff, MAX_TX_DATA_FILE_LINE_SIZE, p_Fp ) != NULL)
                            { 
                                pToken = strtok(fileLineBuff, " \t,;\n\r");
                                while (pToken != NULL)
                                {
                                    if (strncasecmp( (char *)pToken, "0x", 2) == 0)
                                        value_U32 = (uint32_t) strtol((char *)pToken, NULL, 16 );
                                    else
                                        value_U32 = (uint32_t) strtol((char *)pToken, NULL, 10 );

                                    pSlotConfig->txDataBuff[txDataIndex][txDataBuffIndex] = (uint8_t) value_U32;
                                    txDataBuffIndex++; 

                                    if (p_TBS_TLV_Struct->length < 41)
                                    {
                                        XML_LOAD_DEBUG( " %u", (uint8_t) value_U32 );
                                    }
                                    else
                                    {
                                        if (txDataBuffIndex == 20)
                                        {
                                            XML_LOAD_DEBUG( " ..." );
                                        }
                                        else if (txDataBuffIndex < 20 || txDataBuffIndex > p_TBS_TLV_Struct->length-20)
                                        {
                                            XML_LOAD_DEBUG( " %u", (uint8_t) value_U32 );
                                        }
                                    }

                                    if (txDataBuffIndex >= p_TBS_TLV_Struct->length)
                                        break;

                                    pToken = strtok(NULL, " \t,;\n\r");
                                }
                            }
                            XML_LOAD_DEBUG( "\n" );

                            fclose( p_Fp );
                        } 
                        else
                        {
                            XML_LOAD_WARNING( "%s : File open failed for tlvIndex %u\n", subFileName, tlvIndex );
                        }
                    }
                }
            }

            p_TxDataPduInfo->pduLength = pduLength;
            p_TxDataPduInfo = (TxDataPduInfo *) ((uint8_t *) p_TxDataPduInfo + pduLength);
            fapiBodyLength += pduLength;
        }
    }

    p_TxDataReq->numPDUs = pduCountTxData;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Finalize message

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    XML_LOAD_DEBUG( "    MSG_PHY_TX_DATA_REQ loaded : msgLength %u\n", *p_MsgLength );

    return SUCCESS;
}

int loadUlDciReqMsg( xmlNode *pNode_FapiMsg, uint32_t msgBuffSize, uint8_t *p_MsgBuff, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength, pduLength, pduIndex, value_U32;
    uint32_t pduCountPdcch, pduCountUlDci, pduCountTotal, pduNodeCount;
    uint32_t value_U32Array[MAX_MULTIPLE_XML_NODE_COUNT], arrayCount, arrayIndex;
    uint32_t numPRGs, prgIndex, digBFInterfaces, digBFInterfacesIndex;
    uint32_t trpScheme, trpIndex, numTrp, nodeIndexOfBeamIdx, numMuMimoGroups, muMimoGroupIndex;
    uint32_t numDlDci, dlDciIndex;
    uint32_t digBFInterfacesForTrp[MAX_NUM_TRP];
    int32_t value_S32;
    xmlNode *pNode_PduList[MAX_MULTIPLE_XML_NODE_COUNT], *pNode_Pdu, *pNode_Structure;
    xmlChar *pProperty;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    UL_DCI_REQ *p_ULDciReq;
    PDU_Struct *p_pduHeader;
    PDCCH_PDU *p_PdcchPdu;
    TxPrecodingAndBeamforming_PDU *p_TxPrecodingAndBeamforming_PDU;
    Rel16mTRP_TxPrecodingAndBeamforming_PDU *p_Rel16mTRP_TxPrecodingAndBeamforming_PDU;
    Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo;
    Tx_Generalized_Precoding_PDU *p_Tx_Generalized_Precoding_PDU;
    Tx_Generalized_Precoding_MuMimoGroupInfo *p_Tx_Generalized_Precoding_MuMimoGroupInfo;
    PDCCH_PDU_Maintenance_FAPIv3 *p_PDCCH_PDU_Maintenance_FAPIv3;
    PDCCH_PDU_Maintenance_FAPIv3_DCIInfo *p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo;
    PDCCH_PDU_FAPIv4_DCIInfo *p_PDCCH_PDU_FAPIv4_DCIInfo;
    PDCCH_PDU_FAPIv4_MuMimo *p_PDCCH_PDU_FAPIv4_MuMimo;
    PDCCH_PDU_FAPIv4_MuMimo_DCIInfo *p_PDCCH_PDU_FAPIv4_MuMimo_DCIInfo;
    DL_DCI_PDU_part1 *p_DL_DCI_PDU_part1;
    DL_DCI_PDU_part2 *p_DL_DCI_PDU_part2;
    uint16_t *p_Value_U16;

    XML_LOAD_DEBUG( "    MSG_PHY_UL_TTI_REQ\n" );

    memset( p_MsgBuff, 0, msgBuffSize );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) p_MsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    *p_MsgLength = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_PHY_UL_DCI_REQ;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_ULDciReq = (UL_DCI_REQ *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(UL_DCI_REQ);

    p_ULDciReq->sfn = 0;
    p_ULDciReq->slot = 0;
    p_ULDciReq->numPDUs = 0;
    p_ULDciReq->nDlTypes = NUM_OF_ULDCIPDU_TYPE;
    p_ULDciReq->nPDUsOfEachType[ULDCI_PDU_TYPE_INDEX_PDCCH] = 0;
    p_ULDciReq->nPDUsOfEachType[ULDCI_PDU_TYPE_INDEX_ULDCI] = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Load PDUs

    pduCountPdcch = pduCountTotal = 0;
    pduCountUlDci = 0;

    pduNodeCount = 0;

    p_pduHeader = (PDU_Struct *) p_ULDciReq->variablePart;

    searchXmlMultipleNodes( pNode_FapiMsg, pNode_PduList, &pduNodeCount, "PDU" );
    for (pduIndex = 0; pduIndex < pduNodeCount; pduIndex++)
    {
        pNode_Pdu = pNode_PduList[pduIndex];
        pProperty = xmlGetProp(pNode_Pdu, (const xmlChar *) "type");
        if (pProperty == NULL)
        {
            XML_LOAD_DEBUG( "    PDU %u : no type info\n", pduIndex );
            continue;
        }

        pduLength = 0;

        if (strcasecmp( (char *) pProperty, "PDCCH" ) == 0)
        {
            XML_LOAD_DEBUG( "      PDCCH PDU\n" );

            p_pduHeader->pduType = DL_PDU_TYPE_PDCCH;
            pduCountPdcch++;

            p_PdcchPdu = (PDCCH_PDU *) p_pduHeader;
            pduLength += sizeof(PDCCH_PDU);

            numDlDci = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "coresetBWPSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        coresetBWPSize : %u\n", value_U32 );

                p_PdcchPdu->coresetBWPSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "coresetBWPStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        coresetBWPStart : %u\n", value_U32 );

                p_PdcchPdu->coresetBWPStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "subcarrierSpacing", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                p_PdcchPdu->subcarrierSpacing = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicPrefix", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicPrefix : %u\n", value_U32 );

                p_PdcchPdu->cyclicPrefix = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "startSymbolIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        startSymbolIndex : %u\n", value_U32 );

                p_PdcchPdu->startSymbolIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "durationSymbols", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        durationSymbols : %u\n", value_U32 );

                p_PdcchPdu->durationSymbols = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32Array(pNode_Pdu, "freqDomainResource", value_U32Array, &arrayCount ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        freqDomainResource : count:%u values:", arrayCount );
                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                XML_LOAD_DEBUG( "\n" );

                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE; arrayIndex++)
                {
                    p_PdcchPdu->freqDomainResource[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                }
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cceRegMappingType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cceRegMappingType : %u\n", value_U32 );

                p_PdcchPdu->cceRegMappingType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "regBundleSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        regBundleSize : %u\n", value_U32 );

                p_PdcchPdu->regBundleSize = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "interleaverSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        interleaverSize : %u\n", value_U32 );

                p_PdcchPdu->interleaverSize = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "coreSetType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        coreSetType : %u\n", value_U32 );

                p_PdcchPdu->coreSetType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "shiftIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        shiftIndex : %u\n", value_U32 );

                p_PdcchPdu->shiftIndex = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "precoderGranularity", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        precoderGranularity: %u\n", value_U32 );

                p_PdcchPdu->precoderGranularity = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numDlDci", &value_U32 ) == SUCCESS)
            {
                numDlDci = value_U32;
                XML_LOAD_DEBUG( "        numDlDci : %u\n", value_U32 );

                p_PdcchPdu->numDlDci = (uint16_t) value_U32;
            }

            for (dlDciIndex = 0; dlDciIndex < numDlDci; dlDciIndex++)
            {
                XML_LOAD_DEBUG( "        ForEach numDlDci[%u] :\n", dlDciIndex );

                p_DL_DCI_PDU_part1 = (DL_DCI_PDU_part1 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(DL_DCI_PDU_part1);

                numPRGs = 0;
                digBFInterfaces = 0;
                trpScheme = 0;
                memset( digBFInterfacesForTrp, 0, sizeof(digBFInterfacesForTrp) );
                numMuMimoGroups = 0;

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "rnti", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          rnti : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->rnti = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "nIdPdcchData", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          nIdPdcchData : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->nIdPdcchData = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "nRntiPdcchData", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          nRntiPdcchData : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->nRntiPdcchData = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "cceIndex", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          cceIndex : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->cceIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "aggregationLevel", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          aggregationLevel : %u\n", value_U32 );

                    p_DL_DCI_PDU_part1->aggregationLevel = (uint8_t) value_U32;
                }

                if (g_phyRel16Support == FALSE && searchXmlNode_Nth( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamforming", dlDciIndex ) != 0)
                {
                    XML_LOAD_DEBUG( "        PrecodingAndBeamforming :\n" );

                    p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                    if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                    {
                        numPRGs = value_U32;
                        XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                    {
                        digBFInterfaces = value_U32;
                        XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                        p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                    }

                    nodeIndexOfBeamIdx = 0;
                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }

                        for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                        {
                            XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                            p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(uint16_t);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                                *p_Value_U16 = (uint16_t) value_U32;
                            }
                            nodeIndexOfBeamIdx++;
                        }
                    }
                }
                else if (g_phyRel16Support && searchXmlNode_Nth( pNode_Pdu, &pNode_Structure, "PrecodingAndBeamformingRel16", dlDciIndex ) != 0)
                {
                    XML_LOAD_DEBUG( "        PrecodingAndBeamformingRel16 :\n" );

                    if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                    {
                        trpScheme = value_U32;
                        XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );
                    }

                    if (trpScheme == TRP_SCHEME_BASE)    // use Table 3-84
                    {
                        p_TxPrecodingAndBeamforming_PDU = (TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(TxPrecodingAndBeamforming_PDU);

                        p_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                        if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                        {
                            numPRGs = value_U32;
                            XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                            p_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                            p_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32(pNode_Structure, "digBFInterfaces", &value_U32 ) == SUCCESS)
                        {
                            digBFInterfaces = value_U32;
                            XML_LOAD_DEBUG( "          digBFInterfaces : %u\n", value_U32 );

                            p_TxPrecodingAndBeamforming_PDU->digBFInterfaces = (uint8_t) value_U32;
                        }

                        nodeIndexOfBeamIdx = 0;
                        for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                        {
                            XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                            p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(uint16_t);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "pmIdx", &value_U32, prgIndex) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "            pmIdx : %u\n", value_U32 );

                                *p_Value_U16 = (uint16_t) value_U32;
                            }

                            for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                            {
                                XML_LOAD_DEBUG( "            ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                                pduLength += sizeof(uint16_t);

                                if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                                    *p_Value_U16 = (uint16_t) value_U32;
                                }
                                nodeIndexOfBeamIdx++;
                            }
                        }
                    }
                    else
                    {
                        if (trpScheme == TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY)
                            numTrp = 2;
                        else    // TRP_SCHEME_STRP/TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY/TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY
                            numTrp = 1;

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU = (Rel16mTRP_TxPrecodingAndBeamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PDU);

                        p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->trpScheme = (uint8_t) trpScheme;

                        if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                        {
                            numPRGs = value_U32;
                            XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->numPRGs = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->prgSize = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32Array(pNode_Structure, "digBFInterfaces", value_U32Array, &arrayCount ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          digBFInterfaces : count:%u values:", arrayCount );
                            for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                            XML_LOAD_DEBUG( "\n" );

                            for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                            {
                                digBFInterfacesForTrp[arrayIndex] = value_U32Array[arrayIndex];

                                p_Rel16mTRP_TxPrecodingAndBeamforming_PDU->digBFInterfaces[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                            }
                        }
                        nodeIndexOfBeamIdx = 0;
                        for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                        {
                            XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                            p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo = (Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *) ((uint8_t *)p_pduHeader + pduLength);
                            pduLength += sizeof(Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo);

                            if (getXmlNodeValueU32_Nth(pNode_Structure, "layers", &value_U32, prgIndex) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "            layers : %u\n", value_U32 );

                                p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->layers = (uint8_t) value_U32;
                            }

                            if (getXmlNodeValueU32Array(pNode_Structure, "pmIdx", value_U32Array, &arrayCount ) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "            pmIdx : count:%u values:", arrayCount );
                                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                                XML_LOAD_DEBUG( "\n" );

                                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_TRP; arrayIndex++)
                                {
                                    p_Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo->pmIdx[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                                }
                            }

                            for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                            {
                                XML_LOAD_DEBUG( "            ForEach numTrp[%u] :\n", trpIndex );

                                digBFInterfaces = digBFInterfacesForTrp[trpIndex];
                                for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                                {
                                    XML_LOAD_DEBUG( "              ForEach digBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                    p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                                    pduLength += sizeof(uint16_t);

                                    if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                                    {
                                        XML_LOAD_DEBUG( "                beamIdx : %u\n", value_U32 );

                                        *p_Value_U16 = (uint16_t) value_U32;
                                    }
                                    nodeIndexOfBeamIdx++;
                                }
                            }
                        }
                    }
                }
                else
                {
                    // add length for Mandatory fields
                    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);
                }

                if (g_phyFapiV4MuMimoGroupSupport && searchXmlNode_Nth( pNode_Pdu, &pNode_Structure, "TxGenPrecodingPdu", dlDciIndex ) != 0)
                {
                    XML_LOAD_DEBUG( "        TxGenPrecodingPdu :\n" );

                    p_Tx_Generalized_Precoding_PDU = (Tx_Generalized_Precoding_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(Tx_Generalized_Precoding_PDU);

                    if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                        p_Tx_Generalized_Precoding_PDU->trpScheme = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32(pNode_Structure, "numMuMimoGroups", &value_U32 ) == SUCCESS)
                    {
                        numMuMimoGroups = value_U32;
                        XML_LOAD_DEBUG( "          numMuMimoGroups : %u\n", value_U32 );

                        p_Tx_Generalized_Precoding_PDU->numMuMimoGroups = (uint8_t) value_U32;
                    }

                    for (muMimoGroupIndex = 0; muMimoGroupIndex < numMuMimoGroups; muMimoGroupIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numMuMimoGroups[%u] :\n", muMimoGroupIndex );

                        p_Tx_Generalized_Precoding_MuMimoGroupInfo = (Tx_Generalized_Precoding_MuMimoGroupInfo *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(Tx_Generalized_Precoding_MuMimoGroupInfo);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "muMimoGroupIdx", &value_U32, muMimoGroupIndex ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            prachRootSequenceIndex : %u\n", value_U32 );

                            p_Tx_Generalized_Precoding_MuMimoGroupInfo->muMimoGroupIdx = (uint16_t) value_U32;
                        }
                    }
                }
                else
                {
                    // add length for Mandatory fields
                    if (g_phyFapiV4MuMimoGroupSupport)
                        pduLength += sizeof(Tx_Generalized_Precoding_PDU);
                }

                p_DL_DCI_PDU_part2 = (DL_DCI_PDU_part2 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(DL_DCI_PDU_part2);

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "beta_PDCCH_1_0", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        beta_PDCCH_1_0 : %u\n", value_U32 );

                    p_DL_DCI_PDU_part2->beta_PDCCH_1_0 = (uint8_t) value_U32;
                }

                if (getXmlNodeValueS32_Nth(pNode_Pdu, "powerControlOffsetSSProfileNR", &value_S32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        powerControlOffsetSSProfileNR : %d\n", value_S32 );

                    p_DL_DCI_PDU_part2->powerControlOffsetSSProfileNR = (int8_t) value_S32;
                }

                if (getXmlNodeValueU32_Nth(pNode_Pdu, "payloadSizeBits", &value_U32, dlDciIndex ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        payloadSizeBits : %u\n", value_U32 );

                    p_DL_DCI_PDU_part2->payloadSizeBits = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32Array_Nth(pNode_Pdu, "payload", value_U32Array, &arrayCount, dlDciIndex) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "        payload : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < DCI_PAYLOAD_BTYE_LEN; arrayIndex++)
                    {
                        p_DL_DCI_PDU_part2->payload[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                pduCountUlDci++;
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "pdcchMaintenanceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        pdcchMaintenanceFapiV3 :\n" );

                p_PDCCH_PDU_Maintenance_FAPIv3 = (PDCCH_PDU_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PDCCH_PDU_Maintenance_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "pdcchPduIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          pdcchPduIndex : %u\n", value_U32 );

                    p_PDCCH_PDU_Maintenance_FAPIv3->pdcchPduIndex = (uint16_t) value_U32;
                }

                for (dlDciIndex = 0; dlDciIndex < numDlDci; dlDciIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numDlDci[%u] :\n", dlDciIndex );

                    p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo = (PDCCH_PDU_Maintenance_FAPIv3_DCIInfo *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDCCH_PDU_Maintenance_FAPIv3_DCIInfo);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "dciIndex", &value_U32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            dciIndex : %u\n", value_U32 );

                        p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo->dciIndex = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "collocatedAl16Candidate", &value_U32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            collocatedAl16Candidate : %u\n", value_U32 );

                        p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo->collocatedAl16Candidate = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueS32_Nth(pNode_Structure, "pdcchDmrsPowerOffsetProfileSSS", &value_S32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            pdcchDmrsPowerOffsetProfileSSS : %d\n", value_S32 );

                        p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo->pdcchDmrsPowerOffsetProfileSSS = (int16_t) value_S32;
                    }

                    if (getXmlNodeValueS32_Nth(pNode_Structure, "pdcchDataPowerOffsetProfileSSS", &value_S32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            pdcchDataPowerOffsetProfileSSS : %d\n", value_S32 );

                        p_PDCCH_PDU_Maintenance_FAPIv3_DCIInfo->pdcchDataPowerOffsetProfileSSS = (int16_t) value_S32;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PDCCH_PDU_Maintenance_FAPIv3) + numDlDci*sizeof(PDCCH_PDU_Maintenance_FAPIv3_DCIInfo);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "pdcchParametersFapiV4") != 0)
            {
                XML_LOAD_DEBUG( "        pdcchParametersFapiV4 :\n" );

                for (dlDciIndex = 0; dlDciIndex < numDlDci; dlDciIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numDlDci[%u] :\n", dlDciIndex );

                    p_PDCCH_PDU_FAPIv4_DCIInfo = (PDCCH_PDU_FAPIv4_DCIInfo *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(PDCCH_PDU_FAPIv4_DCIInfo);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "nIdPdcchDmrs", &value_U32, dlDciIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            nIdPdcchDmrs : %u\n", value_U32 );

                        p_PDCCH_PDU_FAPIv4_DCIInfo->nIdPdcchDmrs = (uint16_t) value_U32;
                    }
                }

                p_PDCCH_PDU_FAPIv4_MuMimo = (PDCCH_PDU_FAPIv4_MuMimo *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PDCCH_PDU_FAPIv4_MuMimo);

                if (getXmlNodeValueU32(pNode_Structure, "spatialStreamIndicesPresent", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          spatialStreamIndicesPresent : %u\n", value_U32 );

                    p_PDCCH_PDU_FAPIv4_MuMimo->spatialStreamIndicesPresent = (uint8_t) value_U32;
                }

                if (p_PDCCH_PDU_FAPIv4_MuMimo->spatialStreamIndicesPresent)
                {
                    for (dlDciIndex = 0; dlDciIndex < numDlDci; dlDciIndex++)
                    {
                        XML_LOAD_DEBUG( "          ForEach numDlDci[%u] :\n", dlDciIndex );

                        p_PDCCH_PDU_FAPIv4_MuMimo_DCIInfo = (PDCCH_PDU_FAPIv4_MuMimo_DCIInfo *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(PDCCH_PDU_FAPIv4_MuMimo_DCIInfo);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "dciIndex", &value_U32, dlDciIndex ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            dciIndex : %u\n", value_U32 );

                            p_PDCCH_PDU_FAPIv4_MuMimo_DCIInfo->dciIndex = (uint16_t) value_U32;
                        }

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "spatialStreamIndex", &value_U32, dlDciIndex ) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "            spatialStreamIndex : %u\n", value_U32 );

                            p_PDCCH_PDU_FAPIv4_MuMimo_DCIInfo->spatialStreamIndex = (uint16_t) value_U32;
                        }
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += numDlDci*sizeof(PDCCH_PDU_FAPIv4_DCIInfo) + sizeof(PDCCH_PDU_FAPIv4_MuMimo) + numDlDci*sizeof(PDCCH_PDU_Maintenance_FAPIv3_DCIInfo);
            }

            XML_LOAD_DEBUG( "        pdu Length : %u\n", pduLength );

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
    }

    pduCountTotal = pduCountPdcch;

    p_ULDciReq->numPDUs = pduCountTotal;
    p_ULDciReq->nPDUsOfEachType[ULDCI_PDU_TYPE_INDEX_PDCCH] = pduCountPdcch;
    p_ULDciReq->nPDUsOfEachType[ULDCI_PDU_TYPE_INDEX_ULDCI] = pduCountUlDci;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Finalize message

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    XML_LOAD_DEBUG( "    MSG_PHY_UL_DCI_REQ loaded : msgLength %u\n", *p_MsgLength );

    return SUCCESS;
}

int loadUlTtiReqMsg( xmlNode *pNode_FapiMsg, uint32_t msgBuffSize, uint8_t *p_MsgBuff, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength, pduLength, pduIndex, value_U32;
    uint32_t pduCountPrach, pduCountPusch, pduCountPucch, pduCountPucchFormat01, pduCountPucchFormat234, pduCountSrs;
    uint32_t pduCountMsgAPusch, pduCountTotal, pduNodeCount;
    uint32_t numPRGs, prgIndex, digBFInterface, digBFInterfaceIndex, nodeIndexOfBeamIdx, isMsgAPrach, hasMsgAPuschBeamforming;
    uint32_t pduBitmap, numPtrsPorts, ptrsPortsIndex, numPart2s, part2sIndex, numSymbols, symbolIndex, nUsage, numULSpatialStreamsPorts;
    uint32_t indexFdRa, numFdRa, prachFdOccasionIndex, numPrachOcas, prachTdOccasionIndex, msgAPrachMappingConfigIndex;
    uint32_t preambleGroups, grpIndex, grpStart, grpEnd, catIndex, msgAPrachMappingConfigCatIndex;
    uint32_t value_U32Array[MAX_MULTIPLE_XML_NODE_COUNT], arrayCount, arrayIndex;
    xmlNode *pNode_PduList[MAX_MULTIPLE_XML_NODE_COUNT], *pNode_Pdu, *pNode_Structure, *pNode_SubStructure;
    xmlChar *pProperty;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    UL_TTI_REQ *p_UlTtiReq;
    PDU_Struct *p_pduHeader;
    PRACH_PDU *p_PrachPdu;
    PUSCH_PDU *p_PuschPdu;
    PUCCH_PDU *p_PucchPdu;
    SRS_PDU *p_SrsPdu;
    Rx_Beamforming_PDU *p_Rx_Beamforming_PDU;
    PRACH_Maintenance_FAPIv3 *p_PRACH_Maintenance_FAPIv3;
    UplinkSpatialStreamAssignment_FAPIv4 *p_UplinkSpatialStreamAssignment_FAPIv4;
    MsgAPrachToMsgAPuschMap_FAPIv4 *p_MsgAPrachToMsgAPuschMap_FAPIv4;
    MSGA_PRACH_Mapping_Config *p_MSGA_PRACH_Mapping_Config;
    MSGA_PUSCH_PDU *p_MsgAPuschPdu;
    PuschDataInfo *p_PuschDataInfo;
    PuschUCIInfo *p_PuschUCIInfo;
    PuschPTRSInfo *p_PuschPTRSInfo;
    DftsOfdmInfo *p_DftsOfdmInfo;
    PUSCH_Maintenance_FAPIv3 *p_PUSCH_Maintenance_FAPIv3;
    PUSCH_FAPIv4 *p_PUSCH_FAPIv4; 
    PuschUCIInfo_FAPIv3 *p_PuschUCIInfo_FAPIv3;
    UCIPart2Info_FAPIv3 *p_UCIPart2Info_FAPIv3;
    PUCCH_PDU_Maintenance_FAPIv3 *p_PUCCH_PDU_Maintenance_FAPIv3;
    SRS_PDU_FAPIv4 *p_SRS_PDU_FAPIv4;
    SRS_PDU_FAPIv4_SymbolInfo *p_SRS_PDU_FAPIv4_SymbolInfo;
    uint16_t *p_Value_U16;

    XML_LOAD_DEBUG( "    MSG_PHY_UL_TTI_REQ\n" );

    memset( p_MsgBuff, 0, msgBuffSize );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set FAPI API Header

    p_FapiApiHeader = (Fapi_API_Header *) p_MsgBuff;
    p_FapiApiHeader->numOfMessages = 1;
    p_FapiApiHeader->opaqueHandle = 0;
    p_FapiApiHeader->pNext = NULL;
    p_FapiApiHeader->cellIndex = cellIndex;
    p_FapiApiHeader->flags = 0;
    *p_MsgLength = sizeof(Fapi_API_Header);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set the first FAPI message

    p_FapiGenericHeader = (Fapi_Generic_Header *) (p_FapiApiHeader+1);
    p_FapiGenericHeader->type = MSG_PHY_UL_TTI_REQ;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_UlTtiReq = (UL_TTI_REQ *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(UL_TTI_REQ);

    p_UlTtiReq->sfn = 0;
    p_UlTtiReq->slot = 0;
    p_UlTtiReq->numPDUs = 0;
    p_UlTtiReq->nUlTypes = NUM_OF_ULPDU_TYPE;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_PRACH] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_PUSCH] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_FORMAT01_PUCCH] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_FORMAT234_PUCCH] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_SRS] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_MSGA_PUSCH] = 0;
    p_UlTtiReq->nGroup = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Load PDUs

    pduCountPrach = pduCountPusch = pduCountPucch = pduCountSrs = pduCountMsgAPusch = pduCountTotal = 0;
    pduCountPucchFormat01 = pduCountPucchFormat234 = 0;

    pduNodeCount = 0;

    p_pduHeader = (PDU_Struct *) p_UlTtiReq->variablePart;

    searchXmlMultipleNodes( pNode_FapiMsg, pNode_PduList, &pduNodeCount, "PDU" );
    for (pduIndex = 0; pduIndex < pduNodeCount; pduIndex++)
    {
        pNode_Pdu = pNode_PduList[pduIndex];
        pProperty = xmlGetProp(pNode_Pdu, (const xmlChar *) "type");
        if (pProperty == NULL)
        {
            XML_LOAD_DEBUG( "    PDU %u : no type info\n", pduIndex );
            continue;
        }

        pduLength = 0;

        if (strcasecmp( (char *) pProperty, "PRACH" ) == 0)
        {
            XML_LOAD_DEBUG( "      PRACH PDU\n" );

            p_pduHeader->pduType = UL_PDU_TYPE_PRACH;
            pduCountPrach++;

            p_PrachPdu = (PRACH_PDU *) p_pduHeader;
            pduLength += sizeof(PRACH_PDU);

            numPRGs = 0;
            digBFInterface = 0;
            isMsgAPrach = 0;
            hasMsgAPuschBeamforming = 0;
            numPrachOcas = 0;
            indexFdRa = 0;
            numFdRa = 0;
            preambleGroups = 0;
            grpStart = 0;
            grpEnd = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "physCellID", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        physCellID : %u\n", value_U32 );

                p_PrachPdu->physCellID = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numPrachOcas", &value_U32 ) == SUCCESS)
            {
                numPrachOcas = value_U32;
                XML_LOAD_DEBUG( "        numPrachOcas : %u\n", value_U32 );

                p_PrachPdu->numPrachOcas = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "prachFormat", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        prachFormat : %u\n", value_U32 );

                p_PrachPdu->prachFormat = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "indexFdRa", &value_U32 ) == SUCCESS)
            {
                indexFdRa = value_U32;
                XML_LOAD_DEBUG( "        indexFdRa : %u\n", value_U32 );

                p_PrachPdu->indexFdRa = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "prachStartSymbol", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        prachStartSymbol : %u\n", value_U32 );

                p_PrachPdu->prachStartSymbol = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numCs", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        numCs : %u\n", value_U32 );

                p_PrachPdu->numCs = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "isMsgAPrach", &value_U32 ) == SUCCESS)
            {
                isMsgAPrach = value_U32;
                XML_LOAD_DEBUG( "        isMsgAPrach : %u\n", value_U32 );

                p_PrachPdu->isMsgAPrach = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "hasMsgAPuschBeamforming", &value_U32 ) == SUCCESS)
            {
                hasMsgAPuschBeamforming = value_U32;
                XML_LOAD_DEBUG( "        hasMsgAPuschBeamforming : %u\n", value_U32 );

                p_PrachPdu->hasMsgAPuschBeamforming = (uint8_t) value_U32;
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "rxBeamforming") != 0)
            {
                XML_LOAD_DEBUG( "        rxBeamforming :\n" );

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(Rx_Beamforming_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                {
                    numPRGs = value_U32;
                    XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->numPRGs = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "digBFInterface", &value_U32 ) == SUCCESS)
                {
                    digBFInterface = value_U32;
                    XML_LOAD_DEBUG( "          digBFInterface : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->digBFInterface = (uint8_t) value_U32;
                }

                nodeIndexOfBeamIdx = 0;
                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterface; digBFInterfaceIndex++)
                    {
                        XML_LOAD_DEBUG( "            ForEach digBFInterface[%u] :\n", digBFInterfaceIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }
                        nodeIndexOfBeamIdx++;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(Rx_Beamforming_PDU);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "prachMaintenanceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        prachMaintenanceFapiV3 :\n" );

                p_PRACH_Maintenance_FAPIv3 = (PRACH_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PRACH_Maintenance_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "handle", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          handle : %u\n", value_U32 );

                    p_PRACH_Maintenance_FAPIv3->handle = value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prachConfigScope", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prachConfigScope: %u\n", value_U32 );

                    p_PRACH_Maintenance_FAPIv3->prachConfigScope = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prachResConfigIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prachResConfigIndex : %u\n", value_U32 );

                    p_PRACH_Maintenance_FAPIv3->prachResConfigIndex = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numFdRa", &value_U32 ) == SUCCESS)
                {
                    numFdRa = value_U32;
                    XML_LOAD_DEBUG( "          numFdRa : %u\n", value_U32 );

                    p_PRACH_Maintenance_FAPIv3->numFdRa = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "startPreambleIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          startPreambleIndex : %u\n", value_U32 );

                    p_PRACH_Maintenance_FAPIv3->startPreambleIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPreambleIndices", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          numPreambleIndices : %u\n", value_U32 );

                    p_PRACH_Maintenance_FAPIv3->numPreambleIndices = (uint8_t) value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PRACH_Maintenance_FAPIv3);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "uplinkSpatialStreamAssignmentFapiV4") != 0)
            {
                XML_LOAD_DEBUG( "        uplinkSpatialStreamAssignmentFapiV4 :\n" );

                p_UplinkSpatialStreamAssignment_FAPIv4 = (UplinkSpatialStreamAssignment_FAPIv4 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(UplinkSpatialStreamAssignment_FAPIv4);

                if (getXmlNodeValueU32(pNode_Structure, "numSpatialStreams", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          numSpatialStreams : %u\n", value_U32 );

                    p_UplinkSpatialStreamAssignment_FAPIv4->numSpatialStreams = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "spatialStreamIndices", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          spatialStreamIndices : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_UL_SPATIAL_STREAMS; arrayIndex++)
                    {
                        p_UplinkSpatialStreamAssignment_FAPIv4->spatialStreamIndices[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(UplinkSpatialStreamAssignment_FAPIv4);
            }

            if (isMsgAPrach && searchXmlNode( pNode_Pdu, &pNode_Structure, "msgAPrachToMsgAPuschMapFapiV4") != 0)
            {
                XML_LOAD_DEBUG( "        msgAPrachToMsgAPuschMapFapiV4 :\n" );

                p_MsgAPrachToMsgAPuschMap_FAPIv4 = (MsgAPrachToMsgAPuschMap_FAPIv4 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(MsgAPrachToMsgAPuschMap_FAPIv4);

                if (getXmlNodeValueU32(pNode_Structure, "preambleGroups", &value_U32 ) == SUCCESS)
                {
                    preambleGroups = value_U32;
                    XML_LOAD_DEBUG( "          preambleGroups : %u\n", value_U32 );

                    p_MsgAPrachToMsgAPuschMap_FAPIv4->preambleGroups = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "dmrsPorts", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          dmrsPorts : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        p_MsgAPrachToMsgAPuschMap_FAPIv4->dmrsPorts[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "availableDmrsSequenceIds", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          availableDmrsSequenceIds : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        p_MsgAPrachToMsgAPuschMap_FAPIv4->availableDmrsSequenceIds[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "numPuschOcasFd", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          numPuschOcasFd : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        p_MsgAPrachToMsgAPuschMap_FAPIv4->numPuschOcasFd [arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "numPuschOcasTd", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          numPuschOcasTd : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        p_MsgAPrachToMsgAPuschMap_FAPIv4->numPuschOcasTd[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "numPuschOcasSlots", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          numPuschOcasSlots : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        p_MsgAPrachToMsgAPuschMap_FAPIv4->numPuschOcasSlots[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "msgAPUSCHTimeDomainOffset", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          msgAPUSCHTimeDomainOffset : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        p_MsgAPrachToMsgAPuschMap_FAPIv4->msgAPUSCHTimeDomainOffset[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "nPreambles", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          nPreambles : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        p_MsgAPrachToMsgAPuschMap_FAPIv4->nPreambles[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                    }
                }

                p_MSGA_PRACH_Mapping_Config = &p_MsgAPrachToMsgAPuschMap_FAPIv4->msgAPrachtoMapping;
                msgAPrachMappingConfigIndex = 0;
                msgAPrachMappingConfigCatIndex = 0;

                if (searchXmlNode( pNode_Structure, &pNode_SubStructure, "msgAPrachToPruDmrsMapping") != 0)
                {
                    for (prachFdOccasionIndex = 0; prachFdOccasionIndex < numFdRa-indexFdRa; prachFdOccasionIndex++)
                    {
                        if (prachFdOccasionIndex >= MAX_NUM_PRACH_FD_OCCASION)
                            break;

                        XML_LOAD_DEBUG( "      ForEach numFdRa[%u] :\n", prachFdOccasionIndex );

                        for (prachTdOccasionIndex = 0; prachTdOccasionIndex < numPrachOcas; prachTdOccasionIndex++)
                        {
                            if (prachTdOccasionIndex >= MAX_NUM_PRACH_TD_OCCASION)
                                break;

                            XML_LOAD_DEBUG( "        ForEach numPrachOcas[%u] :\n", prachTdOccasionIndex );

                            if (getXmlNodeValueU32_Nth(pNode_SubStructure, "validRO", &value_U32, msgAPrachMappingConfigIndex) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "          validRO : %u\n", value_U32 );

                                p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].validRO = (uint8_t) value_U32;
                            }

                            if (getXmlNodeValueU32Array_Nth(pNode_SubStructure, "catAorBBitmap", value_U32Array, &arrayCount, msgAPrachMappingConfigIndex) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "          catAorBBitmap : count:%u values:", arrayCount );
                                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                                XML_LOAD_DEBUG( "\n" );

                                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_CAT_A_OR_B_BITMAP; arrayIndex++)
                                {
                                    p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].catAorBbimap[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                                }
                            }

                            if (preambleGroups == 1)    // grpA only
                            {
                                grpStart = 0;
                                grpEnd = 0;
                            }
                            else if (preambleGroups == 2)    // grpB only
                            {
                                grpStart = 1;
                                grpEnd = 1;
                            }
                            else if (preambleGroups == 3)    // both grpA and grpB
                            {
                                grpStart = 0;
                                grpEnd = 1;
                            }
                         
                            for (grpIndex = grpStart, catIndex = 0; grpIndex <= grpEnd && catIndex < MAX_NUM_PRACH_GROUP; grpIndex++, catIndex++)
                            {
                                XML_LOAD_DEBUG( "          ForEach grpIndex[%u] (catIndex:%u) :\n", grpIndex, catIndex );

                                if (getXmlNodeValueU32_Nth(pNode_SubStructure, "pruPreambleIndexForStartPreambleIndex", &value_U32, msgAPrachMappingConfigCatIndex) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "            pruPreambleIndexForStartPreambleIndex : %u\n", value_U32 );

                                    p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].catIndex[catIndex].pruPreambleIndexForStartPreambleIndex = (uint16_t) value_U32;
                                }

                                if (getXmlNodeValueU32_Nth(pNode_SubStructure, "numPuschAssocDmrsOccasionsForThisPrachRO", &value_U32, msgAPrachMappingConfigCatIndex) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "            numPuschAssocDmrsOccasionsForThisPrachRO : %u\n", value_U32 );

                                    p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].catIndex[catIndex].numPuschAssocDmrsOccasionsForThisPrachRO = (uint8_t) value_U32;
                                }

                                if (getXmlNodeValueU32Array_Nth(pNode_SubStructure, "msgAPuschAssocDmrsOccasion", value_U32Array, &arrayCount, msgAPrachMappingConfigCatIndex) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "            msgAPuschAssocDmrsOccasion : count:%u values:", arrayCount );
                                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                                    XML_LOAD_DEBUG( "\n" );

                                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_PRU_ASSOC_DMRS; arrayIndex++)
                                    {
                                        p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].catIndex[catIndex].msgAPuschAssocDmrsOccasion[arrayIndex] =
                                            (uint16_t) value_U32Array[arrayIndex];
                                    }
                                }

                                msgAPrachMappingConfigCatIndex++;
                            }

                            msgAPrachMappingConfigIndex++;
                        }
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                if (isMsgAPrach)
                    pduLength += sizeof(MsgAPrachToMsgAPuschMap_FAPIv4);
            }

            if (isMsgAPrach && hasMsgAPuschBeamforming && searchXmlNode( pNode_Pdu, &pNode_Structure, "msgAPuschBeamforning") != 0)
            {
                XML_LOAD_DEBUG( "        msgAPuschBeamforning :\n" );

                numPRGs = 0;
                digBFInterface = 0;

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(Rx_Beamforming_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                {
                    numPRGs = value_U32;
                    XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->numPRGs = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "digBFInterface", &value_U32 ) == SUCCESS)
                {
                    digBFInterface = value_U32;
                    XML_LOAD_DEBUG( "          digBFInterface : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->digBFInterface = (uint8_t) value_U32;
                }

                nodeIndexOfBeamIdx = 0;
                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterface; digBFInterfaceIndex++)
                    {
                        XML_LOAD_DEBUG( "            ForEach digBFInterface[%u] :\n", digBFInterfaceIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }
                        nodeIndexOfBeamIdx++;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                if (isMsgAPrach && hasMsgAPuschBeamforming)
                    pduLength += sizeof(Rx_Beamforming_PDU);
            }

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
        else if (strcasecmp( (char *) pProperty, "PUSCH" ) == 0)
        {
            XML_LOAD_DEBUG( "      PUSCH PDU\n" );

            p_pduHeader->pduType = UL_PDU_TYPE_PUSCH;
            pduCountPusch++;

            p_PuschPdu = (PUSCH_PDU *) p_pduHeader;
            pduLength += sizeof(PUSCH_PDU);

            pduBitmap = 0;
            numPRGs = 0;
            digBFInterface = 0;
            numPart2s = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "pduBitmap", &value_U32 ) == SUCCESS)
            {
                pduBitmap = value_U32;
                XML_LOAD_DEBUG( "        pduBitmap : %u\n", value_U32 );

                p_PuschPdu->pduBitmap = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "rnti", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rnti : %u\n", value_U32 );

                p_PuschPdu->rnti = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "handle", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        handle : %u\n", value_U32 );

                p_PuschPdu->handle = value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpSize : %u\n", value_U32 );

                p_PuschPdu->bwpSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpStart : %u\n", value_U32 );

                p_PuschPdu->bwpStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "subcarrierSpacing", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                p_PuschPdu->subcarrierSpacing = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicPrefix", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicPrefix : %u\n", value_U32 );

                p_PuschPdu->cyclicPrefix = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "targetCodeRate", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        targetCodeRate : %u\n", value_U32 );

                p_PuschPdu->targetCodeRate = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "qamModOrder", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        qamModOrder : %u\n", value_U32 );

                p_PuschPdu->qamModOrder = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "mcsIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        mcsIndex : %u\n", value_U32 );

                p_PuschPdu->mcsIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "mcsTable", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        mcsTable : %u\n", value_U32 );

                p_PuschPdu->mcsTable = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "transformPrecoding", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        transformPrecoding : %u\n", value_U32 );

                p_PuschPdu->transformPrecoding = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nIdPusch", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nIdPusch : %u\n", value_U32 );

                p_PuschPdu->nIdPusch = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nrOfLayers", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nrOfLayers : %u\n", value_U32 );

                p_PuschPdu->nrOfLayers = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "ulDmrsSymbPos", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        ulDmrsSymbPos : %u\n", value_U32 );

                p_PuschPdu->ulDmrsSymbPos = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "dmrsConfigType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        dmrsConfigType : %u\n", value_U32 );

                p_PuschPdu->dmrsConfigType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "puschDmrsScramblingId", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        puschDmrsScramblingId : %u\n", value_U32 );

                p_PuschPdu->puschDmrsScramblingId = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "puschDmrsScramblingIdComplement", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        puschDmrsScramblingIdComplement : %u\n", value_U32 );

                p_PuschPdu->puschDmrsScramblingIdComplement = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "lowPaprDmrs", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        lowPaprDmrs : %u\n", value_U32 );

                p_PuschPdu->lowPaprDmrs = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "puschDmrsIdentity", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        puschDmrsIdentity : %u\n", value_U32 );

                p_PuschPdu->puschDmrsIdentity = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nSCID", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nSCID : %u\n", value_U32 );

                p_PuschPdu->nSCID = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numDmrsCdmGrpsNoData", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        numDmrsCdmGrpsNoData : %u\n", value_U32 );

                p_PuschPdu->numDmrsCdmGrpsNoData = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "dmrsPorts", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        dmrsPorts : %u\n", value_U32 );

                p_PuschPdu->dmrsPorts = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "resourceAlloc", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        resourceAlloc : %u\n", value_U32 );

                p_PuschPdu->resourceAlloc = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32Array(pNode_Pdu, "rbBitmap", value_U32Array, &arrayCount ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "          rbBitmap : count:%u values:", arrayCount );
                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                XML_LOAD_DEBUG( "\n" );

                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_RB_BITMAP_SIZE; arrayIndex++)
                {
                    p_PuschPdu->rbBitmap[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                }
            }

            if (getXmlNodeValueU32(pNode_Pdu, "rbStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rbStart : %u\n", value_U32 );

                p_PuschPdu->rbStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "rbSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rbSize : %u\n", value_U32 );

                p_PuschPdu->rbSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "vrbToPrbMapping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        vrbToPrbMapping : %u\n", value_U32 );

                p_PuschPdu->vrbToPrbMapping = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "intraSlotFrequencyHopping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        intraSlotFrequencyHopping : %u\n", value_U32 );

                p_PuschPdu->intraSlotFrequencyHopping = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "txDirectCurrentLocation", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        txDirectCurrentLocation : %u\n", value_U32 );

                p_PuschPdu->txDirectCurrentLocation = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "uplinkFrequencyShift7p5khz", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        uplinkFrequencyShift7p5khz : %u\n", value_U32 );

                p_PuschPdu->uplinkFrequencyShift7p5khz = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "startSymbolIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        startSymbolIndex : %u\n", value_U32 );

                p_PuschPdu->startSymbolIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nrOfSymbols", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nrOfSymbols : %u\n", value_U32 );

                p_PuschPdu->nrOfSymbols = (uint8_t) value_U32;
            }

            if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_DATA_INCLUDED) && searchXmlNode( pNode_Pdu, &pNode_Structure, "puschData") != 0)
            {
                XML_LOAD_DEBUG( "        puschData :\n" );

                p_PuschDataInfo = (PuschDataInfo *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PuschDataInfo);

                if (getXmlNodeValueU32(pNode_Structure, "rvIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          rvIndex : %u\n", value_U32 );

                    p_PuschDataInfo->rvIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "harqProcessID", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          harqProcessID : %u\n", value_U32 );

                    p_PuschDataInfo->harqProcessID = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "newData", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          newData : %u\n", value_U32 );

                    p_PuschDataInfo->newData = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "tbSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          tbSize : %u\n", value_U32 );

                    p_PuschDataInfo->tbSize = value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numCb", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          numCb : %u\n", value_U32 );

                    p_PuschDataInfo->numCb = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "cbPresentAndPosition", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          cbPresentAndPosition : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_CB_PRESENT_POSITION; arrayIndex++)
                    {
                        p_PuschDataInfo->cbPresentAndPosition[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_DATA_INCLUDED))
                    pduLength += sizeof(PuschDataInfo);
            }

            if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_UCI_INCLUDED) && searchXmlNode( pNode_Pdu, &pNode_Structure, "puschUci") != 0)
            {
                XML_LOAD_DEBUG( "        puschUci :\n" );

                p_PuschUCIInfo = (PuschUCIInfo *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PuschUCIInfo);

                if (getXmlNodeValueU32(pNode_Structure, "harqAckBitLength", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          harqAckBitLength : %u\n", value_U32 );

                    p_PuschUCIInfo->harqAckBitLength = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "csiPart1BitLength", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          csiPart1BitLength : %u\n", value_U32 );

                    p_PuschUCIInfo->csiPart1BitLength = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "flagCsiPart2", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          flagCsiPart2 : %u\n", value_U32 );

                    p_PuschUCIInfo->flagCsiPart2 = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "alphaScaling", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          alphaScaling : %u\n", value_U32 );

                    p_PuschUCIInfo->alphaScaling = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "betaOffsetHarqAck", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          betaOffsetHarqAck : %u\n", value_U32 );

                    p_PuschUCIInfo->betaOffsetHarqAck = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "betaOffsetCsi1", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          betaOffsetCsi1 : %u\n", value_U32 );

                    p_PuschUCIInfo->betaOffsetCsi1 = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "betaOffsetCsi2", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          betaOffsetCsi2 : %u\n", value_U32 );

                    p_PuschUCIInfo->betaOffsetCsi2 = (uint8_t) value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_UCI_INCLUDED))
                    pduLength += sizeof(PuschUCIInfo);
            }

            if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_PTRS_INCLUDED) && searchXmlNode( pNode_Pdu, &pNode_Structure, "puschPtrs") != 0)
            {
                XML_LOAD_DEBUG( "        puschPtrs :\n" );

                p_PuschPTRSInfo = (PuschPTRSInfo *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PuschPTRSInfo);

                numPtrsPorts = 0;

                if (getXmlNodeValueU32(pNode_Structure, "numPtrsPorts", &value_U32 ) == SUCCESS)
                {
                    numPtrsPorts = value_U32;
                    XML_LOAD_DEBUG( "          numPtrsPorts : %u\n", value_U32 );

                    p_PuschPTRSInfo->numPtrsPorts = (uint16_t) value_U32;
                }

                for (ptrsPortsIndex = 0; ptrsPortsIndex < numPtrsPorts ; ptrsPortsIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPtrsPorts[%u] :\n", ptrsPortsIndex );

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "ptrsPortIndex", &value_U32, ptrsPortsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            ptrsPortIndex : %u\n", value_U32 );

                        p_PuschPTRSInfo->ptrsPortInfo[ptrsPortsIndex].ptrsPortIndex = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "ptrsDmrsPort", &value_U32, ptrsPortsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            ptrsDmrsPort : %u\n", value_U32 );

                        p_PuschPTRSInfo->ptrsPortInfo[ptrsPortsIndex].ptrsDmrsPort = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "ptrsReOffset", &value_U32, ptrsPortsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            ptrsReOffset : %u\n", value_U32 );

                        p_PuschPTRSInfo->ptrsPortInfo[ptrsPortsIndex].ptrsReOffset = (uint8_t) value_U32;
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "ptrsTimeDensity", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ptrsTimeDensity : %u\n", value_U32 );

                    p_PuschPTRSInfo->ptrsTimeDensity = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ptrsFreqDensity", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ptrsFreqDensity : %u\n", value_U32 );

                    p_PuschPTRSInfo->ptrsFreqDensity = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ulPTRSPower", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ulPTRSPower : %u\n", value_U32 );

                    p_PuschPTRSInfo->ulPTRSPower = (uint8_t) value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_PTRS_INCLUDED))
                    pduLength += sizeof(PuschPTRSInfo);
            }

            if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_DFTS_OFDM_INCLUDED) && searchXmlNode( pNode_Pdu, &pNode_Structure, "dftsOfdm") != 0)
            {
                XML_LOAD_DEBUG( "        dftsOfdm :\n" );

                p_DftsOfdmInfo = (DftsOfdmInfo *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(DftsOfdmInfo);

                if (getXmlNodeValueU32(pNode_Structure, "lowPaprGroupNumber", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          lowPaprGroupNumber : %u\n", value_U32 );

                    p_DftsOfdmInfo->lowPaprGroupNumber = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "lowPaprSequenceNumber", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          lowPaprSequenceNumber : %u\n", value_U32 );

                    p_DftsOfdmInfo->lowPaprSequenceNumber = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ulPtrsSampleDensity", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ulPtrsSampleDensity : %u\n", value_U32 );

                    p_DftsOfdmInfo->ulPtrsSampleDensity = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ulPtrsTimeDensityTransformPrecoding", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ulPtrsTimeDensityTransformPrecoding : %u\n", value_U32 );

                    p_DftsOfdmInfo->ulPtrsTimeDensityTransformPrecoding = (uint8_t) value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_DFTS_OFDM_INCLUDED))
                    pduLength += sizeof(DftsOfdmInfo);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "rxBeamforming") != 0)
            {
                XML_LOAD_DEBUG( "        rxBeamforming :\n" );

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(Rx_Beamforming_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                {
                    numPRGs = value_U32;
                    XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->numPRGs = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "digBFInterface", &value_U32 ) == SUCCESS)
                {
                    digBFInterface = value_U32;
                    XML_LOAD_DEBUG( "          digBFInterface : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->digBFInterface = (uint8_t) value_U32;
                }

                nodeIndexOfBeamIdx = 0;
                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterface; digBFInterfaceIndex++)
                    {
                        XML_LOAD_DEBUG( "            ForEach digBFInterface[%u] :\n", digBFInterfaceIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }
                        nodeIndexOfBeamIdx++;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(Rx_Beamforming_PDU);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "puschMaintenanceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        puschMaintenanceFapiV3 :\n" );

                p_PUSCH_Maintenance_FAPIv3 = (PUSCH_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PUSCH_Maintenance_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "puschTransType", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          puschTransType : %u\n", value_U32 );

                    p_PUSCH_Maintenance_FAPIv3->puschTransType = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "deltaBwp0StartFromActiveBwp", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          deltaBwp0StartFromActiveBwp : %u\n", value_U32 );

                    p_PUSCH_Maintenance_FAPIv3->deltaBwp0StartFromActiveBwp = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "initialUlBwpSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          initialUlBwpSize : %u\n", value_U32 );

                    p_PUSCH_Maintenance_FAPIv3->initialUlBwpSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "groupOrSequenceHopping", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          groupOrSequenceHopping : %u\n", value_U32 );

                    p_PUSCH_Maintenance_FAPIv3->groupOrSequenceHopping = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "puschSecondHopPRB", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          puschSecondHopPRB : %u\n", value_U32 );

                    p_PUSCH_Maintenance_FAPIv3->puschSecondHopPRB = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ldpcBaseGraph", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ldpcBaseGraph : %u\n", value_U32 );

                    p_PUSCH_Maintenance_FAPIv3->ldpcBaseGraph = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "tbSizeLbrmBytes", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          tbSizeLbrmBytes : %u\n", value_U32 );

                    p_PUSCH_Maintenance_FAPIv3->tbSizeLbrmBytes = value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PUSCH_Maintenance_FAPIv3);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "puschParametersFapiV4") != 0)
            {
                XML_LOAD_DEBUG( "        puschParametersFapiV4 :\n" );

                p_PUSCH_FAPIv4 = (PUSCH_FAPIv4 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PUSCH_FAPIv4);

                if (getXmlNodeValueU32(pNode_Structure, "cbCrsStatusRequest", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          cbCrsStatusRequest : %u\n", value_U32 );

                    p_PUSCH_FAPIv4->cbCrsStatusRequest = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "srsTxPorts", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          srsTxPorts : %u\n", value_U32 );

                    p_PUSCH_FAPIv4->srsTxPorts = value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ulTpmiIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ulTpmiIndex : %u\n", value_U32 );

                    p_PUSCH_FAPIv4->ulTpmiIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numULSpatialStreamPorts", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          numULSpatialStreamPorts : %u\n", value_U32 );

                    p_PUSCH_FAPIv4->numULSpatialStreamPorts = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "cbCrsStatusRequest", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          cbCrsStatusRequest : %u\n", value_U32 );

                    p_PUSCH_FAPIv4->cbCrsStatusRequest = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "ulSpatialStreamPorts", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ulSpatialStreamPorts : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_UL_SPATIAL_STREAM_PORTS; arrayIndex++)
                    {
                        p_PUSCH_FAPIv4->ulSpatialStreamPorts[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PUSCH_FAPIv4);
            }

            if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_UCI_INCLUDED) && searchXmlNode( pNode_Pdu, &pNode_Structure, "puschUciFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        puschUciFapiV3 :\n" );

                p_PuschUCIInfo_FAPIv3 = (PuschUCIInfo_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PuschUCIInfo_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "numPart2s", &value_U32 ) == SUCCESS)
                {
                    numPart2s = value_U32;
                    XML_LOAD_DEBUG( "          numPart2s : %u\n", value_U32 );

                    p_PuschUCIInfo_FAPIv3->numPart2s = (uint16_t) value_U32;
                }

                for (part2sIndex = 0; part2sIndex < numPart2s; part2sIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPart2s[%u] :\n", part2sIndex );

                    p_UCIPart2Info_FAPIv3 = (UCIPart2Info_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(UCIPart2Info_FAPIv3);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "priority", &value_U32, part2sIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            priority : %u\n", value_U32 );

                        p_UCIPart2Info_FAPIv3->priority = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "numPart1Params", &value_U32, part2sIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            numPart1Params : %u\n", value_U32 );

                        p_UCIPart2Info_FAPIv3->numPart1Params = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array_Nth(pNode_Structure, "paramOffsets", value_U32Array, &arrayCount, part2sIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        paramOffsets : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_OF_UCI_PART1_PARAMS; arrayIndex++)
                        {
                            p_UCIPart2Info_FAPIv3->paramOffsets[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32Array_Nth(pNode_Structure, "paramSizes", value_U32Array, &arrayCount, part2sIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        paramSizes : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_OF_UCI_PART1_PARAMS; arrayIndex++)
                        {
                            p_UCIPart2Info_FAPIv3->paramSizes[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "part2SizeMapIndex", &value_U32, part2sIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            part2SizeMapIndex : %u\n", value_U32 );

                        p_UCIPart2Info_FAPIv3->part2SizeMapIndex = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "part2SizeMapScope", &value_U32, part2sIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            part2SizeMapScope : %u\n", value_U32 );

                        p_UCIPart2Info_FAPIv3->part2SizeMapScope = (uint8_t) value_U32;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                if ((pduBitmap & BITMASK_PUSCH_PDU_BITMAP_UCI_INCLUDED))
                    pduLength += sizeof(PuschUCIInfo_FAPIv3);
            }

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
        else if (strcasecmp( (char *) pProperty, "PUCCH" ) == 0)
        {
            XML_LOAD_DEBUG( "      PUCCH PDU\n" );

            p_pduHeader->pduType = UL_PDU_TYPE_PUCCH;
            pduCountPucch++;

            p_PucchPdu = (PUCCH_PDU *) p_pduHeader;
            pduLength += sizeof(PUCCH_PDU);

            numPRGs = 0;
            digBFInterface = 0;
            numPart2s = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "rnti", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rnti : %u\n", value_U32 );

                p_PucchPdu->rnti = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "handle", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        handle : %u\n", value_U32 );

                p_PucchPdu->handle = value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpSize : %u\n", value_U32 );

                p_PucchPdu->bwpSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpStart : %u\n", value_U32 );

                p_PucchPdu->bwpStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "subcarrierSpacing", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                p_PucchPdu->subcarrierSpacing = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicPrefix", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicPrefix : %u\n", value_U32 );

                p_PucchPdu->cyclicPrefix = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "formatType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        formatType : %u\n", value_U32 );

                p_PucchPdu->formatType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "multiSlotTxIndicator", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        multiSlotTxIndicator : %u\n", value_U32 );

                p_PucchPdu->multiSlotTxIndicator = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "pi2Bpsk", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        pi2Bpsk : %u\n", value_U32 );

                p_PucchPdu->pi2Bpsk = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "prbStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        prbStart : %u\n", value_U32 );

                p_PucchPdu->prbStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "prbSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        prbSize : %u\n", value_U32 );

                p_PucchPdu->prbSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "startSymbolIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        startSymbolIndex : %u\n", value_U32 );

                p_PucchPdu->startSymbolIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nrOfSymbols", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nrOfSymbols : %u\n", value_U32 );

                p_PucchPdu->nrOfSymbols = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "intraSlotFrequencyHopping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        intraSlotFrequencyHopping : %u\n", value_U32 );

                p_PucchPdu->intraSlotFrequencyHopping = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "secondHopPRB", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        secondHopPRB : %u\n", value_U32 );

                p_PucchPdu->secondHopPRB = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "pucchGroupHopping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        pucchGroupHopping : %u\n", value_U32 );

                p_PucchPdu->pucchGroupHopping = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "obsolete8bit", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        obsolete8bit : %u\n", value_U32 );

                p_PucchPdu->obsolete8bit = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nIdPucchHopping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nIdPucchHopping : %u\n", value_U32 );

                p_PucchPdu->nIdPucchHopping = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "initialCyclicShift", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        initialCyclicShift : %u\n", value_U32 );

                p_PucchPdu->initialCyclicShift = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nIdPucchScrambling", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nIdPucchScrambling : %u\n", value_U32 );

                p_PucchPdu->nIdPucchScrambling = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "timeDomainOccIdx", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        timeDomainOccIdx : %u\n", value_U32 );

                p_PucchPdu->timeDomainOccIdx = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "preDftOccIdx", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        preDftOccIdx : %u\n", value_U32 );

                p_PucchPdu->preDftOccIdx = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "preDftOccLen", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        preDftOccLen : %u\n", value_U32 );

                p_PucchPdu->preDftOccLen = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "addDmrsFlag", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        addDmrsFlagaddDmrsFlag : %u\n", value_U32 );

                p_PucchPdu->addDmrsFlag = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nId0PucchDmrsScrambling", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nId0PucchDmrsScrambling : %u\n", value_U32 );

                p_PucchPdu->nId0PucchDmrsScrambling = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "m0PucchDmrsCyclicShift", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        m0PucchDmrsCyclicShift : %u\n", value_U32 );

                p_PucchPdu->m0PucchDmrsCyclicShift = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "srBitLen", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        srBitLen : %u\n", value_U32 );

                p_PucchPdu->srBitLen = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bitLenHarq", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bitLenHarq : %u\n", value_U32 );

                p_PucchPdu->bitLenHarq = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "csiPart1BitLength", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        csiPart1BitLength : %u\n", value_U32 );

                p_PucchPdu->csiPart1BitLength = (uint16_t) value_U32;
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "rxBeamforming") != 0)
            {
                XML_LOAD_DEBUG( "        rxBeamforming :\n" );

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(Rx_Beamforming_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                {
                    numPRGs = value_U32;
                    XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->numPRGs = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "digBFInterface", &value_U32 ) == SUCCESS)
                {
                    digBFInterface = value_U32;
                    XML_LOAD_DEBUG( "          digBFInterface : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->digBFInterface = (uint8_t) value_U32;
                }

                nodeIndexOfBeamIdx = 0;
                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterface; digBFInterfaceIndex++)
                    {
                        XML_LOAD_DEBUG( "            ForEach digBFInterface[%u] :\n", digBFInterfaceIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }
                        nodeIndexOfBeamIdx++;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(Rx_Beamforming_PDU);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "pucchMaintenanceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        pucchMaintenanceFapiV3 :\n" );

                p_PUCCH_PDU_Maintenance_FAPIv3 = (PUCCH_PDU_Maintenance_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PUCCH_PDU_Maintenance_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "maxCodeRate", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          maxCodeRate : %u\n", value_U32 );

                    p_PUCCH_PDU_Maintenance_FAPIv3->maxCodeRate = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ulBwpId", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ulBwpId : %u\n", value_U32 );

                    p_PUCCH_PDU_Maintenance_FAPIv3->ulBwpId = (uint8_t) value_U32;
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PUCCH_PDU_Maintenance_FAPIv3);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "uciPart1ToPart2CorrespondenceFapiV3") != 0)
            {
                XML_LOAD_DEBUG( "        uciPart1ToPart2CorrespondenceFapiV3 :\n" );

                p_PuschUCIInfo_FAPIv3 = (PuschUCIInfo_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(PuschUCIInfo_FAPIv3);

                if (getXmlNodeValueU32(pNode_Structure, "numPart2s", &value_U32 ) == SUCCESS)
                {
                    numPart2s = value_U32;
                    XML_LOAD_DEBUG( "          numPart2s : %u\n", value_U32 );

                    p_PuschUCIInfo_FAPIv3->numPart2s = (uint16_t) value_U32;
                }

                for (part2sIndex = 0; part2sIndex < numPart2s; part2sIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPart2s[%u] :\n", part2sIndex );

                    p_UCIPart2Info_FAPIv3 = (UCIPart2Info_FAPIv3 *) ((uint8_t *)p_pduHeader + pduLength);
                    pduLength += sizeof(UCIPart2Info_FAPIv3);

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "priority", &value_U32, part2sIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            priority : %u\n", value_U32 );

                        p_UCIPart2Info_FAPIv3->priority = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "numPart1Params", &value_U32, part2sIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            numPart1Params : %u\n", value_U32 );

                        p_UCIPart2Info_FAPIv3->numPart1Params = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array_Nth(pNode_Structure, "paramOffsets", value_U32Array, &arrayCount, part2sIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        paramOffsets : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_OF_UCI_PART1_PARAMS; arrayIndex++)
                        {
                            p_UCIPart2Info_FAPIv3->paramOffsets[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32Array_Nth(pNode_Structure, "paramSizes", value_U32Array, &arrayCount, part2sIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        paramSizes : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_OF_UCI_PART1_PARAMS; arrayIndex++)
                        {
                            p_UCIPart2Info_FAPIv3->paramSizes[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "part2SizeMapIndex", &value_U32, part2sIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            part2SizeMapIndex : %u\n", value_U32 );

                        p_UCIPart2Info_FAPIv3->part2SizeMapIndex = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "part2SizeMapScope", &value_U32, part2sIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "            part2SizeMapScope : %u\n", value_U32 );

                        p_UCIPart2Info_FAPIv3->part2SizeMapScope = (uint8_t) value_U32;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(PuschUCIInfo_FAPIv3);
            }

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
        else if (strcasecmp( (char *) pProperty, "SRS" ) == 0)
        {
            XML_LOAD_DEBUG( "      SRS PDU\n" );

            p_pduHeader->pduType = UL_PDU_TYPE_SRS;
            pduCountSrs++;

            p_SrsPdu = (SRS_PDU *) p_pduHeader;
            pduLength += sizeof(SRS_PDU);

            numPRGs = 0;
            digBFInterface = 0;
            numSymbols = 0;
            nUsage = 0;
            numULSpatialStreamsPorts = 0;

            if (getXmlNodeValueU32(pNode_Pdu, "rnti", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        rnti : %u\n", value_U32 );

                p_SrsPdu->rnti = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "handle", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        handle : %u\n", value_U32 );

                p_SrsPdu->handle = value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpSize : %u\n", value_U32 );

                p_SrsPdu->bwpSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpStart : %u\n", value_U32 );

                p_SrsPdu->bwpStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "subcarrierSpacing", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                p_SrsPdu->subcarrierSpacing = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicPrefix", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicPrefix : %u\n", value_U32 );

                p_SrsPdu->cyclicPrefix = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numAntPorts", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        numAntPorts : %u\n", value_U32 );

                p_SrsPdu->numAntPorts = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numSymbols", &value_U32 ) == SUCCESS)
            {
                numSymbols = value_U32;
                XML_LOAD_DEBUG( "        numSymbols : %u\n", value_U32 );

                p_SrsPdu->numSymbols = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numRepetitions", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        numRepetitions : %u\n", value_U32 );

                p_SrsPdu->numRepetitions = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "timeStartPosition", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        timeStartPosition : %u\n", value_U32 );

                p_SrsPdu->timeStartPosition = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "configIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        configIndex : %u\n", value_U32 );

                p_SrsPdu->configIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "sequenceId", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        sequenceId : %u\n", value_U32 );

                p_SrsPdu->sequenceId = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bandwidthIndex", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bandwidthIndex : %u\n", value_U32 );

                p_SrsPdu->bandwidthIndex = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "combSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        combSize : %u\n", value_U32 );

                p_SrsPdu->combSize = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "combOffset", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        combOffset : %u\n", value_U32 );

                p_SrsPdu->combOffset = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicShift", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicShift : %u\n", value_U32 );

                p_SrsPdu->cyclicShift = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "frequencyPosition", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        frequencyPosition : %u\n", value_U32 );

                p_SrsPdu->frequencyPosition = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "frequencyShift", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        frequencyShift : %u\n", value_U32 );

                p_SrsPdu->frequencyShift = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "frequencyHopping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        frequencyHopping : %u\n", value_U32 );

                p_SrsPdu->frequencyHopping = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "groupOrSequenceHopping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        groupOrSequenceHopping : %u\n", value_U32 );

                p_SrsPdu->groupOrSequenceHopping = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "resourceType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        resourceType : %u\n", value_U32 );

                p_SrsPdu->resourceType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "tSrs", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        tSrs : %u\n", value_U32 );

                p_SrsPdu->tSrs = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "tOffset", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        tOffset : %u\n", value_U32 );

                p_SrsPdu->tOffset = (uint16_t) value_U32;
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "rxBeamforming") != 0)
            {
                XML_LOAD_DEBUG( "        rxBeamforming :\n" );

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(Rx_Beamforming_PDU);

                if (getXmlNodeValueU32(pNode_Structure, "trpScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          trpScheme : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->trpScheme = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numPRGs", &value_U32 ) == SUCCESS)
                {
                    numPRGs = value_U32;
                    XML_LOAD_DEBUG( "          numPRGs : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->numPRGs = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "digBFInterface", &value_U32 ) == SUCCESS)
                {
                    digBFInterface = value_U32;
                    XML_LOAD_DEBUG( "          digBFInterface : %u\n", value_U32 );

                    p_Rx_Beamforming_PDU->digBFInterface = (uint8_t) value_U32;
                }

                nodeIndexOfBeamIdx = 0;
                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    XML_LOAD_DEBUG( "          ForEach numPRGs[%u] :\n", prgIndex );

                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterface; digBFInterfaceIndex++)
                    {
                        XML_LOAD_DEBUG( "            ForEach digBFInterface[%u] :\n", digBFInterfaceIndex );

                        p_Value_U16 = (uint16_t *) ((uint8_t *)p_pduHeader + pduLength);
                        pduLength += sizeof(uint16_t);

                        if (getXmlNodeValueU32_Nth(pNode_Structure, "beamIdx", &value_U32, nodeIndexOfBeamIdx) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "              beamIdx : %u\n", value_U32 );

                            *p_Value_U16 = (uint16_t) value_U32;
                        }
                        nodeIndexOfBeamIdx++;
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(Rx_Beamforming_PDU);
            }

            if (searchXmlNode( pNode_Pdu, &pNode_Structure, "srsParametersFapiV4") != 0)
            {
                XML_LOAD_DEBUG( "        srsParametersFapiV4 :\n" );

                p_SRS_PDU_FAPIv4 = (SRS_PDU_FAPIv4 *) ((uint8_t *)p_pduHeader + pduLength);
                pduLength += sizeof(SRS_PDU_FAPIv4);

                if (getXmlNodeValueU32(pNode_Structure, "srsBandwidthSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          srsBandwidthSize : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->srsBandwidthSize = (uint16_t) value_U32;
                }

                for (symbolIndex = 0; symbolIndex < numSymbols; symbolIndex++)
                {
                    if (symbolIndex >= MAX_SRS_IND_SYMBOL_INFO_COUNT)
                        break;

                    XML_LOAD_DEBUG( "          ForEach numSymbols[%u] :\n", symbolIndex );

                    p_SRS_PDU_FAPIv4_SymbolInfo = &p_SRS_PDU_FAPIv4->srsSymbolInfo[symbolIndex];

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "srsBandwidthStart", &value_U32, symbolIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        srsBandwidthStart : %u\n", value_U32 );

                        p_SRS_PDU_FAPIv4_SymbolInfo->srsBandwidthStart = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "sequenceGroup", &value_U32, symbolIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        sequenceGroup : %u\n", value_U32 );

                        p_SRS_PDU_FAPIv4_SymbolInfo->sequenceGroup = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Structure, "sequenceNumber", &value_U32, symbolIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        sequenceNumber : %u\n", value_U32 );

                        p_SRS_PDU_FAPIv4_SymbolInfo->sequenceNumber = (uint8_t) value_U32;
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "usage", &value_U32 ) == SUCCESS)
                {
                    nUsage = __builtin_popcount(value_U32);
                    XML_LOAD_DEBUG( "          usage : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->usage = value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "reportType", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          reportType : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < nUsage && arrayIndex < NAX_NUM_SRS_USAGE; arrayIndex++)
                    {
                        p_SRS_PDU_FAPIv4->reportType[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Structure, "singularValueRepresentation", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          singularValueRepresentation : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->singularValueRepresentation = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "iqRepresentation", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          iqRepresentation : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->iqRepresentation = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "prgSize", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          prgSize : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->prgSize = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numTotalUeAntennas", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          numTotalUeAntennas : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->numTotalUeAntennas = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "ueAntennasInThisSrsResourceSet", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ueAntennasInThisSrsResourceSet : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->ueAntennasInThisSrsResourceSet = value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "sampledUeAntennas", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          sampledUeAntennas : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->sampledUeAntennas = value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "reportScope", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          reportScope : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->reportScope = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Structure, "numULSpatialStreamsPorts", &value_U32 ) == SUCCESS)
                {
                    numULSpatialStreamsPorts = value_U32;
                    XML_LOAD_DEBUG( "          numULSpatialStreamsPorts : %u\n", value_U32 );

                    p_SRS_PDU_FAPIv4->numULSpatialStreamsPorts = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Structure, "ulSpatialStreamPorts", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "          ulSpatialStreamPorts : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < numULSpatialStreamsPorts && arrayIndex < MAX_NUM_UL_SPATIAL_STREAM_PORTS; arrayIndex++)
                    {
                        p_SRS_PDU_FAPIv4->ulSpatialStreamPorts[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                    }
                }
            }
            else
            {
                // add length for Mandatory fields
                pduLength += sizeof(SRS_PDU_FAPIv4);
            }

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
        else if (strcasecmp( (char *) pProperty, "MSGA_PUSCH" ) == 0)
        {
            XML_LOAD_DEBUG( "      MSGA_PUSCH PDU\n" );

            p_pduHeader->pduType = UL_PDU_TYPE_MSGA_PUSCH;
            pduCountMsgAPusch++;

            p_MsgAPuschPdu = (MSGA_PUSCH_PDU *) p_pduHeader;
            pduLength += sizeof(MSGA_PUSCH_PDU);

            if (getXmlNodeValueU32(pNode_Pdu, "pduBitmap", &value_U32 ) == SUCCESS)
            {
                pduBitmap = value_U32;
                XML_LOAD_DEBUG( "        pduBitmap : %u\n", value_U32 );

                p_MsgAPuschPdu->pduBitmap = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "prachToPruMapType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        prachToPruMapType : %u\n", value_U32 );

                p_MsgAPuschPdu->prachToPruMapType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "prachToPruMapIndicator", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        prachToPruMapIndicator : %u\n", value_U32 );

                p_MsgAPuschPdu->prachToPruMapIndicator = value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "msgAprachSfn", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        msgAprachSfn : %u\n", value_U32 );

                p_MsgAPuschPdu->msgAprachSfn = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "msgAprachSlot", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        msgAprachSlot : %u\n", value_U32 );

                p_MsgAPuschPdu->msgAprachSlot = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "handle", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        handle : %u\n", value_U32 );

                p_MsgAPuschPdu->handle = value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpSize : %u\n", value_U32 );

                p_MsgAPuschPdu->bwpSize = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "bwpStart", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        bwpStart : %u\n", value_U32 );

                p_MsgAPuschPdu->bwpStart = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "subcarrierSpacing", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                p_MsgAPuschPdu->subcarrierSpacing = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "cyclicPrefix", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        cyclicPrefix : %u\n", value_U32 );

                p_MsgAPuschPdu->cyclicPrefix = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "msgAmcs", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        msgAmcs : %u\n", value_U32 );

                p_MsgAPuschPdu->msgAmcs = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "transformPrecoding", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        transformPrecoding : %u\n", value_U32 );

                p_MsgAPuschPdu->transformPrecoding = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "nIdMsgAPusch", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        nIdMsgAPusch : %u\n", value_U32 );

                p_MsgAPuschPdu->nIdMsgAPusch = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32Array(pNode_Pdu, "puschDmrsScramblingId", value_U32Array, &arrayCount ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "          puschDmrsScramblingId : count:%u values:", arrayCount );
                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                XML_LOAD_DEBUG( "\n" );

                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < 2; arrayIndex++)
                {
                    p_MsgAPuschPdu->puschDmrsScramblingId[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                }
            }

            if (getXmlNodeValueU32(pNode_Pdu, "ulDmrsSymbPos", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        ulDmrsSymbPos : %u\n", value_U32 );

                p_MsgAPuschPdu->ulDmrsSymbPos = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "dmrsMappingType", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        dmrsMappingType : %u\n", value_U32 );

                p_MsgAPuschPdu->dmrsMappingType = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "startRbPuschOcas", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        startRbPuschOcas : %u\n", value_U32 );

                p_MsgAPuschPdu->startRbPuschOcas = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "numRbPUschOcas", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        numRbPUschOcas : %u\n", value_U32 );

                p_MsgAPuschPdu->numRbPUschOcas = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "guardBand", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        guardBand : %u\n", value_U32 );

                p_MsgAPuschPdu->guardBand = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "intraSlotFrequencyHopping", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        intraSlotFrequencyHopping : %u\n", value_U32 );

                p_MsgAPuschPdu->intraSlotFrequencyHopping = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "intraSlotFrequencyHoppingBits", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        intraSlotFrequencyHoppingBits : %u\n", value_U32 );

                p_MsgAPuschPdu->intraSlotFrequencyHoppingBits = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "txDirectCurrentLocation", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        txDirectCurrentLocation : %u\n", value_U32 );

                p_MsgAPuschPdu->txDirectCurrentLocation = (uint16_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "uplinkFrequencyShift7p5khz", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        uplinkFrequencyShift7p5khz : %u\n", value_U32 );

                p_MsgAPuschPdu->uplinkFrequencyShift7p5khz = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "startSymbolIdPuschOcas", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        startSymbolIdPuschOcas : %u\n", value_U32 );

                p_MsgAPuschPdu->startSymbolIdPuschOcas = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "durationPuschOcas", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        durationPuschOcas : %u\n", value_U32 );

                p_MsgAPuschPdu->durationPuschOcas = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "guardPeriod", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        guardPeriod : %u\n", value_U32 );

                p_MsgAPuschPdu->guardPeriod = (uint8_t) value_U32;
            }

            if (getXmlNodeValueU32(pNode_Pdu, "tbSize", &value_U32 ) == SUCCESS)
            {
                XML_LOAD_DEBUG( "        tbSize : %u\n", value_U32 );

                p_MsgAPuschPdu->tbSize = value_U32;
            }

            p_pduHeader->pduSize = pduLength;
            p_pduHeader = (PDU_Struct *) ((uint8_t *) p_pduHeader + pduLength);
            fapiBodyLength += pduLength;
        }
    }

    pduCountTotal = pduCountPrach + pduCountPusch + pduCountPucch + pduCountSrs + pduCountMsgAPusch;

    p_UlTtiReq->numPDUs = pduCountTotal;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_PRACH] = pduCountPrach;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_PUSCH] = pduCountPusch;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_FORMAT01_PUCCH] = pduCountPucchFormat01;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_FORMAT234_PUCCH] = pduCountPucchFormat234;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_SRS] = pduCountSrs;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_MSGA_PUSCH] = pduCountMsgAPusch;
    p_UlTtiReq->nGroup = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Finalize message

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    XML_LOAD_DEBUG( "    === Final msgLength %u\n", *p_MsgLength );

    return SUCCESS;
}

int loadTxConfig( uint32_t slotNum, uint8_t cellIndex )
{
    xmlDoc *pDoc;
    xmlNode *pRoot;
    xmlNode *pNode_FapiMsg[MAX_MULTIPLE_XML_NODE_COUNT], *pNode_FapiMsgType;
    uint32_t fapiMsgCount, fapiMsgIndex, *pFapiMsgSize;
    uint8_t *pFapiMsg;
    slot_config_t *pSlotConfig;

    pSlotConfig = &(g_macRunnerConfig.cellConfig[cellIndex].slotConfig[slotNum]); 

    pDoc = xmlReadFile(pSlotConfig->txConfigFileName, NULL, 0);
    if (!pDoc)
    {
        XML_LOAD_WARNING( "xmlReadFile failed for file %s\n", pSlotConfig->txConfigFileName );
        return FAILURE;
    }

    pRoot = xmlDocGetRootElement(pDoc);
    if (!pRoot)
    {
        XML_LOAD_WARNING( "xmlDocGetRootElement failed for file %s\n", pSlotConfig->txConfigFileName );
        return FAILURE;
    }

    fapiMsgCount = 0;
    pSlotConfig->fapiMsgCount = 0;

    searchXmlMultipleNodes( pRoot, pNode_FapiMsg, &fapiMsgCount, "fapiMsg" );
    if (fapiMsgCount > 0)
    {
        for (fapiMsgIndex = 0; fapiMsgIndex < fapiMsgCount; fapiMsgIndex++)
        {
            pFapiMsg = &(pSlotConfig->fapiMsg[pSlotConfig->fapiMsgCount][0]);
            pFapiMsgSize = &(pSlotConfig->fapiMsgSize[pSlotConfig->fapiMsgCount]);
            *pFapiMsgSize = 0;

            if (searchXmlNode( pNode_FapiMsg[fapiMsgIndex], &pNode_FapiMsgType, "MSG_PHY_DL_TTI_REQ") == SUCCESS )
            {
                loadDlTtiReqMsg( pNode_FapiMsgType, MAX_FAPI_MSG_SIZE, pFapiMsg, pFapiMsgSize, cellIndex );
            }
            else if (searchXmlNode( pNode_FapiMsg[fapiMsgIndex], &pNode_FapiMsgType, "MSG_PHY_TX_DATA_REQ") == SUCCESS )
            {
                loadTxDataReqMsg( pNode_FapiMsgType, MAX_FAPI_MSG_SIZE, pFapiMsg, pFapiMsgSize, pSlotConfig, cellIndex );
            }
            else if (searchXmlNode( pNode_FapiMsg[fapiMsgIndex], &pNode_FapiMsgType, "MSG_PHY_UL_DCI_REQ") == SUCCESS )
            {
                loadUlDciReqMsg( pNode_FapiMsgType, MAX_FAPI_MSG_SIZE, pFapiMsg, pFapiMsgSize, cellIndex );
            }
            else if (searchXmlNode( pNode_FapiMsg[fapiMsgIndex], &pNode_FapiMsgType, "MSG_PHY_UL_TTI_REQ") == SUCCESS )
            {
                loadUlTtiReqMsg( pNode_FapiMsgType, MAX_FAPI_MSG_SIZE, pFapiMsg, pFapiMsgSize, cellIndex );
            }
            else
            {
                XML_LOAD_DEBUG( "      Unknown FAPI Msg\n" );
            }

            if (*pFapiMsgSize > 0)
                pSlotConfig->fapiMsgCount++;
        }
    }
    else
    {
        XML_LOAD_WARNING( "      file %s : No fapiMsg found\n", pSlotConfig->txConfigFileName );
    }

    xmlFreeDoc(pDoc);
    xmlCleanupParser();

    return SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int loadTestSetup( xmlNode *pNode_TestSetup, uint8_t cellIndex )
{
    xmlNode *pNode_SlotTxCfg[MAX_MULTIPLE_XML_NODE_COUNT], *pNode_SlotNum, *pNode_TxCfgFile;
    xmlChar *pProperty, *pValue_SlotNum, *pValue_TxCfgFile;
    uint32_t slotCountToRun, slotTxCfgCount, slotTxCfgIndex, slotNum, slotNumStart, slotNumEnd, slotNumTemp, maxSlotNum;
    FILE *pFp;
    char tokenStr[MAX_XML_STRING_TOKEN_SIZE];
    char subFileName[MAX_FILEPATH_SIZE*2];
    char *pFileName, *pSubToken, *pRangeCharPos;

    XML_LOAD_TRACE( "----------- loadTestSetup for cell %u\n", cellIndex );

    slotCountToRun = 0;
    slotTxCfgCount = 0;

    if (getXmlNodeValueU32( pNode_TestSetup, "slotCountToRun", &slotCountToRun ) == SUCCESS)
    {
        XML_LOAD_DEBUG( "slotCountToRun : %u\n", slotCountToRun );
        g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.slotCountToRun = slotCountToRun;
    }

    strncpy( (char *) g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.basePath, (char *) g_macRunnerConfig.currentWorkingDir, MAX_TEST_CONFIG_FILENAME_LEN );

    if ((pFileName = getXmlNodeValueStr( pNode_TestSetup, "filenameDlIqRef" )) != NULL && strlen(pFileName) > 0)
    {
        strncpy( (char *) g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.filenameDlIqRef, pFileName, MAX_TEST_CONFIG_FILENAME_LEN );
    }

    if ((pFileName = getXmlNodeValueStr( pNode_TestSetup, "filenameDlIqOutput" )) != NULL && strlen(pFileName) > 0)
    {
        strncpy( (char *) g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.filenameDlIqOutput, pFileName, MAX_TEST_CONFIG_FILENAME_LEN );
    }

    if ((pFileName = getXmlNodeValueStr( pNode_TestSetup, "filenameUlIqRef" )) != NULL && strlen(pFileName) > 0)
    {
        strncpy( (char *) g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.filenameUlIqRef, pFileName, MAX_TEST_CONFIG_FILENAME_LEN );
    }

    if ((pFileName = getXmlNodeValueStr( pNode_TestSetup, "filenameUlResultReq" )) != NULL && strlen(pFileName) > 0)
    {
        strncpy( (char *) g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.filenameUlResultRef, pFileName, MAX_TEST_CONFIG_FILENAME_LEN );
    }

    if ((pFileName = getXmlNodeValueStr( pNode_TestSetup, "filenameUlResultOutput" )) != NULL && strlen(pFileName) > 0)
    {
        strncpy( (char *) g_macRunnerConfig.cellConfig[cellIndex].testConfigReq.filenameUlResultOutput, pFileName, MAX_TEST_CONFIG_FILENAME_LEN );
    }

    searchXmlMultipleNodes( pNode_TestSetup, pNode_SlotTxCfg, &slotTxCfgCount, "slotTxCfg" );
    if (slotTxCfgCount == 0)
    {
        XML_LOAD_WARNING( "No slotTxCfg found in loadTestSetup\n" );
        return FAILURE;
    }

    XML_LOAD_DEBUG( "Total slotTxCfg count : %u\n", slotTxCfgCount);
    maxSlotNum = 0;

    for (slotTxCfgIndex = 0; slotTxCfgIndex < slotTxCfgCount; slotTxCfgIndex++)
    {
        pProperty = xmlGetProp(pNode_SlotTxCfg[slotTxCfgIndex], (const xmlChar *) "slotNum");
        if (pProperty != NULL)
        {
            pValue_SlotNum = pProperty;
            pValue_TxCfgFile = xmlNodeGetContent(pNode_SlotTxCfg[slotTxCfgIndex]);
        }
        else
        {
            if (!searchXmlNode( pNode_SlotTxCfg[slotTxCfgIndex], &pNode_SlotNum, "slotNum"))
            {
                XML_LOAD_WARNING( "slotNum not found in slotTxCfg[%u]\n", slotTxCfgIndex );
                continue;
            }

            if (!searchXmlNode( pNode_SlotTxCfg[slotTxCfgIndex], &pNode_TxCfgFile, "txCfgFile"))
            {
                XML_LOAD_WARNING( "txCfgFile not found in slotTxCfg[%u]\n", slotTxCfgIndex );
                continue;
            }

            pValue_SlotNum = xmlNodeGetContent(pNode_SlotNum);
            pValue_TxCfgFile = xmlNodeGetContent(pNode_TxCfgFile);
        }

        sprintf( subFileName, "%s/%s", g_macRunnerConfig.basePath, pValue_TxCfgFile );

        XML_LOAD_DEBUG( "  slotTxCfg[%u] : slotNum:%s file:%s\n", slotTxCfgIndex, pValue_SlotNum, subFileName );

        if ((pFp = fopen( (char *) subFileName, "rt" )) != NULL)
        {
            fclose( pFp );

            strncpy( tokenStr, (char *) pValue_SlotNum, MAX_XML_STRING_TOKEN_SIZE );
            pSubToken = strtok(tokenStr, " \t,;\n\r");
            while (pSubToken != NULL)
            {
                slotNumStart = (uint32_t) atoi(pSubToken);
                if ((pRangeCharPos = strstr(pSubToken,"-")) != NULL)
                {
                    slotNumEnd = (uint32_t) atoi(pRangeCharPos+1);
                    if (slotNumEnd < slotNumStart)
                    {
                        slotNumTemp = slotNumEnd; 
                        slotNumEnd = slotNumStart;
                        slotNumStart = slotNumTemp;
                    }
                }
                else
                {
                    slotNumEnd = slotNumStart;
                }

                for (slotNum = slotNumStart; slotNum <= slotNumEnd; slotNum++)
                {
                    if (slotNum > maxSlotNum)
                        maxSlotNum = slotNum;

                    strncpy( g_macRunnerConfig.cellConfig[cellIndex].slotConfig[slotNum].txConfigFileName, subFileName, MAX_FILEPATH_SIZE );
                }

                pSubToken = strtok(NULL, " \t,;\n\r");
            }
        }
        else
        {
            XML_LOAD_WARNING( "    %s: cannot open file\n", subFileName );
        }

        if (pProperty != NULL)
            xmlFree(pProperty);
    }

    g_macRunnerConfig.cellConfig[cellIndex].slotConfigCount = maxSlotNum+1;
    XML_LOAD_DEBUG( "  == Final slotConfigCount : %u\n", g_macRunnerConfig.cellConfig[cellIndex].slotConfigCount );

    for (slotNum = 0; slotNum <= maxSlotNum; slotNum++)
    {
        XML_LOAD_DEBUG( "\n  Load slotNum %u :%s\n", slotNum, g_macRunnerConfig.cellConfig[cellIndex].slotConfig[slotNum].txConfigFileName );
        loadTxConfig( slotNum, cellIndex );
    }

    return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int loadPhyConfigReq( xmlNode *pNode_ConfigReq, uint8_t cellIndex )
{
    xmlNode *pNode_TlvList[MAX_MULTIPLE_XML_NODE_COUNT], *pNode_TagNum, *pNode_Value;
    xmlChar *pValue_TagNum, *pProperty;
    uint32_t tlvCount, tlvIndex, tagValue, value_U32;
    uint32_t value_U32Array[MAX_MULTIPLE_XML_NODE_COUNT], arrayCount, arrayIndex;
    uint32_t numPrachFdOccasions, prachFdOccasionIndex, numPrachTdOccasions, prachTdOccasionIndex, unusedRootSequenceCount, unusedRootSequenceIndex;
    uint32_t numAssociationPatternPeriod, associationPatternPeriodIndex, ssbMaskSize, valueStrLen, valueStrIndex, slotIndex, symbolIndex;
    uint32_t numUci2Maps, numPart1Params, sumOfSizesPart1Params, mapSize, uci2MapsIndex;
    uint32_t numberOfPrbSymbRateMatchPatterns, prbSymbRateMatchPatternsIndex;
    uint32_t numberOfCoresetRateMatchPatterns, coresetRateMatchPatternsIndex;
    uint32_t numberOfLteCrsRateMatchPatterns, lteCrsRateMatchPatternsIndex;
    uint32_t sizeMbsfnSubframeConfigList, mbsfnSubframeConfigListIndex, mbsfnSubframeConfigEntryIndex, numUlBwpIds, ulBwpIndex;
    uint32_t msgAPrachMappingConfigIndex, msgAPrachMappingConfigCatIndex, catIndex, msgAGroupAorB, grpStart, grpEnd, grpIndex;
    int32_t value_S32;
    char *p_ValueStr;
    FapiConfigInfo *p_FapiConfigInfo;
    PHY_Config *p_PhyConfig;
    SlotTimeDomainInfo *p_SlotTimeDomainInfo;
    Carrier_Config *p_CarrierConfig;
    CarrierConfigInfo *p_CarrierConfigInfo;
    Cell_Config *p_CellConfig;
    IndicationInstancePerSlotInfo *p_IndicationInstancePerSlotInfo;
    RequestInstancePerSlotInfo *p_RequestInstancesPerSlotInfo;
    SsbPowerPBCH_Config *p_SsbPowerPbchConfig;
    PRACH_Config *p_PrachConfig;
    PRACHConfigInfo *p_PrachConfigInfo;
    MSGA_PUSCH_Config *p_MSGA_PUSCH_Config;
    MSGA_PUSCH_ConfigInfo *p_MSGA_PUSCH_ConfigInfo;
    AssociationPatternPeriodInfo *p_AssociationPatternPeriodInfo;
    MSGA_PRACH_Mapping_Config *p_MSGA_PRACH_Mapping_Config;
    SSBResource_Config *p_SSBResource_Config;
    SSBResourceConfigInfo *p_SSBResourceConfigInfo;
    TDD_Config *p_TDD_Config;
    TDDPatternInfo *p_TDDPatternInfo;
    Measurement_Config *p_Measurement_Config;
    UCI_Config *p_UCI_Config;
    PRBSymRateMatchPatterns_Config *p_PRBSymRateMatchPatterns_Config;
    PRBSymRateMatchPatternInfo *p_PRBSymRateMatchPatternInfo;
    CoresetRateMatchPatterns_Config *p_CoresetRateMatchPatterns_Config;
    CoresetRateMatchPatternInfo *p_CoresetRateMatchPatternInfo;
    LTECrsRateMatchPatterns_Config *p_LTECrsRateMatchPatterns_Config;
    LTECrsRateMatchPatternsInfo *p_LTECrsRateMatchPatternsInfo;
    PUCCH_SemiStatic_Config *p_PUCCH_SemiStatic_Config;
    PUCCH_SemiStatic_ConfigInfo *p_PUCCH_SemiStatic_ConfigInfo;
    PDSCH_SemiStatic_Config *p_PDSCH_SemiStatic_Config;
    DelayManagement_Config *p_DelayManagement_Config;
    Rel16_mTRP_Config *p_Rel16_mTRP_Config;

    p_FapiConfigInfo = &g_macRunnerConfig.cellConfig[cellIndex].fapiConfigInfo;
    p_PhyConfig = &p_FapiConfigInfo->phyConfig;
    p_SlotTimeDomainInfo = &p_PhyConfig->slotTimeDomainInfo;
    p_CarrierConfig = &p_FapiConfigInfo->carrierConfig;
    p_CarrierConfigInfo = &p_CarrierConfig->carrierConfigInfo;
    p_CellConfig = &p_FapiConfigInfo->cellConfig;
    p_SsbPowerPbchConfig = &p_FapiConfigInfo->ssbPowerPbchConfig;
    p_PrachConfig = &p_FapiConfigInfo->prachConfig;
    p_PrachConfigInfo = &p_PrachConfig->prachConfigInfo;
    p_MSGA_PUSCH_Config = &p_FapiConfigInfo->msgApuschConfig;
    p_MSGA_PUSCH_ConfigInfo = &p_MSGA_PUSCH_Config->msgAPuschConfigInfo;
    p_AssociationPatternPeriodInfo = &(p_MSGA_PUSCH_Config->associationPatternPeriodInfo[0]);
    p_SSBResource_Config = &p_FapiConfigInfo->ssbResourceConfig;
    p_SSBResourceConfigInfo = &p_SSBResource_Config->ssbResourceConfigInfo;
    p_TDD_Config = &p_FapiConfigInfo->tddConfig;
    p_TDDPatternInfo = &p_TDD_Config->tddPatternInfo;
    p_Measurement_Config = &p_FapiConfigInfo->measurementConfig;
    p_UCI_Config = &p_FapiConfigInfo->uciConfig;
    p_PRBSymRateMatchPatterns_Config = &p_FapiConfigInfo->prbSymbRateMatchPatternsConfig;
    p_PRBSymRateMatchPatternInfo = &p_PRBSymRateMatchPatterns_Config->prbSymbRateMatchPatternInfo[0];
    p_CoresetRateMatchPatterns_Config = &p_FapiConfigInfo->coresetRateMatchPatternsConfig;
    p_CoresetRateMatchPatternInfo = &p_CoresetRateMatchPatterns_Config->coresetRateMatchPatternInfo[0];
    p_LTECrsRateMatchPatterns_Config = &p_FapiConfigInfo->lteCrsRateMatchPatternsConfig;
    p_LTECrsRateMatchPatternsInfo = &p_LTECrsRateMatchPatterns_Config->lteCrsRateMatchPatternInfo[0];
    p_PUCCH_SemiStatic_Config = &p_FapiConfigInfo->pucchSemiStaticConfig;
    p_PUCCH_SemiStatic_ConfigInfo = &p_PUCCH_SemiStatic_Config->pucchSemiStaticConfigInfo[0];
    p_PDSCH_SemiStatic_Config = &p_FapiConfigInfo->pdschSemiStaticConfig;
    p_DelayManagement_Config = &p_FapiConfigInfo->delayManagementConfig;
    p_Rel16_mTRP_Config = &p_FapiConfigInfo->rel16mTRPConfig;

    XML_LOAD_TRACE( "----------- loadPhyConfigReq for cell %u\n", cellIndex );

    numPrachFdOccasions = 0;
    numPrachTdOccasions = 0;

    tlvCount = 0;
     
    searchXmlMultipleNodes( pNode_ConfigReq, pNode_TlvList, &tlvCount, "TLV" );
    if (tlvCount == 0)
    {
        XML_LOAD_WARNING( "No TLV found in phyConfigReq\n" );
        return FAILURE;
    }

    XML_LOAD_DEBUG( "Total TLV count : %u\n", tlvCount );
    
    for (tlvIndex = 0; tlvIndex < tlvCount ; tlvIndex++)
    {
        pProperty = xmlGetProp(pNode_TlvList[tlvIndex], (const xmlChar *) "tag");
        if (pProperty != NULL)
        {
            pValue_TagNum = pProperty;
            pNode_Value = pNode_TlvList[tlvIndex];
        }
        else
        {
            if (!searchXmlNode( pNode_TlvList[tlvIndex], &pNode_TagNum, "tag"))
            {
                XML_LOAD_WARNING( "tag not found in TLV[%u]\n", tlvIndex );
                continue;
            }

            if (!searchXmlNode( pNode_TlvList[tlvIndex], &pNode_Value, "value"))
            {
                XML_LOAD_WARNING( "value not found in TLV[%u]\n", tlvIndex );
                continue;
            }

            pValue_TagNum = xmlNodeGetContent(pNode_TagNum);
        }

        if (strncasecmp( (char *)pValue_TagNum, "0x", 2) == 0)
            tagValue = strtol((char *)pValue_TagNum, NULL, 16 );
        else
            tagValue = strtol((char *)pValue_TagNum, NULL, 10 );

        if (pProperty != NULL)
            xmlFree(pProperty);

        XML_LOAD_DEBUG( "  TLV[%u] : tag:0x%04x\n", tlvIndex, tagValue );
        memset( value_U32Array, 0, sizeof(value_U32Array) );

        switch (tagValue)
        {
            case 0x102a : 
                p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_PHY_PROFILE_ID;

                if (getXmlNodeValueU32(pNode_Value, "phyProfileId", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    phyProfileId : %u\n", value_U32 );

                    p_PhyConfig->phyProfileId = (uint16_t) value_U32;
                }
                break;

            case 0x102b :
                p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_INDICATION_INSTANCES_PER_SLOT;

                if (getXmlNodeValueU32Array(pNode_Value, "indicationInstancePerSlot", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    indicationInstancePerSlot : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    p_IndicationInstancePerSlotInfo = &p_PhyConfig->indicationInstancesPerSlot;
                    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[0] = (uint8_t) value_U32Array[0];    // Rx_Data.IND
                    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[1] = (uint8_t) value_U32Array[1];    // CRC IND
                    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[2] = (uint8_t) value_U32Array[2];    // UCI IND
                    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[3] = (uint8_t) value_U32Array[3];    // RACH IND
                    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[4] = (uint8_t) value_U32Array[4];    // SRS IND
                    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[5] = (uint8_t) value_U32Array[5];    // DL_TTI RESP
                }
                break;

            case 0x102c :
                p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_REQUEST_INSTANCES_PER_SLOT;

                if (getXmlNodeValueU32Array(pNode_Value, "requestInstancesPerSlot", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    requestInstancesPerSlot : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    p_RequestInstancesPerSlotInfo= &p_PhyConfig->requestInstancesPerSlot;
                    p_RequestInstancesPerSlotInfo->requestInstancesPerSlot[0] = (uint8_t) value_U32Array[0];    // DL_TTI REQ
                    p_RequestInstancesPerSlotInfo->requestInstancesPerSlot[1] = (uint8_t) value_U32Array[0];    // UL_TTI REQ
                    p_RequestInstancesPerSlotInfo->requestInstancesPerSlot[2] = (uint8_t) value_U32Array[0];    // UL_DCI REQ
                    p_RequestInstancesPerSlotInfo->requestInstancesPerSlot[3] = (uint8_t) value_U32Array[0];    // TX_DATA REQ
                }
                break;

            case 0x103d :
                p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_SLOT_TIME_DOMAIN_INFO;

                if (getXmlNodeValueU32(pNode_Value, "sfn", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    sfn : %u\n", value_U32 );

                    p_SlotTimeDomainInfo->sfn = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "slot", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    slot : %u\n", value_U32 );

                    p_SlotTimeDomainInfo->slot = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "subcarrierSpacing", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    subcarrierSpacing : %u\n", value_U32 );

                    p_SlotTimeDomainInfo->subcarrierSpacing = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "cyclicPrefixType", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    cyclicPrefixType : %u\n", value_U32 );

                    p_SlotTimeDomainInfo->cyclicPrefixType = (uint8_t) value_U32;
                }
                break;

            case 0x102d :

                p_CarrierConfig->tagBitmask |= CONFIG_TAG_BITMASK_CARRIER_CONFIGURATION;

                if (getXmlNodeValueU32(pNode_Value, "dlBandwidth", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    dlBandwidth : %u\n", value_U32 );

                    p_CarrierConfigInfo->dlBandwidth = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "dlFrequency", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    dlFrequency : %u\n", value_U32 );

                    p_CarrierConfigInfo->dlFrequency = value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Value, "dlk0", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    dlk0 : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUMERLOGY_COUNT; arrayIndex++)
                    {
                        p_CarrierConfigInfo->dlk0[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32Array(pNode_Value, "dlGridSize", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    dlGridSize : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUMERLOGY_COUNT; arrayIndex++)
                    {
                        p_CarrierConfigInfo->dlGridSize[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Value, "numTxAnt", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    numTxAnt : %u\n", value_U32 );

                    p_CarrierConfigInfo->numTxAnt = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "uplinkBandwidth", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    uplinkBandwidth : %u\n", value_U32 );

                    p_CarrierConfigInfo->uplinkBandwidth = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "uplinkFrequency", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    uplinkFrequency : %u\n", value_U32 );

                    p_CarrierConfigInfo->uplinkFrequency = value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Value, "ulk0", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ulk0 : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUMERLOGY_COUNT; arrayIndex++)
                    {
                        p_CarrierConfigInfo->ulk0[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32Array(pNode_Value, "ulGridSize", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ulGridSize : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUMERLOGY_COUNT; arrayIndex++)
                    {
                        p_CarrierConfigInfo->ulGridSize[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Value, "numRxAnt", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    numRxAnt : %u\n", value_U32 );

                    p_CarrierConfigInfo->numRxAnt = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "frequencyShift7p5KHz", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    frequencyShift7p5KHz : %u\n", value_U32 );

                    p_CarrierConfigInfo->frequencyShift7p5KHz = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "powerProfile", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    powerProfile : %u\n", value_U32 );

                    p_CarrierConfigInfo->powerProfile = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "powerOffsetRsIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    powerOffsetRsIndex : %u\n", value_U32 );

                    p_CarrierConfigInfo->powerOffsetRsIndex = (uint8_t) value_U32;
                }

                break;

            case 0x100c :
                p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PHY_CELL_ID;

                if (getXmlNodeValueU32(pNode_Value, "phyCellId", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    phyCellId : %u\n", value_U32 );

                    p_CellConfig->phyCellId = (uint16_t) value_U32;
                }
                break;

            case 0x100d :

                p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_FRAME_DUPLEX_TYPE;

                if (getXmlNodeValueU32(pNode_Value, "frameDuplexType", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    frameDuplexType : %u\n", value_U32 );

                    p_CellConfig->frameDuplexType = (uint8_t) value_U32;
                }
                break;

            case 0x102e :

                p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PDSCH_TRANS_TYPE_VALIDITY;

                if (getXmlNodeValueU32(pNode_Value, "pdschTransTypeValidity", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    pdschTransTypeValidity : %u\n", value_U32 );

                    p_CellConfig->pdschTransTypeValidity = (uint8_t) value_U32;
                }
                break;

            case 0x102f :

                p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PUSCH_TRANS_TYPE_VALIDITY;

                if (getXmlNodeValueU32(pNode_Value, "puschTransTypeValidity", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    puschTransTypeValidity : %u\n", value_U32 );

                    p_CellConfig->puschTransTypeValidity = (uint8_t) value_U32;
                }
                break;

            case 0x100e :
                p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_SS_PBCH_POWER;

                if (getXmlNodeValueU32(pNode_Value, "ssPbchPower", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ssPbchPower: %u\n", value_U32 );

                    p_SsbPowerPbchConfig->ssPbchPower = value_U32;
                }
                break;

            case 0x1030 :
                p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_SS_PBCH_BLOCK_POWER_SCALING;

                if (getXmlNodeValueS32(pNode_Value, "ssPbchBlockPowerScaling", &value_S32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ssPbchBlockPowerScaling : %d\n", value_S32 );

                    p_SsbPowerPbchConfig->ssPbchBlockPowerScaling = (int16_t) value_S32;
                }
                break;

            case 0x100f :
                p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_BCH_PAYLOAD_FLAG;

                if (getXmlNodeValueU32(pNode_Value, "bchPayloadFlag", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    bchPayloadFlag : %u\n", value_U32 );

                    p_SsbPowerPbchConfig->bchPayloadFlag = (uint8_t) value_U32;
                }
                break;

            case 0x1031 :

                p_PrachConfig->tagBitmask |= CONFIG_TAG_BITMASK_PRACH_RESOURCE_CONFIGURATION;

                prachTdOccasionIndex = 0;
                unusedRootSequenceCount = 0;

                if (getXmlNodeValueU32(pNode_Value, "prachResConfigIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    prachResConfigIndex : %u\n", value_U32 );

                    p_PrachConfigInfo->prachResConfigIndex = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "prachSequenceLength", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    prachSequenceLength : %u\n", value_U32 );

                    p_PrachConfigInfo->prachSequenceLength = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "prachSubCSpacing", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    prachSubCSpacing : %u\n", value_U32 );

                    p_PrachConfigInfo->prachSubCSpacing = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "ulBwpPuschScs", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ulBwpPuschScs : %u\n", value_U32 );

                    p_PrachConfigInfo->ulBwpPuschScs = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "restrictedSetConfig", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    restrictedSetConfig : %u\n", value_U32 );

                    p_PrachConfigInfo->restrictedSetConfig = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "numPrachFdOccasions", &value_U32 ) == SUCCESS)
                {
                    numPrachFdOccasions = value_U32;
                    XML_LOAD_DEBUG( "    numPrachFdOccasions : %u\n", value_U32 );

                    p_PrachConfigInfo->numPrachFdOccasions = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "prachConfigIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    prachConfigIndex : %u\n", value_U32 );

                    p_PrachConfigInfo->prachConfigIndex = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "prachFormat", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    prachFormat : %u\n", value_U32 );

                    p_PrachConfigInfo->prachFormat = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "numPrachTdOccasions", &value_U32 ) == SUCCESS)
                {
                    numPrachTdOccasions = value_U32;
                    XML_LOAD_DEBUG( "    numPrachTdOccasions : %u\n", value_U32 );

                    p_PrachConfigInfo->numPrachTdOccasions = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "numberOfPreambles", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    numberOfPreambles : %u\n", value_U32 );

                    p_PrachConfigInfo->numberOfPreambles = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "startPreambleIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    startPreambleIndex : %u\n", value_U32 );

                    p_PrachConfigInfo->startPreambleIndex = (uint8_t) value_U32;
                }

                for (prachFdOccasionIndex = 0; prachFdOccasionIndex < numPrachFdOccasions; prachFdOccasionIndex++)
                {
                    XML_LOAD_DEBUG( "    ForNumPrachFdOccasions[%u] :\n", prachFdOccasionIndex );

                    if (getXmlNodeValueU32_Nth(pNode_Value, "prachRootSequenceIndex", &value_U32, prachFdOccasionIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "      prachRootSequenceIndex : %u\n", value_U32 );

                        p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachRootSequenceIndex = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "numRootSequences", &value_U32, prachFdOccasionIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "      numRootSequences : %u\n", value_U32 );

                        p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numRootSequences = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "prachFrequencyOffset", &value_U32, prachFdOccasionIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "      prachFrequencyOffset : %u\n", value_U32 );

                        p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachFrequencyOffset = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "prachGuardBandOffset", &value_U32, prachFdOccasionIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "      prachGuardBandOffset : %u\n", value_U32 );

                        p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachGuardBandOffset = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "prachZeroCorrConf", &value_U32, prachFdOccasionIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "      prachZeroCorrConf : %u\n", value_U32 );

                        p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachZeroCorrConf = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "numUnusedRootSequences", &value_U32, prachFdOccasionIndex ) == SUCCESS)
                    {
                        unusedRootSequenceCount = value_U32;
                        XML_LOAD_DEBUG( "      numUnusedRootSequences : %u\n", value_U32 );

                        p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences = (uint16_t) value_U32;
                    }

                    for (unusedRootSequenceIndex = 0; unusedRootSequenceIndex < unusedRootSequenceCount; unusedRootSequenceIndex++)
                    {
                        XML_LOAD_DEBUG( "      ForNnusedRootSequences[%u] :\n", unusedRootSequenceIndex );
                        if (getXmlNodeValueU32_Nth(pNode_Value, "unusedRootSequences", &value_U32, prachFdOccasionIndex*unusedRootSequenceCount+unusedRootSequenceIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "        unusedRootSequences[%u][%u] : %u\n", prachFdOccasionIndex, unusedRootSequenceIndex, value_U32 );

                            p_PrachConfig->unusedRootSequencesList[prachFdOccasionIndex][unusedRootSequenceIndex] = (uint16_t) value_U32;
                        }
                    }
                }

                if (getXmlNodeValueU32(pNode_Value, "ssbPerRach", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ssbPerRach : %u\n", value_U32 );

                    p_PrachConfig->ssbPerRach = (uint8_t) value_U32;
                }

                break;

            case 0x103e : 

                p_MSGA_PUSCH_Config->tagBitmask |= CONFIG_TAG_BITMASK_MSGA_PUSCH_CONFIGURATION;

                numAssociationPatternPeriod = 0;
                msgAGroupAorB = 0;

                if (getXmlNodeValueU32(pNode_Value, "msgApuschResConfigIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    msgApuschResConfigIndex : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->msgApuschResConfigIndex = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "msgAprachResConfigIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    msgAprachResConfigIndex : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->msgAprachResConfigIndex = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "msgAGroupAorB", &value_U32 ) == SUCCESS)
                {
                    msgAGroupAorB = value_U32;
                    XML_LOAD_DEBUG( "    msgAGroupAorB : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->msgAGroupAorB = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "nIdMsgAPusch", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    nIdMsgAPusch : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->nIdMsgAPusch = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "dmrsPorts", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    dmrsPorts : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->dmrsPorts = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "availableDmrsSequenceIds", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    availableDmrsSequenceIds : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->availableDmrsSequenceIds = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32Array(pNode_Value, "puschDmrsScramblingIds", value_U32Array, &arrayCount ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    puschDmrsScramblingIds : count:%u values:", arrayCount );
                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                    XML_LOAD_DEBUG( "\n" );

                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < 2; arrayIndex++)
                    {
                        p_MSGA_PUSCH_ConfigInfo->puschDmrsScramblingIds[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                    }
                }

                if (getXmlNodeValueU32(pNode_Value, "numPuschOcasFd", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    numPuschOcasFd : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->numPuschOcasFd = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "numPuschOcasTd", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    numPuschOcasTd : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->numPuschOcasTd = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "numPuschOcasSlots", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    numPuschOcasSlots : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->numPuschOcasSlots = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "msgAPuschTimeDomainOffset", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    msgAPuschTimeDomainOffset : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->msgAPuschTimeDomainOffset = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "nPreambles", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    nPreambles : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->nPreambles = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "associationPatternPeriod", &value_U32 ) == SUCCESS)
                {
                    numAssociationPatternPeriod = value_U32;
                    XML_LOAD_DEBUG( "    associationPatternPeriod : %u\n", value_U32 );

                    p_MSGA_PUSCH_ConfigInfo->associationPatternPeriod = (uint8_t) value_U32;
                }

                msgAPrachMappingConfigIndex = 0;
                msgAPrachMappingConfigCatIndex = 0;

                for (associationPatternPeriodIndex = 0; associationPatternPeriodIndex < numAssociationPatternPeriod; associationPatternPeriodIndex++)
                {
                    XML_LOAD_DEBUG( "    ForAssociationPatternPeriod[%u] :\n", associationPatternPeriodIndex);

                    if (getXmlNodeValueU32_Nth(pNode_Value, "numPrachSlotsforPrachtoPruAndDmrsMap", &value_U32, associationPatternPeriodIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "      numPrachSlotsforPrachtoPruAndDmrsMap : %u\n", value_U32 );

                        p_AssociationPatternPeriodInfo[associationPatternPeriodIndex].numPrachSlotsforPrachtoPruAndDmrsMap = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "prachSlotIndicesModulusApPslots", &value_U32, associationPatternPeriodIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "      prachSlotIndicesModulusApPslots : %u\n", value_U32 );

                        p_AssociationPatternPeriodInfo[associationPatternPeriodIndex].prachSlotIndicesModulusApPslots = (uint16_t) value_U32;
                    }

                    p_MSGA_PRACH_Mapping_Config = &(p_AssociationPatternPeriodInfo[associationPatternPeriodIndex].msgAPrachtoMapping);

                    for (prachFdOccasionIndex = 0; prachFdOccasionIndex < numPrachFdOccasions; prachFdOccasionIndex++)
                    {
                        if (prachFdOccasionIndex >= MAX_NUM_PRACH_FD_OCCASION)
                            break;

                        XML_LOAD_DEBUG( "      ForEach numPrachFdOccasions[%u] :\n", prachFdOccasionIndex );

                        for (prachTdOccasionIndex = 0; prachTdOccasionIndex < numPrachTdOccasions; prachTdOccasionIndex++)
                        {
                            if (prachTdOccasionIndex >= MAX_NUM_PRACH_TD_OCCASION)
                                break;

                            XML_LOAD_DEBUG( "        ForEach numPrachTdOccasions[%u] :\n", prachTdOccasionIndex );

                            if (getXmlNodeValueU32_Nth(pNode_Value, "validRO", &value_U32, msgAPrachMappingConfigIndex) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "          validRO : %u\n", value_U32 );

                                p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].validRO = (uint8_t) value_U32;
                            }

                            if (getXmlNodeValueU32Array_Nth(pNode_Value, "catAorBBitmap", value_U32Array, &arrayCount, msgAPrachMappingConfigIndex) == SUCCESS)
                            {
                                XML_LOAD_DEBUG( "          catAorBBitmap : count:%u values:", arrayCount );
                                for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                    XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                                XML_LOAD_DEBUG( "\n" );

                                for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_CAT_A_OR_B_BITMAP; arrayIndex++)
                                {
                                    p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].catAorBbimap[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                                }
                            }

                            grpStart = msgAGroupAorB;
                            grpEnd = msgAGroupAorB;

                            for (grpIndex = grpStart, catIndex = 0; grpIndex <= grpEnd && catIndex < MAX_NUM_PRACH_GROUP; grpIndex++, catIndex++)
                            {
                                XML_LOAD_DEBUG( "          ForEach grpIndex[%u] (catIndex:%u) :\n", grpIndex, catIndex );

                                if (getXmlNodeValueU32_Nth(pNode_Value, "pruPreambleIndexForStartPreambleIndex", &value_U32, msgAPrachMappingConfigCatIndex) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "            pruPreambleIndexForStartPreambleIndex : %u\n", value_U32 );

                                    p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].catIndex[catIndex].pruPreambleIndexForStartPreambleIndex = (uint16_t) value_U32;
                                }

                                if (getXmlNodeValueU32_Nth(pNode_Value, "numPuschAssocDmrsOccasionsForThisPrachRO", &value_U32, msgAPrachMappingConfigCatIndex) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "            numPuschAssocDmrsOccasionsForThisPrachRO : %u\n", value_U32 );

                                    p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].catIndex[catIndex].numPuschAssocDmrsOccasionsForThisPrachRO = (uint8_t) value_U32;
                                }

                                if (getXmlNodeValueU32Array_Nth(pNode_Value, "msgAPuschAssocDmrsOccasion", value_U32Array, &arrayCount, msgAPrachMappingConfigCatIndex) == SUCCESS)
                                {
                                    XML_LOAD_DEBUG( "            msgAPuschAssocDmrsOccasion : count:%u values:", arrayCount );
                                    for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                                        XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                                    XML_LOAD_DEBUG( "\n" );

                                    for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_NUM_PRU_ASSOC_DMRS; arrayIndex++)
                                    {
                                        p_MSGA_PRACH_Mapping_Config->mapIndex[prachFdOccasionIndex][prachTdOccasionIndex].catIndex[catIndex].msgAPuschAssocDmrsOccasion[arrayIndex] = 
                                            (uint16_t) value_U32Array[arrayIndex];
                                    }
                                }

                                msgAPrachMappingConfigCatIndex++;
                            }

                            msgAPrachMappingConfigIndex++;
                        }
                    }
                }
                break;

            case 0x1033 :

                p_SSBResource_Config->tagBitmask |= CONFIG_TAG_BITMASK_SSB_TRANSMISSION_PATTERN;

                ssbMaskSize = 0;

                if (getXmlNodeValueU32(pNode_Value, "ssbConfigIndex", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ssbConfigIndex : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->ssbConfigIndex = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "ssbOffsetPointA", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ssbOffsetPointA : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->ssbOffsetPointA = (uint16_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "betaPssProfileNR", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    betaPssProfileNR : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->betaPssProfileNR = (uint8_t) value_U32;
                }

                if (getXmlNodeValueS32(pNode_Value, "betaPssProfileSSS", &value_S32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    betaPssProfileSSS : %d\n", value_S32 );

                    p_SSBResourceConfigInfo->betaPssProfileSSS = (int16_t) value_S32;
                }

                if (getXmlNodeValueU32(pNode_Value, "ssbPeriod", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ssbPeriod : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->ssbPeriod = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "ssbSubcarrierOffset", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ssbSubcarrierOffset : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->ssbSubcarrierOffset = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "ssbFirstSymbolMappingCase", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    ssbFirstSymbolMappingCase : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->ssbFirstSymbolMappingCase = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "subcarrierSpacing", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    subcarrierSpacing : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->subcarrierSpacing = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "subCarrierSpacingCommon", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    subCarrierSpacingCommon : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->subCarrierSpacingCommon = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "ssbMaskSize", &value_U32 ) == SUCCESS)
                {
                    ssbMaskSize = value_U32;
                    XML_LOAD_DEBUG( "    ssbMaskSize : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->ssbMaskSize = (uint8_t) value_U32;
                }

                if (ssbMaskSize == 2)
                {
                    if (getXmlNodeValueU32Array(pNode_Value, "ssbMask", value_U32Array, &arrayCount ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "    ssbMask : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < 2; arrayIndex++)
                        {
                            p_SSBResourceConfigInfo->ssbMask[arrayIndex] = value_U32Array[arrayIndex];
                        }
                    }
                }
                else if (ssbMaskSize == 64)
                {
                    if (getXmlNodeValueU32Array(pNode_Value, "beamId", value_U32Array, &arrayCount ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "    beamId : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < 64; arrayIndex++)
                        {
                            p_SSBResourceConfigInfo->beamId[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }
                }

                if (getXmlNodeValueU32(pNode_Value, "lMax", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    lMax : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->lMax = (uint8_t) value_U32;
                }

                if (getXmlNodeValueU32(pNode_Value, "rmsiPresence", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    rmsiPresence : %u\n", value_U32 );

                    p_SSBResourceConfigInfo->rmsiPresence = (uint8_t) value_U32;
                }

                break;

            case 0x1035 : 

                p_TDD_Config->tagBitmask |= CONFIG_TAG_BITMASK_TDD_PATTERN;

                if (getXmlNodeValueU32(pNode_Value, "tddPeriod", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    tddPeriod : %u\n", value_U32 );

                    p_TDDPatternInfo->tddPeriod = (uint8_t) value_U32;
                }

                if ((p_ValueStr = getXmlNodeValueStr(pNode_Value, "slotConfig" )) != NULL)
                {
                    XML_LOAD_DEBUG( "    slotConfig : %s\n", p_ValueStr );

                    valueStrLen = strlen( p_ValueStr );
                    slotIndex = 0;
                    symbolIndex = 0;
                    memset( p_TDDPatternInfo->slotConfig, 0xFF, sizeof(p_TDDPatternInfo->slotConfig) );
                    for (valueStrIndex = 0; valueStrIndex < valueStrLen; valueStrIndex++)
                    {
                        if (p_ValueStr[valueStrIndex] == '0' || p_ValueStr[valueStrIndex] == 'D' || p_ValueStr[valueStrIndex] == 'd')
                            p_TDDPatternInfo->slotConfig[slotIndex][symbolIndex] = 0;
                        else if (p_ValueStr[valueStrIndex] == '1' || p_ValueStr[valueStrIndex] == 'U' || p_ValueStr[valueStrIndex] == 'u')
                            p_TDDPatternInfo->slotConfig[slotIndex][symbolIndex] = 1;
                        else if (p_ValueStr[valueStrIndex] == '2' || p_ValueStr[valueStrIndex] == 'F' || p_ValueStr[valueStrIndex] == 'f'
                          || p_ValueStr[valueStrIndex] == 'S' || p_ValueStr[valueStrIndex] == 's')
                            p_TDDPatternInfo->slotConfig[slotIndex][symbolIndex] = 2;
                        else
                            continue;
 
                        symbolIndex++;
                        if (symbolIndex >= MAX_NUM_OF_SYMBOL_PER_SLOT) 
                        {
                            symbolIndex = 0;
                            slotIndex++;
                            if (slotIndex >= MAX_TDD_PERIODICITY_SLOT_COUNT)
                                break;
                        }
                    }
                }
                break;

            case 0x1028 :

                p_Measurement_Config->tagBitmask |= CONFIG_TAG_BITMASK_RSSI_MEASUREMENT_CONFIGURATION;

                if (getXmlNodeValueU32(pNode_Value, "rssiMeasurement", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    rssiMeasurement : %u\n", value_U32 );

                    p_Measurement_Config->rssiMeasurement = (uint8_t) value_U32;
                }
                break;

            case 0x1040 :

                p_Measurement_Config->tagBitmask |= CONFIG_TAG_BITMASK_RSRP_MEASUREMENT_CONFIGURATION;

                if (getXmlNodeValueU32(pNode_Value, "rsrpMeasurement", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    rsrpMeasurement : %u\n", value_U32 );

                    p_Measurement_Config->rsrpMeasurement = (uint8_t) value_U32;
                }
                break;

            case 0x1036 :

                p_UCI_Config->tagBitmask |= CONFIG_TAG_BITMASK_UCI_INDICATION_CONFIGURATION;

                numUci2Maps = 0;
                numPart1Params = 0;
                sumOfSizesPart1Params = 0;
                mapSize = 0;

                if (getXmlNodeValueU32(pNode_Value, "numUci2Maps", &value_U32 ) == SUCCESS)
                {
                    numUci2Maps = value_U32;

                    XML_LOAD_DEBUG( "    numUci2Maps : %u\n", value_U32 );

                    p_UCI_Config->numUci2Maps = (uint8_t) value_U32;
                }

                for (uci2MapsIndex = 0; uci2MapsIndex < numUci2Maps && uci2MapsIndex < MAX_NUM_UCI2_MAPS; uci2MapsIndex++)
                {
                    XML_LOAD_DEBUG( "      ForNumUci2Maps[%u] :\n", uci2MapsIndex);

                    if (getXmlNodeValueU32_Nth(pNode_Value, "numPart1Params", &value_U32, uci2MapsIndex) == SUCCESS)
                    {
                        numPart1Params = value_U32;
                        XML_LOAD_DEBUG( "        numPart1Params : %u\n", value_U32 );
 
                        p_UCI_Config->uci2MapInfo[uci2MapsIndex].numPart1Params = (uint8_t) value_U32;
                    }

                    sumOfSizesPart1Params = 0;
                    if (getXmlNodeValueU32Array_Nth(pNode_Value, "sizesPart1ParamsValue", value_U32Array, &arrayCount, uci2MapsIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        sizesPart1ParamsValue : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < numPart1Params && arrayIndex < MAX_NUM_OF_UCI_PART1_PARAMS; arrayIndex++)
                        {
                            p_UCI_Config->uci2MapInfo[uci2MapsIndex].sizesPart1Params[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                            sumOfSizesPart1Params += p_UCI_Config->uci2MapInfo[uci2MapsIndex].sizesPart1Params[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32Array_Nth(pNode_Value, "map", value_U32Array, &arrayCount, uci2MapsIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        map : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        mapSize = (sumOfSizesPart1Params > 0) ? (2 << (sumOfSizesPart1Params-1)) : 1;

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < mapSize && arrayIndex < MAX_UCI2_MAP_SIZE; arrayIndex++)
                        {
                            p_UCI_Config->uci2MapInfo[uci2MapsIndex].map[arrayIndex] = (uint16_t) value_U32Array[arrayIndex];
                        }
                    }
                }

                break;

            case 0x1037 : 

                p_PRBSymRateMatchPatterns_Config->tagBitmask |= CONFIG_TAG_BITMASK_PRB_SYMBOL_RATE_MATCH_PATTERN;

                numberOfPrbSymbRateMatchPatterns = 0;

                if (getXmlNodeValueU32(pNode_Value, "numberOfPrbSymbRateMatchPatterns", &value_U32 ) == SUCCESS)
                {
                    numberOfPrbSymbRateMatchPatterns = value_U32;
                    if (numberOfPrbSymbRateMatchPatterns > MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT)
                        numberOfPrbSymbRateMatchPatterns = MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT;

                    XML_LOAD_DEBUG( "    numberOfPrbSymbRateMatchPatterns : %u\n", value_U32 );

                    p_PRBSymRateMatchPatterns_Config->numberOfPrbSymbRateMatchPatterns = (uint8_t) value_U32;
                }

                for (prbSymbRateMatchPatternsIndex = 0; prbSymbRateMatchPatternsIndex < numberOfPrbSymbRateMatchPatterns; prbSymbRateMatchPatternsIndex++)
                {
                    XML_LOAD_DEBUG( "      ForNumberOfPrbSymbRateMatchPatterns[%u] :\n", prbSymbRateMatchPatternsIndex );

                    if (getXmlNodeValueU32_Nth(pNode_Value, "prbSymbRateMatchPatternID", &value_U32, prbSymbRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        prbSymbRateMatchPatternID : %u\n", value_U32 );

                        p_PRBSymRateMatchPatternInfo[prbSymbRateMatchPatternsIndex].prbSymbRateMatchPatternID = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array_Nth(pNode_Value, "freqDomainRB", value_U32Array, &arrayCount, prbSymbRateMatchPatternsIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        freqDomainRB : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_RATEMATCH_BITMAP_SIZE; arrayIndex++)
                        {
                            p_PRBSymRateMatchPatternInfo[prbSymbRateMatchPatternsIndex].freqDomainRB[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "symbolsInRB", &value_U32, prbSymbRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        symbolsInRB : %u\n", value_U32 );

                        p_PRBSymRateMatchPatternInfo[prbSymbRateMatchPatternsIndex].symbolsInRB = value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "subcarrierSpacing", &value_U32, prbSymbRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                        p_PRBSymRateMatchPatternInfo[prbSymbRateMatchPatternsIndex].subcarrierSpacing = (uint8_t) value_U32;
                    }
                }

                break;

            case 0x1041 :

                p_CoresetRateMatchPatterns_Config->tagBitmask |= CONFIG_TAG_BITMASK_PRB_SYMBOL_RATE_MATCH_PATTERN;

                numberOfCoresetRateMatchPatterns = 0;

                if (getXmlNodeValueU32(pNode_Value, "numberOfCoresetRateMatchPatterns", &value_U32 ) == SUCCESS)
                {
                    numberOfCoresetRateMatchPatterns = value_U32;
                    if (numberOfCoresetRateMatchPatterns > MAX_CORESET_RATE_MATCH_PATTERN_COUNT)
                        numberOfCoresetRateMatchPatterns = MAX_CORESET_RATE_MATCH_PATTERN_COUNT;

                    XML_LOAD_DEBUG( "    numberOfCoresetRateMatchPatterns : %u\n", value_U32 );

                    p_CoresetRateMatchPatterns_Config->numberOfCoresetRateMatchPatterns = (uint8_t) value_U32;
                }

                for (coresetRateMatchPatternsIndex = 0; coresetRateMatchPatternsIndex < numberOfCoresetRateMatchPatterns; coresetRateMatchPatternsIndex++)
                {
                    XML_LOAD_DEBUG( "      ForNumberOfCoresetRateMatchPatterns[%u] :\n", coresetRateMatchPatternsIndex );

                    if (getXmlNodeValueU32_Nth(pNode_Value, "coresetRateMatchPatternID", &value_U32, coresetRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        coresetRateMatchPatternID : %u\n", value_U32 );

                        p_CoresetRateMatchPatternInfo[coresetRateMatchPatternsIndex].coresetRateMatchPatternID = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32Array_Nth(pNode_Value, "freqDomainResources", value_U32Array, &arrayCount, coresetRateMatchPatternsIndex ) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        freqDomainResources : count:%u values:", arrayCount );
                        for (arrayIndex = 0; arrayIndex < arrayCount; arrayIndex++)
                            XML_LOAD_DEBUG( "%u ", value_U32Array[arrayIndex] );
                        XML_LOAD_DEBUG( "\n" );

                        for (arrayIndex = 0; arrayIndex < arrayCount && arrayIndex < MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE; arrayIndex++)
                        {
                            p_CoresetRateMatchPatternInfo[coresetRateMatchPatternsIndex].freqDomainResources[arrayIndex] = (uint8_t) value_U32Array[arrayIndex];
                        }
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "symbolsPattern", &value_U32, coresetRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        symbolsPattern : %u\n", value_U32 );

                        p_CoresetRateMatchPatternInfo[coresetRateMatchPatternsIndex].symbolsPattern = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "subcarrierSpacing", &value_U32, coresetRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        subcarrierSpacing : %u\n", value_U32 );

                        p_CoresetRateMatchPatternInfo[coresetRateMatchPatternsIndex].subcarrierSpacing = (uint8_t) value_U32;
                    }
                }

                break;

            case 0x0138 :

                p_LTECrsRateMatchPatterns_Config->tagBitmask |= CONFIG_TAG_BITMASK_LTE_CRS_RATE_MATCH_PATTERN;

                numberOfLteCrsRateMatchPatterns = 0;
                sizeMbsfnSubframeConfigList = 0;

                if (getXmlNodeValueU32(pNode_Value, "numberOfLteCrsRateMatchPatterns", &value_U32 ) == SUCCESS)
                {
                    numberOfLteCrsRateMatchPatterns = value_U32;
                    if (numberOfLteCrsRateMatchPatterns > MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT)
                        numberOfLteCrsRateMatchPatterns = MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT;

                    XML_LOAD_DEBUG( "    numberOfLteCrsRateMatchPatterns : %u\n", value_U32 );

                    p_LTECrsRateMatchPatterns_Config->numberOfLteCrsRateMatchPatterns = (uint8_t) value_U32;
                }

                for (lteCrsRateMatchPatternsIndex = 0; lteCrsRateMatchPatternsIndex < numberOfLteCrsRateMatchPatterns; lteCrsRateMatchPatternsIndex++)
                {
                    XML_LOAD_DEBUG( "      ForNumberOfLteCrsRateMatchPatterns[%u] :\n", lteCrsRateMatchPatternsIndex );

                    if (getXmlNodeValueU32_Nth(pNode_Value, "crsRateMatchPatternID", &value_U32, lteCrsRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        crsRateMatchPatternID : %u\n", value_U32 );

                        p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].crsRateMatchPatternID = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "carrierFreqDL", &value_U32, lteCrsRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        carrierFreqDL : %u\n", value_U32 );

                        p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].carrierFreqDL = (uint16_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "carrierBandwidthDL", &value_U32, lteCrsRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        carrierBandwidthDL : %u\n", value_U32 );

                        p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].carrierBandwidthDL = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "nrofCrsPorts", &value_U32, lteCrsRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        nrofCrsPorts : %u\n", value_U32 );

                        p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].nrofCrsPorts = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "vShift", &value_U32, lteCrsRateMatchPatternsIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        vShift : %u\n", value_U32 );

                        p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].vShift = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "sizeMbsfnSubframeConfigList", &value_U32, lteCrsRateMatchPatternsIndex) == SUCCESS)
                    {
                        sizeMbsfnSubframeConfigList = value_U32;
                        if (sizeMbsfnSubframeConfigList > MAX_MBSFN_SUBFRAME_CONFIG_COUNT)
                            sizeMbsfnSubframeConfigList = MAX_MBSFN_SUBFRAME_CONFIG_COUNT;
                        XML_LOAD_DEBUG( "        sizeMbsfnSubframeConfigList : %u\n", value_U32 );

                        p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].sizeMbsfnSubframeConfigList = (uint8_t) value_U32;
                    }

                    for (mbsfnSubframeConfigListIndex = 0; mbsfnSubframeConfigListIndex < sizeMbsfnSubframeConfigList; mbsfnSubframeConfigListIndex++)
                    {
                        mbsfnSubframeConfigEntryIndex = lteCrsRateMatchPatternsIndex*numberOfLteCrsRateMatchPatterns + mbsfnSubframeConfigListIndex;

                        XML_LOAD_DEBUG( "        ForSizeMbsfnSubframeConfigList[%u] :\n", mbsfnSubframeConfigEntryIndex );

                        if (getXmlNodeValueU32_Nth(pNode_Value, "radioframeAllocationPeriod", &value_U32, mbsfnSubframeConfigEntryIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          radioframeAllocationPeriod : %u\n", value_U32 );

                            p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].mbsfnSubframeConfigInfo[mbsfnSubframeConfigListIndex].radioframeAllocationPeriod = (uint8_t) value_U32;
                        }

                        if (getXmlNodeValueU32_Nth(pNode_Value, "radioframeAllocationOffset", &value_U32, mbsfnSubframeConfigEntryIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          radioframeAllocationOffset : %u\n", value_U32 );

                            p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].mbsfnSubframeConfigInfo[mbsfnSubframeConfigListIndex].radioframeAllocationOffset = (uint8_t) value_U32;
                        }

                        if (getXmlNodeValueU32_Nth(pNode_Value, "lteFrameStructureType", &value_U32, mbsfnSubframeConfigEntryIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          lteFrameStructureType : %u\n", value_U32 );

                            p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].mbsfnSubframeConfigInfo[mbsfnSubframeConfigListIndex].lteFrameStructureType = (uint8_t) value_U32;
                        }

                        if (getXmlNodeValueU32_Nth(pNode_Value, "subframeAllocLength", &value_U32, mbsfnSubframeConfigEntryIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          subframeAllocLength : %u\n", value_U32 );

                            p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].mbsfnSubframeConfigInfo[mbsfnSubframeConfigListIndex].subframeAllocLength = (uint8_t) value_U32;
                        }

                        if (getXmlNodeValueU32_Nth(pNode_Value, "subframeAllocationBitmap", &value_U32, mbsfnSubframeConfigEntryIndex) == SUCCESS)
                        {
                            XML_LOAD_DEBUG( "          subframeAllocationBitmap : %u\n", value_U32 );

                            p_LTECrsRateMatchPatternsInfo[lteCrsRateMatchPatternsIndex].mbsfnSubframeConfigInfo[mbsfnSubframeConfigListIndex].subframeAllocationBitmap = value_U32;
                        }
                    }
                }
                break;

            case 0x1039 : 

                p_PUCCH_SemiStatic_Config->tagBitmask |= CONFIG_TAG_BITMASK_PUCCH_SEMI_STATIC_PARAMETER;

                numUlBwpIds = 0;

                if (getXmlNodeValueU32(pNode_Value, "numUlBwpIds", &value_U32 ) == SUCCESS)
                {
                    numUlBwpIds = value_U32;
                    if (numUlBwpIds > MAX_UL_BWP_COUNT)
                        numUlBwpIds = MAX_UL_BWP_COUNT;

                    XML_LOAD_DEBUG( "    numUlBwpIds : %u\n", value_U32 );

                    p_PUCCH_SemiStatic_Config->numUlBwpIds = (uint8_t) value_U32;
                }

                for (ulBwpIndex = 0; ulBwpIndex < numUlBwpIds; ulBwpIndex++)
                {
                    XML_LOAD_DEBUG( "      ForNumUlBwpIds[%u] :\n", ulBwpIndex );

                    if (getXmlNodeValueU32_Nth(pNode_Value, "pucchGroupHopping", &value_U32, ulBwpIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        pucchGroupHopping : %u\n", value_U32 );

                        p_PUCCH_SemiStatic_ConfigInfo[ulBwpIndex].pucchGroupHopping = (uint8_t) value_U32;
                    }

                    if (getXmlNodeValueU32_Nth(pNode_Value, "nIdPucchHopping", &value_U32, ulBwpIndex) == SUCCESS)
                    {
                        XML_LOAD_DEBUG( "        nIdPucchHopping : %u\n", value_U32 );

                        p_PUCCH_SemiStatic_ConfigInfo[ulBwpIndex].nIdPucchHopping = (uint16_t) value_U32;
                    }
                }

                break;

            case 0x103a :

                p_PDSCH_SemiStatic_Config->tagBitmask |= CONFIG_TAG_BITMASK_PDSCH_SEMI_STATIC_PARAMETER;

                if (getXmlNodeValueU32(pNode_Value, "pdschCbgScheme", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    pdschCbgScheme : %u\n", value_U32 );

                    p_PDSCH_SemiStatic_Config->pdschCbgScheme = (uint8_t) value_U32;
                }
                break;

            case 0x11e  :

                p_DelayManagement_Config->tagBitmask |= CONFIG_TAG_BITMASK_TIMING_WINDOW;

                if (getXmlNodeValueU32(pNode_Value, "timingWindow", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    timingWindow : %u\n", value_U32 );

                    p_DelayManagement_Config->timingWindow = (uint16_t) value_U32;
                }
                break;

            case 0x11f  :

                p_DelayManagement_Config->tagBitmask |= CONFIG_TAG_BITMASK_TIMING_INFO_MODE;

                if (getXmlNodeValueU32(pNode_Value, "timingInfoMode", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    timingInfoMode : %u\n", value_U32 );

                    p_DelayManagement_Config->timingInfoMode = (uint8_t) value_U32;
                }
                break;

            case 0x120  :

                p_DelayManagement_Config->tagBitmask |= CONFIG_TAG_BITMASK_TIMING_INFO_PERIOD;

                if (getXmlNodeValueU32(pNode_Value, "timingInfoPeriod", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    timingInfoPeriod : %u\n", value_U32 );

                    p_DelayManagement_Config->timingInfoPeriod = (uint8_t) value_U32;
                }
                break;

            case 0x103b :

                p_Rel16_mTRP_Config->tagBitmask |= CONFIG_TAG_BITMASK_NUM_TX_PORTS_TRP1;

                if (getXmlNodeValueU32(pNode_Value, "numTxPortsTRP1", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    numTxPortsTRP1 : %u\n", value_U32 );

                    p_Rel16_mTRP_Config->numTxPortsTRP1 = (uint8_t) value_U32;
                }
                break;

            case 0x103c :

                p_Rel16_mTRP_Config->tagBitmask |= CONFIG_TAG_BITMASK_NUM_RX_PORTS_TRP1;

                if (getXmlNodeValueU32(pNode_Value, "numRxPortsTRP1", &value_U32 ) == SUCCESS)
                {
                    XML_LOAD_DEBUG( "    numRxPortsTRP1 : %u\n", value_U32 );

                    p_Rel16_mTRP_Config->numRxPortsTRP1 = (uint8_t) value_U32;
                }
                break;

            default : 
                XML_LOAD_DEBUG( "tag 0x%04x : unknown\n", tagValue );
                break;
        }   
    }

    return SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int parseXmlFile( char *pFileName )
{
    xmlDoc *pDoc;
    xmlNode *pRoot, *pNode_TestSetup, *pNode_ConfigReq;
    xmlNode *pNode_CellInfo[MAX_MULTIPLE_XML_NODE_COUNT];
    xmlChar *pProperty;
    uint8_t cellIndex;
    uint32_t cellInfoCount, cellInfoIndex;
    char currentWorkingDir[MAX_FILEPATH_SIZE];
    char tempStr[MAX_FILEPATH_SIZE*3];

    getcwd( currentWorkingDir, MAX_FILEPATH_SIZE );
    strncpy( g_macRunnerConfig.basePath, pFileName, MAX_FILEPATH_SIZE );
    dirname(g_macRunnerConfig.basePath);
    if (strcmp(g_macRunnerConfig.basePath, pFileName) == 0)
    {
        strcpy( g_macRunnerConfig.basePath, "." );
        strncpy( g_macRunnerConfig.currentWorkingDir, currentWorkingDir, MAX_FILEPATH_SIZE );
    }
    else
    {
        sprintf( tempStr, "%s/%s", currentWorkingDir, g_macRunnerConfig.basePath ); 
        strncpy( g_macRunnerConfig.currentWorkingDir, tempStr, MAX_FILEPATH_SIZE );
    }
    XML_LOAD_WARNING( "currentWorkingDir : %s\n", g_macRunnerConfig.currentWorkingDir );
    XML_LOAD_WARNING( "basePath          : %s\n", g_macRunnerConfig.basePath );

    pDoc = xmlReadFile(pFileName, NULL, 0);
    if (!pDoc)
    {
        XML_LOAD_WARNING( "xmlReadFile failed for file %s\n", pFileName );
        return FAILURE;
    }

    pRoot = xmlDocGetRootElement(pDoc);
    if (!pRoot)
    {
        XML_LOAD_WARNING( "xmlDocGetRootElement failed for file %s\n", pFileName );
        return FAILURE;
    }

    // printXmlNodes(pRoot, 0 );

    cellInfoCount = 0;
    searchXmlMultipleNodes( pRoot, pNode_CellInfo, &cellInfoCount, "cellConfig" );
    if (cellInfoCount == 0)
    {
        XML_LOAD_WARNING( "cellConfig not found\n" );
        return FAILURE;
    }

    for (cellInfoIndex = 0; cellInfoIndex < cellInfoCount; cellInfoIndex++)
    {
        cellIndex = 0;
        pProperty = xmlGetProp(pNode_CellInfo[cellInfoIndex], (const xmlChar *) "cellIndex");
        if (pProperty != NULL)
            cellIndex = (uint8_t) atoi( (char *) pProperty );

        if (!searchXmlNode( pNode_CellInfo[cellInfoIndex], &pNode_ConfigReq, "MSG_PHY_CONFIG_REQ"))
        {
            XML_LOAD_WARNING( "Node not found : MSG_PHY_CONFIG_REQ\n" );
            return FAILURE;
        }

        if (loadPhyConfigReq( pNode_ConfigReq, cellIndex ) != SUCCESS)
        {
            XML_LOAD_WARNING( "phyConfigReq loading failed\n" );
            return FAILURE;
        }

        if (!searchXmlNode( pNode_CellInfo[cellInfoIndex], &pNode_TestSetup, "testSetup"))
        {
            XML_LOAD_WARNING( "Node not found : TestSetup\n" );
            return FAILURE;
        }

        if (loadTestSetup( pNode_TestSetup, cellIndex ) != SUCCESS)
        {
            XML_LOAD_WARNING( "testSetup loading failed\n" );
            return FAILURE;
        }
     
        g_macRunnerConfig.cellConfig[cellIndex].cellStatus = CELL_STATUS_CONFIGURED;
    }     

    xmlFreeDoc(pDoc);
    xmlCleanupParser();

    return SUCCESS;
}


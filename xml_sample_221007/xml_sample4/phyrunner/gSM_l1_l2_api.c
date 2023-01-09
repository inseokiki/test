/**********************************************************************
 * SmartRAN CONFIDENTIAL
 * Copyright 2021-2025 SmartRAN All Rights Reserved.
 * gSM PHY parameter according to SCF 5G FAPI:PHY API(SCF222.10.03, 2021-05)
 * file gSM_l1_l2_api_test.c
 * Version: PHY_21.09
 ***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <math.h>

#include "common.h"
#include "gSM_l1_l2_api.h"

#define FAPI_Output(...) printf(__VA_ARGS__)
#define FAPI_Error(...) printf(__VA_ARGS__)

FapiParamInfo  g_Mac_FapiParamRecvInfo;
FapiConfigInfo g_Phy_FapiConfigRecvInfo;

uint32_t g_phyFeature_FapiV4MuMimoGroupSupport = FALSE;

///////////////////////////////////////////////////////////////////////////////////////////
// TLV AddTag unit routines for both MAC and PHY

uint16_t tlv_AddTag_U16( uint16_t tag, uint16_t value, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    TLV_Header *p_TlvHeader;
    uint32_t tlvValueLength, tlvPaddingLength, tlvLength;
    uint16_t *p_TlvValue;

    p_TlvHeader = *pp_TlvHeader;

    tlvValueLength = sizeof(uint16_t);
    tlvPaddingLength = sizeof(uint32_t)-tlvValueLength;
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->tag = tag;
    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;
    p_TlvValue = (uint16_t *) p_TlvHeader->value;
    *p_TlvValue = value;
   
    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}


uint16_t tlv_AddTag_U8( uint16_t tag, uint8_t value, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    TLV_Header *p_TlvHeader;
    uint32_t tlvValueLength, tlvPaddingLength, tlvLength;

    p_TlvHeader = *pp_TlvHeader;

    tlvValueLength = sizeof(uint8_t);
    tlvPaddingLength = sizeof(uint32_t)-tlvValueLength;
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->tag = tag;
    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;
    p_TlvHeader->value[0] = value;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_U32( uint16_t tag, uint32_t value, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    TLV_Header *p_TlvHeader;
    uint32_t tlvValueLength, tlvLength, *p_TlvValue;

    p_TlvHeader = *pp_TlvHeader;

    tlvValueLength = sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength;

    p_TlvHeader->tag = tag;
    p_TlvHeader->length = tlvValueLength;
    p_TlvValue = (uint32_t *) p_TlvHeader->value;
    *p_TlvValue = value;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

///////////////////////////////////////////////////////////////////////////////////////////
// TLV AddTag unit routines for PHY

uint16_t tlv_AddTag_NumConfigTlvsToReport( uint16_t numConfigTLVsToReport, ConfigTLVToReport *p_ConfigTLVToReportList, 
                                                  TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t *p_TlvValue, configTLVToReportIndex, tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    ConfigTLVToReport *p_ConfigTlvToReport;

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_NUM_CONFIG_TLVS_TO_REPORT;
    tlvValueLength = sizeof(uint16_t);
    p_TlvValue = (uint16_t *) p_TlvHeader->value;
    *p_TlvValue = numConfigTLVsToReport;

    for (configTLVToReportIndex = 0; configTLVToReportIndex < numConfigTLVsToReport; configTLVToReportIndex++)
    {
        // Set sub TLVs for NumConfigTLVsToReport
        p_ConfigTlvToReport = (ConfigTLVToReport *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        p_ConfigTlvToReport->tag = p_ConfigTLVToReportList[configTLVToReportIndex].tag;
        p_ConfigTlvToReport->length = p_ConfigTLVToReportList[configTLVToReportIndex].length;
        p_ConfigTlvToReport->value = p_ConfigTLVToReportList[configTLVToReportIndex].value;

        tlvValueLength += sizeof(ConfigTLVToReport);
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_PowerProfiles( uint8_t powerProfilesSupported, PowerProfileInfo *p_PowerProfileInfoList, 
                                          TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t powerProfileIndex, tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    PowerProfileInfo *p_PowerProfileInfo;

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_POWER_PROFILES;
    tlvValueLength = sizeof(uint8_t);
    p_TlvHeader->value[0] = powerProfilesSupported;

    for (powerProfileIndex = 0; powerProfileIndex < POWER_PROFILE_SIGNAL_INDEX_COUNT; powerProfileIndex++)
    {
        // Set sub TLVs for PowerProfileInfo
        p_PowerProfileInfo = (PowerProfileInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        p_PowerProfileInfo->referenceRS = p_PowerProfileInfoList[powerProfileIndex].referenceRS;
        p_PowerProfileInfo->powerOffsetMin = p_PowerProfileInfoList[powerProfileIndex].powerOffsetMin;
        p_PowerProfileInfo->powerOffsetMax = p_PowerProfileInfoList[powerProfileIndex].powerOffsetMax;

        tlvValueLength += sizeof(PowerProfileInfo);
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_CapabilityValidity( Capability_Validity *p_CapabilityValidity, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t *p_TlvValue, numCapabilities, capabilityValidityEntryIndex, tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    CapabilityValidityEntry *p_CapabilityValidityEntry;

    p_TlvHeader = *pp_TlvHeader;

    numCapabilities = p_CapabilityValidity->numCapabilities;
    if (numCapabilities > MAX_CAPABILITY_VALIDITY_ENTRY_COUNT)
    {
        FAPI_Error( "AddTag: Too large numCapabilities %u : limit to %u\n",
            numCapabilities, MAX_CAPABILITY_VALIDITY_ENTRY_COUNT );
        numCapabilities = MAX_CAPABILITY_VALIDITY_ENTRY_COUNT;
    }

    p_TlvHeader->tag = TAG_CAPABILITIES;
    tlvValueLength = sizeof(uint16_t);
    p_TlvValue = (uint16_t *) p_TlvHeader->value;
    *p_TlvValue = numCapabilities;

    for (capabilityValidityEntryIndex = 0; capabilityValidityEntryIndex < numCapabilities; capabilityValidityEntryIndex++)
    {
        p_CapabilityValidityEntry = (CapabilityValidityEntry *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        *p_CapabilityValidityEntry = p_CapabilityValidity->capabilityValidityEntryList[capabilityValidityEntryIndex];
        tlvValueLength += sizeof(CapabilityValidityEntry);
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_SupportedPhyProfiles( uint16_t numPhyProfiles, uint16_t numDlBbPorts, uint16_t numUlBbPorts,
                                          uint8_t *p_MaxNumPhysList, SupportedPhyProfileInfo *p_SupportedPhyProfileInfoList,
                                          TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t *p_Value_U16, tlvValueLength, tlvPaddingLength, tlvLength, phyProfileIndex, numPhyIndex, portRangeIndex;
    uint8_t *p_Value_U8, maxNumPhys;
    TLV_Header *p_TlvHeader;
    SupportedPhyProfileInfo *p_SupportedPhyProfileInfo;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_SUPPORTED_PHY_PROFILES;
    tlvValueLength = 0;

    p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_Value_U16 = numPhyProfiles;
    tlvValueLength += sizeof(uint16_t);

    p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_Value_U16 = numDlBbPorts;
    tlvValueLength += sizeof(uint16_t);

    p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_Value_U16 = numUlBbPorts;
    tlvValueLength += sizeof(uint16_t);

    for (phyProfileIndex = 0; phyProfileIndex < numPhyProfiles; phyProfileIndex++)
    {
        maxNumPhys = p_MaxNumPhysList[phyProfileIndex];

        p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        *p_Value_U8 = maxNumPhys;
        tlvValueLength += sizeof(uint8_t);

        for (numPhyIndex = 0; numPhyIndex < maxNumPhys; numPhyIndex++)
        {
            p_SupportedPhyProfileInfo = p_SupportedPhyProfileInfoList + phyProfileIndex*MAX_NUM_PHY_PROFILES + numPhyIndex;

            p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
            *p_Value_U8 = p_SupportedPhyProfileInfo->numDlPortRanges;
            tlvValueLength += sizeof(uint8_t);
 
            p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
            for (portRangeIndex = 0; portRangeIndex < p_SupportedPhyProfileInfo->numDlPortRanges; portRangeIndex++)
            {
                *p_Value_U16 = p_SupportedPhyProfileInfo->dlPortRangeStart[portRangeIndex];
                p_Value_U16++;
                tlvValueLength += sizeof(uint16_t);
            }
            for (portRangeIndex = 0; portRangeIndex < p_SupportedPhyProfileInfo->numDlPortRanges; portRangeIndex++)
            {
                *p_Value_U16 = p_SupportedPhyProfileInfo->dlPortRangeLen[portRangeIndex];
                p_Value_U16++;
                tlvValueLength += sizeof(uint16_t);
            }
 
            p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
            *p_Value_U8 = p_SupportedPhyProfileInfo->numUlPortRanges;
            tlvValueLength += sizeof(uint8_t);
 
            p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
            for (portRangeIndex = 0; portRangeIndex < p_SupportedPhyProfileInfo->numUlPortRanges; portRangeIndex++)
            {
                *p_Value_U16 = p_SupportedPhyProfileInfo->ulPortRangeStart[portRangeIndex];
                p_Value_U16++;
                tlvValueLength += sizeof(uint16_t);
            }
            for (portRangeIndex = 0; portRangeIndex < p_SupportedPhyProfileInfo->numUlPortRanges; portRangeIndex++)
            {
                *p_Value_U16 = p_SupportedPhyProfileInfo->ulPortRangeLen[portRangeIndex];
                p_Value_U16++;
                tlvValueLength += sizeof(uint16_t);
            }
        }
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_FapiProtocolVersion( FAPIProtocolVersion *p_FapiProtocolVersion, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    FAPIProtocolVersion *p_FapiProtocolVersionDest;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_FAPI_PROTOCOL_VERSION;
    tlvValueLength = 0;

    p_FapiProtocolVersionDest = (FAPIProtocolVersion *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_FapiProtocolVersionDest = *p_FapiProtocolVersion;
    tlvValueLength += sizeof(FAPIProtocolVersion);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_NumberPrbSymBitmapsPerSlot( PrbSymBitmapsPerSlotInfo *p_MaxNumberPrbSymBitmapsPerSlot, 
                                                TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    PrbSymBitmapsPerSlotInfo *p_PrbSymBitmapsPerSlotInfoDest;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_MAX_NUMBER_PRB_SYM_BITMAPS_PER_SLOT;
    tlvValueLength = 0;

    p_PrbSymBitmapsPerSlotInfoDest = (PrbSymBitmapsPerSlotInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_PrbSymBitmapsPerSlotInfoDest = *p_MaxNumberPrbSymBitmapsPerSlot;
    tlvValueLength += sizeof(PrbSymBitmapsPerSlotInfo);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_MoreThanOneIndicationPerSlot( IndicationInstancePerSlotInfo *p_MoreThanOneIndicationPerSlot, 
                                                  TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    IndicationInstancePerSlotInfo *p_IndicationInstancePerSlotInfoDest;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_MORE_THAN_ONE_INDICATION_PER_SLOT;
    tlvValueLength = 0;

    p_IndicationInstancePerSlotInfoDest = (IndicationInstancePerSlotInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_IndicationInstancePerSlotInfoDest = *p_MoreThanOneIndicationPerSlot;
    tlvValueLength += sizeof(IndicationInstancePerSlotInfo);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_MoreThanOneRequestPerSlot( RequestInstancePerSlotInfo *p_MoreThanOneRequestPerSlot, 
                                               TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    RequestInstancePerSlotInfo *p_RequestInstancePerSlotInfoDest;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_MORE_THAN_ONE_REQUEST_PER_SLOT;
    tlvValueLength = 0;

    p_RequestInstancePerSlotInfoDest = (RequestInstancePerSlotInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_RequestInstancePerSlotInfoDest = *p_MoreThanOneRequestPerSlot;
    tlvValueLength += sizeof(RequestInstancePerSlotInfo);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_ParingDfeProfileParam( ParingDfeProfileParam *p_ParingDfeProfileParam, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t *p_Value_U16, tlvValueLength, tlvPaddingLength, tlvLength, profileValidityMapSize;
    uint8_t *p_Value_U8;
    TLV_Header *p_TlvHeader;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_PAIRING_DFE_PROFILES;
    tlvValueLength = 0;

    p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_Value_U16 = p_ParingDfeProfileParam->numPhyProfiles;
    tlvValueLength += sizeof(uint16_t);

    p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_Value_U16 = p_ParingDfeProfileParam->numDfeProfiles;
    tlvValueLength += sizeof(uint16_t);

    profileValidityMapSize = (p_ParingDfeProfileParam->numPhyProfiles * p_ParingDfeProfileParam->numDfeProfiles + 7) / 8;
    if (profileValidityMapSize > MAX_PROFILE_VALIDITY_MAP_SIZE)
    {
        FAPI_Error( "AddTag: Too large profileValidityMapSize %u : limit to %u\n", 
            profileValidityMapSize, MAX_PROFILE_VALIDITY_MAP_SIZE );
        profileValidityMapSize = MAX_PROFILE_VALIDITY_MAP_SIZE;
    }

    p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    memcpy( p_Value_U8, p_ParingDfeProfileParam->profileValidityMap, profileValidityMapSize );
    tlvValueLength += profileValidityMapSize;

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

///////////////////////////////////////////////////////////////////////////////////////////
// TLV AddTag unit routines for MAC

uint16_t tlv_AddTag_IndicationInstancePerSlotInfo( IndicationInstancePerSlotInfo *p_IndicationInstancePerSlotInfo, 
                                                   TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    IndicationInstancePerSlotInfo *p_IndicationInstancePerSlotInfoDest;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_INDICATION_INSTANCES_PER_SLOT;
    tlvValueLength = 0;

    p_IndicationInstancePerSlotInfoDest = (IndicationInstancePerSlotInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_IndicationInstancePerSlotInfoDest = *p_IndicationInstancePerSlotInfo;
    tlvValueLength += sizeof(IndicationInstancePerSlotInfo);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_RequestInstancePerSlotInfo( RequestInstancePerSlotInfo *p_RequestInstancePerSlotInfo, 
                                                TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    RequestInstancePerSlotInfo *p_RequestInstancePerSlotInfoDest;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_REQUEST_INSTANCES_PER_SLOT;
    tlvValueLength = 0;

    p_RequestInstancePerSlotInfoDest = (RequestInstancePerSlotInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_RequestInstancePerSlotInfoDest = *p_RequestInstancePerSlotInfo;
    tlvValueLength += sizeof(RequestInstancePerSlotInfo);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_SlotTimeDomainInfo( SlotTimeDomainInfo *p_SlotTimeDomainInfo,
                                        TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    SlotTimeDomainInfo *p_SlotTimeDomainInfoDest;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_SLOT_TIME_DOMAIN_INFO;
    tlvValueLength = 0;

    p_SlotTimeDomainInfoDest = (SlotTimeDomainInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_SlotTimeDomainInfoDest = *p_SlotTimeDomainInfo;
    tlvValueLength += sizeof(SlotTimeDomainInfo);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_CarrierConfiguration( CarrierConfigInfo *p_CarrierConfigInfo, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    CarrierConfigInfo *p_CarrierConfigInfoDest;

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_CARRIER_CONFIGURATION;
    tlvValueLength = sizeof(CarrierConfigInfo);

    
    p_CarrierConfigInfoDest = (CarrierConfigInfo *) p_TlvHeader->value;
    *p_CarrierConfigInfoDest = *p_CarrierConfigInfo;

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);    
    return tlvLength;
}

uint16_t tlv_AddTag_PrachResourceConfiguration( PRACH_Config *p_PrachConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength, prachFdOccasionIndex;
    uint16_t *p_Value_U16, unusedRootSequenceCount, unusedRootSequenceIndex;
    uint8_t *p_Value_U8;
    TLV_Header *p_TlvHeader;
    PRACHConfigInfo *p_PrachConfigInfo;
    PRACHOccasionInfo *p_PrachOccasionInfo;

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_PRACH_RESOURCE_CONFIGURATION;
    tlvValueLength = sizeof(PRACHConfigInfo);

    p_PrachConfigInfo = (PRACHConfigInfo *) p_TlvHeader->value;
    *p_PrachConfigInfo = p_PrachConfig->prachConfigInfo;
   
    
    for (prachFdOccasionIndex = 0; prachFdOccasionIndex < p_PrachConfig->prachConfigInfo.numPrachFdOccasions; prachFdOccasionIndex++)
    {
        p_PrachOccasionInfo = (PRACHOccasionInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        *p_PrachOccasionInfo = p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex];
        
        tlvValueLength += sizeof(PRACHOccasionInfo);

        unusedRootSequenceCount = p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences;
        p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        for (unusedRootSequenceIndex = 0; unusedRootSequenceIndex < unusedRootSequenceCount; unusedRootSequenceIndex++)
        {
            *p_Value_U16 = p_PrachConfig->unusedRootSequencesList[prachFdOccasionIndex][unusedRootSequenceIndex];
            
            p_Value_U16++;
            tlvValueLength += sizeof(uint16_t);
        }
    }

    p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    *p_Value_U8 = p_PrachConfig->ssbPerRach;
   
    tlvValueLength += sizeof(uint8_t);
    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    if (p_NumOfTLV != NULL)
        (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_MultiplePrachConfiguration( MultiPRACH_Config *p_MultiPrachConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    uint16_t *p_Value_U16, numPrachConfigurations, multiPrachConfigInfoIndex;
    TLV_Header *p_TlvHeader;
    PRACH_Config *p_PrachConfig;
    numPrachConfigurations = p_MultiPrachConfig->numPrachConfigurations;
    if (numPrachConfigurations > MAX_MULTI_PRACH_CONFIG_COUNT)
    {
        FAPI_Error( "AddTag: Too large numPrachConfigurations %u : limit to %u\n",
            numPrachConfigurations, MAX_MULTI_PRACH_CONFIG_COUNT);
        numPrachConfigurations = MAX_MULTI_PRACH_CONFIG_COUNT;
    }

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_MULTIPLE_PRACH_CONFIGURATION;
    p_Value_U16 = (uint16_t *) p_TlvHeader->value;
    *p_Value_U16 = numPrachConfigurations;
    tlvValueLength = sizeof(uint16_t);

    p_TlvHeader = (TLV_Header *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    for (multiPrachConfigInfoIndex = 0; multiPrachConfigInfoIndex < numPrachConfigurations; multiPrachConfigInfoIndex++)
    {
        
        p_PrachConfig = &p_MultiPrachConfig->prachConfig[multiPrachConfigInfoIndex];
        tlvValueLength += tlv_AddTag_PrachResourceConfiguration( p_PrachConfig, &p_TlvHeader, NULL );
            
   }
    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;
    
    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;
    
    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);
    return tlvLength;
}

uint16_t tlv_AddTag_MsgAPuschConfiguration( MSGA_PUSCH_Config *p_MsgAPuschConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength, numAssociationPatternPeriod, associationPatternPeriodIndex;
    TLV_Header *p_TlvHeader;
    MSGA_PUSCH_ConfigInfo *p_MsgAPuschConfigInfo;
    AssociationPatternPeriodInfo *p_AssociationPatternPeriodInfo;
    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_MSGA_PUSCH_CONFIGURATION;
    p_MsgAPuschConfigInfo = (MSGA_PUSCH_ConfigInfo*) p_TlvHeader->value;
    *p_MsgAPuschConfigInfo = p_MsgAPuschConfig->msgAPuschConfigInfo;
    tlvValueLength = sizeof(MSGA_PUSCH_ConfigInfo);

    numAssociationPatternPeriod = p_MsgAPuschConfigInfo->associationPatternPeriod;
    for (associationPatternPeriodIndex = 0; associationPatternPeriodIndex < numAssociationPatternPeriod; associationPatternPeriodIndex++)
    {
        p_AssociationPatternPeriodInfo = (AssociationPatternPeriodInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        *p_AssociationPatternPeriodInfo = p_MsgAPuschConfig->associationPatternPeriodInfo[associationPatternPeriodIndex];

        tlvValueLength += sizeof(AssociationPatternPeriodInfo);
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;
    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;
    
    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);
    return tlvLength;
}

uint16_t tlv_AddTag_SsbResourceConfig( SSBResourceConfigInfo *p_SsbResourceConfigInfo, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    SSBResourceConfigInfo *p_SsbResourceConfigInfoDest;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_SSB_TRANSMISSION_PATTERN;
    p_SsbResourceConfigInfoDest = (SSBResourceConfigInfo *) p_TlvHeader->value;
    *p_SsbResourceConfigInfoDest= *p_SsbResourceConfigInfo;
    tlvValueLength = sizeof(SSBResourceConfigInfo);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    if (p_NumOfTLV != NULL)
        (*p_NumOfTLV)++;

    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_MultipleSsbConfig( MultiSSBResource_Config *p_MultiSsbResourceConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV)
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength, multiSsbResourceConfigIndex;
    uint8_t numSsbConfigurations;
    TLV_Header *p_TlvHeader;
    SSBResource_Config *p_SsbResourceConfig;

    numSsbConfigurations = p_MultiSsbResourceConfig->numSsbConfigurations;
    if (numSsbConfigurations > MAX_MULTI_SSB_CONFIG_COUNT)
    {
        FAPI_Error( "AddTag: Too large numSsbConfigurations %u : limit to %u\n",
            numSsbConfigurations, MAX_MULTI_PRACH_CONFIG_COUNT);
        numSsbConfigurations = MAX_MULTI_SSB_CONFIG_COUNT;
    }

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_MULTIPLE_SSB_CONFIGURATION;
    p_TlvHeader->value[0] = numSsbConfigurations;
    tlvValueLength = sizeof(uint8_t);

    p_TlvHeader = (TLV_Header *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
    for (multiSsbResourceConfigIndex = 0; multiSsbResourceConfigIndex < numSsbConfigurations; multiSsbResourceConfigIndex++)
    {
        p_SsbResourceConfig = &p_MultiSsbResourceConfig->ssbResourceConfig[multiSsbResourceConfigIndex];
        tlvValueLength += tlv_AddTag_SsbResourceConfig( &p_SsbResourceConfig->ssbResourceConfigInfo, &p_TlvHeader, NULL );
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_TddPattern( TDD_Config *p_TddConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    TLV_Header *p_TlvHeader;
    TDDPatternInfo *p_TddPatternInfo;

    p_TlvHeader = *pp_TlvHeader;
    p_TlvHeader->tag = TAG_TDD_PATTERN;
    p_TddPatternInfo = (TDDPatternInfo *) p_TlvHeader->value;
    *p_TddPatternInfo = p_TddConfig->tddPatternInfo;
    tlvValueLength = sizeof(TDDPatternInfo);

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;

    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_UciConfig( UCI_Config *p_UciConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength, numUci2Maps;
    uint16_t *p_Value_U16, uci2MapsIndex, part1ParamsIndex, sumOfSizesPart1Params, mapSize;
    uint8_t *p_Value_U8, numPart1Params, sizesPart1ParamsValue;
    TLV_Header *p_TlvHeader;

    numUci2Maps = p_UciConfig->numUci2Maps;
    if (numUci2Maps > MAX_NUM_UCI2_MAPS)
    {
        FAPI_Error( "AddTag: Too large numUci2Maps %u : limit to %u\n",
            numUci2Maps, MAX_NUM_UCI2_MAPS);
        numUci2Maps = MAX_NUM_UCI2_MAPS;
    }

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_UCI_INDICATION_CONFIGURATION;
    p_Value_U16 = (uint16_t *) p_TlvHeader->value;
    *p_Value_U16 = numUci2Maps;
    tlvValueLength = sizeof(uint16_t);

    for (uci2MapsIndex = 0; uci2MapsIndex < numUci2Maps; uci2MapsIndex++)
    {
        numPart1Params = p_UciConfig->uci2MapInfo[uci2MapsIndex].numPart1Params;

        p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        *p_Value_U8 = numPart1Params;
        tlvValueLength += sizeof(uint8_t);

        p_Value_U8++;
        sumOfSizesPart1Params = 0;
        for (part1ParamsIndex = 0; part1ParamsIndex < numPart1Params; part1ParamsIndex++)
        {
            sizesPart1ParamsValue = p_UciConfig->uci2MapInfo[uci2MapsIndex].sizesPart1Params[part1ParamsIndex];
            *p_Value_U8 = sizesPart1ParamsValue;
            tlvValueLength += sizeof(uint8_t);
            p_Value_U8++;
             
            sumOfSizesPart1Params += sizesPart1ParamsValue;
        }

        if (sumOfSizesPart1Params > MAX_SUM_OF_SIZES_PART1_PARAMS)
        {
            FAPI_Error( "AddTag: Too large sumOfSizesPart1Params %u : limit to %u\n",
                sumOfSizesPart1Params, MAX_SUM_OF_SIZES_PART1_PARAMS);
            sumOfSizesPart1Params = MAX_SUM_OF_SIZES_PART1_PARAMS;
        }

        // Calculate mapSize as 2^sumOfSizesPart1Params
        mapSize = (sumOfSizesPart1Params > 0) ? (2 << (sumOfSizesPart1Params-1)) : 1;
        if (mapSize > MAX_UCI2_MAP_SIZE)
        {
            FAPI_Error( "AddTag: Too large mapSize %u : limit to %u\n",
                mapSize, MAX_UCI2_MAP_SIZE );
            mapSize = MAX_UCI2_MAP_SIZE;
        }

        memcpy( p_Value_U8, p_UciConfig->uci2MapInfo[uci2MapsIndex].map, mapSize*sizeof(uint16_t) );
       
        tlvValueLength += mapSize*sizeof(uint16_t);
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;

}

uint16_t tlv_AddTag_PrbSymbolRateMatchPattern( PRBSymRateMatchPatterns_Config *p_PrbSymbRateMatchPatternsConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength, prbSymbRateMatchPatternInfoIndex;
    uint8_t numberOfPrbSymbRateMatchPatterns;
    TLV_Header *p_TlvHeader;
    PRBSymRateMatchPatternInfo *p_PrbSymbRateMatchPatternInfo;

    numberOfPrbSymbRateMatchPatterns = p_PrbSymbRateMatchPatternsConfig->numberOfPrbSymbRateMatchPatterns;
    if (numberOfPrbSymbRateMatchPatterns > MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT)
    {
        FAPI_Error( "AddTag: Too large numberOfPrbSymbRateMatchPatterns%u : limit to %u\n",
            numberOfPrbSymbRateMatchPatterns, MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT);
        numberOfPrbSymbRateMatchPatterns = MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT;
    }

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_PRB_SYMBOL_RATE_MATCH_PATTERN;
    p_TlvHeader->value[0] = numberOfPrbSymbRateMatchPatterns;
    tlvValueLength = sizeof(uint8_t);

    for (prbSymbRateMatchPatternInfoIndex = 0; prbSymbRateMatchPatternInfoIndex< numberOfPrbSymbRateMatchPatterns; prbSymbRateMatchPatternInfoIndex++)
    {
        p_PrbSymbRateMatchPatternInfo = (PRBSymRateMatchPatternInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        *p_PrbSymbRateMatchPatternInfo = p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex]; 
        tlvValueLength += sizeof(PRBSymRateMatchPatternInfo);
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_CoresetRateMatchPattern( CoresetRateMatchPatterns_Config *p_CoresetRateMatchPatternsConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength, coresetRateMatchPatternInfoIndex;
    uint8_t numberOfCoresetRateMatchPatterns;
    TLV_Header *p_TlvHeader;
    CoresetRateMatchPatternInfo *p_CoresetRateMatchPatternInfo;

    numberOfCoresetRateMatchPatterns = p_CoresetRateMatchPatternsConfig->numberOfCoresetRateMatchPatterns;
    if (numberOfCoresetRateMatchPatterns > MAX_CORESET_RATE_MATCH_PATTERN_COUNT)
    {
        FAPI_Error( "AddTag: Too large numberOfCoresetRateMatchPatterns %u : limit to %u\n",
            numberOfCoresetRateMatchPatterns, MAX_CORESET_RATE_MATCH_PATTERN_COUNT);
        numberOfCoresetRateMatchPatterns = MAX_CORESET_RATE_MATCH_PATTERN_COUNT;
    }

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_CORESET_RATE_MATCH_PATTERN;
    p_TlvHeader->value[0] = numberOfCoresetRateMatchPatterns;
    tlvValueLength = sizeof(uint8_t);

    for (coresetRateMatchPatternInfoIndex = 0; coresetRateMatchPatternInfoIndex< numberOfCoresetRateMatchPatterns; coresetRateMatchPatternInfoIndex++)
    {
        p_CoresetRateMatchPatternInfo = (CoresetRateMatchPatternInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        *p_CoresetRateMatchPatternInfo = p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex];
        tlvValueLength += sizeof(CoresetRateMatchPatternInfo);
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_LteCrsRateMatchPattern( LTECrsRateMatchPatterns_Config *p_LteCrsRateMatchPatternsConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t *p_Value_U16, tlvValueLength, tlvPaddingLength, tlvLength, lteCrsRateMatchPatternsCount, lteCrsRateMatchPatternsIndex;
    uint16_t mbsfnSubframeConfigCount, mbsfnSubframeConfigIndex;
    uint8_t *p_Value_U8;
    TLV_Header *p_TlvHeader;
    LTECrsRateMatchPatternsInfo *p_LteCrsRateMatchPatternInfo;
    MbsfnSubframeConfigInfo *p_MbsfnSubframeConfigInfo;

    p_TlvHeader = *pp_TlvHeader;

    lteCrsRateMatchPatternsCount = p_LteCrsRateMatchPatternsConfig->numberOfLteCrsRateMatchPatterns;
    if (lteCrsRateMatchPatternsCount > MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT)
    {
        FAPI_Error( "AddTag: Too large lteCrsRateMatchPatternsCount %u : limit to %u\n",
            lteCrsRateMatchPatternsCount, MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT);
        lteCrsRateMatchPatternsCount = MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT;
    }

    p_TlvHeader->tag = TAG_LTE_CRS_RATE_MATCH_PATTERN;
    p_TlvHeader->value[0] = lteCrsRateMatchPatternsCount;
    tlvValueLength = sizeof(uint8_t);
    
    for (lteCrsRateMatchPatternsIndex = 0; lteCrsRateMatchPatternsIndex < lteCrsRateMatchPatternsCount; lteCrsRateMatchPatternsIndex++)
    {
        p_LteCrsRateMatchPatternInfo = &(p_LteCrsRateMatchPatternsConfig->lteCrsRateMatchPatternInfo[lteCrsRateMatchPatternsIndex]);

        p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength); 
        *p_Value_U8 = p_LteCrsRateMatchPatternInfo->crsRateMatchPatternID;
        tlvValueLength += sizeof(uint8_t);
         
        p_Value_U16 = (uint16_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength); 
        *p_Value_U16 = p_LteCrsRateMatchPatternInfo->carrierFreqDL;
        tlvValueLength += sizeof(uint16_t);
         
        p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength); 
        *p_Value_U8 = p_LteCrsRateMatchPatternInfo->carrierBandwidthDL;
        tlvValueLength += sizeof(uint8_t);
         
        p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength); 
        *p_Value_U8 = p_LteCrsRateMatchPatternInfo->nrofCrsPorts;
        tlvValueLength += sizeof(uint8_t);
         
        p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength); 
        *p_Value_U8 = p_LteCrsRateMatchPatternInfo->vShift;
        tlvValueLength += sizeof(uint8_t);
         
        mbsfnSubframeConfigCount = p_LteCrsRateMatchPatternInfo->sizeMbsfnSubframeConfigList;
        if (mbsfnSubframeConfigCount > MAX_MBSFN_SUBFRAME_CONFIG_COUNT)
        {
            FAPI_Error( "AddTag: Too large mbsfnSubframeConfigCount %u : limit to %u\n",
                mbsfnSubframeConfigCount, MAX_MBSFN_SUBFRAME_CONFIG_COUNT);
            mbsfnSubframeConfigCount = MAX_MBSFN_SUBFRAME_CONFIG_COUNT;
        }

        p_Value_U8 = (uint8_t *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength); 
        *p_Value_U8 = mbsfnSubframeConfigCount;
        tlvValueLength += sizeof(uint8_t);

        for (mbsfnSubframeConfigIndex = 0; mbsfnSubframeConfigIndex < mbsfnSubframeConfigCount; mbsfnSubframeConfigIndex++)
        {
            p_MbsfnSubframeConfigInfo = (MbsfnSubframeConfigInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
            *p_MbsfnSubframeConfigInfo = p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex];
            tlvValueLength += sizeof(MbsfnSubframeConfigInfo);
        }
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

uint16_t tlv_AddTag_PucchSemiStaticConfig( PUCCH_SemiStatic_Config *p_PucchSemiStaticConfig, TLV_Header **pp_TlvHeader, uint8_t *p_NumOfTLV )
{
    uint16_t tlvValueLength, tlvPaddingLength, tlvLength;
    uint16_t numUlBwpIds, ulBwpIndex;
    TLV_Header *p_TlvHeader;
    PUCCH_SemiStatic_ConfigInfo *p_PucchSemiStaticConfigInfo;

    numUlBwpIds = p_PucchSemiStaticConfig->numUlBwpIds;
    if (numUlBwpIds > MAX_UL_BWP_COUNT)
    {
        FAPI_Error( "AddTag: Too large numUlBwpIds %u : limit to %u\n",
            numUlBwpIds, MAX_UL_BWP_COUNT);
        numUlBwpIds = MAX_UL_BWP_COUNT;
    }

    p_TlvHeader = *pp_TlvHeader;

    p_TlvHeader->tag = TAG_PUCCH_SEMI_STATIC_PARAMETER;
    p_TlvHeader->value[0] = numUlBwpIds;
    tlvValueLength = sizeof(uint8_t);

    for (ulBwpIndex = 0; ulBwpIndex < numUlBwpIds; ulBwpIndex++)
    {
        p_PucchSemiStaticConfigInfo = (PUCCH_SemiStatic_ConfigInfo *) ((uint8_t *) p_TlvHeader + sizeof(TLV_Header) + tlvValueLength);
        *p_PucchSemiStaticConfigInfo = p_PucchSemiStaticConfig->pucchSemiStaticConfigInfo[ulBwpIndex];
        tlvValueLength += sizeof(PUCCH_SemiStatic_ConfigInfo);
    }

    tlvPaddingLength = (sizeof(uint32_t)-(tlvValueLength%sizeof(uint32_t)))%sizeof(uint32_t);
    tlvLength = sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;

    p_TlvHeader->length = tlvValueLength+tlvPaddingLength;

    (*p_NumOfTLV)++;
    *pp_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);

    return tlvLength;
}

///////////////////////////////////////////////////////////////////////////////////////////
// Message creation routiunes for MAC

void mac_CreateParamReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength, tlvValueLength, tlvPaddingLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    TLV_Header *p_TlvHeader;

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
    p_FapiGenericHeader->type = MSG_PHY_PARAM_REQ;
    p_FapiGenericHeader->length = 0;    // Filled at the end 
    fapiBodyLength = 0;

    p_TlvHeader = (TLV_Header *) (p_FapiGenericHeader+1);
    p_TlvHeader->tag = TAG_PROTOCOL_VERSION;
    tlvValueLength = sizeof(uint8_t);
    tlvPaddingLength = sizeof(uint32_t)-tlvValueLength;
    p_TlvHeader->length = tlvValueLength;
    p_TlvHeader->value[0] = FAPI_VERSION_V3;
    fapiBodyLength += sizeof(TLV_Header)+tlvValueLength+tlvPaddingLength;
   
    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header)+fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "Param Req created : msgLength %u\n", *p_MsgLength );
    
}

uint16_t convertEnumToValue_tddPeriod(uint8_t enumTddPeriod, uint8_t scs)
{
    uint16_t slotCountInMs;
    double tddPeriodByMs;

    slotCountInMs = (1 << scs);

    switch(enumTddPeriod)
    {
        case  0 : tddPeriodByMs =  0.5;   break;
        case  1 : tddPeriodByMs =  0.625; break;
        case  2 : tddPeriodByMs =  1.0;   break;
        case  3 : tddPeriodByMs =  1.25;  break;
        case  4 : tddPeriodByMs =  2.0;   break;
        case  5 : tddPeriodByMs =  2.5;   break;
        case  6 : tddPeriodByMs =  5.0;   break;
        case  7 : tddPeriodByMs = 10.0;   break;
        default : tddPeriodByMs =  0.0;  break;
    }

    return (uint16_t) (tddPeriodByMs*slotCountInMs);
}

void mac_CreateConfigReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, FapiConfigInfo *p_FapiConfigInfo, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    CONFIG_REQ *p_ConfigReq;
    uint8_t *p_NumOfTLV;
    TLV_Header *p_TlvHeader;
    PHY_Config *p_PhyConfig;
    Carrier_Config *p_CarrierConfig;
    Cell_Config *p_CellConfig;
    SsbPowerPBCH_Config *p_SsbPowerPbchConfig;
    PRACH_Config *p_PrachConfig;
    MultiPRACH_Config *p_MultiPrachConfig;
    MSGA_PUSCH_Config *p_MsgAPuschConfig;
    SSBResource_Config *p_SsbResourceConfig;
    MultiSSBResource_Config *p_MultiSsbResourceConfig;
    TDD_Config *p_TddConfig;
    Measurement_Config *p_MeasurementConfig;
    UCI_Config *p_UciConfig;
    PRBSymRateMatchPatterns_Config *p_PrbSymbRateMatchPatternsConfig;
    CoresetRateMatchPatterns_Config *p_CoresetRateMatchPatternsConfig;
    LTECrsRateMatchPatterns_Config *p_LteCrsRateMatchPatternsConfig;
    PUCCH_SemiStatic_Config *p_PucchSemiStaticConfig;
    PDSCH_SemiStatic_Config *p_PdschSemiStaticConfig;
    DelayManagement_Config *p_DelayManagementConfig;
    Rel16_mTRP_Config *p_Rel16mTRPConfig;

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
    p_FapiGenericHeader->type = MSG_PHY_CONFIG_REQ;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_ConfigReq = (CONFIG_REQ *) (p_FapiGenericHeader+1);
    p_ConfigReq->numOfTLV = 0;
    fapiBodyLength += sizeof(CONFIG_REQ);

    p_NumOfTLV = &(p_ConfigReq->numOfTLV);
    p_TlvHeader = &(p_ConfigReq->tlvs[0]);

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PHY configuration in Table 3-36

    p_PhyConfig = &p_FapiConfigInfo->phyConfig;

    if ((p_PhyConfig->tagBitmask & CONFIG_TAG_BITMASK_PHY_PROFILE_ID))
        fapiBodyLength += tlv_AddTag_U16( TAG_PHY_PROFILE_ID, p_PhyConfig->phyProfileId, &p_TlvHeader, p_NumOfTLV );

    if ((p_PhyConfig->tagBitmask & CONFIG_TAG_BITMASK_INDICATION_INSTANCES_PER_SLOT))
        fapiBodyLength += tlv_AddTag_IndicationInstancePerSlotInfo( &p_PhyConfig->indicationInstancesPerSlot, &p_TlvHeader, p_NumOfTLV );

    if ((p_PhyConfig->tagBitmask & CONFIG_TAG_BITMASK_REQUEST_INSTANCES_PER_SLOT))
        fapiBodyLength += tlv_AddTag_RequestInstancePerSlotInfo( &p_PhyConfig->requestInstancesPerSlot, &p_TlvHeader, p_NumOfTLV );

    if ((p_PhyConfig->tagBitmask & CONFIG_TAG_BITMASK_SLOT_TIME_DOMAIN_INFO))
        fapiBodyLength += tlv_AddTag_SlotTimeDomainInfo( &p_PhyConfig->slotTimeDomainInfo, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Carrier configuration in Table 3-37 

    p_CarrierConfig = &p_FapiConfigInfo->carrierConfig;

    if ((p_CarrierConfig->tagBitmask & CONFIG_TAG_BITMASK_CARRIER_CONFIGURATION))
        fapiBodyLength += tlv_AddTag_CarrierConfiguration( &p_CarrierConfig->carrierConfigInfo, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Cell configuration in Table 3-38

    p_CellConfig = &p_FapiConfigInfo->cellConfig;

    if ((p_CellConfig->tagBitmask & CONFIG_TAG_BITMASK_PHY_CELL_ID))
        fapiBodyLength += tlv_AddTag_U16( TAG_PHY_CELL_ID, p_CellConfig->phyCellId, &p_TlvHeader, p_NumOfTLV );

    if ((p_CellConfig->tagBitmask & CONFIG_TAG_BITMASK_FRAME_DUPLEX_TYPE))
        fapiBodyLength += tlv_AddTag_U8( TAG_FRAME_DUPLEX_TYPE, p_CellConfig->frameDuplexType, &p_TlvHeader, p_NumOfTLV );

    if ((p_CellConfig->tagBitmask & CONFIG_TAG_BITMASK_PDSCH_TRANS_TYPE_VALIDITY))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_TRANS_TYPE_VALIDITY, p_CellConfig->pdschTransTypeValidity, &p_TlvHeader, p_NumOfTLV );

    if ((p_CellConfig->tagBitmask & CONFIG_TAG_BITMASK_PUSCH_TRANS_TYPE_VALIDITY))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_TRANS_TYPE_VALIDITY, p_CellConfig->puschTransTypeValidity, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set SSB power and PBCH configuration in Table 3-39

    p_SsbPowerPbchConfig = &p_FapiConfigInfo->ssbPowerPbchConfig;

    if ((p_SsbPowerPbchConfig->tagBitmask & CONFIG_TAG_BITMASK_SS_PBCH_POWER))
        fapiBodyLength += tlv_AddTag_U32( TAG_SS_PBCH_POWER, p_SsbPowerPbchConfig->ssPbchPower, &p_TlvHeader, p_NumOfTLV );

    if ((p_SsbPowerPbchConfig->tagBitmask & CONFIG_TAG_BITMASK_SS_PBCH_BLOCK_POWER_SCALING))
        fapiBodyLength += tlv_AddTag_U16( TAG_SS_PBCH_BLOCK_POWER_SCALING, p_SsbPowerPbchConfig->ssPbchBlockPowerScaling, &p_TlvHeader, p_NumOfTLV );

    if ((p_SsbPowerPbchConfig->tagBitmask & CONFIG_TAG_BITMASK_BCH_PAYLOAD_FLAG))
        fapiBodyLength += tlv_AddTag_U8( TAG_BCH_PAYLOAD_FLAG, p_SsbPowerPbchConfig->bchPayloadFlag, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PRACH configuration in Table 3-40

    p_PrachConfig = &p_FapiConfigInfo->prachConfig;

    if ((p_PrachConfig->tagBitmask & CONFIG_TAG_BITMASK_PRACH_RESOURCE_CONFIGURATION))
        fapiBodyLength += tlv_AddTag_PrachResourceConfiguration( p_PrachConfig, &p_TlvHeader, p_NumOfTLV );
        
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Multi-PRACH configuration table in Table 3-41 

    p_MultiPrachConfig = &p_FapiConfigInfo->multiPrachConfig;

    if ((p_MultiPrachConfig->tagBitmask & CONFIG_TAG_BITMASK_MULTIPLE_PRACH_CONFIGURATION))
        fapiBodyLength += tlv_AddTag_MultiplePrachConfiguration( p_MultiPrachConfig, &p_TlvHeader, p_NumOfTLV );
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set MSGA_PUSCH configuration table in Table 3-42 
     
    p_MsgAPuschConfig= &p_FapiConfigInfo->msgApuschConfig;

    if ((p_MsgAPuschConfig->tagBitmask & CONFIG_TAG_BITMASK_MSGA_PUSCH_CONFIGURATION))
        fapiBodyLength += tlv_AddTag_MsgAPuschConfiguration( p_MsgAPuschConfig, &p_TlvHeader, p_NumOfTLV );
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set SSB resource configuration table in Table 3-45 

    p_SsbResourceConfig = &p_FapiConfigInfo->ssbResourceConfig;

    if ((p_SsbResourceConfig->tagBitmask & CONFIG_TAG_BITMASK_SSB_TRANSMISSION_PATTERN))
        fapiBodyLength += tlv_AddTag_SsbResourceConfig( &p_SsbResourceConfig->ssbResourceConfigInfo, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Multi-SSB resource configuration table in Table 3-46

    p_MultiSsbResourceConfig = &p_FapiConfigInfo->multiSsbResourceConfig;

    if ((p_MultiSsbResourceConfig->tagBitmask & CONFIG_TAG_BITMASK_MULTIPLE_SSB_CONFIGURATION))
        fapiBodyLength += tlv_AddTag_MultipleSsbConfig( p_MultiSsbResourceConfig, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set TDD table in Table 3-47 

    p_TddConfig = &p_FapiConfigInfo->tddConfig;

    if ((p_TddConfig->tagBitmask & CONFIG_TAG_BITMASK_TDD_PATTERN))
        fapiBodyLength += tlv_AddTag_TddPattern( p_TddConfig, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Measurement configuration in Table 3-48

    p_MeasurementConfig = &p_FapiConfigInfo->measurementConfig;

    if ((p_MeasurementConfig->tagBitmask & CONFIG_TAG_BITMASK_RSSI_MEASUREMENT_CONFIGURATION))
        fapiBodyLength += tlv_AddTag_U8( TAG_RSSI_MEASUREMENT_CONFIGURATION, p_MeasurementConfig->rssiMeasurement, &p_TlvHeader, p_NumOfTLV );

    if ((p_MeasurementConfig->tagBitmask & CONFIG_TAG_BITMASK_RSRP_MEASUREMENT_CONFIGURATION))
        fapiBodyLength += tlv_AddTag_U8( TAG_RSRP_MEASUREMENT_CONFIGURATION, p_MeasurementConfig->rsrpMeasurement, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Digital beam table (DBT) PDU in Table 3-61 : No tag is defined in document : Skip for now

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Precoding matrix (PM) PDU in Table 3-62 : No tag is defined in document : Skip for now

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set UCI configuration in Table 3-49

    p_UciConfig = &p_FapiConfigInfo->uciConfig;
    
    if ((p_UciConfig->tagBitmask & CONFIG_TAG_BITMASK_UCI_INDICATION_CONFIGURATION))
        fapiBodyLength += tlv_AddTag_UciConfig( p_UciConfig, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PRB-symbol rate match patterns bitmap (non-CORESET) configuration in Table 3-50

    p_PrbSymbRateMatchPatternsConfig = &p_FapiConfigInfo->prbSymbRateMatchPatternsConfig;

    if ((p_PrbSymbRateMatchPatternsConfig->tagBitmask & CONFIG_TAG_BITMASK_PRB_SYMBOL_RATE_MATCH_PATTERN))
        fapiBodyLength += tlv_AddTag_PrbSymbolRateMatchPattern( p_PrbSymbRateMatchPatternsConfig, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set CORESET rate match patterns bitmap configuration in Table 3-51

    p_CoresetRateMatchPatternsConfig = &p_FapiConfigInfo->coresetRateMatchPatternsConfig;

    if ((p_CoresetRateMatchPatternsConfig->tagBitmask & CONFIG_TAG_BITMASK_CORESET_RATE_MATCH_PATTERN))
        fapiBodyLength += tlv_AddTag_CoresetRateMatchPattern( p_CoresetRateMatchPatternsConfig, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set LTE-CRS rate match patterns configuration in Table 3-52

    p_LteCrsRateMatchPatternsConfig = &p_FapiConfigInfo->lteCrsRateMatchPatternsConfig;

    if ((p_LteCrsRateMatchPatternsConfig->tagBitmask & CONFIG_TAG_BITMASK_LTE_CRS_RATE_MATCH_PATTERN))
        fapiBodyLength += tlv_AddTag_LteCrsRateMatchPattern( p_LteCrsRateMatchPatternsConfig, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PUCCH semi-static configuration in Table 3-53

    p_PucchSemiStaticConfig = &p_FapiConfigInfo->pucchSemiStaticConfig;;

    if ((p_PucchSemiStaticConfig->tagBitmask & CONFIG_TAG_BITMASK_PUCCH_SEMI_STATIC_PARAMETER))
        fapiBodyLength += tlv_AddTag_PucchSemiStaticConfig( p_PucchSemiStaticConfig, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PDSCH semi-static configuration in Table 3-54 :

    p_PdschSemiStaticConfig = &p_FapiConfigInfo->pdschSemiStaticConfig;

    if ((p_PdschSemiStaticConfig->tagBitmask & CONFIG_TAG_BITMASK_PDSCH_SEMI_STATIC_PARAMETER))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_SEMI_STATIC_PARAMETER, p_PdschSemiStaticConfig->pdschCbgScheme, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Delay management configuration in Table 3-55 

    p_DelayManagementConfig = &p_FapiConfigInfo->delayManagementConfig;

    if ((p_DelayManagementConfig->tagBitmask & CONFIG_TAG_BITMASK_TIMING_WINDOW))
        fapiBodyLength += tlv_AddTag_U16( TAG_TIMING_WINDOW, p_DelayManagementConfig->timingWindow, &p_TlvHeader, p_NumOfTLV );

    if ((p_DelayManagementConfig->tagBitmask & CONFIG_TAG_BITMASK_TIMING_INFO_MODE))
        fapiBodyLength += tlv_AddTag_U8( TAG_TIMING_INFO_MODE, p_DelayManagementConfig->timingInfoMode, &p_TlvHeader, p_NumOfTLV );

    if ((p_DelayManagementConfig->tagBitmask & CONFIG_TAG_BITMASK_TIMING_INFO_PERIOD))
        fapiBodyLength += tlv_AddTag_U8( TAG_TIMING_INFO_PERIOD, p_DelayManagementConfig->timingInfoPeriod, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Rel-16 mTRP configuration in Table 3-56 

    p_Rel16mTRPConfig = &p_FapiConfigInfo->rel16mTRPConfig;

    if ((p_Rel16mTRPConfig->tagBitmask & CONFIG_TAG_BITMASK_NUM_TX_PORTS_TRP1))
        fapiBodyLength += tlv_AddTag_U8( TAG_NUM_TX_PORTS_TRP1, p_Rel16mTRPConfig->numTxPortsTRP1, &p_TlvHeader, p_NumOfTLV );

    if ((p_Rel16mTRPConfig->tagBitmask & CONFIG_TAG_BITMASK_NUM_RX_PORTS_TRP1))
        fapiBodyLength += tlv_AddTag_U8( TAG_NUM_RX_PORTS_TRP1, p_Rel16mTRPConfig->numRxPortsTRP1, &p_TlvHeader, p_NumOfTLV );

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "Config req created : msgLength %u\n", *p_MsgLength );
}

void mac_CreateTestConfigReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, TestConfigReq *pTestConfigReqSrc, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    TestConfigReq *pTestConfigReqDest;

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
    p_FapiGenericHeader->type = MSG_TEST_CONFIG_REQ ;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    pTestConfigReqDest = (TestConfigReq *) (p_FapiGenericHeader+1);
    memcpy( pTestConfigReqDest, pTestConfigReqSrc, sizeof(TestConfigReq) );
    fapiBodyLength += sizeof(TestConfigReq);

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "Start req created : msgLength %u\n", *p_MsgLength );
}

void mac_CreateStartReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;

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
    p_FapiGenericHeader->type = MSG_PHY_START_REQ;
    p_FapiGenericHeader->length = 0;   

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header);

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "Start req created : msgLength %u\n", *p_MsgLength );
}

void mac_CreateStopReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;

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
    p_FapiGenericHeader->type = MSG_PHY_STOP_REQ;
    p_FapiGenericHeader->length = 0;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header);

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "Stop req created : msgLength %u\n", *p_MsgLength );
}

void mac_CreateDLTTIReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength, pduLength;
    uint8_t  numOfcodewords, szRm,szCrs,szCsirs, dciPayloadcnt,rbBitmapsize,
    freqRbsize, maxPdschRmPatterncnt;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    DL_TTI_REQ *p_DlTtiReq;
    PDU_Struct *p_pduHeader;
    SSBPBCH_PDU *p_SsbPdu;
    MAC_GenMIB_PDU *p_MacGenMIBPdu;
    TxPrecodingAndBeamforming_PDU *p_TxPrecodingBfPdu;
    SSBPBCH_PDU_Maintenance_FAPIv3 *p_SsbPduV3;
    SSBPBCH_PDU_FAPIv4 *p_SsbPduV4;
    PDCCH_PDU *p_PdcchPdu;
    DL_DCI_PDU_part1 *p_DlDci1Pdu;
    DL_DCI_PDU_part2 *p_DlDci2Pdu;
    PDCCH_PDU_Maintenance_FAPIv3 *p_PdcchPduV3;
    PDCCH_PDU_Maintenance_FAPIv3_DCIInfo *p_PdcchPduV3_Dci; 
    PDCCH_PDU_FAPIv4_DCIInfo *p_PdcchPduV4_Dci;
    PDCCH_PDU_FAPIv4_MuMimo *p_PdcchPduV4_MuMimo;
    PDCCH_PDU_FAPIv4_MuMimo_DCIInfo *p_PdcchPduV4_MuMimo_DCIInfo;
    PDSCH_PDU *p_PdschPdu;
    PDSCH_PTRS *p_PdschPtrs;
    PDSCH_TxPowerInfo *p_Pdsch_Txpower;
    PDSCH_Maintenance_FAPIv3 *p_PdschPduV3;
    PDSCH_PTRS_Maintenance_FAPIv3 *p_PdschPtrsV3;
    Rel16_PDSCH_FAPIv3 *p_PdschRelV3;
    PDSCH_PDU_FAPIv4 *p_PdschPduV4;
    CSI_RS_PDU *p_CsirsPdu;
    CSI_RS_PDU_MaintenanceParam_FAPIv3 *p_CsirsPduV3;
    
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

    p_DlTtiReq->sfn = 123;
    p_DlTtiReq->slot = 5;
    p_DlTtiReq->numPDUs = 0;
    p_DlTtiReq->nDlTypes = NUM_OF_DLPDU_TYPE;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDCCH] = 0;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDSCH] = 0;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_CSI_RS] = 0;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_SSB] = 0;
    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_DLDCI] = 0;
    p_DlTtiReq->nGroup = 0;
    
    p_pduHeader = (PDU_Struct *) p_DlTtiReq->variablePart;
    
    // Add SSB Pdu

    p_pduHeader->pduType = DL_PDU_TYPE_SSB;
    p_SsbPdu = (SSBPBCH_PDU *) p_pduHeader;

    p_SsbPdu->physCellId = 50;
    p_SsbPdu->betaPssProfileNR = BETA_PSS_PRFILE_NR_0_DB;
    p_SsbPdu->ssbBlockIndex = 0;
    p_SsbPdu->ssbSubcarrierOffset = 0;
    p_SsbPdu->ssbOffsetPointA = 12;
    p_SsbPdu->bchPayloadFlag = BCH_PAYLOAD_MAC_GENERATES_FULL;
    pduLength = sizeof(SSBPBCH_PDU);

    p_MacGenMIBPdu = (MAC_GenMIB_PDU *) p_SsbPdu->variablePart;
    p_MacGenMIBPdu->bchPayload = 0x12345678;
    pduLength += sizeof(MAC_GenMIB_PDU);

    p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *) (p_MacGenMIBPdu+1);
    p_TxPrecodingBfPdu->trpScheme = 0;   
    p_TxPrecodingBfPdu->numPRGs = 0;   
    p_TxPrecodingBfPdu->prgSize = 0;   
    p_TxPrecodingBfPdu->digBFInterfaces = 0;   
    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);
 
    p_SsbPduV3 = (SSBPBCH_PDU_Maintenance_FAPIv3 *) p_TxPrecodingBfPdu->variablePart;
    p_SsbPduV3->ssbPduIndex = 0;
    p_SsbPduV3->ssbFirstSymbolMappingCase = SSB_FIRST_SYMBOL_MAPPING_CASE_C;
    p_SsbPduV3->subcarrierSpacing = SUBCARRIER_SPACING_30KHZ;
    p_SsbPduV3->lMax = SSB_FIRST_SYMBOL_MAPPING_L_MAX_8;
    p_SsbPduV3->ssPbchBlockPowerScaling = POWER_0_01_SCALE_0_DB;
    p_SsbPduV3->betaPSSProfileSSS = POWER_0_001_SCALE_0_DB;
    pduLength += sizeof(SSBPBCH_PDU_Maintenance_FAPIv3);

    p_SsbPduV4 =(SSBPBCH_PDU_FAPIv4 *) (p_SsbPduV3+1);
    p_SsbPduV4->spatialStreamIndexPresent=0;
    p_SsbPduV4->spatialStreamIndex =0;
    pduLength += sizeof(SSBPBCH_PDU_FAPIv4);
    

    p_pduHeader->pduSize = pduLength;
    fapiBodyLength += pduLength;

    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_SSB]++;
    p_DlTtiReq->numPDUs++;
    p_pduHeader = (PDU_Struct *) ((uint8_t *)p_pduHeader+pduLength);

    // Add PDCCH Pdu
    p_PdcchPdu = (PDCCH_PDU *) p_pduHeader;
    p_PdcchPdu->sPDU_Hdr.pduType = DL_PDU_TYPE_PDCCH;
    pduLength = sizeof(PDU_Struct);    
    p_PdcchPdu->coresetBWPStart = 0;
    p_PdcchPdu->subcarrierSpacing = 1;
    p_PdcchPdu->cyclicPrefix = 0;
    p_PdcchPdu->startSymbolIndex = 0;
    p_PdcchPdu->durationSymbols = 0;
    p_PdcchPdu->freqDomainResource[0] = 1;
    p_PdcchPdu->freqDomainResource[1] = 1;
    p_PdcchPdu->freqDomainResource[2] = 1;
    p_PdcchPdu->freqDomainResource[3] = 1;
    p_PdcchPdu->freqDomainResource[4] = 1;
    p_PdcchPdu->freqDomainResource[5] = 1;
    p_PdcchPdu->cceRegMappingType = 0;
    p_PdcchPdu->regBundleSize = 0;
    p_PdcchPdu->interleaverSize = 0;
    p_PdcchPdu->coreSetType = 0;
    p_PdcchPdu->shiftIndex = 0;
    p_PdcchPdu->precoderGranularity = 0;
    p_PdcchPdu->numDlDci = 1;
    pduLength += sizeof(PDCCH_PDU);

    p_DlDci1Pdu = (DL_DCI_PDU_part1*)p_PdcchPdu->variablePart;
    p_DlDci1Pdu->rnti = 0;
    p_DlDci1Pdu->nIdPdcchData = 0;
    p_DlDci1Pdu->nRntiPdcchData = 0;
    p_DlDci1Pdu->cceIndex = 0;
    p_DlDci1Pdu->aggregationLevel = 16;
    pduLength +=sizeof(DL_DCI_PDU_part1);

    p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU*)p_DlDci1Pdu->variablePart;                
    p_TxPrecodingBfPdu->trpScheme = 0;                
    p_TxPrecodingBfPdu->numPRGs = 0;                
    p_TxPrecodingBfPdu->prgSize = 0;                
    p_TxPrecodingBfPdu->digBFInterfaces = 0;
    pduLength +=sizeof(TxPrecodingAndBeamforming_PDU);
    
    p_DlDci2Pdu = (DL_DCI_PDU_part2*)p_TxPrecodingBfPdu->variablePart;                
    p_DlDci2Pdu->beta_PDCCH_1_0 = 0;                
    p_DlDci2Pdu->powerControlOffsetSSProfileNR = 0;                
    p_DlDci2Pdu->payloadSizeBits = 1;                
    for (dciPayloadcnt = 0; dciPayloadcnt <DCI_PAYLOAD_BTYE_LEN ;dciPayloadcnt++)
    {
    p_DlDci2Pdu->payload[dciPayloadcnt] = 1;
    }
    
    pduLength +=sizeof(DL_DCI_PDU_part2);

    p_PdcchPduV3 = (PDCCH_PDU_Maintenance_FAPIv3 *)(p_DlDci2Pdu+1);                
    p_PdcchPduV3->pdcchPduIndex = 0;                
    pduLength +=sizeof(PDCCH_PDU_Maintenance_FAPIv3);

    p_PdcchPduV3_Dci = (PDCCH_PDU_Maintenance_FAPIv3_DCIInfo *)p_PdcchPduV3->variablePart;                
    p_PdcchPduV3_Dci->dciIndex = 0;                
    p_PdcchPduV3_Dci->collocatedAl16Candidate = 0;                
    p_PdcchPduV3_Dci->pdcchDmrsPowerOffsetProfileSSS = 0;                
    p_PdcchPduV3_Dci->pdcchDataPowerOffsetProfileSSS = 0;              
    pduLength +=sizeof(PDCCH_PDU_Maintenance_FAPIv3_DCIInfo);

    p_PdcchPduV4_Dci = (PDCCH_PDU_FAPIv4_DCIInfo *) (p_PdcchPduV3_Dci+1);
    p_PdcchPduV4_Dci->nIdPdcchDmrs = 0;
    p_PdcchPduV4_MuMimo = (PDCCH_PDU_FAPIv4_MuMimo *) (p_PdcchPduV4_Dci+1);
    p_PdcchPduV4_MuMimo->spatialStreamIndicesPresent = 0;
    p_PdcchPduV4_MuMimo_DCIInfo = (PDCCH_PDU_FAPIv4_MuMimo_DCIInfo *) p_PdcchPduV4_MuMimo->variablePart;
    p_PdcchPduV4_MuMimo_DCIInfo->dciIndex = 0;
    p_PdcchPduV4_MuMimo_DCIInfo->spatialStreamIndex = 0;

    p_pduHeader->pduSize = pduLength;
    fapiBodyLength += pduLength;

    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDCCH]++;
    p_DlTtiReq->numPDUs++;
    p_pduHeader = (PDU_Struct *) ((uint8_t *)p_pduHeader+pduLength);

    //Add PDSCH Pdu
    p_PdschPdu = (PDSCH_PDU*) p_pduHeader;
    p_PdschPdu->sPDU_Hdr.pduType = DL_PDU_TYPE_PDSCH;
    pduLength = sizeof(PDU_Struct);
    p_PdschPdu->pduBitmap = 0;
    p_PdschPdu->rnti = 0;
    p_PdschPdu->pduIndex = 0;
    p_PdschPdu->bwpSize = 273;
    p_PdschPdu->bwpStart =0;
    p_PdschPdu->subcarrierSpacing = 1;
    p_PdschPdu->cyclicPrefix = 0;
    p_PdschPdu->nrOfCodewords = 0;
    for(numOfcodewords = 0 ; numOfcodewords <MAX_NUM_OF_CORDWORD; numOfcodewords++)
    {
    p_PdschPdu->codewordInfo[numOfcodewords].targetCodeRate = 0;
    p_PdschPdu->codewordInfo[numOfcodewords].qamModOrder =2;
    p_PdschPdu->codewordInfo[numOfcodewords].mcsIndex = 2;
    p_PdschPdu->codewordInfo[numOfcodewords].mcsTable = 1;
    p_PdschPdu->codewordInfo[numOfcodewords].rvIndex =0;
    p_PdschPdu->codewordInfo[numOfcodewords].tbSize =0;
    }
    
                    
    p_PdschPdu->nIdPdsch = 0;
    p_PdschPdu->nrOfLayers = 0;
    p_PdschPdu->transmissionScheme =0;
    p_PdschPdu->refPoint = 0;
    p_PdschPdu->dlDmrsSymbPos = 0;
    p_PdschPdu->dmrsConfigType =0;
    p_PdschPdu->pdschDmrsScramblingId = 0;
    p_PdschPdu->pdschDmrsScramblingIdComplement = 0;
    p_PdschPdu->lowPaprDmrs = 0;
    p_PdschPdu->nSCID = 0;
    p_PdschPdu->numDmrsCdmGrpsNoData = 0;
    p_PdschPdu->dmrsPorts= 0;
    p_PdschPdu->resourceAlloc= 0;

    
    for(rbBitmapsize=0; rbBitmapsize<MAX_RB_BITMAP_SIZE;rbBitmapsize++)
    {
    p_PdschPdu->rbBitmap[rbBitmapsize]= 1;
    }
    p_PdschPdu->rbStart= 0;
    p_PdschPdu->rbSize= 273;
    p_PdschPdu->vrbToPrbMapping= 0;
    p_PdschPdu->startSymbolIndex= 0;
    p_PdschPdu->nrOfSymbols= 14;
    pduLength += sizeof(PDSCH_PDU);
                
    p_PdschPtrs = (PDSCH_PTRS *)p_PdschPdu->variablePart;
    p_PdschPtrs->ptrsPortIndex = 0;
    p_PdschPtrs->ptrsTimeDensity= 0;
    p_PdschPtrs->ptrsFreqDensity= 0;
    p_PdschPtrs->ptrsReOffset = 0;
    p_PdschPtrs->nEpreRatioOfPDSCHToPTRSProfileNR = 0;
    pduLength += sizeof(PDSCH_PTRS);
        
    p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *)(p_PdschPtrs+1);
    p_TxPrecodingBfPdu->trpScheme = 0;
    p_TxPrecodingBfPdu->numPRGs = 0;
    p_TxPrecodingBfPdu->prgSize = 0;
    p_TxPrecodingBfPdu->digBFInterfaces = 0;
    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);    
    
                    
    p_Pdsch_Txpower= (PDSCH_TxPowerInfo *)(p_TxPrecodingBfPdu +1);
    p_Pdsch_Txpower->powerControlOffsetProfileNR = 0;
    p_Pdsch_Txpower->powerControlOffsetSSProfileNR = 0;
    pduLength += sizeof(PDSCH_TxPowerInfo);
    
        
    p_PdschPduV3 = (PDSCH_Maintenance_FAPIv3 *)(p_Pdsch_Txpower+1);
    p_PdschPduV3->pdschTransType = 0;
    p_PdschPduV3->coresetStartPoint = 0;
    p_PdschPduV3->initialDlBwpSize = 48;
    p_PdschPduV3->ldpcBaseGraph = LDPC_BASE_GRAPH_1;
    p_PdschPduV3->tbSizeLbrmBytes = 0;
    p_PdschPduV3->tbCrcRequired = 0;

    uint8_t  pdschSsbRmInfo;
    for(pdschSsbRmInfo = 0; pdschSsbRmInfo < SSB_PDUS_FOR_RATE_MATCHING_SIZE;pdschSsbRmInfo++)
    {
        p_PdschPduV3->ssbPdusForRateMatching[pdschSsbRmInfo] = 0;
    }
    p_PdschPduV3->ssbConfigForRateMatching =0;
    p_PdschPduV3->prbSymRmPatternBitmapSizeByReference = 0;
    //szRm = ceil(prbSymRmPatternBitmapSizeByReference/8);
    for(szRm =0; szRm < PDSCH_RATE_MATCHING_BITMAP_SIZE; szRm++)
    {
        p_PdschPduV3->prbSymRateMatchPatternBitmapByReference[szRm]=0;
    }
    p_PdschPduV3->numPrbSymRmPatternsByValue =1;
        
    for(maxPdschRmPatterncnt =0; maxPdschRmPatterncnt< MAX_PDSCH_PRB_SYM_RM_PATTERNS_INFO_COUNT; maxPdschRmPatterncnt++)
    {
        for(freqRbsize =0 ; freqRbsize < MAX_RATEMATCH_BITMAP_SIZE; freqRbsize++)
        {
            p_PdschPduV3->prbSymRmPatternsInfo[maxPdschRmPatterncnt].freqDomainRB[freqRbsize]= 1;
        }
        p_PdschPduV3->prbSymRmPatternsInfo[maxPdschRmPatterncnt].symbolsInRB = 1;
    }
    
    p_PdschPduV3->numCoresetRmPatterns = 1;
    uint8_t maxPdschCoresetRmPatterncnt;
    
    for(maxPdschCoresetRmPatterncnt =0; maxPdschCoresetRmPatterncnt< MAX_PDSCH_CORESET_RM_PATTERNS_INFO_COUNT; maxPdschCoresetRmPatterncnt++)
    {
        uint8_t freqDomainResource;
        for(freqDomainResource =0; freqDomainResource <MAX_PDSCH_CORESET_FREQDOMAIN_RESOURCE ;freqDomainResource++)
        {
        p_PdschPduV3->coresetRmPatternsInfo[maxPdschCoresetRmPatterncnt].freqDomainResources[freqDomainResource]= 1;
        }
        p_PdschPduV3->coresetRmPatternsInfo[maxPdschCoresetRmPatterncnt].symbolsPattern = 1;
    }
    p_PdschPduV3->pdcchPduIndex = 1;
    p_PdschPduV3->dciIndex = 1;
    p_PdschPduV3->lteCrsRateMatchPatternBitmapSize = 1;
    for(szCrs = 0; szCrs <MAX_LTE_CRS_RATEMATCH_PATTERN_BITMAP_SIZE; szCrs++)
    {
        p_PdschPduV3->lteCrsRateMatchPattern[szCrs] = 0;
    }

    p_PdschPduV3->numCsiRsForRateMatching = 1;

    for(szCsirs = 0; szCsirs <MAX_CSI_RS_RATEMATCH_SIZE; szCsirs++)
    {
        p_PdschPduV3->csiRsForRateMatching[szCsirs] = 0;
    }
    p_PdschPduV3->pdschDmrsPowerOffsetProfileSSS = 0;
    p_PdschPduV3->pdschDataPowerOffsetProfileSSS = 0;

    pduLength += sizeof(PDSCH_Maintenance_FAPIv3);

    //TO DO : Add CBG part

    p_PdschPtrsV3 = (PDSCH_PTRS_Maintenance_FAPIv3 *) (p_PdschPduV3+1);
    p_PdschPtrsV3->pdschPtrsPowerOffsetProfileSSS = 0;
    pduLength += sizeof(PDSCH_PTRS_Maintenance_FAPIv3);


    p_PdschRelV3 = (Rel16_PDSCH_FAPIv3 *)(p_PdschPtrsV3 +1);
    p_PdschRelV3->repetitionScheme = 2;
    pduLength += sizeof(Rel16_PDSCH_FAPIv3);


    p_PdschPduV4 = (PDSCH_PDU_FAPIv4 *)p_PdschRelV3->variablePart;    
    p_PdschPduV4->coresetRmPatternBitmapSizeByReference =0;

    //szRm = ceil(coresetRmPatternBitmapByReference/8);
    for(szRm =0; szRm < PDSCH_RATE_MATCHING_BITMAP_SIZE; szRm++)
    {
        p_PdschPduV4->coresetRmPatternBitmapByReference[szRm]=0;
    }
    p_PdschPduV4->lteCrsMbsfnDerivationMethod = 0;
    
    p_pduHeader->pduSize = pduLength;
    fapiBodyLength += pduLength;

    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDSCH]++;
    p_DlTtiReq->numPDUs++;
    p_pduHeader = (PDU_Struct *) ((uint8_t *)p_pduHeader+pduLength);

    //Add CSIRS Pdu
    p_CsirsPdu= (CSI_RS_PDU *) p_pduHeader;
    p_CsirsPdu->sPDU_Hdr.pduType = DL_PDU_TYPE_CSI_RS;
    pduLength = sizeof(PDU_Struct);  
    p_CsirsPdu->subcarrierSpacing = 1;
    p_CsirsPdu->cyclicPrefix = 0;
    p_CsirsPdu->startRB = 0;
    p_CsirsPdu->nrOfRBs = 273;
    p_CsirsPdu->csiType = 0;
    p_CsirsPdu->row =4;
    p_CsirsPdu->freqDomain =1;
    p_CsirsPdu->symbL0 = 0;
    p_CsirsPdu->symbL1 = 0;
    p_CsirsPdu->cdmType =0;
    p_CsirsPdu->freqDensity= 0;
    p_CsirsPdu->scrambId = 1023;
    p_CsirsPdu->powerControlOffsetProfileNR =0;
    p_CsirsPdu->powerControlOffsetSSProfileNR =0;
    pduLength += sizeof(CSI_RS_PDU);

    p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *)p_CsirsPdu->variablePart;
    p_TxPrecodingBfPdu->trpScheme = 0;
    p_TxPrecodingBfPdu->numPRGs = 0;
    p_TxPrecodingBfPdu->prgSize = 0;
    p_TxPrecodingBfPdu->digBFInterfaces = 0;
    pduLength += sizeof(TxPrecodingAndBeamforming_PDU);    


    p_CsirsPduV3 = (CSI_RS_PDU_MaintenanceParam_FAPIv3 *)(p_TxPrecodingBfPdu +1);
    p_CsirsPduV3->csiRsPduIndex = 0;
    p_CsirsPduV3->csiRsPowerOffsetProfileSSS =0;

    
    p_pduHeader->pduSize = pduLength;
    fapiBodyLength += pduLength;

    p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_CSI_RS]++;
    p_DlTtiReq->numPDUs++;
    p_pduHeader = (PDU_Struct *) ((uint8_t *)p_pduHeader+pduLength);
    

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "DLTTIReq created : msgLength %u\n", *p_MsgLength );
}

void mac_CreateTxDataReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    Tx_DATA_REQ *p_TxDataReq;

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

    p_TxDataReq->sfn = 123;
    p_TxDataReq->slot = 5;
    p_TxDataReq->numPDUs = 0;
    
    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "TxDataReq created : msgLength %u\n", *p_MsgLength );
}

void mac_CreateULDCIReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    UL_DCI_REQ *p_ULDciReq;

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

    p_ULDciReq->sfn = 123;
    p_ULDciReq->slot = 5;
    p_ULDciReq->numPDUs = 0;
    p_ULDciReq->nDlTypes = NUM_OF_ULDCIPDU_TYPE;
    p_ULDciReq->nPDUsOfEachType[ULDCI_PDU_TYPE_INDEX_PDCCH] = 0;
    p_ULDciReq->nPDUsOfEachType[ULDCI_PDU_TYPE_INDEX_ULDCI] = 0;

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "ULDCIReq created : msgLength %u\n", *p_MsgLength );
}

void mac_CreateULTTIReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    UL_TTI_REQ *p_UlTtiReq;

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

    p_UlTtiReq->sfn = 123;
    p_UlTtiReq->slot = 9;
    p_UlTtiReq->numPDUs = 0;
    p_UlTtiReq->nUlTypes = NUM_OF_ULPDU_TYPE;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_PRACH] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_PUSCH] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_FORMAT01_PUCCH] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_FORMAT234_PUCCH] = 0;
    p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_SRS] = 0;
    p_UlTtiReq->nGroup = 0;


    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "ULTTIReq created : msgLength %u\n", *p_MsgLength );
}

///////////////////////////////////////////////////////////////////////////////////////////
// Message creation routiunes for PHY

void phy_CreateParamResp( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, FapiParamInfo *p_FapiParamInfo, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    PARAM_RESP *p_ParamResp;
    uint8_t *p_NumOfTLV;
    TLV_Header *p_TlvHeader;
    Cell_Param *p_CellParam;
    Carrier_Param *p_Carrier_Param;
    PDCCH_Param *p_PdcchParam;
    PUCCH_Param *p_PucchParam;
    PDSCH_Param *p_PdschParam;
    PUSCH_Param *p_PuschParam;
    PRACH_Param *p_PrachParam;
    Measurement_Param *p_MeasurementParam;
    UCI_Param *p_UciParam;
    Capability_Validity *p_CapabilityValidity;
    PHY_Support *p_PhySupport;
    ParingDfeProfileParam *p_ParingDfeProfileParam;
    DelayManagement *p_DelayManagement;
    Rel16_mTRP *p_Rel16_mTRP;

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
    p_FapiGenericHeader->type = MSG_PHY_PARAM_RESP;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_ParamResp = (PARAM_RESP *) (p_FapiGenericHeader+1);
    p_ParamResp->errorCode = MSG_OK;
    p_ParamResp->numOfTLV = 0;
    fapiBodyLength += sizeof(PARAM_RESP);

    p_NumOfTLV = &(p_ParamResp->numOfTLV);
    p_TlvHeader = &(p_ParamResp->tlvs[0]);
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Cell and PHY parameters in Table 3-13

    p_CellParam = &p_FapiParamInfo->cellParam;

    // Set Release capability
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_RELEASE_CAPABILITY))
        fapiBodyLength += tlv_AddTag_U16( TAG_RELEASE_CAPABILITY, p_CellParam->releaseCapability, &p_TlvHeader, p_NumOfTLV );

    // Set PHY State
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_PHY_STATE))
        fapiBodyLength += tlv_AddTag_U16( TAG_PHY_STATE, p_CellParam->phyState, &p_TlvHeader, p_NumOfTLV );

    // Set Skip_blank_DL_CONFIG
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_SKIP_BLANK_DL_CONFIG))
        fapiBodyLength += tlv_AddTag_U8( TAG_SKIP_BLANK_DL_CONFIG, p_CellParam->skipBlankDLConfig, &p_TlvHeader, p_NumOfTLV );

    // Set Skip_blank_DL_CONFIG
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_SKIP_BLANK_UL_CONFIG))
        fapiBodyLength += tlv_AddTag_U8( TAG_SKIP_BLANK_UL_CONFIG, p_CellParam->skipBlankULConfig, &p_TlvHeader, p_NumOfTLV );

    // Set NumConfigTLVsToReport
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_NUM_CONFIG_TLVS_TO_REPORT))
        fapiBodyLength += tlv_AddTag_NumConfigTlvsToReport( p_CellParam->numConfigTLVsToReport, p_CellParam->configTLVToReportList, &p_TlvHeader, p_NumOfTLV );

    // Set Power Profiles
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_POWER_PROFILES))
        fapiBodyLength += tlv_AddTag_PowerProfiles( p_CellParam->powerProfilesSupported, p_CellParam->powerProfileInfoList, &p_TlvHeader, p_NumOfTLV );

    // Set maxNumPDUsInDL_TTI
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_DL_TTI))
        fapiBodyLength += tlv_AddTag_U16( TAG_MAX_NUM_PDUS_IN_DL_TTI, p_CellParam->maxNumPDUsInDL_TTI, &p_TlvHeader, p_NumOfTLV );
    
    // Set SSB_Block_Power
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_SSPBCH_BLOCK_POWER_SCALING_UNIT_CHOICE))
         fapiBodyLength += tlv_AddTag_U8( TAG_SSPBCH_BLOCK_POWER_SCALING_UNIT_CHOICE, p_CellParam->ssPbchBlockPowerScalingUnitChoice, &p_TlvHeader, p_NumOfTLV );
        

    // Set maxNumPDUsInUL_TTI
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_UL_TTI))
        fapiBodyLength += tlv_AddTag_U16( TAG_MAX_NUM_PDUS_IN_UL_TTI, p_CellParam->maxNumPDUsInUL_TTI, &p_TlvHeader, p_NumOfTLV );

    // Set maxNumPDUsInUL_DCI
    if ((p_CellParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_UL_DCI))
        fapiBodyLength += tlv_AddTag_U16( TAG_MAX_NUM_PDUS_IN_UL_DCI, p_CellParam->maxNumPDUsInUL_DCI, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Carrier parameters in Table 3-13 

    p_Carrier_Param = &p_FapiParamInfo->carrierParam;

    // Set cyclicPrefix
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_CYCLIC_PREFIX))
        fapiBodyLength += tlv_AddTag_U8( TAG_CYCLIC_PREFIX, p_Carrier_Param->cyclicPrefix, &p_TlvHeader, p_NumOfTLV );

    // Set supportedSubcarrierSpacingsDl
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_DL))
        fapiBodyLength += tlv_AddTag_U8( TAG_SUPPORTED_SUBCARRIER_SPACING_DL, p_Carrier_Param->supportedSubcarrierSpacingsDl, &p_TlvHeader, p_NumOfTLV );

    // Set supportedSubcarrierSpacingsDl
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB))
        fapiBodyLength += tlv_AddTag_U8( TAG_SUPPORTED_SUBCARRIER_SPACING_SSB, p_Carrier_Param->supportedSubcarrierSpcacingSSB, &p_TlvHeader, p_NumOfTLV );

    // Set supportedBandwidthDl
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_BANDWIDTH_DL))
        fapiBodyLength += tlv_AddTag_U16( TAG_SUPPORTED_BANDWIDTH_DL, p_Carrier_Param->supportedBandwidthDl, &p_TlvHeader, p_NumOfTLV );

    // Set supportedSubcarrierSpacingsUl
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_UL))
        fapiBodyLength += tlv_AddTag_U8( TAG_SUPPORTED_SUBCARRIER_SPACING_UL, p_Carrier_Param->supportedSubcarrierSpacingsUl, &p_TlvHeader, p_NumOfTLV );

    // Set supportedBandwidthUl
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_BANDWIDTH_UL))
        fapiBodyLength += tlv_AddTag_U16( TAG_SUPPORTED_BANDWIDTH_UL, p_Carrier_Param->supportedBandwidthUl, &p_TlvHeader, p_NumOfTLV );

    // Set ssPbchMultipleCarriersInABand
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_SS_PBCH_MULTIPLE_CARRIERS_IN_A_BAND))
        fapiBodyLength += tlv_AddTag_U8( TAG_SS_PBCH_MULTIPLE_CARRIERS_IN_A_BAND, p_Carrier_Param->ssPbchMultipleCarriersInABand, &p_TlvHeader, p_NumOfTLV );

    // Set multipleCellsSsPbchInACarrier
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRIER))
        fapiBodyLength += tlv_AddTag_U8( TAG_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRIER, p_Carrier_Param->multipleCellsSsPbchInACarrier, &p_TlvHeader, p_NumOfTLV );
    
    // Set meangingOfCarrierFreqeuncy
    if ((p_Carrier_Param->tagBitmask & PARAM_TAG_BITMASK_MEANING_OF_CARRIER_FREQUENCY))
        fapiBodyLength += tlv_AddTag_U8( TAG_MEANING_OF_CARRIER_FREQUENCY, p_Carrier_Param->meaningOfCarrierFrequency, &p_TlvHeader, p_NumOfTLV );    

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PDCCH parameters in Table 3-15 

    p_PdcchParam = &p_FapiParamInfo->pdcchParam;

    // Set cceMappingType
    if ((p_PdcchParam->tagBitmask & PARAM_TAG_BITMASK_CCE_MAPPING_TYPE))
        fapiBodyLength += tlv_AddTag_U8( TAG_CCE_MAPPING_TYPE, p_PdcchParam->cceMappingType, &p_TlvHeader, p_NumOfTLV );

    // Set coresetOutsideFirst3OfdmSymsOfSlot
    if ((p_PdcchParam->tagBitmask & PARAM_TAG_BITMASK_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS))
        fapiBodyLength += tlv_AddTag_U8( TAG_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS_OF_SLOT, p_PdcchParam->coresetOutsideFirst3OfdmSymsOfSlot, &p_TlvHeader, p_NumOfTLV );

    // Set precoderGranularityCoreset
    if ((p_PdcchParam->tagBitmask & PARAM_TAG_BITMASK_PRECODER_GRANULARITY_CORESET))
        fapiBodyLength += tlv_AddTag_U8( TAG_PRECODER_GRANULARITY_CORESET, p_PdcchParam->precoderGranularityCoreset, &p_TlvHeader, p_NumOfTLV );

    // Set pdcchMuMimo
    if ((p_PdcchParam->tagBitmask & PARAM_TAG_BITMASK_PDCCH_MU_MIMO))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDCCH_MU_MIMO, p_PdcchParam->pdcchMuMimo, &p_TlvHeader, p_NumOfTLV );

    // Set pdcchPrecoderCycling
    if ((p_PdcchParam->tagBitmask & PARAM_TAG_BITMASK_PDCCH_PRECODER_CYCLING))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDCCH_PRECODER_CYCLING, p_PdcchParam->pdcchPrecoderCycling, &p_TlvHeader, p_NumOfTLV );

    // Set maxPdcchsPerSlot
    if ((p_PdcchParam->tagBitmask & PARAM_TAG_BITMASK_MAX_PDCCHS_PER_SLOT))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_PDCCHS_PER_SLOT, p_PdcchParam->maxPdcchsPerSlot, &p_TlvHeader, p_NumOfTLV ); 

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PUCCH parameters in Table 3-16 

    p_PucchParam = &p_FapiParamInfo->pucchParam;

    // Set pucchFormats
    if ((p_PucchParam->tagBitmask & PARAM_TAG_BITMASK_PUCCH_FORMATS))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUCCH_FORMATS, p_PucchParam->pucchFormats, &p_TlvHeader, p_NumOfTLV );

    // Set maxPucchsPerSlot
    if ((p_PucchParam->tagBitmask & PARAM_TAG_BITMASK_MAX_PUCCHS_PER_SLOT))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_PUCCHS_PER_SLOT, p_PucchParam->maxPucchsPerSlot, &p_TlvHeader, p_NumOfTLV );

    // Set pucchGroupAndSequenceHopping
    if ((p_PucchParam->tagBitmask & PARAM_TAG_BITMASK_PUCCH_GROUP_AND_SEQUENCE_HOPPING))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUCCH_GROUP_AND_SEQUENCE_HOPPING, p_PucchParam->pucchGroupAndSequenceHopping, &p_TlvHeader, p_NumOfTLV );

    // Set maxNumUlBwpIds
    if ((p_PucchParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUM_UL_BWP_IDS))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_NUM_UL_BWP_IDS, p_PucchParam->maxNumUlBwpIds, &p_TlvHeader, p_NumOfTLV );

    // Set pucchAggregation
    if ((p_PucchParam->tagBitmask & PARAM_TAG_BITMASK_PUCCH_AGGREGATION))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUCCH_AGGREGATION, p_PucchParam->pucchAggregation, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PDSCH parameters in Table 3-17 

    p_PdschParam = &p_FapiParamInfo->pdschParam;

    // Set pdschMappingType
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_MAPPING_TYPE))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_MAPPING_TYPE, p_PdschParam->pdschMappingType, &p_TlvHeader, p_NumOfTLV );

    // Set pdschAllocationTypes
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_ALLOCATION_TYPES))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_ALLOCATION_TYPES, p_PdschParam->pdschAllocationTypes, &p_TlvHeader, p_NumOfTLV );

    // Set pdschVrbToPrbMapping
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_VRB_TO_PRB_MAPPING))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_VRB_TO_PRB_MAPPING, p_PdschParam->pdschVrbToPrbMapping, &p_TlvHeader, p_NumOfTLV );

    // Set pdschCbg
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_CBG))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_CBG, p_PdschParam->pdschCbg, &p_TlvHeader, p_NumOfTLV );

    // Set pdschDmrsConfigTypes
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_DMRS_CONFIG_TYPES))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_DMRS_CONFIG_TYPES, p_PdschParam->pdschDmrsConfigTypes, &p_TlvHeader, p_NumOfTLV );

    // Set pdschDmrsMaxLength
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_DMRS_MAX_LENGTH))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_DMRS_MAX_LENGTH, p_PdschParam->pdschDmrsMaxLength, &p_TlvHeader, p_NumOfTLV );

    // Set pdschDmrsAdditionalPos
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_DMRS_ADDITIONAL_POS))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_DMRS_ADDITIONAL_POS, p_PdschParam->pdschDmrsAdditionalPos, &p_TlvHeader, p_NumOfTLV );

    // Set maxPdschsTBsPerSlot
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_PDSCHS_TBS_PER_SLOT))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_PDSCHS_TBS_PER_SLOT, p_PdschParam->maxPdschsTBsPerSlot, &p_TlvHeader, p_NumOfTLV );

    // Set maxNumberMimoLayersPdsch
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_PDSCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_NUMBER_MIMO_LAYERS_PDSCH, p_PdschParam->maxNumberMimoLayersPdsch, &p_TlvHeader, p_NumOfTLV );

    // Set supportedMaxModulationOrderDl
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_MAX_MODULATION_ORDER_DL))
        fapiBodyLength += tlv_AddTag_U8( TAG_SUPPORTED_MAX_MODULATION_ORDER_DL, p_PdschParam->supportedMaxModulationOrderDl, &p_TlvHeader, p_NumOfTLV );

    // Set maxMuMimoUsersDl
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_MU_MIMO_USERS_DL))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_MU_MIMO_USERS_DL, p_PdschParam->maxMuMimoUsersDl, &p_TlvHeader, p_NumOfTLV );

    // Set pdschDataInDmrsSymbols
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_DATA_IN_DMRS_SYMBOLS))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_DATA_IN_DMRS_SYMBOLS, p_PdschParam->pdschDataInDmrsSymbols, &p_TlvHeader, p_NumOfTLV );

    // Set premptionSupport
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PREMPTION_SUPPORT))
        fapiBodyLength += tlv_AddTag_U8( TAG_PREMPTION_SUPPORT, p_PdschParam->premptionSupport, &p_TlvHeader, p_NumOfTLV );

    // Set pdschNonSlotSupport
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_NON_SLOT_SUPPORT))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_NON_SLOT_SUPPORT, p_PdschParam->pdschNonSlotSupport, &p_TlvHeader, p_NumOfTLV );

    // Set ssbRateMatch
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_SSB_RATE_MATCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_SSB_RATE_MATCH, p_PdschParam->ssbRateMatch, &p_TlvHeader, p_NumOfTLV );

    // Set supportedRateMatchPatternType
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_RATE_MATCH_PATTERN_TYPE))
        fapiBodyLength += tlv_AddTag_U16( TAG_SUPPORTED_RATE_MATCH_PATTERN_TYPE, p_PdschParam->supportedRateMatchPatternType, &p_TlvHeader, p_NumOfTLV );

    // Set pdcchRateMatch
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDCCH_RATE_MATCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDCCH_RATE_MATCH, p_PdschParam->pdcchRateMatch, &p_TlvHeader, p_NumOfTLV );

    // Set numOfRateMatchPatternLTECrsPerSlot
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_NUM_OF_RATE_MATCH_PATTERN_LTE_CRS))
        fapiBodyLength += tlv_AddTag_U8( TAG_NUM_OF_RATE_MATCH_PATTERN_LTE_CRS, p_PdschParam->numOfRateMatchPatternLTECrsPerSlot, &p_TlvHeader, p_NumOfTLV );

    // Set numOfRateMatchPatternLTECrsInPhy
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_NUM_OF_RATE_MATCH_PATTER_LTE_CRS_IN_PHY))
        fapiBodyLength += tlv_AddTag_U8( TAG_NUM_OF_RATE_MATCH_PATTER_LTE_CRS_IN_PHY, p_PdschParam->numOfRateMatchPatternLTECrsInPhy, &p_TlvHeader, p_NumOfTLV );

    // Set lteCrsRateMatchMbsfnDerivation
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_LTE_CRS_RATE_MATCH_MB_SFN_DERIVATION))
        fapiBodyLength += tlv_AddTag_U8( TAG_LTE_CRS_RATEMATCH_MB_SFN_DERIVATION, p_PdschParam->lteCrsRateMatchMbsfnDerivation, &p_TlvHeader, p_NumOfTLV );
    // Set supportedLTECrsRateMatchMethod
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_LTE_CRS_RATE_MATCH_METHOD))
        fapiBodyLength += tlv_AddTag_U8( TAG_SUPPORTED_LTE_CRS_RATEMATCH_METHOD, p_PdschParam->supportedLTECrsRateMatchMethod, &p_TlvHeader, p_NumOfTLV );    

    // Set csiRsRateMatch
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_CSI_RS_RATE_MATCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_CSI_RS_RATE_MATCH, p_PdschParam->csiRsRateMatch, &p_TlvHeader, p_NumOfTLV );

    // Set pdschTransTypeSupport
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_TRANS_TYPE_SUPPORT))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_TRANS_TYPE_SUPPORT, p_PdschParam->pdschTransTypeSupport, &p_TlvHeader, p_NumOfTLV );

    // Set pdschMacPduBitAlignment
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_PDSCH_MAC_PDU_BIT_ALIGNMENT))
        fapiBodyLength += tlv_AddTag_U8( TAG_PDSCH_MAC_PDU_BIT_ALIGNMENT, p_PdschParam->pdschMacPduBitAlignment, &p_TlvHeader, p_NumOfTLV );

    // Set maxNumberPrbSymBitmapsPerSlot
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUMBER_PRB_SYM_BITMAPS_PER_SLOT))
        fapiBodyLength += tlv_AddTag_NumberPrbSymBitmapsPerSlot( &p_PdschParam->maxNumberPrbSymBitmapsPerSlot,  &p_TlvHeader, p_NumOfTLV );
/*
    // Set maxNumberCsiRsRateMatchPerSlot
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUMBER_CSI_RS_RATE_MATCH_PER_SLOT))
        fapiBodyLength += tlv_AddTag_U16( TAG_MAX_NUMBER_CSIRS_RATEMATCH_PER_SLOT, *p_PdschParam->maxNumberCsiRsRateMatchPerSlot, &p_TlvHeader, p_NumOfTLV );   
*/
    // Set maxNumSsbPerPdschSlot
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUM_SSB_PER_PDSCH_SLOT))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_NUM_SSB_PER_PDSCH_SLOT, p_PdschParam->maxNumSsbPerPdschSlot, &p_TlvHeader, p_NumOfTLV );   

    // Set universalRateMatch
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_UNIVERSAL_RATE_MATCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_UNIVERSAL_RATEMATCH, p_PdschParam->universalRateMatch, &p_TlvHeader, p_NumOfTLV );   

    // Set requiredPdschSignalingAssociations
    if ((p_PdschParam->tagBitmask & PARAM_TAG_BITMASK_REQUIRED_PDSCH_SIGNALING_ASSOCIATIONS))
        fapiBodyLength += tlv_AddTag_U8( TAG_REQUIRE_PDSCH_SIGNALING_ASSOCIATIONS, p_PdschParam->requiredPdschSignalingAssociations, &p_TlvHeader, p_NumOfTLV );   
    

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PUSCH parameters in Table 3-18

    p_PuschParam = &p_FapiParamInfo->puschParam;

    // Set uciMuxUlschInPusch
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_UCI_MUX_ULSCH_IN_PUSCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_UCI_MUX_ULSCH_IN_PUSCH, p_PuschParam->uciMuxUlschInPusch, &p_TlvHeader, p_NumOfTLV );

    // Set uciOnlyPusch
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_UCI_ONLY_PUSCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_UCI_ONLY_PUSCH, p_PuschParam->uciOnlyPusch, &p_TlvHeader, p_NumOfTLV );

    // Set puschFrequencyHopping
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_FREQUENCY_HOPPING))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_FREQUENCY_HOPPING, p_PuschParam->puschFrequencyHopping, &p_TlvHeader, p_NumOfTLV );

    // Set puschDmrsConfigTypes
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_DMRS_CONFIG_TYPES))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_DMRS_CONFIG_TYPES, p_PuschParam->puschDmrsConfigTypes, &p_TlvHeader, p_NumOfTLV );

    // Set puschDmrsMaxLen
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_DMRS_MAX_LEN))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_DMRS_MAX_LEN, p_PuschParam->puschDmrsMaxLen, &p_TlvHeader, p_NumOfTLV );

    // Set puschDmrsAdditionalPos
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_DMRS_ADDITIONAL_POS))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_DMRS_ADDITIONAL_POS, p_PuschParam->puschDmrsAdditionalPos, &p_TlvHeader, p_NumOfTLV );

    // Set puschCbg
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_CBG))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_CBG, p_PuschParam->puschCbg, &p_TlvHeader, p_NumOfTLV );

    // Set puschMappingType
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_MAPPING_TYPE))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_MAPPING_TYPE, p_PuschParam->puschMappingType, &p_TlvHeader, p_NumOfTLV );

    // Set puschAllocationTypes
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_ALLOCATION_TYPE))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_ALLOCATION_TYPE, p_PuschParam->puschAllocationTypes, &p_TlvHeader, p_NumOfTLV );

    // Set puschVrbToPrbMapping
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_VRB_TO_PRB_MAPPING))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_VRB_TO_PRB_MAPPING, p_PuschParam->puschVrbToPrbMapping, &p_TlvHeader, p_NumOfTLV );

    // Set puschMaxPtrsPorts
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_MAX_PTRS_PORTS))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_MAX_PTRS_PORTS, p_PuschParam->puschMaxPtrsPorts, &p_TlvHeader, p_NumOfTLV );

    // Set maxPduschsTBsPerSlot
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_PDUSCHS_TBS_PER_SLOT))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_PDUSCHS_TBS_PER_SLOT, p_PuschParam->maxPduschsTBsPerSlot, &p_TlvHeader, p_NumOfTLV );

    // Set maxNumberMimoLayersNonCbPusch
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_NON_CB_PUSCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_NUMBER_MIMO_LAYERS_NON_CB_PUSCH, p_PuschParam->maxNumberMimoLayersNonCbPusch, &p_TlvHeader, p_NumOfTLV );

    // Set maxNumberMimoLayersCbPusch
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_CB_PUSCH))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_NUMBER_MIMO_LAYERS_CB_PUSCH, p_PuschParam->maxNumberMimoLayersCbPusch, &p_TlvHeader, p_NumOfTLV );

    // Set supportedMaxModulationOrderUl
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_MAX_MODULATION_ORDER_UL))
        fapiBodyLength += tlv_AddTag_U8( TAG_SUPPORTED_MAX_MODULATION_ORDER_UL, p_PuschParam->supportedMaxModulationOrderUl, &p_TlvHeader, p_NumOfTLV );

    // Set maxMuMimoUsersUl
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_MAX_MU_MIMO_USERS_UL))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_MU_MIMO_USERS_UL, p_PuschParam->maxMuMimoUsersUl, &p_TlvHeader, p_NumOfTLV );

    // Set dftsOfdmSupport
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_DFTS_OFDM_SUPPORT))
        fapiBodyLength += tlv_AddTag_U8( TAG_DFTS_OFDM_SUPPORT, p_PuschParam->dftsOfdmSupport, &p_TlvHeader, p_NumOfTLV );

    // Set puschAggregationFactor
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_AGGREGATION_FACTOR))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_AGGREGATION_FACTOR, p_PuschParam->puschAggregationFactor, &p_TlvHeader, p_NumOfTLV );

    // Set puschLbrmSupport
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_LBRM_SUPPORT))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_LBRM_SUPPORT, p_PuschParam->puschLbrmSupport, &p_TlvHeader, p_NumOfTLV );

    // Set puschTransTypeSupport
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_TRANS_TYPE_SUPPORT))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_TRANS_TYPE_SUPPORT, p_PuschParam->puschTransTypeSupport, &p_TlvHeader, p_NumOfTLV );

    // Set puschMacPduBitAlignment
    if ((p_PuschParam->tagBitmask & PARAM_TAG_BITMASK_PUSCH_MAC_PDU_BIT_ALIGNMENT))
        fapiBodyLength += tlv_AddTag_U8( TAG_PUSCH_MAC_PDU_BIT_ALIGNMENT, p_PuschParam->puschMacPduBitAlignment, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PRACH parameters in Table 3-20 

    p_PrachParam = &p_FapiParamInfo->prachParam;

    // Set prachLongFormats
    if ((p_PrachParam->tagBitmask & PARAM_TAG_BITMASK_PRACH_LONG_FORMATS))
        fapiBodyLength += tlv_AddTag_U8( TAG_PRACH_LONG_FORMATS, p_PrachParam->prachLongFormats, &p_TlvHeader, p_NumOfTLV );

    // Set prachShortFormats 
    if ((p_PrachParam->tagBitmask & PARAM_TAG_BITMASK_PRACH_SHORT_FORMATS))
        fapiBodyLength += tlv_AddTag_U16( TAG_PRACH_SHORT_FORMATS, p_PrachParam->prachShortFormats, &p_TlvHeader, p_NumOfTLV );

    // Set prachRestrictedSets 
    if ((p_PrachParam->tagBitmask & PARAM_TAG_BITMASK_PRACH_RESTRICTED_SETS))
        fapiBodyLength += tlv_AddTag_U8( TAG_PRACH_RESTRICTED_SETS, p_PrachParam->prachRestrictedSets, &p_TlvHeader, p_NumOfTLV );

    // Set maxPrachFdOccasionsInASlot
    if ((p_PrachParam->tagBitmask & PARAM_TAG_BITMASK_MAX_PRACH_FD_OCCASIONS_IN_A_SLOT))
        fapiBodyLength += tlv_AddTag_U8( TAG_MAX_PRACH_FD_OCCASIONS_IN_A_SLOT, p_PrachParam->maxPrachFdOccasionsInASlot, &p_TlvHeader, p_NumOfTLV );

    // Set maxNumPrachConfigurations
    if ((p_PrachParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUM_PRACH_CONFIGURATIONS))
        fapiBodyLength += tlv_AddTag_U16( TAG_MAX_NUM_PRACH_CONFIGURATIONS, p_PrachParam->maxNumPrachConfigurations, &p_TlvHeader, p_NumOfTLV );

    // Set prachMultipleCarriersInABand
    if ((p_PrachParam->tagBitmask & PARAM_TAG_BITMASK_PRACH_MULTIPLE_CARRIERS_IN_A_BAND))
        fapiBodyLength += tlv_AddTag_U8( TAG_PRACH_MULTIPLE_CARRIERS_IN_A_BAND, p_PrachParam->prachMultipleCarriersInABand, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Measurement parameters in Table 3-19 

    p_MeasurementParam = &p_FapiParamInfo->measurementParam;

    // Set rssiMeasurementSupport
    if ((p_MeasurementParam->tagBitmask & PARAM_TAG_BITMASK_RSSI_MEASUREMENT_SUPPORT))
        fapiBodyLength += tlv_AddTag_U8( TAG_RSSI_MEASUREMENT_SUPPORT, p_MeasurementParam->rssiMeasurementSupport, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set UCI parameters in Table 3-20 

    p_UciParam = &p_FapiParamInfo->uciParam;

    // Set maxNumUciMaps
    if ((p_UciParam->tagBitmask & PARAM_TAG_BITMASK_MAX_NUM_UCI_MAPS))
        fapiBodyLength += tlv_AddTag_U16( TAG_MAX_NUM_UCI_MAPS, p_UciParam->maxNumUciMaps, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Capability validity scope parameters in Table 3-21 

    p_CapabilityValidity = &p_FapiParamInfo->capabilityValidity;

    // Set Capability validity
    if ((p_CapabilityValidity->tagBitmask & PARAM_TAG_BITMASK_CAPABILITIES))
        fapiBodyLength += tlv_AddTag_CapabilityValidity( p_CapabilityValidity, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PHY Support parameters in Table 3-22 

    p_PhySupport = &p_FapiParamInfo->phySupport;

    // Set Supported PHY Profiles
    if ((p_PhySupport->tagBitmask & PARAM_TAG_BITMASK_SUPPORTED_PHY_PROFILES))
        fapiBodyLength += tlv_AddTag_SupportedPhyProfiles( p_PhySupport->numPhyProfiles, p_PhySupport->numDlBbPorts, p_PhySupport->numUlBbPorts,
                              p_PhySupport->maxNumPhys, &p_PhySupport->supportedPhyProfileInfo[0][0], &p_TlvHeader, p_NumOfTLV );

    // Set timeManagement
    if ((p_PhySupport->tagBitmask & PARAM_TAG_BITMASK_TIME_MANAGEMENT))
        fapiBodyLength += tlv_AddTag_U8( TAG_TIME_MANAGEMENT, p_PhySupport->timeManagement, &p_TlvHeader, p_NumOfTLV );

    // Set FAPI ProtocolVersion
    if ((p_PhySupport->tagBitmask & PARAM_TAG_BITMASK_FAPI_PROTOCOL_VERSION))
        fapiBodyLength += tlv_AddTag_FapiProtocolVersion( &p_PhySupport->fapiProtocolVersion, &p_TlvHeader, p_NumOfTLV );

    // Set moreThanOneIndicationPerSlot
    if ((p_PhySupport->tagBitmask & PARAM_TAG_BITMASK_MORE_THAN_ONE_INDICATION_PER_SLOT))
        fapiBodyLength += tlv_AddTag_MoreThanOneIndicationPerSlot( &p_PhySupport->moreThanOneIndicationPerSlot, &p_TlvHeader, p_NumOfTLV );

    // Set moreThanOneRequestPerSlot 
    if ((p_PhySupport->tagBitmask & PARAM_TAG_BITMASK_MORE_THAN_ONE_INDICATION_PER_SLOT))
        fapiBodyLength += tlv_AddTag_MoreThanOneRequestPerSlot( &p_PhySupport->moreThanOneRequestPerSlot, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set PHY / DFE Profile Validity Map in Table 3-23

    p_ParingDfeProfileParam = &p_FapiParamInfo->paringDfeProfileParam;

    // Set ParingDfeProfileParam
    if ((p_PhySupport->tagBitmask & PARAM_TAG_BITMASK_PAIRING_DFE_PROFILES))
        fapiBodyLength += tlv_AddTag_ParingDfeProfileParam( p_ParingDfeProfileParam, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Delay management parameters in Table 3-24 

    p_DelayManagement = &p_FapiParamInfo->delayManagement;

    // Set dlTTI_TimingOffset
    if ((p_DelayManagement->tagBitmask & PARAM_TAG_BITMASK_DL_TTI_TIMING_OFFSET))
        fapiBodyLength += tlv_AddTag_U32( TAG_DL_TTI_TIMING_OFFSET, p_DelayManagement->dlTTI_TimingOffset, &p_TlvHeader, p_NumOfTLV );

    // Set ulTTI_TimingOffset
    if ((p_DelayManagement->tagBitmask & PARAM_TAG_BITMASK_UL_TTI_TIMING_OFFSET))
        fapiBodyLength += tlv_AddTag_U32( TAG_UL_TTI_TIMING_OFFSET, p_DelayManagement->ulTTI_TimingOffset, &p_TlvHeader, p_NumOfTLV );

    // Set ulDCI_TimingOffset
    if ((p_DelayManagement->tagBitmask & PARAM_TAG_BITMASK_UL_DCI_TIMING_OFFSET))
        fapiBodyLength += tlv_AddTag_U32( TAG_UL_DCI_TIMING_OFFSET, p_DelayManagement->ulDCI_TimingOffset, &p_TlvHeader, p_NumOfTLV );

    // Set txData_TimingOffset
    if ((p_DelayManagement->tagBitmask & PARAM_TAG_BITMASK_TX_DATA_TIMING_OFFSET))
        fapiBodyLength += tlv_AddTag_U32( TAG_TX_DATA_TIMING_OFFSET, p_DelayManagement->txData_TimingOffset, &p_TlvHeader, p_NumOfTLV );

    // Set timingWindow
    if ((p_DelayManagement->tagBitmask & PARAM_TAG_BITMASK_TIMING_WINDOW))
        fapiBodyLength += tlv_AddTag_U16( TAG_TIMING_WINDOW, p_DelayManagement->timingWindow, &p_TlvHeader, p_NumOfTLV );

    // Set timingInfoPeriod
    if ((p_DelayManagement->tagBitmask & PARAM_TAG_BITMASK_TIMING_INFO_PERIOD))
        fapiBodyLength += tlv_AddTag_U8( TAG_TIMING_INFO_PERIOD, p_DelayManagement->timingInfoPeriod, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Set Rel-16 mTRP parameters in Table 3-25

    p_Rel16_mTRP = &p_FapiParamInfo->rel16_mTRP;

    // Set puschLbrmSupport
    if ((p_Rel16_mTRP->tagBitmask & PARAM_TAG_BITMASK_MTRP_SUPPORT))
        fapiBodyLength += tlv_AddTag_U32( TAG_MTRP_SUPPORT, p_Rel16_mTRP->mTRP_Support, &p_TlvHeader, p_NumOfTLV );

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Finalize

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "Param Resp created : msgLength %u\n", *p_MsgLength );
}

void phy_CreateConfigResp( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    CONFIG_RESP *p_ConfigResp;

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
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "Config resp created : msgLength %u\n", *p_MsgLength );
}

void phy_CreateRxDataInd( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    Rx_Data_IND *p_RxDataInd;

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
    p_FapiGenericHeader->type = MSG_PHY_RX_DATA_IND;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_RxDataInd = (Rx_Data_IND *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(Rx_Data_IND);

    p_RxDataInd->sfn = 123;
    p_RxDataInd->slot = 9;
    p_RxDataInd->numPDUs = 0;

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "RxDataInd created : msgLength %u\n", *p_MsgLength );
}

void phy_CreateCrcInd( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    CRC_IND *p_CrcInd;

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
    p_FapiGenericHeader->type = MSG_PHY_CRC_IND;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_CrcInd = (CRC_IND *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(CRC_IND);

    p_CrcInd->sfn = 123;
    p_CrcInd->slot = 9;
    p_CrcInd->numCRCs = 0;

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "CrcInd created : msgLength %u\n", *p_MsgLength );
}

void phy_CreateUciInd( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    UCI_IND *p_UciInd;

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
    p_FapiGenericHeader->type = MSG_PHY_UCI_IND;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_UciInd = (UCI_IND *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(UCI_IND);

    p_UciInd->sfn = 123;
    p_UciInd->slot = 9;
    p_UciInd->numUCIs = 0;

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "UciInd created : msgLength %u\n", *p_MsgLength );
}

void phy_CreateRachInd( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    RACH_IND *p_RachInd;

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
    p_FapiGenericHeader->type = MSG_PHY_RACH_IND;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_RachInd = (RACH_IND *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(Rx_Data_IND);

    p_RachInd->sfn = 123;
    p_RachInd->slot = 9;
    p_RachInd->numPDUs = 0;

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "RachInd created : msgLength %u\n", *p_MsgLength );
}

void phy_CreateSrsInd( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex )
{
    uint32_t fapiBodyLength;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    SRS_IND *p_SrsInd;

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
    p_FapiGenericHeader->type = MSG_PHY_SRS_IND;
    p_FapiGenericHeader->length = 0;    // Filled at the end
    fapiBodyLength = 0;

    p_SrsInd = (SRS_IND *) (p_FapiGenericHeader+1);
    fapiBodyLength += sizeof(Rx_Data_IND);

    p_SrsInd->sfn = 123;
    p_SrsInd->slot = 9;
    p_SrsInd->numPDUs = 0;

    // Set final fapi length
    p_FapiGenericHeader->length = fapiBodyLength;

    // Set final msg length
    *p_MsgLength += sizeof(Fapi_Generic_Header) + fapiBodyLength;

    p_FapiApiHeader->msgSize = *p_MsgLength;

    FAPI_Output( "SrsInd created : msgLength %u\n", *p_MsgLength );
}

///////////////////////////////////////////////////////////////////////////////////////////
// Parsing routiunes for MAC

void mac_ParseMsg_ParamResp( Fapi_Generic_Header *p_FapiGenericHeader, FapiParamInfo *p_FapiParamInfo )
{
    uint32_t tlvIndex, tlvCount, tlvValueLength, tlvLength, *p_Value_U32, value_U32;
    uint16_t *p_Value_U16, value_U16, releaseCapability, phyState, numConfigTlvsToReport, configTlvsToReportIndex, powerProfileIndex;
    uint16_t numCapabilities, capabilityValidityEntryIndex, numPhyProfiles, phyProfileIndex, numPhyIndex, portRangeIndex;
    uint16_t profileValidityMapSize, supportedBandwidth;
    uint8_t value_U8, *p_Value_U8, supportedSubcarrierSpacing,supportedSubcarrierSpacingSSB;
    uint8_t numerologyIndex;
    PARAM_RESP *p_ParamResp;
    TLV_Header *p_TlvHeader;
    ConfigTLVToReport *p_ConfigTlvToReport;
    PowerProfileInfo *p_PowerProfileInfo;
    CapabilityValidityEntry *p_CapabilityValidityEntry;
    FAPIProtocolVersion *p_FapiProtocolVersion;
    IndicationInstancePerSlotInfo *p_MoreThanOneIndicationPerSlot;
    RequestInstancePerSlotInfo *p_MoreThanOneRequestPerSlot;
    SupportedPhyProfileInfo *p_SupportedPhyProfileInfo;
    PrbSymBitmapsPerSlotInfo *p_PrbSymBitmapsPerSlotInfo;
    Cell_Param *p_CellParam;
    Carrier_Param *p_Carrier_Param;
    PDCCH_Param *p_PdcchParam;
    PUCCH_Param *p_PucchParam;
    PDSCH_Param *p_PdschParam;
    PUSCH_Param *p_PuschParam;
    PRACH_Param *p_PrachParam;
    Measurement_Param *p_MeasurementParam;
    UCI_Param *p_UciParam;
    Capability_Validity *p_CapabilityValidity; 
    PHY_Support *p_PhySupport;
    ParingDfeProfileParam *p_ParingDfeProfileParam;
    DelayManagement *p_DelayManagement;
    Rel16_mTRP *p_Rel16_mTRP;
   
    p_CellParam = &p_FapiParamInfo->cellParam;
    p_Carrier_Param = &p_FapiParamInfo->carrierParam;
    p_PdcchParam = &p_FapiParamInfo->pdcchParam;
    p_PucchParam = &p_FapiParamInfo->pucchParam;
    p_PdschParam = &p_FapiParamInfo->pdschParam;
    p_PuschParam = &p_FapiParamInfo->puschParam;
    p_PrachParam = &p_FapiParamInfo->prachParam;
    p_MeasurementParam = &p_FapiParamInfo->measurementParam;
    p_UciParam = &p_FapiParamInfo->uciParam;
    p_CapabilityValidity = &p_FapiParamInfo->capabilityValidity;
    p_PhySupport = &p_FapiParamInfo->phySupport;
    p_ParingDfeProfileParam = &p_FapiParamInfo->paringDfeProfileParam;
    p_DelayManagement = &p_FapiParamInfo->delayManagement;
    p_Rel16_mTRP = &p_FapiParamInfo->rel16_mTRP;

    FAPI_Output( "    type      : MSG_PHY_PARAM_RESP(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length    : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_ParamResp = (PARAM_RESP *) (p_FapiGenericHeader+1);
    if (p_ParamResp->errorCode == MSG_OK)
    {
        FAPI_Output( "    errorCode : MSG_OK(%u)\n", p_ParamResp->errorCode );
    }
    else
    {
        FAPI_Output( "    errorCode : %u\n", p_ParamResp->errorCode );
    }

    tlvCount = p_ParamResp->numOfTLV;
    FAPI_Output( "    numOfTLV  : %u\n", tlvCount );
    p_TlvHeader = (TLV_Header *) p_ParamResp->tlvs;

    for (tlvIndex = 0; tlvIndex < tlvCount; tlvIndex++)
    {
        FAPI_Output( "    TLV %u/%u : \n", tlvIndex+1, tlvCount );

        tlvValueLength = p_TlvHeader->length;
        tlvLength = sizeof(TLV_Header)+tlvValueLength;

        switch (p_TlvHeader->tag)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-13 : Cell and PHY parameters

            case TAG_RELEASE_CAPABILITY :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                releaseCapability = *p_Value_U16;
                FAPI_Output( "      tag    : TAG_RELEASE_CAPABILITY(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :" );
                if (releaseCapability == 0)
                {
                    FAPI_Output( " %u\n", *p_Value_U16 );
                }
                else    
                {
                    if ((releaseCapability & BITMASK_REL_CAPABILITY_SUPPORT_REL15))
                    {
                        FAPI_Output( " REL_CAPABILITY_SUPPORT_REL15" );
                        if ((releaseCapability & BITMASK_REL_CAPABILITY_SUPPORT_REL16))
                            FAPI_Output( "|REL_CAPABILITY_SUPPORT_REL16" );
                    }
                    else if ((releaseCapability & BITMASK_REL_CAPABILITY_SUPPORT_REL16))
                    {
                        FAPI_Output( "REL_CAPABILITY_SUPPORT_REL16" );
                    }
                    FAPI_Output( " (0x%x)\n", releaseCapability );
                }

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_RELEASE_CAPABILITY;
                p_CellParam->releaseCapability = releaseCapability;
                break;
           
            case TAG_PHY_STATE :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                phyState = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_PHY_STATE(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : " );
                if (phyState == PHY_STATE_IDLE)
                    FAPI_Output( "PHY_STATE_IDLE(%u)\n", phyState );
                else if (phyState == PHY_STATE_CONFIGURED)
                    FAPI_Output( "PHY_STATE_CONFIGURED(%u)\n", phyState );
                else if (phyState == PHY_STATE_RUNNING)
                    FAPI_Output( "PHY_STATE_RUNNING(%u)\n", phyState );
                else 
                    FAPI_Output( "%u : Invalid\n", phyState );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_PHY_STATE;
                p_CellParam->phyState = phyState;
                break;
           
            case TAG_SKIP_BLANK_DL_CONFIG :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_SKIP_BLANK_DL_CONFIG(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                if (value_U8 == FEATURE_NOT_SUPPORTED)
                    FAPI_Output( "      value  : FEATURE_NOT_SUPPORTED(%u)\n", value_U8 );
                else if (value_U8 == FEATURE_SUPPORTED)
                    FAPI_Output( "      value  : FEATURE_SUPPORTED(%u)\n", value_U8 );
                else
                    FAPI_Output( "      value  : %u : Invalid\n", value_U8 );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_SKIP_BLANK_DL_CONFIG;
                p_CellParam->skipBlankDLConfig = value_U8;
                break;

            case TAG_SKIP_BLANK_UL_CONFIG :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_SKIP_BLANK_UL_CONFIG(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                if (value_U8 == FEATURE_NOT_SUPPORTED)
                    FAPI_Output( "      value  : FEATURE_NOT_SUPPORTED(%u)\n", value_U8 );
                else if (value_U8 == FEATURE_SUPPORTED)
                    FAPI_Output( "      value  : FEATURE_SUPPORTED(%u)\n", value_U8 );
                else
                    FAPI_Output( "      value  : %u : Invalid\n", value_U8 );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_SKIP_BLANK_UL_CONFIG;
                p_CellParam->skipBlankULConfig = value_U8;
                break;

            case TAG_NUM_CONFIG_TLVS_TO_REPORT :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                numConfigTlvsToReport = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_NUM_CONFIG_TLVS_TO_REPORT(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u\n", numConfigTlvsToReport );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_NUM_CONFIG_TLVS_TO_REPORT;
                p_CellParam->numConfigTLVsToReport = numConfigTlvsToReport;

                p_ConfigTlvToReport = (ConfigTLVToReport *) ((uint8_t *)p_TlvHeader + sizeof(TLV_Header) + sizeof(uint16_t));
                for (configTlvsToReportIndex = 0; configTlvsToReportIndex < numConfigTlvsToReport; configTlvsToReportIndex++)
                {
                    FAPI_Output( "        Sub TLV %u/%u : \n", configTlvsToReportIndex+1, numConfigTlvsToReport );

                    if (p_ConfigTlvToReport->tag == TAG_PHY_CELL_ID)
                        FAPI_Output( "          tag    : TAG_PHY_CELL_ID(0x%04X)\n", p_ConfigTlvToReport->tag );
                    else if (p_ConfigTlvToReport->tag == TAG_CARRIER_CONFIGURATION)
                        FAPI_Output( "          tag    : TAG_CARRIER_CONFIGURATION(0x%04X)\n", p_ConfigTlvToReport->tag );
                    else
                        FAPI_Output( "          tag    : 0x04%x\n", p_ConfigTlvToReport->tag );

                    FAPI_Output( "          length : %u\n", p_ConfigTlvToReport->length );
                    if (p_ConfigTlvToReport->value == IDLE_ONLY_MANDATORY)
                        FAPI_Output( "          value  : IDLE_ONLY_MANDATORY(%u)\n", p_ConfigTlvToReport->value );
                    else
                        FAPI_Output( "          value  : %u\n", p_ConfigTlvToReport->value );

                    p_CellParam->configTLVToReportList[configTlvsToReportIndex] = *p_ConfigTlvToReport;

                    p_ConfigTlvToReport++;
                }

                break;

            case TAG_POWER_PROFILES :
                value_U8 = p_TlvHeader->value[0];
                p_PowerProfileInfo = (PowerProfileInfo *) (&(p_TlvHeader->value[1]));

                FAPI_Output( "      tag    : TAG_POWER_PROFILES(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : 0x%x : ", value_U8 );
                if ((value_U8 & BITMASK_POWER_PROFILE_SUPPORT_NR))
                    FAPI_Output( " NR" );
                if ((value_U8 & BITMASK_POWER_PROFILE_SUPPORT_SSS))
                    FAPI_Output( " SSS" );
                FAPI_Output( "\n" );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_POWER_PROFILES;
                p_CellParam->powerProfilesSupported = value_U8;

                for (powerProfileIndex = 0; powerProfileIndex < POWER_PROFILE_SIGNAL_INDEX_COUNT; powerProfileIndex++)
                {
                    if (powerProfileIndex == POWER_PROFILE_SIGNAL_INDEX_PDCCH_DMRS)
                    {
                        FAPI_Output( "               Profile Info %u/%u : PDCCH_DMRS\n", powerProfileIndex+1, POWER_PROFILE_SIGNAL_INDEX_COUNT );
                        FAPI_Output( "                 referenceRS    : Support SSS          : %u\n", 
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDCCH_DMRS_SSS_SUPPORT) ? TRUE : FALSE );
                    }
                    else if (powerProfileIndex == POWER_PROFILE_SIGNAL_INDEX_PDCCH_DATA)
                    {
                        FAPI_Output( "               Profile Info %u/%u : PDCCH_DATA\n", powerProfileIndex+1, POWER_PROFILE_SIGNAL_INDEX_COUNT );
                        FAPI_Output( "                 referenceRS    : Support SSS          : %u          Support PDCCH-DMRS   : %u\n", 
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDCCH_DATA_SSS_SUPPORT) ? TRUE : FALSE,
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDCCH_DATA_PDCCH_DMRS_SUPPORT) ? TRUE : FALSE );
                    }
                    else if (powerProfileIndex == POWER_PROFILE_SIGNAL_INDEX_PDSCH_DMRS)
                    {
                        FAPI_Output( "               Profile Info %u/%u : PDSCH_DMRS\n", powerProfileIndex+1, POWER_PROFILE_SIGNAL_INDEX_COUNT );
                        FAPI_Output( "                 referenceRS    : Support SSS          : %u\n", 
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_DMRS_SSS_SUPPORT) ? TRUE : FALSE );
                    }
                    else if (powerProfileIndex == POWER_PROFILE_SIGNAL_INDEX_PDSCH_DATA)
                    {
                        FAPI_Output( "               Profile Info %u/%u : PDSCH_DATA\n", powerProfileIndex+1, POWER_PROFILE_SIGNAL_INDEX_COUNT );
                        FAPI_Output( "                 referenceRS    : Support SSS          : %u          Support PDSCH-DMRS   : %u\n", 
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_DATA_SSS_SUPPORT) ? TRUE : FALSE,
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_DATA_PDSCH_DMRS_SUPPORT) ? TRUE : FALSE );
                    }
                    else if (powerProfileIndex == POWER_PROFILE_SIGNAL_INDEX_PDSCH_PTRS)
                    {
                        FAPI_Output( "               Profile Info %u/%u : PDSCH_PTRS\n", powerProfileIndex+1, POWER_PROFILE_SIGNAL_INDEX_COUNT );
                        FAPI_Output( "                 referenceRS    : Support SSS          : %u          Support PDSCH-DMRS   : %u\n", 
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_PTRS_SSS_SUPPORT) ? TRUE : FALSE,
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_PTRS_PDSCH_DMRS_SUPPORT) ? TRUE : FALSE );
                    }
                    else if (powerProfileIndex == POWER_PROFILE_SIGNAL_INDEX_CSI_RS)
                    {
                        FAPI_Output( "               Profile Info %u/%u : CSI_RS\n", powerProfileIndex+1, POWER_PROFILE_SIGNAL_INDEX_COUNT );
                        FAPI_Output( "                 referenceRS    : Support SSS          : %u          Support SSS_1        : %u\n", 
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_CSI_RS_SSS_SUPPORT) ? TRUE : FALSE,
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_CSI_RS_SSS_1_SUPPORT) ? TRUE : FALSE );
                    }
                    else if (powerProfileIndex == POWER_PROFILE_SIGNAL_INDEX_PSS)
                    {
                        FAPI_Output( "               Profile Info %u/%u : PSS\n", powerProfileIndex+1, POWER_PROFILE_SIGNAL_INDEX_COUNT );
                        FAPI_Output( "                 referenceRS    : Support SSS          : %u          Support SSS_1        : %u\n", 
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_PSS_SSS_SUPPORT) ? TRUE : FALSE,
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_PSS_SSS_1_SUPPORT) ? TRUE : FALSE );
                    }
                    else if (powerProfileIndex == POWER_PROFILE_SIGNAL_INDEX_SSS)
                    {
                        FAPI_Output( "               Profile Info %u/%u : SSS\n", powerProfileIndex+1, POWER_PROFILE_SIGNAL_INDEX_COUNT );
                        FAPI_Output( "                 referenceRS    : Support unscaled SSS : %u          Support SCF-222 v20  : %u\n", 
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_SSS_UNSCALED_SSS_SUPPORT) ? TRUE : FALSE,
                            (p_PowerProfileInfo->referenceRS & REFERENCE_RS_PDSCH_SSS_SCF_222_V20_SUPPORT) ? TRUE : FALSE );
                    }
                    FAPI_Output( "                 powerOffsetMin : %u\n", p_PowerProfileInfo->powerOffsetMin );
                    FAPI_Output( "                 powerOffsetMax : %u\n", p_PowerProfileInfo->powerOffsetMax );

                    p_CellParam->powerProfileInfoList[powerProfileIndex] = *p_PowerProfileInfo;

                    p_PowerProfileInfo++;
                }

                break;

            case TAG_SSPBCH_BLOCK_POWER_SCALING_UNIT_CHOICE:
                p_Value_U8 = (uint8_t *) p_TlvHeader->value;
                value_U8 = *p_Value_U8;

                FAPI_Output( "      tag    : TAG_SSPBCH_BLOCK_POWER_SCALING_UNIT_CHOICE(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u\n", value_U8 );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_SSPBCH_BLOCK_POWER_SCALING_UNIT_CHOICE;
                p_CellParam->ssPbchBlockPowerScalingUnitChoice= value_U8;
                break;     

            case TAG_MAX_NUM_PDUS_IN_DL_TTI :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_MAX_NUM_PDUS_IN_DL_TTI(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u\n", value_U16 );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_DL_TTI;
                p_CellParam->maxNumPDUsInDL_TTI = value_U16;
                break;
            

            case TAG_MAX_NUM_PDUS_IN_UL_TTI :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_MAX_NUM_PDUS_IN_UL_TTI(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u\n", value_U16 );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_UL_TTI;
                p_CellParam->maxNumPDUsInUL_TTI = value_U16;
                break;

            case TAG_MAX_NUM_PDUS_IN_UL_DCI :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_MAX_NUM_PDUS_IN_UL_DCI(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u\n", value_U16 );

                p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_UL_DCI;
                p_CellParam->maxNumPDUsInUL_DCI = value_U16;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-14 : Carrier parameters

            case TAG_CYCLIC_PREFIX :
                value_U8 = p_TlvHeader->value[0];

               
                FAPI_Output( "      tag    : TAG_CYCLIC_PREFIX(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
           
                 if ((p_Carrier_Param->cyclicPrefix & BITMASK_CYCLIC_PREFIX_SUPPORT_NORMAL))
                    FAPI_Output( "      value  : CyclicPrefix Extended\n" );
                 else
                    FAPI_Output( "      value  : CyclicPrefix Normal\n" );

                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_CYCLIC_PREFIX;
                p_Carrier_Param->cyclicPrefix = value_U8;
                break;

            case TAG_SUPPORTED_SUBCARRIER_SPACING_DL :
                supportedSubcarrierSpacing = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_SUPPORTED_SUBCARRIER_SPACING_DL(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", supportedSubcarrierSpacing );

                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_15_KHZ))
                    FAPI_Output( " 15" );
                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_30_KHZ))
                    FAPI_Output( " 30" );
                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_60_KHZ))
                    FAPI_Output( " 60" );
                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_120_KHZ))
                    FAPI_Output( " 120" );
                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_240_KHZ))
                    FAPI_Output( " 240" );
                FAPI_Output( " KHz\n" );

                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_DL;
                p_Carrier_Param->supportedSubcarrierSpacingsDl = supportedSubcarrierSpacing;
                break;

            case TAG_SUPPORTED_SUBCARRIER_SPACING_SSB:
               supportedSubcarrierSpacingSSB = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_SUPPORTED_SUBCARRIER_SPACING_SSB(0x%04X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", supportedSubcarrierSpacingSSB );

                if ((supportedSubcarrierSpacingSSB & BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_15_KHZ))
                    FAPI_Output( " 15" );
                if ((supportedSubcarrierSpacingSSB & BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_30_KHZ))
                    FAPI_Output( " 30" );
                if ((supportedSubcarrierSpacingSSB & BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_120_KHZ))
                    FAPI_Output( " 120" );
                if ((supportedSubcarrierSpacingSSB & BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_240_KHZ))
                    FAPI_Output( " 240" );
                FAPI_Output( " KHz\n" );

                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB;
                p_Carrier_Param->supportedSubcarrierSpcacingSSB = supportedSubcarrierSpacingSSB;
                break;    

            case TAG_SUPPORTED_BANDWIDTH_DL :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                supportedBandwidth = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_SUPPORTED_BANDWIDTH_DL(0x04%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", supportedBandwidth );

                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_5_MHZ))
                    FAPI_Output( " 5" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_10_MHZ))
                    FAPI_Output( " 10" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_15_MHZ))
                    FAPI_Output( " 15" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_20_MHZ))
                    FAPI_Output( " 20" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_25_MHZ))
                    FAPI_Output( " 25" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_40_MHZ))
                    FAPI_Output( " 40" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_50_MHZ))
                    FAPI_Output( " 50" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_60_MHZ))
                    FAPI_Output( " 60" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_70_MHZ))
                    FAPI_Output( " 70" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_80_MHZ))
                    FAPI_Output( " 80" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_90_MHZ))
                    FAPI_Output( " 90" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_100_MHZ))
                    FAPI_Output( " 100" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_200_MHZ))
                    FAPI_Output( " 200" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_400_MHZ))
                    FAPI_Output( " 400" );
                FAPI_Output( " MHz\n" );

                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_BANDWIDTH_DL;
                p_Carrier_Param->supportedBandwidthDl = supportedBandwidth;
                break;

            case TAG_SUPPORTED_SUBCARRIER_SPACING_UL :
                supportedSubcarrierSpacing = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_SUPPORTED_SUBCARRIER_SPACING_UL(0x04%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", supportedSubcarrierSpacing );

                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_15_KHZ))
                    FAPI_Output( " 15" );
                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_30_KHZ))
                    FAPI_Output( " 30" );
                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_60_KHZ))
                    FAPI_Output( " 60" );
                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_120_KHZ))
                    FAPI_Output( " 120" );
                if ((supportedSubcarrierSpacing & BITMASK_SUBCARRIER_SPACING_SUPPORT_240_KHZ))
                    FAPI_Output( " 240" );
                FAPI_Output( " KHz\n" );

                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_UL;
                p_Carrier_Param->supportedSubcarrierSpacingsUl = supportedSubcarrierSpacing;
                break;

            case TAG_SUPPORTED_BANDWIDTH_UL :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                supportedBandwidth = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_SUPPORTED_BANDWIDTH_UL(0x04%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", supportedBandwidth );

                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_5_MHZ))
                    FAPI_Output( " 5" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_10_MHZ))
                    FAPI_Output( " 10" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_15_MHZ))
                    FAPI_Output( " 15" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_20_MHZ))
                    FAPI_Output( " 20" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_25_MHZ))
                    FAPI_Output( " 25" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_40_MHZ))
                    FAPI_Output( " 40" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_50_MHZ))
                    FAPI_Output( " 50" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_60_MHZ))
                    FAPI_Output( " 60" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_70_MHZ))
                    FAPI_Output( " 70" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_80_MHZ))
                    FAPI_Output( " 80" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_90_MHZ))
                    FAPI_Output( " 90" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_100_MHZ))
                    FAPI_Output( " 100" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_200_MHZ))
                    FAPI_Output( " 200" );
                if ((supportedBandwidth & BITMASK_BANDWIDTH_SUPPORT_400_MHZ))
                    FAPI_Output( " 400" );
                FAPI_Output( " MHz\n" );

                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_BANDWIDTH_UL;
                p_Carrier_Param->supportedBandwidthUl = supportedBandwidth;
                break;

            case TAG_SS_PBCH_MULTIPLE_CARRIERS_IN_A_BAND :
                value_U8 = p_TlvHeader->value[0];

                
                FAPI_Output( "      tag    : TAG_SS_PBCH_MULTIPLE_CARRIERS_IN_A_BAND(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                if((value_U8&BITMASK_SS_PBCH_MULTIPLE_CARRIER_IN_A_BAND_ENABLE))
                     FAPI_Output( "Enable\n" );
                else 
                      FAPI_Output( "Disable\n" );
                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_SS_PBCH_MULTIPLE_CARRIERS_IN_A_BAND;
                p_Carrier_Param->ssPbchMultipleCarriersInABand = value_U8;
                break;

            case TAG_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRIER :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRIER(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );

                if((value_U8&BITMASK_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRRIER_ENABLE))
                     FAPI_Output( "Enable\n" );
                else 
                      FAPI_Output( "Disable\n" );

                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRIER;
                p_Carrier_Param->multipleCellsSsPbchInACarrier = value_U8;
                break;

            case TAG_MEANING_OF_CARRIER_FREQUENCY:
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_MEANING_OF_CARRIER_FREQUENCY(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                if((value_U8&BITMASK_MEANING_OF_CARRIER_FREQUENCY_CENTER))
                     FAPI_Output( "RF Center Frequency f0\n" );
                else 
                      FAPI_Output( "Point-A\n" );

                p_Carrier_Param->tagBitmask |= PARAM_TAG_BITMASK_MEANING_OF_CARRIER_FREQUENCY;
                p_Carrier_Param->meaningOfCarrierFrequency= value_U8;
                break;     

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-15 : PDCCH parameters

            case TAG_CCE_MAPPING_TYPE :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_CCE_MAPPING_TYPE(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                if((value_U8&BITMASK_CCE_MAPPING_TYPE_SUPPORT_INTERLEAVED))
                     FAPI_Output( "Non-interleaved\n" );
                else 
                      FAPI_Output( "Interleaved\n" );

                p_PdcchParam->tagBitmask |= PARAM_TAG_BITMASK_CCE_MAPPING_TYPE;
                p_PdcchParam->cceMappingType = value_U8;
                break;

            case TAG_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS_OF_SLOT :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS_OF_SLOT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );

                p_PdcchParam->tagBitmask |= PARAM_TAG_BITMASK_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS;
                p_PdcchParam->coresetOutsideFirst3OfdmSymsOfSlot = value_U8;
                break;

            case TAG_PRECODER_GRANULARITY_CORESET :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PRECODER_GRANULARITY_CORESET(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );

                p_PdcchParam->tagBitmask |= PARAM_TAG_BITMASK_PRECODER_GRANULARITY_CORESET;
                p_PdcchParam->precoderGranularityCoreset = value_U8;
                break;

            case TAG_PDCCH_MU_MIMO :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDCCH_MU_MIMO(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );
                p_PdcchParam->tagBitmask |= PARAM_TAG_BITMASK_PDCCH_MU_MIMO;
                p_PdcchParam->pdcchMuMimo = value_U8;
                FAPI_Output( "      tag    : TAG_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS_OF_SLOT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                break;

            case TAG_PDCCH_PRECODER_CYCLING :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDCCH_PRECODER_CYCLING(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );
                p_PdcchParam->tagBitmask |= PARAM_TAG_BITMASK_PDCCH_PRECODER_CYCLING;
                p_PdcchParam->pdcchPrecoderCycling = value_U8;
                break;

            case TAG_MAX_PDCCHS_PER_SLOT :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_MAX_PDCCHS_PER_SLOT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );
                p_PdcchParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_PDCCHS_PER_SLOT;
                p_PdcchParam->maxPdcchsPerSlot = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-16 : PUCCH parameters

            case TAG_PUCCH_FORMATS :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUCCH_FORMATS(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );

                if ((value_U8 & BITMASK_PUCCH_SUPPORT_FORMAT_0))
                    FAPI_Output( " Format 0\n" );
                if ((value_U8 & BITMASK_PUCCH_SUPPORT_FORMAT_1))
                    FAPI_Output( " Format 1\n" );
                if ((value_U8 & BITMASK_PUCCH_SUPPORT_FORMAT_2))
                    FAPI_Output( " Format 2\n" );
                if ((value_U8 & BITMASK_PUCCH_SUPPORT_FORMAT_3))
                    FAPI_Output( " Format 3\n" );
                if ((value_U8 & BITMASK_PUCCH_SUPPORT_FORMAT_4))
                    FAPI_Output( " Format 4\n" );
                
                p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_PUCCH_FORMATS;
                p_PucchParam->pucchFormats = value_U8;
                break;

            case TAG_MAX_PUCCHS_PER_SLOT :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_MAX_PUCCHS_PER_SLOT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u : \n", value_U8 );

                p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_PUCCHS_PER_SLOT;
                p_PucchParam->maxPucchsPerSlot = value_U8;
                break;

            case TAG_PUCCH_GROUP_AND_SEQUENCE_HOPPING :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_PUCCH_GROUP_AND_SEQUENCE_HOPPING(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );

                if(value_U8&PUCCH_GROUP_AND_SEQ_HOPPING_DYNAMIC_ONLY)
                   FAPI_Output( " Dynamic Only 3\n" );
                if(value_U8&PUCCH_GROUP_AND_SEQ_HOPPING_SEMI_STATIC_ONLY)
                   FAPI_Output( " Semi-Static Only Only 3\n" );
                else
                   FAPI_Output( " Both \n" );

                p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_PUCCH_GROUP_AND_SEQUENCE_HOPPING;
                p_PucchParam->pucchGroupAndSequenceHopping = value_U8;
                break;

            case TAG_MAX_NUM_UL_BWP_IDS :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_MAX_NUM_UL_BWP_IDS(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u : \n", value_U8 );
                
                p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_UL_BWP_IDS;
                p_PucchParam->maxNumUlBwpIds = value_U8;
                break;

            case TAG_PUCCH_AGGREGATION :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUCCH_AGGREGATION(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );


                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );

                p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_PUCCH_AGGREGATION;
                p_PucchParam->pucchAggregation = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-16 : PDSCH parameters

            case TAG_PDSCH_MAPPING_TYPE :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDSCH_MAPPING_TYPE(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u : ", value_U8 );

                if((value_U8 & BITMASK_MAPPING_TYPE_SUPPORT_TYPE_B))
                    FAPI_Output( " Type B \n" );
                else
                    FAPI_Output( " Type A \n" );
                    
                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_MAPPING_TYPE;
                p_PdschParam->pdschMappingType = value_U8;
                break;

            case TAG_PDSCH_ALLOCATION_TYPES :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDSCH_ALLOCATION_TYPES(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u : ", value_U8 );

                if((value_U8 & BITMASK_ALLOC_TYPE_SUPPORT_TYPE_1))
                    FAPI_Output( " Type 1 \n" );
                else
                    FAPI_Output( " Type 0 \n" );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_ALLOCATION_TYPES;
                p_PdschParam->pdschAllocationTypes = value_U8;
                break;

            case TAG_PDSCH_VRB_TO_PRB_MAPPING :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDSCH_VRB_TO_PRB_MAPPING(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );

                if((value_U8 & BITMASK_VRB_PRB_MAPPING_SUPPORT_INTERLEAVED))
                    FAPI_Output( " Interleaved  \n" );
                else
                    FAPI_Output( " Non-Interleaved  \n" );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_VRB_TO_PRB_MAPPING;
                p_PdschParam->pdschVrbToPrbMapping = value_U8;
                break;

            case TAG_PDSCH_CBG :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDSCH_CBG(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u  :", value_U8 );

                
                if((value_U8 == 0))
                    FAPI_Output( " Not Supported \n" );
                if((value_U8 == 1))
                    FAPI_Output( " Supported with segmentation in L2 \n" );
                if((value_U8 == 2))
                    FAPI_Output( " Supported with segmentation in L1 \n" );
                if((value_U8 == 3))
                    FAPI_Output( " Supported with either type of segmentation \n" );  
                    
                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_CBG;
                p_PdschParam->pdschCbg = value_U8;
                break;

            case TAG_PDSCH_DMRS_CONFIG_TYPES :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_PDSCH_DMRS_CONFIG_TYPES(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );

                if((value_U8 & BITMASK_DMRS_CONFIG_SUPPORT_TYPE_2))
                    FAPI_Output( " Type 2  \n" );
                else
                    FAPI_Output( " Type 1  \n" );
                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_DMRS_CONFIG_TYPES;
                p_PdschParam->pdschDmrsConfigTypes = value_U8;
                break;

            case TAG_PDSCH_DMRS_MAX_LENGTH :
                value_U8 = p_TlvHeader->value[0];
                
                FAPI_Output( "      tag    : TAG_PDSCH_DMRS_MAX_LENGTH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_DMRS_MAX_LENGTH;
                p_PdschParam->pdschDmrsMaxLength = value_U8;
                break;

            case TAG_PDSCH_DMRS_ADDITIONAL_POS :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDSCH_DMRS_ADDITIONAL_POS(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );
                
                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_DMRS_ADDITIONAL_POS;
                p_PdschParam->pdschDmrsAdditionalPos = value_U8;
                break;

            case TAG_MAX_PDSCHS_TBS_PER_SLOT :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_MAX_PDSCHS_TBS_PER_SLOT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_PDSCHS_TBS_PER_SLOT;
                p_PdschParam->maxPdschsTBsPerSlot = value_U8;
                break;

            case TAG_MAX_NUMBER_MIMO_LAYERS_PDSCH :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_MAX_NUMBER_MIMO_LAYERS_PDSCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_PDSCH;
                p_PdschParam->maxNumberMimoLayersPdsch = value_U8;
                break;

            case TAG_SUPPORTED_MAX_MODULATION_ORDER_DL :
                value_U8 = p_TlvHeader->value[0];
                
                FAPI_Output( "      tag    : TAG_SUPPORTED_MAX_MODULATION_ORDER_DL(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u ", value_U8 );
                if(value_U8==0)
                   FAPI_Output( " QPSK \n" );
                if(value_U8==1)
                   FAPI_Output( " 16 QAM \n" );
                if(value_U8==2)
                   FAPI_Output( " 64 QAM \n" );
                else
                   FAPI_Output( " 256 QAM \n" );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_MAX_MODULATION_ORDER_DL;
                p_PdschParam->supportedMaxModulationOrderDl = value_U8;
                break;

            case TAG_MAX_MU_MIMO_USERS_DL :
                value_U8 = p_TlvHeader->value[0];
                
                FAPI_Output( "      tag    : TAG_MAX_MU_MIMO_USERS_DL(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_MU_MIMO_USERS_DL;
                p_PdschParam->maxMuMimoUsersDl = value_U8;
                break;

            case TAG_PDSCH_DATA_IN_DMRS_SYMBOLS :
                value_U8 = p_TlvHeader->value[0];
                
                FAPI_Output( "      tag    : TAG_PDSCH_DATA_IN_DMRS_SYMBOLS(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_DATA_IN_DMRS_SYMBOLS;
                p_PdschParam->pdschDataInDmrsSymbols = value_U8;
                break;

            case TAG_PREMPTION_SUPPORT :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_PREMPTION_SUPPORT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PREMPTION_SUPPORT;
                p_PdschParam->premptionSupport = value_U8;
                break;

            case TAG_PDSCH_NON_SLOT_SUPPORT :
                value_U8 = p_TlvHeader->value[0];

                
                FAPI_Output( "      tag    : TAG_PDSCH_NON_SLOT_SUPPORT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_NON_SLOT_SUPPORT;
                p_PdschParam->pdschNonSlotSupport = value_U8;
                break;

            case TAG_SSB_RATE_MATCH :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_SSB_RATE_MATCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );
                

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_SSB_RATE_MATCH;
                p_PdschParam->ssbRateMatch = value_U8;
                break;

            case TAG_SUPPORTED_RATE_MATCH_PATTERN_TYPE :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_SSB_RATE_MATCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U16 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_RATE_MATCH_PATTERN_TYPE;
                p_PdschParam->supportedRateMatchPatternType = value_U16;
                break;

            case TAG_PDCCH_RATE_MATCH :
                value_U8 = p_TlvHeader->value[0];

                
                FAPI_Output( "      tag    : TAG_PDCCH_RATE_MATCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );
                
                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDCCH_RATE_MATCH;
                p_PdschParam->pdcchRateMatch = value_U8;
                break;

            case TAG_NUM_OF_RATE_MATCH_PATTERN_LTE_CRS :
                value_U8 = p_TlvHeader->value[0];
                
                FAPI_Output( "      tag    : TAG_NUM_OF_RATE_MATCH_PATTERN_LTE_CRS(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_NUM_OF_RATE_MATCH_PATTERN_LTE_CRS;
                p_PdschParam->numOfRateMatchPatternLTECrsPerSlot = value_U8;
                break;

            case TAG_NUM_OF_RATE_MATCH_PATTER_LTE_CRS_IN_PHY :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_NUM_OF_RATE_MATCH_PATTER_LTE_CRS_IN_PHY(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_NUM_OF_RATE_MATCH_PATTER_LTE_CRS_IN_PHY;
                p_PdschParam->numOfRateMatchPatternLTECrsInPhy = value_U8;
                break;
            
            case TAG_LTE_CRS_RATEMATCH_MB_SFN_DERIVATION :
                value_U8 = p_TlvHeader->value[0];
                
                FAPI_Output( "      tag    : TAG_LTE_CRS_RATEMATCH_MB_SFN_DERIVATION(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_LTE_CRS_RATE_MATCH_MB_SFN_DERIVATION;
                p_PdschParam->lteCrsRateMatchMbsfnDerivation= value_U8;
                break;
     
            case TAG_SUPPORTED_LTE_CRS_RATEMATCH_METHOD :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_SUPPORTED_LTE_CRS_RATEMATCH_METHOD(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );
     
                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_LTE_CRS_RATE_MATCH_METHOD;
                p_PdschParam->supportedLTECrsRateMatchMethod= value_U8;
                break;      

            case TAG_CSI_RS_RATE_MATCH :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_CSI_RS_RATE_MATCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_CSI_RS_RATE_MATCH;
                p_PdschParam->csiRsRateMatch = value_U8;
                break;

            case TAG_PDSCH_TRANS_TYPE_SUPPORT :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_PDSCH_TRANS_TYPE_SUPPORT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_TRANS_TYPE_SUPPORT;
                p_PdschParam->pdschTransTypeSupport = value_U8;
                break;

            case TAG_PDSCH_MAC_PDU_BIT_ALIGNMENT :
                value_U8 = p_TlvHeader->value[0];
                
                FAPI_Output( "      tag    : TAG_PDSCH_MAC_PDU_BIT_ALIGNMENT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_MAC_PDU_BIT_ALIGNMENT;
                p_PdschParam->pdschMacPduBitAlignment = value_U8;
                break;
               
           case TAG_MAX_NUMBER_PRB_SYM_BITMAPS_PER_SLOT :
                p_PrbSymBitmapsPerSlotInfo = (PrbSymBitmapsPerSlotInfo *) p_TlvHeader->value;

                FAPI_Output( "      tag    : TAG_MAX_NUMBER_PRB_SYM_BITMAPS_PER_SLOT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : \n");

                for(numerologyIndex = 0; numerologyIndex <MAX_NUMEROLOGY;numerologyIndex++)
                {
                   FAPI_Output( "               maxNumberPrbSymBitmapsPerSlot[%u]: %u \n",numerologyIndex,p_PrbSymBitmapsPerSlotInfo->PrbSymBitmapsPerSlot[numerologyIndex]);
                }
                
                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUMBER_PRB_SYM_BITMAPS_PER_SLOT;
                p_PdschParam->maxNumberPrbSymBitmapsPerSlot = * p_PrbSymBitmapsPerSlotInfo;
                break;
                /*
           case TAG_MAX_NUMBER_CSIRS_RATEMATCH_PER_SLOT :
                value_U16= p_TlvHeader->value[0];

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUMBER_CSI_RS_RATE_MATCH_PER_SLOT;
                p_PdschParam->maxNumberCsiRsRateMatchPerSlot= value_U16;
                break;
                */
          case TAG_MAX_NUM_SSB_PER_PDSCH_SLOT :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_MAX_NUM_SSB_PER_PDSCH_SLOT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_SSB_PER_PDSCH_SLOT;
                p_PdschParam->maxNumSsbPerPdschSlot= value_U8;
                break;

          case TAG_UNIVERSAL_RATEMATCH :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_UNIVERSAL_RATEMATCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_UNIVERSAL_RATE_MATCH;
                p_PdschParam->universalRateMatch= value_U8;
                break;

          case TAG_REQUIRE_PDSCH_SIGNALING_ASSOCIATIONS :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_REQUIRE_PDSCH_SIGNALING_ASSOCIATIONS(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_REQUIRED_PDSCH_SIGNALING_ASSOCIATIONS;
                p_PdschParam->requiredPdschSignalingAssociations= value_U8;
                break;   

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-17 : PUSCH parameters

            case TAG_UCI_MUX_ULSCH_IN_PUSCH :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_UCI_MUX_ULSCH_IN_PUSCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                

                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_UCI_MUX_ULSCH_IN_PUSCH;
                p_PuschParam->uciMuxUlschInPusch = value_U8;
                break;

            case TAG_UCI_ONLY_PUSCH :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_UCI_ONLY_PUSCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                

                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );
                

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_UCI_ONLY_PUSCH;
                p_PuschParam->uciOnlyPusch = value_U8;
                break;

            case TAG_PUSCH_FREQUENCY_HOPPING :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_FREQUENCY_HOPPING(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u :", value_U8 );
                
                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );
              
                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_FREQUENCY_HOPPING;
                p_PuschParam->puschFrequencyHopping = value_U8;
                break;

            case TAG_PUSCH_DMRS_CONFIG_TYPES :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_DMRS_CONFIG_TYPES(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );
                

                if((value_U8&DMRS_CONFIG_TYPE_2))
                      FAPI_Output( " Type 2\n" );
                else 
                      FAPI_Output( " Type 1\n" );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_DMRS_CONFIG_TYPES;
                p_PuschParam->puschDmrsConfigTypes = value_U8;
                break;

            case TAG_PUSCH_DMRS_MAX_LEN :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_DMRS_MAX_LEN(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );
                
                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_DMRS_MAX_LEN;
                p_PuschParam->puschDmrsMaxLen = value_U8;
                break;

            case TAG_PUSCH_DMRS_ADDITIONAL_POS :
                value_U8 = p_TlvHeader->value[0];
                
                FAPI_Output( "      tag    : TAG_PUSCH_DMRS_ADDITIONAL_POS(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_DMRS_ADDITIONAL_POS;
                p_PuschParam->puschDmrsAdditionalPos = value_U8;
                break;

            case TAG_PUSCH_CBG :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_CBG(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );
                

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_CBG;
                p_PuschParam->puschCbg = value_U8;
                break;

            case TAG_PUSCH_MAPPING_TYPE :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_MAPPING_TYPE(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u ", value_U8 );
                if((value_U8&BITMASK_MAPPING_TYPE_SUPPORT_TYPE_B))
                      FAPI_Output( " Type B\n" );
                else 
                      FAPI_Output( " Type A\n" );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_MAPPING_TYPE;
                p_PuschParam->puschMappingType = value_U8;
                break;

            case TAG_PUSCH_ALLOCATION_TYPE :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_ALLOCATION_TYPE(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u ", value_U8 );
                if((value_U8&BITMASK_ALLOC_TYPE_SUPPORT_TYPE_1))
                      FAPI_Output( " Type 1\n" );
                else 
                      FAPI_Output( " Type 0\n" );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_ALLOCATION_TYPE;
                p_PuschParam->puschAllocationTypes = value_U8;
                break;

            case TAG_PUSCH_VRB_TO_PRB_MAPPING :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_VRB_TO_PRB_MAPPING(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u ", value_U8 );
                if((value_U8&BITMASK_VRB_PRB_MAPPING_SUPPORT_INTERLEAVED))
                      FAPI_Output( " Interleaved \n" );
                else 
                      FAPI_Output( " Non-Interleaved\n" );
            
                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_VRB_TO_PRB_MAPPING;
                p_PuschParam->puschVrbToPrbMapping = value_U8;
                break;

            case TAG_PUSCH_MAX_PTRS_PORTS :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_MAX_PTRS_PORTS(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );
             

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_MAX_PTRS_PORTS;
                p_PuschParam->puschMaxPtrsPorts = value_U8;
                break;

            case TAG_MAX_PDUSCHS_TBS_PER_SLOT :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_MAX_PDUSCHS_TBS_PER_SLOT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_PDUSCHS_TBS_PER_SLOT;
                p_PuschParam->maxPduschsTBsPerSlot = value_U8;
                break;

            case TAG_MAX_NUMBER_MIMO_LAYERS_NON_CB_PUSCH :
                value_U8 = p_TlvHeader->value[0];

                FAPI_Output( "      tag    : TAG_MAX_NUMBER_MIMO_LAYERS_NON_CB_PUSCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_NON_CB_PUSCH;
                p_PuschParam->maxNumberMimoLayersNonCbPusch = value_U8;
                break;

            case TAG_MAX_NUMBER_MIMO_LAYERS_CB_PUSCH :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_MAX_NUMBER_MIMO_LAYERS_CB_PUSCH(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_CB_PUSCH;
                p_PuschParam->maxNumberMimoLayersCbPusch = value_U8;
                break;

            case TAG_SUPPORTED_MAX_MODULATION_ORDER_UL :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_SUPPORTED_MAX_MODULATION_ORDER_UL(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u ", value_U8 );
                if(value_U8==0)
                   FAPI_Output( " QPSK \n" );
                if(value_U8==1)
                   FAPI_Output( " 16 QAM \n" );
                if(value_U8==2)
                   FAPI_Output( " 64 QAM \n" );
                else
                   FAPI_Output( " 256 QAM \n" );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_MAX_MODULATION_ORDER_UL;
                p_PuschParam->supportedMaxModulationOrderUl = value_U8;
                break;

            case TAG_MAX_MU_MIMO_USERS_UL :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_MAX_MU_MIMO_USERS_UL(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_MU_MIMO_USERS_UL;
                p_PuschParam->maxMuMimoUsersUl = value_U8;
                break;

            case TAG_DFTS_OFDM_SUPPORT :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_DFTS_OFDM_SUPPORT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u ", value_U8 );

                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );
                
                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_DFTS_OFDM_SUPPORT;
                p_PuschParam->dftsOfdmSupport = value_U8;
                break;

            case TAG_PUSCH_AGGREGATION_FACTOR :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_AGGREGATION_FACTOR(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_AGGREGATION_FACTOR;
                p_PuschParam->puschAggregationFactor = value_U8;
                break;

            case TAG_PUSCH_LBRM_SUPPORT :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_LBRM_SUPPORT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                if((value_U8&FEATURE_SUPPORTED))
                      FAPI_Output( " Supported\n" );
                else 
                      FAPI_Output( " Not Supported\n" );
                

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_LBRM_SUPPORT;
                p_PuschParam->puschLbrmSupport = value_U8;
                break;

            case TAG_PUSCH_TRANS_TYPE_SUPPORT :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_TRANS_TYPE_SUPPORT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_TRANS_TYPE_SUPPORT;
                p_PuschParam->puschTransTypeSupport = value_U8;
                break;

            case TAG_PUSCH_MAC_PDU_BIT_ALIGNMENT :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_MAC_PDU_BIT_ALIGNMENT(0x04%X)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  : %u \n", value_U8 );

                p_PuschParam->tagBitmask |= PARAM_TAG_BITMASK_PUSCH_MAC_PDU_BIT_ALIGNMENT;
                p_PuschParam->puschMacPduBitAlignment = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-18 : PRACH parameters 

            case TAG_PRACH_LONG_FORMATS :
                value_U8 = p_TlvHeader->value[0];

                p_PrachParam->tagBitmask |= PARAM_TAG_BITMASK_PRACH_LONG_FORMATS;
                p_PrachParam->prachLongFormats = value_U8;
                break;

            case TAG_PRACH_SHORT_FORMATS :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;

                p_PrachParam->tagBitmask |= PARAM_TAG_BITMASK_PRACH_SHORT_FORMATS;
                p_PrachParam->prachShortFormats = value_U16;
                break;

            case TAG_PRACH_RESTRICTED_SETS :
                value_U8 = p_TlvHeader->value[0];

                p_PrachParam->tagBitmask |= PARAM_TAG_BITMASK_PRACH_RESTRICTED_SETS;
                p_PrachParam->prachRestrictedSets = value_U8;
                break;

            case TAG_MAX_PRACH_FD_OCCASIONS_IN_A_SLOT :
                value_U8 = p_TlvHeader->value[0];

                p_PrachParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_PRACH_FD_OCCASIONS_IN_A_SLOT;
                p_PrachParam->maxPrachFdOccasionsInASlot = value_U8;
                break;

            case TAG_MAX_NUM_PRACH_CONFIGURATIONS :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;

                p_PrachParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_PRACH_CONFIGURATIONS;
                p_PrachParam->maxNumPrachConfigurations = value_U16;
                break;

            case TAG_PRACH_MULTIPLE_CARRIERS_IN_A_BAND :
                value_U8 = p_TlvHeader->value[0];

                p_PrachParam->tagBitmask |= PARAM_TAG_BITMASK_PRACH_MULTIPLE_CARRIERS_IN_A_BAND;
                p_PrachParam->prachMultipleCarriersInABand = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-19 : Measurement parameters 

            case TAG_RSSI_MEASUREMENT_SUPPORT :
                value_U8 = p_TlvHeader->value[0];

                p_MeasurementParam->tagBitmask |= PARAM_TAG_BITMASK_RSSI_MEASUREMENT_SUPPORT;
                p_MeasurementParam->rssiMeasurementSupport = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-20 : UCI parameters

            case TAG_MAX_NUM_UCI_MAPS :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;

                p_UciParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_UCI_MAPS;
                p_UciParam->maxNumUciMaps = value_U16;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-21 : Capability validity scope parameters 

            case TAG_CAPABILITIES :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                numCapabilities = *p_Value_U16;

                if (numCapabilities > MAX_CAPABILITY_VALIDITY_ENTRY_COUNT)
                {
                    FAPI_Error( "AddTag: Too large numCapabilities %u : limit to %u\n",
                        numCapabilities, MAX_CAPABILITY_VALIDITY_ENTRY_COUNT );
                    numCapabilities = MAX_CAPABILITY_VALIDITY_ENTRY_COUNT;
                }

                p_CapabilityValidity->tagBitmask |= PARAM_TAG_BITMASK_CAPABILITIES;
                p_CapabilityValidity->numCapabilities = numCapabilities;

                p_CapabilityValidityEntry = (CapabilityValidityEntry *) (p_Value_U16+1);
                for (capabilityValidityEntryIndex = 0; capabilityValidityEntryIndex < numCapabilities; capabilityValidityEntryIndex++)
                {
                    p_CapabilityValidity->capabilityValidityEntryList[capabilityValidityEntryIndex] = *p_CapabilityValidityEntry;
                    p_CapabilityValidityEntry++;
                }
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-22 : PHY Support 

            case TAG_SUPPORTED_PHY_PROFILES : 
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                numPhyProfiles = *p_Value_U16;

                p_PhySupport->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_PHY_PROFILES;
                p_PhySupport->numPhyProfiles = numPhyProfiles;
                p_Value_U16++;

                p_PhySupport->numDlBbPorts = *p_Value_U16;
                p_Value_U16++;

                p_PhySupport->numUlBbPorts = *p_Value_U16;
                p_Value_U16++;

                for (phyProfileIndex = 0; phyProfileIndex < p_PhySupport->numPhyProfiles; phyProfileIndex++)
                {
                    p_Value_U8 = (uint8_t *) p_Value_U16;
                    p_PhySupport->maxNumPhys[phyProfileIndex] = *p_Value_U8;
                    p_Value_U8++;

                    p_Value_U16 = (uint16_t *) p_Value_U8;
                    for (numPhyIndex = 0; numPhyIndex < p_PhySupport->maxNumPhys[phyProfileIndex]; numPhyIndex++)
                    {
                        p_SupportedPhyProfileInfo = &(p_PhySupport->supportedPhyProfileInfo[phyProfileIndex][numPhyIndex]);

                        p_Value_U8 = (uint8_t *) p_Value_U16;
                        p_SupportedPhyProfileInfo->numDlPortRanges = *p_Value_U8;
                        p_Value_U8++;

                        p_Value_U16 = (uint16_t *) p_Value_U8;
                        for (portRangeIndex = 0; portRangeIndex < p_SupportedPhyProfileInfo->numDlPortRanges; portRangeIndex++)
                        {
                            p_SupportedPhyProfileInfo->dlPortRangeStart[portRangeIndex] = *p_Value_U16;
                            p_Value_U16++;
                        }

                        for (portRangeIndex = 0; portRangeIndex < p_SupportedPhyProfileInfo->numDlPortRanges; portRangeIndex++)
                        {
                            p_SupportedPhyProfileInfo->dlPortRangeLen[portRangeIndex] = *p_Value_U16;
                            p_Value_U16++;
                        }
 
                        p_Value_U8 = (uint8_t *) p_Value_U16;
                        p_SupportedPhyProfileInfo->numUlPortRanges = *p_Value_U8;
                        p_Value_U8++;

                        p_Value_U16 = (uint16_t *) p_Value_U8;
                        for (portRangeIndex = 0; portRangeIndex < p_SupportedPhyProfileInfo->numUlPortRanges; portRangeIndex++)
                        {
                            p_SupportedPhyProfileInfo->ulPortRangeStart[portRangeIndex] = *p_Value_U16;
                            p_Value_U16++;
                        }
                        for (portRangeIndex = 0; portRangeIndex < p_SupportedPhyProfileInfo->numUlPortRanges; portRangeIndex++)
                        {
                            p_SupportedPhyProfileInfo->ulPortRangeLen[portRangeIndex] = *p_Value_U16;
                            p_Value_U16++;
                        }
                    }
                }
                break;

            case TAG_TIME_MANAGEMENT :
                value_U8 = p_TlvHeader->value[0];

                p_PhySupport->tagBitmask |= PARAM_TAG_BITMASK_TIME_MANAGEMENT;
                p_PhySupport->timeManagement = value_U8;
                break;

            case TAG_FAPI_PROTOCOL_VERSION :
                p_FapiProtocolVersion = (FAPIProtocolVersion *) p_TlvHeader->value;

                p_PhySupport->tagBitmask |= PARAM_TAG_BITMASK_FAPI_PROTOCOL_VERSION;
                p_PhySupport->fapiProtocolVersion = *p_FapiProtocolVersion;
                break;

            case TAG_MORE_THAN_ONE_INDICATION_PER_SLOT :
                p_MoreThanOneIndicationPerSlot = (IndicationInstancePerSlotInfo *) p_TlvHeader->value;

                p_PhySupport->tagBitmask |= PARAM_TAG_BITMASK_MORE_THAN_ONE_INDICATION_PER_SLOT;
                p_PhySupport->moreThanOneIndicationPerSlot = *p_MoreThanOneIndicationPerSlot;
                break;

            case TAG_MORE_THAN_ONE_REQUEST_PER_SLOT :
                p_MoreThanOneRequestPerSlot = (RequestInstancePerSlotInfo *) p_TlvHeader->value;

                p_PhySupport->tagBitmask |= PARAM_TAG_BITMASK_MORE_THAN_ONE_REQUEST_PER_SLOT;
                p_PhySupport->moreThanOneRequestPerSlot = *p_MoreThanOneRequestPerSlot;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-23 : PHY / DFE Profile Validity Map

            case TAG_PAIRING_DFE_PROFILES :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                numPhyProfiles = *p_Value_U16;

                p_ParingDfeProfileParam->tagBitmask |= PARAM_TAG_BITMASK_PAIRING_DFE_PROFILES;
                p_ParingDfeProfileParam->numPhyProfiles = numPhyProfiles;
                p_Value_U16++;

                p_ParingDfeProfileParam->numDfeProfiles = *p_Value_U16;
                p_Value_U16++;

                profileValidityMapSize = (numPhyProfiles * p_ParingDfeProfileParam->numDfeProfiles + 7) / 8;
                if (profileValidityMapSize > MAX_PROFILE_VALIDITY_MAP_SIZE)
                {
                    FAPI_Error( "ParseTag: Too large profileValidityMapSize %u : limit to %u\n", 
                        profileValidityMapSize, MAX_PROFILE_VALIDITY_MAP_SIZE );
                    profileValidityMapSize = MAX_PROFILE_VALIDITY_MAP_SIZE;
                }

                p_Value_U8 = (uint8_t *) p_Value_U16;
                memcpy( p_ParingDfeProfileParam->profileValidityMap, p_Value_U8, profileValidityMapSize );
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-24 : Delay management parameters 

            case TAG_DL_TTI_TIMING_OFFSET :
                p_Value_U32 = (uint32_t *) p_TlvHeader->value;
                value_U32 = *p_Value_U32;

                p_DelayManagement->tagBitmask |= PARAM_TAG_BITMASK_DL_TTI_TIMING_OFFSET;
                p_DelayManagement->dlTTI_TimingOffset = value_U32;
                break;

            case TAG_UL_TTI_TIMING_OFFSET :
                p_Value_U32 = (uint32_t *) p_TlvHeader->value;
                value_U32 = *p_Value_U32;

                p_DelayManagement->tagBitmask |= PARAM_TAG_BITMASK_UL_TTI_TIMING_OFFSET;
                p_DelayManagement->ulTTI_TimingOffset = value_U32;
                break;

            case TAG_UL_DCI_TIMING_OFFSET :
                p_Value_U32 = (uint32_t *) p_TlvHeader->value;
                value_U32 = *p_Value_U32;

                p_DelayManagement->tagBitmask |= PARAM_TAG_BITMASK_UL_DCI_TIMING_OFFSET;
                p_DelayManagement->ulDCI_TimingOffset = value_U32;
                break;

            case TAG_TX_DATA_TIMING_OFFSET :
                p_Value_U32 = (uint32_t *) p_TlvHeader->value;
                value_U32 = *p_Value_U32;

                p_DelayManagement->tagBitmask |= PARAM_TAG_BITMASK_TX_DATA_TIMING_OFFSET;
                p_DelayManagement->txData_TimingOffset = value_U32;
                break;

            case TAG_TIMING_WINDOW :
                value_U8 = p_TlvHeader->value[0];

                p_DelayManagement->tagBitmask |= PARAM_TAG_BITMASK_TIMING_WINDOW;
                p_DelayManagement->timingWindow = value_U8;
                break;

            case TAG_TIMING_INFO_PERIOD :
                value_U8 = p_TlvHeader->value[0];

                p_DelayManagement->tagBitmask |= PARAM_TAG_BITMASK_TIMING_INFO_PERIOD;
                p_DelayManagement->timingInfoPeriod = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-25 : Rel-16 mTRP parameters

            case TAG_MTRP_SUPPORT :
                p_Value_U32 = (uint32_t *) p_TlvHeader->value;
                value_U32 = *p_Value_U32;

                p_Rel16_mTRP->tagBitmask |= PARAM_TAG_BITMASK_MTRP_SUPPORT;
                p_Rel16_mTRP->mTRP_Support = value_U32;
                break;

            default :
                FAPI_Output( "      tag %u is not supported\n", p_TlvHeader->tag );
                break;
        }
        p_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);
    }
}

void mac_ParseMsg_ConfigResp( Fapi_Generic_Header *p_FapiGenericHeader )
{
    CONFIG_RESP *p_ConfigResp;

    FAPI_Output( "    type      : MSG_PHY_CONFIG_RESP(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length    : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_ConfigResp = (CONFIG_RESP *) (p_FapiGenericHeader+1);

    if (p_ConfigResp->errorCode == MSG_OK)
    {
        FAPI_Output( "    errorCode : MSG_OK(%u)\n", p_ConfigResp->errorCode );
    }
    else
    {
        FAPI_Output( "    errorCode : %u\n", p_ConfigResp->errorCode );
    }

    FAPI_Output( "    numInvalidOrUnsupportedTLV  : %u\n", p_ConfigResp->numInvalidOrUnsupportedTLV );
    FAPI_Output( "    numInvalidTLVForIdleOnly    : %u\n", p_ConfigResp->numInvalidTLVForIdleOnly );
    FAPI_Output( "    numInvalidTLVForRunningOnly : %u\n", p_ConfigResp->numInvalidTLVForRunningOnly );
    FAPI_Output( "    numMissingTLV               : %u\n", p_ConfigResp->numMissingTLV );
}

void mac_ParseMsg_RxDataInd( Fapi_Generic_Header *p_FapiGenericHeader, uint8_t *p_RxBuffList[], uint32_t *p_rxBuffListCount )
{
    uint32_t pduIndex, pduCount, tbDataIndex;
    Rx_Data_IND *p_RxDataInd;
    Rx_Data_IND_PDU *p_RxDataIndPdu;
    uint64_t *p_Value_U64;
    uint8_t *pRxBuff;

    *p_rxBuffListCount = 0;

    FAPI_Output( "    type            : MSG_PHY_RX_DATA_IND(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_RxDataInd = (Rx_Data_IND *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_RxDataInd->sfn );
    FAPI_Output( "    slot            : %u\n", p_RxDataInd->slot );
    FAPI_Output( "    numPDUs         : %u\n", p_RxDataInd->numPDUs );

    p_RxDataIndPdu = (Rx_Data_IND_PDU *) p_RxDataInd->variablePart;
    pduCount = p_RxDataInd->numPDUs;
    for (pduIndex = 0; pduIndex < pduCount; pduIndex++)
    {
        FAPI_Output( "    ForEach numPDUs[%u] :\n", pduIndex );
        FAPI_Output( "      handle        : %u\n", p_RxDataIndPdu->handle );
        FAPI_Output( "      rnti          : %u\n", p_RxDataIndPdu->rnti );
        FAPI_Output( "      rapId         : %u\n", p_RxDataIndPdu->rapId );
        FAPI_Output( "      harqID        : %u\n", p_RxDataIndPdu->harqID );
        FAPI_Output( "      pduLength     : %u\n", p_RxDataIndPdu->pduLength );
        FAPI_Output( "      pduTag        : %u\n", p_RxDataIndPdu->pduTag );
        if (p_RxDataIndPdu->pduTag == RX_DATA_VALUE_FIELD_POINTER_TO_PAYLOAD)
        {
            p_Value_U64 = (uint64_t *) p_RxDataIndPdu->variablePart;
            pRxBuff = (uint8_t *) *p_Value_U64;

            FAPI_Output( "      pduValue      :" );
            for (tbDataIndex = 0; tbDataIndex < p_RxDataIndPdu->pduLength; tbDataIndex++)
                FAPI_Output( " %02x", pRxBuff[tbDataIndex] );
            FAPI_Output( "\n" );

            if (*p_rxBuffListCount < MAX_RX_DATA_COUNT_IN_SLOT)
            {
                p_RxBuffList[*p_rxBuffListCount] = pRxBuff;
                (*p_rxBuffListCount)++;
            }
            else 
            {
                FAPI_Output( "      Err : Too many pduValue\n" );
            }

            p_RxDataIndPdu = (Rx_Data_IND_PDU *) ((uint8_t *)(p_RxDataIndPdu+1) + sizeof(uint64_t));
        }
        else
        {
            FAPI_Output( "      pduTag %u is not supported\n", p_RxDataIndPdu->pduTag );
            return;
        }
    }
}

void mac_ParseMsg_CrcInd( Fapi_Generic_Header *p_FapiGenericHeader )
{
    uint32_t pduIndex, pduCount, cbCrcStatusIndex;
    CRC_IND *p_CrcInd;
    CRC_IND_PDU *p_CrcIndPdu;

    FAPI_Output( "    type            : MSG_PHY_CRC_IND(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_CrcInd = (CRC_IND *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_CrcInd->sfn );
    FAPI_Output( "    slot            : %u\n", p_CrcInd->slot );
    FAPI_Output( "    numCRCs         : %u\n", p_CrcInd->numCRCs );

    p_CrcIndPdu = (CRC_IND_PDU *) p_CrcInd->variablePart;
    pduCount = p_CrcInd->numCRCs;
    for (pduIndex = 0; pduIndex < pduCount; pduIndex++)
    {
        FAPI_Output( "    ForEach numCRCs[%u] :\n", pduIndex );
        FAPI_Output( "      handle        : %u\n", p_CrcIndPdu->handle );
        FAPI_Output( "      rnti          : %u\n", p_CrcIndPdu->rnti );
        FAPI_Output( "      rapId         : %u\n", p_CrcIndPdu->rapId );
        FAPI_Output( "      harqID        : %u\n", p_CrcIndPdu->harqID );
        FAPI_Output( "      tbCrcStatus   : %u\n", p_CrcIndPdu->tbCrcStatus );
        FAPI_Output( "      numCb         : %u\n", p_CrcIndPdu->numCb );
        FAPI_Output( "      cbCrcStatus   :" );
        for (cbCrcStatusIndex = 0; cbCrcStatusIndex < MAX_NUM_CB_CRC_STATUS; cbCrcStatusIndex++)
            FAPI_Output( " 0x%02x", p_CrcIndPdu->cbCrcStatus[cbCrcStatusIndex] );
        FAPI_Output( "\n" );
        FAPI_Output( "      ulSnrMetric   : %u\n", p_CrcIndPdu->ulSnrMetric );
        FAPI_Output( "      timingAdvanceOffset           : %u\n", p_CrcIndPdu->timingAdvanceOffset );
        FAPI_Output( "      timingAdvanceOffsetNanoSecond : %d\n", p_CrcIndPdu->timingAdvanceOffsetNanoSecond );
        FAPI_Output( "      rssi          : %u\n", p_CrcIndPdu->rssi );
        FAPI_Output( "      rsrp          : %u\n", p_CrcIndPdu->rsrp );
        p_CrcIndPdu++;
    }
}

void mac_ParseMsg_UciInd( Fapi_Generic_Header *p_FapiGenericHeader )
{
    UCI_IND *p_UciInd;

    FAPI_Output( "    type            : MSG_PHY_UCI_IND(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_UciInd = (UCI_IND *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_UciInd->sfn );
    FAPI_Output( "    slot            : %u\n", p_UciInd->slot );
    FAPI_Output( "    numUCIs         : %u\n", p_UciInd->numUCIs );
}

void mac_ParseMsg_RachInd( Fapi_Generic_Header *p_FapiGenericHeader )
{
    RACH_IND *p_RachInd;

    FAPI_Output( "    type            : MSG_PHY_RACH_IND(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_RachInd = (RACH_IND *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_RachInd->sfn );
    FAPI_Output( "    slot            : %u\n", p_RachInd->slot );
    FAPI_Output( "    numPDUs         : %u\n", p_RachInd->numPDUs );
}

void mac_ParseMsg_SrsInd( Fapi_Generic_Header *p_FapiGenericHeader )
{
    SRS_IND *p_SrsInd;

    FAPI_Output( "    type            : MSG_PHY_SRS_IND(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_SrsInd = (SRS_IND *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_SrsInd->sfn );
    FAPI_Output( "    slot            : %u\n", p_SrsInd->slot );
    FAPI_Output( "    numPDUs         : %u\n", p_SrsInd->numPDUs );
}

void mac_ParseMsg( uint8_t *p_MsgBuff, uint32_t msgLength )
{
    uint32_t fapiMsgCount, fapiMsgIndex, msgOffset, rxBuffListCount;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;
    uint8_t *p_RxBuffList[MAX_RX_DATA_COUNT_IN_SLOT];

    FAPI_Output( "Parse Msg : \n" );

    p_FapiApiHeader = (Fapi_API_Header *) p_MsgBuff;
    fapiMsgCount = p_FapiApiHeader->numOfMessages;
    msgOffset = sizeof(Fapi_API_Header);

    if (msgOffset > msgLength) 
    {
        FAPI_Output( "  Size error : invalid Fapi_API_Header\n" );
        return;
    }

    FAPI_Output( "  numOfMessages : %u\n", fapiMsgCount );
    FAPI_Output( "  opaqueHandle  : %u\n", p_FapiApiHeader->opaqueHandle );
 
    for (fapiMsgIndex = 0; fapiMsgIndex < fapiMsgCount; fapiMsgIndex++)  
    {
        p_FapiGenericHeader = (Fapi_Generic_Header *) (p_MsgBuff+msgOffset);

        if (msgOffset+sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length > msgLength) 
        {
            FAPI_Output( "  Size error : invalid Fapi_Generic_Header\n" );
            return;
        }

        FAPI_Output( "  FAPI Msg %u/%u  :\n", fapiMsgIndex+1, fapiMsgCount );

        switch (p_FapiGenericHeader->type) 
        {
            case MSG_PHY_PARAM_RESP :
                mac_ParseMsg_ParamResp( p_FapiGenericHeader, &g_Mac_FapiParamRecvInfo );
                break; 

            case MSG_PHY_CONFIG_RESP :
                mac_ParseMsg_ConfigResp( p_FapiGenericHeader );
                break; 

            case MSG_PHY_RX_DATA_IND :
                mac_ParseMsg_RxDataInd( p_FapiGenericHeader, p_RxBuffList, &rxBuffListCount );
                break; 

            case MSG_PHY_CRC_IND :
                mac_ParseMsg_CrcInd( p_FapiGenericHeader );
                break; 

            case MSG_PHY_UCI_IND :
                mac_ParseMsg_UciInd( p_FapiGenericHeader );
                break; 

            case MSG_PHY_RACH_IND :
                mac_ParseMsg_RachInd( p_FapiGenericHeader );
                break; 

            case MSG_PHY_SRS_IND :
                mac_ParseMsg_SrsInd( p_FapiGenericHeader );
                break; 

            default :
                FAPI_Output( "    type %u is not supported\n", p_FapiGenericHeader->type );
                break;
        }

        msgOffset += sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length;
    }
    FAPI_Output( "\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////
// Parsing routiunes for PHY

void phy_ParseMsg_ParamReq( Fapi_Generic_Header *p_FapiGenericHeader )
{
    TLV_Header *p_TlvHeader;

    FAPI_Output( "    type   : MSG_PHY_PARAM_REQ(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    FAPI_Output( "    TLV    :\n" );
    p_TlvHeader = (TLV_Header *) (p_FapiGenericHeader+1);

    switch (p_TlvHeader->tag)
    {
        case TAG_PROTOCOL_VERSION :
            FAPI_Output( "      tag    : TAG_PROTOCOL_VERSION(0x%x)\n", p_TlvHeader->tag );
            FAPI_Output( "      length : %u\n", p_TlvHeader->length );
            FAPI_Output( "      value  : " );

            if (p_TlvHeader->value[0] == FAPI_VERSION_V3)
                FAPI_Output( "FAPI_VERSION_V3(%u)\n", p_TlvHeader->value[0] );
            else
                FAPI_Output( "%u : not supported\n", p_TlvHeader->value[0] );
            break;

        default :
            FAPI_Output( "      tag %u is not supported\n", p_TlvHeader->tag );
            break;
    }
}

void phy_ParseMsg_ConfigReq( Fapi_Generic_Header *p_FapiGenericHeader, FapiConfigInfo *p_FapiConfigInfo )
{
    uint32_t *p_Value_U32, value_U32, tlvIndex, tlvCount, tlvValueLength, tlvLength, slotIndex, symbolIndex;
    uint16_t *p_Value_U16, value_U16, numerologyIndex, prachFdOccasionIndex, numPrachFdOccasions, ulBwpIndex;
    uint16_t unusedRootSequenceIndex, unusedRootSequenceCount, numPrachConfigurations, multiPrachConfigInfoIndex;
    uint16_t numUci2Maps, uci2MapsIndex, part1ParamsIndex, sumOfSizesPart1Params, mapSize, mapIndex, multiSsbResourceConfigIndex;
    uint16_t prbSymbRateMatchPatternInfoIndex, lteCrsRateMatchPatternsIndex, mbsfnSubframeConfigIndex;
    uint16_t numAssociationPatternPeriod, associationPatternPeriodIndex; 
    uint16_t numberOfPrbSymbRateMatchPatterns, numberOfCoresetRateMatchPatterns, coresetRateMatchPatternInfoIndex;
    uint8_t *p_Value_U8, value_U8, numPart1Params, sizesPart1ParamsValue, numSsbConfigurations, beamIdx;
    uint8_t lteCrsRateMatchPatternsCount, mbsfnSubframeConfigCount, freqDomainRBIndex;
    CONFIG_REQ *p_ConfigReq;
    IndicationInstancePerSlotInfo *p_IndicationInstancePerSlotInfo;
    RequestInstancePerSlotInfo *p_RequestInstancePerSlotInfo;
    SlotTimeDomainInfo *p_SlotTimeDomainInfo;
    CarrierConfigInfo *p_CarrierConfigInfo;
    PRACHConfigInfo *p_PrachConfigInfo;
    PRACHOccasionInfo *p_PrachOccasionInfo;
    MSGA_PUSCH_ConfigInfo *p_MsgAPuschConfigInfo;
    AssociationPatternPeriodInfo *p_AssociationPatternPeriodInfo;
    SSBResourceConfigInfo *p_SsbResourceConfigInfo;
    TDDPatternInfo *p_TddPatternInfo;
    LTECrsRateMatchPatternsInfo *p_LteCrsRateMatchPatternInfo;
    MbsfnSubframeConfigInfo *p_MbsfnSubframeConfigInfo;
    PUCCH_SemiStatic_ConfigInfo *p_PucchSemiStaticConfigInfo;
    TLV_Header *p_TlvHeader, *p_SubTlvHeader;
    PHY_Config *p_PhyConfig;
    Carrier_Config *p_CarrierConfig;
    Cell_Config *p_CellConfig;
    SsbPowerPBCH_Config *p_SsbPowerPbchConfig;
    PRACH_Config *p_PrachConfig, *p_PrachConfigInMulti;
    MultiPRACH_Config *p_MultiPrachConfig;
    MSGA_PUSCH_Config *p_MsgAPuschConfig;
    SSBResource_Config *p_SsbResourceConfig, *p_SsbResourceConfigInMulti;
    MultiSSBResource_Config *p_MultiSsbResourceConfig;
    TDD_Config *p_TddConfig;
    Measurement_Config *p_MeasurementConfig;
    UCI_Config *p_UciConfig;
    PRBSymRateMatchPatterns_Config *p_PrbSymbRateMatchPatternsConfig;
    PRBSymRateMatchPatternInfo *p_PrbSymbRateMatchPatternInfo;
    CoresetRateMatchPatterns_Config *p_CoresetRateMatchPatternsConfig;
    CoresetRateMatchPatternInfo *p_CoresetRateMatchPatternInfo;
    LTECrsRateMatchPatterns_Config *p_LteCrsRateMatchPatternsConfig;
    PUCCH_SemiStatic_Config *p_PucchSemiStaticConfig;
    PDSCH_SemiStatic_Config *p_PdschSemiStaticConfig;
    DelayManagement_Config *p_DelayManagementConfig;
    Rel16_mTRP_Config *p_Rel16mTRPConfig;

    p_PhyConfig = &p_FapiConfigInfo->phyConfig;
    p_CarrierConfig = &p_FapiConfigInfo->carrierConfig;
    p_CellConfig = &p_FapiConfigInfo->cellConfig;
    p_SsbPowerPbchConfig = &p_FapiConfigInfo->ssbPowerPbchConfig;
    p_PrachConfig = &p_FapiConfigInfo->prachConfig;
    p_MultiPrachConfig = &p_FapiConfigInfo->multiPrachConfig;
   
    p_MsgAPuschConfig =&p_FapiConfigInfo->msgApuschConfig;
    p_SsbResourceConfig = &p_FapiConfigInfo->ssbResourceConfig;
    p_MultiSsbResourceConfig = &p_FapiConfigInfo->multiSsbResourceConfig;
    p_TddConfig = &p_FapiConfigInfo->tddConfig;
    p_MeasurementConfig = &p_FapiConfigInfo->measurementConfig;
    p_UciConfig = &p_FapiConfigInfo->uciConfig;
    p_PrbSymbRateMatchPatternsConfig = &p_FapiConfigInfo->prbSymbRateMatchPatternsConfig;
    p_CoresetRateMatchPatternsConfig = &p_FapiConfigInfo->coresetRateMatchPatternsConfig;
    p_LteCrsRateMatchPatternsConfig = &p_FapiConfigInfo->lteCrsRateMatchPatternsConfig;
    p_PucchSemiStaticConfig = &p_FapiConfigInfo->pucchSemiStaticConfig;
    p_PdschSemiStaticConfig = &p_FapiConfigInfo->pdschSemiStaticConfig;
    p_DelayManagementConfig = &p_FapiConfigInfo->delayManagementConfig;
    p_Rel16mTRPConfig = &p_FapiConfigInfo->rel16mTRPConfig;

    FAPI_Output( "    type      : MSG_PHY_CONFIG_REQ(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length    : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_ConfigReq = (CONFIG_REQ *) (p_FapiGenericHeader+1);

    tlvCount = p_ConfigReq->numOfTLV;
    FAPI_Output( "    numOfTLV  : %u\n", tlvCount );
    p_TlvHeader = (TLV_Header *) p_ConfigReq->tlvs;

    for (tlvIndex = 0; tlvIndex < tlvCount; tlvIndex++)
    {
        FAPI_Output( "    TLV %u/%u : \n", tlvIndex, tlvCount );

        tlvValueLength = p_TlvHeader->length;
        tlvLength = sizeof(TLV_Header)+tlvValueLength;

        switch (p_TlvHeader->tag)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-36 : PHY configuration

            case TAG_PHY_PROFILE_ID :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;

                FAPI_Output( "      tag    : TAG_PHY_PROFILE_ID(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        phyProfileId   : %u\n",p_PhyConfig->phyProfileId);

                p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_PHY_PROFILE_ID;
                p_PhyConfig->phyProfileId = value_U16;
                break;

            case TAG_INDICATION_INSTANCES_PER_SLOT :
                p_IndicationInstancePerSlotInfo = (IndicationInstancePerSlotInfo *) p_TlvHeader->value;
                FAPI_Output( "      tag    : TAG_INDICATION_INSTANCES_PER_SLOT(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                
                FAPI_Output( "        indicationInstancePerSlot   : %u\n",p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[0]);
                FAPI_Output( "        indicationInstancePerSlot   : %u\n",p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[1]);
                FAPI_Output( "        indicationInstancePerSlot   : %u\n",p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[2]);
                FAPI_Output( "        indicationInstancePerSlot   : %u\n",p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[3]);
                FAPI_Output( "        indicationInstancePerSlot   : %u\n",p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[4]);
                FAPI_Output( "        indicationInstancePerSlot   : %u\n",p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[5]);

                p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_INDICATION_INSTANCES_PER_SLOT;
                p_PhyConfig->indicationInstancesPerSlot = *p_IndicationInstancePerSlotInfo;
                break;

            case TAG_REQUEST_INSTANCES_PER_SLOT :
                p_RequestInstancePerSlotInfo = (RequestInstancePerSlotInfo *) p_TlvHeader->value;
                FAPI_Output( "      tag    : TAG_REQUEST_INSTANCES_PER_SLOT(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        requestInstancesPerSlot   : %u\n",p_RequestInstancePerSlotInfo->requestInstancesPerSlot[0]);
                FAPI_Output( "        requestInstancesPerSlot   : %u\n",p_RequestInstancePerSlotInfo->requestInstancesPerSlot[1]);
                FAPI_Output( "        requestInstancesPerSlot   : %u\n",p_RequestInstancePerSlotInfo->requestInstancesPerSlot[2]);
                FAPI_Output( "        requestInstancesPerSlot   : %u\n",p_RequestInstancePerSlotInfo->requestInstancesPerSlot[3]);

                p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_REQUEST_INSTANCES_PER_SLOT;
                p_PhyConfig->requestInstancesPerSlot = *p_RequestInstancePerSlotInfo;
                break;

            case TAG_SLOT_TIME_DOMAIN_INFO :
                p_SlotTimeDomainInfo = (SlotTimeDomainInfo *) p_TlvHeader->value;
                FAPI_Output( "      tag    : TAG_SLOT_TIME_DOMAIN_INFO(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        sfn                       : %u\n",p_SlotTimeDomainInfo->sfn);
                FAPI_Output( "        slot                      : %u\n",p_SlotTimeDomainInfo->slot);
                FAPI_Output( "        subcarrierSpacing         : %u\n",p_SlotTimeDomainInfo->subcarrierSpacing);
                FAPI_Output( "        cyclicPrefixType          : %u\n",p_SlotTimeDomainInfo->cyclicPrefixType);

                p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_SLOT_TIME_DOMAIN_INFO;
                p_PhyConfig->slotTimeDomainInfo = *p_SlotTimeDomainInfo;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-37 : Carrier configuration

            case TAG_CARRIER_CONFIGURATION :
                p_CarrierConfigInfo = (CarrierConfigInfo *) p_TlvHeader->value;
                
                numerologyIndex = 1;    // 1: 30Khz
                FAPI_Output( "      tag    : TAG_CARRIER_CONFIGURATION(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        dlBandwidth          : %u\n", p_CarrierConfigInfo->dlBandwidth );
                FAPI_Output( "        dlFrequency          : %u\n", p_CarrierConfigInfo->dlFrequency );
                FAPI_Output( "        dlk0[%u]              : %u\n", numerologyIndex, p_CarrierConfigInfo->dlk0[numerologyIndex] );
                FAPI_Output( "        dlGridSize[%u]        : %u\n", numerologyIndex, p_CarrierConfigInfo->dlGridSize[numerologyIndex] );
                FAPI_Output( "        numTxAnt             : %u\n", p_CarrierConfigInfo->numTxAnt );
                FAPI_Output( "        uplinkBandwidth      : %u\n", p_CarrierConfigInfo->uplinkBandwidth );
                FAPI_Output( "        uplinkFrequency      : %u\n", p_CarrierConfigInfo->uplinkFrequency );
                FAPI_Output( "        ulk0[%u]              : %u\n", numerologyIndex, p_CarrierConfigInfo->ulk0[numerologyIndex] );
                FAPI_Output( "        ulGridSize[%u]        : %u\n", numerologyIndex, p_CarrierConfigInfo->ulGridSize[numerologyIndex] );
                FAPI_Output( "        numRxAnt             : %u\n", p_CarrierConfigInfo->numRxAnt );
                FAPI_Output( "        frequencyShift7p5KHz : %u\n", p_CarrierConfigInfo->frequencyShift7p5KHz );
                FAPI_Output( "        powerProfile         : %u\n", p_CarrierConfigInfo->powerProfile );
                FAPI_Output( "        powerOffsetRsIndex   : %u\n", p_CarrierConfigInfo->powerOffsetRsIndex );

                p_CarrierConfig->tagBitmask |= CONFIG_TAG_BITMASK_CARRIER_CONFIGURATION;
                p_CarrierConfig->carrierConfigInfo = *p_CarrierConfigInfo;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-38 : Cell configuration 

            case TAG_PHY_CELL_ID :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;
                FAPI_Output( "      tag    : TAG_PHY_CELL_ID(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        phyProfileId   : %u\n",value_U16);
                p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PHY_CELL_ID;
                p_CellConfig->phyCellId = value_U16;
                break;

            case TAG_FRAME_DUPLEX_TYPE :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_FRAME_DUPLEX_TYPE(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        frameDuplexType   : %u\n",value_U8);

                p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_FRAME_DUPLEX_TYPE;
                p_CellConfig->frameDuplexType = value_U8;
                break;

            case TAG_PDSCH_TRANS_TYPE_VALIDITY :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDSCH_TRANS_TYPE_VALIDITY(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        pdschTransTypeValidity   : %u\n",value_U8);

                p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PDSCH_TRANS_TYPE_VALIDITY;
                p_CellConfig->pdschTransTypeValidity = value_U8;
                break;

            case TAG_PUSCH_TRANS_TYPE_VALIDITY :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUSCH_TRANS_TYPE_VALIDITY(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        puschTransTypeValidity   : %u\n",value_U8);

                p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PUSCH_TRANS_TYPE_VALIDITY;
                p_CellConfig->puschTransTypeValidity = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-39 : SSB power and PBCH configuration

            case TAG_SS_PBCH_POWER :
                p_Value_U32 = (uint32_t *) p_TlvHeader->value;
                value_U32 = *p_Value_U32;
                FAPI_Output( "      tag    : TAG_SS_PBCH_POWER(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        ssPbchPower   : %u\n",value_U32);
                p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_SS_PBCH_POWER;
                p_SsbPowerPbchConfig->ssPbchPower = value_U32;
                break;

            case TAG_SS_PBCH_BLOCK_POWER_SCALING :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;
                FAPI_Output( "      tag    : TAG_SS_PBCH_BLOCK_POWER_SCALING(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        ssPbchBlockPowerScaling   : %u\n",value_U16);

                p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_SS_PBCH_BLOCK_POWER_SCALING;
                p_SsbPowerPbchConfig->ssPbchBlockPowerScaling = value_U16;
                break;

            case TAG_BCH_PAYLOAD_FLAG :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_BCH_PAYLOAD_FLAG(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        bchPayloadFlag   : %u\n",value_U8);
                p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_BCH_PAYLOAD_FLAG;
                p_SsbPowerPbchConfig->bchPayloadFlag = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-40 : PRACH configuration 

            case TAG_PRACH_RESOURCE_CONFIGURATION :
                p_PrachConfigInfo = (PRACHConfigInfo *) p_TlvHeader->value;
               
                FAPI_Output( "      tag    : TAG_PRACH_RESOURCE_CONFIGURATION(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        prachResConfigIndex           : %u\n", p_PrachConfigInfo->prachResConfigIndex );
                FAPI_Output( "        prachSequenceLength           : %u\n", p_PrachConfigInfo->prachSequenceLength );
                FAPI_Output( "        prachSubCSpacing              : %u\n", p_PrachConfigInfo->prachSubCSpacing );
                FAPI_Output( "        ulBwpPuschScs                 : %u\n", p_PrachConfigInfo->ulBwpPuschScs );
                FAPI_Output( "        restrictedSetConfig           : %u\n", p_PrachConfigInfo->restrictedSetConfig );
                FAPI_Output( "        numPrachFdOccasions           : %u\n", p_PrachConfigInfo->numPrachFdOccasions );
                FAPI_Output( "        prachConfigIndex              : %u\n", p_PrachConfigInfo->prachConfigIndex );
                FAPI_Output( "        prachFormat                   : %u\n", p_PrachConfigInfo->prachFormat );
                FAPI_Output( "        numPrachTdOccasions           : %u\n", p_PrachConfigInfo->numPrachTdOccasions );
                FAPI_Output( "        numberOfPreambles             : %u\n", p_PrachConfigInfo->numberOfPreambles );
                FAPI_Output( "        startPreambleIndex            : %u\n", p_PrachConfigInfo->startPreambleIndex );

                p_PrachConfig->tagBitmask |= CONFIG_TAG_BITMASK_PRACH_RESOURCE_CONFIGURATION;
                p_PrachConfig->prachConfigInfo = *p_PrachConfigInfo;

                p_PrachOccasionInfo = (PRACHOccasionInfo *) (p_PrachConfigInfo+1);
                numPrachFdOccasions = p_PrachConfigInfo->numPrachFdOccasions;
                for (prachFdOccasionIndex = 0; prachFdOccasionIndex < numPrachFdOccasions; prachFdOccasionIndex++)
                {
                    FAPI_Output( "        ForNumPrachFdOccasions[%u] :\n", prachFdOccasionIndex );
                    
                    p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex] = *p_PrachOccasionInfo;
                    FAPI_Output( "          prachRootSequenceIndex        : %u\n", p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachRootSequenceIndex );
                    FAPI_Output( "          numRootSequences              : %u\n", p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numRootSequences );
                    FAPI_Output( "          prachFrequencyOffset          : %u\n", p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachFrequencyOffset );
                    FAPI_Output( "          prachGuardBandOffset          : %u\n", p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachGuardBandOffset );
                    FAPI_Output( "          prachZeroCorrConf             : %u\n", p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachZeroCorrConf );
                    FAPI_Output( "          numUnusedRootSequences        : %u\n", p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences );
                   
                    p_PrachOccasionInfo++;

                    unusedRootSequenceCount = p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences;
                    p_Value_U16 = (uint16_t *) p_PrachOccasionInfo;
                    for (unusedRootSequenceIndex = 0; unusedRootSequenceIndex < unusedRootSequenceCount; unusedRootSequenceIndex++)
                    {
                        FAPI_Output( "          ForNumUnusedRootSequences[%u] :\n", unusedRootSequenceIndex );

                        p_PrachConfig->unusedRootSequencesList[prachFdOccasionIndex][unusedRootSequenceIndex] = *p_Value_U16;
                        FAPI_Output( "            unusedRootSequencesList[%u][%u] : %u\n",
                            prachFdOccasionIndex, unusedRootSequenceIndex, p_PrachConfig->unusedRootSequencesList[prachFdOccasionIndex][unusedRootSequenceIndex]);
                        p_Value_U16++;
                    }
                    p_PrachOccasionInfo = (PRACHOccasionInfo *) p_Value_U16;
                }
                p_Value_U8 = (uint8_t *) p_PrachOccasionInfo;
                p_PrachConfig->ssbPerRach = *p_Value_U8;
                FAPI_Output( "        ssbPerRach                    : %u\n", p_PrachConfig->ssbPerRach);
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-41 : Multi-PRACH configuration 

            case TAG_MULTIPLE_PRACH_CONFIGURATION :
         
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                numPrachConfigurations = *p_Value_U16;
                FAPI_Output( "      tag    : TAG_MULTIPLE_PRACH_CONFIGURATION(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                if (numPrachConfigurations > MAX_MULTI_PRACH_CONFIG_COUNT)
                {
                    FAPI_Error( "ParseTag: Too large numPrachConfigurations %u : limit to %u\n",
                        numPrachConfigurations, MAX_MULTI_PRACH_CONFIG_COUNT);
                    numPrachConfigurations = MAX_MULTI_PRACH_CONFIG_COUNT;
                }
    
                
                p_MultiPrachConfig->tagBitmask |= CONFIG_TAG_BITMASK_MULTIPLE_PRACH_CONFIGURATION;
                p_MultiPrachConfig->numPrachConfigurations = numPrachConfigurations;
                p_SubTlvHeader = (TLV_Header *) (p_Value_U16+1);
                FAPI_Output( "         numPrachConfigurations                 : %u \n ", numPrachConfigurations);
                for (multiPrachConfigInfoIndex = 0; multiPrachConfigInfoIndex < numPrachConfigurations; multiPrachConfigInfoIndex++)
                {
                    if (p_SubTlvHeader->tag == TAG_PRACH_RESOURCE_CONFIGURATION)
                    {
                        p_PrachConfigInMulti = &p_MultiPrachConfig->prachConfig[multiPrachConfigInfoIndex];  
                        p_PrachConfigInfo = (PRACHConfigInfo *) p_SubTlvHeader->value;
                        p_PrachConfigInMulti->prachConfigInfo = *p_PrachConfigInfo;

                        FAPI_Output( "        [%u] prachResConfigIndex         : %u\n ",multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachConfigInfo.prachResConfigIndex);
                        FAPI_Output( "        [%u] prachSequenceLength         : %u\n ",multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachConfigInfo.prachSequenceLength);
                        FAPI_Output( "        [%u] prachSubCSpacing            : %u\n ",multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachConfigInfo.prachSubCSpacing);
                        FAPI_Output( "        [%u] ulBwpPuschScs               : %u\n ",multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachConfigInfo.ulBwpPuschScs);
                        FAPI_Output( "        [%u] restrictedSetConfig         : %u\n ",multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachConfigInfo.restrictedSetConfig);
                        FAPI_Output( "        [%u] numPrachFdOccasions         : %u\n ",multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachConfigInfo.numPrachFdOccasions);
                        FAPI_Output( "        [%u] prachConfigIndex            : %u\n ",multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachConfigInfo.prachConfigIndex);
                        numPrachFdOccasions = p_PrachConfigInMulti->prachConfigInfo.numPrachFdOccasions;
                        p_PrachOccasionInfo = (PRACHOccasionInfo *) (p_PrachConfigInfo+1);
                        for (prachFdOccasionIndex = 0; prachFdOccasionIndex < numPrachFdOccasions; prachFdOccasionIndex++)
                        {
                            p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex] = *p_PrachOccasionInfo;
                             
                            FAPI_Output("        [%u] prachRootSequenceIndex      : %u\n ",
                                multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].prachRootSequenceIndex);
                            FAPI_Output("        [%u] numRootSequences            : %u\n ",
                                multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].numRootSequences) ;
                            FAPI_Output("        [%u] prachZeroCorrConf           : %u\n ",
                                multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].prachZeroCorrConf) ;                                
                            FAPI_Output("        [%u] numUnusedRootSequences      : %u\n ",
                                multiPrachConfigInfoIndex,p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences) ;                      
                  
                            p_PrachOccasionInfo++;

                            unusedRootSequenceCount = p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences;
                            p_Value_U16 = (uint16_t *) p_PrachOccasionInfo;

                            for (unusedRootSequenceIndex = 0; unusedRootSequenceIndex < unusedRootSequenceCount; unusedRootSequenceIndex++)
                            {
                                p_PrachConfigInMulti->unusedRootSequencesList[prachFdOccasionIndex][unusedRootSequenceIndex] = *p_Value_U16;
                                FAPI_Output("        [%u] unusedRootSequencesList[%u][%u]    : %u\n",
                                    multiPrachConfigInfoIndex,prachFdOccasionIndex,unusedRootSequenceIndex,
                                    p_PrachConfigInMulti->unusedRootSequencesList[prachFdOccasionIndex][unusedRootSequenceIndex]) ;
                                p_Value_U16++;
                            }
                            p_PrachOccasionInfo = (PRACHOccasionInfo *) p_Value_U16;
                        }

                        p_Value_U8 = (uint8_t *) p_PrachOccasionInfo;
                        p_PrachConfigInMulti->ssbPerRach = *p_Value_U8;
                        FAPI_Output("         [%u] ssbPerRach                   : %u\n ",multiPrachConfigInfoIndex,p_PrachConfigInMulti->ssbPerRach);
                    }
                    else
                    {
                        FAPI_Error( "Invalid Sub Tags 0x%04x in TAG_MULTIPLE_PRACH_CONFIGURATION\n", p_SubTlvHeader->tag );
                    }
                      
                    p_SubTlvHeader = (TLV_Header *) ((uint8_t *)p_SubTlvHeader + sizeof(TLV_Header) + p_SubTlvHeader->length);
                }
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-42 : MSGA PUSCH configuration table :

            case TAG_MSGA_PUSCH_CONFIGURATION :
                p_MsgAPuschConfigInfo = (MSGA_PUSCH_ConfigInfo*)p_TlvHeader->value;
                FAPI_Output( "      tag    : TAG_MSGA_PUSCH_CONFIGURATION(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        msgApuschResConfigIndex                  : %u\n", p_MsgAPuschConfigInfo->msgApuschResConfigIndex);
                FAPI_Output( "        msgAprachResConfigIndex                  : %u\n", p_MsgAPuschConfigInfo->msgAprachResConfigIndex );
                FAPI_Output( "        msgAGroupAorB                            : %u\n", p_MsgAPuschConfigInfo->msgAGroupAorB );
                FAPI_Output( "        nIdMsgAPusch                             : %u\n", p_MsgAPuschConfigInfo->nIdMsgAPusch );
                FAPI_Output( "        dmrsPorts                                : %u\n", p_MsgAPuschConfigInfo->dmrsPorts );
                FAPI_Output( "        availableDmrsSequenceIds                 : %u\n", p_MsgAPuschConfigInfo->availableDmrsSequenceIds );
                FAPI_Output( "        puschDmrsScramblingIds                   : %u,%u\n", 
                    p_MsgAPuschConfigInfo->puschDmrsScramblingIds[0], p_MsgAPuschConfigInfo->puschDmrsScramblingIds[1] );
                FAPI_Output( "        numPuschOcasFd                           : %u\n", p_MsgAPuschConfigInfo->numPuschOcasFd );
                FAPI_Output( "        numPuschOcasTd                           : %u\n", p_MsgAPuschConfigInfo->numPuschOcasTd );
                FAPI_Output( "        numPuschOcasSlots                        : %u\n", p_MsgAPuschConfigInfo->numPuschOcasSlots );
                FAPI_Output( "        msgAPuschTimeDomainOffset                : %u\n", p_MsgAPuschConfigInfo->msgAPuschTimeDomainOffset );
                FAPI_Output( "        nPreambles                               : %u\n", p_MsgAPuschConfigInfo->nPreambles );
                FAPI_Output( "        associationPatternPeriod                 : %u\n", p_MsgAPuschConfigInfo->associationPatternPeriod);

                p_AssociationPatternPeriodInfo = (AssociationPatternPeriodInfo *) (p_MsgAPuschConfigInfo+1);

                numAssociationPatternPeriod = p_MsgAPuschConfigInfo->associationPatternPeriod;
                for (associationPatternPeriodIndex = 0; associationPatternPeriodIndex < numAssociationPatternPeriod; associationPatternPeriodIndex++)
                {
                    p_MsgAPuschConfig->associationPatternPeriodInfo[associationPatternPeriodIndex] = *p_AssociationPatternPeriodInfo;

                    FAPI_Output( "        ForAssociationPatternPeriod[%u] :\n", associationPatternPeriodIndex );
                    FAPI_Output( "          numPrachSlotsforPrachtoPruAndDmrsMap   : %u\n",
                        p_MsgAPuschConfig->associationPatternPeriodInfo[associationPatternPeriodIndex].numPrachSlotsforPrachtoPruAndDmrsMap );
                    FAPI_Output( "          prachSlotIndicesModulusApPslots        : %u\n",
                        p_MsgAPuschConfig->associationPatternPeriodInfo[associationPatternPeriodIndex].prachSlotIndicesModulusApPslots );
                    FAPI_Output( "          msgAPrachtoMapping                     :\n" );    // TODO : Table 3-43

                    p_AssociationPatternPeriodInfo++;
                }
    
                p_MsgAPuschConfig->tagBitmask |= CONFIG_TAG_BITMASK_MSGA_PUSCH_CONFIGURATION;
                p_MsgAPuschConfig->msgAPuschConfigInfo = *p_MsgAPuschConfigInfo;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            
            // Table 3-45 : SSB resource configuration table : 

            case TAG_SSB_TRANSMISSION_PATTERN :
                p_SsbResourceConfigInfo = (SSBResourceConfigInfo *) p_TlvHeader->value;
                FAPI_Output( "      tag    : TAG_SSB_TRANSMISSION_PATTERN(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        ssbConfigIndex                : %u\n", p_SsbResourceConfigInfo->ssbConfigIndex );
                FAPI_Output( "        ssbOffsetPointA               : %u\n", p_SsbResourceConfigInfo->ssbOffsetPointA );
                FAPI_Output( "        betaPssProfileNR              : %u\n", p_SsbResourceConfigInfo->betaPssProfileNR );
                FAPI_Output( "        betaPssProfileSSS             : %u\n", p_SsbResourceConfigInfo->betaPssProfileSSS );
                FAPI_Output( "        ssbPeriod                     : %u\n", p_SsbResourceConfigInfo->ssbPeriod );
                FAPI_Output( "        ssbSubcarrierOffset           : %u\n", p_SsbResourceConfigInfo->ssbSubcarrierOffset );
                FAPI_Output( "        ssbFirstSymbolMappingCase     : %u\n", p_SsbResourceConfigInfo->ssbFirstSymbolMappingCase );
                FAPI_Output( "        subcarrierSpacing             : %u\n", p_SsbResourceConfigInfo->subcarrierSpacing );
                FAPI_Output( "        subCarrierSpacingCommon       : %u\n", p_SsbResourceConfigInfo->subCarrierSpacingCommon );
                FAPI_Output( "        ssbMaskSize                   : %u\n", p_SsbResourceConfigInfo->ssbMaskSize);

                if(p_SsbResourceConfigInfo->ssbMaskSize == 2 )
                {
                    FAPI_Output( "          ssbMask[0]                  : %u\n", p_SsbResourceConfigInfo->ssbMask[0]);
                    FAPI_Output( "          ssbMask[1]                  : %u\n", p_SsbResourceConfigInfo->ssbMask[1]);
                }
                else 
                {
                    for (beamIdx= 0; beamIdx <MAX_SSB_MASK_SIZE_SIXTY_FOUR; beamIdx++)
                    {
                        FAPI_Output( "          beamId[%02u]                  : %u\n", beamIdx,p_SsbResourceConfigInfo->beamId[beamIdx]);
                    }
                }
                FAPI_Output( "        lMax                          : %u\n", p_SsbResourceConfigInfo->lMax);
                FAPI_Output( "        rmsiPresence                  : %u\n", p_SsbResourceConfigInfo->rmsiPresence);
                
                p_SsbResourceConfig->tagBitmask |= CONFIG_TAG_BITMASK_SSB_TRANSMISSION_PATTERN;
                p_SsbResourceConfig->ssbResourceConfigInfo = *p_SsbResourceConfigInfo;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-37 : Multi-SSB resource configuration table

            case TAG_MULTIPLE_SSB_CONFIGURATION :
                p_Value_U8 = &p_TlvHeader->value[0];
                numSsbConfigurations = *p_Value_U8;
                FAPI_Output( "      tag    : TAG_MULTIPLE_SSB_CONFIGURATION(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "      numSsbConfigurations  :%u\n",numSsbConfigurations );
                if (numSsbConfigurations > MAX_MULTI_SSB_CONFIG_COUNT)
                {
                    FAPI_Error( "AddTag: Too large numSsbConfigurations %u : limit to %u\n",
                        numSsbConfigurations, MAX_MULTI_PRACH_CONFIG_COUNT);
                    numSsbConfigurations = MAX_MULTI_SSB_CONFIG_COUNT;
                }

                p_MultiSsbResourceConfig->tagBitmask |= CONFIG_TAG_BITMASK_MULTIPLE_SSB_CONFIGURATION;
                p_MultiSsbResourceConfig->numSsbConfigurations = numSsbConfigurations;
            
                p_SubTlvHeader = (TLV_Header *) (p_Value_U8+1);
                    
                for (multiSsbResourceConfigIndex = 0; multiSsbResourceConfigIndex < numSsbConfigurations; multiSsbResourceConfigIndex++)
                {
                    if (p_SubTlvHeader->tag == TAG_SSB_TRANSMISSION_PATTERN)
                    {
                        p_SsbResourceConfigInMulti = &p_MultiSsbResourceConfig->ssbResourceConfig[multiSsbResourceConfigIndex];
                        
                        p_SsbResourceConfigInfo = (SSBResourceConfigInfo *) p_SubTlvHeader->value;
                        p_SsbResourceConfigInMulti->ssbResourceConfigInfo = *p_SsbResourceConfigInfo;
                        p_SsbResourceConfigInMulti = &p_MultiSsbResourceConfig->ssbResourceConfig[multiSsbResourceConfigIndex];
                        FAPI_Output( "        ssbConfigIndex                : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbConfigIndex );
                        FAPI_Output( "        ssbOffsetPointA               : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbOffsetPointA );
                        FAPI_Output( "        betaPssProfileNR              : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.betaPssProfileNR );
                        FAPI_Output( "        betaPssProfileSSS             : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.betaPssProfileSSS );
                        FAPI_Output( "        ssbPeriod                     : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbPeriod );
                        FAPI_Output( "        ssbSubcarrierOffset           : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbSubcarrierOffset );
                        FAPI_Output( "        ssbFirstSymbolMappingCase     : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbFirstSymbolMappingCase );
                        FAPI_Output( "        subcarrierSpacing             : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.subcarrierSpacing );
                        FAPI_Output( "        subCarrierSpacingCommon       : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.subCarrierSpacingCommon );
                        FAPI_Output( "        ssbMaskSize                   : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbMaskSize);                        
                       
                        if ( p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbMaskSize ==2)
                        {
                            FAPI_Output( "        ssbMask[0]                    : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbMask[0]);
                            FAPI_Output( "        ssbMask[1]                    : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbMask[1]);
                        }
                        else
                        {
                            for (beamIdx= 0; beamIdx <MAX_SSB_MASK_SIZE_SIXTY_FOUR; beamIdx++)
                            {        
                                FAPI_Output( "        beamId[%u]       : %u\n", beamIdx,p_SsbResourceConfigInMulti->ssbResourceConfigInfo.beamId[beamIdx]);
                            }
                        }    
                        FAPI_Output( "        lMax                          : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.lMax);
                        FAPI_Output( "        rmsiPresence                  : %u\n", p_SsbResourceConfigInMulti->ssbResourceConfigInfo.rmsiPresence);
                    }
                    else
                    {
                        FAPI_Error( "Invalid Sub Tags 0x%04x TAG_MULTIPLE_SSB_CONFIGURATION\n", p_SubTlvHeader->tag );
                    }

                    p_SubTlvHeader = (TLV_Header *) ((uint8_t *)p_SubTlvHeader + sizeof(TLV_Header) + p_SubTlvHeader->length);
                }
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-38 : TDD table 

            case TAG_TDD_PATTERN :
                p_TddPatternInfo = (TDDPatternInfo *) p_TlvHeader->value;
                FAPI_Output( "      tag    : TAG_TDD_PATTERN(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        tddPeriod                     : %u\n", p_TddPatternInfo->tddPeriod);
                FAPI_Output( "        slotConfig                    : ");

                for(slotIndex = 0; slotIndex < MAX_TDD_PERIODICITY_SLOT_COUNT; slotIndex++)
                {
                    for(symbolIndex = 0; symbolIndex < MAX_NUM_OF_SYMBOL_PER_SLOT; symbolIndex++)
                    {
                        if (p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] == 0)
                        {
                            FAPI_Output("D");
                        }
                        else if (p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] == 1)
                        {
                            FAPI_Output("U");
                        }
                        else if (p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] == 2)
                        {
                            FAPI_Output("F");
                        }
                        else if (p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] == 0xFF)
                        {
                            slotIndex = MAX_TDD_PERIODICITY_SLOT_COUNT;
                            symbolIndex = MAX_NUM_OF_SYMBOL_PER_SLOT;
                        }
                    }
                    FAPI_Output(" ");
                }
                FAPI_Output("\n");
                
                p_TddConfig->tagBitmask |= CONFIG_TAG_BITMASK_TDD_PATTERN;
                p_TddConfig->tddPatternInfo = *p_TddPatternInfo;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-48 : Measurement configuration 

            case TAG_RSSI_MEASUREMENT_CONFIGURATION :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_MEASUREMENT_CONFIGURATION(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        rssiMeasurement               : %u\n",value_U8);       

                p_MeasurementConfig->tagBitmask |= CONFIG_TAG_BITMASK_RSSI_MEASUREMENT_CONFIGURATION;
                p_MeasurementConfig->rssiMeasurement = value_U8;
                break; 

            case TAG_RSRP_MEASUREMENT_CONFIGURATION :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_RSRP_MEASUREMENT_CONFIGURATION(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        rsrpMeasurement               : %u\n",value_U8);

                p_MeasurementConfig->tagBitmask |= CONFIG_TAG_BITMASK_RSRP_MEASUREMENT_CONFIGURATION;
                p_MeasurementConfig->rsrpMeasurement = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-51 : Digital beam table (DBT) PDU : No tag is defined in document : Skip for now

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-52 : Precoding matrix (PM) PDU : No tag is defined in document : Skip for now

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-49 : UCI configuration 

            case TAG_UCI_INDICATION_CONFIGURATION :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                numUci2Maps = *p_Value_U16;
                FAPI_Output( "      tag    : TAG_UCI_INDICATION_CONFIGURATION(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        numUci2Maps                   : %u\n",numUci2Maps);     

                if (numUci2Maps > MAX_NUM_UCI2_MAPS)
                {
                    FAPI_Error( "AddTag: Too large numUci2Maps %u : limit to %u\n",
                        numUci2Maps, MAX_NUM_UCI2_MAPS);
                    numUci2Maps = MAX_NUM_UCI2_MAPS;
                }

                p_UciConfig->tagBitmask |= CONFIG_TAG_BITMASK_UCI_INDICATION_CONFIGURATION;
                p_UciConfig->numUci2Maps = numUci2Maps;

                p_Value_U8 = (uint8_t *) (p_Value_U16+1);
                for (uci2MapsIndex = 0; uci2MapsIndex < numUci2Maps; uci2MapsIndex++)
                {
                    FAPI_Output( "        ForNumUci2Maps[%u]             :\n", uci2MapsIndex );

                    numPart1Params = *p_Value_U8;
                    if (numPart1Params > MAX_NUM_OF_UCI_PART1_PARAMS)
                    {
                        FAPI_Error( "AddTag: Too large numPart1Params %u : limit to %u\n",
                            numPart1Params, MAX_NUM_OF_UCI_PART1_PARAMS );
                        numPart1Params = MAX_NUM_OF_UCI_PART1_PARAMS;
                    }

                    p_UciConfig->uci2MapInfo[uci2MapsIndex].numPart1Params = numPart1Params;
                    FAPI_Output( "          numPart1Params              : %u\n", numPart1Params );

                    p_Value_U8++;

                    sumOfSizesPart1Params = 0;
                    FAPI_Output( "          sizesPart1ParamsValue       :" );
                    for (part1ParamsIndex = 0; part1ParamsIndex < numPart1Params; part1ParamsIndex++)
                    {
                        sizesPart1ParamsValue = *p_Value_U8;  
                        p_UciConfig->uci2MapInfo[uci2MapsIndex].sizesPart1Params[part1ParamsIndex] = sizesPart1ParamsValue;
                        FAPI_Output( " %u", sizesPart1ParamsValue );
                        p_Value_U8++;

                        sumOfSizesPart1Params += sizesPart1ParamsValue;
                    }
                    FAPI_Output( "\n" );

                    if (sumOfSizesPart1Params > MAX_SUM_OF_SIZES_PART1_PARAMS)
                    {
                        FAPI_Error( "AddTag: Too large sumOfSizesPart1Params %u : limit to %u\n",
                            sumOfSizesPart1Params, MAX_SUM_OF_SIZES_PART1_PARAMS);
                        sumOfSizesPart1Params = MAX_SUM_OF_SIZES_PART1_PARAMS;
                    }
                    
                    // Calculate mapSize as 2^sumOfSizesPart1Params
                    mapSize = (sumOfSizesPart1Params > 0) ? (2 << (sumOfSizesPart1Params-1)) : 1;
                    if (mapSize > MAX_UCI2_MAP_SIZE)
                    {
                        FAPI_Error( "AddTag: Too large mapSize %u : limit to %u\n",
                            mapSize, MAX_UCI2_MAP_SIZE );
                        mapSize = MAX_UCI2_MAP_SIZE;
                    }

                    memcpy( p_UciConfig->uci2MapInfo[uci2MapsIndex].map, p_Value_U8, mapSize*sizeof(uint16_t) );

                    FAPI_Output( "          map                         :" );
                    for(mapIndex = 0; mapIndex < mapSize; mapIndex++)
                    {
                        FAPI_Output( " %u", p_UciConfig->uci2MapInfo[uci2MapsIndex].map[mapIndex] ); 
                    }
                    FAPI_Output( "\n" );
                }
                break; 

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-41 : PRB-symbol rate match patterns bitmap (non-CORESET) configuration

            case TAG_PRB_SYMBOL_RATE_MATCH_PATTERN :
                p_Value_U8 = &p_TlvHeader->value[0];
                numberOfPrbSymbRateMatchPatterns = *p_Value_U8;
                FAPI_Output( "      tag    : TAG_PRB_SYMBOL_RATE_MATCH_PATTERN(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        numberOfPrbSymbRateMatchPatterns        : %u\n",numberOfPrbSymbRateMatchPatterns);    

                if (numberOfPrbSymbRateMatchPatterns > MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT)
                {
                    FAPI_Error( "AddTag: Too large numberOfPrbSymbRateMatchPatterns %u : limit to %u\n",
                        numberOfPrbSymbRateMatchPatterns, MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT);
                    numberOfPrbSymbRateMatchPatterns = MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT;
                }

                p_PrbSymbRateMatchPatternsConfig->tagBitmask |= CONFIG_TAG_BITMASK_PRB_SYMBOL_RATE_MATCH_PATTERN;
                p_PrbSymbRateMatchPatternsConfig->numberOfPrbSymbRateMatchPatterns = numberOfPrbSymbRateMatchPatterns;

                p_PrbSymbRateMatchPatternInfo = (PRBSymRateMatchPatternInfo *) (p_Value_U8+1);
                for (prbSymbRateMatchPatternInfoIndex = 0; prbSymbRateMatchPatternInfoIndex< numberOfPrbSymbRateMatchPatterns; prbSymbRateMatchPatternInfoIndex++)
                {
                    FAPI_Output( "        ForNumberOfPrbSymbRateMatchPatterns[%u]  :\n", prbSymbRateMatchPatternInfoIndex );

                    p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex] = *p_PrbSymbRateMatchPatternInfo;
                    FAPI_Output( "          prbSymbRateMatchPatternID             : %u\n",
                        p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex].prbSymbRateMatchPatternID);
                    FAPI_Output( "          freqDomainRB                          :");
                    for (freqDomainRBIndex = 0; freqDomainRBIndex < MAX_RATEMATCH_BITMAP_SIZE; freqDomainRBIndex++)
                    {
                        FAPI_Output( " %u", p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex].freqDomainRB[freqDomainRBIndex]);
                    }
                    FAPI_Output("\n");
                    
                    FAPI_Output( "          symbolsInRB                           : %u\n",
                        p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex].symbolsInRB);
                    FAPI_Output( "          subcarrierSpacing                     : %u\n",
                        p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex].subcarrierSpacing);
                    p_PrbSymbRateMatchPatternInfo++;
                }
                break; 

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-41 : PRB-symbol rate match patterns bitmap (non-CORESET) configuration

            case TAG_CORESET_RATE_MATCH_PATTERN :

                p_Value_U8 = &p_TlvHeader->value[0];
                numberOfCoresetRateMatchPatterns = *p_Value_U8;
                FAPI_Output( "      tag    : TAG_CORESET_RATE_MATCH_PATTERN(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        numberOfCoresetRateMatchPatterns        : %u\n", numberOfCoresetRateMatchPatterns);

                if (numberOfCoresetRateMatchPatterns > MAX_CORESET_RATE_MATCH_PATTERN_COUNT)
                {
                    FAPI_Error( "AddTag: Too large numberOfCoresetRateMatchPatterns %u : limit to %u\n",
                        numberOfCoresetRateMatchPatterns, MAX_CORESET_RATE_MATCH_PATTERN_COUNT);
                    numberOfCoresetRateMatchPatterns = MAX_CORESET_RATE_MATCH_PATTERN_COUNT;
                }

                p_CoresetRateMatchPatternsConfig->tagBitmask |= CONFIG_TAG_BITMASK_CORESET_RATE_MATCH_PATTERN;
                p_CoresetRateMatchPatternsConfig->numberOfCoresetRateMatchPatterns = numberOfCoresetRateMatchPatterns;

                p_CoresetRateMatchPatternInfo = (CoresetRateMatchPatternInfo *) (p_Value_U8+1);
                for (coresetRateMatchPatternInfoIndex = 0; coresetRateMatchPatternInfoIndex < numberOfCoresetRateMatchPatterns; coresetRateMatchPatternInfoIndex++)
                {
                    FAPI_Output( "        ForNumberOfCoresetRateMatchPatterns[%u]  :\n", coresetRateMatchPatternInfoIndex );

                    p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex] = *p_CoresetRateMatchPatternInfo;
                    FAPI_Output( "          coresetRateMatchPatternID             : %u\n",
                        p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex].coresetRateMatchPatternID);
                    FAPI_Output( "          freqDomainResources                   :");
                    for (freqDomainRBIndex = 0; freqDomainRBIndex < MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE; freqDomainRBIndex++)
                    {
                        FAPI_Output( " %u", p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex].freqDomainResources[freqDomainRBIndex]);
                    }
                    FAPI_Output("\n");

                    FAPI_Output( "          symbolsPattern                        : %u\n",
                        p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex].symbolsPattern);
                    FAPI_Output( "          subcarrierSpacing                     : %u\n",
                        p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex].subcarrierSpacing);
                    p_CoresetRateMatchPatternInfo++;
                }
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-42 : LTE-CRS rate match patterns configuration

            case TAG_LTE_CRS_RATE_MATCH_PATTERN :
                p_Value_U8 = &p_TlvHeader->value[0];
                lteCrsRateMatchPatternsCount = *p_Value_U8;
                p_Value_U8++;
                FAPI_Output( "      tag    : TAG_LTE_CRS_RATE_MATCH_PATTERN(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        lteCrsRateMatchPatternsCount                       : %u\n",lteCrsRateMatchPatternsCount);   
                if (lteCrsRateMatchPatternsCount > MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT)
                {
                    FAPI_Error( "AddTag: Too large lteCrsRateMatchPatternsCount %u : limit to %u\n",
                        lteCrsRateMatchPatternsCount, MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT);
                    lteCrsRateMatchPatternsCount = MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT;
                }

                p_LteCrsRateMatchPatternsConfig->tagBitmask |= CONFIG_TAG_BITMASK_LTE_CRS_RATE_MATCH_PATTERN;
                p_LteCrsRateMatchPatternsConfig->numberOfLteCrsRateMatchPatterns = lteCrsRateMatchPatternsCount;

                for (lteCrsRateMatchPatternsIndex = 0; lteCrsRateMatchPatternsIndex < lteCrsRateMatchPatternsCount; lteCrsRateMatchPatternsIndex++)
                {
                    FAPI_Output( "        ForLteCrsRateMatchPatternsCount[%u]                 :\n", lteCrsRateMatchPatternsIndex );

                    p_LteCrsRateMatchPatternInfo = &(p_LteCrsRateMatchPatternsConfig->lteCrsRateMatchPatternInfo[lteCrsRateMatchPatternsIndex]);

                    p_LteCrsRateMatchPatternInfo->crsRateMatchPatternID = *p_Value_U8;
                    FAPI_Output( "          crsRateMatchPatternID                            : %u\n", p_LteCrsRateMatchPatternInfo->crsRateMatchPatternID); 
                    p_Value_U8++;

                    p_Value_U16 = (uint16_t *) p_Value_U8;
                    p_LteCrsRateMatchPatternInfo->carrierFreqDL = *p_Value_U16;
                    FAPI_Output( "          carrierFreqDL                                    : %u\n", p_LteCrsRateMatchPatternInfo->carrierFreqDL); 
                    p_Value_U16++;

                    p_Value_U8 = (uint8_t *) p_Value_U16;
                    p_LteCrsRateMatchPatternInfo->carrierBandwidthDL= *p_Value_U8;
                    FAPI_Output( "          carrierBandwidthDL                               : %u\n", p_LteCrsRateMatchPatternInfo->carrierBandwidthDL);
                    p_Value_U8++;

                    p_LteCrsRateMatchPatternInfo->nrofCrsPorts = *p_Value_U8;
                    FAPI_Output( "          nrofCrsPorts                                     : %u\n", p_LteCrsRateMatchPatternInfo->nrofCrsPorts); 
                    p_Value_U8++;

                    p_LteCrsRateMatchPatternInfo->vShift = *p_Value_U8;
                    FAPI_Output( "          vShift                                           : %u\n", p_LteCrsRateMatchPatternInfo->vShift);
                    p_Value_U8++;

                    mbsfnSubframeConfigCount = *p_Value_U8;
                    FAPI_Output( "          sizeMbsfnSubframeConfigList                      : %u\n", mbsfnSubframeConfigCount);               
                    p_Value_U8++;
                    if (mbsfnSubframeConfigCount > MAX_MBSFN_SUBFRAME_CONFIG_COUNT)
                    {
                        FAPI_Error( "AddTag: Too large mbsfnSubframeConfigCount %u : limit to %u\n",
                            mbsfnSubframeConfigCount, MAX_MBSFN_SUBFRAME_CONFIG_COUNT);
                        mbsfnSubframeConfigCount= MAX_MBSFN_SUBFRAME_CONFIG_COUNT;
                    }
                    p_LteCrsRateMatchPatternInfo->sizeMbsfnSubframeConfigList = mbsfnSubframeConfigCount;

                    p_MbsfnSubframeConfigInfo = (MbsfnSubframeConfigInfo *) p_Value_U8;
                    for (mbsfnSubframeConfigIndex = 0; mbsfnSubframeConfigIndex < mbsfnSubframeConfigCount; mbsfnSubframeConfigIndex++)
                    {
                        FAPI_Output( "          ForSizeMbsfnSubframeConfigList[%u]                :\n", mbsfnSubframeConfigIndex );

                        p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex] = *p_MbsfnSubframeConfigInfo;

                        FAPI_Output( "            radioframeAllocationPeriod                     : %u\n",
                            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].radioframeAllocationPeriod);
                        FAPI_Output( "            radioframeAllocationOffset                     : %u\n",
                            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].radioframeAllocationOffset);
                        FAPI_Output( "            lteFrameStructureType                          : %u\n",
                            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].lteFrameStructureType);
                        FAPI_Output( "            subframeAllocLength                            : %u\n",
                            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].subframeAllocLength);
                        FAPI_Output( "            subframeAllocationBitmap                       : %u\n",
                            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].subframeAllocationBitmap);

                        p_MbsfnSubframeConfigInfo++;
                    }
                    p_Value_U8 = (uint8_t *) p_MbsfnSubframeConfigInfo;
                }
                break; 

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-43 : PUCCH semi-static configuration 

            case TAG_PUCCH_SEMI_STATIC_PARAMETER :
                p_Value_U8 = &p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PUCCH_SEMI_STATIC_PARAMETER(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        numUlBwpIds                             : %u\n", *p_Value_U8); 

                p_PucchSemiStaticConfig->tagBitmask |= CONFIG_TAG_BITMASK_PUCCH_SEMI_STATIC_PARAMETER;
                p_PucchSemiStaticConfig->numUlBwpIds = *p_Value_U8;
                p_Value_U8++;
                p_PucchSemiStaticConfigInfo = (PUCCH_SemiStatic_ConfigInfo *) (p_Value_U8);
                for (ulBwpIndex = 0; ulBwpIndex < p_PucchSemiStaticConfig->numUlBwpIds; ulBwpIndex++)
                {
                    FAPI_Output( "        ForNumUlBwpIds[%u]                       :\n", ulBwpIndex );

                    p_PucchSemiStaticConfig->pucchSemiStaticConfigInfo[ulBwpIndex] = *p_PucchSemiStaticConfigInfo;
                    FAPI_Output( "          pucchGroupHopping                     : %u\n",
                        p_PucchSemiStaticConfig->pucchSemiStaticConfigInfo[ulBwpIndex].pucchGroupHopping); 
                    FAPI_Output( "          nIdPucchHopping                       : %u\n",
                        p_PucchSemiStaticConfig->pucchSemiStaticConfigInfo[ulBwpIndex].nIdPucchHopping); 
                    p_PucchSemiStaticConfigInfo++;
                }
                break; 

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-44 : PDSCH semi-static configuration 

            case TAG_PDSCH_SEMI_STATIC_PARAMETER :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_PDSCH_SEMI_STATIC_PARAMETER(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        pdschCbgScheme                : %u\n",value_U8); 

                p_PdschSemiStaticConfig->tagBitmask |= CONFIG_TAG_BITMASK_PDSCH_SEMI_STATIC_PARAMETER;
                p_PdschSemiStaticConfig->pdschCbgScheme = value_U8;
                break; 

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-45 : Delay management configuration 

            case TAG_TIMING_WINDOW :
                p_Value_U16 = (uint16_t *) p_TlvHeader->value;
                value_U16 = *p_Value_U16;
                FAPI_Output( "      tag    : TAG_TIMING_WINDOW(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        timingWindow                  : %u\n",value_U16); 

                p_DelayManagementConfig->tagBitmask |= CONFIG_TAG_BITMASK_TIMING_WINDOW;
                p_DelayManagementConfig->timingWindow = value_U16;
                break;

            case TAG_TIMING_INFO_MODE :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_TIMING_INFO_MODE(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        timingInfoMode                : %u\n",value_U8); 

                p_DelayManagementConfig->tagBitmask |= CONFIG_TAG_BITMASK_TIMING_INFO_MODE;
                p_DelayManagementConfig->timingInfoMode = value_U8;
                break;

            case TAG_TIMING_INFO_PERIOD :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_TIMING_INFO_PERIOD(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        timingInfoPeriod              : %u\n",value_U8); 

                p_DelayManagementConfig->tagBitmask |= CONFIG_TAG_BITMASK_TIMING_INFO_PERIOD;
                p_DelayManagementConfig->timingInfoPeriod = value_U8;
                break;

            ///////////////////////////////////////////////////////////////////////////////////////////
            // Table 3-46 : Rel-16 mTRP configuration 

            case TAG_NUM_TX_PORTS_TRP1 :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_NUM_TX_PORTS_TRP1(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        numTxPortsTRP1                : %u\n",value_U8);

                p_Rel16mTRPConfig->tagBitmask |= CONFIG_TAG_BITMASK_NUM_TX_PORTS_TRP1;
                p_Rel16mTRPConfig->numTxPortsTRP1 = value_U8;
                break;

            case TAG_NUM_RX_PORTS_TRP1 :
                value_U8 = p_TlvHeader->value[0];
                FAPI_Output( "      tag    : TAG_NUM_RX_PORTS_TRP1(0x%x)\n", p_TlvHeader->tag );
                FAPI_Output( "      length : %u\n", tlvValueLength );
                FAPI_Output( "      value  :\n" );
                FAPI_Output( "        numRxPortsTRP1                : %u\n",value_U8);

                p_Rel16mTRPConfig->tagBitmask |= CONFIG_TAG_BITMASK_NUM_RX_PORTS_TRP1;
                p_Rel16mTRPConfig->numRxPortsTRP1 = value_U8;
                break;

            default :
                FAPI_Output( "      tag %u is not supported\n", p_TlvHeader->tag );
                break;
        }
        p_TlvHeader = (TLV_Header *) ((uint8_t *)p_TlvHeader + tlvLength);
    }
}

void phy_ParseMsg_TestConfigReq( Fapi_Generic_Header *p_FapiGenericHeader, TestConfigReq *p_TestConfigReqDest )
{
    TestConfigReq *p_TestConfigReqSrc;

    FAPI_Output( "    type   : MSG_TEST_CONFIG_REQ(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_TestConfigReqSrc = (TestConfigReq *) (p_FapiGenericHeader+1);
    memcpy( p_TestConfigReqDest, p_TestConfigReqSrc, sizeof(TestConfigReq) );

    FAPI_Output( "    slotCountToRun         : %u\n", p_TestConfigReqSrc->slotCountToRun );
    FAPI_Output( "    basePath               : %s\n", p_TestConfigReqSrc->basePath );
    FAPI_Output( "    filenameDlIqRef        : %s\n", p_TestConfigReqSrc->filenameDlIqRef );
    FAPI_Output( "    filenameDlIqOutput     : %s\n", p_TestConfigReqSrc->filenameDlIqOutput );
    FAPI_Output( "    filenameUlIqRef        : %s\n", p_TestConfigReqSrc->filenameUlIqRef );
    FAPI_Output( "    filenameUlResultRef    : %s\n", p_TestConfigReqSrc->filenameUlResultRef );
    FAPI_Output( "    filenameUlResultOutput : %s\n", p_TestConfigReqSrc->filenameUlResultOutput );
}

void phy_ParseMsg_DLTTIReq( Fapi_Generic_Header *p_FapiGenericHeader )
{
    uint16_t pduIndex, pduCount;
    uint8_t  numOfcodewords, szRm,szCrs,szCsirs, dciPayloadcnt, rbBitmapsize;
    uint8_t  freqRbsize, maxPdschRmPatterncnt, maxPdschCoresetRmPatterncnt,freqDomainResource, pdschSsbRmInfo;
    uint32_t numPRGs, prgIndex, digBFInterfaces, digBFInterfacesIndex, numTrp, trpIndex, arrayIndex, numMuMimoGroups, muMimoGroupIndex;
    uint32_t dlDciIndex, layerIndex;
    DL_TTI_REQ *p_DlTtiReq;
    PDU_Struct *p_pduHeader;
    SSBPBCH_PDU *p_SsbPdu;
    MAC_GenMIB_PDU *p_MacGenMIBPdu;
    PHY_GenMIB_PDU *p_PHY_GenMIB_PDU;
    TxPrecodingAndBeamforming_PDU *p_TxPrecodingBfPdu;
    Rel16mTRP_TxPrecodingAndBeamforming_PDU *p_Rel16mTRP_TxPrecodingBfPdu;
    Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo;
    SSBPBCH_PDU_Maintenance_FAPIv3 *p_SsbPduV3;
    SSBPBCH_PDU_FAPIv4 *p_SsbPduV4;
    PDCCH_PDU  *p_PdcchPdu;
    DL_DCI_PDU_part1 *p_DlDci1Pdu;
    DL_DCI_PDU_part2 *p_DlDci2Pdu;
    PDCCH_PDU_Maintenance_FAPIv3 *p_PdcchPduV3;
    PDCCH_PDU_Maintenance_FAPIv3_DCIInfo *p_PdcchPduV3_Dci; 
    PDCCH_PDU_FAPIv4_DCIInfo *p_PdcchPduV4_Dci;
    PDCCH_PDU_FAPIv4_MuMimo *p_PdcchPduV4_MuMimo;
    PDCCH_PDU_FAPIv4_MuMimo_DCIInfo *p_PdcchPduV4_MuMimo_DCIInfo;
    PDSCH_PDU *p_PdschPdu;
    PDSCH_PTRS *p_PdschPtrs;
    PDSCH_TxPowerInfo *p_Pdsch_Txpower;
    PDSCH_Maintenance_FAPIv3 *p_PdschPduV3;
    PDSCH_PTRS_Maintenance_FAPIv3 *p_PdschPtrsV3;
    Rel16_PDSCH_FAPIv3 *p_PdschRelV3;
    PDSCH_CBGReTxCtlInfo *p_PDSCH_CBGReTxCtlInfo;
    PDSCH_CBGTxInfo *p_PDSCH_CBGTxInfo;
    PDSCH_PDU_FAPIv4 *p_PdschPduV4;
    CSI_RS_PDU *p_CsirsPdu;
    CSI_RS_PDU_MaintenanceParam_FAPIv3 *p_CsirsPduV3;
    CSI_RS_PDU_FAPIv4 *p_CsirsPduV4;
    Tx_Generalized_Precoding_PDU *p_Tx_Generalized_Precoding_PDU;
    Tx_Generalized_Precoding_MuMimoGroupInfo *p_Tx_Generalized_Precoding_MuMimoGroupInfo;
    uint16_t *p_Value_U16;

    FAPI_Output( "    type            : MSG_PHY_DL_TTI_REQ(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_DlTtiReq = (DL_TTI_REQ *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_DlTtiReq->sfn );
    FAPI_Output( "    slot            : %u\n", p_DlTtiReq->slot );
    FAPI_Output( "    numPDUs         : %u\n", p_DlTtiReq->numPDUs );
    FAPI_Output( "    nDlTypes        : %u\n", p_DlTtiReq->nDlTypes );
    FAPI_Output( "    nPDUsOfEachType : PDCCH %u PDSCH %u CSI-RS %u SSB %u DLDCI %u\n",
        p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDCCH],
        p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_PDSCH],
        p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_CSI_RS],
        p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_SSB],
        p_DlTtiReq->nPDUsOfEachType[DL_PDU_TYPE_INDEX_DLDCI] );

    pduCount = p_DlTtiReq->numPDUs;
    p_pduHeader = (PDU_Struct *) p_DlTtiReq->variablePart;
    for (pduIndex = 0; pduIndex < pduCount; pduIndex++)
    {
        if (((uint8_t *)p_pduHeader - (uint8_t *)p_DlTtiReq)+p_pduHeader->pduSize > p_FapiGenericHeader->length)
        {
            FAPI_Output( "    Length overflowed. pduType:%u [%u %u %u]\n", p_pduHeader->pduType, 
                (uint32_t)((uint8_t *)p_pduHeader - (uint8_t *)p_DlTtiReq), p_pduHeader->pduSize, p_FapiGenericHeader->length );
            break;
        }

        switch (p_pduHeader->pduType)
        {
            case DL_PDU_TYPE_PDCCH :     
                FAPI_Output( "    PDU [%u/%u] : PDCCH Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_PdcchPdu = (PDCCH_PDU *)p_pduHeader;
                
                FAPI_Output( "      coresetBWPStart                : %u\n", p_PdcchPdu->coresetBWPStart );
                FAPI_Output( "      subcarrierSpacing              : %u\n", p_PdcchPdu->subcarrierSpacing );
                FAPI_Output( "      cyclicPrefix                   : %u\n", p_PdcchPdu->cyclicPrefix );
                FAPI_Output( "      startSymbolIndex               : %u\n", p_PdcchPdu->startSymbolIndex );
                FAPI_Output( "      durationSymbols                : %u\n", p_PdcchPdu->durationSymbols );
                FAPI_Output( "      freqDomainResource             : ");
                for(freqRbsize = 0; freqRbsize< MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE; freqRbsize++)
                {
                    FAPI_Output( "%u",p_PdcchPdu->freqDomainResource[freqRbsize]);
                }
                FAPI_Output("\n");
                
                FAPI_Output( "      cceRegMappingType              : %u\n", p_PdcchPdu->cceRegMappingType );
                FAPI_Output( "      regBundleSize                  : %u\n", p_PdcchPdu->regBundleSize );
                FAPI_Output( "      interleaverSize                : %u\n", p_PdcchPdu->interleaverSize );
                FAPI_Output( "      coreSetType                    : %u\n", p_PdcchPdu->coreSetType );
                FAPI_Output( "      shiftIndex                     : %u\n", p_PdcchPdu->shiftIndex );
                FAPI_Output( "      precoderGranularity            : %u\n", p_PdcchPdu->precoderGranularity );
                FAPI_Output( "      numDlDci                       : %u\n", p_PdcchPdu->numDlDci );

                p_DlDci1Pdu = (DL_DCI_PDU_part1*)p_PdcchPdu->variablePart;

                for (dlDciIndex = 0; dlDciIndex < p_PdcchPdu->numDlDci; dlDciIndex++)
                {
                    FAPI_Output( "      rnti                           : %u\n", p_DlDci1Pdu->rnti );
                    FAPI_Output( "      nIdPdcchData                   : %u\n", p_DlDci1Pdu->nIdPdcchData );
                    FAPI_Output( "      nRntiPdcchData                 : %u\n", p_DlDci1Pdu->nRntiPdcchData);
                    FAPI_Output( "      cceIndex                       : %u\n", p_DlDci1Pdu->cceIndex);
                    FAPI_Output( "      aggregationLevel               : %u\n", p_DlDci1Pdu->aggregationLevel);

                    p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *) p_DlDci1Pdu->variablePart;
                    FAPI_Output( "      trpScheme                      : %u\n", p_TxPrecodingBfPdu->trpScheme );
                    FAPI_Output( "      numPRGs                        : %u\n", p_TxPrecodingBfPdu->numPRGs );
                    FAPI_Output( "      prgSize                        : %u\n", p_TxPrecodingBfPdu->prgSize );
                    FAPI_Output( "      digBFInterfaces                : %u\n", p_TxPrecodingBfPdu->digBFInterfaces );

                    p_DlDci2Pdu = (DL_DCI_PDU_part2 *) p_TxPrecodingBfPdu->variablePart;

                    FAPI_Output( "      beta_PDCCH_1_0                 : %u\n", p_DlDci2Pdu->beta_PDCCH_1_0 );
                    FAPI_Output( "      powerControlOffsetSSProfileNR  : %u\n", p_DlDci2Pdu->powerControlOffsetSSProfileNR );
                    FAPI_Output( "      payloadSizeBits                : %u\n", p_DlDci2Pdu->payloadSizeBits );
                    FAPI_Output( "      payload                        :");
                    for(dciPayloadcnt = 0; dciPayloadcnt <DCI_PAYLOAD_BTYE_LEN ;dciPayloadcnt++)
                    {
                        FAPI_Output( "%u", p_DlDci2Pdu->payload[dciPayloadcnt]);
                    }
                    FAPI_Output("\n");

                    p_DlDci1Pdu = (DL_DCI_PDU_part1*) (p_DlDci2Pdu+1);
                }

                p_PdcchPduV3 = (PDCCH_PDU_Maintenance_FAPIv3 *) p_DlDci1Pdu;
                FAPI_Output( "      pdcchPduIndex                  : %u\n", p_PdcchPduV3->pdcchPduIndex );

                p_PdcchPduV3_Dci = (PDCCH_PDU_Maintenance_FAPIv3_DCIInfo *) p_PdcchPduV3->variablePart;
                for (dlDciIndex = 0; dlDciIndex < p_PdcchPdu->numDlDci; dlDciIndex++)
                {
                    FAPI_Output( "      dciIndex                       : %u\n", p_PdcchPduV3_Dci->dciIndex );
                    FAPI_Output( "      collocatedAl16Candidate        : %u\n", p_PdcchPduV3_Dci->collocatedAl16Candidate );
                    FAPI_Output( "      pdcchDmrsPowerOffsetProfileSSS : %u\n", p_PdcchPduV3_Dci->pdcchDmrsPowerOffsetProfileSSS );
                    FAPI_Output( "      pdcchDataPowerOffsetProfileSSS : %u\n", p_PdcchPduV3_Dci->pdcchDataPowerOffsetProfileSSS );

                    p_PdcchPduV3_Dci++;
                }

                p_PdcchPduV4_Dci = (PDCCH_PDU_FAPIv4_DCIInfo *) p_PdcchPduV3_Dci;
 
                for (dlDciIndex = 0; dlDciIndex < p_PdcchPdu->numDlDci; dlDciIndex++)
                {
                    FAPI_Output( "      nIdPdcchDmrs                   : %u\n", p_PdcchPduV4_Dci->nIdPdcchDmrs );

                    p_PdcchPduV4_Dci++;
                }

                p_PdcchPduV4_MuMimo = (PDCCH_PDU_FAPIv4_MuMimo *) p_PdcchPduV4_Dci;

                FAPI_Output( "      spatialStreamIndicesPresent    : %u\n", p_PdcchPduV4_MuMimo->spatialStreamIndicesPresent );

                p_PdcchPduV4_MuMimo_DCIInfo = (PDCCH_PDU_FAPIv4_MuMimo_DCIInfo *) (p_PdcchPduV4_MuMimo+1);
                for (dlDciIndex = 0; dlDciIndex < p_PdcchPdu->numDlDci; dlDciIndex++)
                {
                    FAPI_Output( "      dciIndex                       : %u\n", p_PdcchPduV4_MuMimo_DCIInfo->dciIndex );
                    FAPI_Output( "      spatialStreamIndex             : %u\n", p_PdcchPduV4_MuMimo_DCIInfo->spatialStreamIndex );

                    p_PdcchPduV4_MuMimo_DCIInfo++;
                }
                break;

            case DL_PDU_TYPE_PDSCH :     
                FAPI_Output( "    PDU [%u/%u] : PDSCH Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_PdschPdu = (PDSCH_PDU *)p_pduHeader;

                FAPI_Output( "      pduBitmap                      : %u\n", p_PdschPdu->pduBitmap );
                FAPI_Output( "      rnti                           : %u\n", p_PdschPdu->rnti );
                FAPI_Output( "      pduIndex                       : %u\n", p_PdschPdu->pduIndex );
                FAPI_Output( "      bwpSize                        : %u\n", p_PdschPdu->bwpSize );
                FAPI_Output( "      bwpStart                       : %u\n", p_PdschPdu->bwpStart);
                FAPI_Output( "      subcarrierSpacing              : %u\n", p_PdschPdu->subcarrierSpacing );
                FAPI_Output( "      cyclicPrefix                   : %u\n", p_PdschPdu->cyclicPrefix);
                FAPI_Output( "      nrOfCodewords                  : %u\n", p_PdschPdu->nrOfCodewords);

                for(numOfcodewords = 0 ; numOfcodewords < p_PdschPdu->nrOfCodewords; numOfcodewords++)
                {
                    FAPI_Output( "      targetCodeRate[%u]              : %u\n", numOfcodewords, p_PdschPdu->codewordInfo[numOfcodewords].targetCodeRate );
                    FAPI_Output( "      qamModOrder[%u]                 : %u\n", numOfcodewords, p_PdschPdu->codewordInfo[numOfcodewords].qamModOrder );
                    FAPI_Output( "      mcsIndex[%u]                    : %u\n", numOfcodewords, p_PdschPdu->codewordInfo[numOfcodewords].mcsIndex );
                    FAPI_Output( "      mcsTable[%u]                    : %u\n", numOfcodewords, p_PdschPdu->codewordInfo[numOfcodewords].mcsTable );
                    FAPI_Output( "      rvIndex[%u]                     : %u\n", numOfcodewords, p_PdschPdu->codewordInfo[numOfcodewords].rvIndex );
                    FAPI_Output( "      tbSize[%u]                      : %u\n", numOfcodewords, p_PdschPdu->codewordInfo[numOfcodewords].tbSize );
                }

                FAPI_Output( "      nIdPdsch                       : %u\n", p_PdschPdu->nIdPdsch);
                FAPI_Output( "      nrOfLayers                     : %u\n", p_PdschPdu->nrOfLayers);
                FAPI_Output( "      transmissionScheme             : %u\n", p_PdschPdu->transmissionScheme);
                FAPI_Output( "      refPoint                       : %u\n", p_PdschPdu->refPoint);
                FAPI_Output( "      dlDmrsSymbPos                  : %u\n", p_PdschPdu->dlDmrsSymbPos);
                FAPI_Output( "      dmrsConfigType                 : %u\n", p_PdschPdu->dmrsConfigType);
                FAPI_Output( "      pdschDmrsScramblingId          : %u\n", p_PdschPdu->pdschDmrsScramblingId);
                FAPI_Output( "      pdschDmrsScramblingIdComplement: %u\n", p_PdschPdu->pdschDmrsScramblingIdComplement);
                FAPI_Output( "      lowPaprDmrs                    : %u\n", p_PdschPdu->lowPaprDmrs);
                FAPI_Output( "      nSCID                          : %u\n", p_PdschPdu->nSCID);
                FAPI_Output( "      numDmrsCdmGrpsNoData           : %u\n", p_PdschPdu->numDmrsCdmGrpsNoData);
                FAPI_Output( "      dmrsPorts                      : %u\n", p_PdschPdu->dmrsPorts);
                FAPI_Output( "      resourceAlloc                  : %u\n", p_PdschPdu->resourceAlloc);
                FAPI_Output( "      rbBitmap                       : ");
                for(rbBitmapsize =0; rbBitmapsize <MAX_RB_BITMAP_SIZE ;rbBitmapsize++)
                {
                    FAPI_Output( "%u", p_PdschPdu->rbBitmap[rbBitmapsize]);
                }
                FAPI_Output("\n");
                
                FAPI_Output( "      rbStart                        : %u\n", p_PdschPdu->rbStart);
                FAPI_Output( "      rbSize                         : %u\n", p_PdschPdu->rbSize);
                FAPI_Output( "      vrbToPrbMapping                : %u\n", p_PdschPdu->vrbToPrbMapping);
                FAPI_Output( "      startSymbolIndex               : %u\n", p_PdschPdu->startSymbolIndex);
                FAPI_Output( "      nrOfSymbols                    : %u\n", p_PdschPdu->nrOfSymbols);
                
                p_PdschPtrs = (PDSCH_PTRS *) p_PdschPdu->variablePart;

                if ((p_PdschPdu->pduBitmap & 0x01))
                {
                    FAPI_Output( "      ptrsPortIndex                  : %u\n", p_PdschPtrs->ptrsPortIndex);
                    FAPI_Output( "      ptrsTimeDensity                : %u\n", p_PdschPtrs->ptrsTimeDensity);
                    FAPI_Output( "      ptrsFreqDensity                : %u\n", p_PdschPtrs->ptrsFreqDensity);
                    FAPI_Output( "      ptrsReOffset                   : %u\n", p_PdschPtrs->ptrsReOffset);
                    FAPI_Output( "      nEpreRatioOfPDSCHToPTRSProfileNR   : %u\n", p_PdschPtrs->nEpreRatioOfPDSCHToPTRSProfileNR);

                    p_PdschPtrs++;
                }
                
                p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *) p_PdschPtrs;

                FAPI_Output( "      trpScheme                 : %u\n", p_TxPrecodingBfPdu->trpScheme );
                if (p_TxPrecodingBfPdu->trpScheme == TRP_SCHEME_BASE)
                {
                    FAPI_Output( "      numPRGs                   : %u\n", p_TxPrecodingBfPdu->numPRGs );
                    FAPI_Output( "      prgSize                   : %u\n", p_TxPrecodingBfPdu->prgSize );
                    FAPI_Output( "      digBFInterfaces           : %u\n", p_TxPrecodingBfPdu->digBFInterfaces );

                    numPRGs = p_TxPrecodingBfPdu->numPRGs;
                    digBFInterfaces = p_TxPrecodingBfPdu->digBFInterfaces;

                    p_Value_U16 = (uint16_t *) p_TxPrecodingBfPdu->variablePart;

                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        FAPI_Output( "      ForNumPRGs [%u] :\n", prgIndex );
                        FAPI_Output( "        pmIdx                   : %u\n", *p_Value_U16 );
                        p_Value_U16++;

                        for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                        {
                            FAPI_Output( "        ForDigBFInterfaces [%u] :\n", digBFInterfacesIndex );

                            FAPI_Output( "          beamIdx               : %u\n", *p_Value_U16 );
                            p_Value_U16++;
                        }
                    }

                    p_Pdsch_Txpower = (PDSCH_TxPowerInfo *) p_Value_U16;
                }
                else
                {
                    p_Rel16mTRP_TxPrecodingBfPdu = (Rel16mTRP_TxPrecodingAndBeamforming_PDU *) p_TxPrecodingBfPdu;

                    if (p_Rel16mTRP_TxPrecodingBfPdu->trpScheme == TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY)
                        numTrp = 2;
                    else    // TRP_SCHEME_STRP/TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY/TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY
                        numTrp = 1;

                    FAPI_Output( "      numPRGs                   : %u\n", p_Rel16mTRP_TxPrecodingBfPdu->numPRGs );
                    FAPI_Output( "      prgSize                   : %u\n", p_Rel16mTRP_TxPrecodingBfPdu->prgSize );
                    FAPI_Output( "      digBFInterfaces           :" );
                    for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                    {
                        FAPI_Output( " [%u]:%u", trpIndex, p_Rel16mTRP_TxPrecodingBfPdu->digBFInterfaces[trpIndex] );
                    }
                    FAPI_Output( "\n" );

                    numPRGs = p_Rel16mTRP_TxPrecodingBfPdu->numPRGs;
                    p_Value_U16 = (uint16_t *) p_Rel16mTRP_TxPrecodingBfPdu->variablePart;

                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo = (Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *) p_Value_U16;

                        FAPI_Output( "      ForNumPRGs[%u] :\n", prgIndex );
                        FAPI_Output( "        layers                  : %u\n", p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->layers );
                        FAPI_Output( "        pmIdx                   :" );
                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            FAPI_Output( " [%u]:%u", trpIndex, p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->pmIdx[prgIndex] );
                        }
                        FAPI_Output( "\n" );

                        p_Value_U16 = (uint16_t *) p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->variablePart;

                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            FAPI_Output( "        ForNumTrp[%u] :\n", trpIndex );

                            digBFInterfaces = p_Rel16mTRP_TxPrecodingBfPdu->digBFInterfaces[trpIndex];
                            for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                            {
                                FAPI_Output( "          ForDigBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                FAPI_Output( "            beamIdx             : %u\n", *p_Value_U16 );
                                p_Value_U16++;
                            }
                        }
                    }

                    p_Pdsch_Txpower = (PDSCH_TxPowerInfo *) p_Value_U16;
                }

                FAPI_Output( "      powerControlOffsetProfileNR    : %u\n", p_Pdsch_Txpower->powerControlOffsetProfileNR);
                FAPI_Output( "      powerControlOffsetSSProfileNR  : %u\n", p_Pdsch_Txpower->powerControlOffsetSSProfileNR);
 
                p_PDSCH_CBGReTxCtlInfo = (PDSCH_CBGReTxCtlInfo *) (p_Pdsch_Txpower+1);
           
                if ((p_PdschPdu->pduBitmap & 0x02))
                {
                    FAPI_Output( "      isLastCbPresent                : %u\n", p_PDSCH_CBGReTxCtlInfo->isLastCbPresent );
                    FAPI_Output( "      isInlineTbCrcl                 : %u\n", p_PDSCH_CBGReTxCtlInfo->isInlineTbCrcl );
                    FAPI_Output( "      dlTbCrcCW                      : %u %u\n", p_PDSCH_CBGReTxCtlInfo->dlTbCrcCW[0], p_PDSCH_CBGReTxCtlInfo->dlTbCrcCW[1] );

                    p_PDSCH_CBGReTxCtlInfo++;
                }

                p_PdschPduV3 = (PDSCH_Maintenance_FAPIv3 *) p_PDSCH_CBGReTxCtlInfo;
                FAPI_Output( "      pdschTransType                 : %u\n", p_PdschPduV3->pdschTransType);
                FAPI_Output( "      coresetStartPoint              : %u\n", p_PdschPduV3->coresetStartPoint);
                FAPI_Output( "      initialDlBwpSize               : %u\n", p_PdschPduV3->initialDlBwpSize);
                FAPI_Output( "      ldpcBaseGraph                  : %u\n", p_PdschPduV3->ldpcBaseGraph);
                FAPI_Output( "      tbSizeLbrmBytes                : %u\n", p_PdschPduV3->tbSizeLbrmBytes);
                FAPI_Output( "      tbCrcRequired                  : %u\n", p_PdschPduV3->tbCrcRequired);
                FAPI_Output( "      ssbPdusForRateMatching         :");
                for(pdschSsbRmInfo = 0; pdschSsbRmInfo < SSB_PDUS_FOR_RATE_MATCHING_SIZE;pdschSsbRmInfo++)
                {
                    FAPI_Output(" %u",p_PdschPduV3->ssbPdusForRateMatching[pdschSsbRmInfo]);
                }
                FAPI_Output("\n");

                FAPI_Output( "      ssbConfigForRateMatching       : %u\n", p_PdschPduV3->ssbConfigForRateMatching);
                FAPI_Output( "      prbSymRmPatternBitmapSizeByReference    : %u\n", p_PdschPduV3->prbSymRmPatternBitmapSizeByReference);
                FAPI_Output( "      prbSymRateMatchPatternBitmapByReference :");
                for(szRm =0; szRm < PDSCH_RATE_MATCHING_BITMAP_SIZE; szRm++)
                {
                    FAPI_Output( " %u", p_PdschPduV3->prbSymRateMatchPatternBitmapByReference[szRm]);
                }
                FAPI_Output("\n");
                FAPI_Output( "      numPrbSymRmPatternsByValue    : %u\n", p_PdschPduV3->numPrbSymRmPatternsByValue);
                    
                for(maxPdschRmPatterncnt =0; maxPdschRmPatterncnt<ceil((float)p_PdschPduV3->numPrbSymRmPatternsByValue/8) ; maxPdschRmPatterncnt++)
                {
                    FAPI_Output( "      prbSymRmPatternsInfo[%u] freqDomainRB  : ", maxPdschRmPatterncnt);
                    for(freqRbsize =0 ; freqRbsize < MAX_RATEMATCH_BITMAP_SIZE; freqRbsize++)
                    {
                        FAPI_Output( "%u",  p_PdschPduV3->prbSymRmPatternsInfo[maxPdschRmPatterncnt].freqDomainRB[freqRbsize]);
                    }
                    FAPI_Output("\n");
                    
                    FAPI_Output( "      prbSymRmPatternsInfo[%u] symbolsInRB   : %u\n",maxPdschRmPatterncnt ,p_PdschPduV3->prbSymRmPatternsInfo[maxPdschRmPatterncnt].symbolsInRB);
                }
                FAPI_Output( "      numCoresetRmPatterns    : %u\n", p_PdschPduV3->numCoresetRmPatterns);
                   
                for(maxPdschCoresetRmPatterncnt = 0; maxPdschCoresetRmPatterncnt< p_PdschPduV3->numCoresetRmPatterns; maxPdschCoresetRmPatterncnt++)
                {
                    FAPI_Output( "      coresetRmPatternsInfo[%u] freqDomainResources  : ", maxPdschCoresetRmPatterncnt);
                    for(freqDomainResource = 0; freqDomainResource <MAX_PDSCH_CORESET_FREQDOMAIN_RESOURCE ;freqDomainResource++)
                    {
                       FAPI_Output( "%u", p_PdschPduV3->coresetRmPatternsInfo[maxPdschCoresetRmPatterncnt].freqDomainResources[freqDomainResource]);
                    }
                    FAPI_Output("\n");
                    FAPI_Output( "      coresetRmPatternsInfo[%u] symbolsPattern  : %u\n",
                        maxPdschCoresetRmPatterncnt , p_PdschPduV3->coresetRmPatternsInfo[maxPdschCoresetRmPatterncnt].symbolsPattern);
                }
                FAPI_Output( "      pdcchPduIndex                            : %u\n", p_PdschPduV3->pdcchPduIndex);
                FAPI_Output( "      dciIndex                                 : %u\n", p_PdschPduV3->dciIndex);
                FAPI_Output( "      lteCrsRateMatchPatternBitmapSize         : %u\n", p_PdschPduV3->lteCrsRateMatchPatternBitmapSize);
                
                for(szCrs = 0; szCrs <p_PdschPduV3->lteCrsRateMatchPatternBitmapSize; szCrs++)
                {
                    FAPI_Output( "      lteCrsRateMatchPattern[%u]                : %u\n",szCrs,p_PdschPduV3->lteCrsRateMatchPattern[szCrs]);
                }
                FAPI_Output( "      numCsiRsForRateMatching                  : %u\n", p_PdschPduV3->numCsiRsForRateMatching);
                
                for(szCsirs = 0; szCsirs <p_PdschPduV3->numCsiRsForRateMatching; szCsirs++)
                {
                    FAPI_Output( "      csiRsForRateMatching[%u]                  : %u\n", szCsirs,p_PdschPduV3->csiRsForRateMatching[szCsirs]);
                }

                FAPI_Output( "      pdschDmrsPowerOffsetProfileSSS           : %u\n", p_PdschPduV3->pdschDmrsPowerOffsetProfileSSS);
                FAPI_Output( "      pdschDataPowerOffsetProfileSSS           : %u\n", p_PdschPduV3->pdschDataPowerOffsetProfileSSS);
                
                p_PDSCH_CBGTxInfo = (PDSCH_CBGTxInfo *) (p_PdschPduV3+1);

                if ((p_PdschPdu->pduBitmap & 0x02))
                {
                    FAPI_Output( "      maxNumCbgPerTb                           : %u\n", p_PDSCH_CBGTxInfo->maxNumCbgPerTb );
                    FAPI_Output( "      cbgTxInformation                         : %u %u\n", 
                        p_PDSCH_CBGTxInfo->cbgTxInformation[0], p_PDSCH_CBGTxInfo->cbgTxInformation[1] );
  
                    p_PDSCH_CBGTxInfo++;
                }

                p_PdschPtrsV3 = (PDSCH_PTRS_Maintenance_FAPIv3 *) p_PDSCH_CBGTxInfo;

                if ((p_PdschPdu->pduBitmap & 0x01))
                {
                    FAPI_Output( "      pdschPtrsPowerOffsetProfileSSS           : %u\n", p_PdschPtrsV3->pdschPtrsPowerOffsetProfileSSS);

                    p_PdschPtrsV3++;
                }

                p_PdschRelV3 = (Rel16_PDSCH_FAPIv3 *) p_PdschPtrsV3;
                
                FAPI_Output( "      repetitionScheme                         : %u\n", p_PdschRelV3->repetitionScheme );

                p_PdschPtrs = (PDSCH_PTRS *) (p_PdschRelV3+1);

                if ((p_PdschPdu->pduBitmap & 0x01))
                {
                    FAPI_Output( "      ptrsPortIndex                            : %u\n", p_PdschPtrs->ptrsPortIndex );
                    FAPI_Output( "      ptrsTimeDensity                          : %u\n", p_PdschPtrs->ptrsTimeDensity );
                    FAPI_Output( "      ptrsFreqDensity                          : %u\n", p_PdschPtrs->ptrsFreqDensity );
                    FAPI_Output( "      ptrsReOffset                             : %u\n", p_PdschPtrs->ptrsReOffset );
                    FAPI_Output( "      nEpreRatioOfPDSCHToPTRSProfileNR         : %u\n", p_PdschPtrs->nEpreRatioOfPDSCHToPTRSProfileNR );

                    p_PdschPtrs++;
                }
                
                p_PdschPtrsV3 = (PDSCH_PTRS_Maintenance_FAPIv3 *) p_PdschPtrs;

                if ((p_PdschPdu->pduBitmap & 0x01))
                {
                    FAPI_Output( "      pdschPtrsPowerOffsetProfileSSS           : %u\n", p_PdschPtrsV3->pdschPtrsPowerOffsetProfileSSS);

                    p_PdschPtrsV3++;
                }

                p_PdschPduV4 = (PDSCH_PDU_FAPIv4 *) p_PdschPtrsV3;
                FAPI_Output( "      coresetRmPatternBitmapSizeByReference : %u\n", p_PdschPduV4->coresetRmPatternBitmapSizeByReference );
                FAPI_Output( "      coresetRmPatternBitmapByReference     :" );
                for(maxPdschCoresetRmPatterncnt = 0; maxPdschCoresetRmPatterncnt < MAX_SIZE_OF_RATE_MATCH_PATTERN; maxPdschCoresetRmPatterncnt++)
                {
                    FAPI_Output( " %u", p_PdschPduV4->coresetRmPatternBitmapByReference[maxPdschCoresetRmPatterncnt] );
                }
                FAPI_Output("\n");

                FAPI_Output( "      lteCrsMbsfnDerivationMethod           : %u\n", p_PdschPduV4->lteCrsMbsfnDerivationMethod );
                FAPI_Output( "      lteCrsMbsfnPattern                    :" );
                for(szCrs = 0; szCrs < MAX_SIZE_OF_CRS_RATE_MATCH_PATTERN_MBSFN; szCrs++)            
                {
                    FAPI_Output( " %u", p_PdschPduV4->lteCrsMbsfnPattern[szCrs] );
                }
                FAPI_Output("\n");

                FAPI_Output( "      spatialStreamIndicesPresent           : %u\n", p_PdschPduV4->spatialStreamIndicesPresent );

                for(numOfcodewords = 0 ; numOfcodewords < MAX_NUM_OF_CORDWORD; numOfcodewords++)
                {
                    FAPI_Output( "      [cw%u] numLayersForCW                  : %u\n", 
                        numOfcodewords, p_PdschPduV4->layerMappiongcwInfo[numOfcodewords].numLayersForCW );

                    for(layerIndex = 0 ; layerIndex < MAX_LAYERS_FOR_CW; layerIndex++)
                    {
                        FAPI_Output( "      [cw%u][layer%u] spatialStreamIndicesForCW : %u\n", 
                            numOfcodewords, layerIndex, p_PdschPduV4->layerMappiongcwInfo[numOfcodewords].spatialStreamIndicesForCW[layerIndex] );
                    }
                } 
                break;

            case DL_PDU_TYPE_CSI_RS :    
                FAPI_Output( "    PDU [%u/%u] : CSI-RS Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_CsirsPdu = (CSI_RS_PDU *) p_pduHeader;
                FAPI_Output( "      subcarrierSpacing              : %u\n", p_CsirsPdu->subcarrierSpacing);
                FAPI_Output( "      cyclicPrefix                   : %u\n", p_CsirsPdu->cyclicPrefix );
                FAPI_Output( "      startRB                        : %u\n", p_CsirsPdu->startRB );
                FAPI_Output( "      nrOfRBs                        : %u\n", p_CsirsPdu->nrOfRBs  );
                FAPI_Output( "      csiType                        : %u\n", p_CsirsPdu->csiType );
                FAPI_Output( "      row                            : %u\n", p_CsirsPdu->row );
                FAPI_Output( "      freqDomain                     : %u\n", p_CsirsPdu->freqDomain );
                FAPI_Output( "      symbL0                         : %u\n", p_CsirsPdu->symbL0 );
                FAPI_Output( "      symbL1                         : %u\n", p_CsirsPdu->symbL1 );
                FAPI_Output( "      cdmType                        : %u\n", p_CsirsPdu->cdmType );
                FAPI_Output( "      freqDensity                    : %u\n", p_CsirsPdu->freqDensity );
                FAPI_Output( "      scrambId                       : %u\n", p_CsirsPdu->scrambId );
                FAPI_Output( "      powerControlOffsetProfileNR    : %u\n", p_CsirsPdu->powerControlOffsetProfileNR );
                FAPI_Output( "      powerControlOffsetSSProfileNR  : %u\n", p_CsirsPdu->powerControlOffsetSSProfileNR );
              
                p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *) p_CsirsPdu->variablePart;

                FAPI_Output( "      trpScheme                 : %u\n", p_TxPrecodingBfPdu->trpScheme );
                if (p_TxPrecodingBfPdu->trpScheme == TRP_SCHEME_BASE)
                {
                    FAPI_Output( "      numPRGs                   : %u\n", p_TxPrecodingBfPdu->numPRGs );
                    FAPI_Output( "      prgSize                   : %u\n", p_TxPrecodingBfPdu->prgSize );
                    FAPI_Output( "      digBFInterfaces           : %u\n", p_TxPrecodingBfPdu->digBFInterfaces );

                    numPRGs = p_TxPrecodingBfPdu->numPRGs;
                    digBFInterfaces = p_TxPrecodingBfPdu->digBFInterfaces;

                    p_Value_U16 = (uint16_t *) p_TxPrecodingBfPdu->variablePart;

                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        FAPI_Output( "      ForNumPRGs [%u] :\n", prgIndex );
                        FAPI_Output( "        pmIdx                   : %u\n", *p_Value_U16 );
                        p_Value_U16++;

                        for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                        {
                            FAPI_Output( "        ForDigBFInterfaces [%u] :\n", digBFInterfacesIndex );

                            FAPI_Output( "          beamIdx               : %u\n", *p_Value_U16 );
                            p_Value_U16++;
                        }
                    }

                    p_CsirsPduV3 = (CSI_RS_PDU_MaintenanceParam_FAPIv3 *) p_Value_U16;
                }
                else
                {
                    p_Rel16mTRP_TxPrecodingBfPdu = (Rel16mTRP_TxPrecodingAndBeamforming_PDU *) p_TxPrecodingBfPdu;

                    if (p_Rel16mTRP_TxPrecodingBfPdu->trpScheme == TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY)
                        numTrp = 2;
                    else    // TRP_SCHEME_STRP/TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY/TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY
                        numTrp = 1;

                    FAPI_Output( "      numPRGs                   : %u\n", p_Rel16mTRP_TxPrecodingBfPdu->numPRGs );
                    FAPI_Output( "      prgSize                   : %u\n", p_Rel16mTRP_TxPrecodingBfPdu->prgSize );
                    FAPI_Output( "      digBFInterfaces           :" );
                    for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                    {
                        FAPI_Output( " [%u]:%u", trpIndex, p_Rel16mTRP_TxPrecodingBfPdu->digBFInterfaces[trpIndex] );
                    }
                    FAPI_Output( "\n" );

                    numPRGs = p_Rel16mTRP_TxPrecodingBfPdu->numPRGs;
                    p_Value_U16 = (uint16_t *) p_Rel16mTRP_TxPrecodingBfPdu->variablePart;

                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo = (Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *) p_Value_U16;

                        FAPI_Output( "      ForNumPRGs[%u] :\n", prgIndex );
                        FAPI_Output( "        layers                  : %u\n", p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->layers );
                        FAPI_Output( "        pmIdx                   :" );
                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            FAPI_Output( " [%u]:%u", trpIndex, p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->pmIdx[prgIndex] );
                        }
                        FAPI_Output( "\n" );

                        p_Value_U16 = (uint16_t *) p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->variablePart;

                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            FAPI_Output( "        ForNumTrp[%u] :\n", trpIndex );

                            digBFInterfaces = p_Rel16mTRP_TxPrecodingBfPdu->digBFInterfaces[trpIndex];
                            for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                            {
                                FAPI_Output( "          ForDigBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                FAPI_Output( "            beamIdx             : %u\n", *p_Value_U16 );
                                p_Value_U16++;
                            }
                        }
                    }

                    p_CsirsPduV3 = (CSI_RS_PDU_MaintenanceParam_FAPIv3 *) p_Value_U16;
                }
 
                if (g_phyFeature_FapiV4MuMimoGroupSupport)
                {
                    p_Tx_Generalized_Precoding_PDU = (Tx_Generalized_Precoding_PDU *) p_CsirsPduV3;

                    FAPI_Output( "      trpScheme                 : %u\n", p_Tx_Generalized_Precoding_PDU->trpScheme );
                    FAPI_Output( "      numMuMimoGroups           : %u\n", p_Tx_Generalized_Precoding_PDU->numMuMimoGroups );

                    p_Tx_Generalized_Precoding_MuMimoGroupInfo = (Tx_Generalized_Precoding_MuMimoGroupInfo *) p_Tx_Generalized_Precoding_PDU->variablePart;

                    numMuMimoGroups = p_Tx_Generalized_Precoding_PDU->numMuMimoGroups;
                    for (muMimoGroupIndex = 0; muMimoGroupIndex < numMuMimoGroups; muMimoGroupIndex++)
                    {
                        FAPI_Output( "      ForEach numMuMimoGroups[%u] :\n", muMimoGroupIndex );
                        FAPI_Output( "        muMimoGroupIdx            : %u\n", p_Tx_Generalized_Precoding_MuMimoGroupInfo->muMimoGroupIdx );

                        p_Tx_Generalized_Precoding_MuMimoGroupInfo++;
                    }

                    p_CsirsPduV3 = (CSI_RS_PDU_MaintenanceParam_FAPIv3 *) p_Tx_Generalized_Precoding_MuMimoGroupInfo;
                }

                FAPI_Output( "      csiRsPduIndex                   : %u\n", p_CsirsPduV3->csiRsPduIndex );
                FAPI_Output( "      csiRsPowerOffsetProfileSSS      : %d\n", p_CsirsPduV3->csiRsPowerOffsetProfileSSS);
              
                p_CsirsPduV4 = (CSI_RS_PDU_FAPIv4 *) (p_CsirsPduV3+1);

                FAPI_Output( "      spatialStreamIndicesPresent     : %u\n", p_CsirsPduV4->spatialStreamIndicesPresent );
                FAPI_Output( "      spatialStreamIndices            :" );
                for (arrayIndex = 0; arrayIndex < MAX_CDM_GROUP_COUNT; arrayIndex++)
                {
                    FAPI_Output( " [%u]:%u", arrayIndex, p_CsirsPduV4->spatialStreamIndices[arrayIndex] );
                }
                FAPI_Output( "\n" );

                break;

            case DL_PDU_TYPE_SSB :
                FAPI_Output( "    PDU [%u/%u] : SSB Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_SsbPdu = (SSBPBCH_PDU *) p_pduHeader;
                FAPI_Output( "      physCellId                : %u\n", p_SsbPdu->physCellId );
                FAPI_Output( "      betaPssProfileNR          : %u\n", p_SsbPdu->betaPssProfileNR );
                FAPI_Output( "      ssbBlockIndex             : %u\n", p_SsbPdu->ssbBlockIndex );
                FAPI_Output( "      ssbSubcarrierOffset       : %u\n", p_SsbPdu->ssbSubcarrierOffset );
                FAPI_Output( "      ssbOffsetPointA           : %u\n", p_SsbPdu->ssbOffsetPointA );
                FAPI_Output( "      bchPayloadFlag            : %u\n", p_SsbPdu->bchPayloadFlag );
                if (p_SsbPdu->bchPayloadFlag == BCH_PAYLOAD_MAC_GENERATES_FULL)
                {
                    p_MacGenMIBPdu = (MAC_GenMIB_PDU *) p_SsbPdu->variablePart;
                    FAPI_Output( "      bchPayload                : 0x%08x (MAC Generated Full)\n", p_MacGenMIBPdu->bchPayload );

                    p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *) (p_MacGenMIBPdu+1);
                }
                else if (p_SsbPdu->bchPayloadFlag == BCH_PAYLOAD_PHY_GENERATES_TIMING)
                {
                    p_MacGenMIBPdu = (MAC_GenMIB_PDU *) p_SsbPdu->variablePart;
                    FAPI_Output( "      bchPayload                : 0x%08x (MAC Generated Timing)\n", p_MacGenMIBPdu->bchPayload );

                    p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *) (p_MacGenMIBPdu+1);
                }
                else if (p_SsbPdu->bchPayloadFlag == BCH_PAYLOAD_PHY_GENERATES_FULL)
                {
                    p_PHY_GenMIB_PDU = (PHY_GenMIB_PDU *) p_SsbPdu->variablePart;
                    FAPI_Output( "      PHY_GenMIB_PDU            :\n" );
                    FAPI_Output( "        bchPayload              : %u\n", p_PHY_GenMIB_PDU->dmrsTypeAPosition );
                    FAPI_Output( "        pdcchConfigSib1         : %u\n", p_PHY_GenMIB_PDU->pdcchConfigSib1 );
                    FAPI_Output( "        cellBarred              : %u\n", p_PHY_GenMIB_PDU->cellBarred );
                    FAPI_Output( "        intraFreqReselection    : %u\n", p_PHY_GenMIB_PDU->intraFreqReselection );

                    p_TxPrecodingBfPdu = (TxPrecodingAndBeamforming_PDU *) (p_PHY_GenMIB_PDU+1);
                }
                else 
                {
                    FAPI_Error( "      Invalid bchPayloadFlag value %u\n", p_SsbPdu->bchPayloadFlag );
                    return;
                }

                FAPI_Output( "      trpScheme                 : %u\n", p_TxPrecodingBfPdu->trpScheme );
                if (p_TxPrecodingBfPdu->trpScheme == TRP_SCHEME_BASE)
                {
                    FAPI_Output( "      numPRGs                   : %u\n", p_TxPrecodingBfPdu->numPRGs );
                    FAPI_Output( "      prgSize                   : %u\n", p_TxPrecodingBfPdu->prgSize );
                    FAPI_Output( "      digBFInterfaces           : %u\n", p_TxPrecodingBfPdu->digBFInterfaces );

                    numPRGs = p_TxPrecodingBfPdu->numPRGs;
                    digBFInterfaces = p_TxPrecodingBfPdu->digBFInterfaces;

                    p_Value_U16 = (uint16_t *) p_TxPrecodingBfPdu->variablePart;

                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        FAPI_Output( "      ForNumPRGs [%u] :\n", prgIndex );
                        FAPI_Output( "        pmIdx                   : %u\n", *p_Value_U16 );
                        p_Value_U16++;

                        for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                        {
                            FAPI_Output( "        ForDigBFInterfaces [%u] :\n", digBFInterfacesIndex );

                            FAPI_Output( "          beamIdx               : %u\n", *p_Value_U16 );
                            p_Value_U16++;
                        }
                    }

                    p_SsbPduV3 = (SSBPBCH_PDU_Maintenance_FAPIv3 *) p_Value_U16;
                }
                else
                {
                    p_Rel16mTRP_TxPrecodingBfPdu = (Rel16mTRP_TxPrecodingAndBeamforming_PDU *) p_TxPrecodingBfPdu;

                    if (p_Rel16mTRP_TxPrecodingBfPdu->trpScheme == TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY)
                        numTrp = 2;
                    else    // TRP_SCHEME_STRP/TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY/TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY
                        numTrp = 1;

                    FAPI_Output( "      numPRGs                   : %u\n", p_Rel16mTRP_TxPrecodingBfPdu->numPRGs );
                    FAPI_Output( "      prgSize                   : %u\n", p_Rel16mTRP_TxPrecodingBfPdu->prgSize );
                    FAPI_Output( "      digBFInterfaces           :" );
                    for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                    {
                        FAPI_Output( " [%u]:%u", trpIndex, p_Rel16mTRP_TxPrecodingBfPdu->digBFInterfaces[trpIndex] );
                    }
                    FAPI_Output( "\n" );

                    numPRGs = p_Rel16mTRP_TxPrecodingBfPdu->numPRGs;
                    p_Value_U16 = (uint16_t *) p_Rel16mTRP_TxPrecodingBfPdu->variablePart;

                    for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                    {
                        p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo = (Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo *) p_Value_U16;

                        FAPI_Output( "      ForNumPRGs[%u] :\n", prgIndex );
                        FAPI_Output( "        layers                  : %u\n", p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->layers );
                        FAPI_Output( "        pmIdx                   :" );
                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            FAPI_Output( " [%u]:%u", trpIndex, p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->pmIdx[prgIndex] );
                        }
                        FAPI_Output( "\n" );

                        p_Value_U16 = (uint16_t *) p_Rel16mTRP_TxPrecodingBfPdu_PrgInfo->variablePart;

                        for (trpIndex = 0; trpIndex < numTrp; trpIndex++)
                        {
                            FAPI_Output( "        ForNumTrp[%u] :\n", trpIndex );

                            digBFInterfaces = p_Rel16mTRP_TxPrecodingBfPdu->digBFInterfaces[trpIndex];
                            for (digBFInterfacesIndex = 0; digBFInterfacesIndex < digBFInterfaces; digBFInterfacesIndex++)
                            {
                                FAPI_Output( "          ForDigBFInterfaces[%u] :\n", digBFInterfacesIndex );

                                FAPI_Output( "            beamIdx             : %u\n", *p_Value_U16 );
                                p_Value_U16++;
                            }
                        }
                    }

                    p_SsbPduV3 = (SSBPBCH_PDU_Maintenance_FAPIv3 *) p_Value_U16;
                }

                FAPI_Output( "      ssbPduIndex               : %u\n", p_SsbPduV3->ssbPduIndex );
                FAPI_Output( "      ssbFirstSymbolMappingCase : %u\n", p_SsbPduV3->ssbFirstSymbolMappingCase );
                FAPI_Output( "      subcarrierSpacing         : %u\n", p_SsbPduV3->subcarrierSpacing );
                FAPI_Output( "      lMax                      : %u\n", p_SsbPduV3->lMax );
                FAPI_Output( "      ssPbchBlockPowerScaling   : %d\n", p_SsbPduV3->ssPbchBlockPowerScaling );
                FAPI_Output( "      betaPSSProfileSSS         : %d\n", p_SsbPduV3->betaPSSProfileSSS );

                p_SsbPduV4 = (SSBPBCH_PDU_FAPIv4*) (p_SsbPduV3+1);
                FAPI_Output( "      spatialStreamIndexPresent : %u\n", p_SsbPduV4->spatialStreamIndexPresent );
                FAPI_Output( "      spatialStreamIndex        : %u\n", p_SsbPduV4->spatialStreamIndex );
                break;

            default :
                break;
        }

        p_pduHeader = (PDU_Struct *) ((uint8_t *)p_pduHeader+p_pduHeader->pduSize);
    }
}

void phy_ParseMsg_TxDataReq( Fapi_Generic_Header *p_FapiGenericHeader )
{
    uint32_t pduIndex, pduCount, pduLength, tlvCount, tlvIndex, valueLength, valueIndex;
    Tx_DATA_REQ *p_TxDataReq;
    TxDataPduInfo *p_TxDataPduInfo;
    TBS_TLV_Struct *p_TBS_TLV_Struct;
    uint64_t *p_Value_U64;
    uint8_t *p_value;

    FAPI_Output( "    type            : MSG_PHY_TX_DATA_REQ(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_TxDataReq = (Tx_DATA_REQ *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_TxDataReq->sfn );
    FAPI_Output( "    slot            : %u\n", p_TxDataReq->slot );
    FAPI_Output( "    numPDUs         : %u\n", p_TxDataReq->numPDUs );

    pduCount = p_TxDataReq->numPDUs;
    p_TxDataPduInfo = (TxDataPduInfo *) p_TxDataReq->variablePart;
    for (pduIndex = 0; pduIndex < pduCount; pduIndex++)
    {
        FAPI_Output( "    ForEach numPDUs[%u] :\n", pduIndex );

        pduLength = p_TxDataPduInfo->pduLength;
        tlvCount = p_TxDataPduInfo->numTLV;

        FAPI_Output( "      pduLength     : %u\n", pduLength );
        FAPI_Output( "      numTLV        : %u\n", tlvCount );

        p_TBS_TLV_Struct = (TBS_TLV_Struct *) p_TxDataPduInfo->variablePart;
        for (tlvIndex = 0; tlvIndex < tlvCount; tlvIndex++)
        {
            FAPI_Output( "      ForEach numTLV[%u] :\n", tlvIndex );
            FAPI_Output( "        tag         : %u\n", p_TBS_TLV_Struct->tag );
            FAPI_Output( "        length      : %u\n", p_TBS_TLV_Struct->length );

            if (p_TBS_TLV_Struct->tag == VALUE_FIELD_PAYLOAD_VALUES)
            {
                valueLength = p_TBS_TLV_Struct->length;

                FAPI_Output( "        value       :" );
                for (valueIndex = 0; valueIndex < valueLength; valueIndex++)
                {
                    FAPI_Output( " %u", p_TBS_TLV_Struct->variablePart[valueIndex] );
                }
                FAPI_Output( "\n" );

                if (valueLength%4 != 0)
                    valueLength += 4-(valueLength%4);
            }
            else    // VALUE_FIELD_POINTER_TO_PAYLOAD
            {
                p_Value_U64 = (uint64_t *) p_TBS_TLV_Struct->variablePart;
                valueLength = sizeof(uint64_t);

                p_value = (uint8_t *) *p_Value_U64;

                FAPI_Output( "        value       :" );
                if (p_TBS_TLV_Struct->length < 41) 
                {
                    for (valueIndex = 0; valueIndex < p_TBS_TLV_Struct->length; valueIndex++)
                    {
                        FAPI_Output( " %u", p_value[valueIndex] );
                    }
                }
                else
                {
                    for (valueIndex = 0; valueIndex < 20; valueIndex++)
                    {
                        FAPI_Output( " %u", p_value[valueIndex] );
                    }

                    FAPI_Output( " ..." );

                    for (valueIndex = p_TBS_TLV_Struct->length-20; valueIndex < p_TBS_TLV_Struct->length; valueIndex++)
                    {
                        FAPI_Output( " %u", p_value[valueIndex] );
                    }
                }
                FAPI_Output( "\n" );
            }

            p_TBS_TLV_Struct = (TBS_TLV_Struct *) ((uint8_t *)(p_TBS_TLV_Struct+1) + valueLength);
        }

        p_TxDataPduInfo = (TxDataPduInfo *) ((uint8_t *)p_TxDataPduInfo + pduLength);
    }
}

void phy_ParseMsg_ULDCIReq( Fapi_Generic_Header *p_FapiGenericHeader )
{
    UL_DCI_REQ *p_ULDciReq;

    FAPI_Output( "    type            : MSG_PHY_UL_DCI_REQ(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_ULDciReq = (UL_DCI_REQ *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_ULDciReq->sfn );
    FAPI_Output( "    slot            : %u\n", p_ULDciReq->slot );
    FAPI_Output( "    numPDUs         : %u\n", p_ULDciReq->numPDUs );
    FAPI_Output( "    nDlTypes        : %u\n", p_ULDciReq->nDlTypes );
    FAPI_Output( "    nPDUsOfEachType : PDCCH %u ULDCI %u\n",
        p_ULDciReq->nPDUsOfEachType[ULDCI_PDU_TYPE_INDEX_PDCCH],
        p_ULDciReq->nPDUsOfEachType[ULDCI_PDU_TYPE_INDEX_ULDCI] );
}

void phy_ParseMsg_ULTTIReq( Fapi_Generic_Header *p_FapiGenericHeader )
{
    uint16_t pduIndex, pduCount;
    uint32_t numPRGs, prgIndex, digBFInterfaces, digBFInterfaceIndex, ulSpatialStreamIndex, arrayIndex;
    uint32_t part2sIndex, ptrsPortsIndex;
    UL_TTI_REQ *p_UlTtiReq;
    PDU_Struct *p_pduHeader;
    PRACH_PDU *p_PrachPdu;
    PUSCH_PDU *p_PuschPdu;
    Rx_Beamforming_PDU *p_Rx_Beamforming_PDU;
    PRACH_Maintenance_FAPIv3 *p_PRACH_Maintenance_FAPIv3;
    UplinkSpatialStreamAssignment_FAPIv4 *p_UplinkSpatialStreamAssignment_FAPIv4;
    MsgAPrachToMsgAPuschMap_FAPIv4 *p_MsgAPrachToMsgAPuschMap_FAPIv4;
    PuschDataInfo *p_PuschDataInfo;
    PuschUCIInfo *p_PuschUCIInfo;
    PuschPTRSInfo *p_PuschPTRSInfo;
    DftsOfdmInfo *p_DftsOfdmInfo;
    PUSCH_Maintenance_FAPIv3 *p_PUSCH_Maintenance_FAPIv3;
    PUSCH_FAPIv4 *p_PUSCH_FAPIv4;
    PuschUCIInfo_FAPIv3 *p_PuschUCIInfo_FAPIv3;
    UCIPart2Info_FAPIv3 *p_UCIPart2Info_FAPIv3;
    PUCCH_PDU *p_PucchPdu;
    PUCCH_PDU_Maintenance_FAPIv3 *p_PUCCH_PDU_Maintenance_FAPIv3;
    SRS_PDU *p_SrsPdu;
    SRS_PDU_FAPIv4 *p_SRS_PDU_FAPIv4;
    MSGA_PUSCH_PDU *p_MsgAPuschPdu;
    uint16_t *p_Value_U16;

    FAPI_Output( "    type            : MSG_PHY_UL_TTI_REQ(%u)\n", p_FapiGenericHeader->type );
    FAPI_Output( "    length          : %u\n", p_FapiGenericHeader->length );

    if (p_FapiGenericHeader->length == 0)
        return;

    p_UlTtiReq = (UL_TTI_REQ *) (p_FapiGenericHeader+1);

    FAPI_Output( "    sfn             : %u\n", p_UlTtiReq->sfn );
    FAPI_Output( "    slot            : %u\n", p_UlTtiReq->slot );
    FAPI_Output( "    numPDUs         : %u\n", p_UlTtiReq->numPDUs );
    FAPI_Output( "    nUlTypes        : %u\n", p_UlTtiReq->nUlTypes );
    FAPI_Output( "    nPDUsOfEachType : PRACH %u PUSCH %u PUCCH[Format0_1 %u Format2_3_4 %u] SRS %u MSGA_PUSCH %u\n",
        p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_PRACH],
        p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_PUSCH],
        p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_FORMAT01_PUCCH],
        p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_FORMAT234_PUCCH],
        p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_SRS],
        p_UlTtiReq->nPDUsOfEachType[UL_PDU_TYPE_INDEX_MSGA_PUSCH] );

    pduCount = p_UlTtiReq->numPDUs;
    p_pduHeader = (PDU_Struct *) p_UlTtiReq->variablePart;
    for (pduIndex = 0; pduIndex < pduCount; pduIndex++)
    {
        if (((uint8_t *)p_pduHeader - (uint8_t *)p_UlTtiReq)+p_pduHeader->pduSize > p_FapiGenericHeader->length)
        {
            FAPI_Output( "    Length overflowed. pduType:%u [%u %u %u]\n", p_pduHeader->pduType,
                (uint32_t)((uint8_t *)p_pduHeader - (uint8_t *)p_UlTtiReq), p_pduHeader->pduSize, p_FapiGenericHeader->length );
            break;
        }

        switch (p_pduHeader->pduType)
        {
            case UL_PDU_TYPE_PRACH :
                FAPI_Output( "    PDU [%u/%u] : PRACH Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_PrachPdu = (PRACH_PDU *)p_pduHeader;

                FAPI_Output( "      physCellID                     : %u\n", p_PrachPdu->physCellID );
                FAPI_Output( "      numPrachOcas                   : %u\n", p_PrachPdu->numPrachOcas );
                FAPI_Output( "      prachFormat                    : %u\n", p_PrachPdu->prachFormat );
                FAPI_Output( "      indexFdRa                      : %u\n", p_PrachPdu->indexFdRa );
                FAPI_Output( "      prachStartSymbol               : %u\n", p_PrachPdu->prachStartSymbol );
                FAPI_Output( "      numCs                          : %u\n", p_PrachPdu->numCs );
                FAPI_Output( "      isMsgAPrach                    : %u\n", p_PrachPdu->isMsgAPrach );
                FAPI_Output( "      hasMsgAPuschBeamforming        : %u\n", p_PrachPdu->hasMsgAPuschBeamforming );

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) p_PrachPdu->variablePart;
                FAPI_Output( "      rxBeamforming :\n" );
                FAPI_Output( "        trpScheme                    : %u\n", p_Rx_Beamforming_PDU->trpScheme );
                FAPI_Output( "        numPRGs                      : %u\n", p_Rx_Beamforming_PDU->numPRGs );
                FAPI_Output( "        prgSize                      : %u\n", p_Rx_Beamforming_PDU->prgSize );
                FAPI_Output( "        digBFInterface               : %u\n", p_Rx_Beamforming_PDU->digBFInterface );

                numPRGs = p_Rx_Beamforming_PDU->numPRGs;
                digBFInterfaces = p_Rx_Beamforming_PDU->digBFInterface;

                p_Value_U16 = (uint16_t *) p_Rx_Beamforming_PDU->variablePart;

                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    FAPI_Output( "        ForNumPRGs [%u] :\n", prgIndex );
                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterfaces; digBFInterfaceIndex++)
                    {
                        FAPI_Output( "        ForDigBFInterfaces [%u] :\n", digBFInterfaceIndex );
                        FAPI_Output( "          beamIdx                   : %u\n", *p_Value_U16 );
                        p_Value_U16++;
                    }
                }

                p_PRACH_Maintenance_FAPIv3 = (PRACH_Maintenance_FAPIv3 *) p_Value_U16;
                FAPI_Output( "      prachMaintenanceFapiV3 :\n" );
                FAPI_Output( "        handle                       : %u\n", p_PRACH_Maintenance_FAPIv3->handle );
                FAPI_Output( "        prachConfigScope             : %u\n", p_PRACH_Maintenance_FAPIv3->prachConfigScope );
                FAPI_Output( "        prachResConfigIndex          : %u\n", p_PRACH_Maintenance_FAPIv3->prachResConfigIndex );
                FAPI_Output( "        numFdRa                      : %u\n", p_PRACH_Maintenance_FAPIv3->numFdRa );
                FAPI_Output( "        startPreambleIndex           : %u\n", p_PRACH_Maintenance_FAPIv3->startPreambleIndex );
                FAPI_Output( "        numPreambleIndices           : %u\n", p_PRACH_Maintenance_FAPIv3->numPreambleIndices );

                p_UplinkSpatialStreamAssignment_FAPIv4 = (UplinkSpatialStreamAssignment_FAPIv4 *) (p_PRACH_Maintenance_FAPIv3+1);
                FAPI_Output( "      uplinkSpatialStreamAssignmentFapiV4 :\n" );
                FAPI_Output( "        numSpatialStreams            : %u\n", p_UplinkSpatialStreamAssignment_FAPIv4->numSpatialStreams );
                FAPI_Output( "        spatialStreamIndices         :" );
                for (ulSpatialStreamIndex = 0; ulSpatialStreamIndex < p_UplinkSpatialStreamAssignment_FAPIv4->numSpatialStreams; ulSpatialStreamIndex++)
                { 
                    FAPI_Output( " %u", p_UplinkSpatialStreamAssignment_FAPIv4->spatialStreamIndices[ulSpatialStreamIndex] );
                } 
                FAPI_Output( "\n" );

                if (p_PrachPdu->isMsgAPrach)
                {
                    p_MsgAPrachToMsgAPuschMap_FAPIv4 = (MsgAPrachToMsgAPuschMap_FAPIv4 *) (p_UplinkSpatialStreamAssignment_FAPIv4+1);

                    FAPI_Output( "      msgAPrachToMsgAPuschMapFapiV4 :\n" );
                    FAPI_Output( "        preambleGroups               : %u\n", p_MsgAPrachToMsgAPuschMap_FAPIv4->preambleGroups );
                    FAPI_Output( "        dmrsPorts                    :" );
                    for (arrayIndex = 0; arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_MsgAPrachToMsgAPuschMap_FAPIv4->dmrsPorts[arrayIndex] );
                    }
                    FAPI_Output( "\n" );
                    FAPI_Output( "        availableDmrsSequenceIds     :" );
                    for (arrayIndex = 0; arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_MsgAPrachToMsgAPuschMap_FAPIv4->availableDmrsSequenceIds[arrayIndex] );
                    }
                    FAPI_Output( "\n" );
                    FAPI_Output( "        numPuschOcasFd               :" );
                    for (arrayIndex = 0; arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_MsgAPrachToMsgAPuschMap_FAPIv4->numPuschOcasFd[arrayIndex] );
                    }
                    FAPI_Output( "\n" );
                    FAPI_Output( "        numPuschOcasTd               :" );
                    for (arrayIndex = 0; arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_MsgAPrachToMsgAPuschMap_FAPIv4->numPuschOcasTd[arrayIndex] );
                    }
                    FAPI_Output( "\n" );
                    FAPI_Output( "        numPuschOcasSlots            :" );
                    for (arrayIndex = 0; arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_MsgAPrachToMsgAPuschMap_FAPIv4->numPuschOcasSlots[arrayIndex] );
                    }
                    FAPI_Output( "\n" );
                    FAPI_Output( "        msgAPUSCHTimeDomainOffset    :" );
                    for (arrayIndex = 0; arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_MsgAPrachToMsgAPuschMap_FAPIv4->msgAPUSCHTimeDomainOffset[arrayIndex] );
                    }
                    FAPI_Output( "\n" );
                    FAPI_Output( "        nPreambles                   :" );
                    for (arrayIndex = 0; arrayIndex < MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_MsgAPrachToMsgAPuschMap_FAPIv4->nPreambles[arrayIndex] );
                    }
                    FAPI_Output( "\n" );
                    FAPI_Output( "        msgAPrachtoMapping           :\n" );    // TODO : Table 3-43

                    if (p_PrachPdu->hasMsgAPuschBeamforming)
                    {
                        p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) (p_MsgAPrachToMsgAPuschMap_FAPIv4+1);
                        FAPI_Output( "      rxBeamforming :\n" );
                        FAPI_Output( "        trpScheme                    : %u\n", p_Rx_Beamforming_PDU->trpScheme );
                        FAPI_Output( "        numPRGs                      : %u\n", p_Rx_Beamforming_PDU->numPRGs );
                        FAPI_Output( "        prgSize                      : %u\n", p_Rx_Beamforming_PDU->prgSize );
                        FAPI_Output( "        digBFInterface               : %u\n", p_Rx_Beamforming_PDU->digBFInterface );
                
                        numPRGs = p_Rx_Beamforming_PDU->numPRGs;
                        digBFInterfaces = p_Rx_Beamforming_PDU->digBFInterface;
                
                        p_Value_U16 = (uint16_t *) p_Rx_Beamforming_PDU->variablePart;
                
                        for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                        {
                            FAPI_Output( "        ForNumPRGs [%u] :\n", prgIndex );
                            for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterfaces; digBFInterfaceIndex++)
                            {
                                FAPI_Output( "        ForDigBFInterfaces [%u] :\n", digBFInterfaceIndex );
                                FAPI_Output( "          beamIdx                   : %u\n", *p_Value_U16 );
                                p_Value_U16++;
                            }
                        }
                    }
                }

                break;

            case UL_PDU_TYPE_PUSCH :
                FAPI_Output( "    PDU [%u/%u] : PUSCH Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_PuschPdu = (PUSCH_PDU *)p_pduHeader;

                FAPI_Output( "      pduBitmap                      : %u\n", p_PuschPdu->pduBitmap );
                FAPI_Output( "      rnti                           : %u\n", p_PuschPdu->rnti );
                FAPI_Output( "      handle                         : %u\n", p_PuschPdu->handle );
                FAPI_Output( "      bwpSize                        : %u\n", p_PuschPdu->bwpSize );
                FAPI_Output( "      bwpStart                       : %u\n", p_PuschPdu->bwpStart );
                FAPI_Output( "      subcarrierSpacing              : %u\n", p_PuschPdu->subcarrierSpacing );
                FAPI_Output( "      cyclicPrefix                   : %u\n", p_PuschPdu->cyclicPrefix );
                FAPI_Output( "      targetCodeRate                 : %u\n", p_PuschPdu->targetCodeRate );
                FAPI_Output( "      qamModOrder                    : %u\n", p_PuschPdu->qamModOrder );
                FAPI_Output( "      mcsIndex                       : %u\n", p_PuschPdu->mcsIndex );
                FAPI_Output( "      mcsTable                       : %u\n", p_PuschPdu->mcsTable );
                FAPI_Output( "      transformPrecoding             : %u\n", p_PuschPdu->transformPrecoding );
                FAPI_Output( "      nIdPusch                       : %u\n", p_PuschPdu->nIdPusch );
                FAPI_Output( "      nrOfLayers                     : %u\n", p_PuschPdu->nrOfLayers );
                FAPI_Output( "      ulDmrsSymbPos                  : %u\n", p_PuschPdu->ulDmrsSymbPos );
                FAPI_Output( "      dmrsConfigType                 : %u\n", p_PuschPdu->dmrsConfigType );
                FAPI_Output( "      puschDmrsScramblingId          : %u\n", p_PuschPdu->puschDmrsScramblingId );
                FAPI_Output( "      puschDmrsScramblingIdComplement : %u\n", p_PuschPdu->puschDmrsScramblingIdComplement );
                FAPI_Output( "      lowPaprDmrs                    : %u\n", p_PuschPdu->lowPaprDmrs );
                FAPI_Output( "      puschDmrsIdentity              : %u\n", p_PuschPdu->puschDmrsIdentity );
                FAPI_Output( "      nSCID                          : %u\n", p_PuschPdu->nSCID );
                FAPI_Output( "      numDmrsCdmGrpsNoData           : %u\n", p_PuschPdu->numDmrsCdmGrpsNoData );
                FAPI_Output( "      dmrsPorts                      : %u\n", p_PuschPdu->dmrsPorts );
                FAPI_Output( "      resourceAlloc                  : %u\n", p_PuschPdu->resourceAlloc );
                FAPI_Output( "      rbBitmap                       :" );
                for (arrayIndex = 0; arrayIndex < MAX_RB_BITMAP_SIZE; arrayIndex++)
                {
                    FAPI_Output( " %u", p_PuschPdu->rbBitmap[arrayIndex] );
                }
                FAPI_Output( "\n" );
                FAPI_Output( "      rbStart                        : %u\n", p_PuschPdu->rbStart );
                FAPI_Output( "      rbSize                         : %u\n", p_PuschPdu->rbSize );
                FAPI_Output( "      vrbToPrbMapping                : %u\n", p_PuschPdu->vrbToPrbMapping );
                FAPI_Output( "      intraSlotFrequencyHopping      : %u\n", p_PuschPdu->intraSlotFrequencyHopping );
                FAPI_Output( "      txDirectCurrentLocation        : %u\n", p_PuschPdu->txDirectCurrentLocation );
                FAPI_Output( "      uplinkFrequencyShift7p5khz     : %u\n", p_PuschPdu->uplinkFrequencyShift7p5khz );
                FAPI_Output( "      startSymbolIndex               : %u\n", p_PuschPdu->startSymbolIndex );
                FAPI_Output( "      nrOfSymbols                    : %u\n", p_PuschPdu->nrOfSymbols );

                p_PuschDataInfo = (PuschDataInfo *) (p_PuschPdu+1);
                if ((p_PuschPdu->pduBitmap & 0x01))
                {
                    FAPI_Output( "      puschData :\n" );
                    FAPI_Output( "        rvIndex                      : %u\n", p_PuschDataInfo->rvIndex );
                    FAPI_Output( "        harqProcessID                : %u\n", p_PuschDataInfo->harqProcessID );
                    FAPI_Output( "        newData                      : %u\n", p_PuschDataInfo->newData );
                    FAPI_Output( "        tbSize                       : %u\n", p_PuschDataInfo->tbSize );
                    FAPI_Output( "        numCb                        : %u\n", p_PuschDataInfo->numCb );
                    FAPI_Output( "        cbPresentAndPosition         :" );
                    for (arrayIndex = 0; arrayIndex < p_PuschDataInfo->numCb; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_PuschDataInfo->cbPresentAndPosition[arrayIndex] );
                    }
                    FAPI_Output( "\n" );

                    p_PuschDataInfo++;
                }

                p_PuschUCIInfo = (PuschUCIInfo *) p_PuschDataInfo;
                if ((p_PuschPdu->pduBitmap & 0x02))
                {
                    FAPI_Output( "      puschUci :\n" );
                    FAPI_Output( "        harqAckBitLength             : %u\n", p_PuschUCIInfo->harqAckBitLength );
                    FAPI_Output( "        csiPart1BitLength            : %u\n", p_PuschUCIInfo->csiPart1BitLength );
                    FAPI_Output( "        flagCsiPart2                 : %u\n", p_PuschUCIInfo->flagCsiPart2 );
                    FAPI_Output( "        alphaScaling                 : %u\n", p_PuschUCIInfo->alphaScaling );
                    FAPI_Output( "        betaOffsetHarqAck            : %u\n", p_PuschUCIInfo->betaOffsetHarqAck );
                    FAPI_Output( "        betaOffsetCsi1               : %u\n", p_PuschUCIInfo->betaOffsetCsi1 );
                    FAPI_Output( "        betaOffsetCsi2               : %u\n", p_PuschUCIInfo->betaOffsetCsi2 );

                    p_PuschUCIInfo++;
                }

                p_PuschPTRSInfo = (PuschPTRSInfo *) p_PuschUCIInfo;
                if ((p_PuschPdu->pduBitmap & 0x04))
                {
                    FAPI_Output( "      puschPtrs :\n" );
                    FAPI_Output( "        numPtrsPorts                 : %u\n", p_PuschPTRSInfo->numPtrsPorts );
                    for (ptrsPortsIndex = 0; ptrsPortsIndex < p_PuschPTRSInfo->numPtrsPorts; ptrsPortsIndex++)
                    {
                        FAPI_Output( "        ForEach numPtrsPorts[%u] :\n", ptrsPortsIndex );
                        FAPI_Output( "          ptrsPortIndex              : %u\n", p_PuschPTRSInfo->ptrsPortInfo[ptrsPortsIndex].ptrsPortIndex );
                        FAPI_Output( "          ptrsDmrsPort               : %u\n", p_PuschPTRSInfo->ptrsPortInfo[ptrsPortsIndex].ptrsDmrsPort );
                        FAPI_Output( "          ptrsReOffset               : %u\n", p_PuschPTRSInfo->ptrsPortInfo[ptrsPortsIndex].ptrsReOffset );
                    }
                    FAPI_Output( "        ptrsTimeDensity              : %u\n", p_PuschPTRSInfo->ptrsTimeDensity );
                    FAPI_Output( "        ptrsFreqDensity              : %u\n", p_PuschPTRSInfo->ptrsFreqDensity );
                    FAPI_Output( "        ulPTRSPower                  : %u\n", p_PuschPTRSInfo->ulPTRSPower );

                    p_PuschPTRSInfo++;
                }

                p_DftsOfdmInfo = (DftsOfdmInfo *) p_PuschPTRSInfo;
                if ((p_PuschPdu->pduBitmap & 0x08))
                {
                    FAPI_Output( "      dftsOfdm :\n" );
                    FAPI_Output( "        lowPaprGroupNumber           : %u\n", p_DftsOfdmInfo->lowPaprGroupNumber );
                    FAPI_Output( "        lowPaprSequenceNumber        : %u\n", p_DftsOfdmInfo->lowPaprSequenceNumber );
                    FAPI_Output( "        ulPtrsSampleDensity          : %u\n", p_DftsOfdmInfo->ulPtrsSampleDensity );
                    FAPI_Output( "        ulPtrsTimeDensityTransformPrecoding : %u\n", p_DftsOfdmInfo->ulPtrsTimeDensityTransformPrecoding );

                    p_DftsOfdmInfo++;
                }

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) p_DftsOfdmInfo;
                FAPI_Output( "      rxBeamforming :\n" );
                FAPI_Output( "        trpScheme                    : %u\n", p_Rx_Beamforming_PDU->trpScheme );
                FAPI_Output( "        numPRGs                      : %u\n", p_Rx_Beamforming_PDU->numPRGs );
                FAPI_Output( "        prgSize                      : %u\n", p_Rx_Beamforming_PDU->prgSize );
                FAPI_Output( "        digBFInterface               : %u\n", p_Rx_Beamforming_PDU->digBFInterface );

                numPRGs = p_Rx_Beamforming_PDU->numPRGs;
                digBFInterfaces = p_Rx_Beamforming_PDU->digBFInterface;

                p_Value_U16 = (uint16_t *) p_Rx_Beamforming_PDU->variablePart;

                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    FAPI_Output( "        ForNumPRGs [%u] :\n", prgIndex );
                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterfaces; digBFInterfaceIndex++)
                    {
                        FAPI_Output( "        ForDigBFInterfaces [%u] :\n", digBFInterfaceIndex );
                        FAPI_Output( "          beamIdx                   : %u\n", *p_Value_U16 );
                        p_Value_U16++;
                    }
                }

                p_PUSCH_Maintenance_FAPIv3 = (PUSCH_Maintenance_FAPIv3 *) p_Value_U16;
                FAPI_Output( "      puschMaintenanceFapiV3 :\n" );
                FAPI_Output( "        puschTransType               : %u\n", p_PUSCH_Maintenance_FAPIv3->puschTransType );
                FAPI_Output( "        deltaBwp0StartFromActiveBwp  : %u\n", p_PUSCH_Maintenance_FAPIv3->deltaBwp0StartFromActiveBwp );
                FAPI_Output( "        initialUlBwpSize             : %u\n", p_PUSCH_Maintenance_FAPIv3->initialUlBwpSize );
                FAPI_Output( "        groupOrSequenceHopping       : %u\n", p_PUSCH_Maintenance_FAPIv3->groupOrSequenceHopping );
                FAPI_Output( "        puschSecondHopPRB            : %u\n", p_PUSCH_Maintenance_FAPIv3->puschSecondHopPRB );
                FAPI_Output( "        ldpcBaseGraph                : %u\n", p_PUSCH_Maintenance_FAPIv3->ldpcBaseGraph );
                FAPI_Output( "        tbSizeLbrmBytes              : %u\n", p_PUSCH_Maintenance_FAPIv3->tbSizeLbrmBytes );

                p_PUSCH_FAPIv4 = (PUSCH_FAPIv4 *) (p_PUSCH_Maintenance_FAPIv3+1);
                FAPI_Output( "      puschParametersFapiV4 :\n" );
                FAPI_Output( "        cbCrsStatusRequest           : %u\n", p_PUSCH_FAPIv4->cbCrsStatusRequest );
                FAPI_Output( "        srsTxPorts                   : %u\n", p_PUSCH_FAPIv4->srsTxPorts );
                FAPI_Output( "        ulTpmiIndex                  : %u\n", p_PUSCH_FAPIv4->ulTpmiIndex );
                FAPI_Output( "        numULSpatialStreamPorts      : %u\n", p_PUSCH_FAPIv4->numULSpatialStreamPorts );
                FAPI_Output( "        ulSpatialStreamPorts         :" );
                for (arrayIndex = 0; arrayIndex < p_PUSCH_FAPIv4->numULSpatialStreamPorts && arrayIndex < MAX_NUM_UL_SPATIAL_STREAM_PORTS; arrayIndex++)
                {
                    FAPI_Output( " %u", p_PUSCH_FAPIv4->ulSpatialStreamPorts[arrayIndex] );
                }
                FAPI_Output( "\n" );

                p_PuschUCIInfo_FAPIv3 = (PuschUCIInfo_FAPIv3 *) (p_PUSCH_FAPIv4+1);
                p_UCIPart2Info_FAPIv3 = (UCIPart2Info_FAPIv3 *) p_PuschUCIInfo_FAPIv3->variablePart;;
                if ((p_PuschPdu->pduBitmap & 0x02))
                {
                    FAPI_Output( "      puschUciFapiV3 :\n" );
                    FAPI_Output( "        numPart2s                    : %u\n", p_PuschUCIInfo_FAPIv3->numPart2s );
                    for (part2sIndex = 0; part2sIndex < p_PuschUCIInfo_FAPIv3->numPart2s; part2sIndex++)
                    {
                        FAPI_Output( "        ForEach numPart2s[%u] :\n", part2sIndex );
                        FAPI_Output( "          priority                   : %u\n", p_UCIPart2Info_FAPIv3->priority );
                        FAPI_Output( "          numPart1Params             : %u\n", p_UCIPart2Info_FAPIv3->numPart1Params );
                        FAPI_Output( "          paramOffsets               :" );
                        for (arrayIndex = 0; arrayIndex < p_PuschUCIInfo_FAPIv3->numPart2s; arrayIndex++)
                        {
                            FAPI_Output( " %u", p_UCIPart2Info_FAPIv3->paramOffsets[arrayIndex] );
                        }
                        FAPI_Output( "\n" );

                        FAPI_Output( "          paramSizes                 :" );
                        for (arrayIndex = 0; arrayIndex < p_PuschUCIInfo_FAPIv3->numPart2s; arrayIndex++)
                        {
                            FAPI_Output( " %u", p_UCIPart2Info_FAPIv3->paramSizes[arrayIndex] );
                        }
                        FAPI_Output( "\n" );

                        FAPI_Output( "          part2SizeMapIndex          : %u\n", p_UCIPart2Info_FAPIv3->part2SizeMapIndex );
                        FAPI_Output( "          part2SizeMapScope          : %u\n", p_UCIPart2Info_FAPIv3->part2SizeMapScope );

                        p_UCIPart2Info_FAPIv3++;
                    }
                }

                break;

            case UL_PDU_TYPE_PUCCH :
                FAPI_Output( "    PDU [%u/%u] : PUCCH Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_PucchPdu = (PUCCH_PDU *)p_pduHeader;

                FAPI_Output( "      rnti                           : %u\n", p_PucchPdu->rnti );
                FAPI_Output( "      handle                         : %u\n", p_PucchPdu->handle );
                FAPI_Output( "      bwpSize                        : %u\n", p_PucchPdu->bwpSize );
                FAPI_Output( "      bwpStart                       : %u\n", p_PucchPdu->bwpStart );
                FAPI_Output( "      subcarrierSpacing              : %u\n", p_PucchPdu->subcarrierSpacing );
                FAPI_Output( "      cyclicPrefix                   : %u\n", p_PucchPdu->cyclicPrefix );
                FAPI_Output( "      formatType                     : %u\n", p_PucchPdu->formatType );
                FAPI_Output( "      multiSlotTxIndicator           : %u\n", p_PucchPdu->multiSlotTxIndicator );
                FAPI_Output( "      pi2Bpsk                        : %u\n", p_PucchPdu->pi2Bpsk );
                FAPI_Output( "      prbStart                       : %u\n", p_PucchPdu->prbStart );
                FAPI_Output( "      prbSize                        : %u\n", p_PucchPdu->prbSize );
                FAPI_Output( "      startSymbolIndex               : %u\n", p_PucchPdu->startSymbolIndex );
                FAPI_Output( "      nrOfSymbols                    : %u\n", p_PucchPdu->nrOfSymbols );
                FAPI_Output( "      intraSlotFrequencyHopping      : %u\n", p_PucchPdu->intraSlotFrequencyHopping );
                FAPI_Output( "      secondHopPRB                   : %u\n", p_PucchPdu->secondHopPRB );
                FAPI_Output( "      pucchGroupHopping              : %u\n", p_PucchPdu->pucchGroupHopping );
                FAPI_Output( "      obsolete8bit                   : %u\n", p_PucchPdu->obsolete8bit );
                FAPI_Output( "      nIdPucchHopping                : %u\n", p_PucchPdu->nIdPucchHopping );
                FAPI_Output( "      initialCyclicShift             : %u\n", p_PucchPdu->initialCyclicShift );
                FAPI_Output( "      nIdPucchScrambling             : %u\n", p_PucchPdu->nIdPucchScrambling );
                FAPI_Output( "      timeDomainOccIdx               : %u\n", p_PucchPdu->timeDomainOccIdx );
                FAPI_Output( "      preDftOccIdx                   : %u\n", p_PucchPdu->preDftOccIdx );
                FAPI_Output( "      preDftOccLen                   : %u\n", p_PucchPdu->preDftOccLen );
                FAPI_Output( "      addDmrsFlag                    : %u\n", p_PucchPdu->addDmrsFlag );
                FAPI_Output( "      nId0PucchDmrsScrambling        : %u\n", p_PucchPdu->nId0PucchDmrsScrambling );
                FAPI_Output( "      m0PucchDmrsCyclicShift         : %u\n", p_PucchPdu->m0PucchDmrsCyclicShift );
                FAPI_Output( "      srBitLen                       : %u\n", p_PucchPdu->srBitLen );
                FAPI_Output( "      bitLenHarq                     : %u\n", p_PucchPdu->bitLenHarq );                     
                FAPI_Output( "      csiPart1BitLength              : %u\n", p_PucchPdu->csiPart1BitLength );

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) (p_PucchPdu+1);
                FAPI_Output( "      rxBeamforming :\n" );
                FAPI_Output( "        trpScheme                    : %u\n", p_Rx_Beamforming_PDU->trpScheme );
                FAPI_Output( "        numPRGs                      : %u\n", p_Rx_Beamforming_PDU->numPRGs );
                FAPI_Output( "        prgSize                      : %u\n", p_Rx_Beamforming_PDU->prgSize );
                FAPI_Output( "        digBFInterface               : %u\n", p_Rx_Beamforming_PDU->digBFInterface );

                numPRGs = p_Rx_Beamforming_PDU->numPRGs;
                digBFInterfaces = p_Rx_Beamforming_PDU->digBFInterface;

                p_Value_U16 = (uint16_t *) p_Rx_Beamforming_PDU->variablePart;

                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    FAPI_Output( "        ForNumPRGs [%u] :\n", prgIndex );
                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterfaces; digBFInterfaceIndex++)
                    {
                        FAPI_Output( "        ForDigBFInterfaces [%u] :\n", digBFInterfaceIndex );
                        FAPI_Output( "          beamIdx                   : %u\n", *p_Value_U16 );
                        p_Value_U16++;
                    }
                }

                p_PUCCH_PDU_Maintenance_FAPIv3 = (PUCCH_PDU_Maintenance_FAPIv3 *) p_Value_U16;
                FAPI_Output( "      pucchMaintenanceFapiV3 :\n" );
                FAPI_Output( "        maxCodeRate                  : %u\n", p_PUCCH_PDU_Maintenance_FAPIv3->maxCodeRate );
                FAPI_Output( "        ulBwpId                      : %u\n", p_PUCCH_PDU_Maintenance_FAPIv3->ulBwpId );

                p_PuschUCIInfo_FAPIv3 = (PuschUCIInfo_FAPIv3 *) (p_PUCCH_PDU_Maintenance_FAPIv3+1);
                p_UCIPart2Info_FAPIv3 = (UCIPart2Info_FAPIv3 *) p_PuschUCIInfo_FAPIv3->variablePart;;
                FAPI_Output( "      uciPart1ToPart2CorrespondenceFapiV3 :\n" );
                FAPI_Output( "        numPart2s                    : %u\n", p_PuschUCIInfo_FAPIv3->numPart2s );
                for (part2sIndex = 0; part2sIndex < p_PuschUCIInfo_FAPIv3->numPart2s; part2sIndex++)
                {
                    FAPI_Output( "        ForEach numPart2s[%u] :\n", part2sIndex );
                    FAPI_Output( "          priority                   : %u\n", p_UCIPart2Info_FAPIv3->priority );
                    FAPI_Output( "          numPart1Params             : %u\n", p_UCIPart2Info_FAPIv3->numPart1Params );
                    FAPI_Output( "          paramOffsets               :" );
                    for (arrayIndex = 0; arrayIndex < p_PuschUCIInfo_FAPIv3->numPart2s; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_UCIPart2Info_FAPIv3->paramOffsets[arrayIndex] );
                    }
                    FAPI_Output( "\n" );

                    FAPI_Output( "          paramSizes                 :" );
                    for (arrayIndex = 0; arrayIndex < p_PuschUCIInfo_FAPIv3->numPart2s; arrayIndex++)
                    {
                        FAPI_Output( " %u", p_UCIPart2Info_FAPIv3->paramSizes[arrayIndex] );
                    }
                    FAPI_Output( "\n" );

                    FAPI_Output( "          part2SizeMapIndex          : %u\n", p_UCIPart2Info_FAPIv3->part2SizeMapIndex );
                    FAPI_Output( "          part2SizeMapScope          : %u\n", p_UCIPart2Info_FAPIv3->part2SizeMapScope );

                    p_UCIPart2Info_FAPIv3++;
                }

                break;

            case UL_PDU_TYPE_SRS :
                FAPI_Output( "    PDU [%u/%u] : SRS Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_SrsPdu = (SRS_PDU *)p_pduHeader;

                FAPI_Output( "      rnti                           : %u\n", p_SrsPdu->rnti );
                FAPI_Output( "      handle                         : %u\n", p_SrsPdu->handle );
                FAPI_Output( "      bwpSize                        : %u\n", p_SrsPdu->bwpSize );
                FAPI_Output( "      bwpStart                       : %u\n", p_SrsPdu->bwpStart );
                FAPI_Output( "      subcarrierSpacing              : %u\n", p_SrsPdu->subcarrierSpacing );
                FAPI_Output( "      cyclicPrefix                   : %u\n", p_SrsPdu->cyclicPrefix );
                FAPI_Output( "      numAntPorts                    : %u\n", p_SrsPdu->numAntPorts );
                FAPI_Output( "      numSymbols                     : %u\n", p_SrsPdu->numSymbols );
                FAPI_Output( "      numRepetitions                 : %u\n", p_SrsPdu->numRepetitions );
                FAPI_Output( "      timeStartPosition              : %u\n", p_SrsPdu->timeStartPosition );
                FAPI_Output( "      configIndex                    : %u\n", p_SrsPdu->configIndex );
                FAPI_Output( "      sequenceId                     : %u\n", p_SrsPdu->sequenceId );
                FAPI_Output( "      bandwidthIndex                 : %u\n", p_SrsPdu->bandwidthIndex );
                FAPI_Output( "      combSize                       : %u\n", p_SrsPdu->combSize );
                FAPI_Output( "      combOffset                     : %u\n", p_SrsPdu->combOffset );
                FAPI_Output( "      cyclicShift                    : %u\n", p_SrsPdu->cyclicShift );
                FAPI_Output( "      frequencyPosition              : %u\n", p_SrsPdu->frequencyPosition );
                FAPI_Output( "      frequencyShift                 : %u\n", p_SrsPdu->frequencyShift );
                FAPI_Output( "      frequencyHopping               : %u\n", p_SrsPdu->frequencyHopping );
                FAPI_Output( "      groupOrSequenceHopping         : %u\n", p_SrsPdu->groupOrSequenceHopping );
                FAPI_Output( "      resourceType                   : %u\n", p_SrsPdu->resourceType );
                FAPI_Output( "      tSrs                           : %u\n", p_SrsPdu->tSrs );
                FAPI_Output( "      tOffset                        : %u\n", p_SrsPdu->tOffset );

                p_Rx_Beamforming_PDU = (Rx_Beamforming_PDU *) (p_SrsPdu+1);
                FAPI_Output( "      rxBeamforming :\n" );
                FAPI_Output( "        trpScheme                    : %u\n", p_Rx_Beamforming_PDU->trpScheme );
                FAPI_Output( "        numPRGs                      : %u\n", p_Rx_Beamforming_PDU->numPRGs );
                FAPI_Output( "        prgSize                      : %u\n", p_Rx_Beamforming_PDU->prgSize );
                FAPI_Output( "        digBFInterface               : %u\n", p_Rx_Beamforming_PDU->digBFInterface );

                numPRGs = p_Rx_Beamforming_PDU->numPRGs;
                digBFInterfaces = p_Rx_Beamforming_PDU->digBFInterface;

                p_Value_U16 = (uint16_t *) p_Rx_Beamforming_PDU->variablePart;

                for (prgIndex = 0; prgIndex < numPRGs; prgIndex++)
                {
                    FAPI_Output( "        ForNumPRGs [%u] :\n", prgIndex );
                    for (digBFInterfaceIndex = 0; digBFInterfaceIndex < digBFInterfaces; digBFInterfaceIndex++)
                    {
                        FAPI_Output( "        ForDigBFInterfaces [%u] :\n", digBFInterfaceIndex );
                        FAPI_Output( "          beamIdx                   : %u\n", *p_Value_U16 );
                        p_Value_U16++;
                    }
                }

                p_SRS_PDU_FAPIv4 = (SRS_PDU_FAPIv4 *) p_Value_U16;
                FAPI_Output( "      srsParametersFapiV4 :\n" );
                FAPI_Output( "        srsBandwidthSize             : %u\n", p_SRS_PDU_FAPIv4->srsBandwidthSize );
                for (arrayIndex = 0; arrayIndex < MAX_SRS_IND_SYMBOL_INFO_COUNT; arrayIndex++)
                {
                    FAPI_Output( "        srsSymbolInfo[%u]             :\n", arrayIndex );
                    FAPI_Output( "          srsBandwidthStart          : %u\n", p_SRS_PDU_FAPIv4->srsSymbolInfo[arrayIndex].srsBandwidthStart ); 
                    FAPI_Output( "          sequenceGroup              : %u\n", p_SRS_PDU_FAPIv4->srsSymbolInfo[arrayIndex].sequenceGroup ); 
                    FAPI_Output( "          sequenceNumber             : %u\n", p_SRS_PDU_FAPIv4->srsSymbolInfo[arrayIndex].sequenceNumber ); 
                }
                FAPI_Output( "        usage                        : %u\n", p_SRS_PDU_FAPIv4->usage );
                FAPI_Output( "        reportType                   :" );
                for (arrayIndex = 0; arrayIndex < MAX_SRS_IND_SYMBOL_INFO_COUNT; arrayIndex++)
                {
                    FAPI_Output( " %u", p_SRS_PDU_FAPIv4->reportType[arrayIndex] );
                }
                FAPI_Output( "\n" );
                FAPI_Output( "        singularValueRepresentation  : %u\n", p_SRS_PDU_FAPIv4->singularValueRepresentation );
                FAPI_Output( "        iqRepresentation             : %u\n", p_SRS_PDU_FAPIv4->iqRepresentation );
                FAPI_Output( "        prgSize                      : %u\n", p_SRS_PDU_FAPIv4->prgSize );
                FAPI_Output( "        numTotalUeAntennas           : %u\n", p_SRS_PDU_FAPIv4->numTotalUeAntennas );
                FAPI_Output( "        ueAntennasInThisSrsResourceSet : %u\n", p_SRS_PDU_FAPIv4->ueAntennasInThisSrsResourceSet );
                FAPI_Output( "        sampledUeAntennas            : %u\n", p_SRS_PDU_FAPIv4->sampledUeAntennas );
                FAPI_Output( "        reportScope                  : %u\n", p_SRS_PDU_FAPIv4->reportScope );
                FAPI_Output( "        numULSpatialStreamsPorts     : %u\n", p_SRS_PDU_FAPIv4->numULSpatialStreamsPorts );
                FAPI_Output( "        ulSpatialStreamPorts         :" );
                for (arrayIndex = 0; arrayIndex < p_SRS_PDU_FAPIv4->numULSpatialStreamsPorts && arrayIndex < MAX_NUM_UL_SPATIAL_STREAM_PORTS; arrayIndex++)
                {
                    FAPI_Output( " %u", p_SRS_PDU_FAPIv4->ulSpatialStreamPorts[arrayIndex] );
                }
                FAPI_Output( "\n" );

                break;

            case UL_PDU_TYPE_MSGA_PUSCH :
                FAPI_Output( "    PDU [%u/%u] : MSGA_PUSCH Pdu (Size:%u)\n", pduIndex+1, pduCount, p_pduHeader->pduSize );

                p_MsgAPuschPdu = (MSGA_PUSCH_PDU *)p_pduHeader;

                FAPI_Output( "      pduBitmap                      : %u\n", p_MsgAPuschPdu->pduBitmap );
                FAPI_Output( "      prachToPruMapType              : %u\n", p_MsgAPuschPdu->prachToPruMapType );
                FAPI_Output( "      prachToPruMapIndicator         : %u\n", p_MsgAPuschPdu->prachToPruMapIndicator );
                FAPI_Output( "      msgAprachSfn                   : %u\n", p_MsgAPuschPdu->msgAprachSfn );
                FAPI_Output( "      msgAprachSlot                  : %u\n", p_MsgAPuschPdu->msgAprachSlot );
                FAPI_Output( "      handle                         : %u\n", p_MsgAPuschPdu->handle );
                FAPI_Output( "      bwpSize                        : %u\n", p_MsgAPuschPdu->bwpSize );
                FAPI_Output( "      bwpStart                       : %u\n", p_MsgAPuschPdu->bwpStart );
                FAPI_Output( "      subcarrierSpacing              : %u\n", p_MsgAPuschPdu->subcarrierSpacing );
                FAPI_Output( "      cyclicPrefix                   : %u\n", p_MsgAPuschPdu->cyclicPrefix );
                FAPI_Output( "      msgAmcs                        : %u\n", p_MsgAPuschPdu->msgAmcs );
                FAPI_Output( "      transformPrecoding             : %u\n", p_MsgAPuschPdu->transformPrecoding );
                FAPI_Output( "      nIdMsgAPusch                   : %u\n", p_MsgAPuschPdu->nIdMsgAPusch );
                FAPI_Output( "      puschDmrsScramblingId[]        : %u %u\n", p_MsgAPuschPdu->puschDmrsScramblingId[0], p_MsgAPuschPdu->puschDmrsScramblingId[1] );
                FAPI_Output( "      ulDmrsSymbPos                  : %u\n", p_MsgAPuschPdu->ulDmrsSymbPos );
                FAPI_Output( "      dmrsMappingType                : %u\n", p_MsgAPuschPdu->dmrsMappingType );
                FAPI_Output( "      startRbPuschOcas               : %u\n", p_MsgAPuschPdu->startRbPuschOcas );
                FAPI_Output( "      numRbPUschOcas                 : %u\n", p_MsgAPuschPdu->numRbPUschOcas );
                FAPI_Output( "      guardBand                      : %u\n", p_MsgAPuschPdu->guardBand );
                FAPI_Output( "      intraSlotFrequencyHopping      : %u\n", p_MsgAPuschPdu->intraSlotFrequencyHopping );
                FAPI_Output( "      intraSlotFrequencyHoppingBits  : %u\n", p_MsgAPuschPdu->intraSlotFrequencyHoppingBits );
                FAPI_Output( "      txDirectCurrentLocation        : %u\n", p_MsgAPuschPdu->txDirectCurrentLocation );
                FAPI_Output( "      uplinkFrequencyShift7p5khz     : %u\n", p_MsgAPuschPdu->uplinkFrequencyShift7p5khz );
                FAPI_Output( "      startSymbolIdPuschOcas         : %u\n", p_MsgAPuschPdu->startSymbolIdPuschOcas );
                FAPI_Output( "      durationPuschOcas              : %u\n", p_MsgAPuschPdu->durationPuschOcas );
                FAPI_Output( "      guardPeriod                    : %u\n", p_MsgAPuschPdu->guardPeriod );
                FAPI_Output( "      tbSize                         : %u\n", p_MsgAPuschPdu->tbSize );

                break;

            default :
                break;
        }

        p_pduHeader = (PDU_Struct *) ((uint8_t *)p_pduHeader+p_pduHeader->pduSize);
    }
}

void phy_ParseMsg( uint8_t *p_MsgBuff, uint32_t msgLength )
{
    uint32_t fapiMsgCount, fapiMsgIndex, msgOffset;
    Fapi_API_Header *p_FapiApiHeader;
    Fapi_Generic_Header *p_FapiGenericHeader;

    FAPI_Output( "Parse Msg : \n" );

    p_FapiApiHeader = (Fapi_API_Header *) p_MsgBuff;
    fapiMsgCount = p_FapiApiHeader->numOfMessages;
    msgOffset = sizeof(Fapi_API_Header);

    if (msgOffset > msgLength) 
    {
        FAPI_Output( "  Size error : invalid Fapi_API_Header\n" );
        return;
    }

    FAPI_Output( "  numOfMessages : %u\n", fapiMsgCount );
    FAPI_Output( "  opaqueHandle  : %u\n", p_FapiApiHeader->opaqueHandle );
 
    for (fapiMsgIndex = 0; fapiMsgIndex < fapiMsgCount; fapiMsgIndex++)  
    {
        p_FapiGenericHeader = (Fapi_Generic_Header *) (p_MsgBuff+msgOffset);

        if (msgOffset+sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length > msgLength) 
        {
            FAPI_Output( "  Size error : invalid Fapi_Generic_Header\n" );
            return;
        }

        FAPI_Output( "  FAPI Msg %u/%u  :\n", fapiMsgIndex+1, fapiMsgCount );

        switch (p_FapiGenericHeader->type) 
        {
            case MSG_PHY_PARAM_REQ :
                phy_ParseMsg_ParamReq( p_FapiGenericHeader );
                break; 

            case MSG_PHY_CONFIG_REQ :
                phy_ParseMsg_ConfigReq( p_FapiGenericHeader, &g_Phy_FapiConfigRecvInfo );
                break; 

            case MSG_PHY_DL_TTI_REQ :
                phy_ParseMsg_DLTTIReq( p_FapiGenericHeader );
                break; 

            case MSG_PHY_TX_DATA_REQ : 
                phy_ParseMsg_TxDataReq( p_FapiGenericHeader );
                break; 

            case MSG_PHY_UL_DCI_REQ :
                phy_ParseMsg_ULDCIReq( p_FapiGenericHeader );
                break; 

            case MSG_PHY_UL_TTI_REQ :
                phy_ParseMsg_ULTTIReq( p_FapiGenericHeader );
                break; 

            default :
                FAPI_Output( "    type %u is not supported\n", p_FapiGenericHeader->type );
                break;
        }

        msgOffset += sizeof(Fapi_Generic_Header)+p_FapiGenericHeader->length;
    }
    FAPI_Output( "\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////
// Main routiunes

void phy_SetFapiParamInfo( FapiParamInfo *p_FapiParamInfo )
{
    uint32_t index;
    Cell_Param *p_CellParam;
    Carrier_Param *p_CarrierParam;
    PDCCH_Param *p_PdcchParam;
    PUCCH_Param *p_PucchParam;
    PDSCH_Param *p_PdschParam;
    PUSCH_Param *p_PuschParam;


    PrbSymBitmapsPerSlotInfo *p_PrbSymBitmapsPerSlotInfo;
    

    // Set : Table 3-13 : Cell and PHY parameters

    p_CellParam = &p_FapiParamInfo->cellParam;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_RELEASE_CAPABILITY;
    p_CellParam->releaseCapability = BITMASK_REL_CAPABILITY_SUPPORT_REL15;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_PHY_STATE; 
    p_CellParam->phyState = PHY_STATE_IDLE;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_SKIP_BLANK_DL_CONFIG;
    p_CellParam->skipBlankDLConfig = FEATURE_NOT_SUPPORTED;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_SKIP_BLANK_UL_CONFIG;
    p_CellParam->skipBlankULConfig = FEATURE_NOT_SUPPORTED;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_NUM_CONFIG_TLVS_TO_REPORT;
    index = 0;
    p_CellParam->configTLVToReportList[index].tag = TAG_PHY_CELL_ID;
    p_CellParam->configTLVToReportList[index].length = sizeof(uint8_t);
    p_CellParam->configTLVToReportList[index].value = IDLE_ONLY_MANDATORY;
    index++;
    p_CellParam->configTLVToReportList[index].tag = TAG_CARRIER_CONFIGURATION;
    p_CellParam->configTLVToReportList[index].length = sizeof(uint8_t);
    p_CellParam->configTLVToReportList[index].value = IDLE_ONLY_MANDATORY;
    index++;
    p_CellParam->numConfigTLVsToReport = index;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_POWER_PROFILES;
    p_CellParam->powerProfilesSupported |= BITMASK_POWER_PROFILE_SUPPORT_NR;
    for (index = 0; index < POWER_PROFILE_SIGNAL_INDEX_COUNT; index++)
    {
        p_CellParam->powerProfileInfoList[index].referenceRS = 0;
        p_CellParam->powerProfileInfoList[index].powerOffsetMin = 0; 
        p_CellParam->powerProfileInfoList[index].powerOffsetMax = 0;
    }

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_DL_TTI;
    p_CellParam->maxNumPDUsInDL_TTI = 1;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_SSPBCH_BLOCK_POWER_SCALING_UNIT_CHOICE;
    p_CellParam->ssPbchBlockPowerScalingUnitChoice= 1;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_UL_TTI;
    p_CellParam->maxNumPDUsInUL_TTI = 1;

    p_CellParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_UL_DCI;
    p_CellParam->maxNumPDUsInUL_DCI = 1;

    // Set : Table 3-14 : Carrier parameters

    p_CarrierParam = &p_FapiParamInfo->carrierParam;

    p_CarrierParam->tagBitmask |= PARAM_TAG_BITMASK_CYCLIC_PREFIX;
    p_CarrierParam->cyclicPrefix =BITMASK_CYCLIC_PREFIX_SUPPORT_NORMAL|BITMASK_CYCLIC_PREFIX_SUPPORT_EXTENDED;

   
    p_CarrierParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_DL; 
    p_CarrierParam->supportedSubcarrierSpacingsDl = BITMASK_SUBCARRIER_SPACING_SUPPORT_15_KHZ|BITMASK_SUBCARRIER_SPACING_SUPPORT_30_KHZ;

    p_CarrierParam->tagBitmask|= PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB;
    p_CarrierParam->supportedSubcarrierSpcacingSSB = BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_15_KHZ;
    
    p_CarrierParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_BANDWIDTH_DL; 
    p_CarrierParam->supportedBandwidthDl = BITMASK_BANDWIDTH_SUPPORT_80_MHZ|BITMASK_BANDWIDTH_SUPPORT_100_MHZ;
   
    p_CarrierParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_UL; 
    p_CarrierParam->supportedSubcarrierSpacingsUl = BITMASK_SUBCARRIER_SPACING_SUPPORT_15_KHZ|BITMASK_SUBCARRIER_SPACING_SUPPORT_30_KHZ;

    p_CarrierParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_BANDWIDTH_UL; 
    p_CarrierParam->supportedBandwidthUl = BITMASK_BANDWIDTH_SUPPORT_80_MHZ|BITMASK_BANDWIDTH_SUPPORT_100_MHZ;

    

    p_CarrierParam->tagBitmask |= PARAM_TAG_BITMASK_SS_PBCH_MULTIPLE_CARRIERS_IN_A_BAND;
    p_CarrierParam->ssPbchMultipleCarriersInABand = 0;

    p_CarrierParam->tagBitmask |= PARAM_TAG_BITMASK_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRIER;
    p_CarrierParam->multipleCellsSsPbchInACarrier=0 ;
    
    p_CarrierParam->tagBitmask |= PARAM_TAG_BITMASK_MEANING_OF_CARRIER_FREQUENCY;
    p_CarrierParam->meaningOfCarrierFrequency = BITMASK_MEANING_OF_CARRIER_FREQUENCY_POINT_A;


    // Set : Table 3-15 : PDCCH parameters

    p_PdcchParam= &p_FapiParamInfo->pdcchParam;

    p_PdcchParam->tagBitmask|= PARAM_TAG_BITMASK_CCE_MAPPING_TYPE;
    p_PdcchParam->cceMappingType = BITMASK_CCE_MAPPING_TYPE_SUPPORT_INTERLEAVED;


    p_PdcchParam->tagBitmask|= PARAM_TAG_BITMASK_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS;
    p_PdcchParam->coresetOutsideFirst3OfdmSymsOfSlot= 0;

    p_PdcchParam->tagBitmask|= PARAM_TAG_BITMASK_PRECODER_GRANULARITY_CORESET;
    p_PdcchParam->precoderGranularityCoreset= 0;


    p_PdcchParam->tagBitmask|= PARAM_TAG_BITMASK_PDCCH_MU_MIMO;
    p_PdcchParam->pdcchMuMimo= 0;

    p_PdcchParam->tagBitmask|= PARAM_TAG_BITMASK_PDCCH_PRECODER_CYCLING;
    p_PdcchParam->pdcchPrecoderCycling= 0;

    p_PdcchParam->tagBitmask|= PARAM_TAG_BITMASK_MAX_PDCCHS_PER_SLOT;
    p_PdcchParam->maxPdcchsPerSlot= 1;


    // Set : Table 3-16 : PUCCH parameters

    p_PucchParam =& p_FapiParamInfo->pucchParam;

    p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_PUCCH_FORMATS;
    p_PucchParam->pucchFormats= BITMASK_PUCCH_SUPPORT_FORMAT_2;

    p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_PUCCHS_PER_SLOT;
    p_PucchParam->maxPucchsPerSlot= 2;


    p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_PUCCH_GROUP_AND_SEQUENCE_HOPPING;
    p_PucchParam->pucchGroupAndSequenceHopping= PUCCH_GROUP_AND_SEQ_HOPPING_DYNAMIC_ONLY;

    p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUM_UL_BWP_IDS;
    p_PucchParam->pucchGroupAndSequenceHopping= 0;

    p_PucchParam->tagBitmask |= PARAM_TAG_BITMASK_PUCCH_AGGREGATION;
    p_PucchParam->maxNumUlBwpIds= BITMASK_PUCCH_AGGREGATION_REPETITION_OVER_MULTI_SLOTS;

    // Set : Table 3-17 : PDSCH parameters

    p_PdschParam =&p_FapiParamInfo->pdschParam;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_MAPPING_TYPE;
    p_PdschParam->pdschMappingType = BITMASK_MAPPING_TYPE_SUPPORT_TYPE_A;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_ALLOCATION_TYPES;
    p_PdschParam->pdschAllocationTypes= BITMASK_ALLOC_TYPE_SUPPORT_TYPE_1;
    
    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_VRB_TO_PRB_MAPPING;
    p_PdschParam->pdschVrbToPrbMapping= BITMASK_VRB_PRB_MAPPING_SUPPORT_NON_INTERLEAVED;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_CBG;
    p_PdschParam->pdschCbg= PDSCH_CBG_SUPPORTED_SEGMENTATION_IN_L2;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_DMRS_CONFIG_TYPES;
    p_PdschParam->pdschDmrsConfigTypes= BITMASK_DMRS_CONFIG_SUPPORT_TYPE_1;
    
    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_DMRS_MAX_LENGTH;
    p_PdschParam->pdschDmrsMaxLength= DMRS_MAX_LENGTH_1;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_DMRS_ADDITIONAL_POS;
    p_PdschParam->pdschDmrsAdditionalPos= BITMASK_DMRS_ADD_POS_SUPPORT_POS_2;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_PDSCHS_TBS_PER_SLOT;
    p_PdschParam->maxPdschsTBsPerSlot= 1;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_PDSCH;
    p_PdschParam->maxNumberMimoLayersPdsch= 4;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_MAX_MODULATION_ORDER_DL;
    p_PdschParam->supportedMaxModulationOrderDl= MAX_MODULATION_ORDER_256QAM;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_MAX_MU_MIMO_USERS_DL;
    p_PdschParam->maxMuMimoUsersDl= 1;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_DATA_IN_DMRS_SYMBOLS;
    p_PdschParam->pdschDataInDmrsSymbols= 0;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PREMPTION_SUPPORT;
    p_PdschParam->premptionSupport= 0;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDSCH_NON_SLOT_SUPPORT;
    p_PdschParam->pdschNonSlotSupport= 0;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_SSB_RATE_MATCH;
    p_PdschParam->ssbRateMatch=SSB_RATE_MATCH_NOT_SUPPORTED;
    
    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_RATE_MATCH_PATTERN_TYPE;
    p_PdschParam->supportedRateMatchPatternType=BITMASK_RATE_MATCH_PATTERN_SUPPORT_SCS_30KHZ;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_PDCCH_RATE_MATCH;
    p_PdschParam->pdcchRateMatch= 1;
    
    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_NUM_OF_RATE_MATCH_PATTERN_LTE_CRS;
    p_PdschParam->pdcchRateMatch= 0;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_NUM_OF_RATE_MATCH_PATTER_LTE_CRS_IN_PHY;
    p_PdschParam->numOfRateMatchPatternLTECrsInPhy= 0;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_LTE_CRS_RATE_MATCH_MB_SFN_DERIVATION;
    p_PdschParam->lteCrsRateMatchMbsfnDerivation= 0;

    p_PdschParam->tagBitmask |= PARAM_TAG_BITMASK_SUPPORTED_LTE_CRS_RATE_MATCH_METHOD;
    p_PdschParam->supportedLTECrsRateMatchMethod= 0;

    p_PdschParam->tagBitmask |=  PARAM_TAG_BITMASK_CSI_RS_RATE_MATCH;
    p_PdschParam->csiRsRateMatch= 1;

    p_PdschParam->tagBitmask |=  PARAM_TAG_BITMASK_PDSCH_TRANS_TYPE_SUPPORT;
    p_PdschParam->pdschTransTypeSupport= TRANS_TYPE_SUPPORT_REQUIRE_VALID_TYPE;

    p_PdschParam->tagBitmask |=  PARAM_TAG_BITMASK_PDSCH_MAC_PDU_BIT_ALIGNMENT;
    p_PdschParam->pdschMacPduBitAlignment= MAC_PDU_BIT_ALIGNMENT_32BIT;

    p_PdschParam->tagBitmask |=  PARAM_TAG_BITMASK_MAX_NUMBER_PRB_SYM_BITMAPS_PER_SLOT;
    p_PrbSymBitmapsPerSlotInfo= &p_PdschParam->maxNumberPrbSymBitmapsPerSlot;

    p_PrbSymBitmapsPerSlotInfo->PrbSymBitmapsPerSlot[0]= 1;
    p_PrbSymBitmapsPerSlotInfo->PrbSymBitmapsPerSlot[1]= 1;
    p_PrbSymBitmapsPerSlotInfo->PrbSymBitmapsPerSlot[2]= 1;
    p_PrbSymBitmapsPerSlotInfo->PrbSymBitmapsPerSlot[3]= 1;
    p_PrbSymBitmapsPerSlotInfo->PrbSymBitmapsPerSlot[4]= 1;
    
/*
    p_PdschParam->tagBitmask |=  PARAM_TAG_BITMASK_MAX_NUMBER_CSI_RS_RATE_MATCH_PER_SLOT;
    p_PdschParam->maxNumberCsiRsRateMatchPerSlot[1]= 1;
*/
    p_PdschParam->tagBitmask |=  PARAM_TAG_BITMASK_MAX_NUM_SSB_PER_PDSCH_SLOT;
    p_PdschParam->maxNumSsbPerPdschSlot=8;

    p_PdschParam->tagBitmask |=  PARAM_TAG_BITMASK_UNIVERSAL_RATE_MATCH;
    p_PdschParam->universalRateMatch= 0;

    p_PdschParam->tagBitmask |=  PARAM_TAG_BITMASK_REQUIRED_PDSCH_SIGNALING_ASSOCIATIONS;
    p_PdschParam->requiredPdschSignalingAssociations= 0;


    // Set : Table 3-18 : PUSCH parameters

    p_PuschParam =&p_FapiParamInfo->puschParam;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_UCI_MUX_ULSCH_IN_PUSCH;
    p_PuschParam->uciMuxUlschInPusch= FEATURE_SUPPORTED;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_UCI_ONLY_PUSCH;
    p_PuschParam->uciOnlyPusch = FEATURE_SUPPORTED;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_FREQUENCY_HOPPING;
    p_PuschParam->puschFrequencyHopping= FEATURE_SUPPORTED;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_DMRS_CONFIG_TYPES;
    p_PuschParam->puschDmrsConfigTypes= BITMASK_PUSCH_DMRS_CONFIG_TYPE1;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_DMRS_MAX_LEN;
    p_PuschParam->puschDmrsMaxLen= PUSCH_DMRS_MAX_LENGTH1;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_DMRS_ADDITIONAL_POS;
    p_PuschParam->puschDmrsAdditionalPos= BITMASK_DMRS_ADD_POS_SUPPORT_POS_2;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_CBG;
    p_PuschParam->puschCbg= FEATURE_NOT_SUPPORTED;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_MAPPING_TYPE;
    p_PuschParam->puschMappingType= BITMASK_MAPPING_TYPE_SUPPORT_TYPE_A;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_ALLOCATION_TYPE;
    p_PuschParam->puschAllocationTypes= BITMASK_ALLOC_TYPE_SUPPORT_TYPE_1;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_VRB_TO_PRB_MAPPING;
    p_PuschParam->puschVrbToPrbMapping= FEATURE_NOT_SUPPORTED;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_MAX_PTRS_PORTS;
    p_PuschParam->puschMaxPtrsPorts= 0;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_MAX_PDUSCHS_TBS_PER_SLOT;
    p_PuschParam->maxPduschsTBsPerSlot= 10;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_NON_CB_PUSCH;
    p_PuschParam->uciOnlyPusch = 2;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_CB_PUSCH;
    p_PuschParam->maxNumberMimoLayersNonCbPusch= 2;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_SUPPORTED_MAX_MODULATION_ORDER_UL;
    p_PuschParam->maxNumberMimoLayersCbPusch= MAX_MODULATION_ORDER_256QAM;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_MAX_MU_MIMO_USERS_UL;
    p_PuschParam->maxMuMimoUsersUl=1;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_DFTS_OFDM_SUPPORT;
    p_PuschParam->dftsOfdmSupport= FEATURE_SUPPORTED;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_AGGREGATION_FACTOR;
    p_PuschParam->puschAggregationFactor= BITMASK_PUSCH_AGGREGATION_SUPPORT_FACTOR4;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_LBRM_SUPPORT;
    p_PuschParam->puschLbrmSupport=FEATURE_SUPPORTED;

    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_TRANS_TYPE_SUPPORT;
    p_PuschParam->puschTransTypeSupport=1;
    
    p_PuschParam->tagBitmask|= PARAM_TAG_BITMASK_PUSCH_MAC_PDU_BIT_ALIGNMENT;
    p_PuschParam->puschMacPduBitAlignment=MAC_PDU_BIT_ALIGNMENT_32BIT;
    
    
}

void mac_SetFapiConfigInfo( FapiConfigInfo *p_FapiConfigInfo )
{
    uint16_t numerologyIndex, prachFdOccasionIndex, numPrachFdOccasions, ulBwpIndex;
    uint16_t unusedRootSequenceIndex, unusedRootSequenceCount, numPrachConfigurations, multiPrachConfigInfoIndex;
    uint16_t numAssociationPatternPeriod, associationPatternPeriodIndex, scs, tddPeriodBySlot, slotIndex, symbolIndex;
    uint16_t numUci2Maps, numPart1Params, uci2MapsIndex, part1ParamsIndex, sumOfSizesPart1Params, mapSize, mapIndex, multiSsbResourceConfigIndex;
    uint16_t prbSymbRateMatchPatternInfoIndex, lteCrsRateMatchPatternsIndex, mbsfnSubframeConfigIndex;
    uint16_t numberOfCoresetRateMatchPatterns, coresetRateMatchPatternInfoIndex;
    uint8_t numSsbConfigurations, numberOfPrbSymbRateMatchPatterns, beamIdx;
    uint8_t lteCrsRateMatchPatternsCount, mbsfnSubframeConfigCount, freqDomainRBIndex;
    IndicationInstancePerSlotInfo *p_IndicationInstancePerSlotInfo;
    RequestInstancePerSlotInfo *p_RequestInstancesPerSlotInfo;
    CarrierConfigInfo *p_CarrierConfigInfo;
    PRACHConfigInfo *p_PrachConfigInfo;
    PRACHOccasionInfo *p_PrachOccasionInfo;
    MSGA_PUSCH_ConfigInfo *p_MsgAPuschConfigInfo;
    AssociationPatternPeriodInfo *p_AssociationPatternPeriodInfo;
    SSBResourceConfigInfo *p_SsbResourceConfigInfo;
    TDDPatternInfo *p_TddPatternInfo;
    LTECrsRateMatchPatternsInfo *p_LteCrsRateMatchPatternInfo;
    PUCCH_SemiStatic_ConfigInfo *p_PucchSemiStaticConfigInfo;
    PHY_Config *p_PhyConfig;
    Carrier_Config *p_CarrierConfig;
    Cell_Config *p_CellConfig;
    SsbPowerPBCH_Config *p_SsbPowerPbchConfig;
    PRACH_Config *p_PrachConfig, *p_PrachConfigInMulti;
    MultiPRACH_Config *p_MultiPrachConfig;
    MSGA_PUSCH_Config *p_MsgAPuschConfig;
    SSBResource_Config *p_SsbResourceConfig, *p_SsbResourceConfigInMulti;
    MultiSSBResource_Config *p_MultiSsbResourceConfig;
    TDD_Config *p_TddConfig;
    Measurement_Config *p_MeasurementConfig;
    UCI_Config *p_UciConfig;
    PRBSymRateMatchPatterns_Config *p_PrbSymbRateMatchPatternsConfig;
    CoresetRateMatchPatterns_Config *p_CoresetRateMatchPatternsConfig;
    LTECrsRateMatchPatterns_Config *p_LteCrsRateMatchPatternsConfig;
    PUCCH_SemiStatic_Config *p_PucchSemiStaticConfig;
    PDSCH_SemiStatic_Config *p_PdschSemiStaticConfig;
    DelayManagement_Config *p_DelayManagementConfig;
    Rel16_mTRP_Config *p_Rel16mTRPConfig;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-30 : PHY configuration

    p_PhyConfig = & p_FapiConfigInfo->phyConfig;
    p_PhyConfig ->tagBitmask |= CONFIG_TAG_BITMASK_PHY_PROFILE_ID;
    p_PhyConfig ->phyProfileId =0;
    
    p_PhyConfig ->tagBitmask |= CONFIG_TAG_BITMASK_INDICATION_INSTANCES_PER_SLOT;
    p_IndicationInstancePerSlotInfo=&p_PhyConfig->indicationInstancesPerSlot;
    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[0]=1; //Rx_Data.IND
    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[1]=1; //CRC IND
    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[2]=1; //UCI IND
    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[3]=1; //RACH IND
    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[4]=1; //SRS IND
    p_IndicationInstancePerSlotInfo->indicationInstancePerSlot[5]=1; //DL_TTI RESP
    
    p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_REQUEST_INSTANCES_PER_SLOT;
    p_RequestInstancesPerSlotInfo= &p_PhyConfig->requestInstancesPerSlot;
    p_RequestInstancesPerSlotInfo->requestInstancesPerSlot[0]=1;  //DL_TTI REQ
    p_RequestInstancesPerSlotInfo->requestInstancesPerSlot[1]=1;  //UL_TTI REQ
    p_RequestInstancesPerSlotInfo->requestInstancesPerSlot[2]=1;  //UL_DCI REQ
    p_RequestInstancesPerSlotInfo->requestInstancesPerSlot[3]=1;  //TX_DATA REQ
  
    p_PhyConfig->tagBitmask |= CONFIG_TAG_BITMASK_SLOT_TIME_DOMAIN_INFO;
    p_PhyConfig->slotTimeDomainInfo.sfn = 0;
    p_PhyConfig->slotTimeDomainInfo.slot = 0;
    p_PhyConfig->slotTimeDomainInfo.subcarrierSpacing = 1;
    p_PhyConfig->slotTimeDomainInfo.cyclicPrefixType = 0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-31 : Carrier configuration

    p_CarrierConfig = &p_FapiConfigInfo->carrierConfig;
    p_CarrierConfig->tagBitmask |= CONFIG_TAG_BITMASK_CARRIER_CONFIGURATION;
    p_CarrierConfigInfo = &p_CarrierConfig->carrierConfigInfo;

    numerologyIndex = 1;    // 1: 30Khz
    p_CarrierConfigInfo->dlBandwidth = 0;
    p_CarrierConfigInfo->dlFrequency = 0;
    p_CarrierConfigInfo->dlk0[numerologyIndex] = 0;
    p_CarrierConfigInfo->dlGridSize[numerologyIndex] = 0;
    p_CarrierConfigInfo->numTxAnt = 4;
    p_CarrierConfigInfo->uplinkBandwidth = 0;
    p_CarrierConfigInfo->uplinkFrequency = 0;
    p_CarrierConfigInfo->ulk0[numerologyIndex] = 0;
    p_CarrierConfigInfo->ulGridSize[numerologyIndex] = 0;
    p_CarrierConfigInfo->numRxAnt = 4;
    p_CarrierConfigInfo->frequencyShift7p5KHz = 0;
    p_CarrierConfigInfo->powerProfile = 0;
    p_CarrierConfigInfo->powerOffsetRsIndex = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-32 : Cell configuration

    p_CellConfig = &p_FapiConfigInfo->cellConfig;
    p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PHY_CELL_ID;
    p_CellConfig->phyCellId = 50;


    p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_FRAME_DUPLEX_TYPE;
    p_CellConfig->frameDuplexType = 1;

    p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PDSCH_TRANS_TYPE_VALIDITY;
    p_CellConfig->pdschTransTypeValidity =1;

    p_CellConfig->tagBitmask |= CONFIG_TAG_BITMASK_PUSCH_TRANS_TYPE_VALIDITY;
    p_CellConfig->puschTransTypeValidity =1;


    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-33 : SSB Power and PBCH configuration

    p_SsbPowerPbchConfig =&p_FapiConfigInfo->ssbPowerPbchConfig;
    p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_SS_PBCH_POWER;
    p_SsbPowerPbchConfig->ssPbchPower = 50 ; //TBD

    p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_SS_PBCH_BLOCK_POWER_SCALING;
    p_SsbPowerPbchConfig->ssPbchBlockPowerScaling = 1000;

    p_SsbPowerPbchConfig->tagBitmask |= CONFIG_TAG_BITMASK_BCH_PAYLOAD_FLAG;
    p_SsbPowerPbchConfig->bchPayloadFlag =0;
    
    

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-34 : PRACH configuration 
    
    p_PrachConfig = &p_FapiConfigInfo->prachConfig;
    p_PrachConfig->tagBitmask |= CONFIG_TAG_BITMASK_PRACH_RESOURCE_CONFIGURATION;
    p_PrachConfigInfo = &p_PrachConfig->prachConfigInfo;
    p_PrachConfigInfo->prachResConfigIndex = 0;
    p_PrachConfigInfo->prachSequenceLength = 1;
    p_PrachConfigInfo->prachSubCSpacing = 1;
    p_PrachConfigInfo->ulBwpPuschScs = 0;
    p_PrachConfigInfo->restrictedSetConfig = 0;
    p_PrachConfigInfo->numPrachFdOccasions =1;
    p_PrachConfigInfo->prachConfigIndex = 159;
     
    numPrachFdOccasions = p_PrachConfigInfo->numPrachFdOccasions;
      
    for (prachFdOccasionIndex =0 ; prachFdOccasionIndex <numPrachFdOccasions; prachFdOccasionIndex++)
    {
     
     p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachRootSequenceIndex = 1;
     p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numRootSequences = 2;
     p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].prachZeroCorrConf =0;
     p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences=1 ;
    
    
    p_PrachOccasionInfo++;

    unusedRootSequenceCount = p_PrachConfig->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences;
    
    for( unusedRootSequenceIndex =0; unusedRootSequenceIndex< unusedRootSequenceCount; unusedRootSequenceIndex++)
     {     
        p_PrachConfig->unusedRootSequencesList[prachFdOccasionIndex][unusedRootSequenceIndex]=1;
    
     }
    }

    p_PrachConfig->ssbPerRach= 100;

   ///////////////////////////////////////////////////////////////////////////////////////////
   // Table 3-35 : MULTI PRACH configuration
   
    p_MultiPrachConfig = &p_FapiConfigInfo->multiPrachConfig;
    p_MultiPrachConfig->tagBitmask |= CONFIG_TAG_BITMASK_MULTIPLE_PRACH_CONFIGURATION;
    
    p_MultiPrachConfig->numPrachConfigurations = 1;
    numPrachConfigurations= p_MultiPrachConfig->numPrachConfigurations;
    
    for (multiPrachConfigInfoIndex = 0; multiPrachConfigInfoIndex < numPrachConfigurations; multiPrachConfigInfoIndex++)
   {
         
         p_PrachConfigInMulti = &p_MultiPrachConfig->prachConfig[multiPrachConfigInfoIndex];
         p_PrachConfigInMulti->prachConfigInfo.prachResConfigIndex = 1;
         p_PrachConfigInMulti->prachConfigInfo.prachSequenceLength = 222;
         p_PrachConfigInMulti->prachConfigInfo.prachSubCSpacing = 1;
         p_PrachConfigInMulti->prachConfigInfo.ulBwpPuschScs = 0;
         p_PrachConfigInMulti->prachConfigInfo.restrictedSetConfig = 0;
         p_PrachConfigInMulti->prachConfigInfo.numPrachFdOccasions=1;
         p_PrachConfigInMulti->prachConfigInfo.prachConfigIndex = 161;
        
         numPrachFdOccasions = p_PrachConfigInMulti->prachConfigInfo.numPrachFdOccasions;
    
         
         for (prachFdOccasionIndex = 0; prachFdOccasionIndex < numPrachFdOccasions; prachFdOccasionIndex++)
         {  
            p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].prachRootSequenceIndex=3;
            p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].numRootSequences=3;
            p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].prachZeroCorrConf = 1;
            p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences= 1;
        
            unusedRootSequenceCount = p_PrachConfigInMulti->prachOccasionInfo[prachFdOccasionIndex].numUnusedRootSequences;
      
            for (unusedRootSequenceIndex = 0; unusedRootSequenceIndex < unusedRootSequenceCount; unusedRootSequenceIndex++)
                {
                p_PrachConfigInMulti->unusedRootSequencesList[prachFdOccasionIndex][unusedRootSequenceIndex]=11;
                }
              
            }   
       
          p_PrachConfigInMulti->ssbPerRach=32;
      }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-42 : MSGA PUSCH configuration
    
    p_MsgAPuschConfig =&p_FapiConfigInfo->msgApuschConfig;
    p_MsgAPuschConfig->tagBitmask |= CONFIG_TAG_BITMASK_MSGA_PUSCH_CONFIGURATION;
    p_MsgAPuschConfigInfo = &p_MsgAPuschConfig->msgAPuschConfigInfo;
     
    p_MsgAPuschConfigInfo->msgApuschResConfigIndex=1;
    p_MsgAPuschConfigInfo->msgAprachResConfigIndex=2;
    p_MsgAPuschConfigInfo->msgAGroupAorB=3;
    p_MsgAPuschConfigInfo->nIdMsgAPusch=4;
    p_MsgAPuschConfigInfo->dmrsPorts=1;
    p_MsgAPuschConfigInfo->availableDmrsSequenceIds=2;
    p_MsgAPuschConfigInfo->puschDmrsScramblingIds[0]=3;
    p_MsgAPuschConfigInfo->numPuschOcasFd=2;
    p_MsgAPuschConfigInfo->numPuschOcasTd=2;
    p_MsgAPuschConfigInfo->msgAPuschTimeDomainOffset=2;
    p_MsgAPuschConfigInfo->nPreambles=4;
    p_MsgAPuschConfigInfo->associationPatternPeriod = 1;

    p_AssociationPatternPeriodInfo = &p_MsgAPuschConfig->associationPatternPeriodInfo[0];
    numAssociationPatternPeriod = p_MsgAPuschConfigInfo->associationPatternPeriod;

    for (associationPatternPeriodIndex = 0; associationPatternPeriodIndex < numAssociationPatternPeriod; associationPatternPeriodIndex++)
    {
        p_AssociationPatternPeriodInfo->numPrachSlotsforPrachtoPruAndDmrsMap = 1;
        p_AssociationPatternPeriodInfo->prachSlotIndicesModulusApPslots = 1;

        p_AssociationPatternPeriodInfo++;
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-36 : SSB resource configuration table : 
    
    p_SsbResourceConfig = &p_FapiConfigInfo->ssbResourceConfig;
    p_SsbResourceConfig->tagBitmask |= CONFIG_TAG_BITMASK_SSB_TRANSMISSION_PATTERN;
    p_SsbResourceConfigInfo = &p_SsbResourceConfig->ssbResourceConfigInfo;
    p_SsbResourceConfigInfo->ssbConfigIndex =0;
    p_SsbResourceConfigInfo->ssbOffsetPointA = 1500;
    p_SsbResourceConfigInfo->betaPssProfileNR =1;
    p_SsbResourceConfigInfo->betaPssProfileSSS = 0;
    p_SsbResourceConfigInfo->ssbPeriod = 2; // 0: ms5 1:ms10 2:ms20 3:ms40 4:ms80 5:ms160
    p_SsbResourceConfigInfo->ssbSubcarrierOffset =0;
    p_SsbResourceConfigInfo->ssbFirstSymbolMappingCase = 2;
    p_SsbResourceConfigInfo->subcarrierSpacing =1;
    p_SsbResourceConfigInfo->ssbMaskSize = 2;
    if ( p_SsbResourceConfigInfo->ssbMaskSize ==2)
    {
      p_SsbResourceConfigInfo->ssbMask[0]=1;
      p_SsbResourceConfigInfo->ssbMask[1]=1;
    }
    else
    {
        for (beamIdx= 0; beamIdx <MAX_SSB_MASK_SIZE_SIXTY_FOUR; beamIdx++)
        {        
           p_SsbResourceConfigInfo->beamId[beamIdx]=0;
        }
    }    
    p_SsbResourceConfigInfo->lMax = 8;
    p_SsbResourceConfigInfo->rmsiPresence = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-37 : Multi-SSB resource configuration table
    
    p_MultiSsbResourceConfig= &p_FapiConfigInfo->multiSsbResourceConfig;
    p_MultiSsbResourceConfig->tagBitmask |= CONFIG_TAG_BITMASK_MULTIPLE_PRACH_CONFIGURATION;
    p_MultiSsbResourceConfig->numSsbConfigurations = 1;
    numSsbConfigurations=p_MultiSsbResourceConfig->numSsbConfigurations;
    
    for (multiSsbResourceConfigIndex = 0; multiSsbResourceConfigIndex < numSsbConfigurations; multiSsbResourceConfigIndex++)
    {
                    
      p_SsbResourceConfigInMulti = &p_MultiSsbResourceConfig->ssbResourceConfig[multiSsbResourceConfigIndex];
     
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbConfigIndex =0;
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbOffsetPointA = 1500;
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.betaPssProfileNR =1;
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.betaPssProfileSSS = 0;
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbPeriod = 2; // 0: ms5 1:ms10 2:ms20 3:ms40 4:ms80 5:ms160
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbSubcarrierOffset =0;
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbFirstSymbolMappingCase = 2;
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.subcarrierSpacing =1;
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbMaskSize = 2;
      if ( p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbMaskSize ==2)
      {
       p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbMask[0]=1;
       p_SsbResourceConfigInMulti->ssbResourceConfigInfo.ssbMask[1]=1;
      }
      else
      {
        for (beamIdx= 0; beamIdx <MAX_SSB_MASK_SIZE_SIXTY_FOUR; beamIdx++)
        {        
           p_SsbResourceConfigInMulti->ssbResourceConfigInfo.beamId[beamIdx]=0;
        }
      }    
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.lMax = 8;
      p_SsbResourceConfigInMulti->ssbResourceConfigInfo.rmsiPresence = 0;            
    }

  
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-38 : TDD table

    p_TddConfig = &p_FapiConfigInfo->tddConfig;
    p_TddConfig->tagBitmask |= CONFIG_TAG_BITMASK_TDD_PATTERN;
    p_TddPatternInfo = &p_TddConfig->tddPatternInfo;
   
    p_TddPatternInfo->tddPeriod = 5; // 0:ms0p5 1:ms0p625 2: ms1 3:ms1p25 4:ms2 5:ms2p5 6: ms5 7:ms 10
    scs = p_PhyConfig->slotTimeDomainInfo.subcarrierSpacing;
   
    memset( p_TddPatternInfo->slotConfig, 0xFF, sizeof(p_TddPatternInfo->slotConfig) );
    tddPeriodBySlot = convertEnumToValue_tddPeriod(p_TddPatternInfo->tddPeriod, scs);
    for (slotIndex = 0; slotIndex < tddPeriodBySlot; slotIndex++)
    {
        for (symbolIndex = 0; symbolIndex < MAX_NUM_OF_SYMBOL_PER_SLOT; symbolIndex++)
        {
            if (slotIndex <= 2)
            {
                p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] = 0;
            }
            else if (slotIndex == 3)
            {
                if (symbolIndex <= 9)
                    p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] = 0;
                else if (symbolIndex <= 11)
                    p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] = 2;
                else
                    p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] = 1;
            }
            else
            {
                p_TddPatternInfo->slotConfig[slotIndex][symbolIndex] = 1;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-39 : Measurement configuration

    p_MeasurementConfig =&p_FapiConfigInfo->measurementConfig;
    p_MeasurementConfig->tagBitmask |= CONFIG_TAG_BITMASK_RSSI_MEASUREMENT_CONFIGURATION;
    p_MeasurementConfig->rssiMeasurement = 1;

    p_MeasurementConfig->tagBitmask |= CONFIG_TAG_BITMASK_RSRP_MEASUREMENT_CONFIGURATION;
    p_MeasurementConfig->rsrpMeasurement = 1;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-49 : UCI configuration

    p_UciConfig= &p_FapiConfigInfo->uciConfig;
    p_UciConfig->tagBitmask |= CONFIG_TAG_BITMASK_UCI_INDICATION_CONFIGURATION;
    p_UciConfig->numUci2Maps = 1;
    numUci2Maps = p_UciConfig->numUci2Maps;

    for (uci2MapsIndex = 0; uci2MapsIndex < numUci2Maps && uci2MapsIndex < MAX_NUM_UCI2_MAPS; uci2MapsIndex++)
    {
        p_UciConfig->uci2MapInfo[uci2MapsIndex].numPart1Params = 2;

        numPart1Params = p_UciConfig->uci2MapInfo[uci2MapsIndex].numPart1Params;

        sumOfSizesPart1Params = 0;
        for (part1ParamsIndex = 0; part1ParamsIndex < numPart1Params && part1ParamsIndex < MAX_NUM_OF_UCI_PART1_PARAMS; part1ParamsIndex++)
        {
            p_UciConfig->uci2MapInfo[uci2MapsIndex].sizesPart1Params[part1ParamsIndex] = 1;
            sumOfSizesPart1Params += p_UciConfig->uci2MapInfo[uci2MapsIndex].sizesPart1Params[part1ParamsIndex];
        }

        // Calculate mapSize as 2^sumOfSizesPart1Params
        mapSize = (sumOfSizesPart1Params > 0) ? (2 << (sumOfSizesPart1Params-1)) : 1;

        for(mapIndex = 0; mapIndex < mapSize && mapIndex < MAX_UCI2_MAP_SIZE; mapIndex++)
        {
            p_UciConfig->uci2MapInfo[uci2MapsIndex].map[mapIndex] = mapIndex;
        }              
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-50 : PRB-symbol rate match patterns bitmap (non-CORESET) configuration

    p_PrbSymbRateMatchPatternsConfig = &p_FapiConfigInfo->prbSymbRateMatchPatternsConfig;
    p_PrbSymbRateMatchPatternsConfig->tagBitmask |= CONFIG_TAG_BITMASK_PRB_SYMBOL_RATE_MATCH_PATTERN;
    p_PrbSymbRateMatchPatternsConfig->numberOfPrbSymbRateMatchPatterns = 1;

    numberOfPrbSymbRateMatchPatterns=p_PrbSymbRateMatchPatternsConfig->numberOfPrbSymbRateMatchPatterns;
    for (prbSymbRateMatchPatternInfoIndex = 0; prbSymbRateMatchPatternInfoIndex< numberOfPrbSymbRateMatchPatterns; prbSymbRateMatchPatternInfoIndex++)
    {
        p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex].prbSymbRateMatchPatternID = 0;
        
        for (freqDomainRBIndex = 0; freqDomainRBIndex < MAX_RATEMATCH_BITMAP_SIZE; freqDomainRBIndex++)
        {
            p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex].freqDomainRB[freqDomainRBIndex] = 0;
        }
        p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex].symbolsInRB=0;
        p_PrbSymbRateMatchPatternsConfig->prbSymbRateMatchPatternInfo[prbSymbRateMatchPatternInfoIndex].subcarrierSpacing =1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-51 : CORESET rate match patterns bitmap configuration

    p_CoresetRateMatchPatternsConfig = &p_FapiConfigInfo->coresetRateMatchPatternsConfig;
    p_CoresetRateMatchPatternsConfig->tagBitmask |= CONFIG_TAG_BITMASK_CORESET_RATE_MATCH_PATTERN;
    p_CoresetRateMatchPatternsConfig->numberOfCoresetRateMatchPatterns = 1;

    numberOfCoresetRateMatchPatterns = p_CoresetRateMatchPatternsConfig->numberOfCoresetRateMatchPatterns;
    for (coresetRateMatchPatternInfoIndex = 0; coresetRateMatchPatternInfoIndex < numberOfCoresetRateMatchPatterns; coresetRateMatchPatternInfoIndex++)
    {
        p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex].coresetRateMatchPatternID = 0;
       
        for (freqDomainRBIndex = 0; freqDomainRBIndex < MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE; freqDomainRBIndex++)
        {
            p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex].freqDomainResources[freqDomainRBIndex] = 0;
        }
 
        p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex].symbolsPattern = 0;
        p_CoresetRateMatchPatternsConfig->coresetRateMatchPatternInfo[coresetRateMatchPatternInfoIndex].subcarrierSpacing = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-42 : LTE-CRS rate match patterns configuration

    p_LteCrsRateMatchPatternsConfig = &p_FapiConfigInfo->lteCrsRateMatchPatternsConfig;
    p_LteCrsRateMatchPatternsConfig->tagBitmask |= CONFIG_TAG_BITMASK_LTE_CRS_RATE_MATCH_PATTERN;
    p_LteCrsRateMatchPatternsConfig->numberOfLteCrsRateMatchPatterns= 1;

    lteCrsRateMatchPatternsCount = p_LteCrsRateMatchPatternsConfig->numberOfLteCrsRateMatchPatterns;
    for (lteCrsRateMatchPatternsIndex = 0; lteCrsRateMatchPatternsIndex < lteCrsRateMatchPatternsCount; lteCrsRateMatchPatternsIndex++)
    {
        p_LteCrsRateMatchPatternInfo = &(p_LteCrsRateMatchPatternsConfig->lteCrsRateMatchPatternInfo[lteCrsRateMatchPatternsIndex]);
        p_LteCrsRateMatchPatternInfo->crsRateMatchPatternID = 3;
        p_LteCrsRateMatchPatternInfo->carrierFreqDL = 3 ;
        p_LteCrsRateMatchPatternInfo->carrierBandwidthDL = 3;
        p_LteCrsRateMatchPatternInfo->nrofCrsPorts = 1;
        p_LteCrsRateMatchPatternInfo->vShift = 2;

        p_LteCrsRateMatchPatternInfo->sizeMbsfnSubframeConfigList = 1;

        mbsfnSubframeConfigCount= p_LteCrsRateMatchPatternInfo->sizeMbsfnSubframeConfigList; 
        for (mbsfnSubframeConfigIndex = 0; mbsfnSubframeConfigIndex < mbsfnSubframeConfigCount; mbsfnSubframeConfigIndex++)
        {
            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].radioframeAllocationPeriod = 1;
            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].radioframeAllocationOffset = 1;
            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].lteFrameStructureType = 1;
            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].subframeAllocLength = 2;
            p_LteCrsRateMatchPatternInfo->mbsfnSubframeConfigInfo[mbsfnSubframeConfigIndex].subframeAllocationBitmap = 2;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-43 : PUCCH semi-static configuration 

    p_PucchSemiStaticConfig= &p_FapiConfigInfo->pucchSemiStaticConfig;
    p_PucchSemiStaticConfig->tagBitmask |= CONFIG_TAG_BITMASK_PUCCH_SEMI_STATIC_PARAMETER;
    p_PucchSemiStaticConfig->numUlBwpIds = 1;
    
    for (ulBwpIndex = 0; ulBwpIndex < p_PucchSemiStaticConfig->numUlBwpIds; ulBwpIndex++)
    {    
       p_PucchSemiStaticConfigInfo = &p_PucchSemiStaticConfig->pucchSemiStaticConfigInfo[ulBwpIndex];
       p_PucchSemiStaticConfigInfo->pucchGroupHopping = 7;
       p_PucchSemiStaticConfigInfo->nIdPucchHopping = 9;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-44 : PDSCH semi-static configuration  
    
    p_PdschSemiStaticConfig = &p_FapiConfigInfo->pdschSemiStaticConfig;
    p_PdschSemiStaticConfig->tagBitmask|= CONFIG_TAG_BITMASK_PDSCH_SEMI_STATIC_PARAMETER;
    p_PdschSemiStaticConfig->pdschCbgScheme =0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-45 : Delay management configuration 

    p_DelayManagementConfig = &p_FapiConfigInfo->delayManagementConfig;
    p_DelayManagementConfig->tagBitmask |=CONFIG_TAG_BITMASK_TIMING_WINDOW;
    p_DelayManagementConfig->timingWindow= 0;
    p_DelayManagementConfig->tagBitmask |=CONFIG_TAG_BITMASK_TIMING_INFO_MODE;
    p_DelayManagementConfig->timingInfoMode =1;
    p_DelayManagementConfig->tagBitmask|=CONFIG_TAG_BITMASK_TIMING_INFO_PERIOD;
    p_DelayManagementConfig->timingInfoPeriod =1;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Table 3-46 : Rel-16 mTRP configuration

    p_Rel16mTRPConfig = &p_FapiConfigInfo->rel16mTRPConfig;
    p_Rel16mTRPConfig->tagBitmask |= CONFIG_TAG_BITMASK_NUM_TX_PORTS_TRP1;
    p_Rel16mTRPConfig->numTxPortsTRP1 =1;
    p_Rel16mTRPConfig->tagBitmask |= CONFIG_TAG_BITMASK_NUM_RX_PORTS_TRP1;
    p_Rel16mTRPConfig->numRxPortsTRP1 =1;
    
}    


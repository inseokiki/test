/**********************************************************************
 * SmartRAN CONFIDENTIAL
 * Copyright 2021-2025 SmartRAN All Rights Reserved.
 * gSM PHY parameter according to SCF 5G FAPI:PHY API(SCF222.10.04, 2021-11)
 * file gSM_l1_l2_api.h
 * Version: PHY_22.07
 ***********************************************************************/

#ifndef _GSM_L1_L2_API_H_
#define _GSM_L1_L2_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

//PHY API message types

#define MSG_PHY_PARAM_REQ                                         (0x00)
#define MSG_PHY_PARAM_RESP                                        (0x01)
#define MSG_PHY_CONFIG_REQ                                        (0x02)
#define MSG_PHY_CONFIG_RESP                                       (0x03)
#define MSG_PHY_START_REQ                                         (0x04)
#define MSG_PHY_START_RESP                                        (0x108)
#define MSG_PHY_STOP_REQ                                          (0x05)
#define MSG_PHY_STOP_IND                                          (0x06)
#define MSG_PHY_ERROR_IND                                         (0x07)
#define MSG_PHY_DL_TTI_REQ                                        (0x80)
#define MSG_PHY_DL_TTI_RESP                                       (0x8a)
#define MSG_PHY_UL_TTI_REQ                                        (0x81)
#define MSG_PHY_SLOT_IND                                          (0x82)
#define MSG_PHY_UL_DCI_REQ                                        (0x83)
#define MSG_PHY_TX_DATA_REQ                                       (0x84)
#define MSG_PHY_RX_DATA_IND                                       (0x85)
#define MSG_PHY_CRC_IND                                           (0x86)
#define MSG_PHY_UCI_IND                                           (0x87)
#define MSG_PHY_SRS_IND                                           (0x88)
#define MSG_PHY_RACH_IND                                          (0x89)
 
// for internal use                                                                
#define MSG_TEST_CONFIG_REQ                                       (0xF0)
#define MSG_TEST_CONFIG_RESP                                      (0xF1)
#define MSG_PHY_TX_DATA_BLOCK0                                    (0xF2)
#define MSG_PHY_TX_DATA_BLOCK1                                    (0xF3)
#define MAX_RX_DATA_COUNT_IN_SLOT                                 (32)

// List of TAGs for PARAM.req                                    
                                                                 
#define TAG_PROTOCOL_VERSION                                      (0x0037)
                                                                 
// List of TAGs for PARAM.resp                                   
                                                           
//  Tags in Table 3-13                                    
                                                           
#define TAG_RELEASE_CAPABILITY                                    (0x0001)
#define TAG_PHY_STATE                                             (0x0002)
#define TAG_SKIP_BLANK_DL_CONFIG                                  (0x0003)
#define TAG_SKIP_BLANK_UL_CONFIG                                  (0x0004)
#define TAG_NUM_CONFIG_TLVS_TO_REPORT                             (0x0005)
#define TAG_POWER_PROFILES                                        (0x0038)
#define TAG_MAX_NUM_PDUS_IN_DL_TTI                                (0x0039)
#define TAG_SSPBCH_BLOCK_POWER_SCALING_UNIT_CHOICE                (0x0058)
#define TAG_MAX_NUM_PDUS_IN_UL_TTI                                (0x003A)
#define TAG_MAX_NUM_PDUS_IN_UL_DCI                                (0x003B)
                                                           
//  Tags in Table 3-13                                     
                                                           
#define TAG_CYCLIC_PREFIX                                         (0x0006)
#define TAG_SUPPORTED_SUBCARRIER_SPACING_DL                       (0x0007)
#define TAG_SUPPORTED_SUBCARRIER_SPACING_SSB                      (0X0059)
#define TAG_SUPPORTED_BANDWIDTH_DL                                (0x0008)
#define TAG_SUPPORTED_SUBCARRIER_SPACING_UL                       (0x0009)
#define TAG_SUPPORTED_BANDWIDTH_UL                                (0x000A)
#define TAG_SS_PBCH_MULTIPLE_CARRIERS_IN_A_BAND                   (0x003C)
#define TAG_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRIER                   (0x003D)
#define TAG_MEANING_OF_CARRIER_FREQUENCY                          (0x005A) 

                                                          
//  Tags in Table 3-15                                     
                                                           
#define TAG_CCE_MAPPING_TYPE                                      (0x000B)
#define TAG_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS_OF_SLOT             (0x000C)
#define TAG_PRECODER_GRANULARITY_CORESET                          (0x000D)
#define TAG_PDCCH_MU_MIMO                                         (0x000E)
#define TAG_PDCCH_PRECODER_CYCLING                                (0x000F)
#define TAG_MAX_PDCCHS_PER_SLOT                                   (0x0010)
                                                           
//  Tags in Table 3-16                                     
                                                           
#define TAG_PUCCH_FORMATS                                         (0x0011)
#define TAG_MAX_PUCCHS_PER_SLOT                                   (0x0012)
#define TAG_PUCCH_GROUP_AND_SEQUENCE_HOPPING                      (0x003E)
#define TAG_MAX_NUM_UL_BWP_IDS                                    (0x003F)
#define TAG_PUCCH_AGGREGATION                                     (0x0040)
                                                           
//  Tags in Table 3-17                                     
                                                           
#define TAG_PDSCH_MAPPING_TYPE                                    (0x0013)
#define TAG_PDSCH_ALLOCATION_TYPES                                (0x0014)
#define TAG_PDSCH_VRB_TO_PRB_MAPPING                              (0x0015)
#define TAG_PDSCH_CBG                                             (0x0016)
#define TAG_PDSCH_DMRS_CONFIG_TYPES                               (0x0017)
#define TAG_PDSCH_DMRS_MAX_LENGTH                                 (0x0018)
#define TAG_PDSCH_DMRS_ADDITIONAL_POS                             (0x0019)
#define TAG_MAX_PDSCHS_TBS_PER_SLOT                               (0x001A)
#define TAG_MAX_NUMBER_MIMO_LAYERS_PDSCH                          (0x001B)
#define TAG_SUPPORTED_MAX_MODULATION_ORDER_DL                     (0x001C)
#define TAG_MAX_MU_MIMO_USERS_DL                                  (0x001D)
#define TAG_PDSCH_DATA_IN_DMRS_SYMBOLS                            (0x001E)
#define TAG_PREMPTION_SUPPORT                                     (0x001F)
#define TAG_PDSCH_NON_SLOT_SUPPORT                                (0x0020)
#define TAG_SSB_RATE_MATCH                                        (0x0041)
#define TAG_SUPPORTED_RATE_MATCH_PATTERN_TYPE                     (0x0042)
#define TAG_PDCCH_RATE_MATCH                                      (0x0043)
#define TAG_NUM_OF_RATE_MATCH_PATTERN_LTE_CRS                     (0x0044)
#define TAG_NUM_OF_RATE_MATCH_PATTER_LTE_CRS_IN_PHY               (0x0045)
#define TAG_LTE_CRS_RATEMATCH_MB_SFN_DERIVATION                   (0x005B)
#define TAG_SUPPORTED_LTE_CRS_RATEMATCH_METHOD                    (0X005C)
#define TAG_CSI_RS_RATE_MATCH                                     (0x0046)
#define TAG_PDSCH_TRANS_TYPE_SUPPORT                              (0x0047)
#define TAG_PDSCH_MAC_PDU_BIT_ALIGNMENT                           (0x0048)
#define TAG_MAX_NUMBER_PRB_SYM_BITMAPS_PER_SLOT                   (0x005D)
#define TAG_MAX_NUMBER_CSIRS_RATEMATCH_PER_SLOT                   (0x005E)
#define TAG_MAX_NUM_SSB_PER_PDSCH_SLOT                            (0x005F)
#define TAG_UNIVERSAL_RATEMATCH                                   (0x0060)
#define TAG_REQUIRE_PDSCH_SIGNALING_ASSOCIATIONS                  (0x0061)

                                                           
//  Tags in Table 3-18                                     
                                                           
#define TAG_UCI_MUX_ULSCH_IN_PUSCH                                (0x0021)
#define TAG_UCI_ONLY_PUSCH                                        (0x0022)
#define TAG_PUSCH_FREQUENCY_HOPPING                               (0x0023)
#define TAG_PUSCH_DMRS_CONFIG_TYPES                               (0x0024)
#define TAG_PUSCH_DMRS_MAX_LEN                                    (0x0025)
#define TAG_PUSCH_DMRS_ADDITIONAL_POS                             (0x0026)
#define TAG_PUSCH_CBG                                             (0x0027)
#define TAG_PUSCH_MAPPING_TYPE                                    (0x0028)
#define TAG_PUSCH_ALLOCATION_TYPE                                 (0x0029)
#define TAG_PUSCH_VRB_TO_PRB_MAPPING                              (0x002A)
#define TAG_PUSCH_MAX_PTRS_PORTS                                  (0x002B)
#define TAG_MAX_PDUSCHS_TBS_PER_SLOT                              (0x002C)
#define TAG_MAX_NUMBER_MIMO_LAYERS_NON_CB_PUSCH                   (0x002D)
#define TAG_MAX_NUMBER_MIMO_LAYERS_CB_PUSCH                       (0x0049)
#define TAG_SUPPORTED_MAX_MODULATION_ORDER_UL                     (0x002E)
#define TAG_MAX_MU_MIMO_USERS_UL                                  (0x002F)
#define TAG_DFTS_OFDM_SUPPORT                                     (0x0030)
#define TAG_PUSCH_AGGREGATION_FACTOR                              (0x0031)
#define TAG_PUSCH_LBRM_SUPPORT                                    (0x004A)
#define TAG_PUSCH_TRANS_TYPE_SUPPORT                              (0x004B)
#define TAG_PUSCH_MAC_PDU_BIT_ALIGNMENT                           (0x004C)
                                                           
//  Tags in Table 3-19                                     
                                                           
#define TAG_MSGA_NROF_PUSCH_RESCONFIG                             (0x0062)
#define TAG_MSGA_PUSCH_TRAN_PRECODING                             (0x0063)
#define TAG_MSGA_INTRASLOT_PUSCH_FREQHOPH                         (0x0064)
#define TAG_MSGA_MAX_PUSCH_FD_OCCASIONS                           (0x0065)
#define TAG_MSGA_GUARD_BAND                                       (0x0066)
#define TAG_MSGA_GUARD_PERIOD                                     (0x0067)
#define TAG_MSGA_PUSCH_MAPPING_TYPE                               (0x0068)
#define TAG_MSGA_PUSCH_DMRS_MAX_LEN                               (0x0069)
#define TAG_MSGA_PUSCH_DMRS_CDM_GROUP                             (0x006A)
#define TAG_MSGA_PUSCH_DMRS_ADDITIONAL_POS                        (0x006B)
#define TAG_MSGA_SUPPORTED_MAX_MODULATION_ORDER_UL                (0x006C)
#define TAG_MSGA_MAX_PUSCH_PER_PRU                                (0x006D)
#define TAG_MSGA_MAX_NROF_CDM_GROUP                               (0x006E)
#define TAG_MSGA_MAX_NROF_DMRS_PORT                               (0x006F)
#define TAG_MSGA_PRACH_TO_PUSCH_MAPPING_SCHEME                    (0x0070)


//  Tags in Table 3-20

#define TAG_PRACH_LONG_FORMATS                                    (0x0032)
#define TAG_PRACH_SHORT_FORMATS                                   (0x0033)
#define TAG_PRACH_RESTRICTED_SETS                                 (0x0034)
#define TAG_MAX_PRACH_FD_OCCASIONS_IN_A_SLOT                      (0x0035)
#define TAG_MAX_NUM_PRACH_CONFIGURATIONS                          (0x004D)
#define TAG_PRACH_MULTIPLE_CARRIERS_IN_A_BAND                     (0x004E)
#define TAG_PRACH_FREQUENCY_OFFSET_MEANING                        (0x0071)
#define TAG_MAX_NUM_TD_PRACH_OCCASION_PER_SLOT                    (0x0072)
#define TAG_PRACH_PRACH_ROOT_PROCESSING_RATE                      (0x0073)
#define TAG_PRACH_OCCASION_QUEUE_SIZE                             (0x0074)

//  Tags in Table 3-21

#define TAG_MSGA_MAX_NUM_PRACH_CONFIGURATIONS                     (0X0075)
#define TAG_MSGA_PRACH_LONG_FORMATS                               (0X0076)
#define TAG_MSGA_PRACH_SHORT_FORMATS                              (0X0077)
#define TAG_MSGA_PRACH_RESTRICTED_SLOT                            (0X0078)
#define TAG_MSGA_MAX_PRACH_FD_OCCASION_IN_A_SLOT                  (0X0079)
#define TAG_MSGA_PRACH_MULTIPLE_CARRIERS_IN_A_BAND                (0X007A)


//  Tags in table 3-22
                                                           
#define TAG_RSSI_MEASUREMENT_SUPPORT                              (0x0036)
#define TAG_RSRP_MEASUREMENT_SUPPORT                              (0x007B)
                                                           
//  Tags in Table 3-23                                     
                                                           
#define TAG_MAX_NUM_UCI_MAPS                                      (0x004F)
#define TAG_UCI_REPORT_FORMAT_PUCCH_FORMAT_234                    (0x007C)                                                           

//  Tags in Table 3-24                                    
                                                           
#define TAG_CAPABILITIES                                          (0x0050)
                                                           
//  Tags in Table 3-25:                                     
                                                           
#define TAG_SUPPORTED_PHY_PROFILES                                (0x0051)
#define TAG_TIME_MANAGEMENT                                       (0x0052)
#define TAG_FAPI_PROTOCOL_VERSION                                 (0x0053)
#define TAG_MORE_THAN_ONE_INDICATION_PER_SLOT                     (0x0055)
#define TAG_MORE_THAN_ONE_REQUEST_PER_SLOT                        (0x0056)
                                                           
//  Tags in Table 3-26                                     
                                                           
#define TAG_PAIRING_DFE_PROFILES                                  (0x0054)
                                                           
//  Tags in Table 3-27                                     
                                                           
#define TAG_DL_TTI_TIMING_OFFSET                                  (0x0106)
#define TAG_UL_TTI_TIMING_OFFSET                                  (0x0107)
#define TAG_UL_DCI_TIMING_OFFSET                                  (0x0108)
#define TAG_TX_DATA_TIMING_OFFSET                                 (0x0109)
#define TAG_TIMING_WINDOW                                         (0x011E)
#define TAG_TIMING_INFO_PERIOD                                    (0x0120)
                                                           
//  Tags in Table 3-28                                    
                                                           
#define TAG_MTRP_SUPPORT                                          (0x0057)
 
//  Tags in Table 3-29

#define TAG_USER_PLANE_ENCAPSULATION_CONTROL_SEPARATION           (0x007D)
#define TAG_MESSAGE_ENCODING_SUPPORT                              (0x007E)

//  Tags in Table 3-30

#define TAG_SUPPORTED_SRS_USAGE_AND_PAIRING                       (0x0080)
#define TAG_RB_SUB_SAMPLING_RESOLUTION_FOR_SRS_DERIVED_REOPORT    (0x0081)
#define TAG_MAX_SRS_CHANNEL_REPORTS_PER_SLOT                      (0x0082)
#define TAG_MAX_SRS_PDUS_PER_SLOT                                 (0x0083)
#define TAG_IQ_SAMPLE_FORMATS                                     (0x0084)
#define TAG_SINGULAR_VALUE_FOMRATS                                (0x0085)
#define TAG_MAX_SRS_TX_ANT_PORTS_PER_UE                           (0x0086)
#define TAG_MAX_NUM_CONSECUTIVE_SRS_SLOTS                         (0x0087)
#define TAG_MAX_DUTY_CYCLE_IN_PERCENTAGE                          (0x0088)
#define TAG_MAX_SRS_COMB_SIZE                                     (0x0089)
#define TAG_MAX_NUM_SRS_CYCLIC_SHIFTS                             (0x008A)
#define TAG_SRS_SYMBOLS_BITMAP                                    (0x008B)
#define TAG_MAX_NUM_SRS_SYMB_PER_SLOT                             (0x008C)

//  Tags in Table 3-31

#define TAG_MAX_NUM_CARRIER_BW_LAYERS_PRODUCT_DL                  (0x008E)
#define TAG_MAX_NUM_CARRIER_BW_LAYERS_PRODUCT_UL                  (0x008F)
#define TAG_MAX_NUM_CARRIER_BW_ANTENNAS_PRODUCT_DL                (0x0090)
#define TAG_MAX_NUM_CARRIER_BW_ANTENNAS_PRODUCT_UL                (0x0091)
#define TAG_MU_MIMO_DL                                            (0x0092)
#define TAG_SUPPORTED_SPATIAL_MUX_PER_DL_CHANNEL_TYPE             (0x0093)
#define TAG_MAX_NUM_MU_MIMO_LAYERS_PER_DL_CHANNEL_TYPE            (0x0094)
#define TAG_CROSS_CHANNEL_SPATIAL_MULTIPLEXING_FOR_DL             (0x0095)
#define TAG_MU_MIMO_UL                                            (0x0096)
#define TAG_SUPPORTED_SPATIAL_MUX_PER_UL_CHANNEL_TYPE             (0x0097)
#define TAG_MAX_NUM_MU_MIMO_LAYERS_PER_UL_CHANNEL_TPYE            (0x0098)
#define TAG_CROOS_CHANNEL_SPATIAL_MULTIFLEXING_FOR_UL             (0x0099)
#define TAG_MAX_MU_MIMO_GROUPS_IN_FD_PER_DL_CHANNEL               (0x009A)
#define TAG_MAX_MU_MIMO_GROUPS_IN_TD_PER_DL_CHANNEL               (0x009B)
#define TAG_MAX_MU_MIMO_NEW_PRECODERS_PER_SLOT                    (0x009C)
#define TAG_ALL_PRB_GROUPS_PRGS                                   (0x009D)
#define TAG_MAX_MU_MIMO_GROUPS_IN_FD_PER_UL_CHANNEL               (0x009E)
#define TAG_MAX_MU_MIMO_GROUPS_IN_TD_PER_UL_CHANNEL               (0x009F)
#define TAG_MIN_RB_RESOLUTION_FOR_SRS_DERIVED_REPORTS             (0x0150)
#define TAG_MAX_NUMBER_DL_SPATIAL_STREAMS                         (0x0151)
#define TAG_DL_SPATIAL_STREAM_CHANNEL_PRIORITY                    (0x0152)
#define TAG_MAX_NUMBER_UL_SPATIAL_STREAMS                         (0x0153)
#define TAG_UL_TPMI_CAPABILITY                                    (0x0154)

                                                          
// List of TAGs for CONFIG.response                        
                                                           
// Tags in Table 3-36                                      
                                                           
#define TAG_PHY_PROFILE_ID                                        (0x102A)
#define TAG_INDICATION_INSTANCES_PER_SLOT                         (0x102B)
#define TAG_REQUEST_INSTANCES_PER_SLOT                            (0x102C)
#define TAG_SLOT_TIME_DOMAIN_INFO                                 (0x103D)                                                          

// Tags in Table 3-37                                     
                                                           
#define TAG_CARRIER_CONFIGURATION                                 (0x102D)
                                                           
//  Tags in Table 3-38                                    
                                                           
#define TAG_PHY_CELL_ID                                           (0x100C)
#define TAG_FRAME_DUPLEX_TYPE                                     (0x100D)
#define TAG_PDSCH_TRANS_TYPE_VALIDITY                             (0x102E)
#define TAG_PUSCH_TRANS_TYPE_VALIDITY                             (0x102F)
                                                           
//  Tags in Table 3-39                                    
                                                           
#define TAG_SS_PBCH_POWER                                         (0x100E)
#define TAG_SS_PBCH_BLOCK_POWER_SCALING                           (0x1030)
#define TAG_BCH_PAYLOAD_FLAG                                      (0x100F)
                                                           
//  Tags in Table 3-40                                     
                                                           
#define TAG_PRACH_RESOURCE_CONFIGURATION                          (0x1031)
                                                           
//  Tags in Table 3-41                                     
                                                           
#define TAG_MULTIPLE_PRACH_CONFIGURATION                          (0x1032)

//  Tags in Table 3-42

#define TAG_MSGA_PUSCH_CONFIGURATION                              (0x103E)

//  Tags in Table 3-44

#define TAG_MULTI_MSGA_PUSCH_CONFIGURATION                        (0x103F)

                                                           
//  Tags in Table 3-45                                     
                                                           
#define TAG_SSB_TRANSMISSION_PATTERN                              (0x1033)
                                                           
//  Tags in Table 3-46                                     
                                                           
#define TAG_MULTIPLE_SSB_CONFIGURATION                            (0x1034)
                                                           
//  Tags in Table 3-47                                     
                                                           
#define TAG_TDD_PATTERN                                           (0x1035)
                                                           
//  Tags in Table 3-48                                     
                                                           
#define TAG_RSSI_MEASUREMENT_CONFIGURATION                        (0x1028)
#define TAG_RSRP_MEASUREMENT_CONFIGURATION                        (0x1040)

//  Tags in Table 3-49                              
                                                           
#define TAG_UCI_INDICATION_CONFIGURATION                          (0x1036)
                                                           
//  Tags in Table 3-50                                     
                                                           
#define TAG_PRB_SYMBOL_RATE_MATCH_PATTERN                         (0x1037)

//  Tags in Table 3-51

#define TAG_CORESET_RATE_MATCH_PATTERN                            (0x1041)
                                                           
//  Tags in Table 3-52                                     
                                                           
#define TAG_LTE_CRS_RATE_MATCH_PATTERN                            (0x0138)
                                                           
//  Tags in Table 3-53                                     
                                                           
#define TAG_PUCCH_SEMI_STATIC_PARAMETER                           (0x1039)
                                                           
//  Tags in Table 3-54                                     
                                                           
#define TAG_PDSCH_SEMI_STATIC_PARAMETER                           (0x103A)
                                                           
//  Tags in Table 3-55                                     
                                                           
#define TAG_TIMING_WINDOW                                         (0x011E)
#define TAG_TIMING_INFO_MODE                                      (0x011F)
#define TAG_TIMING_INFO_PERIOD                                    (0x0120)
                                                           
//  Tags in Table 3-56                                     
                                                           
#define TAG_NUM_TX_PORTS_TRP1                                     (0x103B)
#define TAG_NUM_RX_PORTS_TRP1                                     (0x103C)

                                                          
// List of Values in TLV for PARAM.req                           
                                                                 
#define FAPI_VERSION_V3                                           (3)
                                                                 
// List of Values in TLV for PARAM.resp : Table 3-12

#define PARAM_MSG_OK                                              (0x00)
#define PARAM_MSG_INVALID_STATE                                   (0x01)
 
#define BITMASK_REL_CAPABILITY_SUPPORT_REL15                      (0x0001)
#define BITMASK_REL_CAPABILITY_SUPPORT_REL16                      (0x0002)

#define PHY_STATE_IDLE                                            (0x0000)
#define PHY_STATE_CONFIGURED                                      (0x0001)
#define PHY_STATE_RUNNING                                         (0x0002)
                                                                 
#define FEATURE_NOT_SUPPORTED                                     (0)
#define FEATURE_SUPPORTED                                         (1)

#define IDLE_ONLY_OPTIONAL                                        (0)
#define IDLE_ONLY_MANDATORY                                       (1)
#define IDLE_CONFIGURED_OPTIONAL                                  (2)
#define IDLE_MANDATORY_CONFIGURED_OPTIONAL                        (3)
#define IDLE_CONFIGURED_RUNNING_OPTIONAL                          (4)
#define IDLE_MANDATORY_CONFIGURED_RUNNING_OPTIONAL                (5)

#define BITMASK_POWER_PROFILE_SUPPORT_NR                          (0x01)
#define BITMASK_POWER_PROFILE_SUPPORT_SSS                         (0x02)

#define POWER_PROFILE_SIGNAL_INDEX_COUNT                          (8)
#define POWER_PROFILE_SIGNAL_INDEX_PDCCH_DMRS                     (0)
#define POWER_PROFILE_SIGNAL_INDEX_PDCCH_DATA                     (1)
#define POWER_PROFILE_SIGNAL_INDEX_PDSCH_DMRS                     (2)
#define POWER_PROFILE_SIGNAL_INDEX_PDSCH_DATA                     (3)
#define POWER_PROFILE_SIGNAL_INDEX_PDSCH_PTRS                     (4)
#define POWER_PROFILE_SIGNAL_INDEX_CSI_RS                         (5)
#define POWER_PROFILE_SIGNAL_INDEX_PSS                            (6)
#define POWER_PROFILE_SIGNAL_INDEX_SSS                            (7)
                                                           
#define REFERENCE_RS_PDCCH_DMRS_SSS_SUPPORT                       (1)
                                                           
#define REFERENCE_RS_PDCCH_DATA_SSS_SUPPORT                       (1)
#define REFERENCE_RS_PDCCH_DATA_PDCCH_DMRS_SUPPORT                (2)
                                                           
#define REFERENCE_RS_PDSCH_DMRS_SSS_SUPPORT                       (1)
                                                           
#define REFERENCE_RS_PDSCH_DATA_SSS_SUPPORT                       (1)
#define REFERENCE_RS_PDSCH_DATA_PDSCH_DMRS_SUPPORT                (2)
                                                           
#define REFERENCE_RS_PDSCH_PTRS_SSS_SUPPORT                       (1)
#define REFERENCE_RS_PDSCH_PTRS_PDSCH_DMRS_SUPPORT                (2)
                                                           
#define REFERENCE_RS_PDSCH_CSI_RS_SSS_SUPPORT                     (1)
#define REFERENCE_RS_PDSCH_CSI_RS_SSS_1_SUPPORT                   (2)
                                                           
#define REFERENCE_RS_PDSCH_PSS_SSS_SUPPORT                        (1)
#define REFERENCE_RS_PDSCH_PSS_SSS_1_SUPPORT                      (2)
                                                           
#define REFERENCE_RS_PDSCH_SSS_UNSCALED_SSS_SUPPORT               (1)
#define REFERENCE_RS_PDSCH_SSS_SCF_222_V20_SUPPORT                (2)
                                                           
#define POWER_0_01_SCALE_0_DB                                     (0)
#define POWER_0_001_SCALE_0_DB                                    (0)

// List of Values in TLV for PARAM.resp : Table 3-13

#define BITMASK_CYCLIC_PREFIX_SUPPORT_NORMAL                      (0x01)
#define BITMASK_CYCLIC_PREFIX_SUPPORT_EXTENDED                    (0x02)

#define BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_15_KHZ           (0X01)
#define BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_30_KHZ           (0X02)
#define BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_120_KHZ          (0X04)
#define BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB_240_KHZ          (0X08)

#define BITMASK_SUBCARRIER_SPACING_SUPPORT_15_KHZ                 (0x01)
#define BITMASK_SUBCARRIER_SPACING_SUPPORT_30_KHZ                 (0x02)
#define BITMASK_SUBCARRIER_SPACING_SUPPORT_60_KHZ                 (0x04)
#define BITMASK_SUBCARRIER_SPACING_SUPPORT_120_KHZ                (0x08)
#define BITMASK_SUBCARRIER_SPACING_SUPPORT_240_KHZ                (0x10)

#define BITMASK_BANDWIDTH_SUPPORT_5_MHZ                           (0x0001)
#define BITMASK_BANDWIDTH_SUPPORT_10_MHZ                          (0x0002)
#define BITMASK_BANDWIDTH_SUPPORT_15_MHZ                          (0x0004)
#define BITMASK_BANDWIDTH_SUPPORT_20_MHZ                          (0x0008)
#define BITMASK_BANDWIDTH_SUPPORT_25_MHZ                          (0x0010)
#define BITMASK_BANDWIDTH_SUPPORT_40_MHZ                          (0x0020)
#define BITMASK_BANDWIDTH_SUPPORT_50_MHZ                          (0x0040)
#define BITMASK_BANDWIDTH_SUPPORT_60_MHZ                          (0x0080)
#define BITMASK_BANDWIDTH_SUPPORT_70_MHZ                          (0x0100)
#define BITMASK_BANDWIDTH_SUPPORT_80_MHZ                          (0x0200)
#define BITMASK_BANDWIDTH_SUPPORT_90_MHZ                          (0x0400)
#define BITMASK_BANDWIDTH_SUPPORT_100_MHZ                         (0x0800)
#define BITMASK_BANDWIDTH_SUPPORT_200_MHZ                         (0x1000)
#define BITMASK_BANDWIDTH_SUPPORT_400_MHZ                         (0x2000)

#define BITMASK_SS_PBCH_MULTIPLE_CARRIER_IN_A_BAND_DISABLE        (0)
#define BITMASK_SS_PBCH_MULTIPLE_CARRIER_IN_A_BAND_ENABLE         (1)

#define BITMASK_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRRIER_DISABLE      (0)
#define BITMASK_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRRIER_ENABLE       (1)

#define BITMASK_MEANING_OF_CARRIER_FREQUENCY_POINT_A              (0)
#define BITMASK_MEANING_OF_CARRIER_FREQUENCY_CENTER               (1)

#define FEATURE_DISABLED                                          (0)
#define FEATURE_ENABLED                                           (1)


// List of Values in TLV for PARAM.resp : Table 3-15

#define BITMASK_CCE_MAPPING_TYPE_SUPPORT_INTERLEAVED              (0x01)
#define BITMASK_CCE_MAPPING_TYPE_SUPPORT_NON_INTERLEAVED          (0x02)

// List of Values in TLV for PARAM.resp : Table 3-16

#define BITMASK_PUCCH_SUPPORT_FORMAT_0                            (0x01)
#define BITMASK_PUCCH_SUPPORT_FORMAT_1                            (0x02)
#define BITMASK_PUCCH_SUPPORT_FORMAT_2                            (0x04)
#define BITMASK_PUCCH_SUPPORT_FORMAT_3                            (0x08)
#define BITMASK_PUCCH_SUPPORT_FORMAT_4                            (0x10)

#define PUCCH_GROUP_AND_SEQ_HOPPING_DYNAMIC_ONLY                  (0)
#define PUCCH_GROUP_AND_SEQ_HOPPING_SEMI_STATIC_ONLY              (1)
#define PUCCH_GROUP_AND_SEQ_HOPPING_BOTH                          (2)

#define BITMASK_PUCCH_AGGREGATION_REPETITION_OVER_MULTI_SLOTS     (0x01)

// List of Values in TLV for PARAM.resp :  Table 3-17

#define BITMASK_MAPPING_TYPE_SUPPORT_TYPE_A                       (0x01)
#define BITMASK_MAPPING_TYPE_SUPPORT_TYPE_B                       (0x02)

#define BITMASK_ALLOC_TYPE_SUPPORT_TYPE_0                         (0x01)
#define BITMASK_ALLOC_TYPE_SUPPORT_TYPE_1                         (0x02)

#define BITMASK_VRB_PRB_MAPPING_SUPPORT_NON_INTERLEAVED           (0x01)
#define BITMASK_VRB_PRB_MAPPING_SUPPORT_INTERLEAVED               (0x02)

#define PDSCH_CBG_NOT_SUPPORTED                                   (0)
#define PDSCH_CBG_SUPPORTED_SEGMENTATION_IN_L2                    (1)
#define PDSCH_CBG_SUPPORTED_SEGMENTATION_IN_L1                    (2)
#define PDSCH_CBG_SUPPORTED_EITHER_TYPE                           (3)

#define BITMASK_DMRS_CONFIG_SUPPORT_TYPE_1                        (0x01)
#define BITMASK_DMRS_CONFIG_SUPPORT_TYPE_2                        (0x02)

#define DMRS_MAX_LENGTH_1                                         (0)
#define DMRS_MAX_LENGTH_2                                         (1)

#define BITMASK_DMRS_ADD_POS_SUPPORT_POS_0                        (0x01)
#define BITMASK_DMRS_ADD_POS_SUPPORT_POS_1                        (0x02)
#define BITMASK_DMRS_ADD_POS_SUPPORT_POS_2                        (0x04)
#define BITMASK_DMRS_ADD_POS_SUPPORT_POS_3                        (0x08)
#define BITMASK_DMRS_ADD_POS_SUPPORT_DOUBLE_SYMBOL_POS_0          (0x10)
#define BITMASK_DMRS_ADD_POS_SUPPORT_DOUBLE_SYMBOL_POS_1          (0x20)

#define MAX_MODULATION_ORDER_QPSK                                 (0)
#define MAX_MODULATION_ORDER_16QAM                                (1)
#define MAX_MODULATION_ORDER_64QAM                                (2)
#define MAX_MODULATION_ORDER_256QAM                               (3)

#define SSB_RATE_MATCH_NOT_SUPPORTED                              (0)
#define SSB_RATE_MATCH_SUPPORTED_CONFIG_REF                       (1)
#define SSB_RATE_MATCH_SUPPORTED_PDU_INDEX_REF                    (2)
#define SSB_RATE_MATCH_SUPPORTED_EITHER                           (3)

#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_SCS_15KHZ              (0x0001)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_SCS_30KHZ              (0x0002)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_SCS_NCP_60KHZ          (0x0004)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_SCS_ECP_60KHZ          (0x0008)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_SCS_120KHZ             (0x0010)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_BY_REF                 (0x0020)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_BY_VALUE               (0x0040)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_CORESET_LVL_15KHZ      (0x0080)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_CORESET_LVL_30KHZ      (0x0100)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_CORESET_LVL_NCP_60KHZ  (0x0200)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_CORESET_LVL_ECP_60KHZ  (0x0400)
#define BITMASK_RATE_MATCH_PATTERN_SUPPORT_CORESET_LVL_120KHZ     (0x0800)

#define BTIMASK_LTE_CRS_RATE_MATCH_MB_SFN_DERIVATION_NOT_SUPPORT  (0x01)
#define BTIMASK_LTE_CRS_RATE_MATCH_MB_SFN_DERIVATION_SUPPORT      (0x02)


#define TRANS_TYPE_SUPPORT_CANNOT_INTERPRET                       (0)
#define TRANS_TYPE_SUPPORT_REQUIRE_VALID_TYPE                     (1)
#define TRANS_TYPE_SUPPORT_BOTH                                   (2)

#define MAC_PDU_BIT_ALIGNMENT_8BIT                                (1)
#define MAC_PDU_BIT_ALIGNMENT_16BIT                               (2)
#define MAC_PDU_BIT_ALIGNMENT_32BIT                               (3)
#define MAC_PDU_BIT_ALIGNMENT_64BIT                               (4)
#define MAC_PDU_BIT_ALIGNMENT_128BIT                              (5)
#define MAC_PDU_BIT_ALIGNMENT_256BIT                              (6)

#define BITMASK_PUSCH_DMRS_CONFIG_TYPE1                           (0x01)
#define BITMASK_PUSCH_DMRS_CONFIG_TYPE2                           (0x02)

#define PUSCH_DMRS_MAX_LENGTH1                                    (0)
#define PUSCH_DMRS_MAX_LENGTH2                                    (1)


#define BITMASK_PUSCH_AGGREGATION_SUPPORT_FACTOR1                 (0x01)
#define BITMASK_PUSCH_AGGREGATION_SUPPORT_FACTOR2                 (0x02)
#define BITMASK_PUSCH_AGGREGATION_SUPPORT_FACTOR4                 (0x04)
#define BITMASK_PUSCH_AGGREGATION_SUPPORT_FACTOR8                 (0x08)

#define MAX_NUMEROLOGY                                            (5)

// List of Values in TLV for PARAM.resp : Table 3-20

#define BITMASK_LONG_PRACH_SUPPORT_FORMAT_0                       (0x01)
#define BITMASK_LONG_PRACH_SUPPORT_FORMAT_1                       (0x02)
#define BITMASK_LONG_PRACH_SUPPORT_FORMAT_2                       (0x04)
#define BITMASK_LONG_PRACH_SUPPORT_FORMAT_3                       (0x08)

#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_A1                     (0x0001)
#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_A2                     (0x0002)
#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_A3                     (0x0004)
#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_B1                     (0x0008)
#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_B2                     (0x0010)
#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_B3                     (0x0020)
#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_B4                     (0x0040)
#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_C0                     (0x0080)
#define BITMASK_SHORT_PRACH_SUPPORT_FORMAT_C2                     (0x0100)

#define PRACH_RESTRICTED_SETS_NOT_SUPPORTED                       (0)
#define PRACH_RESTRICTED_SUPPORT_TYPE_A_AND_B                     (1)
#define PRACH_RESTRICTED_SUPPORT_TYPE_A_ONLY                      (2)
#define PRACH_RESTRICTED_SUPPORT_TYPE_B_ONLY                      (3)

#define MAX_PRACH_FD_OCCATIONS_IN_SLOT_VALUE_1                    (0)
#define MAX_PRACH_FD_OCCATIONS_IN_SLOT_VALUE_2                    (1)
#define MAX_PRACH_FD_OCCATIONS_IN_SLOT_VALUE_4                    (2)
#define MAX_PRACH_FD_OCCATIONS_IN_SLOT_VALUE_8                    (3)

#define BITMASK_RSSI_MEASUREMENT_SUPPORT_DBM                      (0x01)
#define BITMASK_RSSI_MEASUREMENT_SUPPORT_DBFS                     (0x02)
#define MAX_PRACH_SCS                                             (5)
#define MAX_RACH_ROOT_FORMAT                                      (14)
#define MAX_RACH_WEIGHT                                           (14)

// List of Values in TLV for PARAM.resp : Table 3-24

#define CAPABILITY_VALIDITY_SCOPE_ANYTIME                         (0)
#define CAPABILITY_VALIDITY_SCOPE_AFTER_PHY_PROFILE               (1)
#define CAPABILITY_VALIDITY_SCOPE_AFTER_PHY_AND_DFE_PROFILE       (2)

// List of Values in TLV for PARAM.resp : Table 3-25

#define TIME_MANAGEMENT_MECHANISM_SFN_SL_SYNC                     (0)
#define TIME_MANAGEMENT_MECHANISM_DELAY                           (1)

#define FAPI_NEGO_VERSION_NO_PROVISIONED                          (0)
#define FAPI_NEGO_VERSION_V1                                      (1)
#define FAPI_NEGO_VERSION_V2                                      (2)
#define FAPI_NEGO_VERSION_V3                                      (3)
#define FAPI_NEGO_VERSION_NO_SUPPORT                              (255)

// List of Values in TLV for PARAM.resp : Table 3-28

#define BITMASK_MTRP_SUPPORT_SDM_SINGLE_DCI                       (0x0001)
#define BITMASK_MTRP_SUPPORT_FDM_SINGLE_DCI_SCHEME_2a             (0x0002)
#define BITMASK_MTRP_SUPPORT_FDM_SINGLE_DCI_SCHEME_2b             (0x0004)
#define BITMASK_MTRP_SUPPORT_TDM_SINGLE_DCI_SCHEME_3              (0x0008)
#define BITMASK_MTRP_SUPPORT_TDM_SINGLE_DCI_SCHEME_4              (0x0010)
#define BITMASK_MTRP_SUPPORT_MULTI_DCI                            (0x0020)
#define BITMASK_MTRP_SUPPORT_ONE_OF_TWO_MTRP                      (0x0040)
#define BITMASK_MTRP_SUPPORT_BOTH_MTRP                            (0x0080)

// List of Values in TLV for PARAM.resp : Table 3-30

#define MAX_REPORT_USAGE                                          (10)
#define MAX_ANT_USAGE                                             (4)
#define CASE_OF_CP                                                (2) // 0: normal 1: extended


// List of Values in TLV for PARAM.resp : Table 3-31

#define MAX_SM_DL_CHANNEL_TYPE                                    (3)  //0 :PDSCH 1: PDCCH 2:CSI-RS
#define MAX_SM_UL_CHANNEL_TYPE                                    (2)  //0 :PUSCH 1: PUCCH

// List of Values in TLV for CONFIG.req : Table 3-30

#define MORE_THAN_ONE_INDICATION_PER_SLOT_NO_CONFIG               (0)
#define MORE_THAN_ONE_INDICATION_PER_SLOT_NOT_SUPPORT             (1)
#define MORE_THAN_ONE_INDICATION_PER_SLOT_SUPPORT                 (2)

#define MORE_THAN_ONE_REQUEST_PER_SLOT_NO_CONFIG                  (0)
#define MORE_THAN_ONE_REQUEST_PER_SLOT_NOT_SUPPORT                (1)
#define MORE_THAN_ONE_REQUEST_PER_SLOT_SUPPORT                    (2)

// List of Values in TLV for CONFIG.req : Table 3-37

#define BANDWIDTH_IN_MHZ_5                                        (5)
#define BANDWIDTH_IN_MHZ_10                                       (10)
#define BANDWIDTH_IN_MHZ_15                                       (15)
#define BANDWIDTH_IN_MHZ_20                                       (20)
#define BANDWIDTH_IN_MHZ_25                                       (25)
#define BANDWIDTH_IN_MHZ_30                                       (30)
#define BANDWIDTH_IN_MHZ_40                                       (40)
#define BANDWIDTH_IN_MHZ_50                                       (50)
#define BANDWIDTH_IN_MHZ_60                                       (60)
#define BANDWIDTH_IN_MHZ_70                                       (70)
#define BANDWIDTH_IN_MHZ_80                                       (80)
#define BANDWIDTH_IN_MHZ_90                                       (90)
#define BANDWIDTH_IN_MHZ_100                                      (100)
#define BANDWIDTH_IN_MHZ_200                                      (200)
#define BANDWIDTH_IN_MHZ_400                                      (400)

#define POWER_PROFILE_NR                                          (0)
#define POWER_PROFILE_SSS                                         (1)

#define BITMASK_POWER_PROFILE_CHANNEL_PDCCH_DMRS                  (0x01)
#define BITMASK_POWER_PROFILE_CHANNEL_PDCCH_DATA                  (0x02)
#define BITMASK_POWER_PROFILE_CHANNEL_PDSCH_DMRS                  (0x04)
#define BITMASK_POWER_PROFILE_CHANNEL_PDSCH_DATA                  (0x08)
#define BITMASK_POWER_PROFILE_CHANNEL_PDSCH_PTRS                  (0x10)
#define BITMASK_POWER_PROFILE_CHANNEL_CSI_RS                      (0x20)
#define BITMASK_POWER_PROFILE_CHANNEL_PSS                         (0x40)
#define BITMASK_POWER_PROFILE_CHANNEL_SSS                         (0x80)

#define POWER_OFFSETS_RESPECT_TO_RS_INDEX_0                       (0)
#define POWER_OFFSETS_RESPECT_TO_RS_INDEX_1                       (1)

// List of Values in TLV for CONFIG.req : Table 3-38

#define FRAME_DUPLEX_TYPE_FDD                                     (0)
#define FRAME_DUPLEX_TYPE_TDD                                     (1)

#define TRANS_TYPE_VALIDITY_L1_MUST_IGNORE                        (0)
#define TRANS_TYPE_VALIDITY_L1_MUST_INTERPRET                     (1)

// List of Values in TLV for CONFIG.req : Table 3-39

#define BCH_PAYLOAD_MAC_GENERATES_FULL                            (0)
#define BCH_PAYLOAD_PHY_GENERATES_TIMING                          (1)
#define BCH_PAYLOAD_PHY_GENERATES_FULL                            (2)

// List of Values in TLV for CONFIG.req : Table 3-40

#define PRACH_SEQUENCE_LENGTH_LONG                                (0)
#define PRACH_SEQUENCE_LENGTH_SHORT                               (1)

#define PRACH_SUBCARRIER_SPACING_15KHZ                            (0)
#define PRACH_SUBCARRIER_SPACING_30KHZ                            (1)
#define PRACH_SUBCARRIER_SPACING_60KHZ                            (2)
#define PRACH_SUBCARRIER_SPACING_120KHZ                           (3)
#define PRACH_SUBCARRIER_SPACING_1_DOT_25KHZ                      (4)
#define PRACH_SUBCARRIER_SPACING_5KHZ                             (5)

#define PRACH_RESTRICTED_SETS_UNRESTRICTED                        (0)
#define PRACH_RESTRICTED_SETS_TYPE_A                              (1)
#define PRACH_RESTRICTED_SETS_TYPE_B                              (2)

#define SSB_PER_RACH_OCCASION_ONE_EIGHTH                          (0)
#define SSB_PER_RACH_OCCASION_ONE_FOURTH                          (1)
#define SSB_PER_RACH_OCCASION_ONE_SECOND                          (2)
#define SSB_PER_RACH_OCCASION_ONE                                 (3)
#define SSB_PER_RACH_OCCASION_TWO                                 (4)
#define SSB_PER_RACH_OCCASION_FOUR                                (5)
#define SSB_PER_RACH_OCCASION_EIGHT                               (6)
#define SSB_PER_RACH_OCCASION_SIXTEEN                             (7)

// List of Values in TLV for CONFIG.req : Table 3-45

#define BETA_PSS_PRFILE_NR_0_DB                                   (0)
#define BETA_PSS_PRFILE_NR_3_DB                                   (1)

#define SSB_PERIOD_5MS                                            (0)
#define SSB_PERIOD_10MS                                           (1)
#define SSB_PERIOD_20MS                                           (2)
#define SSB_PERIOD_40MS                                           (3)
#define SSB_PERIOD_80MS                                           (4)
#define SSB_PERIOD_160MS                                          (5)

#define SSB_FIRST_SYMBOL_MAPPING_CASE_A                           (0)
#define SSB_FIRST_SYMBOL_MAPPING_CASE_B                           (1)
#define SSB_FIRST_SYMBOL_MAPPING_CASE_C                           (2)
#define SSB_FIRST_SYMBOL_MAPPING_CASE_D                           (3)
#define SSB_FIRST_SYMBOL_MAPPING_CASE_E                           (4)

#define RMSI_ABSENT                                               (0)
#define RMSI_PRESENT                                              (1)

// List of Values in TLV for CONFIG.req : Table 3-47

#define TDD_PERIOD_0_DOT_5MS                                      (0)
#define TDD_PERIOD_0_DOT_625MS                                    (1)
#define TDD_PERIOD_1MS                                            (2)
#define TDD_PERIOD_1_DOT_25MS                                     (3)
#define TDD_PERIOD_2MS                                            (4)
#define TDD_PERIOD_2_DOT_5MS                                      (5)
#define TDD_PERIOD_5MS                                            (6)
#define TDD_PERIOD_10MS                                           (7)

#define SYMBOL_TYPE_DL                                            (0)
#define SYMBOL_TYPE_UL                                            (1)
#define SYMBOL_TYPE_FLEXIBLE                                      (2)

// List of Values in TLV for CONFIG.req : Table 3-48

#define RSSI_MEASUREMENT_UNIT_DO_NOT_REPORT                       (0)
#define RSSI_MEASUREMENT_UNIT_DBM                                 (1)
#define RSSI_MEASUREMENT_UNIT_DBFS                                (2)

// List of Values in TLV for CONFIG.req : Table 3-50

#define PRB_SYMB_RATE_MATCH_PATTERN_BITMAP_COVERS_ONE_SLOT        (1)
#define PRB_SYMB_RATE_MATCH_PATTERN_BITMAP_COVERS_TWO_SLOT        (2)

// List of Values in TLV for CONFIG.req : Table 3-52

#define SUBFRAME_ALLOC_LENGTH_ONE_SIXT                            (0)
#define SUBFRAME_ALLOC_LENGTH_ONE_EIGHTH                          (1)
#define SUBFRAME_ALLOC_LENGTH_FOUR_TWENTI_FOUR                    (2)
#define SUBFRAME_ALLOC_LENGTH_FOUR_THIRTY_TWO                     (3)

// List of Values in TLV for CONFIG.req : Table 3-54

#define PDSCH_CGB_SCHEME_NO_DL_CBG_RE_TX                          (0)
#define PDSCH_CGB_SCHEME_DL_CBG_RE_TX_L2_SEGMENTATION             (1)
#define PDSCH_CGB_SCHEME_DL_CBG_RE_TX_L1_SEGMENTATION             (2)

// List of Values in TLV for CONFIG.req : Table 3-56

#define NUM_TX_PORTS_TRP1_ONLY_HOSTING_ONE_TRP                    (0)
#define NUM_RX_PORTS_TRP1_ONLY_HOSTING_ONE_TRP                    (0)

// PHY API error codes : Table 3-60 
                                                           
#define MSG_OK                                                    (0x0)
#define MSG_INVALID_STATE                                         (0x1)
#define MSG_INVALID_CONFIG                                        (0x2)
#define OUT_OF_SYNC                                               (0x3)
#define MSG_SLOT_ERR                                              (0x4)
#define MSG_BCH_MISSING                                           (0x5)
#define MSG_INVALID_SFN                                           (0x6)
#define MSG_UL_DCI_ERR                                            (0x7)
#define MSG_TX_ERR                                                (0x8)
#define MSG_INVALID_PHY_ID                                        (0x9)
#define MSG_UNINSTANTIATED_PHY                                    (0xA)
#define MSG_INVALID_DFE_PROFILE                                   (0xB)
#define PHY_PROFILE_SELECTION                                     (0xC)
                                                                 
// List of Valus for DL_TTI.request message body : Table 3-64
                                                                 
#define DL_PDU_TYPE_PDCCH                                         (0)
#define DL_PDU_TYPE_PDSCH                                         (1)
#define DL_PDU_TYPE_CSI_RS                                        (2)
#define DL_PDU_TYPE_SSB                                           (3)
                                                           
#define DL_PDU_TYPE_INDEX_PDCCH                                   (0)
#define DL_PDU_TYPE_INDEX_PDSCH                                   (1)
#define DL_PDU_TYPE_INDEX_CSI_RS                                  (2)
#define DL_PDU_TYPE_INDEX_SSB                                     (3)
#define DL_PDU_TYPE_INDEX_DLDCI                                   (4)

// List of Valus for PDCCH PDU : Table 3-66

#define CYCLIC_PREFIX_NORMAL                                      (0)
#define CYCLIC_PREFIX_EXTENDED                                    (1)

#define DURATION_SYMBOLS_ONE                                      (1)
#define DURATION_SYMBOLS_TWO                                      (2)
#define DURATION_SYMBOLS_THREE                                    (3)

#define CCE_REG_MAPPING_TYPE_NON_INTERLEAVED                      (0)
#define CCE_REG_MAPPING_TYPE_INTERLEAVED                          (1)

#define REG_BUNDLE_SIZE_TWO                                       (2)
#define REG_BUNDLE_SIZE_THREE                                     (3)
#define REG_BUNDLE_SIZE_SIX                                       (6)

#define INTERLEAVER_SIZE_TWO                                      (2)
#define INTERLEAVER_SIZE_THREE                                    (3)
#define INTERLEAVER_SIZE_SIX                                      (6)

#define CORESET_TYPE_CONFIGURED_BY_PBCH_OR_SIB1                   (0)
#define CORESET_TYPE_OTHERWISE                                    (1)

#define PRECODER_GRANULARITY_SAME_AS_REG_BUNDLE                   (0)
#define PRECODER_GRANULARITY_ALL_CONTIGUOUS_RBS                   (1)

// List of Valus for DL DCI PDU : Table 3-67

#define CCE_AGGREGATION_LEVEL_ONE                                 (1)
#define CCE_AGGREGATION_LEVEL_TWO                                 (2)
#define CCE_AGGREGATION_LEVEL_FOUR                                (4)
#define CCE_AGGREGATION_LEVEL_EIGHT                               (8)
#define CCE_AGGREGATION_LEVEL_SIXTEEN                             (16)

// List of Valus for PDSCH PDU : Table 3-70

#define BITMASK_PDSCH_PDU_BITMAP_PTRS_INCLUDED                    (0x01)
#define BITMASK_PDSCH_PDU_BITMAP_CBG_RETX_CTRL_INCLUDED           (0x02)

#define QAM_MOD_ORDER_TWO                                         (2)
#define QAM_MOD_ORDER_FOUR                                        (4)
#define QAM_MOD_ORDER_SIX                                         (6)
#define QAM_MOD_ORDER_EIGHT                                       (8)

#define PDSCH_MCS_TABLE_NOT_QAM256                                (0)
#define PDSCH_MCS_TABLE_QAM256                                    (1)
#define PDSCH_MCS_TABLE_QAM64_LOW_SE                              (2)

#define PDSCH_DRMS_REF_POINT_TYPE_POINT_A                         (0)
#define PDSCH_DRMS_REF_POINT_TYPE_BWP_START_RB_SUBCARRIER_ZERO    (1)

#define DMRS_CONFIG_TYPE_1                                        (0)
#define DMRS_CONFIG_TYPE_2                                        (1)

#define RESOURCE_ALLOCATION_TYPE_0                                (0)
#define RESOURCE_ALLOCATION_TYPE_1                                (1)

#define VRB_TO_PRB_MAPPING_NON_INTERLEAVED                        (0)
#define VRB_TO_PRB_MAPPING_INTERLEAVED_RB_SIZE_TWO                (1)
#define VRB_TO_PRB_MAPPING_INTERLEAVED_RB_SIZE_FOUR               (2)

#define POWER_CONTROL_OFFSET_SS_PROFILE_NR_MINUS_3DB              (0)
#define POWER_CONTROL_OFFSET_SS_PROFILE_NR_0DB                    (1)
#define POWER_CONTROL_OFFSET_SS_PROFILE_NR_3DB                    (2)
#define POWER_CONTROL_OFFSET_SS_PROFILE_NR_6DB                    (3)
#define POWER_CONTROL_OFFSET_SS_PROFILE_NR_INVALID                (255)

#define TB_CRC_PART_OF_DATA_PAYLOAD                               (0)
#define TB_CRC_PART_OF_CONTROL_PAYLOAD                            (1)

#define PTRS_TIME_DENSITY_ONE                                     (0)
#define PTRS_TIME_DENSITY_TWO                                     (1)
#define PTRS_TIME_DENSITY_FOUR                                    (2)

#define PTRS_FRQ_DENSITY_TWO                                      (0)
#define PTRS_FRQ_DENSITY_FOUR                                     (1)

// List of Values for PDSCH maintenance parameters V3 : Table 3-72

#define PDSCH_TRANS_TYPE_NON_INTERLEAVED_CSS                      (0)
#define PDSCH_TRANS_TYPE_ANY_NON_INTERLEAVED_EXCEPT_ABOVE         (1)
#define PDSCH_TRANS_TYPE_INTERLEAVED_TYPE0_CSS                    (2)
#define PDSCH_TRANS_TYPE_INTERLEAVED_NON_TYPE0_CSS_W_CORESET0     (3)
#define PDSCH_TRANS_TYPE_INTERLEAVED_NON_TYPE0_CSS_WO_CORESET0    (4)
#define PDSCH_TRANS_TYPE_ANY_INTERLEAVED_EXCEPT_ABOVE             (5)

#define LDPC_BASE_GRAPH_1                                         (1)
#define LDPC_BASE_GRAPH_2                                         (2)

#define SYMBOL_PATTERNS_NO_RATE_MATCH                             (0)
#define SYMBOL_PATTERNS_RATE_MATCH                                (1)

#define SSB_PDUS_FOR_RATE_MATCHING_SIZE                           (6)
#define PDSCH_RATE_MATCHING_BITMAP_SIZE                           (8)

#define MAX_PDSCH_PRB_SYM_RM_PATTERNS_INFO_COUNT                  (8)
#define MAX_PDSCH_CORESET_RM_PATTERNS_INFO_COUNT                  (8)
#define MAX_PDSCH_CORESET_FREQDOMAIN_RESOURCE                     (6)

                                                           
// List of Valus for Rel-16 PDSCH parameters V3 : Table 3-74

#define MTRP_REPETITION_SCHEME_FDM_SCHEMEB_PRG2                  (1)
#define MTRP_REPETITION_SCHEME_FDM_SCHEMEB_PRG4                  (2)
#define MTRP_REPETITION_SCHEME_FDM_SCHEMEB_PRG_WIDEBAND          (3)
#define MTRP_REPETITION_SCHEME_FDM_SCHEMEB_INVALID               (255)

// List of Values for PDSCH Parameters V4 : Table 3-75 

#define MAX_SIZE_OF_RATE_MATCH_PATTERN                           (10)  // ceil((szRm = ceil(coresetRmPatternBitmapSizeByReference/ 8 )
#define MAX_SIZE_OF_CRS_RATE_MATCH_PATTERN_MBSFN                 (10)  // TBD



// List of Valus for CSI-RS PDU : Table 3-76

#define CSI_TYPE_TRS                                             (0)
#define CSI_TYPE_CSI_RS_NZP                                      (1)
#define CSI_TYPE_CSI_RS_ZP                                       (2)

#define CDM_TYPE_NO_CDM                                          (0)
#define CDM_TYPE_FD_CDM2                                         (1)
#define CDM_TYPE_CDM4_FD2_TD2                                    (2)
#define CDM_TYPE_CDM8_FD2_TD4                                    (3)

#define FREQ_DENSITY_DOT5_EVEN_RB                                (0)
#define FREQ_DENSITY_DOT5_ODD_RB                                 (1)
#define FREQ_DENSITY_ONE                                         (2)
#define FREQ_DENSITY_THREE                                       (3)

// List of Valus for SSB/PBCH PDU maintenance : Table 3-79

#define SSB_FIRST_SYMBOL_MAPPING_L_MAX_8                          (8)
#define SSB_FIRST_SYMBOL_MAPPING_L_MAX_64                         (64)

#define SUBCARRIER_SPACING_15KHZ                                  (0)
#define SUBCARRIER_SPACING_30KHZ                                  (1)
#define SUBCARRIER_SPACING_60KHZ                                  (2)
#define SUBCARRIER_SPACING_120KHZ                                 (3)
#define SUBCARRIER_SPACING_240KHZ                                 (4)

// List of Valus for Tx precoding and beamforming PDU : Table 3-84

#define TRP_SCHEME_BASE                                           (0)
#define TRP_SCHEME_STRP                                           (1)
#define TRP_SCHEME_MTRP_SINGLE_TRP_FIRST_IN_PHY                   (2)
#define TRP_SCHEME_MTRP_SINGLE_TRP_SECOND_IN_PHY                  (3)
#define TRP_SCHEME_MTRP_BOTH_TRPS_IN_PHY                          (4)

// List of Valus for UL_TTI.request message body : Table 3-88

#define UL_PDU_TYPE_PRACH                                         (0)
#define UL_PDU_TYPE_PUSCH                                         (1)
#define UL_PDU_TYPE_PUCCH                                         (2)
#define UL_PDU_TYPE_SRS                                           (3)
#define UL_PDU_TYPE_MSGA_PUSCH                                    (4) 
                                                                 
#define UL_PDU_TYPE_INDEX_PRACH                                   (0)
#define UL_PDU_TYPE_INDEX_PUSCH                                   (1)
#define UL_PDU_TYPE_INDEX_FORMAT01_PUCCH                          (2)
#define UL_PDU_TYPE_INDEX_FORMAT234_PUCCH                         (3)
#define UL_PDU_TYPE_INDEX_SRS                                     (4)
#define UL_PDU_TYPE_INDEX_MSGA_PUSCH                              (5)
 
// List of Valus for PRACH PDU : Table 3-89

#define PRACH_FORMAT_0                                            (0)
#define PRACH_FORMAT_1                                            (1)
#define PRACH_FORMAT_2                                            (2)
#define PRACH_FORMAT_3                                            (3)
#define PRACH_FORMAT_A1                                           (4)
#define PRACH_FORMAT_A2                                           (5)
#define PRACH_FORMAT_A3                                           (6)
#define PRACH_FORMAT_B1                                           (7)
#define PRACH_FORMAT_B4                                           (8)
#define PRACH_FORMAT_C0                                           (9)
#define PRACH_FORMAT_C2                                           (10)
#define PRACH_FORMAT_A1_B1                                        (11)
#define PRACH_FORMAT_A2_B2                                        (12)
#define PRACH_FORMAT_A3_B3                                        (13)

// List of Valus for PUSCH PDU : Table 3-93

#define BITMASK_PUSCH_PDU_BITMAP_DATA_INCLUDED                    (0x01)
#define BITMASK_PUSCH_PDU_BITMAP_UCI_INCLUDED                     (0x02)
#define BITMASK_PUSCH_PDU_BITMAP_PTRS_INCLUDED                    (0x04)
#define BITMASK_PUSCH_PDU_BITMAP_DFTS_OFDM_INCLUDED               (0x08)

#define PUSCH_MCS_TABLE_NOT_QAM256                                (0)
#define PUSCH_MCS_TABLE_QAM256                                    (1)
#define PUSCH_MCS_TABLE_QAM64_LOW_SE                              (2)
#define PUSCH_MCS_TABLE_NOT_QAM256_WITH_TRANSFORM_PRECODING       (3)
#define PUSCH_MCS_TABLE_QAM64_LOW_SE_WITH_TRANSFORM_PRECODING     (4)

// List of Valus for PUSCH maintenance FAPIv3 : Table 3-94

#define PUSCH_TRANS_TYPE_INITIAL_UL_BWP_FOR_MSG3                  (0)
#define PUSCH_TRANS_TYPE_ACTIVE_UL_BWP_FOR_MSG3                   (1)
#define PUSCH_TRANS_TYPE_ACTIVE_UL_BWP_FOR_OTHERS                 (2)

#define PUSCH_DMRS_HOPPING_MODE_NEITHER                           (0)
#define PUSCH_DMRS_HOPPING_MODE_ENABLE_GROUP_DISABLE_SEQUENCE     (1)
#define PUSCH_DMRS_HOPPING_MODE_DISABLE_GROUP_ENABLE_SEQUENCE     (2)

// List of Valus for Optional puschUci information : Table 3-97

#define ALPHA_SCALING_0_DOT_5                                     (0)
#define ALPHA_SCALING_0_DOT_65                                    (1)
#define ALPHA_SCALING_0_DOT_8                                     (2)
#define ALPHA_SCALING_1                                           (3)

// List of Valus for Optional puschPtrs information : Table 3-98

#define UL_PTRS_POWER_0DB                                         (0)
#define UL_PTRS_POWER_3DB                                         (1)
#define UL_PTRS_POWER_4_DOT_77DB                                  (2)
#define UL_PTRS_POWER_6DB                                         (3)

// List of Valus for PUCCH PDU : Table 3-102

#define PUCCH_MULTI_SLOT_TX_INDICATOR_NO_TX                       (0)
#define PUCCH_MULTI_SLOT_TX_INDICATOR_START                       (1)
#define PUCCH_MULTI_SLOT_TX_INDICATOR_CONTINUE                    (2)
#define PUCCH_MULTI_SLOT_TX_INDICATOR_END                         (3)

#define PUCCH_GROUP_HOPPING_NEITHER                               (0)
#define PUCCH_GROUP_HOPPING_ENABLE_GROUP_DISABLE_SEQUENCE         (1)
#define PUCCH_GROUP_HOPPING_DISABLE_GROUP_ENABLE_SEQUENCE         (2)

#define PUCCH_SR_FLAG_NO_SR                                       (0)
#define PUCCH_SR_FLAG_SR_OPPORTUNITY                              (1)


// List of Valus for SRS PDU : Table 3-105 

#define SRS_NUM_SYMBOLS_ONE                                       (0)
#define SRS_NUM_SYMBOLS_TWO                                       (1)
#define SRS_NUM_SYMBOLS_FOUR                                      (2)

#define SRS_COMB_SIZE_TWO                                         (0)
#define SRS_COMB_SIZE_FOUR                                        (1)

#define SRS_NO_HOPPING                                            (0)
#define SRS_GROUP_HOPPING                                         (1)
#define SRS_SEQUENCE_HOPPING                                      (2)

#define SRS_RESOURCE_TYPE_APERIODIC                               (0)
#define SRS_RESOURCE_TYPE_SEMI_PERSISTENT                         (1)
#define SRS_RESOURCE_TYPE_PERIODIC                                (2)

// List of Valus for UL_DCI.request message body : Table 3-109

#define ULDCI_PDU_TYPE_INDEX_PDCCH                                (0)
#define ULDCI_PDU_TYPE_INDEX_ULDCI                                (1)

// List of Valus for TX_Data.request message body : Table 3-114

#define VALUE_FIELD_PAYLOAD_VALUES                                (0)
#define VALUE_FIELD_POINTER_TO_PAYLOAD                            (1)
#define VALUE_FIELD_OFFSET_FROM_FIRST_ADDRESS                     (2)
#define VALUE_FIELD_OFFSET_FROM_END_OF_CONTROL_PORTION            (3)

// List of Valus for CRC.indication message body : Table 3-117

#define CRC_STATUS_PASS                                           (0)
#define CRC_STATUS_FAIL                                           (1)

// List of Valus for UCI.indication message body : Table 3-118

#define UCI_ON_PUSCH                                              (0)
#define UCI_ON_PUCCH_FORMAT01                                     (1)
#define UCI_ON_PUCCH_FORMAT234                                    (2)

// List of Valus for UCI PUSCH PDU : Table 3-119

#define BITMASK_UCI_PUSCH_PDU_BITMAP_NOTHING_INCLUDED             (0x01)
#define BITMASK_UCI_PUSCH_PDU_BITMAP_HARQ_INCLUDED                (0x02)
#define BITMASK_UCI_PUSCH_PDU_BITMAP_CSI_PART1_INCLUDED           (0x04)
#define BITMASK_UCI_PUSCH_PDU_BITMAP_CSI_PART2_INCLUDED           (0x08)

// List of Valus for Rx_Data.indication message body : Table 3-116

#define RX_DATA_VALUE_FIELD_PAYLOAD_VALUES                        (0)
#define RX_DATA_VALUE_FIELD_OFFSET_FROM_END_OF_CONTROL_PORTION    (1)
#define RX_DATA_VALUE_FIELD_POINTER_TO_PAYLOAD                    (2)    // custom definition

// List of Valus for UCI PUCCH format 0 or 1 PDU : Table 3-120

#define BITMASK_UCI_PUCCH_FORMAT01_PDU_BITMAP_SR_INCLUDED         (0x01)
#define BITMASK_UCI_PUCCH_FORMAT01_PDU_BITMAP_HARQ_INCLUDED       (0x02)

// List of Valus for UCI PUCCH format 2, 3 or 4 PDU : Table 3-121

#define BITMASK_UCI_PUCCH_FORMAT234_PDU_BITMAP_SR_INCLUDED        (0x01)
#define BITMASK_UCI_PUCCH_FORMAT234_PDU_BITMAP_HARQ_INCLUDED      (0x02)
#define BITMASK_UCI_PUCCH_FORMAT234_PDU_BITMAP_CSI_PART1_INCLUDED (0x04)
#define BITMASK_UCI_PUCCH_FORMAT234_PDU_BITMAP_CSI_PART2_INCLUDED (0x08)

// List of Valus for SR PDU for format 0 or 1 : Table 3-122

#define SR_NOT_DETECTED                                           (0)
#define SR_DETECTED                                               (1)

#define SR_CONFIDENCE_LEVEL_GOOD                                  (0)
#define SR_CONFIDENCE_LEVEL_BAD                                   (1)

// List of Valus for HARQ PDU for format 0 or 1 : Table 3-123

#define HARQ_CONFIDENCE_LEVEL_GOOD                                (0)
#define HARQ_CONFIDENCE_LEVEL_BAD                                 (1)

#define HARQ_VALUE_ACK                                            (0)
#define HARQ_VALUE_NACK                                           (1)
#define HARQ_VALUE_DTX                                            (2)

// List of Valus for HARQ PDU for format 2, 3 or 4 : Table 3-125

#define HARQ_CRC_PASS                                             (0)
#define HARQ_CRC_FAIL                                             (1)
#define HARQ_CRC_NOT_PRESENT                                      (2)

// List of Valus for CSI Part1 and Part2 PDU : Table 3-126 and Table 3-127

#define CSI_CRC_PASS                                              (0)
#define CSI_CRC_FAIL                                              (1)
#define CSI_CRC_NOT_PRESENT                                       (2)

// Others : Size of Table entries, etc

#define MAX_NUM_OF_DLPORTS                                        (32)     // TBD
#define MAX_NUM_OF_ULPORTS                                        (32)     // TBD 
#define MAX_INDICATION_TYPE_COUNT                                 (6)      // 0: Rx_Data.ind 1: CRC.ind 2: UCI.ind 3: RACH.ind 4: SRS.ind 5: DL_TTI.resp
#define MAX_REQUEST_TYPE_COUNT                                    (4)      // 0: DL_TTI.req 1: UL_TTI.req 2: UL_DCI.req 3: Tx_Data.req
#define MAX_PROFILE_VALIDITY_MAP_SIZE                             (8)
#define MAX_NUMERLOGY_COUNT                                       (5)      // 0:15k 1:30k 2:60k 3:120k 4:240k
#define MAX_SSB_MASK_SIZE_TWO                                     (2)
#define MAX_SSB_MASK_SIZE_SIXTY_FOUR                              (64)
#define MAX_TDD_PERIODICITY_SLOT_COUNT                            (320)    // pattern1/pattern2 * 16 slots per ms (subcarrier spacing 240k) * max TDD period 10ms
#define MAX_NUM_OF_SYMBOL_PER_SLOT                                (14)  
#define MAX_NUM_OF_UCI_PART1_PARAMS                               (4)
#define MAX_NUM_OF_UCI_PART2                                      (100) 
#define MAX_RATEMATCH_BITMAP_SIZE                                 (35)
#define PRB_SYMB_RATE_TIME_DOMAIN_PATTERN_SIZE                    (5)
#define MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE                       (6) 
#define NUM_OF_DLPDU_TYPE                                         (5)
#define NUM_OF_ULPDU_TYPE                                         (6)
#define NUM_OF_ULDCIPDU_TYPE                                      (2)
#define MAX_NUM_OF_CORDWORD                                       (2)
#define MAX_RB_BITMAP_SIZE                                        (36)
#define DCI_PAYLOAD_BTYE_LEN                                      (12)     // TBD
#define MAX_NUM_OF_PRG                                            (275)
#define MAX_NUM_OF_LOGICAL_ANT_PORT                               (255)
#define MAX_UL_PTRS_PORT                                          (2)
#define MAX_LTE_CRS_RATEMATCH_PATTERN_BITMAP_SIZE                 (4)      // TBD
#define MAX_CSI_RS_RATEMATCH_SIZE                                 (4)      // TBD
                                                           
#define MAX_CODEBLOCKGROUP_PER_TB                                 (8)
                                                           
#define MAX_UE_IN_UL_UE_GROUP                                     (12)
#define MAX_UCI_IND_HARQ_PAYLOAD_SIZE                             (16)     // TBD
                                                           
#define MAX_UCI_IND_HARQ_FORMAT01_PAYLOAD_SIZE                    (2) 
#define MAX_UCI_IND_HARQ_FORMAT234_PAYLOAD_SIZE                   (8)      // TBD
#define MAX_UCI_IND_CSI_PART1_PAYLOAD_SIZE                        (8)      // TBD
#define MAX_UCI_IND_CSI_PART2_PAYLOAD_SIZE                        (8)      // TBD
#define MAX_UCI_IND_SR_FORMAT234_PAYLOAD_SIZE                     (8)      // TBD
#define MAX_UCI_IND_PAYLOAD_SIZE                                  (8)      // TBD
                                                           
#define MAX_SRS_IND_SYMBOL_INFO_COUNT                             (4)
#define NAX_NUM_SRS_USAGE                                         (4)
#define MAX_SRS_IND_NUM_RB                                        (272)
#define MAX_PRG_INDEX_COUNT                                       (10)      // TBD
#define MAX_NUM_OF_MIMO_GROUP                                     (4)       // TBD
#define MAX_CHANNEL_ARRAY_SIZE                                    (1000000) // TBD : check the size about Channl Matrix decomposition size...
#define MAX_NUM_PRACH_GROUP                                       (10)      // TBD
#define MAX_NUM_CAT_A_OR_B_BITMAP                                 (10)      // TBD
#define MAX_NUM_PRU_ASSOC_DMRS                                    (10)      // TBD
#define MAX_NUM_PRACH_FD_OCCASION                                 (10)      // TBD
#define MAX_NUM_PRACH_TD_OCCASION                                 (10)      // TBD

#define MAX_NUM_TRP                                               (2)
#define MAX_CDM_GROUP_COUNT                                       (2)
#define MAX_LAYERS_FOR_CW                                         (8)        // TBD
#define MAX_NUM_UL_SPATIAL_STREAMS                                (4)        // TBD
#define MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG               (2)        // TBD

#define MAX_SIZE_CORESET_P5                                       (4)        // TBD
#define MAX_SIZE_LTE_CRS_P5                                       (4)        // TBD
#define MAX_SIZE_PRB_SYM_P5                                       (4)        // TBD

#define MAX_NUM_CB                                                (64)       // TBD
#define MAX_NUM_CB_PRESENT_POSITION                               ((MAX_NUM_CB+7)/8)
#define MAX_NUM_UL_SPATIAL_STREAM_PORTS                           (256)
#define MAX_NUM_CB_CRC_STATUS                                     ((MAX_NUM_CB+7)/8)

///////////////////////////////////////////////////////// FAPI Header //////////////////////////////////////////////////

// Table 3-2 : PHY API message header

typedef struct tFapi_API_Header
{
    uint8_t                             numOfMessages;
    uint8_t                             opaqueHandle;

    // for internal use
    struct tFapi_API_Header            *pNext;
    uint32_t                            msgSize;
    uint8_t                             cellIndex;
    uint8_t                             flags;
} Fapi_API_Header;

// Table 3-3 : General PHY API message structure

typedef struct tFapi_Header
{
    uint16_t                            type;
    uint32_t                            length;
} Fapi_Generic_Header;

// Table 3-8 : Error Code


// Table 3-9 : TLV format

typedef struct tTLV_Header
{
    uint16_t                            tag;
    uint16_t                            length;
    uint8_t                             value[];    // Note: length of value should ensure the complete TLV is a multiple of 4-bytes
} TLV_Header;

///////////////////////////////////////////////////////// PARAM //////////////////////////////////////////////////

// This table contains the top-level configuration parameters relating to the cell.

// Table 3-7 : PARAM.response message body

typedef struct tPARAM_RESP
{
    uint8_t                             errorCode;
    uint8_t                             numOfTLV;
    TLV_Header                          tlvs[];
} PARAM_RESP;

///////////////////////////////////////////////////////// Local DBs for PARAM //////////////////////////////////////////////////

#define MAX_CONFIG_TLV_TO_REPORT_COUNT                            (20)    // Counts of all CONFIG TLV Tags
#define MAX_CAPABILITY_VALIDITY_ENTRY_COUNT                       (91)    // Counts of all PARAM TLV Tags except 0x0050
#define MAX_NUM_PHY_PROFILES                                      (8)     // TBD
#define MAX_NUM_PHYS                                              (8)     // TBD

// Table 3-13 : Cell and PHY parameters

#define PARAM_TAG_BITMASK_RELEASE_CAPABILITY                      (0x0001)
#define PARAM_TAG_BITMASK_PHY_STATE                               (0x0002)
#define PARAM_TAG_BITMASK_SKIP_BLANK_DL_CONFIG                    (0x0004)
#define PARAM_TAG_BITMASK_SKIP_BLANK_UL_CONFIG                    (0x0008)
#define PARAM_TAG_BITMASK_NUM_CONFIG_TLVS_TO_REPORT               (0x0010)
#define PARAM_TAG_BITMASK_POWER_PROFILES                          (0x0020)
#define PARAM_TAG_BITMASK_SSPBCH_BLOCK_POWER_SCALING_UNIT_CHOICE  (0X0040)
#define PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_DL_TTI                  (0x0080)
#define PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_UL_TTI                  (0x0100)
#define PARAM_TAG_BITMASK_MAX_NUM_PDUS_IN_UL_DCI                  (0x0200)

// Sub Structures for Tag 0x0005 in Table 3-13

typedef struct tConfigTLVToReport
{
    uint16_t                            tag;
    uint8_t                             length;
    uint8_t                             value;
} ConfigTLVToReport;

// Sub Structures for Tag 0x0038 in Table 3-13

typedef struct tPowerProfileInfo
{
    uint8_t                             referenceRS;
    int16_t                             powerOffsetMin;
    int16_t                             powerOffsetMax;
} PowerProfileInfo;

typedef struct tCell_Param
{
    uint32_t                            tagBitmask;

    uint16_t                            releaseCapability;
    uint16_t                            phyState;
    uint8_t                             skipBlankDLConfig;
    uint8_t                             skipBlankULConfig;
    uint16_t                            numConfigTLVsToReport;
    ConfigTLVToReport                   configTLVToReportList[MAX_CONFIG_TLV_TO_REPORT_COUNT];
    uint8_t                             powerProfilesSupported;
    PowerProfileInfo                    powerProfileInfoList[POWER_PROFILE_SIGNAL_INDEX_COUNT];
    uint8_t                             ssPbchBlockPowerScalingUnitChoice;
    uint16_t                            maxNumPDUsInDL_TTI;
    uint16_t                            maxNumPDUsInUL_TTI;
    uint16_t                            maxNumPDUsInUL_DCI;
} Cell_Param;

// Table 3-14 : Carrier parameters

#define PARAM_TAG_BITMASK_CYCLIC_PREFIX                           (0x0001)
#define PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_DL         (0x0002)
#define PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_SSB        (0x0004)
#define PARAM_TAG_BITMASK_SUPPORTED_BANDWIDTH_DL                  (0x0008)
#define PARAM_TAG_BITMASK_SUPPORTED_SUBCARRIER_SPACING_UL         (0x0010)
#define PARAM_TAG_BITMASK_SUPPORTED_BANDWIDTH_UL                  (0x0020)
#define PARAM_TAG_BITMASK_SS_PBCH_MULTIPLE_CARRIERS_IN_A_BAND     (0x0040)
#define PARAM_TAG_BITMASK_MULTIPLE_CELLS_SS_PBCH_IN_A_CARRIER     (0x0080)
#define PARAM_TAG_BITMASK_MEANING_OF_CARRIER_FREQUENCY            (0X0100)

typedef struct tCarrier_Param 
{
    uint32_t                            tagBitmask;

    uint8_t                             cyclicPrefix;                     // For each bit: 0=not supported 1=supported  Bits:0=Normal 1=Extended
    uint8_t                             supportedSubcarrierSpacingsDl;    // Bits: 0:15k~4:240khz
    uint8_t                             supportedSubcarrierSpcacingSSB;   // Bits: 0:15k~4:240khz
    uint16_t                            supportedBandwidthDl;             // Bits: 0:5MHz~13:400MHz
    uint8_t                             supportedSubcarrierSpacingsUl;
    uint16_t                            supportedBandwidthUl;
    uint8_t                             ssPbchMultipleCarriersInABand;
    uint8_t                             multipleCellsSsPbchInACarrier;
    uint8_t                             meaningOfCarrierFrequency;
} Carrier_Param;

// Table 3-15 : PDCCH parameters

#define PARAM_TAG_BITMASK_CCE_MAPPING_TYPE                        (0x0001)
#define PARAM_TAG_BITMASK_CORESET_OUTSIDE_FIRST_3_OFDM_SYMS       (0x0002)
#define PARAM_TAG_BITMASK_PRECODER_GRANULARITY_CORESET            (0x0004)
#define PARAM_TAG_BITMASK_PDCCH_MU_MIMO                           (0x0008)
#define PARAM_TAG_BITMASK_PDCCH_PRECODER_CYCLING                  (0x0010)
#define PARAM_TAG_BITMASK_MAX_PDCCHS_PER_SLOT                     (0x0020)

typedef struct tPDCCH_Param
{
    uint32_t                            tagBitmask;

    uint8_t                             cceMappingType;
    uint8_t                             coresetOutsideFirst3OfdmSymsOfSlot;
    uint8_t                             precoderGranularityCoreset;
    uint8_t                             pdcchMuMimo;
    uint8_t                             pdcchPrecoderCycling;
    uint8_t                             maxPdcchsPerSlot;
} PDCCH_Param;

// Table 3-16 : PUCCH parameters

#define PARAM_TAG_BITMASK_PUCCH_FORMATS                           (0x0001)
#define PARAM_TAG_BITMASK_MAX_PUCCHS_PER_SLOT                     (0x0002)
#define PARAM_TAG_BITMASK_PUCCH_GROUP_AND_SEQUENCE_HOPPING        (0x0004)
#define PARAM_TAG_BITMASK_MAX_NUM_UL_BWP_IDS                      (0x0008)
#define PARAM_TAG_BITMASK_PUCCH_AGGREGATION                       (0x0010)

typedef struct tPUCCH_Param
{    
    uint32_t                            tagBitmask;

    uint8_t                             pucchFormats;
    uint8_t                             maxPucchsPerSlot;
    uint8_t                             pucchGroupAndSequenceHopping;     // 0: Dynamic only 1: Semi_static only 2: both Dynamic and Semi_static
    uint8_t                             maxNumUlBwpIds;
    uint8_t                             pucchAggregation;
} PUCCH_Param;

// Table 3-17 : PDSCH parameters

#define PARAM_TAG_BITMASK_PDSCH_MAPPING_TYPE                      (0x00000001)
#define PARAM_TAG_BITMASK_PDSCH_ALLOCATION_TYPES                  (0x00000002)
#define PARAM_TAG_BITMASK_PDSCH_VRB_TO_PRB_MAPPING                (0x00000004)
#define PARAM_TAG_BITMASK_PDSCH_CBG                               (0x00000008)
#define PARAM_TAG_BITMASK_PDSCH_DMRS_CONFIG_TYPES                 (0x00000010)
#define PARAM_TAG_BITMASK_PDSCH_DMRS_MAX_LENGTH                   (0x00000020)
#define PARAM_TAG_BITMASK_PDSCH_DMRS_ADDITIONAL_POS               (0x00000040)
#define PARAM_TAG_BITMASK_MAX_PDSCHS_TBS_PER_SLOT                 (0x00000080)
#define PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_PDSCH            (0x00000100)
#define PARAM_TAG_BITMASK_SUPPORTED_MAX_MODULATION_ORDER_DL       (0x00000200)
#define PARAM_TAG_BITMASK_MAX_MU_MIMO_USERS_DL                    (0x00000400)
#define PARAM_TAG_BITMASK_PDSCH_DATA_IN_DMRS_SYMBOLS              (0x00000800)
#define PARAM_TAG_BITMASK_PREMPTION_SUPPORT                       (0x00001000)
#define PARAM_TAG_BITMASK_PDSCH_NON_SLOT_SUPPORT                  (0x00002000)
#define PARAM_TAG_BITMASK_SSB_RATE_MATCH                          (0x00004000)
#define PARAM_TAG_BITMASK_SUPPORTED_RATE_MATCH_PATTERN_TYPE       (0x00008000)
#define PARAM_TAG_BITMASK_PDCCH_RATE_MATCH                        (0x00010000)
#define PARAM_TAG_BITMASK_NUM_OF_RATE_MATCH_PATTERN_LTE_CRS       (0x00020000)
#define PARAM_TAG_BITMASK_NUM_OF_RATE_MATCH_PATTER_LTE_CRS_IN_PHY (0x00040000)
#define PARAM_TAG_BITMASK_LTE_CRS_RATE_MATCH_MB_SFN_DERIVATION    (0x00080000)
#define PARAM_TAG_BITMASK_SUPPORTED_LTE_CRS_RATE_MATCH_METHOD     (0x00100000)
#define PARAM_TAG_BITMASK_CSI_RS_RATE_MATCH                       (0x00200000)
#define PARAM_TAG_BITMASK_PDSCH_TRANS_TYPE_SUPPORT                (0x00400000)
#define PARAM_TAG_BITMASK_PDSCH_MAC_PDU_BIT_ALIGNMENT             (0x00800000)
#define PARAM_TAG_BITMASK_MAX_NUMBER_PRB_SYM_BITMAPS_PER_SLOT     (0x01000000)
#define PARAM_TAG_BITMASK_MAX_NUMBER_CSI_RS_RATE_MATCH_PER_SLOT   (0x02000000)
#define PARAM_TAG_BITMASK_MAX_NUM_SSB_PER_PDSCH_SLOT              (0x04000000)
#define PARAM_TAG_BITMASK_UNIVERSAL_RATE_MATCH                    (0x08000000)
#define PARAM_TAG_BITMASK_REQUIRED_PDSCH_SIGNALING_ASSOCIATIONS   (0x10000000)

typedef struct tPrbSymBitmapsPerSlotInfo
{
    uint16_t                            PrbSymBitmapsPerSlot[MAX_NUMEROLOGY];

}PrbSymBitmapsPerSlotInfo;

typedef struct tCsiRsRateMatchPerSlotInfo
{
    uint16_t                            CsiRsRateMatchPerSlot[MAX_NUMEROLOGY];

}CsiRsRateMatchPerSlotInfo;


typedef struct tPDSCH_Param
{
    uint32_t                            tagBitmask;

    uint8_t                             pdschMappingType;                 // 0:Type A, 1:Type B
    uint8_t                             pdschAllocationTypes;             // 0:Type 0, 1:Type 1
    uint8_t                             pdschVrbToPrbMapping;             // 0:not supported 1:supported, bits 0:Non-Interleaved, 1:Interleaved
    uint8_t                             pdschCbg;
    uint8_t                             pdschDmrsConfigTypes;
    uint8_t                             pdschDmrsMaxLength;
    uint8_t                             pdschDmrsAdditionalPos;
    uint8_t                             maxPdschsTBsPerSlot;
    uint8_t                             maxNumberMimoLayersPdsch;
    uint8_t                             supportedMaxModulationOrderDl;    // 0:QPSK 1:16QAM 2:64QAM 3:256QAM
    uint8_t                             maxMuMimoUsersDl;
    uint8_t                             pdschDataInDmrsSymbols;
    uint8_t                             premptionSupport;
    uint8_t                             pdschNonSlotSupport;
    uint8_t                             ssbRateMatch;
    uint16_t                            supportedRateMatchPatternType;
    uint8_t                             pdcchRateMatch;
    uint8_t                             numOfRateMatchPatternLTECrsPerSlot;
    uint8_t                             numOfRateMatchPatternLTECrsInPhy;
    uint8_t                             lteCrsRateMatchMbsfnDerivation;
    uint8_t                             supportedLTECrsRateMatchMethod;
    uint8_t                             csiRsRateMatch;
    uint8_t                             pdschTransTypeSupport;
    uint8_t                             pdschMacPduBitAlignment;
    PrbSymBitmapsPerSlotInfo            maxNumberPrbSymBitmapsPerSlot;
    CsiRsRateMatchPerSlotInfo           maxNumberCsiRsRateMatchPerSlot;
    uint8_t                             maxNumSsbPerPdschSlot;
    uint8_t                             universalRateMatch;
    uint8_t                             requiredPdschSignalingAssociations;
 
} PDSCH_Param;

// Table 3-18 : PUSCH parameters

#define PARAM_TAG_BITMASK_UCI_MUX_ULSCH_IN_PUSCH                  (0x000001)
#define PARAM_TAG_BITMASK_UCI_ONLY_PUSCH                          (0x000002)
#define PARAM_TAG_BITMASK_PUSCH_FREQUENCY_HOPPING                 (0x000004)
#define PARAM_TAG_BITMASK_PUSCH_DMRS_CONFIG_TYPES                 (0x000008)
#define PARAM_TAG_BITMASK_PUSCH_DMRS_MAX_LEN                      (0x000010)
#define PARAM_TAG_BITMASK_PUSCH_DMRS_ADDITIONAL_POS               (0x000020)
#define PARAM_TAG_BITMASK_PUSCH_CBG                               (0x000040)
#define PARAM_TAG_BITMASK_PUSCH_MAPPING_TYPE                      (0x000080)
#define PARAM_TAG_BITMASK_PUSCH_ALLOCATION_TYPE                   (0x000100)
#define PARAM_TAG_BITMASK_PUSCH_VRB_TO_PRB_MAPPING                (0x000200)
#define PARAM_TAG_BITMASK_PUSCH_MAX_PTRS_PORTS                    (0x000400)
#define PARAM_TAG_BITMASK_MAX_PDUSCHS_TBS_PER_SLOT                (0x000800)
#define PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_NON_CB_PUSCH     (0x001000)
#define PARAM_TAG_BITMASK_MAX_NUMBER_MIMO_LAYERS_CB_PUSCH         (0x002000)
#define PARAM_TAG_BITMASK_SUPPORTED_MAX_MODULATION_ORDER_UL       (0x004000)
#define PARAM_TAG_BITMASK_MAX_MU_MIMO_USERS_UL                    (0x008000)
#define PARAM_TAG_BITMASK_DFTS_OFDM_SUPPORT                       (0x010000)
#define PARAM_TAG_BITMASK_PUSCH_AGGREGATION_FACTOR                (0x020000)
#define PARAM_TAG_BITMASK_PUSCH_LBRM_SUPPORT                      (0x040000)
#define PARAM_TAG_BITMASK_PUSCH_TRANS_TYPE_SUPPORT                (0x080000)
#define PARAM_TAG_BITMASK_PUSCH_MAC_PDU_BIT_ALIGNMENT             (0x100000)

typedef struct tPUSCH_Param
{
    uint32_t                            tagBitmask;

    uint8_t                             uciMuxUlschInPusch;
    uint8_t                             uciOnlyPusch;
    uint8_t                             puschFrequencyHopping;
    uint8_t                             puschDmrsConfigTypes;
    uint8_t                             puschDmrsMaxLen;
    uint8_t                             puschDmrsAdditionalPos;
    uint8_t                             puschCbg;
    uint8_t                             puschMappingType;
    uint8_t                             puschAllocationTypes;
    uint8_t                             puschVrbToPrbMapping;
    uint8_t                             puschMaxPtrsPorts;
    uint8_t                             maxPduschsTBsPerSlot;
    uint8_t                             maxNumberMimoLayersNonCbPusch;
    uint8_t                             maxNumberMimoLayersCbPusch;
    uint8_t                             supportedMaxModulationOrderUl;
    uint8_t                             maxMuMimoUsersUl;
    uint8_t                             dftsOfdmSupport;
    uint8_t                             puschAggregationFactor;
    uint8_t                             puschLbrmSupport;
    uint8_t                             puschTransTypeSupport;
    uint8_t                             puschMacPduBitAlignment;
} PUSCH_Param;

// Table 3-19 : MSGA_PUSCH parameters

#define PARAM_TAG_BITMASK_MSGA_NR_OF_PUSCH_RESCONFIG              (0x0001)
#define PARAM_TAG_BITMASK_MSGA_PUSCH_TRAN_PRECODING               (0x0002)
#define PARAM_TAG_BITMASK_MSGA_INTRA_SLOT_PUCCH_FREQ_HOPH         (0x0004)
#define PARAM_TAG_BITMASK_MSGA_MAX_PUSCH_FD_OCCASIONS             (0x0008)
#define PARAM_TAG_BITMASK_MSGA_GUARD_BAND                         (0x0010)
#define PARAM_TAG_BITMASK_MSGA_GUARD_PERIOD                       (0x0020)
#define PARAM_TAG_BITMASK_MSGA_PUSCH_MAPPING_TYPE                 (0x0040)
#define PARAM_TAG_BITMASK_MSGA_PUSCH_DMRS_MAX_LEN                 (0x0080)
#define PARAM_TAG_BITMASK_MSGA_PUSCH_DMRS_CDM_GROUP               (0x0100)
#define PARAM_TAG_BITMASK_MSGA_PUSCH_DMRS_ADDITIONAL_POS          (0x0200)
#define PARAM_TAG_BITMASK_MSGA_SUPPORTED_MAX_MODULATION_ORDER_UL  (0x0400)
#define PARAM_TAG_BITMASK_MSGA_MAX_PUSCH_PER_PRU                  (0x0800)
#define PARAM_TAG_BITMASK_MSGA_MAX_NR_OF_CDM_GROUP                (0x1000)
#define PARAM_TAG_BITMASK_MSGA_MAX_NR_OF_DMRS_PORT                (0x2000)
#define PARAM_TAG_BITMASK_MSGA_PRACH_TO_PUSCH_MAPPING_SCHEME      (0x4000)

typedef struct tMSGA_PUSCH_Param
{
    uint32_t                            tagBitmask;
    
    uint16_t                            msgAnrOfpuschResConfig;
    uint8_t                             msgApuschTranPrecoding;
    uint16_t                            msgAIntraSlotPuschFreqHopH;
    uint8_t                             msgAmaxPuschFdOccasions;
    uint8_t                             msgAguardBand;
    uint8_t                             msgAguardPeriod;
    uint8_t                             msgApuschMappingType;
    uint8_t                             msgApuschDmrsMaxLen;
    uint8_t                             msgApuschDmrsCdmGroup;
    uint8_t                             msgApuschDmrsAdditionalPos;
    uint8_t                             msgAsupportedMaxModulationOrderUl;
    uint8_t                             msgAmaxPuschPerPru;
    uint8_t                             msgAmaxNrofCdmGroup;
    uint8_t                             msgAmaxNrofDmrsPort;
    uint8_t                             msgAPrachToPuschMappingScheme;

} MSGA_PUSCH_Param;

// Table 3-20 : PRACH parameters

#define PARAM_TAG_BITMASK_PRACH_LONG_FORMATS                      (0x0001)
#define PARAM_TAG_BITMASK_PRACH_SHORT_FORMATS                     (0x0002)
#define PARAM_TAG_BITMASK_PRACH_RESTRICTED_SETS                   (0x0004)
#define PARAM_TAG_BITMASK_MAX_PRACH_FD_OCCASIONS_IN_A_SLOT        (0x0008)
#define PARAM_TAG_BITMASK_MAX_NUM_PRACH_CONFIGURATIONS            (0x0010)
#define PARAM_TAG_BITMASK_PRACH_MULTIPLE_CARRIERS_IN_A_BAND       (0x0020)
#define PARAM_TAG_BITMASK_PRACH_FREQUENCY_OFFSET_MEANING          (0x0040)
#define PARAM_TAG_BITMASK_MAX_NUMTDFD_PRACH_OCCASION_PER_SLOT     (0x0080)
#define PARAM_TAG_BITMASK_PRACH_OCCASION_QUEUE_SIZE               (0x0100)

typedef struct tPRACH_Param
{
    uint32_t                            tagBitmask;

    uint8_t                             prachLongFormats;
    uint16_t                            prachShortFormats;
    uint8_t                             prachRestrictedSets;
    uint8_t                             maxPrachFdOccasionsInASlot;
    uint16_t                            maxNumPrachConfigurations;
    uint8_t                             prachMultipleCarriersInABand;
    uint8_t                             prachFrequencyOffsetMeaning;
    uint16_t                            maxNumTdFdPrachOccasionsPerSlot[MAX_PRACH_SCS];
    uint16_t                            prachPrachRootProcessingRate[MAX_RACH_ROOT_FORMAT];
    uint32_t                            prachOccasionQueueSize[MAX_RACH_WEIGHT];

} PRACH_Param;

// Table 3-21 : MSGA_PRACH parameters

#define PARAM_TAG_BITMASK_MSGA_MAX_NUM_PRACH_CONFIGURATIONS       (0x0001)
#define PARAM_TAG_BITMASK_MSGA_PRACH_LONG_FORMATS                 (0x0002)
#define PARAM_TAG_BITMASK_MSGA_PRACH_SHORT_FORMATS                (0x0004)
#define PARAM_TAG_BITMASK_MSGA_PRACH_RESTRICTED_SLOT              (0x0008)
#define PARAM_TAG_BITMASK_MSGA_MAX_PRACH_FD_OCCASION_IN_A_SLOT    (0x0010)
#define PARAM_TAG_BITMASK_MSGA_PRACH_MULTIPLE_CARRIER_IN_A_BAND   (0x0020)

typedef struct tMSGA_PRACH_Param
{

    uint32_t                            tagBitmask;

    uint16_t                            msgAmaxNumPrachConfigurations;
    uint8_t                             msgAprachLongFormats;
    uint16_t                            msgAprachShortFormats;
    uint8_t                             msgAprachRestrictedSets;
    uint8_t                             msgAmaxprachFdOccasionInASlot;
    uint8_t                             msgAprachMultipleCarriersInABand;
} MSGA_PRACH_Param;

// Table 3-22 : Measurement parameters

#define PARAM_TAG_BITMASK_RSSI_MEASUREMENT_SUPPORT                (0x0001)
#define PARAM_TAG_BITMASK_RSRP_MEASUREMENT_SUPPORT                (0x0002)

typedef struct tMeasurement_Param
{
    uint32_t                            tagBitmask;

    uint8_t                             rssiMeasurementSupport;
    uint8_t                             rsrpMeasurementSupport;

} Measurement_Param;    

// Table 3-23 : UCI parameters

#define PARAM_TAG_BITMASK_MAX_NUM_UCI_MAPS                        (0x0001)
#define PARAM_TAG_BITMASK_UCI_REPORT_FORMAT_PUCCH_FORMAT_234      (0x0002)

typedef struct tUCI_Param
{
    uint32_t                            tagBitmask;

    uint16_t                            maxNumUciMaps;
    uint8_t                             uciReportFormatPucchFormat234;

} UCI_Param;
 
// Table 3-24 : Capability validity scope   

#define PARAM_TAG_BITMASK_CAPABILITIES                            (0x0001)

typedef struct tCapabilityValidityEntry
{
    uint16_t                            capabilityTag;
    uint8_t                             validityScope;
} CapabilityValidityEntry;

typedef struct tCapability_Validity
{
    uint32_t                            tagBitmask;

    uint16_t                            numCapabilities;
    CapabilityValidityEntry             capabilityValidityEntryList[MAX_CAPABILITY_VALIDITY_ENTRY_COUNT];
} Capability_Validity;

// Table 3-25 : PHY Support

#define PARAM_TAG_BITMASK_SUPPORTED_PHY_PROFILES                  (0x0001)
#define PARAM_TAG_BITMASK_TIME_MANAGEMENT                         (0x0002)
#define PARAM_TAG_BITMASK_FAPI_PROTOCOL_VERSION                   (0x0004)
#define PARAM_TAG_BITMASK_MORE_THAN_ONE_INDICATION_PER_SLOT       (0x0008)
#define PARAM_TAG_BITMASK_MORE_THAN_ONE_REQUEST_PER_SLOT          (0x0010)

// Sub Structures for Tag 0x0051 in Table 3-25

typedef struct tSupportedPhyProfileInfo
{
    uint8_t                             numDlPortRanges;
    uint16_t                            dlPortRangeStart[MAX_NUM_OF_DLPORTS];   
    uint16_t                            dlPortRangeLen[MAX_NUM_OF_DLPORTS];      
    uint8_t                             numUlPortRanges;
    uint16_t                            ulPortRangeStart[MAX_NUM_OF_ULPORTS];   
    uint16_t                            ulPortRangeLen[MAX_NUM_OF_ULPORTS];     
} SupportedPhyProfileInfo;

// Sub Structures for Tag 0x0053 in Table 3-25

typedef struct tFAPIProtocolVersion
{
    uint8_t                             phyFapiProtocolVersion;
    uint8_t                             phyFapiNegotiatedProtocolVersion;
} FAPIProtocolVersion;

// Sub Structures for Tag 0x0055 in Table 3-25 and also for Tag 0x102B in Table 3-36

typedef struct tIndicationInstancePerSlotInfo
{
    uint8_t                             indicationInstancePerSlot[MAX_INDICATION_TYPE_COUNT];
} IndicationInstancePerSlotInfo;

// Sub Structures for Tag 0x0056 in Table 3-25 and also for Tag 0x102C in Table 3-36

typedef struct tRequestInstancePerSlotInfo
{
    uint8_t                             requestInstancesPerSlot[MAX_REQUEST_TYPE_COUNT];
} RequestInstancePerSlotInfo;

// Sub Structures for Tag 0x103D in Table 3-36

typedef struct tSlotTimeDomainInfo
{
   uint16_t                             sfn;
   uint8_t                              slot;
   uint8_t                              subcarrierSpacing;
   uint8_t                              cyclicPrefixType;
} SlotTimeDomainInfo;

typedef struct tPHY_Support
{
    uint32_t                            tagBitmask;

    uint16_t                            numPhyProfiles;
    uint16_t                            numDlBbPorts;
    uint16_t                            numUlBbPorts;

    uint8_t                             maxNumPhys[MAX_NUM_PHY_PROFILES];
    SupportedPhyProfileInfo             supportedPhyProfileInfo[MAX_NUM_PHY_PROFILES][MAX_NUM_PHYS];

    uint8_t                             timeManagement;

    FAPIProtocolVersion                 fapiProtocolVersion;

    IndicationInstancePerSlotInfo       moreThanOneIndicationPerSlot;
    RequestInstancePerSlotInfo          moreThanOneRequestPerSlot;
} PHY_Support;

// Table 3-26 : PHY / DFE Profile Validity Map

#define PARAM_TAG_BITMASK_PAIRING_DFE_PROFILES                    (0x0001)

typedef struct tParingDfeProfileParam
{
    uint32_t                            tagBitmask;

    uint16_t                            numPhyProfiles;
    uint16_t                            numDfeProfiles;
    uint8_t                             profileValidityMap[MAX_PROFILE_VALIDITY_MAP_SIZE];
} ParingDfeProfileParam;

// Table 3-27 : Delay management parameters
//   This table contains parameters pertaining to delay management, or other reserved parameter tags. 
//   These parameters shall only be defined by PHYs supporting Delay Management.

#define PARAM_TAG_BITMASK_DL_TTI_TIMING_OFFSET                    (0x0001)
#define PARAM_TAG_BITMASK_UL_TTI_TIMING_OFFSET                    (0x0002)
#define PARAM_TAG_BITMASK_UL_DCI_TIMING_OFFSET                    (0x0004)
#define PARAM_TAG_BITMASK_TX_DATA_TIMING_OFFSET                   (0x0008)
#define PARAM_TAG_BITMASK_TIMING_WINDOW                           (0x0010)
#define PARAM_TAG_BITMASK_TIMING_INFO_PERIOD                      (0x0020)

typedef struct tDelayManagement
{
    uint32_t                            tagBitmask;

    uint32_t                            dlTTI_TimingOffset;
    uint32_t                            ulTTI_TimingOffset;
    uint32_t                            ulDCI_TimingOffset;
    uint32_t                            txData_TimingOffset;
    uint16_t                            timingWindow;
    uint8_t                             timingInfoPeriod;
} DelayManagement;

// Table 3-28 : Rel-16 mTRP parameters
//   This table contains parameters pertaining to support of the 3GPP Rel-16 mTRP Feature, per 3GPP TS 38.300 [11], section 6.12

#define PARAM_TAG_BITMASK_MTRP_SUPPORT                            (0x0001)

typedef struct tRel_16_mTRP
{
    uint32_t                            tagBitmask;

    uint32_t                            mTRP_Support;
} Rel16_mTRP; 

//  Tags in Table 3-29 : User Plane and Encapsulation parameters

#define PARAM_TAG_BITMASK_USER_PLANE_ENCAPSULATION_CONTROL_SEPARATION           (0x0001)
#define PARAM_TAG_BITMASK_MESSAGE_ENCODING_SUPPORT                              (0x0002)

typedef struct tUserPlaneAndEncapsulation
{
    uint32_t                            tagBitmask;

    uint32_t                            userPlaneEncapsulation_controlPlaneSeparation;
    uint8_t                             messageEncodingSupport;
} UserPlaneAndEncapsulation;

//  Tags in Table 3-30 : SRS parameters

#define PARAM_TAG_BITMASK_SRS_USAGE_AND_PAIRING                                 (0x00001)
#define PARAM_TAG_BITMASK_RB_SUB_SAMPLING_RESOLUTION_FOR_SRS_DERIVED_REOPORT    (0x00002)
#define PARAM_TAG_BITMASK_MAX_SRS_CHANNEL_REPORTS_PER_SLOT                      (0x00004)
#define PARAM_TAG_BITMASK_MAX_SRS_PDUS_PER_SLOT                                 (0x00008)
#define PARAM_TAG_BITMASK_IQ_SAMPLE_FORMATS                                     (0x00010)
#define PARAM_TAG_BITMASK_SINGULAR_VALUE_FOMRATS                                (0x00020)
#define PARAM_TAG_BITMASK_MAX_SRS_TX_ANT_PORTS_PER_UE                           (0x00040)
#define PARAM_TAG_BITMASK_MAX_NUM_CONSECUTIVE_SRS_SLOTS                         (0x00080)
#define PARAM_TAG_BITMASK_MAX_DUTY_CYCLE_IN_PERCENTAGE                          (0x00100)
#define PARAM_TAG_BITMASK_MAX_SRS_COMB_SIZE                                     (0x00200)
#define PARAM_TAG_BITMASK_MAX_NUM_SRS_CYCLIC_SHIFTS                             (0x00400)
#define PARAM_TAG_BITMASK_SRS_SYMBOLS_BITMAP                                    (0x00800)
#define PARAM_TAG_BITMASK_MAX_NUM_SRS_SYMB_PER_SLOT                             (0x01000)

typedef struct tSRS_Param
{
    uint32_t                            tagBitmask;
    uint32_t                            supportSrsUsageAndPairing;
    uint16_t                            rbSubsamplingResolutionForSrsDerivedReport[MAX_REPORT_USAGE];
    uint16_t                            maxSrsChannelReportsPerSlot[MAX_REPORT_USAGE];
    uint16_t                            maxNumSrsPDUsPerSlot;
    uint32_t                            iqSampleFormats;
    uint32_t                            singularValueFormats;
    uint8_t                             maxSrsTxAntPortsPerUE[MAX_ANT_USAGE];
    uint8_t                             maxNumConsecutiveSrsSlots;
    uint16_t                            maxDutyCycleInPercentage;
    uint8_t                             maxSrsCombSize;
    uint8_t                             maxNumSrsCyclicShifts;
    uint16_t                            srsSymbolsBitmap[CASE_OF_CP];
    uint8_t                             maxNumSrsSymbPerSlot;
} SRS_Param;

//  Tags in Table 3-31 : Spatial Multiplexing and MIMO Capabilities

#define PARAM_TAG_BITMASK_MAX_NUM_CARRIER_BW_LAYERS_PRODUCT_DL                  (0x008E)
#define PARAM_TAG_BITMASK_MAX_NUM_CARRIER_BW_LAYERS_PRODUCT_UL                  (0x008F)
#define PARAM_TAG_BITMASK_MAX_NUM_CARRIER_BW_ANTENNAS_PRODUCT_DL                (0x0090)
#define PARAM_TAG_BITMASK_MAX_NUM_CARRIER_BW_ANTENNAS_PRODUCT_UL                (0x0091)
#define PARAM_TAG_BITMASK_MU_MIMO_DL                                            (0x0092)
#define PARAM_TAG_BITMASK_SUPPORTED_SPATIAL_MUX_PER_DL_CHANNEL_TYPE             (0x0093)
#define PARAM_TAG_BITMASK_MAX_NUM_MU_MIMO_LAYERS_PER_DL_CHANNEL_TYPE            (0x0094)
#define PARAM_TAG_BITMASK_CROSS_CHANNEL_SPATIAL_MULTIPLEXING_FOR_DL             (0x0095)
#define PARAM_TAG_BITMASK_MU_MIMO_UL                                            (0x0096)
#define PARAM_TAG_BITMASK_SUPPORTED_SPATIAL_MUX_PER_UL_CHANNEL_TYPE             (0x0097)
#define PARAM_TAG_BITMASK_MAX_NUM_MU_MIMO_LAYERS_PER_UL_CHANNEL_TPYE            (0x0098)
#define PARAM_TAG_BITMASK_CROOS_CHANNEL_SPATIAL_MULTIFLEXING_FOR_UL             (0x0099)
#define PARAM_TAG_BITMASK_MAX_MU_MIMO_GROUPS_IN_FD_PER_DL_CHANNEL               (0x009A)
#define PARAM_TAG_BITMASK_MAX_MU_MIMO_GROUPS_IN_TD_PER_DL_CHANNEL               (0x009B)
#define PARAM_TAG_BITMASK_MAX_MU_MIMO_NEW_PRECODERS_PER_SLOT                    (0x009C)
#define PARAM_TAG_BITMASK_ALL_PRB_GROUPS_PRGS                                   (0x009D)
#define PARAM_TAG_BITMASK_MAX_MU_MIMO_GROUPS_IN_FD_PER_UL_CHANNEL               (0x009E)
#define PARAM_TAG_BITMASK_MAX_MU_MIMO_GROUPS_IN_TD_PER_UL_CHANNEL               (0x009F)
#define PARAM_TAG_BITMASK_MIN_RB_RESOLUTION_FOR_SRS_DERIVED_REPORTS             (0x0150)
#define PARAM_TAG_BITMASK_MAX_NUMBER_DL_SPATIAL_STREAMS                         (0x0151)
#define PARAM_TAG_BITMASK_DL_SPATIAL_STREAM_CHANNEL_PRIORITY                    (0x0152)
#define PARAM_TAG_BITMASK_MAX_NUMBER_UL_SPATIAL_STREAMS                         (0x0153)
#define PARAM_TAG_BITMASK_UL_TPMI_CAPABILITY                                    (0x0154)

typedef struct tSpatialMultiplexing_MimoCapability
{
    uint32_t                            tagBitmask;

    uint16_t                            maxNumCarriersBWLayersProductDL[MAX_NUMEROLOGY];
    uint16_t                            maxNumCarriersBWLayersProductUL[MAX_NUMEROLOGY];
    uint16_t                            maxNumCarriersBWAntennasProductDL[MAX_NUMEROLOGY];
    uint16_t                            maxNumCarriersBWAntennasProductUL[MAX_NUMEROLOGY];
    uint32_t                            muMimoDl;  //0 :PDSCH 1: PDCCH 2: CSI-RS 3-31: Reserved
    uint32_t                            supportedSpatialMuxPerDlChannelType[MAX_SM_DL_CHANNEL_TYPE];
    uint8_t                             maxNumMuMimoLayersPerDlChannelType[MAX_SM_DL_CHANNEL_TYPE];
    uint16_t                            crossChannelSpatialMultiplexingForDl;
    uint32_t                            muMimoUl;
    uint32_t                            supportedSpatialMuxPerUlChannelType[MAX_SM_UL_CHANNEL_TYPE];    
    uint8_t                             maxNumMuMimoLayersPerUlchannelType[MAX_SM_UL_CHANNEL_TYPE];
    uint16_t                            crossChannelSpatialMultiplexingForUl;
    uint16_t                            maxMuMimoGroupsInFdPerDlChannel[MAX_SM_DL_CHANNEL_TYPE];
    uint8_t                             maxMuMimoGroupsInTdPerDlChannel[MAX_SM_DL_CHANNEL_TYPE];
    uint16_t                            maxMuMimoNewPrecodersPerSlot;
    uint8_t                             allPrbsGroupsPrgs;
    uint16_t                            maxMuMimoGroupsInFdPerUlChannel[MAX_SM_UL_CHANNEL_TYPE];
    uint8_t                             maxMuMimoGroupsInTdPerUlChannel[MAX_SM_UL_CHANNEL_TYPE];
    uint16_t                            minRbResolutionForSrsDerivedReports;
    uint16_t                            maxNumberDlSpatialStreams;
    uint32_t                            dlSpatialStreamChannelPriority;
    uint16_t                            maxNumberUlSpatialStreams;
    uint8_t                             ulTpmiCapability;
     
} SpatialMultiplexing_MimoCapability;

// Structure for whole PARAM info

typedef struct tFapiParamInfo
{
    Cell_Param                          cellParam;
    Carrier_Param                       carrierParam;
    PDCCH_Param                         pdcchParam;
    PUCCH_Param                         pucchParam;
    PDSCH_Param                         pdschParam;
    PUSCH_Param                         puschParam;
    MSGA_PUSCH_Param                    msgApuschParam;
    PRACH_Param                         prachParam;
    MSGA_PRACH_Param                    msgAprachParam;
    Measurement_Param                   measurementParam;
    UCI_Param                           uciParam;
    Capability_Validity                 capabilityValidity;
    PHY_Support                         phySupport;
    ParingDfeProfileParam               paringDfeProfileParam;
    DelayManagement                     delayManagement;
    Rel16_mTRP                          rel16_mTRP;
    UserPlaneAndEncapsulation           userPlaneAndEncapsulation;
    SRS_Param                           srsParam;
    SpatialMultiplexing_MimoCapability  spatialMultiplexingAndMimoCapability;    
} FapiParamInfo;

///////////////////////////////////////////////////////// CONFIG //////////////////////////////////////////////////

// Table 3-32 : CONFIG.request message body

typedef struct tCONFIG_REQ
{
    uint8_t                             numOfTLV;
    TLV_Header                          tlvs[];
} CONFIG_REQ;

// Table 3-33 : CONFIG.response message body

typedef struct tCONFIG_RESP
{
    uint8_t                             errorCode;
    uint8_t                             numInvalidOrUnsupportedTLV;
    uint8_t                             numInvalidTLVForIdleOnly;
    uint8_t                             numInvalidTLVForRunningOnly;
    uint8_t                             numMissingTLV;
    TLV_Header                          tlvs[];
} CONFIG_RESP;

// Table 3-37 : Carrier configuration : with Tag 0x102D

typedef struct tCarrierConfigInfo
{
    uint16_t                            dlBandwidth;
    uint32_t                            dlFrequency;
    uint16_t                            dlk0[MAX_NUMERLOGY_COUNT];
    uint16_t                            dlGridSize[MAX_NUMERLOGY_COUNT];
    uint16_t                            numTxAnt;
    uint16_t                            uplinkBandwidth;
    uint32_t                            uplinkFrequency;
    uint16_t                            ulk0[MAX_NUMERLOGY_COUNT];
    uint16_t                            ulGridSize[MAX_NUMERLOGY_COUNT];
    uint16_t                            numRxAnt;
    uint8_t                             frequencyShift7p5KHz;
    uint8_t                             powerProfile;
    uint8_t                             powerOffsetRsIndex;
} CarrierConfigInfo;

// Sub Structures for Tag 0x1031 in Table 3-40

typedef struct tPRACHConfigInfo
{
    uint16_t                            prachResConfigIndex;
    uint8_t                             prachSequenceLength;
    uint8_t                             prachSubCSpacing;
    uint8_t                             ulBwpPuschScs;
    uint8_t                             restrictedSetConfig;
    uint8_t                             numPrachFdOccasions;
    uint8_t                             prachConfigIndex;
    uint8_t                             prachFormat;
    uint8_t                             numPrachTdOccasions;
    uint8_t                             numberOfPreambles;
    uint8_t                             startPreambleIndex;
} PRACHConfigInfo;

typedef struct tPRACHOccasionInfo
{
    uint32_t                            tagBitmask;

    uint16_t                            prachRootSequenceIndex;
    uint8_t                             numRootSequences;
    uint16_t                            prachFrequencyOffset;
    uint16_t                            prachGuardBandOffset;
    uint8_t                             prachZeroCorrConf;
    uint16_t                            numUnusedRootSequences;
} PRACHOccasionInfo;

typedef struct tUnusedRootSequenceInfo
{
    uint16_t                            unusedRootSequences;
} UnusedRootSequenceInfo;

typedef struct tSSBPerRachInfo
{
    uint8_t                             ssbPerRach;
} SSBPerRachInfo;

// Table 3-43 :MsgA-PRACH-to-(PRU & DMRS) mapping structure

typedef struct tMSGA_PRACH_Mapping_Config
{
    struct
    {
        uint8_t                             validRO;
        uint8_t                             catAorBbimap[MAX_NUM_CAT_A_OR_B_BITMAP];   //ceil(numofPreamble)

        struct
        {
            uint16_t                            pruPreambleIndexForStartPreambleIndex;
            uint8_t                             numPuschAssocDmrsOccasionsForThisPrachRO;
            uint16_t                            msgAPuschAssocDmrsOccasion[MAX_NUM_PRU_ASSOC_DMRS];
        } catIndex[MAX_NUM_PRACH_GROUP];

    } mapIndex[MAX_NUM_PRACH_FD_OCCASION][MAX_NUM_PRACH_TD_OCCASION];
} MSGA_PRACH_Mapping_Config;

// Sub Structures for Tag 0x103E in Table 3-42

typedef struct tAssociationPatternPeriodInfo
{
    uint16_t                            numPrachSlotsforPrachtoPruAndDmrsMap;
    uint16_t                            prachSlotIndicesModulusApPslots;
    MSGA_PRACH_Mapping_Config           msgAPrachtoMapping;
} AssociationPatternPeriodInfo;

typedef struct tMSGA_PUSCH_ConfigInfo
{
    uint16_t                            msgApuschResConfigIndex;
    uint16_t                            msgAprachResConfigIndex;
    uint8_t                             msgAGroupAorB;
    uint16_t                            nIdMsgAPusch;
    uint16_t                            dmrsPorts;
    uint8_t                             availableDmrsSequenceIds;
    uint16_t                            puschDmrsScramblingIds[2];   //To Do: check Rel15 vs Rel16
    uint8_t                             numPuschOcasFd;
    uint8_t                             numPuschOcasTd;
    uint8_t                             numPuschOcasSlots;
    uint8_t                             msgAPuschTimeDomainOffset;
    uint16_t                            nPreambles;
    uint8_t                             associationPatternPeriod;
} MSGA_PUSCH_ConfigInfo;

// Sub Structures for Tag 0x1033 in Table 3-45

typedef struct tSSBResourceConfigInfo
{
    uint16_t                            ssbConfigIndex;
    uint16_t                            ssbOffsetPointA;
    uint8_t                             betaPssProfileNR;
    int16_t                             betaPssProfileSSS;
    uint8_t                             ssbPeriod;
    uint8_t                             ssbSubcarrierOffset;
    uint8_t                             ssbFirstSymbolMappingCase;
    uint8_t                             subcarrierSpacing;
    uint8_t                             subCarrierSpacingCommon;
    uint8_t                             ssbMaskSize;  
    uint32_t                            ssbMask[MAX_SSB_MASK_SIZE_TWO];
    uint8_t                             beamId[MAX_SSB_MASK_SIZE_SIXTY_FOUR];
    uint8_t                             lMax;
    uint8_t                             rmsiPresence;
} SSBResourceConfigInfo;

// Sub Structures for Tag 0x1037 in Table 3-50

typedef struct tPRBSymRateMatchPatternInfo
{
    uint8_t                             prbSymbRateMatchPatternID;
    uint8_t                             freqDomainRB[MAX_RATEMATCH_BITMAP_SIZE]; 
    uint32_t                            symbolsInRB;
    uint8_t                             subcarrierSpacing;
} PRBSymRateMatchPatternInfo;

// Sub Structures for Tag 0x1038 in Table 3-52

typedef struct tMbsfnSubframeConfigInfo
{
    uint8_t                             radioframeAllocationPeriod;
    uint8_t                             radioframeAllocationOffset;
    uint8_t                             lteFrameStructureType;
    uint8_t                             subframeAllocLength;
    uint32_t                            subframeAllocationBitmap;
} MbsfnSubframeConfigInfo;

// Sub Structures for Tag 0x1039 in Table 3-53

typedef struct tPUCCH_SemiStatic_ConfigInfo
{
    uint8_t                             pucchGroupHopping;
    uint16_t                            nIdPucchHopping;
} PUCCH_SemiStatic_ConfigInfo;

///////////////////////////////////////////////////////// Local DBs for CONFIG //////////////////////////////////////////////////

#define MAX_PRACH_OCCASION_COUNT                                  (10)    // TBD
#define MAX_PRACH_IN_SLOT                                         (4)     // TBD
#define MAX_UNUSED_ROOT_SEQUENCES_COUNT                           (20)    // TBD
#define MAX_MULTI_PRACH_CONFIG_COUNT                              (10)    // TBD
#define MAX_MULTI_SSB_CONFIG_COUNT                                (10)    // TBD
#define MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT                     (10)    // TBD
#define MAX_CORESET_RATE_MATCH_PATTERN_COUNT                      (10)    // TBD
#define MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT                      (10)    // TBD
#define MAX_MBSFN_SUBFRAME_CONFIG_COUNT                           (10)    // TBD
#define MAX_UL_BWP_COUNT                                          (4)     // TBD
#define MAX_BASEBAND_PORT_COUNT                                   (8)     // TBD
#define MAX_DIG_BEAM_COUNT                                        (8)     // TBD
#define MAX_NUM_LAYERS                                            (2)     // TBD
#define MAX_NUM_ANT_PORTS                                         (16)    // TBD
#define MAX_NUM_UCI2_MAPS                                         (4)     // TBD
#define MAX_SUM_OF_SIZES_PART1_PARAMS                             (12)
#define MAX_UCI2_MAP_SIZE                                         (4096)  // 2^MAX_SUM_OF_SIZES_PART1_PARAMS

// Table 3-36 : PHY configuration

#define CONFIG_TAG_BITMASK_PHY_PROFILE_ID                         (0x0001)
#define CONFIG_TAG_BITMASK_INDICATION_INSTANCES_PER_SLOT          (0x0002)
#define CONFIG_TAG_BITMASK_REQUEST_INSTANCES_PER_SLOT             (0x0004)
#define CONFIG_TAG_BITMASK_SLOT_TIME_DOMAIN_INFO                  (0x0008)

typedef struct tPHY_Config
{    
    uint32_t                            tagBitmask;

    uint16_t                            phyProfileId;
    IndicationInstancePerSlotInfo       indicationInstancesPerSlot;
    RequestInstancePerSlotInfo          requestInstancesPerSlot;
    SlotTimeDomainInfo                  slotTimeDomainInfo;
} PHY_Config;

// Table 3-37 : Carrier configuration 

#define CONFIG_TAG_BITMASK_CARRIER_CONFIGURATION                  (0x0001)

typedef struct tCarrier_Config
{
    uint32_t                            tagBitmask;

    CarrierConfigInfo                   carrierConfigInfo;
} Carrier_Config;

// Table 3-38 : Cell configuration

#define CONFIG_TAG_BITMASK_PHY_CELL_ID                            (0x0001)
#define CONFIG_TAG_BITMASK_FRAME_DUPLEX_TYPE                      (0x0002)
#define CONFIG_TAG_BITMASK_PDSCH_TRANS_TYPE_VALIDITY              (0x0004)
#define CONFIG_TAG_BITMASK_PUSCH_TRANS_TYPE_VALIDITY              (0x0008)

typedef struct tCell_Config
{
    uint32_t                            tagBitmask;

    uint16_t                            phyCellId;
    uint8_t                             frameDuplexType;
    uint8_t                             pdschTransTypeValidity;
    uint8_t                             puschTransTypeValidity;
} Cell_Config;

// Table 3-39 : SSB power and PBCH configuration

#define CONFIG_TAG_BITMASK_SS_PBCH_POWER                          (0x0001)
#define CONFIG_TAG_BITMASK_SS_PBCH_BLOCK_POWER_SCALING            (0x0002)
#define CONFIG_TAG_BITMASK_BCH_PAYLOAD_FLAG                       (0x0004)

typedef struct tSsbPowerPBCH_Config
{
    uint32_t                            tagBitmask;

    uint32_t                            ssPbchPower;
    int16_t                             ssPbchBlockPowerScaling;
    uint8_t                             bchPayloadFlag;
} SsbPowerPBCH_Config;

// Table 3-40 : PRACH configuration

#define CONFIG_TAG_BITMASK_PRACH_RESOURCE_CONFIGURATION           (0x0001)

typedef struct tPRACH_Config
{    
    uint32_t                            tagBitmask;

    PRACHConfigInfo                     prachConfigInfo;
    PRACHOccasionInfo                   prachOccasionInfo[MAX_PRACH_OCCASION_COUNT];
    uint16_t                            unusedRootSequencesList[MAX_PRACH_OCCASION_COUNT][MAX_UNUSED_ROOT_SEQUENCES_COUNT];
    uint8_t                             ssbPerRach;
} PRACH_Config;

// Table 3-41 : Multi-PRACH configuration table

#define CONFIG_TAG_BITMASK_MULTIPLE_PRACH_CONFIGURATION           (0x0001)

typedef struct tMultiPRACH_Config
{    
    uint32_t                            tagBitmask;

    uint16_t                            numPrachConfigurations;
    PRACH_Config                        prachConfig[MAX_MULTI_PRACH_CONFIG_COUNT];
} MultiPRACH_Config;

// Table 3-42 : MsgA PUSCH configuration table

#define CONFIG_TAG_BITMASK_MSGA_PUSCH_CONFIGURATION               (0x0001)

typedef struct tMSGA_PUSCH_Config
{
    uint32_t                            tagBitmask;

    MSGA_PUSCH_ConfigInfo               msgAPuschConfigInfo;
    AssociationPatternPeriodInfo        associationPatternPeriodInfo[MAX_PRACH_IN_SLOT];
} MSGA_PUSCH_Config;

// Table 3-44 : Multi-MsgA-PUSCH configuration table

typedef struct tMultiMSGA_PUSCH_Config
{
    uint16_t                        numMsgApuschConfigurations;
    MSGA_PUSCH_Config               msgApuschcConfigInfo;
} MultiMSGA_PUSCH_Config; 


// Table 3-45 : SSB resource configuration table

#define CONFIG_TAG_BITMASK_SSB_TRANSMISSION_PATTERN               (0x0001)

typedef struct tSSBResource_Config
{
    uint32_t                            tagBitmask;
    
    SSBResourceConfigInfo               ssbResourceConfigInfo;
} SSBResource_Config;

// Table 3-46 : Multi-SSB resource configuration table

#define CONFIG_TAG_BITMASK_MULTIPLE_SSB_CONFIGURATION             (0x0001)

typedef struct tMultiSSBResource_Config
{    
    uint32_t                            tagBitmask;
    
    uint8_t                             numSsbConfigurations;
    SSBResource_Config                  ssbResourceConfig[MAX_MULTI_SSB_CONFIG_COUNT];
} MultiSSBResource_Config;

// Table 3-47 : TDD table 

#define CONFIG_TAG_BITMASK_TDD_PATTERN                            (0x0001)

typedef struct tTDDPatternInfo
{
    uint8_t                             tddPeriod;
    uint8_t                             slotConfig[MAX_TDD_PERIODICITY_SLOT_COUNT][MAX_NUM_OF_SYMBOL_PER_SLOT];
} TDDPatternInfo;

typedef struct tTDD_Config
{
    uint32_t                            tagBitmask;

    TDDPatternInfo                      tddPatternInfo;
} TDD_Config;

// Table 3-48 : Measurement configuration

#define CONFIG_TAG_BITMASK_RSSI_MEASUREMENT_CONFIGURATION         (0x0001)
#define CONFIG_TAG_BITMASK_RSRP_MEASUREMENT_CONFIGURATION         (0x0002)

typedef struct tMeasurement_Config
{
    uint32_t                            tagBitmask;
    
    uint8_t                             rssiMeasurement;
    uint8_t                             rsrpMeasurement;
} Measurement_Config;

// Table 3-61 : Digital beam table (DBT) PDU

#define CONFIG_TAG_BITMASK_DBT_PDU                                (0x0001)

typedef struct tDBT_PDU
{
    uint32_t                            tagBitmask;
    
    uint16_t                            numDigBeams;
    uint16_t                            numBasebandPorts;
 
    struct    
    {
        uint16_t                            beamIdx;
        struct
        {
            int16_t                             digBeamWeightRe;
            int16_t                             digBeamWeightIm;
        } beamWeight[MAX_BASEBAND_PORT_COUNT];
    } digBeam[MAX_DIG_BEAM_COUNT];
} DBT_PDU;

// Table 3-62 : Precoding matrix (PM) PDU

#define CONFIG_TAG_BITMASK_PM_PDU                                 (0x0001)

typedef struct tPM_PDU
{
    uint32_t                            tagBitmask;
    
    uint16_t                            pmIdx;
    uint16_t                            numLayers;
    uint16_t                            numAntPorts;

    struct  
    {
        int16_t                             precoderWeightRe;
        int16_t                             precoderWeightIm;
    } precoderWeight[MAX_NUM_LAYERS][MAX_NUM_ANT_PORTS];
} PM_PDU;

// Table 3-49 : UCI configuration 

#define CONFIG_TAG_BITMASK_UCI_INDICATION_CONFIGURATION           (0x0001)

typedef struct tUCI2MapInfo
{
    uint8_t                             numPart1Params;
    uint8_t                             sizesPart1Params[MAX_NUM_OF_UCI_PART1_PARAMS];
    uint16_t                            map[MAX_UCI2_MAP_SIZE];
} UCI2MapInfo;

typedef struct tUCI_Config
{
    uint32_t                            tagBitmask;
    
    uint16_t                            numUci2Maps;
    UCI2MapInfo                         uci2MapInfo[MAX_NUM_UCI2_MAPS];
} UCI_Config;    

// Table 3-50 : PRB-symbol rate match patterns bitmap (non-CORESET) configuration

#define CONFIG_TAG_BITMASK_PRB_SYMBOL_RATE_MATCH_PATTERN          (0x0001)

typedef struct tPRBSymRateMatchPatterns_Config
{    
    uint32_t                            tagBitmask;
    
    uint8_t                             numberOfPrbSymbRateMatchPatterns;
    PRBSymRateMatchPatternInfo          prbSymbRateMatchPatternInfo[MAX_PRB_SYMB_RATE_MATCH_PATTERN_COUNT];
} PRBSymRateMatchPatterns_Config;

// Table 3-51 : CORESET  rate match patterns configuration

#define CONFIG_TAG_BITMASK_CORESET_RATE_MATCH_PATTERN             (0x0001)

typedef struct tCoresetRateMatchPatternInfo
{
    uint8_t                             coresetRateMatchPatternID;
    uint8_t                             freqDomainResources[MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE];
    uint16_t                            symbolsPattern;
    uint8_t                             subcarrierSpacing;
} CoresetRateMatchPatternInfo;

typedef struct tCoresetRateMatchPatterns_Config
{
    uint32_t                            tagBitmask;
    
    uint8_t                             numberOfCoresetRateMatchPatterns;
    CoresetRateMatchPatternInfo         coresetRateMatchPatternInfo[MAX_CORESET_RATE_MATCH_PATTERN_COUNT];
} CoresetRateMatchPatterns_Config;

// Table 3-52 : LTE-CRS rate match patterns configuration

#define CONFIG_TAG_BITMASK_LTE_CRS_RATE_MATCH_PATTERN             (0x0001)

typedef struct tLTECrsRateMatchPatternsInfo
{
    uint8_t                             crsRateMatchPatternID;
    uint16_t                            carrierFreqDL;
    uint8_t                             carrierBandwidthDL;
    uint8_t                             nrofCrsPorts;
    uint8_t                             vShift;
    uint8_t                             sizeMbsfnSubframeConfigList;

    MbsfnSubframeConfigInfo             mbsfnSubframeConfigInfo[MAX_MBSFN_SUBFRAME_CONFIG_COUNT]; 
} LTECrsRateMatchPatternsInfo;

typedef struct tLTECrsRateMatchPatterns_Config
{
    uint32_t                            tagBitmask;
    
    uint8_t                             numberOfLteCrsRateMatchPatterns;
    LTECrsRateMatchPatternsInfo         lteCrsRateMatchPatternInfo[MAX_LTE_CRS_RATE_MATCH_PATTERN_COUNT];
} LTECrsRateMatchPatterns_Config;

// Table 3-53 : PUCCH semi-static configuration

#define CONFIG_TAG_BITMASK_PUCCH_SEMI_STATIC_PARAMETER            (0x0001)

typedef struct tPUCCH_SemiStatic_Config
{
    uint32_t                            tagBitmask;
    
    uint8_t                             numUlBwpIds;
    PUCCH_SemiStatic_ConfigInfo         pucchSemiStaticConfigInfo[MAX_UL_BWP_COUNT];
} PUCCH_SemiStatic_Config;

// Table 3-54 : PDSCH semi-static configuration

#define CONFIG_TAG_BITMASK_PDSCH_SEMI_STATIC_PARAMETER            (0x0001)

typedef struct tPDSCH_SemiStatic_Config
{
    uint32_t                            tagBitmask;
    
    uint8_t                             pdschCbgScheme;
} PDSCH_SemiStatic_Config;

// Table 3-55 : Delay management configuration

#define CONFIG_TAG_BITMASK_TIMING_WINDOW                          (0x0001)
#define CONFIG_TAG_BITMASK_TIMING_INFO_MODE                       (0x0002)
#define CONFIG_TAG_BITMASK_TIMING_INFO_PERIOD                     (0x0004)

typedef struct tDelayManagement_Config
{
    uint32_t                            tagBitmask;
    
    uint16_t                            timingWindow;
    uint8_t                             timingInfoMode;
    uint8_t                             timingInfoPeriod;
} DelayManagement_Config;

// Table 3-56 : Rel-16 mTRP configuration

#define CONFIG_TAG_BITMASK_NUM_TX_PORTS_TRP1                      (0x0001)
#define CONFIG_TAG_BITMASK_NUM_RX_PORTS_TRP1                      (0x0002)

typedef struct tRel16_mTRP_Config
{
    uint32_t                            tagBitmask;
    
    uint8_t                             numTxPortsTRP1;
    uint8_t                             numRxPortsTRP1;
} Rel16_mTRP_Config;

// Structure for whole CONFIG info

typedef struct tFapiConfigInfo
{
    PHY_Config                          phyConfig;
    Carrier_Config                      carrierConfig;
    Cell_Config                         cellConfig;
    SsbPowerPBCH_Config                 ssbPowerPbchConfig;
    PRACH_Config                        prachConfig;
    MultiPRACH_Config                   multiPrachConfig;
    MSGA_PUSCH_Config                   msgApuschConfig;
    MSGA_PRACH_Mapping_Config           msgAprachMappingConfig;
    MultiMSGA_PUSCH_Config              multimsgApuschConfig;
    SSBResource_Config                  ssbResourceConfig;
    MultiSSBResource_Config             multiSsbResourceConfig;
    TDD_Config                          tddConfig;
    Measurement_Config                  measurementConfig;
    DBT_PDU                             dbtPdu;
    PM_PDU                              pmPdu;
    UCI_Config                          uciConfig;
    PRBSymRateMatchPatterns_Config      prbSymbRateMatchPatternsConfig;
    CoresetRateMatchPatterns_Config     coresetRateMatchPatternsConfig;
    LTECrsRateMatchPatterns_Config      lteCrsRateMatchPatternsConfig;
    PUCCH_SemiStatic_Config             pucchSemiStaticConfig;
    PDSCH_SemiStatic_Config             pdschSemiStaticConfig;
    DelayManagement_Config              delayManagementConfig;
    Rel16_mTRP_Config                   rel16mTRPConfig;
} FapiConfigInfo;

///////////////////////////////////////////////////////// START //////////////////////////////////////////////////

// Table 3-12 of SCF225 document : START.response parameters

typedef struct tSTART_RESP
{
    uint8_t                             errorCode;
} START_RESP;

// Table 3-59 : ERROR.indication message body

typedef struct tERROR_IND
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint8_t                             messageID;
    uint8_t                             errorCode;
    uint16_t                            expectedSfn;
    uint16_t                            expectedSlot;
} ERROR_IND;

///////////////////////////////////////////////////////// PDUs //////////////////////////////////////////////////

typedef struct tPDU_Struct
{
    uint16_t                            pduType;
    uint16_t                            pduSize;
} PDU_Struct;

// Table 3-63 : Slot indication message body

typedef struct tSLOT_IND
{
    uint16_t                            sfn;
    uint16_t                            slot;
} Slot_IND;

///////////////////////////////////////////////////////// PDCCH PDUs //////////////////////////////////////////////////

// Table 3-84 : Tx precoding and beamforming PDU

typedef struct tTxPrecodingAndBeamforming_PDU
{
    uint8_t                             trpScheme;
    uint16_t                            numPRGs;
    uint16_t                            prgSize;
    uint8_t                             digBFInterfaces;

    uint8_t                             variablePart[];    // List of [uint16_t pmiIdx and uint16_t beamIdx for each digBFInterfaces] for each numPRGs

} TxPrecodingAndBeamforming_PDU;


// Table 3-85 Rel16mTRP_Tx precoding and beamforming PDU

typedef struct tRel16mTRP_TxPrecodingAndBeamforming_PrgInfo
{
    uint8_t                             layers;
    uint16_t                            pmIdx[MAX_NUM_TRP];    // Use fixed size for efficiency

    uint8_t                             variablePart[];    // List of [uint16_t beamIdx for each digBFInterface] for each TRP

} Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo;

typedef struct tRel16mTRP_TxPrecodingAndBeamforming_PDU
{
    uint8_t                             trpScheme;
    uint16_t                            numPRGs;
    uint16_t                            prgSize;
    uint8_t                             digBFInterfaces[MAX_NUM_TRP];    // Use fixed size for efficiency

    uint8_t                             variablePart[];    // List of Rel16mTRP_TxPrecodingAndBeamforming_PrgInfo for each numPRGs

} Rel16mTRP_TxPrecodingAndBeamforming_PDU;


// Table 3-86 : Tx generalized Precoding PDU

typedef struct tTx_Generalized_Precoding_MuMimoGroupInfo
{
    uint16_t                            muMimoGroupIdx;
} Tx_Generalized_Precoding_MuMimoGroupInfo;

typedef struct tTx_Generalized_Precoding_PDU
{
    uint8_t                             trpScheme;
    uint8_t                             numMuMimoGroups;

    uint8_t                             variablePart[]; // List of Tx_generalized Precoding Mumimo group info each numMuMimoGroup
} Tx_Generalized_Precoding_PDU;

// Table 3-67 : DL DCI PDU 

typedef struct tDL_DCI_PDU_part1
{
    uint16_t                            rnti;
    uint16_t                            nIdPdcchData;
    uint16_t                            nRntiPdcchData;
    uint8_t                             cceIndex;
    uint8_t                             aggregationLevel;
 
    uint8_t                             variablePart[];    // TxPrecodingAndBeamforming_PDU (Rel15) or Rel16mTRP_TxPrecodingAndBeamforming_PDU (Rel16)
} DL_DCI_PDU_part1;

typedef struct tDL_DCI_PDU_part2
{
    uint8_t                             beta_PDCCH_1_0;
    int8_t                              powerControlOffsetSSProfileNR;
    uint16_t                            payloadSizeBits;
    uint8_t                             payload[DCI_PAYLOAD_BTYE_LEN];
} DL_DCI_PDU_part2;

// Table 3-68 : PDCCH PDU maintenance FAPIv3

typedef struct tPDCCH_PDU_Maintenance_FAPIv3_DCIInfo
{
    uint16_t                            dciIndex;
    uint8_t                             collocatedAl16Candidate;
    int16_t                             pdcchDmrsPowerOffsetProfileSSS;
    int16_t                             pdcchDataPowerOffsetProfileSSS;
} PDCCH_PDU_Maintenance_FAPIv3_DCIInfo;

typedef struct tPDCCH_PDU_Maintenance_FAPIv3
{
    uint16_t                            pdcchPduIndex;
    uint16_t                            obsolete16bit;

    uint8_t                             variablePart[];    // List of PDCCH_PDU_Maintenance_FAPIv3_DCIInfo for each DL DCIs

} PDCCH_PDU_Maintenance_FAPIv3;

// Table 3-66 : PDCCH PDU 

typedef struct tPDCCH_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint16_t                            coresetBWPSize;
    uint16_t                            coresetBWPStart;
    uint8_t                             subcarrierSpacing;
    uint8_t                             cyclicPrefix;
                                       
    uint8_t                             startSymbolIndex;
    uint8_t                             durationSymbols;
    uint8_t                             freqDomainResource[MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE];
    uint8_t                             cceRegMappingType;
    uint8_t                             regBundleSize;
    uint8_t                             interleaverSize;
    uint8_t                             coreSetType;
    uint16_t                            shiftIndex;
    uint8_t                             precoderGranularity;
    uint16_t                            numDlDci;

    uint8_t                             variablePart[];  

    // Variable parts following : for numDlDci :
    //   DL_DCI_PDU_part1 (front part of Table 3-56), and then
    //       TxPrecodingAndBeamforming_PDU (Rel15) or Rel16mTRP_TxPrecodingAndBeamforming_PDU (Rel16), and then
    //       DL_DCI_PDU_part2 (rear part of Table 3-56)
    //   PDCCH_PDU_Maintenance_FAPIv3_part1 (front part of Table 3-57), and then
    //       List of PDCCH_PDU_Maintenance_FAPIv3_part2 (rear part of Table 3-57) for each DL DCIs 

} PDCCH_PDU;

// Table 3-69 :PDCCH PDU parameters FAPIv4 
typedef struct tPDCCH_PDU_FAPIv4_DCIInfo
{
    uint16_t                            nIdPdcchDmrs;
} PDCCH_PDU_FAPIv4_DCIInfo;

typedef struct tPDCCH_PDU_FAPIv4_MuMimo
{
    uint8_t                             spatialStreamIndicesPresent;

    uint8_t                             variablePart[];
} PDCCH_PDU_FAPIv4_MuMimo;

typedef struct tPDCCH_PDU_FAPIv4_MuMimo_DCIInfo
{
    uint16_t                            dciIndex;
    uint16_t                            spatialStreamIndex;
} PDCCH_PDU_FAPIv4_MuMimo_DCIInfo;

///////////////////////////////////////////////////////// PDSCH PDUs //////////////////////////////////////////////////

// Table 3-71 : PDSCH PTRS parameters

typedef struct tPDSCH_PTRS
{
    uint8_t                             ptrsPortIndex;
    uint8_t                             ptrsTimeDensity;
    uint8_t                             ptrsFreqDensity;
    uint8_t                             ptrsReOffset;
    uint8_t                             nEpreRatioOfPDSCHToPTRSProfileNR;
} PDSCH_PTRS;

// Table 3-70 : PDSCH PDU

typedef struct tPDSCH_CodewordInfo
{
    uint16_t                            targetCodeRate;
    uint8_t                             qamModOrder;
    uint8_t                             mcsIndex;
    uint8_t                             mcsTable;
    uint8_t                             rvIndex;
    uint32_t                            tbSize;
} PDSCH_CodewordInfo;

typedef struct tPDSCH_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint16_t                            pduBitmap;
    uint16_t                            rnti;
    uint16_t                            pduIndex;
                                       
    // BWP                              
                                       
    uint16_t                            bwpSize;
    uint16_t                            bwpStart;
    uint8_t                             subcarrierSpacing;
    uint8_t                             cyclicPrefix;
                                       
    // Codewords                        
                                       
    uint8_t                             nrOfCodewords;
    PDSCH_CodewordInfo                  codewordInfo[MAX_NUM_OF_CORDWORD];    // Use fixed size for efficiency

    uint16_t                            nIdPdsch;
    uint8_t                             nrOfLayers;
    uint8_t                             transmissionScheme;
    uint8_t                             refPoint;
                                       
    // DMRS                             
                                       
    uint16_t                            dlDmrsSymbPos;
    uint8_t                             dmrsConfigType;
    uint16_t                            pdschDmrsScramblingId;
    uint16_t                            pdschDmrsScramblingIdComplement;
    uint8_t                             lowPaprDmrs;
    uint8_t                             nSCID;
    uint8_t                             numDmrsCdmGrpsNoData;
    uint16_t                            dmrsPorts;
                                       
    // PDSCH allocation                 
                                       
    uint8_t                             resourceAlloc;
    uint8_t                             rbBitmap[MAX_RB_BITMAP_SIZE];
    uint16_t                            rbStart;
    uint16_t                            rbSize;
    uint8_t                             vrbToPrbMapping;
    uint8_t                             startSymbolIndex;
    uint8_t                             nrOfSymbols;
    
    uint8_t                             variablePart[];    

    // Variable parts following :
    //   PDSCH_PTRS (Table 3-59) if pduBitmap[0] = 1  
    //   TxPrecodingAndBeamforming_PDU (Table 3-69 : Rel15) or Rel16mTRP_TxPrecodingAndBeamforming_PDU (Table 3-70 : Rel16)
    //   PDSCH_TxPowerInfo and then CBGReTxCtlInfo if pduBitmap[1] = 1 and L1 is configured to support L2 CBG segmentation
    //   PDSCH_CBGReTxCtlInfo if pduBitmap[1] = 1
    //   PDSCH_Maintenance_FAPIv3 (Table 3-60) and then PDSCH_CBGTxInfo if configured
    //   PDSCH_PTRS_Maintenance_FAPIv3 (Table 3-61) if pduBitmap[0] = 1
    //   Rel16_PDSCH_FAPIv3 (Table 3-62) and then PDSCH_PTRS if pduBitmap[0] = 1

} PDSCH_PDU;

typedef struct tPDSCH_CBGReTxCtlInfo
{
    uint8_t                             isLastCbPresent;
    uint8_t                             isInlineTbCrcl;
    uint32_t                            dlTbCrcCW[MAX_NUM_OF_CORDWORD];
} PDSCH_CBGReTxCtlInfo;

typedef struct tPDSCH_TxPowerInfo
{
    uint8_t                             powerControlOffsetProfileNR;
    uint8_t                             powerControlOffsetSSProfileNR;

    uint8_t                             variablePart[];    // CBGReTxCtlInfo if pduBitmap[1] = 1 and L1 is configured to support L2 CBG segmentation
} PDSCH_TxPowerInfo;

// Table 3-72 : PDSCH maintenance parameters V3

typedef struct tPDSCH_PrbSymRmPatternsInfo
{
    uint8_t                             freqDomainRB[MAX_RATEMATCH_BITMAP_SIZE];
    uint16_t                            symbolsInRB;
} PDSCH_PrbSymRmPatternsInfo;

typedef struct tPDSCH_CoresetRmPatternsInfo
{
    uint8_t                             freqDomainResources[MAX_CORESET_FREQ_DOMAIN_BITMAP_SIZE];
    uint16_t                            symbolsPattern;
} PDSCH_CoresetRmPatternsInfo;

typedef struct tPDSCH_CBGTxInfo
{
    uint8_t                             maxNumCbgPerTb;
    uint8_t                             cbgTxInformation[MAX_NUM_OF_CORDWORD];    // Use fixed size for efficiency
} PDSCH_CBGTxInfo;

typedef struct tPDSCH_Maintenance_FAPIv3
{
    uint8_t                             pdschTransType;
    uint16_t                            coresetStartPoint;
    uint16_t                            initialDlBwpSize;
    
    // Codeword information
    
    uint8_t                             ldpcBaseGraph;
    uint32_t                            tbSizeLbrmBytes;
    uint8_t                             tbCrcRequired;
    
    // Rate Matching references

    uint16_t                            ssbPdusForRateMatching[SSB_PDUS_FOR_RATE_MATCHING_SIZE];
    uint16_t                            ssbConfigForRateMatching;
    uint8_t                             prbSymRmPatternBitmapSizeByReference;
    uint8_t                             prbSymRateMatchPatternBitmapByReference[PDSCH_RATE_MATCHING_BITMAP_SIZE];

    // PrbSymRmPatternsInfo

    uint8_t                             numPrbSymRmPatternsByValue;
    PDSCH_PrbSymRmPatternsInfo          prbSymRmPatternsInfo[MAX_PDSCH_PRB_SYM_RM_PATTERNS_INFO_COUNT];    // Use fixed size for efficiency
    
    // CoresetRmPatterns

    uint8_t                             numCoresetRmPatterns;
    PDSCH_CoresetRmPatternsInfo         coresetRmPatternsInfo[MAX_PDSCH_CORESET_RM_PATTERNS_INFO_COUNT];    // Use fixed size for efficiency
    
    uint16_t                            pdcchPduIndex;
    uint16_t                            dciIndex;
    uint8_t                             lteCrsRateMatchPatternBitmapSize;
    uint8_t                             lteCrsRateMatchPattern[MAX_LTE_CRS_RATEMATCH_PATTERN_BITMAP_SIZE];    // Use fixed size for efficiency

    uint8_t                             numCsiRsForRateMatching;
    uint16_t                            csiRsForRateMatching[MAX_CSI_RS_RATEMATCH_SIZE];    // Use fixed size for efficiency
                                      
    // Tx Power info                   
                                      
    int16_t                             pdschDmrsPowerOffsetProfileSSS;
    int16_t                             pdschDataPowerOffsetProfileSSS;
                                      
    uint8_t                             variablePart[];    // PDSCH_CBGTxInfo if configured
} PDSCH_Maintenance_FAPIv3;

// Table 3-73 : PDSCH PTRS maintenance parameters V3

typedef struct  tPDSCH_PTRS_Maintenance_FAPIv3
{
    int16_t                             pdschPtrsPowerOffsetProfileSSS;
} PDSCH_PTRS_Maintenance_FAPIv3;

// Table 3-74 : Rel-16 PDSCH parameters V3

typedef struct  tRel16_PDSCH_FAPIv3
{
    uint8_t                             repetitionScheme;
    uint8_t                             variablePart[];    // PDSCH_PTRS if pduBitmap[0] = 1
} Rel16_PDSCH_FAPIv3;

// Table 3-75 PDSCH Parameters FAPIv4

typedef struct tLayerMappingcwInfo
{
    uint8_t                              numLayersForCW;
    uint8_t                              spatialStreamIndicesForCW[MAX_LAYERS_FOR_CW];    // Use fixed size for efficiency
} LayerMappingcwInfo; 

typedef struct  tPDSCH_PDU_FAPIv4 
{
    uint8_t                              coresetRmPatternBitmapSizeByReference;
    uint8_t                              coresetRmPatternBitmapByReference[MAX_SIZE_OF_RATE_MATCH_PATTERN];  // ceil((szRm = ceil(coresetRmPatternBitmapSizeByReference/ 8 )
    uint8_t                              lteCrsMbsfnDerivationMethod;
    uint8_t                              lteCrsMbsfnPattern[MAX_SIZE_OF_CRS_RATE_MATCH_PATTERN_MBSFN];
    uint8_t                              spatialStreamIndicesPresent;
    LayerMappingcwInfo                   layerMappiongcwInfo[MAX_NUM_OF_CORDWORD];    // Use fixed size for efficiency
} PDSCH_PDU_FAPIv4;

///////////////////////////////////////////////////////// CSI-RS PDUs ///////////////////////////////////////////////////

// Table 3-76 : CSI-RS PDU

typedef struct tCSI_RS_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint8_t                             subcarrierSpacing;
    uint8_t                             cyclicPrefix;
    uint16_t                            startRB;
    uint16_t                            nrOfRBs;
    uint8_t                             csiType;
    uint8_t                             row;
    uint16_t                            freqDomain;
    uint8_t                             symbL0;
    uint8_t                             symbL1;
    uint8_t                             cdmType;
    uint8_t                             freqDensity;
    uint16_t                            scrambId;
                                       
    // Tx Power Info                    
                                       
    uint8_t                             powerControlOffsetProfileNR;
    uint8_t                             powerControlOffsetSSProfileNR;
    
    uint8_t                             variablePart[]; 
    
    // Variable parts following :
    //   TxPrecodingAndBeamforming_PDU (Table 3-69 : Rel15) or Rel16mTRP_TxPrecodingAndBeamforming_PDU (Table 3-70 : Rel16)
    //   CSI_RS_PDU_MaintenanceParam_FAPIv3 (Table 3-64)

} CSI_RS_PDU;

// Table 3-77 : CSI-RS PDU maintenance parameters from FAPIv3

typedef struct tCSI_RS_PDU_MaintenanceParam_FAPIv3
{
    //Set of CSI-RS PDU parameters added in FAPIv3
    
    uint16_t                            csiRsPduIndex;
    int16_t                             csiRsPowerOffsetProfileSSS;
} CSI_RS_PDU_MaintenanceParam_FAPIv3;


// Tables 3-78 : CSI-RS PDU Parameters for FAPIv4

typedef struct tCSI_RS_PDU_FAPIv4
{
    uint8_t                             spatialStreamIndicesPresent;
    uint8_t                             spatialStreamIndices[MAX_CDM_GROUP_COUNT];    // Use fixed size for efficiency
} CSI_RS_PDU_FAPIv4;

///////////////////////////////////////////////////////// SSB PDUs //////////////////////////////////////////////////

// Table 3-79 : SSB/PBCH PDU

typedef struct tSSBPBCH_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint16_t                            physCellId;
    uint8_t                             betaPssProfileNR;
    uint8_t                             ssbBlockIndex;
    uint8_t                             ssbSubcarrierOffset;
    uint16_t                            ssbOffsetPointA;
    uint8_t                             bchPayloadFlag;
                                       
    uint8_t                             variablePart[]; 
   
    // Variable parts following :
    //   MAC_GenMIB_PDU (Table 3-67) or PHY_GenMIB_PDU (3-68), if bchPayloadFlag is [0,1] or 2, respectively
    //   TxPrecodingAndBeamforming_PDU (Table 3-69 : Rel15) or Rel16mTRP_TxPrecodingAndBeamforming_PDU (Table 3-70 : Rel16)
    //   SSBPBCH_PDU_Maintenance_FAPIv3 (Table 3-66)

} SSBPBCH_PDU;

// Table 3-81 : MAC generated MIB PDU

typedef struct tMAC_GenMIB_PDU
{
    uint32_t                            bchPayload;    // use 32 or 24 bits if bchPayloadFlag is 0 or 1, respectively
} MAC_GenMIB_PDU;

// Table 3-82 : PHY generated MIB PDU

typedef struct tPHY_GenMIB_PDU
{
    uint8_t                             dmrsTypeAPosition;
    uint8_t                             pdcchConfigSib1;
    uint8_t                             cellBarred;
    uint8_t                             intraFreqReselection;
} PHY_GenMIB_PDU;

// Table 3-80 SSB/PBCH PDU maintenance FAPIv3

typedef struct tSSBPBCH_PDU_Maintenance_FAPIv3
{
    uint8_t                             ssbPduIndex;
    uint8_t                             ssbFirstSymbolMappingCase;
    uint8_t                             subcarrierSpacing;
    uint8_t                             lMax;

    // Tx Power Info                    
                                       
    int16_t                             ssPbchBlockPowerScaling;
    int16_t                             betaPSSProfileSSS;
} SSBPBCH_PDU_Maintenance_FAPIv3;


// Table 3-83: SSB PDU Parameters for FAPIv4

typedef struct tSSBPBCH_PDU_FAPIv4
{
    uint8_t                             spatialStreamIndexPresent;
    uint8_t                             spatialStreamIndex;

} SSBPBCH_PDU_FAPIv4;

///////////////////////////////////////////////////////// DL_TTI.request //////////////////////////////////////////////////

typedef struct tPrbSymbRmP7
{
    uint8_t                             subcarrierSpacing;
    uint8_t                             freqDomainRB[35];
    uint16_t                            symbolsInRB;
} PrbSymbRmP7;

typedef struct tTopLevelCoresetRmInfo
{
    uint16_t                            numCoresetRmPattern;
    uint8_t                             coresetRmConfigurationMethod; //0 : P5 numPrbSymbRmP5Ref = numPrbSymRmPatterns    1: P7 numPrbSymbRmP5Ref = numPrbSymRmPatterns
    uint8_t                             coresetRmPatternP5Bitmap[MAX_SIZE_CORESET_P5]; // szCsP5 = ceil(numCoresetRmP5Ref / 8 )
} TopLevelCoresetRmInfo;
 
typedef struct tCorsetRmP7
{
    uint8_t                             subcarrierSpacing;
    uint8_t                             freqDomainResources[6];
    uint16_t                            symbolsInRB;
} CorsetRmP7;

typedef struct tTopLevelLteRmInfo
{
    uint16_t                            numLteCrsRmPattern;
    uint8_t                             lteCrsRmConfigurationMethod; //0 : P5 numLteCrsRmP5Ref = numLteCrsRmPatterns   1: P7 numLteCrsRmP7Sig = numLteCrsRmPatterns
    uint8_t                             lteCrsRmPatternP5Bitmap[MAX_SIZE_LTE_CRS_P5]; // szCsP5 = ceil(numLteCrsRmP5Ref / 8 )
} TopLevelLteRmInfo;

typedef struct tLteCrstRmP7
{
    uint16_t                            carrierFreqDL;
    uint8_t                             carrierBandwidthDL;
    uint8_t                             nrofCrsPorts;
    uint8_t                             vShifts;
    uint8_t                             sizeMbsfnSubframeConfigList;

    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of MbsfnSubframeConfigInfo
     
} LteCrstRmP7;

// Top_level Rate Matching Patterns

typedef struct tTopLevelRateMatchPatterns
{
    uint16_t                            numPrbSymRmPatterns;
    uint8_t                             prbSymbRmConfigurationMethod;  //0 : P5 numPrbSymbRmP5Ref = numPrbSymRmPatterns    1: P7 numPrbSymbRmP5Ref = numPrbSymRmPatterns
    uint8_t                             prbSymRMPatternP5Bitmap[MAX_SIZE_PRB_SYM_P5];    //szPsP5 = ceil(numPrbSymbRmP5Ref / 8 )    
    
    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of PrbSymbRmP7,CorsetRmP7,LteCrstRmP7
     
} TopLevelRateMatchPatterns;

// Table 3-64 : DL_TTI.request message body 

typedef struct tDL_TTI_REQ
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint16_t                            numPDUs;
    uint8_t                             nDlTypes;
    uint16_t                            nPDUsOfEachType[NUM_OF_DLPDU_TYPE];
    uint8_t                             nGroup;

    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of PDCCH_PDU (Table 3-55), PDSCH_PDU (Table 3-58), CSI_RS_PDU (Table 3-63) or SSBPBCH_PDU (Table 3-65) for numPDUs
    //   List of DL_UE_GroupInfo for each nGroup

} DL_TTI_REQ;

typedef struct tDL_UE_GroupInfo
{
    uint8_t                             nPdus;
    uint16_t                            prgSize;
    uint8_t                             prbBitmap[MAX_RB_BITMAP_SIZE];
    uint16_t                            symbolBitmap;

    uint8_t                             variablePart[];

     // Variable parts following :
     //   List of DL_UE_Group_PDU_Info and TopLevelRateMatchPatterns

} DL_UE_GroupInfo;

typedef struct tDL_UE_Group_PDU_Info
{
    uint8_t                            pduIdx;
    struct
    {
        uint8_t                            chosenLayerBitmapForCW0;
        uint8_t                            chosenLayerBitmapForCW1;
    } txPrecoder[MAX_PRG_INDEX_COUNT];
} DL_UE_Group_PDU_Info;

///////////////////////////////////////////////////////// DL_TTI.response //////////////////////////////////////////////////

// Table 3-87 : DL_TTI.response message body

typedef struct tDL_TBCrcInfo
{
    uint16_t                            pduIndex;
    uint8_t                             cw;
    uint32_t                            tbCrc;
} DL_TBCrcInfo;

typedef struct tDL_TTI_RESP
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint16_t                            numPDUs;

    // Variable parts following :
    //   List of DL_TBCrcInfo for each numPDUs

} DL_TTI_RESP;

///////////////////////////////////////////////////////// Tx_Data.request //////////////////////////////////////////////////

// Table 3-114 : TBS-TLV structure

typedef struct tTBS_TLV_Struct
{
    uint16_t                            tag; 
    uint32_t                            length; 

    uint8_t                             variablePart[];    // size is determined by the tag/length and pdschMacPduBitAlignment capability
} TBS_TLV_Struct;

// Table 3-113 : Tx_Data.request message

typedef struct  tNumOfPDU
{
    uint32_t                            pduLength;
    uint16_t                            pduIndex;
    uint8_t                             cwIndex;
    uint32_t                            numTLV;

    uint8_t                             variablePart[];    // List of TBS_TLV_Struct for each numTLV
} TxDataPduInfo; 

typedef struct tTx_DATA_REQ
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint16_t                            numPDUs;
    uint16_t                            controlLength;

    uint8_t                             variablePart[]; 
    
    // Variable parts following :
    //   List of TxDataPduInfo for each numPDUs

} Tx_DATA_REQ; 

///////////////////////////////////////////////////////// UL_DCI.request //////////////////////////////////////////////////

// Table 3-109 : UL_DCI.request message body

typedef struct tUL_DCI_REQ
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint16_t                            numPDUs;
    uint8_t                             nDlTypes;
    uint16_t                            nPDUsOfEachType[NUM_OF_ULDCIPDU_TYPE];

    uint8_t                             variablePart[]; 
    
    // Variable parts following :
    //   List of PDCCH_PDU (Table 3-55) for each numPDUs

} UL_DCI_REQ;

///////////////////////////////////////////////////////// PRACH PDU //////////////////////////////////////////////////

// Table 3-107 : Rx beamforming PDU

typedef struct tRx_Beamforming_PDU
{
    uint8_t                             trpScheme;
    uint16_t                            numPRGs; 
    uint16_t                            prgSize; 
    uint8_t                             digBFInterface; 
                       
    uint8_t                             variablePart[];    // List of [uint16_t beamIdx for each digBFInterface] for each numPRGs
} Rx_Beamforming_PDU;

// Table 3-108 : SRS_based Rx beamforming PDU

typedef struct tSRS_based_Rx_Beamforming_PDU
{
    uint8_t                             trpScheme;
    uint8_t                             numMuMimoGroups;
    uint16_t                            muMimoGroupIdx[MAX_NUM_OF_MIMO_GROUP]; 
} SRS_based_Rx_Beamforming_PDU;

// Table 3-90 : PRACH maintenance FAPIv3

typedef struct tPRACH_Maintenance_FAPIv3
{
    uint32_t                            handle;
    uint8_t                             prachConfigScope;
    uint16_t                            prachResConfigIndex;
    uint8_t                             numFdRa;
    uint8_t                             startPreambleIndex;
    uint8_t                             numPreambleIndices;
} PRACH_Maintenance_FAPIv3;

// Table 3-91 : Uplink spatial stream assignment FAPIv4

typedef struct tUplinkSpatialStreamAssignment_FAPIv4
{
    uint8_t                             numSpatialStreams;
    uint8_t                             spatialStreamIndices[MAX_NUM_UL_SPATIAL_STREAMS];    // Use fixed size for efficiency
} UplinkSpatialStreamAssignment_FAPIv4;

// Table 3-92 : MsgA-PRACH to MsgA-PUSCH map signalling in FAPIv4

typedef struct tMsgAPrachToMsgAPuschMap_FAPIv4
{
    uint8_t                             preambleGroups;
    uint16_t                            dmrsPorts[MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG];
    uint8_t                             availableDmrsSequenceIds[MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG];
    uint8_t                             numPuschOcasFd[MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG];
    uint8_t                             numPuschOcasTd[MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG];
    uint8_t                             numPuschOcasSlots[MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG];
    uint8_t                             msgAPUSCHTimeDomainOffset[MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG];
    uint16_t                            nPreambles[MAX_NUM_MSGA_PRACH_TO_MSGA_PUSCH_MAP_CONFIG];
    MSGA_PRACH_Mapping_Config           msgAPrachtoMapping;
} MsgAPrachToMsgAPuschMap_FAPIv4;

// Table 3-89 : PRACH PDU

typedef struct tPRACH_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint16_t                            physCellID;
    uint8_t                             numPrachOcas;
    uint8_t                             prachFormat;
    uint8_t                             indexFdRa;
    uint8_t                             prachStartSymbol;
    uint16_t                            numCs;
    uint8_t                             isMsgAPrach;
    uint8_t                             hasMsgAPuschBeamforming;

    uint8_t                             variablePart[];    

    // Variable parts following : 
    //   Rx_Beamforming_PDU (Table 3-107), and then List of [uint16_t beamIdx for each digBFInterface] for each numPRGs
    //   PRACH_Maintenance_FAPIv3 (Table 3-90)

} PRACH_PDU;

///////////////////////////////////////////////////////// PUSCH PDU //////////////////////////////////////////////////

// Table 3-93 : PUSCH PDU

typedef struct tPUSCH_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint16_t                            pduBitmap;
    uint16_t                            rnti;
    uint32_t                            handle;
                                       
    // BWP                              
                                       
    uint16_t                            bwpSize;
    uint16_t                            bwpStart;
    uint8_t                             subcarrierSpacing;
    uint8_t                             cyclicPrefix;
    
    // PUSCH information always included
    
    uint16_t                            targetCodeRate;
    uint8_t                             qamModOrder;
    uint8_t                             mcsIndex;
    uint8_t                             mcsTable;
    uint8_t                             transformPrecoding;
    uint16_t                            nIdPusch;
    uint8_t                             nrOfLayers;
                                       
    // DMRS                             
                                       
    uint16_t                            ulDmrsSymbPos;
    uint8_t                             dmrsConfigType;
    uint16_t                            puschDmrsScramblingId;
    uint16_t                            puschDmrsScramblingIdComplement;
    uint8_t                             lowPaprDmrs;
    uint16_t                            puschDmrsIdentity;
    uint8_t                             nSCID;
    uint8_t                             numDmrsCdmGrpsNoData;
    uint16_t                            dmrsPorts;
    
    // Pusch Allocation in frequency domain
    
    uint8_t                             resourceAlloc;
    uint8_t                             rbBitmap[MAX_RB_BITMAP_SIZE];
    uint16_t                            rbStart;
    uint16_t                            rbSize;
    uint8_t                             vrbToPrbMapping;
    uint8_t                             intraSlotFrequencyHopping;
    uint16_t                            txDirectCurrentLocation;
    uint8_t                             uplinkFrequencyShift7p5khz;

    // Resource Allocation in time domain

    uint8_t                             startSymbolIndex;
    uint8_t                             nrOfSymbols;
    
    uint8_t                             variablePart[];

    // Variable parts following :
    //   PuschDataInfo (Table 3-96) if the Bit 0 of pduBitmap is 1
    //   PuschUCIInfo  (Table 3-97) if the Bit 1 of pduBitmap is 1
    //   PuschPTRSInfo (Table 3-98) if the Bit 2 of pduBitmap is 1
    //   DftsOfdmInfo  (Table 3-99) if the Bit 3 of pduBitmap is 1
    //   Rx_Beamforming_PDU (Table 3-107), and then List of [uint16_t beamIdx for each digBFInterface] for each numPRGs
    //   PUSCH_Maintenance_FAPIv3 (Table 3-94)
    //   PUSCH_FAPIv4 (Table 3-100)
    //   UCIInfo_FAPIv3 (Table 3-77) if the Bit 1 of pduBitmap is 1

} PUSCH_PDU;

// Table 3-96 : Optional puschData information

typedef struct tPuschDataInfo
{
    uint8_t                             rvIndex; 
    uint8_t                             harqProcessID; 
    uint8_t                             newData;
    uint32_t                            tbSize; 
    uint16_t                            numCb; 
    uint8_t                             cbPresentAndPosition[MAX_NUM_CB_PRESENT_POSITION]; 
} PuschDataInfo;

// Table 3-97 : Optional puschUci information

typedef struct tPuschUCIInfo
{
    uint16_t                            harqAckBitLength; 
    uint16_t                            csiPart1BitLength; 
    uint16_t                            flagCsiPart2;
    uint8_t                             alphaScaling; 
    uint8_t                             betaOffsetHarqAck; 
    uint8_t                             betaOffsetCsi1; 
    uint8_t                             betaOffsetCsi2;
} PuschUCIInfo;

// Table 3-98 : Optional puschPtrs information

typedef struct tPuschPTRSPortInfo
{
    uint16_t                            ptrsPortIndex; 
    uint8_t                             ptrsDmrsPort; 
    uint8_t                             ptrsReOffset; 
} PuschPTRSPortInfo;

typedef struct tPuschPTRSInfo
{
    uint8_t                             numPtrsPorts;
    PuschPTRSPortInfo                   ptrsPortInfo[MAX_UL_PTRS_PORT];    // Use fixed size for efficiency
 
    uint8_t                             ptrsTimeDensity; 
    uint8_t                             ptrsFreqDensity; 
    uint8_t                             ulPTRSPower;    
} PuschPTRSInfo;

// Table 3-99 : Optional dftsOfdm information

typedef struct tDftsOfdmInfo
{
    uint8_t                             lowPaprGroupNumber; 
    uint16_t                            lowPaprSequenceNumber; 
    uint8_t                             ulPtrsSampleDensity; 
    uint8_t                             ulPtrsTimeDensityTransformPrecoding; 
} DftsOfdmInfo;

// Table 3-94 : PUSCH maintenance FAPIv3

typedef struct tPUSCH_Maintenance_FAPIv3
{
    uint8_t                             puschTransType;
    uint16_t                            deltaBwp0StartFromActiveBwp;
    uint16_t                            initialUlBwpSize;
                                       
    // DMRS

    uint8_t                             groupOrSequenceHopping;
    
    // FrequencyDomain Allocation
    
    uint16_t                            puschSecondHopPRB;
    uint8_t                             ldpcBaseGraph;
    uint32_t                            tbSizeLbrmBytes;
} PUSCH_Maintenance_FAPIv3;

// Table 3-100 : Pusch Parameters added in FAPIv4

typedef struct tPUSCH_FAPIv4
{
    uint8_t                             cbCrsStatusRequest;
    uint32_t                            srsTxPorts;
    uint8_t                             ulTpmiIndex;
    uint8_t                             numULSpatialStreamPorts;
    uint8_t                             ulSpatialStreamPorts[MAX_NUM_UL_SPATIAL_STREAM_PORTS];
} PUSCH_FAPIv4;

// Table 3-101 : MsgA Pusch PDu

typedef struct MSGA_PUSCH_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint16_t                            pduBitmap;
    uint8_t                             prachToPruMapType;
    uint32_t                            prachToPruMapIndicator;
    uint16_t                            msgAprachSfn;
    uint16_t                            msgAprachSlot;
    uint32_t                            handle;
    uint16_t                            bwpSize;
    uint16_t                            bwpStart;
    uint8_t                             subcarrierSpacing;
    uint8_t                             cyclicPrefix;
    uint8_t                             msgAmcs;
    uint8_t                             transformPrecoding;
    uint16_t                            nIdMsgAPusch;
    uint16_t                            puschDmrsScramblingId[2];
    uint16_t                            ulDmrsSymbPos;
    uint8_t                             dmrsMappingType;
    uint16_t                            startRbPuschOcas;
    uint8_t                             numRbPUschOcas;
    uint8_t                             guardBand;
    uint8_t                             intraSlotFrequencyHopping;
    uint8_t                             intraSlotFrequencyHoppingBits;
    uint16_t                            txDirectCurrentLocation;
    uint8_t                             uplinkFrequencyShift7p5khz;
    uint8_t                             startSymbolIdPuschOcas;
    uint8_t                             durationPuschOcas;
    uint8_t                             guardPeriod;   
    uint32_t                            tbSize;
} MSGA_PUSCH_PDU;

// Table 3-95 : Uci information for determining UCI Part1 to Part2 correspondence, added in FAPIv3

typedef struct tUCIPart2Info_FAPIv3
{
    uint16_t                            priority;
    uint8_t                             numPart1Params;
    uint16_t                            paramOffsets[MAX_NUM_OF_UCI_PART1_PARAMS];    // Use fixed size for efficiency
    uint8_t                             paramSizes[MAX_NUM_OF_UCI_PART1_PARAMS];      // Use fixed size for efficiency
    uint16_t                            part2SizeMapIndex;
    uint8_t                             part2SizeMapScope;                             
} UCIPart2Info_FAPIv3;

typedef struct tPuschUCIInfo_FAPIv3
{
    uint16_t                            numPart2s;

    uint8_t                             variablePart[];    // List of UCIPart2Info_FAPIv3 for each numPart2s
} PuschUCIInfo_FAPIv3;

///////////////////////////////////////////////////////// PUCCH PDU //////////////////////////////////////////////////

// Table 3-102 : PUCCH PDU

typedef struct tPUCCH_PDU
{   
    PDU_Struct                          sPDU_Hdr; 

    uint16_t                            rnti;
    uint32_t                            handle;
                       
    // BWP
                       
    uint16_t                            bwpSize;
    uint16_t                            bwpStart;
    uint8_t                             subcarrierSpacing;
    uint8_t                             cyclicPrefix;
                       
    uint8_t                             formatType;
    uint8_t                             multiSlotTxIndicator;
    uint8_t                             pi2Bpsk;
                       
    uint16_t                            prbStart;
    uint16_t                            prbSize;
                       
    uint8_t                             startSymbolIndex;
    uint8_t                             nrOfSymbols;
                       
    uint8_t                             intraSlotFrequencyHopping;
    uint16_t                            secondHopPRB;
    uint8_t                             pucchGroupHopping;
    uint8_t                             obsolete8bit;
    uint16_t                            nIdPucchHopping;
    uint16_t                            initialCyclicShift;
                       
    uint16_t                            nIdPucchScrambling;
    uint8_t                             timeDomainOccIdx;
    uint8_t                             preDftOccIdx; 
    uint8_t                             preDftOccLen;
                       
    // DMRS

    uint8_t                             addDmrsFlag; 
    uint16_t                            nId0PucchDmrsScrambling;
    uint8_t                             m0PucchDmrsCyclicShift;
    uint8_t                             srBitLen;
    uint16_t                            bitLenHarq; 
    uint16_t                            csiPart1BitLength;
    
    uint8_t                             variablePart[];

    // Variable parts following :
    //   Rx_Beamforming_PDU (Table 3-107), and then List of [uint16_t beamIdx for each digBFInterface] for each numPRGs
    //   PUCCH_BasicExtension (Table 3-103)
    //   UCIInfo_FAPIv3 (Table 3-95) 

} PUCCH_PDU;

// Table 3-103 : PUCCH basic extension for FAPIv3

typedef struct tPUCCH_PDU_Maintenance_FAPIv3
{
    uint8_t                             maxCodeRate;
    uint8_t                             ulBwpId;
} PUCCH_PDU_Maintenance_FAPIv3;

// Table 3-104 : PUCCH basic extension

typedef struct    tPUCCH_BasicExtension_FAPIv4_SRS_PDU
{
    uint8_t                             uciReportFormat;
    uint8_t                             numUlSpatialStreamsPorts;
    uint8_t                             ulSpatialStreamPorts[MAX_NUM_UL_SPATIAL_STREAM_PORTS];
} PUCCH_BasicExtension_FAPIv4_SRS_PDU;

///////////////////////////////////////////////////////// SRS PDU ////////////////////////////////////////////////////

// Table 3-105 : SRS PDU

typedef struct tSRS_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint16_t                            rnti;
    uint32_t                            handle;
                       
    // BWP

    uint16_t                            bwpSize;
    uint16_t                            bwpStart;
    uint8_t                             subcarrierSpacing;
    uint8_t                             cyclicPrefix;
                       
    uint8_t                             numAntPorts;
    uint8_t                             numSymbols;
    uint8_t                             numRepetitions;
    uint8_t                             timeStartPosition;
    uint8_t                             configIndex;
    uint16_t                            sequenceId;
    uint8_t                             bandwidthIndex;
    uint8_t                             combSize;
    uint8_t                             combOffset;
    uint8_t                             cyclicShift;
    uint8_t                             frequencyPosition;
    uint16_t                            frequencyShift;
    uint8_t                             frequencyHopping;
    uint8_t                             groupOrSequenceHopping;
    uint8_t                             resourceType;
    uint16_t                            tSrs;
    uint16_t                            tOffset;
    
    uint8_t                             variablePart[];

    // Variable parts following :
    //   Rx_Beamforming_PDU (Table 3-107), and then List of [uint16_t beamIdx for each digBFInterface] for each numPRGs
    //   SRS_PDU_FAPIv4 (Table 3-106)

} SRS_PDU;

// Table 3-106 FAPIv4 SRS Parameters

typedef struct tSRS_PDU_FAPIv4_SymbolInfo
{
    uint16_t                            srsBandwidthStart;
    uint8_t                             sequenceGroup;
    uint8_t                             sequenceNumber;
} SRS_PDU_FAPIv4_SymbolInfo;

typedef struct tSRS_PDU_FAPIv4
{
    uint16_t                            srsBandwidthSize;

    SRS_PDU_FAPIv4_SymbolInfo           srsSymbolInfo[MAX_SRS_IND_SYMBOL_INFO_COUNT];    // Use fixed size for efficiency

    uint32_t                            usage;
    uint8_t                             reportType[NAX_NUM_SRS_USAGE];    
    uint8_t                             singularValueRepresentation;
    uint8_t                             iqRepresentation;
    uint16_t                            prgSize;
    uint8_t                             numTotalUeAntennas;
    uint32_t                            ueAntennasInThisSrsResourceSet;
    uint32_t                            sampledUeAntennas;
    uint8_t                             reportScope;
    uint8_t                             numULSpatialStreamsPorts;
    uint8_t                             ulSpatialStreamPorts[MAX_NUM_UL_SPATIAL_STREAM_PORTS];
} SRS_PDU_FAPIv4;

///////////////////////////////////////////////////////// UL_TTI.request //////////////////////////////////////////////////

// Table 3-88 : UL_TTI.request message body

typedef struct tUL_TTI_REQ
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint16_t                            numPDUs;
                       
    uint8_t                             nUlTypes;
    uint16_t                            nPDUsOfEachType[NUM_OF_ULPDU_TYPE];
    uint8_t                             nGroup;
    
    uint8_t                             variablePart[];

     // Variable parts following :
     //   List of PRACH_PDU (Table 3-73), PUSCH_PDU (Table 3-75), PUCCH_PDU (Table 3-82) or SRS_PDU (Table 3-84) for each numPDUs
     //   List of UL_UE_GroupInfo for each nGroup

} UL_TTI_REQ;

typedef struct tUL_UE_GroupInfo
{
    uint8_t                             nPDUs;

    uint8_t                             variablePart[];

     // Variable parts following :
     //   List of UL_UE_Group_PDU_Info

} UL_UE_GroupInfo;

typedef struct tUL_UE_Group_PDU_Info
{
    uint8_t                            pduIdx;
    uint16_t                           prgSize;
    uint8_t                            prbBitmap[MAX_RB_BITMAP_SIZE];
    uint16_t                           symbolBitmap;
} UL_UE_Group_PDU_Info;

///////////////////////////////////////////////////////// RX_Data.indication //////////////////////////////////////////////////

// Table 3-116 : RX_Data.indication message body

typedef struct tRx_Data_IND_PDU
{
    uint32_t                            handle;
    uint16_t                            rnti;
    uint8_t                             rapId;
    uint8_t                             harqID;
    uint32_t                            pduLength;
    uint8_t                             pduTag;
    uint8_t                             variablePart[];    // Contents of PDU : pduLength and padding for puschMacPduBitAlignment
} Rx_Data_IND_PDU;

typedef struct tRx_Data_IND
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint16_t                            controlLength;
    uint16_t                            numPDUs;
                       
    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of Rx_Data_IND_PDU with variable-size contents of PDU for each numPDUs

} Rx_Data_IND;

///////////////////////////////////////////////////////// CRC.indication //////////////////////////////////////////////////

// Table 3-117 : CRC.indication message body

typedef struct tCRC_IND_PDU
{
    uint32_t                            handle; 
    uint16_t                            rnti;
    uint8_t                             rapId; 
    uint8_t                             harqID; 
    uint8_t                             tbCrcStatus; 
    uint16_t                            numCb; 
    uint8_t                             cbCrcStatus[MAX_NUM_CB_CRC_STATUS]; 
    uint16_t                            ulSnrMetric;
    uint16_t                            timingAdvanceOffset;
    int16_t                             timingAdvanceOffsetNanoSecond; 
    uint16_t                            rssi; 
    uint16_t                            rsrp;
} CRC_IND_PDU;

typedef struct tCRC_IND
{
    uint16_t                            sfn; 
    uint16_t                            slot; 
    uint16_t                            numCRCs; 

    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of CRC_IND_PDU for each numCRCs

} CRC_IND;

///////////////////////////////////////////////////////// UCI.indication //////////////////////////////////////////////////

// Table 3-118 : UCI.indication message body

typedef struct tUCI_IND
{
    uint16_t                            sfn; 
    uint16_t                            slot; 
    uint16_t                            numUCIs; 
                       
    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of UCI_IND_PUSCH_PDU, UCI_IND_PUCCH_PDU_Format01 or UCI_IND_PUCCH_PDU_Format234 for each numUCIs

} UCI_IND;

// Table 3-119: UCI PUSCH PDU

typedef struct tUCI_IND_PUSCH_PDU
{
    PDU_Struct                          sPDU_Hdr;

    uint8_t                             pduBitmap;
    uint32_t                            handle;
    uint16_t                            rnti;
    int16_t                             ulSnrMetric;
    uint16_t                            timingAdvanceOffset;
    int16_t                             timingAdvanceOffsetNanoSecond;
    uint16_t                            rssi;
    uint16_t                            rsrp;

    uint8_t                             variablePart[];

    // Variable parts following :
    //   UCI_IND_HarqInfo_Format234 (Table 3-125) if the Bit 1 of pduBitmap is 1
    //   UCI_IND_CSI_Part1Info      (Table 3-126) if the Bit 2 of pduBitmap is 1
    //   UCI_IND_CSI_Part2Info      (Table 3-127) if the Bit 3 of pduBitmap is 1

} UCI_IND_PUSCH_PDU;
 
// Table 3-125 : HARQ PDU for format 2, 3 or 4
  
typedef struct tUCI_IND_HarqInfo_Format234
{
    uint8_t                             harqDetectionStatus;
    uint16_t                            expectedHarqBitLen;
    uint8_t                             harqPayload[MAX_UCI_IND_HARQ_FORMAT234_PAYLOAD_SIZE];    // Use fixed size for efficiency
} UCI_IND_HarqInfo_Format234;

// Table 3-126 : CSI Part1 PDU

typedef struct tUCI_IND_CSI_Part1Info
{
    uint8_t                             csiPart1DetectionStatus;
    uint16_t                            expectedCsiPart1BitLen;
    uint8_t                             csiPart1Payload[MAX_UCI_IND_CSI_PART1_PAYLOAD_SIZE];    // Use fixed size for efficiency
} UCI_IND_CSI_Part1Info;

// Table 3-127 : CSI Part2 PDU

typedef struct tUCI_IND_CSI_Part2Info
{
    uint8_t                             csiPart2DetectionStatus;
    uint16_t                            expectedCsiPart2BitLen;
    uint8_t                             csiPart2Payload[MAX_UCI_IND_CSI_PART2_PAYLOAD_SIZE];    // Use fixed size for efficiency
} UCI_IND_CSI_Part2Info;


// Table 3-128 : UCI Payload for PUSCH or PUCCH transport

typedef struct tUCI_IND_UCI_PayloadInfo
{
    uint8_t                             uciDetectionStatus;
    uint16_t                            expectedUciPayloadSize;
    uint8_t                             uciPayload[MAX_UCI_IND_PAYLOAD_SIZE];   /// ceil(Expected UCI payload size/8) or zero
} UCI_IND_UCI_PayloadInfo;
// Table 3-120 : UCI PUCCH format 0 or 1 PDU

typedef struct tUCI_IND_PUCCH_PDU_Format01
{
    PDU_Struct                          sPDU_Hdr;

    uint8_t                             pduBitmap;
    uint32_t                            handle;
    uint16_t                            rnti;
    uint8_t                             pucchFormat;
    uint16_t                            ulSnrMetric;
    uint16_t                            timingAdvanceOffset;
    int16_t                             timingAdvanceOffsetNanoSecond;
    uint16_t                            rssi;
    uint16_t                            rsrp;
    
    uint8_t                             variablePart[];

    // Variable parts following :
    //   UCI_IND_SRInfo_Format01   (Table 3-122 ) if the Bit 0 of pduBitmap is 1
    //   UCI_IND_HarqInfo_Format01 (Table 3-123) if the Bit 1 of pduBitmap is 1

} UCI_IND_PUCCH_PDU_Format01;

// Table 3-122 : SR PDU for format 0 or 1

typedef struct tUCI_IND_SRInfo_Format01
{
    uint8_t                             srIndication;
    uint8_t                             srConfidenceLevel;
} UCI_IND_SRInfo_Format01;

// Table 3-123 : HARQ PDU for format 0 or 1

typedef struct tUCI_IND_HarqInfo_Format01
{
    uint8_t                             numHarq;
    uint8_t                             harqConfidenceLevel;
    uint8_t                             harqValue[MAX_UCI_IND_HARQ_FORMAT01_PAYLOAD_SIZE];    // Use fixed size for efficiency
} UCI_IND_HarqInfo_Format01;

// Table 3-121 : UCI PUCCH format 2, 3 or 4 PDU

typedef struct tUCI_IND_PUCCH_PDU_Format234
{
    PDU_Struct                          sPDU_Hdr;

    uint8_t                             pduBitmap;
    uint32_t                            handle;
    uint16_t                            rnti;
    uint8_t                             pucchFormat;
    uint16_t                            ulSnrMetric;
    uint16_t                            timingAdvanceOffset;
    int16_t                             timingAdvanceOffsetNanoSecond;
    uint16_t                            rssi;
    uint16_t                            rsrp;
    
    uint8_t                             variablePart[];

    // Variable parts following :
    //   UCI_IND_SRInfo_Format234   (Table 3-124) if the Bit 0 of pduBitmap is 1
    //   UCI_IND_HarqInfo_Format234 (Table 3-125) if the Bit 1 of pduBitmap is 1
    //   UCI_IND_CSI_Part1Info      (Table 3-126) if the Bit 2 of pduBitmap is 1
    //   UCI_IND_CSI_Part2Info      (Table 3-127) if the Bit 3 of pduBitmap is 1
    //   UCI_IND_UCI_PayloadInfo    (Table 3-128) 
} UCI_IND_PUCCH_PDU_Format234;

// Table 3-124 : SR PDU for format 2, 3 or 4

typedef struct tUCI_IND_SRInfo_Format234
{
    uint16_t                            srBitLen;
    uint16_t                            srPayload[MAX_UCI_IND_SR_FORMAT234_PAYLOAD_SIZE];
} UCI_IND_SRInfo_Format234;

///////////////////////////////////////////////////////// RACH.indication //////////////////////////////////////////////////

// Table 3-134 RACH.indication message body

typedef struct tRACH_IND_PreambleInfo
{
    uint8_t                             preambleIndex;
    uint16_t                            timingAdvanceOffset;
    int32_t                             timingAdvanceOffsetNanoSeconds;
    uint32_t                            preamblePwr;
    uint8_t                             preambleSnr;
} RACH_IND_PreambleInfo;

typedef struct tRACH_IND_PDU
{
    uint16_t                            handle;
    uint8_t                             symbolIndex;
    uint8_t                             slotIndex;
    uint8_t                             raIndex;
    uint16_t                            avgRssi;
    uint16_t                            rsrp;
    uint8_t                             avgSnr;
    uint8_t                             numPreambles;
                       
    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of RACH_IND_PreambleInfo for each numPreambles

} RACH_IND_PDU;

typedef struct tRACH_IND
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint8_t                             numPDUs;

    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of RACH_IND_PDU for each numPDUs

} RACH_IND;

///////////////////////////////////////////////////////// SRS.indication //////////////////////////////////////////////////

// Table 3-129: SRS.indication message body

typedef struct tSRS_Report_TLV
{
    uint16_t                            tag;
    uint32_t                            length;
    uint32_t                            value;
} SRS_Report_TLV;

typedef struct tSRS_IND_SymbolInfo
{
    uint16_t                            numPRGs;
    uint8_t                             rbSNR[MAX_SRS_IND_NUM_RB];    // Use fixed size for efficiency
} SRS_IND_SymbolInfo;

typedef struct tSRS_IND_PDU
{
    uint32_t                            handle;
    uint16_t                            rnti;
    uint16_t                            timingAdvanceOffset;
    int16_t                             timingAdvanceOffsetNanoSeconds;
    uint8_t                             srsUsage;
    uint8_t                             reportType;
    SRS_Report_TLV                      reportTLV;                      
} SRS_IND_PDU;

typedef struct tSRS_IND
{
    uint16_t                            sfn;
    uint16_t                            slot;
    uint16_t                            controlLength;
    uint8_t                             numPDUs;
                       
    uint8_t                             variablePart[];

    // Variable parts following :
    //   List of SRS_IND_PDU for each numPDUs

} SRS_IND;

// Table 3-131 : FAPIv3 Beamforming report, with PRG-level resolution

typedef struct tFAPIv3_Beamforming_Report_PRG_Level
{
    uint16_t                        prgSize;
    uint8_t                         numSymbols;
    uint8_t                         wideBandSNR;
    uint8_t                         numReportedSymbols;

    uint8_t                         variablePart[];
    
    // Variable parts following :
    //   List of SRS_IND_SymbolInfo for each numReportedSymbols
} FAPIv3_Beamforming_Report_PRG_Level;

// Table 3-132 : Normalized Channel I/Q Matrix

typedef struct tNormalized_Channel_IQ_Matrix
{
    uint8_t                         normalizedIQrepresentaion;
    uint16_t                        numGnbAntennaElements;
    uint16_t                        numUeSrsPorts;
    uint16_t                        prgSize;
    uint16_t                        numPRGs;
    uint8_t                         arrayRepresentingChannelMatrixH[MAX_CHANNEL_ARRAY_SIZE]; 

} Normalized_Channel_IQ_Matrix;

// Table 3-133 : Channel SVD Representation

typedef struct tChannel_SVD_Info
{
    uint8_t                         leftEigenVectors[MAX_CHANNEL_ARRAY_SIZE];  //U
    uint8_t                         singularMatrix[MAX_CHANNEL_ARRAY_SIZE];    // digonal entry..
    uint8_t                         rightEigenVectors[MAX_CHANNEL_ARRAY_SIZE]; 
} Channel_SVD_Info;


typedef struct tChannel_SVD_Representaion
{
    uint8_t                         normalizedIQrepresentaion;
    uint8_t                         normalizedSingularValueRepresentation;
    int8_t                          singularValueScaling;
    uint16_t                        numGnbAntennaElements;
    uint8_t                         nunUeSrsPorts;
    uint16_t                        prgSize;
    uint16_t                        numPRGs;
    
    uint8_t                         variablePart[];
    
    // Variable parts following :
    //   List of Channel Decomposion Info for each PRGs

} Channel_SVD_Representaion;

///////////////////////////////////////////////////////// for internal use //////////////////////////////////////////////////

#define MAX_TEST_CONFIG_FILENAME_LEN 1024

typedef struct tTestConfigReq
{
    uint32_t                        slotCountToRun;
    uint8_t                         basePath[MAX_TEST_CONFIG_FILENAME_LEN];
    uint8_t                         filenameDlIqRef[MAX_TEST_CONFIG_FILENAME_LEN];
    uint8_t                         filenameDlIqOutput[MAX_TEST_CONFIG_FILENAME_LEN];
    uint8_t                         filenameUlIqRef[MAX_TEST_CONFIG_FILENAME_LEN];
    uint8_t                         filenameUlResultRef[MAX_TEST_CONFIG_FILENAME_LEN];
    uint8_t                         filenameUlResultOutput[MAX_TEST_CONFIG_FILENAME_LEN];
} TestConfigReq;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mac_CreateConfigReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, FapiConfigInfo *p_FapiConfigInfo, uint8_t cellIndex );
void mac_CreateTestConfigReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, TestConfigReq *pTestConfigReqSrc, uint8_t cellIndex );
void mac_CreateStartReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex );
void mac_CreateStopReq( uint8_t *p_MsgBuff, uint32_t msgBuffSize, uint32_t *p_MsgLength, uint8_t cellIndex );
void mac_ParseMsg_ConfigResp( Fapi_Generic_Header *p_FapiGenericHeader );
void mac_ParseMsg_CrcInd( Fapi_Generic_Header *p_FapiGenericHeader );
void mac_ParseMsg_RxDataInd( Fapi_Generic_Header *p_FapiGenericHeader, uint8_t *p_RxBuffList[], uint32_t *p_rxBuffListCount );

void phy_ParseMsg_ConfigReq( Fapi_Generic_Header *p_FapiGenericHeader, FapiConfigInfo *p_FapiConfigInfo );
void phy_ParseMsg_TestConfigReq( Fapi_Generic_Header *p_FapiGenericHeader, TestConfigReq *p_TestConfigReqDest );
void phy_ParseMsg_DLTTIReq( Fapi_Generic_Header *p_FapiGenericHeader );
void phy_ParseMsg_TxDataReq( Fapi_Generic_Header *p_FapiGenericHeader );
void phy_ParseMsg_ULTTIReq( Fapi_Generic_Header *p_FapiGenericHeader );

#pragma pack()

#ifdef __cplusplus
}
#endif

#endif    // of _GSM_L1_L2_API_H_



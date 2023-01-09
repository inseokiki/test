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
 * @brief This file consists of parameters that are to be read from phycfg.xml (Radio Mode) /
 * phycfg_timer.xml (Timer Mode) to configure the PHY at system initialization
 * @file phycfg.h
 * @ingroup group_source_auxlib_phycfg
 * @author Intel Corporation
 **/



#ifndef _PHYCFG_H_
#define _PHYCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common_typedef.h"
#include "common_phy_def.h"

#define PHYCFG_FILE_NAME            "phycfg.xml"
#define PHYCFG_FILE_VERSION         "20.04"

#define PHYCFG_MAX_STRING_LENGTH        ( 128 )

#define PHYCFG_ERRORCODE__SUCCESS       ( 0 )
#define PHYCFG_ERRORCODE__FAIL          ( 1 )
#define PHYCFG_ERRORCODE__VER_MISMATCH  ( 2 )

#define DEFAULT_STATS_STREAM_IP          "127.0.0.1"
#define DEFAULT_STATS_STREAM_PORT        2000

#define MAX_NUM_RADIO_CFG               ( 5 )
#define IQ_LOG_FILE_DL_IFFT_IN          ( 1<<0 )
#define IQ_LOG_FILE_DL_IFFT_OUT         ( 1<<1 )
#define IQ_LOG_FILE_UL_FFT_OUT          ( 1<<2 )
#define IQ_LOG_FILE_UL_IFFT_OUT_1200    ( 1<<3 )
#define IQ_LOG_FILE_UL_FFTALAW          ( 1<<4 )
#define IQ_LOG_FILE_UL_FFT_OUT_1        ( 1<<5 )
#define IQ_LOG_FILE_UL_FFT_OUT_64       ( 1<<6 )
#define IQ_LOG_FILE_UL_FFT_OUT_EXP_AGC  ( 1<<7 )
#define IQ_LOG_FILE_UL_PRACH_IN         ( 1<<8 )

#define MAX_PHY_PER_PORT 6

typedef struct tRadioConfigXml
{
    char dpdk_pcie_eth_dev[512];
    char riu_mac_str[512];
    uint32_t dpdkRxCore;
    uint32_t dpdkTxCore;
    uint32_t bandwidth;
    uint32_t nTxAnt;
    uint32_t nRxAnt;
    uint32_t ulAgc;
    uint32_t numCell;
    uint32_t phyInsId[MAX_PHY_PER_PORT];
} RADIO_CFG_XML, *PRADIO_CFG_XML;

typedef struct tThreadInfo
{
    uint64_t coreMask;
    int corePriority;
    int corePolicy;
} THREAD_INFO, *PTHREAD_INFO;

typedef struct tPhyCfgVars
{
    char phy_cfg_filename[512];

    //-----------------------------------
    // General
    //-----------------------------------
    uint32_t appRunMode;

    //-----------------------------------
    // API Delivery
    //-----------------------------------
    uint32_t successiveNoApi;
    uint64_t nWlsMemorySize;
    char wls_dev_filename[512];

    //-----------------------------------
    // PHY Logs
    //-----------------------------------
    uint32_t dlIqLog;
    uint32_t ulIqLog;
    uint32_t iqLogDumpToFile;
    uint32_t phyMlog;
    uint32_t PhyStats;

#ifdef PHASE2_3_VALIDATION
    uint32_t dlIqLogStartTime;       // The absolute time point (in the unit 1ms) when starting to log the DL iq data.
    uint32_t dlIqLogStartSfn;        // The sub-frame number (0 ~ 10239) when starting to log the DL iq data.
    uint32_t dlIqLogTotalSfn;        // The total sub-frame number that will be stored into the memmory.
    uint16_t dlIqLogType;            // 0: Normal; 256: Phase2 validation IQ log
    uint16_t dlIqLogFlag;            // Bit0~7:  To indicate to log the DL iq data of Car0~7 or not (1 or 0)
                                // Bit8~15: To indicate to log the DL iq data of Car0~7 cyclely or once (1 or 0)
#endif

    //-----------------------------------
    // Power Related
    //-----------------------------------
    uint32_t powerSaveEna;    /*< enable features to improve power consumption */
    uint32_t powerMeasureEna; /*< enable power measurement */

    //-----------------------------------
    // DPDK Settings
    //-----------------------------------
    uint32_t dpdkMemorySize;
    uint32_t dpdkIoVaMode;
    uint32_t dpdkBasebandFecMode;
    char     dpdkBasebandDevice[512];

    //-----------------------------------
    // Radio Settings
    //-----------------------------------
    uint32_t radioEna;
    uint32_t ferryBridgeMode;
    uint32_t ferryBridgeEthPort;
    uint32_t ferryBridgeSyncPorts;
    uint32_t ferryBridgeOptCableLoopback;

    RADIO_CFG_XML radioCfgXml[MAX_NUM_RADIO_CFG];

    uint32_t radioPort0;
    uint32_t radioPort1;
    uint32_t radioPort2;
    uint32_t radioPort3;

    //-----------------------------------
    // PHY Globals
    //-----------------------------------
    // PDSCH
    uint32_t FecEncBypass;
    uint32_t FecEncSplit;
#ifdef NR5G
    uint32_t PdschSymbolSplit;
    uint32_t PdschDlWeightSplit;
#endif

    // PUSCH
    uint32_t taFiltEnable;
    uint32_t ircEnable;
    uint32_t mmseDisable;
    uint32_t FecDecEarlyTermDisable;
    uint32_t FecDecBypass;
    uint32_t FecDecSplit;
#ifndef NR5G
    uint32_t FecDecNumHalfIter;
#else
    uint32_t FecDecNumIter;
    uint32_t llrOutDecimalDigit;
    int32_t IrcEnableThreshold;
    int16_t CEInterpMethod;
#endif

    // PUCCH
    uint32_t pucchFormat2DetectThreshold;

    // PRACH
    uint32_t prachDetectThreshold;

#ifdef NR5G
    // PUSCH
    uint32_t PuschChanEstSplit;
    uint32_t PuschMmseSplit;
    uint32_t PuschLlrRxSplit;
    uint32_t PuschUlWeightSplit;
    // PUCCH
    uint32_t PucchSplit;
    // SRS
    uint32_t SrsCeSplit;
#endif

#ifndef NR5G
    // COMMON
    uint32_t TimerModeFreqDomain;
#endif

    //-----------------------------------
    // MLOG
    //-----------------------------------
    uint32_t mlogSubframes;
    uint32_t mlogCores;
    uint32_t mlogSize;

    THREAD_INFO systemThread;
    THREAD_INFO timerThread;
    THREAD_INFO xRANThread;
    THREAD_INFO xRANWorker;

    THREAD_INFO dpdkMasterThread;

#ifdef NR5G
    THREAD_INFO FpgaDriverCpuInfo;
    THREAD_INFO FrontHaulCpuInfo;
#ifdef INSNODE_U50FH // bskim; using Terasic FEC fpga & Xilinx U50 FH fpga
    THREAD_INFO FrontHaulIfCpuInfo;
#ifdef INSNODE_U50FH_INTERFACE /* //skpark */
    THREAD_INFO FrontHaulWCpuInfo;
    THREAD_INFO FrontHaulRCpuInfo;
#ifdef BBDEV_FEC_ACCL_NR5G //skpark : 0504_BUGFIX
    THREAD_INFO FecRestartCpuInfo;    
#endif
#ifdef INSNODE_U50FH_INTERFACE_IQ_DUMP
    THREAD_INFO IQDumpCpuInfo;
#endif
#endif // of INSNODE_U50FH_INTERFACE
#endif // of INSNODE_U50FH
#endif

    //-----------------------------------
    // BBU Pool Section
    //-----------------------------------
    uint32_t bbuPoolSleepEnable;
    uint32_t bbuPoolThreadCorePriority;
    uint32_t bbuPoolThreadCorePolicy;
    uint64_t bbuPoolThreadDefault_0_63;
    uint64_t bbuPoolThreadDefault_64_127;

#ifdef NR5G
    uint64_t bbuPoolThreadSrs_0_63;
    uint64_t bbuPoolThreadSrs_64_127;
    uint64_t bbuPoolThreadDlbeam_0_63;
    uint64_t bbuPoolThreadDlbeam_64_127;
    uint64_t bbuPoolThreadUrllc;
#endif

    //-----------------------------------
    // FPGA Section
    //-----------------------------------
#ifdef NR5G
    uint32_t FrontHaulTimeAdvance;
    uint32_t nEthPorts;
    uint32_t nPhaseCompFlag;

    uint32_t nFecFpgaVersionMu3;
    uint32_t nFecFpgaVersionMu0_1;
    uint32_t nFhFpgaVersionMu3;
    uint32_t nFhFpgaVersionMu0_1;
#endif

    //-----------------------------------
    // xRAN Section
    //-----------------------------------
#ifdef NR5G
    char     dpdkXranDeviceCp[512];
    char     dpdkXranDeviceUp[512];
    char     du_mac_str[512];
    char     ru_mac_str[512];

    uint16_t nPrbElemDl;
    char     prb_elm_str_dl[8][512];
    uint16_t nPrbElemUl;
    char     prb_elm_str_ul[8][512];
    uint8_t  du_addr_bytes[16][6];
    uint8_t  ru_addr_bytes[16][6];

    uint16_t Tadv_cp_dl;
    uint16_t T2a_min_cp_dl;
    uint16_t T2a_max_cp_dl;
    uint16_t T2a_min_cp_ul;
    uint16_t T2a_max_cp_ul;
    uint16_t T2a_min_up;
    uint16_t T2a_max_up;
    uint16_t Ta3_min;
    uint16_t Ta3_max;
    uint16_t T1a_min_cp_dl;
    uint16_t T1a_max_cp_dl;
    uint16_t T1a_min_cp_ul;
    uint16_t T1a_max_cp_ul;
    uint16_t T1a_min_up;
    uint16_t T1a_max_up;
    uint16_t Ta4_min;
    uint16_t Ta4_max;
    uint32_t mtu;
    uint32_t xranCategory;
    uint32_t xranCompMethod;
    uint32_t xranIoPmdSleep;

    uint8_t enableCP;    /**<  enable C-plane */
    uint8_t cp_vlan_tag; /**<  C-plane vlan tag */
    uint8_t up_vlan_tag; /**<  U-plane vlan tag */
    uint8_t DynamicSectionEna;  /**<  enable dynamic section allocation for C-Plane */
    uint32_t xRanNumDLPRBs;  /**<  Total Number of DL PRBs per symbol (starting from RB 0) that is transmitted (used for testing. If 0, then value is used from PHY_CONFIG_API */
    uint32_t xRanNumULPRBs;  /**<  Total Number of UL PRBs per symbol (starting from RB 0) that is received (used for testing. If 0, then value is used from PHY_CONFIG_API */
    int32_t Gps_Alpha;
    int32_t Gps_Beta;
#endif

#ifdef INSNODE_U50FH_INTERFACE /* //skpark */
    //-----------------------------------
    // INSNODE CUSTOM Param
    //-----------------------------------
    uint32_t FrontHaulRUConfig;
    uint32_t FrontHaulTDDConfig;
    uint32_t FrontHaulDelayConfig;
    uint32_t FrontHaulTimingAutoConfig;    
    uint32_t FrontHaulCpri10msConfig;
    uint32_t FrontHaulTxOffsetConfig;
    uint32_t FrontHaulRxOffsetConfig;
    uint32_t FrontHaulTxGainConfig;
    uint32_t FrontHaulRxGainConfig;
    
    uint32_t FlexRanLogLevel;
    
    uint32_t FronthHaulCpriCheckTime; /* sec */
    uint32_t FronthHaulCpriCheckNum;
    
#ifdef INSNODE_U50FH_INTERFACE_GPS
    uint32_t GPSEnable;
    uint32_t GPSOffset;
#endif
#if defined (INSNODE_U50FH_INTERFACE_MULTI_CELL) || defined (INSNODE_U50FH_INTERFACE_COPY_CELL)
    uint32_t FrontHaulRUOnOffConfig;
    uint32_t FrontHaulShiftValue;
#endif
#ifdef INSNODE_U50FH_INTERFACE_MULTI_CELL
    uint32_t FrontHaulCellConfig;
#endif
#ifdef INSNODE_U50FH_INTERFACE_IQ_DUMP
    uint32_t nIQDumpMode;
#endif
#endif


    //-----------------------------------
    // Stream Stats
    //-----------------------------------
#ifdef NR5G
    uint32_t nStreamStats;
    char nStreamIp[256];
    uint32_t nStreamPort;
#endif
} PHYCFG_VARS, *PPHYCFG_VARS;

PPHYCFG_VARS phycfg_get_ctx(void);
void    phycfg_set_cfg_filename(int argc, char *argv[]);
uint32_t     phycfg_init_from_xml(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _PHYCFG_H_ */


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
 * @brief This file consists of the Phy Data Interface (phydi) Functions to initialize and
 *        destroy Timer / Radio / Other interfaces which provide TTI to the PHY Application
 * @file phydi.h
 * @ingroup group_source_auxlib_phydi
 * @author Intel Corporation
 **/


#ifndef _PHY_DI_H_
#define _PHY_DI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common_typedef.h"
#include "common_phy_def.h"

#include <stdint.h>
#ifdef NR5G
#include "gnb_l1_l2_api.h"
#endif


#define PHY_STATE_INIT              0
#define PHY_STATE_START             1
#define PHY_STATE_STOP              2
#define PHY_STATE_SHUTDOWN          3
#define PHY_STATE_STOPPED           4
#define PHY_STATE_FORCE_STOP        5

#define PHY_API_STATE_START         1
#define PHY_API_STATE_STOP          2


#define PHY_DI_STOP_MSG             (10)
#define PHY_DI_SHUTDOWN_MSG         (11)

#define PHY_DI_EXTRA_TTI            (5)

#ifndef NR5G
#define PHY_DI_MAX_IQ_LOG_SIZE_PER_TTI_PER_ANT          (30720*2)               // Space required for Logging IQ samples for 20 MHz 1 Ant in samples

#ifdef PHASE2_3_VALIDATION
#define PHY_DI_IQ_LOG_EXP                               (7)
#else
#define PHY_DI_IQ_LOG_EXP                               (6)
#endif

#define PHY_DI_DL_IQ_LOG_NUM_TTI                        (1<<PHY_DI_IQ_LOG_EXP)
#define PHY_DI_UL_IQ_LOG_NUM_TTI                        (1<<PHY_DI_IQ_LOG_EXP)

#define PHY_DI_DL_IQ_LOG_SIZE                           (PHY_DI_MAX_IQ_LOG_SIZE_PER_TTI_PER_ANT * PHY_DI_DL_IQ_LOG_NUM_TTI)
#define PHY_DI_DL_IQ_LOG_SIZE_TIME_DOM                  (PHY_DI_MAX_IQ_LOG_SIZE_PER_TTI_PER_ANT_TIME_DOM * PHY_DI_DL_IQ_LOG_NUM_TTI)
#define PHY_DI_UL_IQ_LOG_SIZE                           (PHY_DI_MAX_IQ_LOG_SIZE_PER_TTI_PER_ANT * PHY_DI_UL_IQ_LOG_NUM_TTI)
#define PHY_DI_UL_IQ_LOG_SIZE_TIME_DOM                  (PHY_DI_MAX_IQ_LOG_SIZE_PER_TTI_PER_ANT_TIME_DOM * PHY_DI_UL_IQ_LOG_NUM_TTI)

#else   /* #ifndef NR5G */

#define PHY_DI_MAX_IQ_LOG_SIZE_PER_TTI_PER_ANT_TIME_DOM (61440*2)   // 4096 Point FFT x 15 Symbols

#define PHY_DI_DL_IQ_LOG_NUM_TTI_TIMER_MODE (64)   // This number is in ms. This will be multiplied by numeorlogy factor based on phyinit parameter
#define PHY_DI_UL_IQ_LOG_NUM_TTI_TIMER_MODE (64)   // This number is in ms. This will be multiplied by numeorlogy factor based on phyinit parameter

#define PHY_DI_DL_IQ_LOG_NUM_TTI_RADIO_MODE (200)   // This number is in ms. This will be multiplied by numeorlogy factor based on phyinit parameter
#define PHY_DI_UL_IQ_LOG_NUM_TTI_RADIO_MODE (200)   // This number is in ms. This will be multiplied by numeorlogy factor based on phyinit parameter

#define PHY_DI_DL_IQ_LOG_NUM_TTI_MIN (10)   // This number is in ms. This will be multiplied by numeorlogy factor based on phyinit parameter. This is used for tests with more than normal number of slots
#define PHY_DI_UL_IQ_LOG_NUM_TTI_MIN (10)   // This number is in ms. This will be multiplied by numeorlogy factor based on phyinit parameter. This is used for tests with more than normal number of slots

#endif  /* #ifndef NR5G */


#ifdef NR5G
typedef struct tPhyDiMlogStats
{
    uint32_t cnt;
    uint32_t max;
    uint32_t min;
    float avg;
    uint32_t histLow;
    uint32_t histHigh;
} PHY_DI_MLOG_STATS, *PPHY_DI_MLOG_STATS;

typedef struct tPhyDiMlogTaskId
{
    uint32_t taskId;
    uint32_t nType;
    char taskName[128];
    uint32_t cnt[MAX_PHY_INSTANCES];
    float min[MAX_PHY_INSTANCES];
    float avg[MAX_PHY_INSTANCES];
    float max[MAX_PHY_INSTANCES];
    float histLow[MAX_PHY_INSTANCES];
    float histHigh[MAX_PHY_INSTANCES];
} PHY_DI_MLOG_TASKID, *PPHY_DI_MLOG_TASKID;

enum phyDiType
{
    PHYDI_TYPE_DL_PDSCH = 0,
    PHYDI_TYPE_DL_PDSCH_FEC,
    PHYDI_TYPE_DL_CONTROL,
    PHYDI_TYPE_UL_PUSCH,
    PHYDI_TYPE_UL_PUSCH_FEC,
    PHYDI_TYPE_UL_PUCCH,
    PHYDI_TYPE_UL_PRACH,
    PHYDI_TYPE_UL_SRS,
    PHYDI_TYPE_DL_BEAM,
    PHYDI_TYPE_UL_BEAM,
    PHYDI_TYPE_DL_API,
    PHYDI_TYPE_UL_API,
    PHYDI_TYPE_CLEANUP_DL,
    PHYDI_TYPE_CLEANUP_UL,
    PHYDI_TYPE_MAX
};
#endif

typedef struct tPhyDiCtx
{
#ifndef NR5G
    char ulIqFileName[MAX_PHY_INSTANCES][MAX_NUM_ANT][512];

    int16_t *pBsInSamp[MAX_PHY_INSTANCES][MAX_NUM_ANT];
    int16_t *pBsInSampTimeDom[MAX_PHY_INSTANCES][MAX_NUM_ANT];
    int16_t *pBsInSampPrach[MAX_PHY_INSTANCES][MAX_NUM_ANT];
    int16_t *pBsOutSamp[MAX_PHY_INSTANCES][MAX_NUM_ANT];
    int16_t *pBsOutSampTimeDom[MAX_PHY_INSTANCES][MAX_NUM_ANT];

    uint32_t nDlCnt[MAX_PHY_INSTANCES];
    uint32_t nUlCnt[MAX_PHY_INSTANCES];
    uint32_t nPrachCnt[MAX_PHY_INSTANCES];

    float *pBsPrachCorrIn[MAX_PHY_INSTANCES];

    float *pBsFftOut[MAX_PHY_INSTANCES][MAX_NUM_ANT];   // UL
    int16_t *pBsFftOutFix[MAX_PHY_INSTANCES][MAX_NUM_ANT];   // UL
    int16_t *pBsIfftIn[MAX_PHY_INSTANCES];   // DL

    uint32_t nFft[MAX_PHY_INSTANCES];
    uint32_t nTxAnt[MAX_PHY_INSTANCES];
    uint32_t dlIqBufSubfrSize[MAX_PHY_INSTANCES];
    uint32_t numDlSubfr[MAX_PHY_INSTANCES];
    uint32_t nDlSfCnt[MAX_PHY_INSTANCES];

    uint32_t nRxAnt[MAX_PHY_INSTANCES];
    uint32_t ulIqBufSubfrSize[MAX_PHY_INSTANCES];
    uint32_t numUlSubfr[MAX_PHY_INSTANCES];
    uint32_t numUlSubfrFile[MAX_PHY_INSTANCES];
    uint32_t ulPrachIqBufSize[MAX_PHY_INSTANCES];
    uint32_t ulPrachIqBufSizeRead[MAX_PHY_INSTANCES];

    uint32_t PhyState[MAX_PHY_INSTANCES];
    uint32_t PhyApiState;
    uint32_t PhyStateCount;
    uint32_t PhyStartMode;
    uint32_t PhyStartPeriod;
    int32_t  PhyStartCount;      // Number of SFNs to run. if -1, keep running
    uint32_t PhyStartCurrCount;

    uint32_t dlScheduled;

    uint32_t frb_nPorts;
    uint32_t frb_offload;

    /* PRACH init*/
    int16_t prachConfigurationIndex;
    int16_t prachFrequencyOffset;
    int16_t prachHighSpeedFlag;
    int16_t prachCyclicShiftConfig;
    int16_t prachRootSequenceIndex;
    int16_t prachSubcSpacing;

    /* TDD configuration */
    uint8_t frtypeDuplexmode;
    uint8_t ulDlConfig;
    uint8_t specialSubframeConfig;

#else

    char ulIqFileName[MAX_PHY_INSTANCES][MAX_UL_VIRTUAL_PORTS][512];

    int16_t *pBsInSamp[MAX_PHY_INSTANCES][MAX_UL_VIRTUAL_PORTS];
    int16_t *pBsInSampTimeDom[MAX_PHY_INSTANCES][MAX_UL_VIRTUAL_PORTS];
    int16_t *pBsInSampPrach[MAX_PHY_INSTANCES][MAX_UL_VIRTUAL_PORTS];
    int16_t *pBsInSampSrs[MAX_PHY_INSTANCES][MAX_UL_ANTENNA];
    int16_t *pBsOutSamp[MAX_PHY_INSTANCES][MAX_DL_PORTS];
    int16_t *pBsOutSampTimeDom[MAX_PHY_INSTANCES][MAX_DL_PORTS];
    int16_t *pBsOutSampBeamWeightsDL[MAX_PHY_INSTANCES][MAX_DL_ANTENNA][MAX_DL_PORTS];
    int16_t *pBsOutSampBeamWeightsUL[MAX_PHY_INSTANCES][MAX_UL_VIRTUAL_PORTS][MAX_UL_ANTENNA];

    uint32_t nDlCnt[MAX_PHY_INSTANCES];
    uint32_t nUlCnt[MAX_PHY_INSTANCES];
    uint32_t nPrachCnt[MAX_PHY_INSTANCES];
    uint32_t nSrsCnt[MAX_PHY_INSTANCES];

    uint32_t nFft[MAX_PHY_INSTANCES];
    uint32_t nNrOfTxAnt[MAX_PHY_INSTANCES];
    uint32_t nNrOfDLPorts[MAX_PHY_INSTANCES];
    uint32_t dlIqBufSubfrSize[MAX_PHY_INSTANCES];
    uint32_t numDlSubfr[MAX_PHY_INSTANCES];
    uint32_t nDlSfCnt[MAX_PHY_INSTANCES];

    uint32_t nUrllcCapable[MAX_PHY_INSTANCES];
    uint32_t nNrOfRxAnt[MAX_PHY_INSTANCES];
    uint32_t nNrOfULPorts[MAX_PHY_INSTANCES];
    uint32_t ulIqBufSubfrSize[MAX_PHY_INSTANCES];
    uint32_t numUlSubfr[MAX_PHY_INSTANCES];
    uint32_t numUlSubfrFile[MAX_PHY_INSTANCES];
    uint32_t numUlSrsSubfrFile[MAX_PHY_INSTANCES];
    uint32_t ulPrachIqBufSize[MAX_PHY_INSTANCES];
    uint32_t ulPrachIqBufSizeRead[MAX_PHY_INSTANCES];

    uint32_t PhyState[MAX_PHY_INSTANCES];
    uint32_t PhyApiState;
    uint32_t PhyStateCount;
    uint32_t PhyStartMode;
    uint32_t PhyStartPeriod;
    int32_t  PhyStartCount;      // Number of SFNs to run. if -1, keep running
    uint32_t PhyStartCurrCount;

    uint32_t dlScheduled;

    uint32_t frb_nPorts;
    uint32_t frb_offload;

    /* PRACH init*/
    int16_t prachConfigurationIndex;
    int16_t prachFrequencyOffset;
    int16_t prachHighSpeedFlag;
    int16_t prachCyclicShiftConfig;
    int16_t prachRootSequenceIndex;
    int16_t prachSubcSpacing;

    /* 5G NR */
    /*absolute Frequency of  pointA in KHz
     Value: 450000->52600000*/
    uint32_t     nDLAbsFrePointA[MAX_PHY_INSTANCES];
    uint32_t     nULAbsFrePointA[MAX_PHY_INSTANCES];

    /* ifft/fft size for DL and UL
    Value: 512, 1024, 2048, 4096 */
    uint16_t     nDLFftSize[MAX_PHY_INSTANCES];
    uint16_t     nULFftSize[MAX_PHY_INSTANCES];

    uint16_t     nDLRBs[MAX_PHY_INSTANCES];
    uint16_t     nULRBs[MAX_PHY_INSTANCES];

    /*Frame Duplex type:  0 -> FDD, 1 -> TDD*/
    uint8_t      nFrameDuplexType[MAX_PHY_INSTANCES];
    /*Numerology, determine sub carrier spacing, Value: 0->4 */
    /* 0: 15khz,  1: 30khz,  2: 60khz
       3: 120khz, 4: 240khz */
    uint8_t      nNumerology[MAX_PHY_INSTANCES];
    /* TDD Period - If nFrameDuplexType = TDD(1), then this config defines the TDD periodicity */
    uint8_t      nTddPeriod[MAX_PHY_INSTANCES];

    /**** word 15 - 335 *****/
    /* TDD Slot configuration - If nFrameDuplexType = TDD(1), then this config defines the slot config type for each slot.*/
    /* The number of slots need to be equal to nTddPeriod */
    SLOTCONFIGStruct sSlotConfig[MAX_PHY_INSTANCES][MAX_TDD_PERIODICITY];

    uint32_t TimerModeFreqDomain;
    uint32_t PhaseCompensationEnable;

    uint32_t startFrameNum[MAX_PHY_INSTANCES];
    uint32_t startSlotNum[MAX_PHY_INSTANCES];
    uint32_t startSymNum[MAX_PHY_INSTANCES];

    /* Ferry Bridge Radio Mode parameters */
    int32_t nMainHandlerSfn;
    int32_t nMainHandlerCnt;
#endif
} PHYDI_CTX, *PPHYDI_CTX;

typedef struct tPhyDiFileInfo
{
    uint32_t carrNum;
    uint32_t numSubframes;
#ifdef NR5G
    uint32_t nIsRadioMode;
    uint32_t TimerModeFreqDomain;
    uint32_t PhaseCompensationEnable;
    uint32_t startFrameNum;
    uint32_t startSlotNum;
    uint32_t startSymNum;
    char filename_in_ul_iq[MAX_UL_VIRTUAL_PORTS][576];
    char filename_in_prach_iq[MAX_UL_VIRTUAL_PORTS][576];
    char filename_in_srs_iq[MAX_UL_ANTENNA][576];
    char filename_out_dl_iq[576];
    char filename_out_dl_beam[MAX_DL_PORTS][576];
    char filename_out_ul_beam[MAX_UL_VIRTUAL_PORTS][576];
#else
    uint32_t startSamps;
    char filename[MAX_NUM_ANT][576];
    char filename_prach[MAX_NUM_ANT][576];
#endif
    uint8_t buffer[4096];
} PHY_DI_FILE_INFO, *PPHY_DI_FILE_INFO;

PPHYDI_CTX phydi_get_ctx(void);
uint32_t phydi_copy_iq(uint8_t * *pIn, uint32_t nAnt, uint32_t nCarr, uint32_t sfn, uint32_t slotAntNum, uint32_t direction);

#ifdef PHASE2_3_VALIDATION
uint32_t phydi_copy_iq_phase2(uint8_t * *pIn, uint32_t nAnt, uint32_t nCarr, uint32_t sfn, uint32_t slotAntNum, uint32_t direction);
uint32_t phydi_copy_iq_freq_domain_phase2(uint32_t nCtxNum, uint32_t phyInstance, uint32_t size, uint32_t nAnt, uint32_t nCarr, uint32_t sfn, uint32_t direction);
#endif

uint32_t phydi_copy_iq_freq_domain(uint32_t nCtxNum, uint32_t phyInstance, uint32_t size, uint32_t nAnt, uint32_t nCarr, uint32_t sfn, uint32_t direction);
uint32_t phydi_copy_iq_frb_bypass_mode(uint8_t * *pInOut, uint32_t nAnt, uint32_t nCarr, uint32_t direction);
uint32_t phydi_copy_iq_frb_slot_bypass_mode(uint8_t * *pInOut, uint32_t nAnt, uint32_t nSlot, uint32_t nCarr, uint32_t direction);
uint32_t phydi_copy_prach_out_frb_lte_mode(int16_t * pInOut, uint32_t nAnt, uint32_t nCarr, uint32_t direction);
uint32_t phydi_copy_prach_out_roe_freq_mode(int16_t * pInOut, uint32_t nAnt, uint32_t nCarr, uint32_t direction);
uint32_t phydi_copy_ifft_in_frb_lte_mode(int16_t *pInOut[], uint32_t nAnt, uint32_t nCarr, uint32_t direction);
uint32_t phydi_copy_ifft_in_roe_freq_mode(int16_t *pInOut[], uint32_t nAnt, uint32_t nCarr, uint32_t direction);
uint32_t phydi_copy_iq_fft_out_frb_lte_mode(uint32_t nCtxNum, uint32_t nAnt, uint32_t nCarr, uint32_t direction);
uint32_t phydi_copy_iq_fft_out_roe_freq_mode(uint32_t nCtxNum, uint32_t nAnt, uint32_t nCarr, uint32_t direction);

uint32_t phydi_destroy_buffers(uint32_t phyInstance);

#ifdef NR5G
uint32_t phydi_init(CONFIGREQUESTStruct *pInitParam, uint32_t nDLRes, uint32_t nULRes, uint32_t phyInstance);
uint32_t phydi_iq_logging(void *pCfg, void *psTx, void *psRx, uint32_t phyInstance, uint32_t direction);
uint32_t phydi_prach_iq_logging(void *pCfg, void *psRx, uint32_t phyInstance);
uint32_t phydi_srs_iq_logging(void *pCfg, void *psRx, uint32_t phyInstance);
uint32_t phydi_allocate_buffers(uint32_t isRadioMode, uint32_t phyInstance, uint32_t nNumerology, uint32_t numTxAnt, uint32_t numRxAnt, uint32_t nREsPerSlotDL, uint32_t nREsPerSlotUL, uint32_t nDLFftSize, uint32_t nULFftSize);
uint32_t phydi_allocate_massive_mimo_buffers(uint32_t isRadioMode, uint32_t phyInstance, uint32_t nNumerology, uint32_t numTxPorts, uint32_t numRxPorts, uint32_t numTxAnt, uint32_t numRxAnt, uint32_t nREsPerSlotDL, uint32_t nREsPerSlotUL, uint32_t nREsPerSlotSrs);
uint32_t phydi_init_mlog_stats(uint64_t nTscFreq);
#else
uint32_t phydi_init(uint32_t nTxAnt, uint32_t nRxAnt, uint32_t nFft, uint32_t phyInstance,
uint8_t frtypeDuplexmode, uint8_t ulDlConfig, uint8_t specialSubframeConfig,
int16_t prachConfigurationIndex, int16_t prachFrequencyOffset, int16_t prachHighSpeedFlag,
int16_t prachCyclicShiftConfig, int16_t prachRootSequenceIndex);
uint32_t phydi_allocate_buffers(uint32_t phyInstance, uint32_t nNumerology, uint32_t numTxAnt, uint32_t numRxAnt);

#endif

uint32_t phydi_read_write_iq_samples(uint32_t direction, PPHY_DI_FILE_INFO pFileInfo);
uint32_t phydi_set_iq_buf(uint32_t nCtxNum, uint32_t phyInstance, uint32_t nFft, uint32_t nAnt, uint32_t nAntId, uint32_t sfn, uint32_t direction);

uint32_t phydi_start(uint32_t phyInstance, uint32_t mode, uint32_t count, uint32_t period);
uint32_t phydi_stop(uint32_t phyInstance, uint32_t sendStop);
uint32_t phydi_shutdown(uint32_t phyInstance, uint16_t bSendMsg, uint32_t testFileName);
void phydi_stop_handler(void);
uint32_t phydi_query_state(uint32_t phyInstance);
uint32_t phydi_set_state(uint32_t phyInstance, uint32_t state);
void phydi_rx_handler(void);
uint32_t phydi_query_api_state(void);
uint32_t phydi_set_api_state(uint32_t state);

#ifdef INSNODE_U50FH_INTERFACE
void phydi_set_loglevel_handler(uint32_t logLevel);
#ifdef INSNODE_U50FH_INTERFACE_IQ_DUMP
void phydi_dump_handler(uint32_t nDumpType, uint32_t nDumpSlotNum);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif                          // _PHY_DI_H_


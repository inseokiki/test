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
 * @brief This file has the PHY #defines and enums definitions used across the code
 * @file common_phy_def.h
 * @ingroup group_source_nr5g_common
 * @author Intel Corporation
**/

#ifndef _COMMON_PHYDEF_H_
#define _COMMON_PHYDEF_H_

#include "common_ran_parameters.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define PHY_AVX512
#define AVX_BUFFER_BYTE_ALIGN           ( 64 )
#define AVX_BUFFER_FLP32_ALIGN          ( AVX_BUFFER_BYTE_ALIGN/4 )

#define RUP512B(x) (((x)+511)&(~511))
#define RUP256B(x) (((x)+255)&(~255))
#define RUP128B(x) (((x)+127)&(~127))
#define RUP64B(x) (((x)+63)&(~63))
#define RUP32B(x) (((x)+31)&(~31))
#define RUP16B(x) (((x)+15)&(~15))
#define RUP8B(x)  (((x)+7)&(~7))
#define RUP4B(x)  (((x)+3)&(~3))
#define RUP2B(x)  (((x)+1)&(~1))

#define CRAN_ALIGNTO        64
#define CRAN_ALIGN(length) (((length)+CRAN_ALIGNTO-1)&~(CRAN_ALIGNTO-1))

#ifndef _DIVIDE_FLOOR_
#define _DIVIDE_FLOOR_
#define divide_floor(x,y)               ( (x)/(y) )
#endif /* _DIVIDE_FLOOR_ */

#ifndef _DIVIDE_CEIL_
#define _DIVIDE_CEIL_
#define divide_ceil(x,y)                ( ((int32_t)((x)+(y)-1))/(y) )
#endif /* _DIVIDE_CEIL_ */

#define Nr5gBsConstructSfn(frame, slotNum, numSlotPerSfn)   ((frame * numSlotPerSfn) + slotNum)
#define Nr5gBsConstructSfnSym(frame, slotNum, symNum, numSlotPerSfn)   ((((frame * numSlotPerSfn) + slotNum)*NUM_OF_SYMBOL_PER_SLOT)+symNum)
#define Nr5gBsIncrementSlotNum(slotNum, numSlotPerSfn)      (slotNum >= (numSlotPerSfn - 1) ? 0 : slotNum+1)
#define Nr5gBsIncrementFrameNum(frameNum, numSlotPerSfn)    ((frameNum >= (1024 - 1)) ? 0 : frameNum+1)
#define Nr5gBsIncrementSfn(sfn, numSlotPerSfn)              ((sfn >= ((numSlotPerSfn * 1024) - 1)) ? 0 : sfn+1)
#define Nr5gBsDecrementSfn(sfn, numSlotPerSfn)              ((sfn == 0) ? ((numSlotPerSfn * 1024) - 1) : sfn-1)
#define Nr5gBsGetFrameNum(sfn, numSlotPerSfn)               ((uint32_t) ((sfn / numSlotPerSfn) % 1024))
#define Nr5gBsGetSlotNum(sfn, numSlotPerSfn)                (sfn % numSlotPerSfn)
#define Nr5gBsDiffSfn(prevSfn, currSfn, numSlotPerSfn)      ((prevSfn > currSfn) ? ((currSfn + (numSlotPerSfn * 1024)) - prevSfn) : (currSfn - prevSfn))
#define Nr5gBsGetTicksDiff(CurrTick, LastTick)              ((CurrTick >= LastTick) ? (CurrTick - LastTick) : (0xFFFFFFFF - LastTick + CurrTick))

#define Nr5gBsCheckUeCategory(category)                     (((category < 1)||(category > MAX_NUM_UE_CATEGORY)) ? MAX_NUM_UE_CATEGORY : category)
#define Nr5gBsCheckNumTxLayers(numTxLayers)                 (((numTxLayers < 1)||(numTxLayers > 8)) ? 4 : numTxLayers)


#define CalcMcs(Itbs, Qm)                                (Itbs + ((Qm >> 1) - 1))
#define Nr5gBsIsCqiPmi(a)                                ((a > 2) ? 1 : 0)

#ifndef likely
#define likely(x)   __builtin_expect((x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect((x), 0)
#endif

enum PhyChBwOptions
{
    PHY_BW_5_0_MHZ = 5,   PHY_BW_10_0_MHZ = 10, PHY_BW_15_0_MHZ = 15, PHY_BW_20_0_MHZ = 20, PHY_BW_25_0_MHZ = 25,
    PHY_BW_30_0_MHZ = 30, PHY_BW_40_0_MHZ = 40, PHY_BW_50_0_MHZ = 50, PHY_BW_60_0_MHZ = 60, PHY_BW_70_0_MHZ = 70,
    PHY_BW_80_0_MHZ = 80, PHY_BW_90_0_MHZ = 90, PHY_BW_100_0_MHZ = 100, PHY_BW_200_0_MHZ = 200, PHY_BW_400_0_MHZ = 400
};

enum PhyNumRbspSlotOptions
{
    PHY_RB_SLOT_6 = 6, PHY_RB_SLOT_15 = 15, PHY_RB_SLOT_25 = 25, PHY_RB_SLOT_50 = 50, PHY_RB_SLOT_66 = 66, PHY_RB_SLOT_75 = 75, PHY_RB_SLOT_100 = 100, PHY_RB_SLOT_275 = 275
};

enum PhysicalChannelType
{
    PHY_DL_PDU_TYPE_DLDCI = 1, PHY_DL_PDU_TYPE_ULDCI, PHY_DL_PDU_TYPE_DLSCH, PHY_DL_PDU_TYPE_PBCH, PHY_DL_PDU_TYPE_CSI_RS,
    PHY_UL_PDU_TYPE_ULSCH, PHY_UL_PDU_TYPE_ULCCH_UCI, PHY_UL_PDU_TYPE_PRACH, PHY_UL_PDU_TYPE_SRS
};

enum PhyModulationType
{
    PHY_BPSK = 1, PHY_QPSK = 2, PHY_QAM16 = 4, PHY_QAM64 = 6, PHY_QAM256 = 8
};

enum PhyCyclicPrefixType
{
    PHY_NORMAL = 0, PHY_EXTENDED
};

enum PhySymbolsPerSlot
{
    SL_THREE = 3, SL_SIX = 6, SL_SEVEN = 7, SL_FOURTEEN = 14
};

enum PhyResourceBandwidth
{
    PHY_PRB_12 = 12, PHY_PRB_24 = 24
};

enum PhyPucchFormatType
{
    PHY_PUCCH_FORMAT0 = 0, PHY_PUCCH_FORMAT1, PHY_PUCCH_FORMAT2, PHY_PUCCH_FORMAT3, PHY_PUCCH_FORMAT4, PHY_PUCCH_LAST
};

enum PhyPucchHarqFeedBackMethod
{
    PHY_FORMAT_1A_1B = 0, PHY_DONT_CARE, PHY_CHANNEL_SELECTION, PHY_UCI_FORMAT_3
};

enum PhyPuschHarqFeedBackMethod
{
    PHY_BUNDLING = 0, PHY_MULTIPLEXING
};

enum PhySrsHopping
{
    PHY_SRS_NEITHER_HOPPING = 0, PHY_SRS_GROUP_HOPPING = 1, PHY_SRS_SEQUENCE_HOPPING = 2
};

enum PhySrsResourceType
{
    PHY_SRS_APERIODIC = 0, PHY_SRS_SEMI_PERSISTENT = 1, PHY_SRS_PERIODIC = 2
};

enum PhyFrameDuplexType
{
    PHY_FDD = 0, PHY_TDD
};

enum PhyDirection
{
    PHY_DL = 0, PHY_UL, PHY_SP
};

enum PhyAntPortCntOptions
{
    PHY_ANT_ONE = 1, PHY_ANT_TWO, PHY_ANT_FOUR = 4, PHY_ANT_EIGHT = 8
};

enum PhyPUSCHCompress
{
    PHY_PUSCH_NORMAL = 0, PHY_PUSCH_COMPRESS = 1
};

typedef enum
{
    RB_SPLIT = 0,
    UE_GROUP_SPLIT = 1,
    LAYER_SPLIT = 2,
    UE_SPLIT = 3
} TaskSplitType;

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _COMMON_PHYDEF_H_ */


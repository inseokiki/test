/*******************************************************************************
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
*******************************************************************************/

/**
* @file common_ran_parameters.h
* @ingroup group_source_nr5g_common
* @brief This header file defines those parameters defined in spec.
**/

#ifndef _COMMON_RAN_PARAMETERS_  /* the macro is file name in capture */
#define _COMMON_RAN_PARAMETERS_

//numerology and frame structure
#define NUM_NUMEROLOGY                  ( 5 ) //the total number of numerology
#define NUM_OF_SYMBOL_PER_SLOT          ( 14 )
#define NUM_OF_SUBFRAME_PER_FRAME       ( 10 )
#define MAX_NUM_FRAMES                  ( 1024 )
#define MAX_NUM_SLOTS                   ( 16 * NUM_OF_SUBFRAME_PER_FRAME )

// physical resource
#define N_SC_PER_PRB 12
#define MAX_DMRS_SYMBOL (4)
#define MAX_SYMB_PER_SL_DMRS 2
#define MAX_DATA_SYMB_PER_SL_UL 13
#define MAX_DATA_SYMB_PER_SL_DL 12
#define MAX_DATA_SYMB_PER_SL MAX_DATA_SYMB_PER_SL_UL
#define MAX_N_FFT_SIZE (4096)
#define MAX_CP (352)
#define MAX_NUM_OF_PRB_IN_FULL_BAND (273)
#define MAX_N_FULLBAND_SC (MAX_NUM_OF_PRB_IN_FULL_BAND * N_SC_PER_PRB)
#define MAX_PUCCH_F2_WIENER_TABLE_LEN (4*12*141*2)
#define MAX_PUCCH_F3_WIENER_TABLE_LEN (12*12*141*2)
#define MAX_UE_HARQ 200
#define MAX_HARQ_ID 16
#define MAX_HARQ_RETRANS 4
#define FPGA_TO_SW_PRACH_RX_BUFFER_LEN   (8192)
#define FH_CONFIG 0x030f0f

#define MAX_NUM_MCS                     ( 32 )      //maximum number of MCS Values assigned [0 .. 31]
#define MAX_CODE_BLOCK_SIZE 8448
#define MAX_RM_OUT_CODE_BLOCK_SIZE_BYTES 4672
#define MAX_DL_RE_GRID_SIZE 183456 // 273*12*14*4

// (max 13 symbols per slot)*(max 273 PRBs in 100MHz)*(12 SCs per PRB)*(max 4 layers)*(max 256 QAM)*(max 1 code rate)
// = 13*273*12*4*8=1362816 bits. According to 38.214 5.1.3.4, the TBS is 1376264 bits.
// = 172096 bytes with 64bytes aligned
#define MAX_UL_TB_SIZE 172096
#define MAX_DL_TB_SIZE 172096
#define PRB_NUM_PER_RBG   2

#define MOD_QPSK 2
#define MOD_16QAM 4
#define MOD_64QAM 6
#define MOD_256QAM 8
#define MAX_BITS_NUM_PER_RE (MOD_256QAM)

// system resource para
#define MAX_PATH_LEN 256

#define MAX_PHY_INSTANCES   ( 12 )
#define MAX_SECTOR_NR       ( 12 )
#define CACHE_ALIGN_FACTOR  ( 2 ) //to compensate the buffer size for the fragmentation in cache line aligned buffer fetch

#define MAX_UE_NUM          ( 16 )
#define MAX_MU_GROUP        MAX_UE_NUM
#define MAX_MU_UE_PER_GROUP ( 4 )

#ifdef INSNODE_U50FH_INTERFACE /* //U50_BOARD //DATA_FRAME */
#ifdef INSNODE_U50FH_INTERFACE_MULTI_CELL
#define FH_MAX_CELL_NUM       ( 3 )
#else
#define FH_MAX_CELL_NUM       ( 1 )
#endif
#define MAX_FH_PORT_NUM       ( 4 )
#endif 

#define LOW_LAYER_EVENT_QUEUE_LENGTH  (40)

#define MAX_MUMIMO_GROUP_NUM          ( 16 )
#define MAX_PAIRED_UE_NUM             ( 8 )

#define MAX_DL_PORTS            ( 16 )
#define MAX_UL_PORTS            ( 2 )

#define MAX_DL_PORTS_PER_USER   ( 4 )
#define MAX_UL_VIRTUAL_PORTS    ( 8 )

#define MAX_DL_ANTENNA          ( 64 )
#define MAX_UL_ANTENNA          ( 64 )

#define MAX_DL_CODEWORD         ( 2 )
#define MAX_UL_CODEWORD         ( 1 )

#define NUM_ANT_MASSIVE_MIMO    ( 16 )       // If numRxAnt of cell is more than this number, then the cell is a massive MIMO cell

#define CheckIsMassiveMimoConfig(nNrOfRxAnt)  ((nNrOfRxAnt >= NUM_ANT_MASSIVE_MIMO) ? 1 : 0)

// DMRS
#define MAX_DMRS_PER_PRB 6
#define N_DMRS_TYPE1_SC_PER_RB 6
#define N_DMRS_TYPE2_SC_PER_RB 4
#define MAX_DMRS_SC_NUM (MAX_NUM_OF_PRB_IN_FULL_BAND*MAX_DMRS_PER_PRB)
#define DMRS_FOR_PDSCH 0
#define DMRS_FOR_PUSCH 1
#define DMRS_TP_SEQ_GROUP 30

//UCI multiplex with PUSCH
#define N_ACK_RVD_BITS (2)
#define N_ACK_MAX_SYM  (3)

//PDCCH
#define MAX_PDCCH_PAYLOAD          32          //256 bit, 32byte
#define MAX_PDCCH_SYMBOL           3
#define NUM_PDCCH_DMRS_PRE_PRB     3
#define NUM_PDCCH_DATA_PRE_PRB     9
#define MAX_PDCCH_DMRS             (MAX_NUM_OF_PRB_IN_FULL_BAND * MAX_DMRS_PER_PRB) //each PRB has 3 DMRS, each has re/im.
#define MAX_PDCCH_AllO_NUM_PER_TTI 32
#define MAX_PDCCH_POLAR_CODE_BYTE  128         //Maximum number of data after Polar Code is 1024bit, 128Byte.
#define NUM_REGS_PER_CCE           6           //A coutrol-channel element consists of Six resource-element groups (REGs)
#define NUM_DATA_BITS_PER_CCE      NUM_REGS_PER_CCE * NUM_PDCCH_DATA_PRE_PRB * 2
#define NUM_DATA_BYTE_PER_CCE      14          //(NUM_DATA_BITS_PER_CCE/8+1)
#define MAX_SINGLE_PDCCH_NUM_CCE   16          //One PDCCH has maximum 16 CCEs
#define MAX_PDCCH_AllOW_NUM_CCE    135         //One symbol has maximum 45 CCEs and maximum 3 symbols.
#define MAX_PDCCH_POLAR_OUT        1024



//PDCCH Cases Parameter
#define PDCCHVectorInitial 1
#define PDCCH_USING_VEC_NUM 0
#define PDCCH_TOTAL_VEC_NUM 1
#define PDCCH_REGRESSION 0
#define PRINT_WHEN_PASS 0
#define QUANTIZATION_ERR_THRE  32
#define CORESET_SPLIT_NUM_TO_32BIT 2

//PDSCH
#define MAX_DL_RBG_BIT_NUM              (5)  // bitmap, For resource allocation type 0.The maximum number is 138 with 275 RBs and RBG size 2.
#define MAX_DL_PER_UE_DMRS_PORT_NUM     (8)
#define MAX_DL_PER_UE_PTRS_PORT_NUM     (2)


// RX link module related configruation
// SW mode: assume received signal is 13 bits
#define RX_DATA_FIXED_POINT (13)
#define CE_NOISE_EST_FACTOR ((RX_DATA_FIXED_POINT-1)*2)
#define MMSE_X_LEFT_SHIFT (RX_DATA_FIXED_POINT)
#define MMSE_LEMMA_SCALING ((RX_DATA_FIXED_POINT)*2)
#define MMSE_NOISE_POWER ((RX_DATA_FIXED_POINT)*2)
#define DE_MOD_RIGHT_SHIFT ((RX_DATA_FIXED_POINT)-5)

// polar encoder and decoder
#define MAX_E 1728
#define MAX_N 1024
#define INFO_TAB_LEN 4736726
#define INFOSORT_TAB_LEN 4736726
#define TAB_INDEX_LEN 52028
#define INTERLEAVER_TAB_LEN 46240
#define NUM_E 100
#define NUM_K 400

// ssb para common
#define SSBNUM_4 4
#define SSBNUM_8 8
#define SSBNUM_64 64
#define BYTE_4   4

//CSI dimension common
#define MAX_CSI_NZP 16
#define MAX_CSI_ZP 16
#define MAX_TRS 32

//PRACH para
#define MAX_PRACH_CONFIG_IDX            ( 255 )
#define MAX_PRACH_ZERO_ZONE_CONFIG      ( 15 )
#define MAX_PRACH_ROOT_SEQ_IDX          ( 837 )
#define MAX_PRACH_ANT_NUM               ( 4 )

//PUCCH para
#define MAX_PUCCH_ANT_NUM               ( 4 )

/* define HARQ process number*/
#define MAX_UL_HARQ_PROCESS (8)
#define MAX_DL_HARQ_PROCESS (8)

#define NUM_IQ_SAMPS_20MHZ              ( 30720 )
#define NUM_IQ_SAMPS_PER_SYM_20MHZ      ( 4096 )
#define DL_IFFT_IN_SCALE_FACTOR         (1.f / 20.f * 32768.f) // experimental value obtained base on matching FlexRAN DL input to FRB
#define DL_IFFT_IN_SCALE_FACTOR_FX      0.2f // experimental value obtained base on matching FlexRAN DL input to FRB

//FEC FPGA buffer
#define FPGA_TO_SW_DECODE_OUT_BUFFER_LEN  (4096*256)  // in byte
#define FPGA_TO_SW_ENCODE_OUT_BUFFER_LEN  (4096*256)  // in byte
#define SW_TO_FPGA_ENCODE_IN_BUFFER_LEN  (4096*256)  // in byte
#define SW_TO_FPGA_DECODE_IN_BUFFER_LEN  (4096*256)  // in byte

#define MAX_UL_CB_NUM 192
#define MAX_DL_CB_NUM 192
#define MAX_DL_CB_NUM_PER_CELL 1024
#define MAX_UL_CB_NUM_PER_CELL 1024

#define DUMP_SLOT_LENGTH 160

/******Processing Latencies***/
#define DL_PROC_ADVANCE_MU0     (1)
#define DL_PROC_ADVANCE_MU1     (2)
#define DL_PROC_ADVANCE_MU3     (2)

#define UL_PROC_ADVANCE_MU0     (0)
#define UL_PROC_ADVANCE_MU1     (0)
#define UL_PROC_ADVANCE_MU3     (0)

#endif /* _COMMON_RAN_PARAMETERS_ */


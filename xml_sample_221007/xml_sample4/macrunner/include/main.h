#ifndef _MAIN_H_
#define _MAIN_H_

/////////////////////////////////////////////////////////////////////////////////

#define MAX_SLOT_PERIOD_COUNT          120
#define MAX_FILEPATH_SIZE              1024
#define MAX_FAPI_MSG_IN_SLOT           10
#define MAX_FAPI_MSG_SIZE              10000
#define MAX_FAPI_TB_SIZE_IN_SLOT       200000
#define MAX_TX_DATA_COUNT_IN_SLOT      20
#define MAX_TX_DATA_FILE_LINE_SIZE     4096

enum
{
    CELL_STATUS_INIT,
    CELL_STATUS_CONFIGURED,
    CELL_STATUS_ACTIVE,
    CELL_STATUS_STOP,
};

typedef struct
{
    char              txConfigFileName[MAX_FILEPATH_SIZE];

    uint8_t           fapiMsgCount;
    uint32_t          fapiMsgSize[MAX_FAPI_MSG_IN_SLOT];
    uint8_t           fapiMsg[MAX_FAPI_MSG_IN_SLOT][MAX_FAPI_MSG_SIZE];

    uint32_t          txDataCount;
    uint32_t          txDataSize[MAX_TX_DATA_COUNT_IN_SLOT];
    uint8_t           txDataBuff[MAX_TX_DATA_COUNT_IN_SLOT][MAX_FAPI_TB_SIZE_IN_SLOT];
} slot_config_t;

typedef struct
{
    TestConfigReq     testConfigReq;

    uint8_t           cellStatus;
    uint32_t          slotCountRemaining;
    FapiConfigInfo    fapiConfigInfo;

    uint32_t          slotConfigCount;
    slot_config_t     slotConfig[MAX_SLOT_PERIOD_COUNT];
} cell_config_t;

typedef struct
{
    char              currentWorkingDir[MAX_FILEPATH_SIZE];
    char              basePath[MAX_FILEPATH_SIZE];

    cell_config_t     cellConfig[MAX_CELL_COUNT];
} mac_runner_config_t;

/////////////////////////////////////////////////////////////////////////////////

extern mac_runner_config_t g_macRunnerConfig;

/////////////////////////////////////////////////////////////////////////////////

#endif    // of _MAIN_H


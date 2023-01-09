#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "common.h"
#include "gSM_l1_l2_api.h"
#include "xml_parser.h"
#include "mac_runner.h"
#include "main.h"

mac_runner_config_t g_macRunnerConfig;

int main( int argc, char *argv[] )
{
    if (argc < 2)
    {
        printf( "Usage: %s xmlFile [SlotCountToRun]\n", argv[0] );
        exit(0);
    }

    if (parseXmlFile(argv[1]) != SUCCESS)
    {
        printf( "%s : xml parsing failed\n", argv[1] );
        return -1;
    }

    macRunner();

    return 0;
}

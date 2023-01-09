#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "common.h"
#include "xml_lib.h"

void printXmlNodes(xmlNode *pStartNode, uint32_t depth)
{
    xmlNode *pNode = NULL;
    uint32_t idx;

    for(pNode = pStartNode; pNode != NULL; pNode = pNode->next)  
    {
        if(pNode->type == XML_ELEMENT_NODE)
        {
            for (idx = 0; idx < depth; idx++)
                printf( " " );

            printf("%s\n", pNode->name );
        }

        printXmlNodes(pNode->children, depth+1);
    }
}

int searchXmlNode( xmlNode *pHeadNode, xmlNode **ppDestNode, char *pNodeName )
{
    xmlNode *pPeerNode;

    if (pHeadNode == NULL)
        return FALSE;

    for (pPeerNode = pHeadNode; pPeerNode != NULL; pPeerNode = pPeerNode->next)
    {
        if (pPeerNode->type == XML_ELEMENT_NODE) 
        {
            if (strcmp((char *) pPeerNode->name, pNodeName ) == 0)
            {
                *ppDestNode = pPeerNode;
                return TRUE;
            }
        }

        if (pPeerNode->children != NULL && searchXmlNode(pPeerNode->children, ppDestNode, pNodeName))
            return TRUE;      
    }

    return FALSE;
}

int searchXmlNode_Nth( xmlNode *pHeadNode, xmlNode **ppDestNode, char *pNodeName, uint32_t indexN )
{
    xmlNode *pNode_ItemList[MAX_MULTIPLE_XML_NODE_COUNT];
    uint32_t itemCount;

    itemCount = 0;

    searchXmlMultipleNodes( pHeadNode, pNode_ItemList, &itemCount, pNodeName );
    if (itemCount == 0 || itemCount < indexN+1)
    {
        printf( "Node %s not found\n", pNodeName );
        return FALSE;
    }

    *ppDestNode = pNode_ItemList[indexN];

    return TRUE;
}

int searchXmlMultipleNodes( xmlNode *pHeadNode, xmlNode *ppDestNode[], uint32_t *pNodeCount, char *pNodeName )
{
    xmlNode *pPeerNode;

    if (pHeadNode == NULL)
        return FALSE;

    for (pPeerNode = pHeadNode; pPeerNode != NULL; pPeerNode = pPeerNode->next)
    {
        if (pPeerNode->type == XML_ELEMENT_NODE)
        {
            if (strcmp((char *) pPeerNode->name, pNodeName ) == 0)
            {
                if (*pNodeCount < MAX_MULTIPLE_XML_NODE_COUNT)
                {
                    ppDestNode[*pNodeCount] = pPeerNode;
                    (*pNodeCount)++;
                }
            }
        }

        if (pPeerNode->children != NULL)
        {
            searchXmlMultipleNodes( pPeerNode->children, ppDestNode, pNodeCount, pNodeName );
        }
    }

    return FALSE;
}

int getXmlNodeValueU32( xmlNode *pHeadNode, char *pNodeName, uint32_t *pValue )
{
    xmlNode *pNode_Item;
    xmlChar *pValue_Item;

    if (!searchXmlNode(pHeadNode, &pNode_Item, pNodeName))
    {
        printf( "Node %s not found\n", pNodeName );
        return FAILURE;
    }

    pValue_Item = xmlNodeGetContent(pNode_Item);

    if (strncasecmp( (char *)pValue_Item, "0x", 2) == 0)
        *pValue = (uint32_t) strtol((char *)pValue_Item, NULL, 16 );
    else
        *pValue = (uint32_t) strtol((char *)pValue_Item, NULL, 10 );

    return SUCCESS;
}

int getXmlNodeValueS32( xmlNode *pHeadNode, char *pNodeName, int32_t *pValue )
{
    xmlNode *pNode_Item;
    xmlChar *pValue_Item;

    if (!searchXmlNode(pHeadNode, &pNode_Item, pNodeName))
    {
        printf( "Node %s not found\n", pNodeName );
        return FAILURE;
    }

    pValue_Item = xmlNodeGetContent(pNode_Item);
    *pValue = (int32_t) atoi((char *) pValue_Item);

    return SUCCESS;
}

int getXmlNodeValueU32_Nth( xmlNode *pHeadNode, char *pNodeName, uint32_t *pValue, uint32_t indexN )
{
    xmlNode *pNode_ItemList[MAX_MULTIPLE_XML_NODE_COUNT];
    xmlChar *pValue_Item;
    uint32_t itemCount;

    itemCount = 0;

    searchXmlMultipleNodes( pHeadNode, pNode_ItemList, &itemCount, pNodeName );
    if (itemCount == 0 || itemCount < indexN+1)
    {
        printf( "Node %s not found\n", pNodeName );
        return FAILURE;
    }

    pValue_Item = xmlNodeGetContent(pNode_ItemList[indexN]);

    if (strncasecmp( (char *)pValue_Item, "0x", 2) == 0)
        *pValue = (uint32_t) strtol((char *)pValue_Item, NULL, 16 );
    else
        *pValue = (uint32_t) strtol((char *)pValue_Item, NULL, 10 );

    return SUCCESS;
}

int getXmlNodeValueS32_Nth( xmlNode *pHeadNode, char *pNodeName, int32_t *pValue, uint32_t indexN )
{
    xmlNode *pNode_ItemList[MAX_MULTIPLE_XML_NODE_COUNT];
    xmlChar *pValue_Item;
    uint32_t itemCount;

    itemCount = 0;

    searchXmlMultipleNodes( pHeadNode, pNode_ItemList, &itemCount, pNodeName );
    if (itemCount == 0 || itemCount < indexN+1)
    {
        printf( "Node %s not found\n", pNodeName );
        return FAILURE;
    }

    pValue_Item = xmlNodeGetContent(pNode_ItemList[indexN]);

    if (strncasecmp( (char *)pValue_Item, "0x", 2) == 0)
        *pValue = (int32_t) strtol((char *)pValue_Item, NULL, 16 );
    else
        *pValue = (int32_t) strtol((char *)pValue_Item, NULL, 10 );

    return SUCCESS;
}

int getXmlNodeValueU32Array( xmlNode *pHeadNode, char *pNodeName, uint32_t *pValueArray, uint32_t *pValueCount )
{
    xmlNode *pNode_Item;
    xmlChar *pValue_Item;
    char tokenStr[MAX_XML_STRING_TOKEN_SIZE];
    char *pSubToken;
    uint32_t valueCount, value;

    if (!searchXmlNode(pHeadNode, &pNode_Item, pNodeName))
    {
        printf( "Node %s not found\n", pNodeName );
        return FAILURE;
    }

    pValue_Item = xmlNodeGetContent(pNode_Item);
    strncpy( tokenStr, (char *) pValue_Item, MAX_XML_STRING_TOKEN_SIZE );

    valueCount = 0;
    pSubToken = strtok(tokenStr, " \t,;\n\r");
    while (pSubToken != NULL)
    {
        if (strncasecmp( (char *)pSubToken, "0x", 2) == 0)
            value = (uint32_t) strtol((char *)pSubToken, NULL, 16 );
        else
            value = (uint32_t) strtol((char *)pSubToken, NULL, 10 );

        pValueArray[valueCount] = value;
        valueCount++;
        pSubToken = strtok(NULL, " \t,;\n\r");
    }

    *pValueCount = valueCount; 

    return SUCCESS;
}

int getXmlNodeValueU32Array_Nth( xmlNode *pHeadNode, char *pNodeName, uint32_t *pValueArray, uint32_t *pValueCount, uint32_t indexN )
{
    xmlNode *pNode_ItemList[MAX_MULTIPLE_XML_NODE_COUNT];
    xmlChar *pValue_Item;
    uint32_t itemCount, valueCount, value;
    char tokenStr[MAX_XML_STRING_TOKEN_SIZE];
    char *pSubToken;

    itemCount = 0;

    searchXmlMultipleNodes( pHeadNode, pNode_ItemList, &itemCount, pNodeName );
    if (itemCount == 0 || itemCount < indexN+1)
    {
        printf( "Node %s not found\n", pNodeName );
        return FAILURE;
    }

    pValue_Item = xmlNodeGetContent(pNode_ItemList[indexN]);
    strncpy( tokenStr, (char *) pValue_Item, MAX_XML_STRING_TOKEN_SIZE );

    valueCount = 0;
    pSubToken = strtok(tokenStr, " \t,;\n\r");
    while (pSubToken != NULL)
    {
        if (strncasecmp( (char *)pSubToken, "0x", 2) == 0)
            value = (uint32_t) strtol((char *)pSubToken, NULL, 16 );
        else
            value = (uint32_t) strtol((char *)pSubToken, NULL, 10 );

        pValueArray[valueCount] = value;
        valueCount++;
        pSubToken = strtok(NULL, " \t,;\n\r");
    }

    *pValueCount = valueCount;

    return SUCCESS;
}

char *getXmlNodeValueStr( xmlNode *pHeadNode, char *pNodeName )
{
    xmlNode *pNode_Item;
    xmlChar *pValue_Item;

    if (!searchXmlNode(pHeadNode, &pNode_Item, pNodeName))
    {
        return NULL;
    }

    pValue_Item = xmlNodeGetContent(pNode_Item);
    return (char *) pValue_Item;
}

char *getXmlNodeValueStr_Nth( xmlNode *pHeadNode, char *pNodeName, uint32_t indexN )
{
    xmlNode *pNode_ItemList[MAX_MULTIPLE_XML_NODE_COUNT];
    xmlChar *pValue_Item;
    uint32_t itemCount;

    itemCount = 0;

    searchXmlMultipleNodes( pHeadNode, pNode_ItemList, &itemCount, pNodeName );
    if (itemCount == 0 || itemCount < indexN+1)
    {
        return NULL;
    }

    pValue_Item = xmlNodeGetContent(pNode_ItemList[indexN]);
    return (char *) pValue_Item;
}


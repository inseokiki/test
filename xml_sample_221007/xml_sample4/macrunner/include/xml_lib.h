#ifndef _XML_LIB_H_
#define _XML_LIB_H_

#include <libxml/xmlreader.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define MAX_MULTIPLE_XML_NODE_COUNT    128
#define MAX_XML_STRING_TOKEN_SIZE      1024

void printXmlNodes(xmlNode *pStartNode, uint32_t depth);
int searchXmlNode( xmlNode *pHeadNode, xmlNode **ppDestNode, char *pNodeName );
int searchXmlNode_Nth( xmlNode *pHeadNode, xmlNode **ppDestNode, char *pNodeName, uint32_t indexN );
int searchXmlMultipleNodes( xmlNode *pHeadNode, xmlNode *ppDestNode[], uint32_t *pNodeCount, char *pNodeName );
int getXmlNodeValueU32( xmlNode *pHeadNode, char *pNodeName, uint32_t *pValue );
int getXmlNodeValueS32( xmlNode *pHeadNode, char *pNodeName, int32_t *pValue );
int getXmlNodeValueU32_Nth( xmlNode *pHeadNode, char *pNodeName, uint32_t *pValue, uint32_t indexN );
int getXmlNodeValueS32_Nth( xmlNode *pHeadNode, char *pNodeName, int32_t *pValue, uint32_t indexN );
int getXmlNodeValueU32Array( xmlNode *pHeadNode, char *pNodeName, uint32_t *pValueArray, uint32_t *pValueCount );
int getXmlNodeValueU32Array_Nth( xmlNode *pHeadNode, char *pNodeName, uint32_t *pValueArray, uint32_t *pValueCount, uint32_t indexN );
char *getXmlNodeValueStr( xmlNode *pHeadNode, char *pNodeName );
char *getXmlNodeValueStr_Nth( xmlNode *pHeadNode, char *pNodeName, uint32_t indexN );

#endif    // of _XML_LIB_H_

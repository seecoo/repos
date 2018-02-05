#ifndef __UnionXML_H__
#define __UnionXML_H__

/* 
** head files from system. Perhaps be different among variant platforms.
*/
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif
#include <stdlib.h>

/* head files from libxml */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
//#include "jb_type.h"

/*
 * define the context structure for UnionXML processing.
 */
typedef struct{
	xmlDocPtr	doc;			/* XML document pointer */
	xmlNodePtr	curNode;		/* the current xml node pointer */
	int		nIsFmt;			/* the dump format flag */
#ifdef _WIN32_
	int		nIsInit;
#endif
}TUnionXMLCtxt;
typedef TUnionXMLCtxt		*PUnionXMLCtxt;


// XML初始化接口
void UnionXMLInitParser();

// XML清除资源接口
void UnionXMLCleanupParser();

/*
** UnionXMLFree: free the all the related memory for UnionXML context.
*/
void UnionXMLFree(PUnionXMLCtxt pxmlc);

/*
** UnionXMLReset: reset the all the related memory for UnionXML context.
*/
void UnionXMLReset(PUnionXMLCtxt pxmlc);

/*
** UnionXMLInit: init the UnionXML context with a root element.
** 
** @pxmlc: The caller must declare the UnionXML Context, pointed by pxmlc.
** @rootname: The caller must specify the name of the root element, 
**            which can not be NULL.
** 
** Return values: 0, success; -1, failure.
**
** note: want to build a new UnionXML context, you should call this API first.
*/
int UnionXMLInit(PUnionXMLCtxt pxmlc, const char* rootname, const char* encoding);

/*
** UnionXMLInitFromBuf: Init the UnionXML context from a string buffer in XML. 
**
** @pxmlc: The caller must declare the UnionXML Context, pointed by pxmlc.
** @buf: The buffer, pointed by @buf, is a string buf in XML, ended by zero.
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLInitFromBuf(PUnionXMLCtxt pxmlc, const char* buf);

/*
** UnionXMLInitFromFile: Init the UnionXML context from a XML document. 
**
** @pxmlc: The caller must declare the UnionXML Context, pointed by pxmlc.
** @fn: file name of the XML document
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLInitFromFile(PUnionXMLCtxt pxmlc, const char* fn);

/*
** UnionXMLSetDumpFmt: Set the dump format, the default is without formatting spaces.
**
** @pxmlc: the UnionXML Context pointer.
** @nIsFmted: 0, without formatting spaces; 1, with formatting spaces.
**
** NOTE: this API will affect the result of dump.
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLSetDumpFmt(PUnionXMLCtxt pxmlc, int nIsFmt);

/*
** UnionXMLDumpToBuf: Dump UnionXML to a string buffer.
**
** @pxmlc: The UnionXML Context, pointed by @pxmlc, to be dumped.
** @pBuf: The buffer, pointed by @pBuf, will load the XML string.
** @pSize: The integer, pointed by @pSize, which specifies the size
**         of the buffer pointed by @pBuf, will be changed to the actual
**         length of the string after dump.
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLDumpToBuf(PUnionXMLCtxt pxmlc, char* pBuf, int pSize);

/*
** UnionXMLDumpToFile: Dump UnionXML to a file.
**
** @pxmlc: The UnionXML Context, pointed by @pxmlc, to be dumped.
** @fn: name of the file to be written; if file exists, it will be truncated.
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLDumpToFile(PUnionXMLCtxt pxmlc, const char* fn);


/*
** UnionXMLSelect: Select and Locate the UnionXML Context on the node 
**             directed by a node path according to the flag.
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @nId: The index attribute of the node.
**
** NODE PATH: 
**   1) To show absolute path: /root/a/b
**   2) To show relative path: a/b, the same as /root/a/b, if locate /root now.
**
** INDEX ATTRIBUTE:
**   1) must be equal or greater than 0
**   2) if 0, there should not be two nodes with same name and path.
**   3) else, locate the exact @nId-th node.
** 
** RETURNS:
**   -1, failure; 1, success; 0, no nodes found.
*/
int UnionXMLSelect(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId);

/*
** UnionXMLLocate: Locate the UnionXML Context on the node directed by a node path.
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @nId: The index attribute of the node.
**
** NODE PATH: 
**   1) To show absolute path: /root/a/b
**   2) To show relative path: a/b, the same as /root/a/b, if locate /root now.
**
** INDEX ATTRIBUTE:
**   1) must be equal or greater than 0
**   2) if 0, there should not be two nodes with same name and path.
**   3) else, locate the exact @nId-th node.
*/
int UnionXMLLocate(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId);

/*
** UnionXMLGet: get the content of the node directed by @pcXPath
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @nId: The index attribute of the node.
** @pcBuf: @pcBuf will load the content.
** @nSize: with the original size of @pcBuf, will be the actual length.
**
** NODE PATH: 
**   1) To show absolute path: /root/a/b
**   2) To show relative path: a/b, the same as /root/a/b, if locate /root now.
**
** INDEX ATTRIBUTE:
**   1) must be equal or greater than 0
**   2) if 0, there should not be two nodes with same name and path.
**   3) else, locate the exact @nId-th node.
*/
int UnionXMLGet(PUnionXMLCtxt pxmlc, const char* pcXPath, int nId, char* pcBuf, int nSize);

/*
** UnionXMLSelectGet: get the content of the node directed by @pcXPath
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @nId: The index attribute of the node.
** @pcBuf: @pcBuf will load the content.
** @nSize: with the original size of @pcBuf, will be unchanged.
**
** NODE PATH: 
**   1) To show absolute path: /root/a/b
**   2) To show relative path: a/b, the same as /root/a/b, if locate /root now.
**
** INDEX ATTRIBUTE:
**   1) must be equal or greater than 0
**   2) if 0, there should not be two nodes with same name and path.
**   3) else, locate the exact @nId-th node.
** 
** RETURNS:
**   -1, failure; 1, success; 0, no nodes found.
*/
int UnionXMLSelectGet(PUnionXMLCtxt pxmlc, const char* pcXPath, int nId, char* pcBuf, int nSize);

/*
** UnionXMLPut: Add a new node and write the content.
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @pcContent: point to the content string, if null, shows not a leaf node.
** @nId: The index attribute of the node.
**
** NODE PATH: 
**   1) To show absolute path: /root/a/b
**   2) To show relative path: a/b, the same as /root/a/b, if locate /root now.
**
** INDEX ATTRIBUTE:
**   1) must be equal or greater than 0
**   2) if 0, there should not be two nodes with same name and path.
**   3) else, locate the exact @nId-th node.
**
** NOTE:
**   1) If parent of the node directed by @pcXPath not exists, creates it.
**   2) If parent of the parent not exists, creates it too.
**   3  And so on. 
*/
int UnionXMLPut(PUnionXMLCtxt pxmlc, const char* pcXPath, char* pcContent, unsigned int nId);

/*
** UnionXMLLocateNew: Locate the UnionXML Context on the node directed by a node path;
**               If not exists, creates first and locate on it.
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @nId: The index attribute of the node.
**
** NODE PATH: 
**   1) To show absolute path: /root/a/b
**   2) To show relative path: a/b, the same as /root/a/b, if locate /root now.
**
** INDEX ATTRIBUTE:
**   1) must be equal or greater than 0
**   2) if 0, there should not be two nodes with same name and path.
**   3) else, locate the exact @nId-th node.
*/
int UnionXMLLocateNew(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId);
int UnionXMLLocateNewEx(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId);

/*
** UnionXMLDumpToTmpBuf: Dump UnionXML to a xmlChar buffer, return to caller
**
** @pxmlc: The UnionXML Context, pointed by @pxmlc, to be dumped.
** @ppBuf: will be set to the address of the xmlChar buffer.
** @pSize:  will be set to the actual length of the xmlChar buffer
** 
** Return values: 0, success; -1, failure.
**
** NOTE: The caller must call UnionXMLFreeDumpTmpBuf to free memory,
**       pointed by (*ppBuf)
*/
int UnionXMLDumpToTmpBuf(PUnionXMLCtxt pxmlc, char* ppBuf, int pSize);
void UnionXMLFreeDumpTmpBuf(char* pBuf);

/*
** UnionXMLExchgId: Exchange the ids of two nodes with same path.
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @nId1, @nId2: The two ids.
**
** NODE PATH: 
**   1) To show absolute path: /root/a/b
**   2) To show relative path: a/b, the same as /root/a/b, if locate /root now.
**
** INDEX ATTRIBUTE:
**   1) must be greater than 0
**
** NOTE:
**   1) The two nodes must exist.
*/
int UnionXMLExchgId(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId1, unsigned int nId2);

/*
** UnionXMLRevIds: Reverse the ids of two nodes between @nFirstId and @nLastId.
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @nFirstId, @nLastId: The first id and the last id.
**
** NODE PATH: 
**   1) To show absolute path: /root/a/b
**   2) To show relative path: a/b, the same as /root/a/b, if locate /root now.
**
** INDEX ATTRIBUTE:
**   1) must be greater than 0
**
** NOTE:
**   1) all the nodes must exist.
*/
int UnionXMLRevIds(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nFirstId, unsigned int nLastId);

//代码转换:从一种编码转为另一种编码
int UnionXMLCodeConvert(char *from_charset,char *to_charset,char *inbuf,size_t *inlen,char *outbuf,size_t *outlen);

int UnionXMLLog(PUnionXMLCtxt pxmlc);

// 删除节点
int UnionXMLDelete(PUnionXMLCtxt pxmlc, const char* pcXPath, int nId);

xmlNodePtr UnionXMLSearchChild(xmlNodePtr parent, const char* name, unsigned int nId);

#endif

/* end of unionXML.h */

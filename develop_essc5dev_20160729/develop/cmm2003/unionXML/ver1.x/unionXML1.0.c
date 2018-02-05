/*
 *
 * unionXML.c: UnionXML库API源程序文件
 *
 */

#define _UnionLogMDL_3_x_	
#define _UnionEnv_3_x_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
 
#include "unionXML.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/debugXML.h>
#include <libxml/xmlerror.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxml/globals.h>

#include <iconv.h>

/* the separator of UnionXML PATH string */
#define UnionXMLS_PATH_SEP		"/"

/* the name of ID attribute */
#define UnionXMLS_ATTR_ID	"ID"

#define UnionXMLIsAbsolutePath(a)	(a[0]=='/')

//xml错误日志函数
void    xmlErrLog(void *ctx ATTRIBUTE_UNUSED, const char *msg,...)
{
	int	len;
	char	buf[1024+1];
	va_list	params;
	va_start(params, msg);
	memset(buf,0,sizeof(buf));
	len = vsnprintf(buf, 1024, msg, params);
	buf[len] = 0;
	va_end(params);
	UnionFilterHeadAndTailBlank(buf);
        UnionUserErrLog(buf);
}
// XML初始化接口
void UnionXMLInitParser()
{
	xmlInitParser();
	return;
}

// XML清除资源接口
void UnionXMLCleanupParser()
{
	xmlCleanupParser();
	xmlMemoryDump();
	return;
}

/*
** UnionXMLFree: free the all the related memory for UnionXML context.
*/
void UnionXMLFree(PUnionXMLCtxt pxmlc)
{
	if (pxmlc == NULL)
		return;
	if (pxmlc->doc)
	{
#ifdef _WIN32
		if ((pxmlc->doc->encoding) && (pxmlc->nIsInit == 1))
#else
		if (pxmlc->doc->encoding)
#endif			
		{
			free((void *)pxmlc->doc->encoding);
			pxmlc->doc->encoding = NULL;
		}
		xmlFreeDoc(pxmlc->doc);
		pxmlc->doc = NULL;
	}
	memset(pxmlc, 0, sizeof(*pxmlc));
}

/*
** UnionXMLInit:: init the UnionXML context with a root element.
** 
** @pxmlc: The caller must declare the UnionXML Context, pointed by pxmlc.
** @rootname: The caller must specify the name of the root element, 
**            which can not be NULL.
** @encoding: "GBK","UTF-8","ISO-8859-1"
**
** Return values: 0, success; -1, failure.
**
** note: want to build a new UnionXML context, you should call this API first.
*/
int UnionXMLInit(PUnionXMLCtxt pxmlc, const char* rootname, const char* encoding)
{
	if (pxmlc == NULL) {
		UnionUserErrLog("in UnionXMLInit:: pxmlc can not be null.\n");
		return (errCodeParameter);
	}

	xmlSetGenericErrorFunc(NULL, xmlErrLog);
	
	memset(pxmlc, 0, sizeof(TUnionXMLCtxt));
	
	xmlKeepBlanksDefault(0);
	pxmlc->doc = xmlNewDoc(BAD_CAST "1.0");
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLInit:: xmlNewDoc eror.\n");
		return (errCodeParameter);
	}
	//doc->charset = XML_CHAR_ENCODING_8859_1;
	//pgencoding = strdup("ISO-8859-1");
	if (encoding)
	{
		pxmlc->doc->encoding = xmlStrdup((xmlChar *)encoding);
#ifdef _WIN32	
		pxmlc->nIsInit = 1;
#endif
	}
	pxmlc->curNode = xmlNewNode(NULL, BAD_CAST rootname);
	if (pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLInit:: xmlNewNode root element error.\n");
		UnionXMLFree(pxmlc);
		return (errCodeParameter);
	}
	xmlDocSetRootElement(pxmlc->doc, pxmlc->curNode);
	
	return (0);
}

/*
** UnionXMLInitFromBuf:: Init the UnionXML context from a string buffer in XML. 
**
** @pxmlc: The caller must declare the UnionXML Context, pointed by pxmlc.
** @buf: The buffer, pointed by @buf, is a string buf in XML, ended by zero.
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLInitFromBuf(PUnionXMLCtxt pxmlc, const char* buf)
{
	
	if (pxmlc == NULL) {
		UnionUserErrLog("in UnionXMLInitFromBuf:: pxmlc can not be NULL.\n");
		return (errCodeParameter);
	}
	
	xmlKeepBlanksDefault(0);
	xmlSetGenericErrorFunc(NULL, xmlErrLog);

	pxmlc->doc = xmlParseDoc(BAD_CAST buf);
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLInitFromBuf:: xmlParseDoc failed.\n");
		return (errCodeParameter);
	}
	
#ifdef _WIN32	
	pxmlc->nIsInit = 0;
#endif
	
	pxmlc->curNode = xmlDocGetRootElement(pxmlc->doc);
	if (pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLInitFromBuf:: empty document.\n");
		UnionXMLFree(pxmlc);
		return (errCodeParameter);
	}
	
	return (0);
}

/*
** UnionXMLInitFromFile: Init the UnionXML context from a XML document. 
**
** @pxmlc: The caller must declare the UnionXML Context, pointed by pxmlc.
** @fn: file name of the XML document
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLInitFromFile(PUnionXMLCtxt pxmlc, const char* fn)
{
	if (pxmlc == NULL) {
		UnionUserErrLog("in UnionXMLInitFromBuf:: pxmlc can not be NULL.\n");
		return (errCodeParameter);
	}
	
	memset(pxmlc, 0, sizeof(TUnionXMLCtxt));
	
	xmlKeepBlanksDefault(0);
	xmlSetGenericErrorFunc(NULL, xmlErrLog);

	pxmlc->doc = xmlParseFile(fn);
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLInitFromBuf:: xmlParseFile failed.\n");
		return (errCodeParameter);
	}

#ifdef _WIN32
	pxmlc->nIsInit = 0;
#endif

	pxmlc->curNode = xmlDocGetRootElement(pxmlc->doc);
	if (pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLInitFromBuf:: empty document.\n");
		UnionXMLFree(pxmlc);
		return (errCodeParameter);
	}
	
	return (0);
}

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
int UnionXMLSetDumpFmt(PUnionXMLCtxt pxmlc, int nIsFmt)
{
	if (pxmlc == NULL) {
		UnionUserErrLog("in UnionXMLSetDumpFmt:: pxmlc can not be null.\n");
		return (errCodeParameter);
	}
	
	if (nIsFmt != 0)
		pxmlc->nIsFmt = 1;
	else
		pxmlc->nIsFmt = 0;
	
	return 0;
}

/*
** UnionXMLDumpToBuf:: Dump UnionXML to a string buffer.
**
** @pxmlc: The UnionXML Context, pointed by @pxmlc, to be dumped.
** @pBuf: The buffer, pointed by @pBuf, will load the XML string.
** @pSize: The integer, pointed by @pSize, which specifies the size
**         of the buffer pointed by @pBuf, will be changed to the actual
**         length of the string after dump.
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLDumpToBuf(PUnionXMLCtxt pxmlc, char* pBuf, int pSize)
{
	xmlChar		*pxcBuf;
	int		nBufSize = 0;
	
	if (pxmlc == NULL || pBuf == NULL) {
		UnionUserErrLog("in UnionXMLDumpToBuf:: pxmlc or pBuf can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLDumpToBuf:: doc cannot be null.\n");
		return (errCodeParameter);
	}
	
	xmlDocDumpFormatMemory(pxmlc->doc, &pxcBuf, &nBufSize, pxmlc->nIsFmt);
	if (pxcBuf == NULL || nBufSize == 0) {
		UnionUserErrLog("in UnionXMLDumpToTmpBuf:: xmlDocDumpFormatMemory error!\n");
		if (pxcBuf)
			xmlFree(pxcBuf);
		return (errCodeParameter);
	}
	if (nBufSize+1 > pSize) {
		//UnionUserErrLog("in UnionXMLDumpToBuf:: [%d] > [%d] buffer size is not enough to dump.\n", nBufSize+1, pSize);
		xmlFree(pxcBuf);
		return (errCodeSmallBuffer);
	}
	memcpy(pBuf, pxcBuf, nBufSize);
	pBuf[nBufSize] = 0;
	xmlFree(pxcBuf);
	
	return(nBufSize);
}

/*
void UnionXMLFreeDumpTmpBuf(char* pBuf)
{
	if (pBuf)
		xmlFree(BAD_CAST pBuf);
}
*/

/*
** UnionXMLDumpToTmpBuf:: Dump UnionXML to a xmlChar buffer, return to caller
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
int UnionXMLDumpToTmpBuf(PUnionXMLCtxt pxmlc, char *ppBuf, int pSize)
{
	xmlChar		*pxcBuf = NULL;
	int		nBufSize = 0;
	
	if (pxmlc == NULL || ppBuf == NULL) {
		UnionUserErrLog("in UnionXMLDumpToTmpBuf:: pxmlc or ppBuf can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLDumpToTmpBuf:: doc cannot be null.\n");
		return (errCodeParameter);
	}
	
	xmlDocDumpFormatMemory(pxmlc->doc, &pxcBuf, &nBufSize, pxmlc->nIsFmt);
	if (pxcBuf == NULL || nBufSize == 0) {
		UnionUserErrLog("in UnionXMLDumpToTmpBuf:: dump mem error!\n");
		return (errCodeParameter);
	}
	if (nBufSize > pSize - 1)
	{
		UnionUserErrLog("in UnionXMLDumpToTmpBuf:: nBufSize[%d] > pSize[%d] - 1!\n",nBufSize,pSize);
		return(errCodeSmallBuffer);
	}
	memcpy(ppBuf,pxcBuf,nBufSize);
	free(pxcBuf);
	
	return(nBufSize);
}

/*
** UnionXMLDumpToFile::  Dump UnionXML to a file.
**
** @pxmlc: The UnionXML Context, pointed by @pxmlc, to be dumped.
** @fn: name of the file to be written; if file exists, it will be truncated.
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLDumpToFile(PUnionXMLCtxt pxmlc, const char* fn)
{
	int	ret;
	
	if (pxmlc == NULL || fn == NULL) {
		UnionUserErrLog("in UnionXMLDumpToFile::  pxmlc or fn can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLDumpToFile::  doc cannot be null.\n");
		return (errCodeParameter);
	}
	
	ret = xmlSaveFormatFile(fn, pxmlc->doc, pxmlc->nIsFmt);
	if (ret < 0) {
		UnionUserErrLog("in UnionXMLDumpToFile::  xmlSaveFormatFile error!\n");
		return (errCodeParameter);
	}
	
	return 0;
}


/*
** UnionXMLSelect:: Select and Locate the UnionXML Context on the node 
**             directed by a node path according to the flag.
**
** @pxmlc: UnionXML Context pointer
** @pcXPath: a constant string to show a node path
** @nId: The index attribute of the node.
** @nFlag: 0, Select flag; 1, Locate flag.
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
int UnionXMLSelect(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId)
{
	xmlChar*		pxc;
	xmlXPathContextPtr	pXPCtxt = NULL;
	xmlXPathObjectPtr	pXPObj = NULL;
	char			sAttr[64];
	
	if (pxmlc == NULL || pcXPath == NULL) {
		UnionUserErrLog("in UnionXMLSelect:: pxmlc or pcXPath can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLSelect:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	if (UnionXMLIsAbsolutePath(pcXPath)) {
		pxc = xmlStrdup(BAD_CAST pcXPath);
	} else {
		pxc = xmlGetNodePath(pxmlc->curNode);
		if (pxc == NULL) {
			UnionUserErrLog("in UnionXMLSelect:: xmlGetNodePath fail.\n");
			return (errCodeParameter);
		}
		pxc = xmlStrcat(pxc, BAD_CAST "/");
		if (pxc == NULL) {
			UnionUserErrLog("in UnionXMLSelect:: xmlStrcat[1] fail.\n");
			return (errCodeParameter);
		}
		pxc = xmlStrcat(pxc, BAD_CAST pcXPath);
		if (pxc == NULL) {
			UnionUserErrLog("in UnionXMLSelect:: xmlStrcat[2] fail.\n");
			return (errCodeParameter);
		}
	}
	if (nId > 0) {
		sprintf(sAttr, "[attribute::%s='%d']", UnionXMLS_ATTR_ID, nId);
		pxc = xmlStrcat(pxc, BAD_CAST sAttr);
		if (pxc == NULL) {
			UnionUserErrLog("in UnionXMLSelect:: xmlStrcat[3] fail.\n");
			return (errCodeParameter);
		}
	}
		
	pXPCtxt = xmlXPathNewContext(pxmlc->doc);
	if (pXPCtxt == NULL) {
		UnionUserErrLog("in UnionXMLSelect:: xmlXPathNewContext fail.\n");
		xmlFree(pxc);
		return (errCodeParameter);
	}
	pXPObj = xmlXPathEval(pxc, pXPCtxt);
	if (pXPObj == NULL) {
		UnionUserErrLog("in UnionXMLSelect[%s]:: path eval error.\n", pxc);
		xmlFree(pxc);
		xmlXPathFreeContext(pXPCtxt);
		return (errCodeParameter);
	}
	if (pXPObj->type != XPATH_NODESET) {                                      	
		UnionUserErrLog("in UnionXMLSelect[%s]:: XPATHOBJ type is not NODESET.\n", pxc);   	
		xmlFree(pxc);                                                     	
		xmlXPathFreeContext(pXPCtxt);                                     	
		xmlXPathFreeObject(pXPObj);                                       	
		return (errCodeParameter);                                                      	
	}
	if (pXPObj->nodesetval == NULL)
	{
		UnionUserErrLog("in UnionXMLSelect[%s]:: none of nodes found.\n", pxc);            	
		xmlFree(pxc);                                                     	
		xmlXPathFreeContext(pXPCtxt);
		xmlXPathFreeObject(pXPObj);
		return (errCodeObjectMDL_FieldNotExist);
	}
	/* 2006-11-22 */
	switch (pXPObj->nodesetval->nodeNr) {
	case 0:
		UnionUserErrLog("in UnionXMLSelect[%s]:: none of nodes found.\n", pxc);            	
		xmlFree(pxc);                                                     	
		xmlXPathFreeContext(pXPCtxt);                                     	
		xmlXPathFreeObject(pXPObj);                                       	
		return (errCodeObjectMDL_FieldNotExist);                                                      	
	case 1:
		break;
	default:
		UnionUserErrLog("in UnionXMLSelect[%s]:: there is %d nodes found.\n",
			pxc, pXPObj->nodesetval->nodeNr);
		xmlFree(pxc);
		xmlXPathFreeContext(pXPCtxt);
		xmlXPathFreeObject(pXPObj);
		return (errCodeObjectMDL_FieldNumberTooMuch);                                                      	
	}                                                                         	
	pxmlc->curNode = pXPObj->nodesetval->nodeTab[0];                           	
	xmlFree(pxc);                                                             	
	xmlXPathFreeContext(pXPCtxt);                                             	
	xmlXPathFreeObject(pXPObj);      
	                                         	
	return (1);
}                                                              	

/*
** UnionXMLLocate:: Locate the UnionXML Context on the node directed by a node path.
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
int UnionXMLLocate(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId)
{
	int			i;
	char			newPath[512+1];
	xmlChar*		pxc;
	xmlXPathContextPtr	pXPCtxt = NULL;
	xmlXPathObjectPtr	pXPObj = NULL;
	char			sAttr[64];
	
	if (pxmlc == NULL || pcXPath == NULL) {
		UnionUserErrLog("in UnionXMLLocate:: pxmlc or pcXPath can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLLocate:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	memset(newPath,0,sizeof(newPath));
	for (i = 0; i < 2; i++)
	{	

		if (i == 0)
			strcpy(newPath,pcXPath);
		else
			UnionToLowerCase(newPath);
		
		if (UnionXMLIsAbsolutePath(newPath)) {
			pxc = xmlStrdup(BAD_CAST newPath);
		} else {
			pxc = xmlGetNodePath(pxmlc->curNode);
			if (pxc == NULL) {
				UnionUserErrLog("in UnionXMLLocate:: xmlGetNodePath fail.\n");
				return (errCodeParameter);
			}
			pxc = xmlStrcat(pxc, BAD_CAST "/");
			if (pxc == NULL) {
				UnionUserErrLog("in UnionXMLLocate:: xmlStrcat[1] fail.\n");
				return (errCodeParameter);
			}
			pxc = xmlStrcat(pxc, BAD_CAST newPath);
			if (pxc == NULL) {
				UnionUserErrLog("in UnionXMLLocate:: xmlStrcat[2] fail.\n");
				return (errCodeParameter);
			}
		}
		
		if (nId > 0) {
			sprintf(sAttr, "[attribute::%s='%d']", UnionXMLS_ATTR_ID, nId);
			pxc = xmlStrcat(pxc, BAD_CAST sAttr);
			if (pxc == NULL) {
				UnionUserErrLog("in UnionXMLLocate:: xmlStrcat[3] fail.\n");
				return (errCodeParameter);
			}
		}
		
		pXPCtxt = xmlXPathNewContext(pxmlc->doc);
		if (pXPCtxt == NULL) {
			UnionUserErrLog("in UnionXMLLocate:: xmlXPathNewContext fail.\n");
			xmlFree(pxc);
			return (errCodeParameter);
		}
		pXPObj = xmlXPathEval(pxc, pXPCtxt);
		if (pXPObj == NULL) {
			UnionUserErrLog("in UnionXMLLocate[%s]:: path eval error.\n", pxc);
			xmlFree(pxc);
			xmlXPathFreeContext(pXPCtxt);
			return (errCodeParameter);
		}
		if (pXPObj->type != XPATH_NODESET) {                                      	
			UnionUserErrLog("in UnionXMLLocate[%s]:: XPATHOBJ type is not NODESET.\n", pxc);   	
			xmlFree(pxc);                                                     	
			xmlXPathFreeContext(pXPCtxt);                                     	
			xmlXPathFreeObject(pXPObj);                                       	
			return (errCodeParameter);                                                      	
		}
		/* 2006-11-22
		** 如"/nbrcs/acct/acctname"形式的PATH, 如果XML中"acct"节点不存在,
		** xmlXPathEval调用完成后, pXPOjb->nodesetval=NULL, 继续进行下面
		** 操作在LINUX平台则会core, 因此在此进行预判断, 从而屏蔽core文件
		** 产生。
		**/
		if (pXPObj->nodesetval == NULL)
		{
			//UnionLog("in UnionXMLLocate[%s]:: none of nodes found.\n", pxc);            	
			xmlFree(pxc);                                                     	
			xmlXPathFreeContext(pXPCtxt);                                     	
			xmlXPathFreeObject(pXPObj);                                       	
			continue;                                                      	
		}
		/* 2006-11-22 */
		switch (pXPObj->nodesetval->nodeNr) { 
		case 0:                                                                   	
			//UnionLog("in UnionXMLLocate[%s]:: none of nodes found.\n", pxc);            	
			xmlFree(pxc);
			xmlXPathFreeContext(pXPCtxt);
			xmlXPathFreeObject(pXPObj);
			continue;
		case 1:
			break;
		default:                                                                  	
			UnionUserErrLog("in UnionXMLLocate[%s]:: there is %d nodes found.\n",              	
				pxc, pXPObj->nodesetval->nodeNr);
			xmlFree(pxc);
			xmlXPathFreeContext(pXPCtxt);
			xmlXPathFreeObject(pXPObj);
			return (errCodeObjectMDL_FieldNumberTooMuch);                                                      	
		}                                                                         	
		pxmlc->curNode = pXPObj->nodesetval->nodeTab[0];                           	
		xmlFree(pxc);                                                             	
		xmlXPathFreeContext(pXPCtxt);                                             	
		xmlXPathFreeObject(pXPObj);      
		                                         	
		return (0);
	}
	//UnionDebugLog("in UnionXMLLocate:: nodes[%s] is not found!\n", pcXPath);            	
	return(errCodeObjectMDL_FieldNotExist);
}                                                              	

/*
** UnionXMLGet:: get the content of the node directed by @pcXPath
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
*/
int UnionXMLGet(PUnionXMLCtxt pxmlc, const char* pcXPath, int nId, char* pcBuf, int nSize)
{
	int		ret;
	size_t		inlen, outlen;
	xmlNodePtr	curOldNode;
	xmlChar*	pcContent = NULL;
	xmlChar*	pcTmpCont = NULL;
	
	if (pxmlc == NULL || pcXPath == NULL || pcBuf == NULL) {
		UnionUserErrLog("in UnionXMLGet:: pxmlc or pcXPath or pcBuf can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLGet:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	curOldNode = pxmlc->curNode;
	ret = UnionXMLLocate(pxmlc, pcXPath, nId);
	if (ret < 0)
	{
		if (ret != errCodeObjectMDL_FieldNotExist)
			UnionUserErrLog("in UnionXMLGet:: UnionXMLLocate[%s]\n",pcXPath);
		return(ret);
	}
	pcContent = xmlNodeGetContent(pxmlc->curNode);
	if (pcContent == NULL) {
		UnionUserErrLog("in UnionXMLGet:: get null content.\n");
		// 2012-12-17 add
		pxmlc->curNode = curOldNode;
		return (errCodeParameter);
	}

	//if (pxmlc->doc->charset == XML_CHAR_ENCODING_UTF8) {
	//if (strcmp(pgencoding,"UTF-8") != 0) {
	if (strcmp((char *)pxmlc->doc->encoding,"UTF-8") != 0) {
		inlen = xmlStrlen(pcContent);
		outlen = inlen * 2 + 1;
		pcTmpCont = (xmlChar*)xmlMalloc((outlen+1) * sizeof(xmlChar));
		ret = UnionXMLCodeConvert("UTF-8",(char *)pxmlc->doc->encoding,(char *)pcContent, &inlen,(char *)pcTmpCont, &outlen);
		if (ret < 0) {
			UnionUserErrLog("in UnionXMLGet:: UnionXMLCodeConvert error!\n");
			xmlFree(pcContent);
			xmlFree(pcTmpCont);
			// 2012-12-17 add
			pxmlc->curNode = curOldNode;
			return (ret);
		}
		pcTmpCont[outlen] = 0;
		xmlFree(pcContent);
		pcContent = pcTmpCont;
	}
	/* 
	else if (pxmlc->doc->charset != XML_CHAR_ENCODING_8859_1) {
		UnionUserErrLog("in UnionXMLGet:: illegal charset[%d].\n", pxmlc->doc->charset);
		xmlFree(pcContent);
		return (-1);
	}
	*/
	if (xmlStrlen(pcContent)+1 > nSize) {
		UnionUserErrLog("in UnionXMLGet:: [%d] + 1 > [%d],not enough buffer.\n",xmlStrlen(pcContent),nSize);
		// 2012-12-17 add
		pxmlc->curNode = curOldNode;
		xmlFree(pcContent);
		return (errCodeSmallBuffer);
	}
	// 2012-12-17 add
	pxmlc->curNode = curOldNode;

	strcpy(pcBuf, (char*) pcContent);
	xmlFree(pcContent);
	return(strlen(pcBuf));
}

/*
** UnionXMLSelectGet:: get the content of the node directed by @pcXPath
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
int UnionXMLSelectGet(PUnionXMLCtxt pxmlc, const char* pcXPath, int nId, char* pcBuf, int nSize)
{
	int		ret;
	size_t		inlen, outlen;
	xmlNodePtr	curOldNode;
	xmlChar*	pcContent;
	xmlChar*	pcTmpCont = NULL;
	
	if (pxmlc == NULL || pcXPath == NULL || pcBuf == NULL) {
		UnionUserErrLog("in UnionXMLSelectGet:: pxmlc or pcXPath or pcBuf can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLSelectGet:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	curOldNode = pxmlc->curNode;
	ret = UnionXMLSelect(pxmlc, pcXPath, nId);
	if (ret < 0)
		return ret;
	else if (ret == 0)
		return 0;
	
	pcContent = xmlNodeGetContent(pxmlc->curNode);
	if (pcContent == NULL) {
		UnionUserErrLog("in UnionXMLSelectGet:: get null content.\n");
		return (errCodeParameter);
	}
	if (strcmp((char *)pxmlc->doc->encoding,"UTF-8") != 0) {
		inlen = xmlStrlen(pcContent);
		outlen = inlen * 2 + 1;
		pcTmpCont = (xmlChar*)xmlMalloc((outlen+1) * sizeof(xmlChar));
		ret = UnionXMLCodeConvert("UTF-8",(char *)pxmlc->doc->encoding,(char *)pcContent, &inlen,(char *)pcTmpCont, &outlen);
		if (ret < 0) {
			UnionUserErrLog("in UnionXMLGet:: UnionXMLCodeConvert error!\n");
			xmlFree(pcContent);
			xmlFree(pcTmpCont);
			return (ret);
		}
		pcTmpCont[outlen] = 0;
		xmlFree(pcContent);
		pcContent = pcTmpCont;
	}
	if (xmlStrlen(pcContent)+1 > nSize) {
		UnionUserErrLog("in UnionXMLSelectGet:: not enough buffer.\n");
		// 2012-12-17 add
		pxmlc->curNode = curOldNode;
		xmlFree(pcContent);
		return (errCodeSmallBuffer);
	}
	// 2012-12-17 add
	pxmlc->curNode = curOldNode;

	strcpy(pcBuf, (char*) pcContent);
	xmlFree(pcContent);
	return(strlen(pcBuf));
}

/*
** UnionXMLIsRightRoot: compare the name of the root element of @doc with @root.
**
** Return values: 1, if true; 0, if false.
*/
static int UnionXMLIsRightRoot(xmlDocPtr doc, const char* root)
{
	xmlNodePtr	cur = NULL;
	
	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		UnionUserErrLog("in UnionXMLIsRightRoot:: cannot find root element!\n");
		return (errCodeParameter);
	}
	return (! xmlStrcmp(cur->name, BAD_CAST root));
}

/*
** UnionXMLSearchChild: search a child of @parent, with the same name as @name
*/
//static xmlNodePtr UnionXMLSearchChild(xmlNodePtr parent, const char* name, unsigned int nId)
xmlNodePtr UnionXMLSearchChild(xmlNodePtr parent, const char* name, unsigned int nId)
{
	xmlNodePtr	cur = NULL;
	xmlChar*	pxc;
	char		sId[64];
	
	sprintf(sId, "%d", nId);
	cur = parent->children;
	while (cur) {
		if (xmlStrcmp(cur->name, BAD_CAST name) == 0) {
			if (nId == 0) return cur;
			pxc = xmlGetProp(cur, BAD_CAST UnionXMLS_ATTR_ID);
			if (pxc == NULL && nId == 1) {
				xmlFree(pxc);
				return cur;
			} else if (xmlStrcmp(pxc, BAD_CAST sId) == 0) {
				xmlFree(pxc);
				return cur;
			}
			xmlFree(pxc);
		}
		cur = cur->next;
	}
	
	return cur;
}
	                                                                                  
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
int UnionXMLPut(PUnionXMLCtxt pxmlc, const char* pcXPath, char* pcContent, unsigned int nId)
{
	int		ret;
	size_t		inlen, outlen;
	char*		s;
	xmlNodePtr	curParent, curChild;
	char*		pcTmpPath;
	xmlChar*	pcTmpCont = NULL;
	
	if (pxmlc == NULL || pcXPath == NULL) {
		UnionUserErrLog("in UnionXMLPut:: pxmlc or pcXPath can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLPut:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	pcTmpPath = (char*) xmlStrdup(BAD_CAST pcXPath);
	if (pcTmpPath == NULL) {
		UnionUserErrLog("in UnionXMLPut:: xmlStrdup fail.\n");
		return (errCodeParameter);
	}
	
	if (UnionXMLIsAbsolutePath(pcXPath)) {
		s = strtok(pcTmpPath+1, UnionXMLS_PATH_SEP);
		if (s == NULL) {
			UnionUserErrLog("in UnionXMLPut[%s]:: NULL Path.\n", pcXPath);
			xmlFree((xmlChar*)pcTmpPath);
			return (errCodeParameter);
		}
		curParent = xmlDocGetRootElement(pxmlc->doc);
		ret = UnionXMLIsRightRoot(pxmlc->doc, s);
		if (ret < 0) {
			xmlFree((xmlChar*)pcTmpPath);
			return ret;
		} else if (ret == 0) {
			UnionUserErrLog("in UnionXMLPut[%s]:: root path[%s] error!", pcXPath, s);
			xmlFree((xmlChar*)pcTmpPath);
			return (errCodeParameter);
		}
		s = strtok(NULL, UnionXMLS_PATH_SEP);
	} else {
		curParent = pxmlc->curNode;
		s = strtok(pcTmpPath, UnionXMLS_PATH_SEP);
	}
	while (s) {
		curChild = UnionXMLSearchChild(curParent, s, 0);
		if (curChild == NULL) { 
			curChild = xmlNewTextChild(curParent, NULL, BAD_CAST s, NULL);    
			if (curChild == NULL) {                                       
				UnionUserErrLog("in UnionXMLPut:: cannot new child[%s]", s);         	
				xmlFree((xmlChar*)pcTmpPath);                         
				return (errCodeObjectMDL_FieldNotExist);                                          
			}                                                             
		}                                                                     
		curParent = curChild;                                                 
		s = strtok(NULL, UnionXMLS_PATH_SEP);                                      
	}                                                                             
	if (nId > 0) {
		curChild = curParent;
		curParent = curChild->parent;
		curParent = UnionXMLSearchChild(curParent, (const char*)curChild->name, nId);
		if (curParent == NULL) {
			curParent = curChild->parent;
			curChild = xmlNewTextChild(curParent, NULL, curChild->name, NULL);
			if (curChild == NULL) {
				UnionUserErrLog("in UnionXMLPut[2]:: cannot new child[%s]", curChild->name);
				xmlFree((xmlChar*)pcTmpPath);                         
				return (errCodeObjectMDL_FieldNotExist);                                          
			}
			curParent = curChild;
		}
	}

	xmlFree((xmlChar*)pcTmpPath);
	if (pcContent && (strcmp((char *)pxmlc->doc->encoding,"UTF-8") != 0))
	//if (pcContent && pxmlc->doc->charset == XML_CHAR_ENCODING_UTF8)
	{
		inlen = strlen(pcContent);
		outlen = inlen * 2 + 1;
		pcTmpCont = (xmlChar*)xmlMalloc((outlen+1) * sizeof(xmlChar));
		ret = UnionXMLCodeConvert((char *)pxmlc->doc->encoding,"UTF-8",(char*)pcContent, &inlen,(char *)pcTmpCont, &outlen);
		if (ret < 0) {
			UnionUserErrLog("in UnionXMLPut:: UnionXMLCodeConvert fail[%s].\n",(unsigned char*)pcContent);
			xmlFree(pcTmpCont);
			return (ret);
		}
		pcTmpCont[outlen] = 0;
		xmlNodeSetContent(curParent, pcTmpCont);
		xmlFree(pcTmpCont);  
	} else if (pcContent) {
		xmlNodeSetContent(curParent, BAD_CAST pcContent);                     
	}

	if (nId > 0) {
		char	sId[64];

		sprintf(sId, "%d", nId);
		if (xmlSetProp(curParent,BAD_CAST UnionXMLS_ATTR_ID,BAD_CAST sId)==NULL) {
			UnionUserErrLog("in UnionXMLPut:: set prop[%s=%s] fail.\n", UnionXMLS_ATTR_ID, sId);
			return (errCodeParameter);                                                  
		}                                                                     
	}

	return (0);
}

/*
** UnionXMLSetId:: set current node's ID attribute
**
** @pxmlc: UnionXML Context pointer
** @nId: The index attribute of the node.
**
*/
int UnionXMLSetId(PUnionXMLCtxt pxmlc, int nId)
{
	if (pxmlc == NULL) {
		UnionUserErrLog("in UnionXMLSetId:: pxmlc can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLSetId:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	if (nId >= 0) {
		char	sId[64];

		sprintf(sId, "%d", nId);
		if (xmlSetProp(pxmlc->curNode, BAD_CAST UnionXMLS_ATTR_ID,BAD_CAST sId)==NULL)
		{
			UnionUserErrLog("in UnionXMLSetId:: set prop[%s=%s] fail.\n", UnionXMLS_ATTR_ID, sId);
			return (errCodeParameter);                                                  
		}
	}

	return (0);
}

/*
** UnionXMLExchgId:: Exchange the ids of two nodes with same path.
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
int UnionXMLExchgId(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId1, unsigned int nId2)
{
	int			ret;
	char*		s;
	xmlNodePtr	curParent, curChild;
	xmlNodePtr	curNode1, curNode2;
	char*		pcTmpPath;
	char		sId[64];
	
	if (pxmlc == NULL || pcXPath == NULL) {
		UnionUserErrLog("in UnionXMLExchgId:: pxmlc or pcXPath can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLExchgId:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	if (nId1 == 0 || nId2 == 0) {
		UnionUserErrLog("in UnionXMLExchgId:: nId1 or nId2 is equal to zero.\n");
		return (errCodeParameter);
	}
	if (nId1 == nId2) return 0;
	
	pcTmpPath = (char*) xmlStrdup(BAD_CAST pcXPath);
	if (pcTmpPath == NULL) {
		UnionUserErrLog("in UnionXMLExchgId:: xmlStrdup fail.\n");
		return (errCodeParameter);
	}
	
	if (UnionXMLIsAbsolutePath(pcXPath)) {
		s = strtok(pcTmpPath+1, UnionXMLS_PATH_SEP);
		if (s == NULL) {
			UnionUserErrLog("in UnionXMLExchgId[%s]: NULL Path.\n", pcXPath);
			xmlFree((xmlChar*)pcTmpPath);
			return (errCodeParameter);
		}
		curParent = xmlDocGetRootElement(pxmlc->doc);
		ret = UnionXMLIsRightRoot(pxmlc->doc, s);
		if (ret < 0) {
			xmlFree((xmlChar*)pcTmpPath);
			return ret;
		} else if (ret == 0) {
			UnionUserErrLog("in UnionXMLExchgId[%s]: root path[%s] error!", pcXPath, s);
			xmlFree((xmlChar*)pcTmpPath);
			return (errCodeParameter);
		}
		s = strtok(NULL, UnionXMLS_PATH_SEP);
	} else {
		curParent = pxmlc->curNode;
		s = strtok(pcTmpPath, UnionXMLS_PATH_SEP);
	}
	while (s) {
		curChild = UnionXMLSearchChild(curParent, s, 0);
		if (curChild == NULL) { 
			curChild = xmlNewTextChild(curParent, NULL, BAD_CAST s, NULL);    
			if (curChild == NULL) {                                       
				UnionUserErrLog("in UnionXMLExchgId:: cannot new child[%s]", s);         	
				xmlFree((xmlChar*)pcTmpPath);                         
				return (errCodeParameter);                                          
			}                                                             
		}                                                                     
		curParent = curChild;                                                 
		s = strtok(NULL, UnionXMLS_PATH_SEP);                                      
	}

	xmlFree((xmlChar*)pcTmpPath);
	curChild = curParent;
	curParent = curChild->parent;
	curNode1 =  UnionXMLSearchChild(curParent, (const char*)curChild->name, nId1);
	if (curNode1 == NULL) {
		UnionUserErrLog("in UnionXMLExchgId:: Cannot find node by %d.\n", nId1);
		return (errCodeParameter);
	}
	curNode2 =  UnionXMLSearchChild(curParent, (const char*)curChild->name, nId2);
	if (curNode2 == NULL) {
		UnionUserErrLog("in UnionXMLExchgId:: Cannot find node by %d.\n", nId2);
		return (errCodeParameter);
	}

	sprintf(sId, "%d", nId2);
	if (xmlSetProp(curNode1,BAD_CAST UnionXMLS_ATTR_ID,BAD_CAST sId)==NULL) {
		UnionUserErrLog("in UnionXMLExchgId:: set prop[%s=%s] fail.\n", UnionXMLS_ATTR_ID, sId);
		return (errCodeParameter);                                                  
	}

	sprintf(sId, "%d", nId1);
	if (xmlSetProp(curNode2,BAD_CAST UnionXMLS_ATTR_ID,BAD_CAST sId)==NULL) {
		UnionUserErrLog("in UnionXMLExchgId:: set prop[%s=%s] fail.\n", UnionXMLS_ATTR_ID, sId);
		return (errCodeParameter);                                                  
	}

	return (0);
}

/*
** UnionXMLRevIds:: Reverse the ids of two nodes between @nFirstId and @nLastId.
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
int UnionXMLRevIds(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nFirstId, unsigned int nLastId)
{
	int			ret;
	int			nLId, nGId;
	int			i;

	if (pxmlc == NULL || pcXPath == NULL) {
		UnionUserErrLog("in UnionXMLRevIds:: pxmlc or pcXPath can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLRevIds:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	if (nFirstId == 0 || nLastId == 0) {
		UnionUserErrLog("in UnionXMLRevIds:: nFirstId or nLastId is equal to zero.\n");
		return (errCodeParameter);
	}
	if (nLastId == nFirstId) 
		return 0;

	if (nFirstId < nLastId) {
		nLId = nFirstId;
		nGId = nLastId;
	} else {
		nLId = nLastId;
		nGId = nFirstId;
	}
	
	for (i = 0; i < (nGId + 1 - nLId) / 2; i ++) {
		ret = UnionXMLExchgId(pxmlc, pcXPath, nLId+i, nGId-i);
		if (ret < 0) 
			return ret;
	}
	
	return 0;
}

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
int UnionXMLLocateNew(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId)
{
	int	ret;
	
	ret = UnionXMLPut(pxmlc, pcXPath, NULL, nId);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionXMLLocateNew:: UnionXMLPut\n");
		return ret;
	}
	
	return (UnionXMLLocate(pxmlc, pcXPath, nId));
}

int UnionXMLLocateNewEx(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId)
{
	int	ret;
	
	ret = UnionXMLPut(pxmlc, pcXPath, NULL, nId);
	if (ret < 0) return ret;
	
	ret = UnionXMLLocate(pxmlc, pcXPath, nId);
	if (ret < 0) return ret;

	if(nId == 0){
		ret = UnionXMLSetId(pxmlc, nId);
		if (ret < 0) return ret;
	}

	return 0;
}

//代码转换:从一种编码转为另一种编码
int UnionXMLCodeConvert(char *from_charset,char *to_charset,char *inbuf,size_t *inlen,char *outbuf,size_t *outlen)
{
	/*
	int				ret;
	xmlBufferPtr			in,out; 
	xmlCharEncodingHandlerPtr	handler;
	if (strcmp(from_charset,"UTF-8") == 0)
	{
		handler = xmlFindCharEncodingHandler(to_charset);
		if (!handler) 
		{
			UnionUserErrLog("in UnionXMLCodeConvert:: handler is NULL!\n");
			return(errCodeParameter);
		}
		in = xmlBufferCreate();
		xmlBufferWriteChar(in,inbuf); 
		out = xmlBufferCreate(); 
		if((ret = xmlCharEncOutFunc(handler,out,in)) < 0)
		{ 
			xmlBufferFree(in); 
			xmlBufferFree(out); 
			return(ret); 
		}
		else
		{
			memcpy(outbuf,out->content,out->use);
			*outlen = out->use;
			xmlBufferFree(in);
			xmlBufferFree(out);
			return(*outlen);
		}
	}
	else
	{
		handler = xmlFindCharEncodingHandler(from_charset);
		if (!handler) 
		{
			UnionUserErrLog("in UnionXMLCodeConvert:: handler is NULL!\n");
			return(errCodeParameter);
		}
		in = xmlBufferCreate();
		xmlBufferWriteChar(in,inbuf); 
		out = xmlBufferCreate(); 
		if((ret = xmlCharEncInFunc(handler,out,in)) < 0)
		{ 
			xmlBufferFree(in); 
			xmlBufferFree(out); 
			return(ret); 
		}
		else
		{
			memcpy(outbuf,out->content,out->use);
			*outlen = out->use;
			xmlBufferFree(in);
			xmlBufferFree(out);
			return(*outlen);
		}
	}
	*/
	iconv_t		cd;
	size_t		icv_inlen, icv_outlen;
	char		**pin = &inbuf;
	char		**pout = &outbuf;

	icv_inlen = *inlen;
	icv_outlen = *outlen;
	
	cd = iconv_open(to_charset,from_charset);
	if (cd == 0)
	{
		UnionUserErrLog("in UnionXMLCodeConvert:: iconv_open!\n");
		return(errCodeParameter);
	}

	memset(outbuf,0,*outlen);
	iconv(cd, pin, &icv_inlen, pout, &icv_outlen);

	*inlen = icv_inlen;

	iconv_close(cd);
	return 0;
}

int UnionXMLLog(PUnionXMLCtxt pxmlc)
{
	int		ret;
	char		tmpBuf[8192+1];
	
	if (pxmlc == NULL)
	{
		UnionUserErrLog("in UnionXMLLog:: pxmlc can not be null.\n");
		return(errCodeParameter);
	}
	
	UnionXMLSetDumpFmt(pxmlc,1);
	if ((ret = UnionXMLDumpToBuf(pxmlc,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionNullLog("长度大于[%zu],不打印日志\n",sizeof(tmpBuf)-1);
		return(0);
	}
	tmpBuf[ret] = 0;
	UnionNullLog("[%d][%s]\n",ret,tmpBuf);
	
	return(0);
}

// 删除节点
int UnionXMLDelete(PUnionXMLCtxt pxmlc, const char* pcXPath, int nId)
{
	int		ret;
	xmlNodePtr	curOldNode;
	//xmlNodePtr	tempNode;
	
	if (pxmlc == NULL || pcXPath == NULL) {
		UnionUserErrLog("in UnionXMLDelete:: pxmlc or pcXPath can not be null.\n");
		return (errCodeParameter);
	}
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLDelete:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	curOldNode = pxmlc->curNode;
	ret = UnionXMLLocate(pxmlc, pcXPath, nId);
	if (ret < 0)
	{
		UnionLog("in UnionXMLDelete:: UnionXMLLocate[%s]\n",pcXPath);
		return(0);
	}
	//tempNode = pxmlc->curNode->next;

	xmlUnlinkNode(pxmlc->curNode);

	xmlFreeNode(pxmlc->curNode);

	pxmlc->curNode = curOldNode;
	
	return(0);
}


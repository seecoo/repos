#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionMXML.h"

/* the separator of UnionXML PATH string */
#define UnionXMLS_PATH_SEP		'/'

/* the name of ID attribute */
#define UnionXMLS_ATTR_ID		"ID"

#define UnionXMLIsAbsolutePath(a)	(a[0]=='/')

#define DEFAULT_CB whitespace_cb

#define WIN_NEWLINE		"\r\n" 

const char *whitespace_cb(mxml_cache_t *mxml_cache, mxml_node_t *node,int where)
{
	mxml_node_t     *parent = NULL;	 /* Parent node */
	int	     level;		  /* Indentation level */
	const char      *name = NULL;		   /* Name of element */
	static const char *strLv[6] = {"\n","\n  ","\n    ","\n      ","\n	"};

	name = mxml_cache->buffer + node->value.element.name_offset;

	if (!strncmp(name, "?xml", 4))
	{
		return(NULL);
	}
	else if((where==MXML_WS_BEFORE_CLOSE)&&
		(node->child)&&
		(node->child->type == MXML_OPAQUE)&&
//		(mxml_cache->buffer + node->child->value.opaque_offset)&&
		(node->child->value.opaque_offset)&&
		(node->child == node->last_child))
	{
		return (NULL);
	}
	else if (where == MXML_WS_BEFORE_OPEN||where == MXML_WS_BEFORE_CLOSE)
	{
		for (level = -1, parent = node->parent;parent;level ++, parent = parent->parent);
		if (level > 6)
			level = 6;
		else if (level < 0)
			level = 0;
		return (strLv[level]);
	}
	return (NULL);
}

//xml错误日志函数
void xmlErrLog(const char *msg)
{
        UnionUserErrLog("%s\n",(char *)msg);
}

void UnionXMLInitParser()
{
	return;
}

void UnionXMLCleanupParser()
{
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
		pxmlc->doc=NULL;
	
	if (pxmlc->xmlCache)
	{
		mxml_free_cache(pxmlc->xmlCache);
		pxmlc->xmlCache = NULL;
	}
	return;
}

/*
** UnionXMLReset: reset the all the related memory for UnionXML context.
*/
void UnionXMLReset(PUnionXMLCtxt pxmlc)
{
	if (pxmlc == NULL)
		return;
	if (pxmlc->doc)
		pxmlc->doc=NULL;
	mxml_reset_cache(pxmlc->xmlCache);
	return;
}

/*
** UnionXMLInit:: init the UnionXML context with a root element.
** 
** @pxmlc: The caller must declare the UnionXML Context, pointed by pxmlc.
** @rootname: The caller must specify the name of the root element, 
**	    which can not be NULL.
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
		
	if (pxmlc->xmlCache == NULL)
		pxmlc->xmlCache = mxml_new_cache();

	pxmlc->xmlCache->isNewXML = 1;

	mxmlSetErrorCallback(xmlErrLog);
	pxmlc->doc = mxmlNewXML(pxmlc->xmlCache,"1.0",encoding);
	
	snprintf(pxmlc->encoding,sizeof(pxmlc->encoding),"%s",encoding);
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLInit:: mxmlNewXML eror.\n");
		return (errCodeParameter);
	}

	pxmlc->curNode = mxmlNewElement(pxmlc->xmlCache,pxmlc->doc,(char*)rootname);
	if (pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLInit:: mxmlNewXML root element error.\n");
		UnionXMLReset(pxmlc);
		return (errCodeParameter);
	}

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
	
	if (pxmlc->xmlCache == NULL)
		pxmlc->xmlCache = mxml_new_cache();

	pxmlc->xmlCache->isNewXML = 0;

	mxmlSetErrorCallback(xmlErrLog);
	pxmlc->doc = mxmlLoadString(pxmlc->xmlCache,NULL,buf,MXML_OPAQUE_CALLBACK);
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLInitFromBuf:: mxmlLoadString failed.\n");
		return(errCodeParameter);
	}
	
	pxmlc->curNode = pxmlc->doc->last_child;	
	if (pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLInitFromBuf:: mxmlLoadString failed. curNode is NULL\n");
		return (errCodeParameter);
	}
	memcpy((char *)pxmlc->encoding,"GBK",3);
	pxmlc->encoding[3] = 0;
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
	//FILE	*fp = NULL;
	int	fd;
	
	if (pxmlc == NULL) {
		UnionUserErrLog("in UnionXMLInitFromFile:: pxmlc can not be NULL.\n");
		return (errCodeParameter);
	}

	if (pxmlc->xmlCache == NULL)
		pxmlc->xmlCache = mxml_new_cache();

	pxmlc->xmlCache->isNewXML = 0;

	mxmlSetErrorCallback(xmlErrLog);
	/*
	fp = fopen(fn,"rb");
	if(!fp){
		UnionUserErrLog("in UnionXMLInitFromFile:: open file failed.\n");
		return (errCodeParameter);
	}
	*/
	fd = open(fn,O_RDONLY);
	if (fd < 0)
	{
		UnionUserErrLog("in UnionXMLInitFromFile:: open file failed.\n");
		return (errCodeParameter);
	}
	pxmlc->doc = mxmlLoadFd(pxmlc->xmlCache,NULL,fd,MXML_OPAQUE_CALLBACK);
	if (pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLInitFromFile:: mxmlLoadFd failed.\n");
		//fclose(fp);
		close(fd);
		return (errCodeParameter);
	}
	//fclose(fp);
	close(fd);
	
	pxmlc->curNode = pxmlc->doc->last_child;	
	if (pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLInitFromFile:: mxmlLoadFd failed. curNode is NULL\n");
		return (errCodeParameter);
	}
	memcpy((char *)pxmlc->encoding,"GBK",3);
	pxmlc->encoding[3] = 0;
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
**	 of the buffer pointed by @pBuf, will be changed to the actual
**	 length of the string after dump.
** 
** Return values: 0, success; -1, failure.
*/
int UnionXMLDumpToBuf(PUnionXMLCtxt pxmlc, char* pBuf, int pSize)
{
	int	ret = 0;
	
	if (pxmlc == NULL || pBuf == NULL || pxmlc->doc == NULL) 
	{
		UnionUserErrLog("in UnionXMLDumpToBuf:: pxmlc or pBuf or doccan not be null.\n");
		return (errCodeParameter);
	}

	if(pxmlc->nIsFmt==0)
		ret = mxmlSaveString(pxmlc->xmlCache,pxmlc->doc,pBuf,pSize,NULL);
	else
		ret = mxmlSaveString(pxmlc->xmlCache,pxmlc->doc,pBuf,pSize,DEFAULT_CB); 
	if (ret < 0) 
	{
		UnionUserErrLog("in UnionXMLDumpToBuf:: mxmlSaveString error!\n");
		return(ret);
	}
	
	if (ret > pSize-1) {
		//UnionUserErrLog("in UnionXMLDumpToBuf:: sizeOf error!\n");
		return(errCodeSmallBuffer);
	}
	return(ret);
}

/*
** UnionXMLDumpToTmpBuf:: Dump UnionXML to a xmlChar buffer, return to caller
**
** @pxmlc: The UnionXML Context, pointed by @pxmlc, to be dumped.
** @ppBuf: will be set to the address of the xmlChar buffer.
** @pSize:  will be set to the actual length of the xmlChar buffer
** 
** Return values: 0, success; -1, failure.
**
** NOTE: The caller must call UnionFreeDumpTmpBuf to free memory,
**       pointed by (*ppBuf)
*/
int UnionXMLDumpToTmpBuf(PUnionXMLCtxt pxmlc, char *ppBuf, int pSize)
{
	int		ret = 0;
	
	if (pxmlc == NULL || ppBuf == NULL || pxmlc->doc == NULL) 
	{
		UnionUserErrLog("in UnionXMLDumpToTmpBuf:: pxmlc or ppBuf or doccan not be null.\n");
		return (errCodeParameter);
	}
	
	if(pxmlc->nIsFmt==0)
		ret = mxmlSaveString(pxmlc->xmlCache,pxmlc->doc,ppBuf,pSize,NULL);
	else
		ret = mxmlSaveString(pxmlc->xmlCache,pxmlc->doc,ppBuf,pSize,DEFAULT_CB); 
	if (ret < 0) 
	{
		UnionUserErrLog("in UnionXMLDumpToTmpBuf:: mxmlSaveString error!\n");
		return(ret);
	}
	
	if (ret > pSize-1) {
		UnionUserErrLog("in UnionXMLDumpToTmpBuf:: sizeOf error!\n");
		return(errCodeSmallBuffer);
	}
	return(ret);
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
	FILE	*fp = NULL;
	//char	tmpFn[128];
	//char	cmd[512];
	//struct	stat	fileInfo;
	//int	tmp;
	
	if (pxmlc == NULL || fn == NULL || pxmlc->doc == NULL) {
		UnionUserErrLog("in UnionXMLDumpToFile::  pxmlc or fn or doc can not be null.\n");
		return (errCodeParameter);
	}
	
	if ((fp = fopen(fn,"w")) == NULL)
	{
		UnionUserErrLog("in UnionXMLDumpToFile::  fopen[%s] error!\n",fn);
		return (errCodeUseOSErrCode);
	}
	
	if(pxmlc->nIsFmt == 0)
		ret = mxmlSaveFile(pxmlc->xmlCache,pxmlc->doc,fp,NULL);
	else
		ret = mxmlSaveFile(pxmlc->xmlCache,pxmlc->doc,fp,DEFAULT_CB);
	fflush(fp);
	fclose(fp);
	
	if (ret < 0)
	{
		UnionUserErrLog("in UnionXMLDumpToFile::  mxmlSaveFile error!\n");
		return (ret);
	}
	
	/*
	if(pxmlc->nIsFmt == 1)
	{
		memset(tmpFn,0,sizeof(tmpFn));
		sprintf(tmpFn,"%s.tmp",fn);
		memset(cmd,0,sizeof(cmd));
		sprintf(cmd,"xmllint --format %s > %s",fn,tmpFn);
		system(cmd);
		
		// 检查tmpFn是否存在，并且大小大于0
		if (access(tmpFn,0) >= 0)
		{
			if ((tmp = stat(tmpFn,&fileInfo)) >= 0)
			{
				if (fileInfo.st_size > 0)	
					rename(tmpFn,fn);
			}
			else
				remove(tmpFn);
		}
	}
	*/
	return(ret);
}


/*
** UnionXMLSelect:: Select and Locate the UnionXML Context on the node 
**	     directed by a node path according to the flag.
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
	mxml_node_t		*pNodeNow = NULL;
	char			*ptr1 = NULL;
	char			*ptr2 = NULL;
	char			*pPathEnd = NULL;
	char			tmpSubPath[64];
	int			nSubPathLen=0;
	
	if (pxmlc == NULL || pcXPath == NULL) {
		UnionUserErrLog("in UnionXMLSelect:: pxmlc or pcXPath can not be null.\n");
		return (errCodeParameter);
	}
	
	pPathEnd = (char*)pcXPath + strlen((char*)pcXPath) - 1;
	
	if (pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLSelect:: doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	if (UnionXMLIsAbsolutePath(pcXPath)) {
		pNodeNow = pxmlc->doc;
		ptr1 = (char*)pcXPath+1;
		ptr2 = (char*)pcXPath+1;
	} else {
		pNodeNow = pxmlc->curNode;
		ptr1 = (char*)pcXPath;
		ptr2 = (char*)pcXPath;
	}
	ptr2 = memchr(ptr1,'/',strlen(ptr1));
	while(ptr2){
		nSubPathLen = ptr2-ptr1;
		memcpy(tmpSubPath,ptr1,nSubPathLen);
		tmpSubPath[nSubPathLen] = '\0';
		pNodeNow = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,tmpSubPath,NULL,NULL,MXML_DESCEND_FIRST);
		if(!pNodeNow){
			UnionUserErrLog("in UnionXMLSelect:: doc or curNode cannot be null.\n");
	 		return (errCodeParameter);
		}
		ptr2++;
		ptr1 = ptr2;
		ptr2 = memchr(ptr1,'/',strlen(ptr1));
	}
	if(*pPathEnd == '/'){
		pPathEnd--;
	}
	nSubPathLen = pPathEnd-ptr1+1;
	memcpy(tmpSubPath,ptr1,nSubPathLen);
	tmpSubPath[nSubPathLen] = '\0';
	if(nId == 0){
		pNodeNow = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,tmpSubPath,NULL,NULL,MXML_DESCEND_FIRST);
		if(!pNodeNow){
			UnionUserErrLog("in UnionXMLSelect:: doc or curNode cannot be null.\n");
			return (errCodeParameter);
		}
	}else{
		char sId[32];
		snprintf(sId,sizeof(sId),"%d",nId);
		pNodeNow = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,tmpSubPath,UnionXMLS_ATTR_ID,sId,MXML_DESCEND_FIRST);
		if(!pNodeNow){
			UnionUserErrLog("in UnionXMLSelect:: doc or curNode cannot be null.\n");
			return (errCodeParameter);
		}
	}
	pxmlc->curNode = pNodeNow;
	return 1;
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
	//int		i = 0,j = 0;
	//int		fldNum = 0;
	//char		newPath[128];
	//char		pathGrp[10][128];
	char		tmpPath[128];
	char		*ptr = NULL;
	char		sId[32];
	mxml_node_t 	*searchNode = NULL;
	mxml_node_t 	*pNodeNow = NULL;
	mxml_node_t 	*pNodeLeaf = NULL;
	
	if (pxmlc == NULL || pcXPath == NULL || pxmlc->doc == NULL || pxmlc->curNode == NULL)
	{
		UnionUserErrLog("in UnionXMLLocate:: pxmlc or pcXPath or doc or curNode can not be null.\n");
		return (errCodeParameter);
	}
	
	if (UnionXMLIsAbsolutePath(pcXPath))
		searchNode = pxmlc->doc;
	else
		searchNode = pxmlc->curNode;
	
	if (nId > 0)
	{
		snprintf(tmpPath,sizeof(tmpPath),"%s",pcXPath);
		if ((ptr = strrchr(tmpPath,'/')) != NULL)
		{
			*ptr = 0;
			pNodeNow = mxmlFindPath(pxmlc->xmlCache,searchNode,tmpPath,MXML_PATH_NO);
			
			if (pNodeNow == NULL)
				return(errCodeObjectMDL_FieldNotExist);
			ptr ++;
		}
		else
		{
			pNodeNow = searchNode;
			ptr = tmpPath;
		}
		
		snprintf(sId,sizeof(sId),"%d",nId);
		pNodeLeaf = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,ptr,UnionXMLS_ATTR_ID,sId,MXML_DESCEND_FIRST);
	}
	else
	{
		pNodeLeaf = mxmlFindPath(pxmlc->xmlCache,searchNode,pcXPath,MXML_PATH_NO);
	}
	
	if (pNodeLeaf == NULL)
	{
		//UnionDebugLog("in UnionXMLLocate:: nodes[%s] is not found!\n", pcXPath);	
		return(errCodeObjectMDL_FieldNotExist);
	}
	
	pxmlc->curNode = pNodeLeaf;
	return(0);
	
	/*
	//memset(newPath,0,sizeof(newPath));
	//for (i = 0; i < 2; i++)
	{	
		if (i == 0)
			strcpy(newPath,pcXPath);
		else
			UnionToLowerCase(newPath);

		memset(&pathGrp,0,sizeof(pathGrp));
		if (UnionXMLIsAbsolutePath(newPath))
		{
			fldNum = UnionSeprateVarStrIntoVarGrp((char *)newPath + 1,strlen(newPath) - 1,UnionXMLS_PATH_SEP,pathGrp,10);
			pNodeNow = pxmlc->doc;
		}
		else
		{
			fldNum = UnionSeprateVarStrIntoVarGrp((char *)newPath,strlen(newPath),UnionXMLS_PATH_SEP,pathGrp,10);
			pNodeNow = pxmlc->curNode;
		}
		
		for (j = 0; j < fldNum - 1; j++)
		{
			pNodeNow = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,pathGrp[j],NULL,NULL,MXML_DESCEND_FIRST);
			if(!pNodeNow)
			{
				UnionDebugLog("in UnionXMLLocate:: mxmlFindElement nodes[%s] is not found!\n", pcXPath);	    	
				return(errCodeObjectMDL_FieldNotExist);
			}
		}
		if(nId == 0)
		{
			if ((pNodeLeaf = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,pathGrp[fldNum - 1],NULL, NULL,MXML_DESCEND_FIRST)) == NULL)
			{
				//UnionUserErrLog("in UnionXMLLocate:: mxmlFindElement[%s]\n",pathGrp[fldNum - 1]);
				//continue;
				return(errCodeObjectMDL_FieldNotExist);
			}
		}
		else
		{
			char	sId[10] = {0};
			sprintf(sId,"%d",nId);
			pNodeLeaf = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,pathGrp[fldNum - 1],UnionXMLS_ATTR_ID,sId,MXML_DESCEND_FIRST);
			if(!pNodeLeaf)
			{
				//UnionUserErrLog("in UnionXMLLocate:: mxmlFindElement[%s]!\n",pathGrp[fldNum - 1]);
				//continue;
				return(errCodeObjectMDL_FieldNotExist);
			}
		}
		pxmlc->curNode = pNodeLeaf;
		return(0);
	}
	//UnionDebugLog("in UnionXMLLocate:: nodes[%s] is not found!\n", pcXPath);	    	
	return(errCodeObjectMDL_FieldNotExist);
	*/
	
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
	int		ret = 0;
	int		len;
	mxml_node_t	*curOldNode = NULL;
	const char	*pcContent = NULL;
	
	if (pxmlc == NULL || pcXPath == NULL || pxmlc->doc == NULL || pxmlc->curNode == NULL) 
	{
		UnionUserErrLog("in UnionXMLGet:: pxmlc or pcXPath or doc or curNode can not be null.\n");
		return (errCodeParameter);
	}
	
	curOldNode = pxmlc->curNode;
	if ((ret = UnionXMLLocate(pxmlc, pcXPath, nId)) < 0)
	{
		UnionLog("in UnionXMLGet:: UnionXMLLocate[%s]\n",pcXPath);
		pcBuf[0] = 0;
		return(ret);
	}
	
	pcContent = mxmlGetOpaque(pxmlc->xmlCache,pxmlc->curNode);
	if (pcContent == NULL)
	{
		pxmlc->curNode = curOldNode;
		pcBuf[0] = 0;
		return(0);
	}
	pxmlc->curNode = curOldNode;
	len = snprintf(pcBuf,nSize,"%s",(char*)pcContent);
	if (len >= nSize) 
	{
		UnionUserErrLog("in UnionXMLGet:: [%d] >= [%d],not enough buffer.\n",len,nSize);
		return(errCodeParameter);
	}
	return(len);
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
	int		len = 0;
	mxml_node_t*	curOldNode = NULL;
	mxml_node_t*	node = NULL;
	mxml_node_t*	tmpNode = NULL;
	const char	*pcContent = NULL;
	
	if (pxmlc == NULL || pcXPath == NULL || pxmlc->doc == NULL || pxmlc->curNode == NULL) 
	{
		UnionUserErrLog("in UnionXMLSelectGet:: pxmlc or pcXPath or doc or curNode can not be null.\n");
		return (errCodeParameter);
	}
	
	curOldNode = pxmlc->curNode;
	
	ret = UnionXMLLocate(pxmlc, pcXPath, nId);
	if (ret < 0)
	{
		//UnionUserErrLog("in UnionXMLSelectGet:: UnionXMLLocate[%s]\n",pcXPath);
		return(ret);
	}
	node = pxmlc->curNode;
	while(node)
	{
		tmpNode = mxmlWalkNext(node,pxmlc->curNode,MXML_DESCEND);
     		if ((pcContent = mxmlGetOpaque(pxmlc->xmlCache,tmpNode)) == NULL)
		{
			node = tmpNode;
			continue;
		}
		else
		{
			if (strlen(pcContent) >= (unsigned int)nSize - len) {
				UnionUserErrLog("in UnionXMLSelectGet:: [%zu] >= [%d],not enough buffer.\n",strlen(pcContent),nSize-len);
				pxmlc->curNode = curOldNode;
				return (errCodeObjectMDL_FieldValueTooLong);
			}
			len += snprintf(pcBuf + len,nSize - len,"%s",pcContent);
			node = tmpNode->child;			
		}
	}
	pxmlc->curNode = curOldNode;
	return(len);
}

/*
mxml_node_t* UnionXMLSearchChild(mxml_node_t* parent, const char* name, unsigned int nId)
{
	char		sId[64];

	if(parent==NULL||!name)
		return NULL;

	sprintf(sId, "%d", nId);
	if (nId == 0)
		return mxmlFindElement(pxmlc->xmlCache,parent,parent,name,NULL,NULL,MXML_DESCEND_FIRST);
	else
		return mxmlFindElement(pxmlc->xmlCache,parent,parent,name,UnionXMLS_ATTR_ID,sId,MXML_DESCEND_FIRST);
}
*/

int UnionXMLPut(PUnionXMLCtxt pxmlc, const char* pcXPath, char* pcContent, unsigned int nId)
{
	//int		i,j;
	int		len = 0;
	mxml_node_t	*searchNode = NULL;
	mxml_node_t	*pNodeNow = NULL;
	mxml_node_t	*pNodeTmp = NULL;
	char		tmpPath[128];
	char		*ptr = NULL;
	char		*tmp = NULL;
	char		*tmpContent = NULL;
	char		tmpBuf[81920];
	char		sId[32];

	if (pxmlc == NULL || pcXPath == NULL || pxmlc->doc == NULL || pxmlc->curNode == NULL) 
	{
		UnionUserErrLog("in UnionXMLPut:: pxmlc or pcXPath or doc or curNode can not be null.\n");
		return (errCodeParameter);
	}
	
	// 去掉换行符号
	if (pcContent)
	{
		ptr = pcContent;
		if ((tmp = strstr(ptr,WIN_NEWLINE)) == NULL)
			tmpContent = pcContent;
		else
		{
			tmpContent = tmpBuf;
			while(1)
			{
				len = tmp - ptr;
				memcpy(tmpContent,ptr,len);
				tmpContent[len] = '\n';
				tmpContent += len + 1;
				ptr = tmp + 2;
				tmp = strstr(ptr,WIN_NEWLINE);
				if (!tmp)
				{
					strcpy(tmpContent,ptr);
					break;
				}
			}
			tmpContent = tmpBuf;
		}
		/*
		tmpContent = tmpBuf;
		len = strlen(pcContent);
		for (i = 0,j = 0; i < len + 1; i++)
		{
			if (pcContent[i] == 0x0d)	// 回车
			{
				if (pcContent[i+1] == 0x0a)	// 换行
					i++;
				tmpContent[j] = '\n';
			}
			else
				tmpContent[j] = pcContent[i];
			j++;
		}
		*/
	}

	if (UnionXMLIsAbsolutePath(pcXPath))
		searchNode = pxmlc->doc;
	else
		searchNode = pxmlc->curNode;
	
	snprintf(tmpPath,sizeof(tmpPath),"%s",pcXPath);
	if (((ptr = strrchr(tmpPath,'/')) != NULL) && (ptr != tmpPath))
	{
		*ptr = 0;
		pNodeNow = mxmlFindPath(pxmlc->xmlCache,searchNode,tmpPath,MXML_PATH_NEW);
		if (pNodeNow == NULL)
			return(errCodeObjectMDL_FieldNotExist);
		ptr ++;
	}
	else
	{
		pNodeNow = searchNode;
		if (ptr)
			ptr ++;
		else
			ptr = tmpPath;
	}

	if (nId == 0)
	{
		pNodeTmp = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,ptr,NULL,NULL,MXML_DESCEND_FIRST);
		if(!pNodeTmp)
			pNodeTmp = mxmlNewElement(pxmlc->xmlCache,pNodeNow,ptr);
		pNodeNow = pNodeTmp;
	}
	else
	{

		snprintf(sId,sizeof(sId),"%d",nId);
		pNodeTmp = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,ptr,UnionXMLS_ATTR_ID,sId,MXML_DESCEND_FIRST);
		if(!pNodeTmp)
			pNodeTmp = mxmlNewElement(pxmlc->xmlCache,pNodeNow,ptr);
		pNodeNow = pNodeTmp;
		mxmlElementSetAttr(pxmlc->xmlCache,pNodeNow,UnionXMLS_ATTR_ID,sId);
	}

	if(!pNodeNow->last_child || pNodeNow->last_child->type != MXML_OPAQUE)
		mxmlNewOpaque(pxmlc->xmlCache,pNodeNow,tmpContent);
	else
		mxmlSetOpaque(pxmlc->xmlCache,pNodeNow->last_child,tmpContent);
	
	return(len);
}

int UnionXMLSetId(PUnionXMLCtxt pxmlc, int nId)
{
	if (pxmlc == NULL || pxmlc->doc == NULL || pxmlc->curNode == NULL) 
	{
		UnionUserErrLog("in UnionXMLSetId:: pxmlc or doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	if (nId >= 0) {
		char	sId[32];
		snprintf(sId,sizeof(sId),"%d",nId);
		mxmlElementSetAttr(pxmlc->xmlCache,pxmlc->curNode,UnionXMLS_ATTR_ID,sId);
	}
	return (0);
}

int UnionXMLExchgId(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId1, unsigned int nId2)
{
	mxml_node_t*	pNode1=NULL;
	mxml_node_t*	pNode2=NULL;
	mxml_node_t*	pOldNode=NULL;
	int		ret;
  	char		sId1[32];
	char		sId2[32];

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
	if (nId1 == nId2) 
		return 0;
	
	pOldNode = pxmlc->curNode;
	
	ret = UnionXMLLocate(pxmlc,pcXPath,nId1);
	if (ret < 0){
		UnionUserErrLog("in UnionXMLExchgId:: pcXPath[%s],Cannot find node by %d.\n",pcXPath, nId1);
		return (ret);
	}
	pNode1 = pxmlc->curNode;
	pxmlc->curNode = pOldNode;

	ret = UnionXMLLocate(pxmlc,pcXPath,nId2);
	if(ret < 0){
		UnionUserErrLog("in UnionXMLExchgId:: pcXPath[%s],Cannot find node by %d.\n",pcXPath, nId2);
		return (ret);
	}
	pNode2 = pxmlc->curNode;
	pxmlc->curNode = pOldNode;
	
	snprintf(sId1,sizeof(sId1), "%d", nId1);
	mxmlElementSetAttr(pxmlc->xmlCache,pNode2,UnionXMLS_ATTR_ID,sId1);
	
	snprintf(sId2,sizeof(sId2), "%d", nId2);
	mxmlElementSetAttr(pxmlc->xmlCache,pNode1,UnionXMLS_ATTR_ID,sId2);
	return 0;
}

int UnionXMLLocateNew(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId)
{
	/*
	int	ret;
	
	ret = UnionXMLPut(pxmlc, pcXPath, NULL, nId);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionXMLLocateNew:: UnionXMLPut\n");
		return(ret);
	}
	
	return (UnionXMLLocate(pxmlc, pcXPath, nId));
	*/

	char	*ptr = NULL;
	char	sId[32];
	char	tmpPath[128];
	mxml_node_t	*searchNode = NULL;
	mxml_node_t	*pNodeNow = NULL;
	mxml_node_t	*pNodeTmp = NULL;

	if (UnionXMLIsAbsolutePath(pcXPath))
		searchNode = pxmlc->doc;
	else
		searchNode = pxmlc->curNode;
	
	if (nId == 0)
	{
		pNodeNow = mxmlFindPath(pxmlc->xmlCache,searchNode,pcXPath,MXML_PATH_NEW);
		if (pNodeNow == NULL)
			return(errCodeObjectMDL_FieldNotExist);
	}
	else
	{
		snprintf(tmpPath,sizeof(tmpPath),"%s",pcXPath);
		if (((ptr = strrchr(tmpPath,'/')) != NULL) && (ptr != tmpPath))
		{
			*ptr = 0;
			pNodeNow = mxmlFindPath(pxmlc->xmlCache,searchNode,tmpPath,MXML_PATH_NEW);
			if (pNodeNow == NULL)
				return(errCodeObjectMDL_FieldNotExist);
			ptr ++;
		}
		else
		{
			pNodeNow = searchNode;
			if (ptr)
				ptr ++;
			else
				ptr = tmpPath;
		}

		snprintf(sId,sizeof(sId),"%d",nId);
		pNodeTmp = mxmlFindElement(pxmlc->xmlCache,pNodeNow,pxmlc->doc,ptr,UnionXMLS_ATTR_ID,sId,MXML_DESCEND_FIRST);
		if(!pNodeTmp)
			pNodeTmp = mxmlNewElement(pxmlc->xmlCache,pNodeNow,ptr);
		pNodeNow = pNodeTmp;
		mxmlElementSetAttr(pxmlc->xmlCache,pNodeNow,UnionXMLS_ATTR_ID,sId);
	}

	pxmlc->curNode = pNodeNow;
	return(0);
}

int UnionXMLLocateNewEx(PUnionXMLCtxt pxmlc, const char* pcXPath, unsigned int nId)
{
	int	ret;
	
	/*
	ret = UnionXMLPut(pxmlc, pcXPath, NULL, nId);
	if (ret < 0) return ret;
	
	ret = UnionXMLLocate(pxmlc, pcXPath, nId);
	if (ret < 0) 
		return ret;
	*/
	ret =  UnionXMLLocateNew(pxmlc, pcXPath, nId);
	if (ret < 0)
		return(ret);
	
	if(nId == 0){
		ret = UnionXMLSetId(pxmlc, nId);
		if (ret < 0) 
			return ret;
	}

	return 0;
}

int UnionXMLLog(PUnionXMLCtxt pxmlc)
{
	int		ret;
	char		tmpBuf[8192];
	mxml_node_t*	curOldNode;
	
	if (pxmlc == NULL)
	{
		UnionUserErrLog("in UnionXMLLog:: pxmlc can not be null.\n");
		return(errCodeParameter);
	}
	
	curOldNode = pxmlc->curNode;
	UnionXMLSetDumpFmt(pxmlc,1);
	if ((ret = UnionXMLDumpToBuf(pxmlc,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionNullLog("长度大于[%zu],不打印日志\n",sizeof(tmpBuf) - 1);
		pxmlc->curNode = curOldNode;
		return(0);
	}
	tmpBuf[ret] = 0;
	UnionNullLog("[%d][%s]\n",ret,tmpBuf);
	pxmlc->curNode = curOldNode;
	
	return(0);
}

int UnionXMLDelete(PUnionXMLCtxt pxmlc, const char* pcXPath, int nId)
{
	int		ret;
	mxml_node_t*	curOldNode;
	
	if (pxmlc == NULL || pcXPath == NULL || pxmlc->doc == NULL || pxmlc->curNode == NULL) {
		UnionUserErrLog("in UnionXMLDelete:: pxmlc or pcXPath or doc or curNode cannot be null.\n");
		return (errCodeParameter);
	}
	
	curOldNode=pxmlc->curNode;
	ret = UnionXMLLocate(pxmlc, pcXPath, nId);
	if (ret < 0)
	{
		UnionLog("in UnionXMLDelete:: UnionXMLLocate[%s]\n",pcXPath);
		return(0);
	}
	mxmlDelete(pxmlc->curNode);
	pxmlc->curNode = curOldNode;
	return(0);
}


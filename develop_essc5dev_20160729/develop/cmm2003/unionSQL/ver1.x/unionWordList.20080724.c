// Wolfgang Wang
// 2008/7/24

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "UnionStr.h"
#include "unionWordList.h"
#include "unionErrCode.h"

/*
功能	初始化word串
输入参数
	word		值
输出参数
	无
返回值
	字符串指定
*/
PUnionWordList UnionNewWordList(char *word)
{
	PUnionWordList	plist;
	int		len;
	
	if (word == NULL)
		return(NULL);
	if ((plist = (PUnionWordList)malloc(sizeof(*plist))) == NULL)
		return(NULL);
	if ((plist->word = (char *)malloc((len = strlen(word))+1)) == NULL)
	{
		free(plist);
		return(NULL);
	}
	memset(plist->word,0,len+1);
	memcpy(plist->word,word,len);
	plist->next = NULL;
	return(plist);
}

/*
功能	释放word串
输入参数
	plist		串
输出参数
	无
返回值
	无
*/
void UnionFreeWordList(PUnionWordList plist)
{
	if (plist == NULL)
		return;
	UnionFreeWordList(plist->next);
	if (plist->word)
	{
		free(plist->word);
		plist->word = NULL;
	}
	free(plist);
	plist = NULL;
}

/*
功能	将一个字符串拆成字串
输入参数
	recStr		记录串
	lenOfRecStr	记录串长度
输出参数
	无
返回值
	字符串指定
*/
PUnionWordList UnionConvertStrIntoWordList(char *recStr,int lenOfRecStr)
{
}

/*
功能	打印一个串
输入参数
	plist		串
	fileName	文件名称
输出参数
	无
返回值
	>=0		字数量
	<0		错误代码
*/
int UnionPrintWordListToFile(PUnionWordList plist,char *fileName);


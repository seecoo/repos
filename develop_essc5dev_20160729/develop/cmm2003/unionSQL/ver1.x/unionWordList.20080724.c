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
����	��ʼ��word��
�������
	word		ֵ
�������
	��
����ֵ
	�ַ���ָ��
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
����	�ͷ�word��
�������
	plist		��
�������
	��
����ֵ
	��
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
����	��һ���ַ�������ִ�
�������
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	�ַ���ָ��
*/
PUnionWordList UnionConvertStrIntoWordList(char *recStr,int lenOfRecStr)
{
}

/*
����	��ӡһ����
�������
	plist		��
	fileName	�ļ�����
�������
	��
����ֵ
	>=0		������
	<0		�������
*/
int UnionPrintWordListToFile(PUnionWordList plist,char *fileName);


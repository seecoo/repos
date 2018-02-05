//	Wolfgang Wang
//	2009/4/29

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionTree.h"
#include "unionUserInterfaceItemGrp.h"

PUnionTree	pgunionUserInterfaceItemGrp = NULL;

/*
����	
	�жϽ����������Ƿ��ѱ���ʼ����
�������
	��
�������
	��
����ֵ
	1	�ѳ�ʼ��
	0	δ��ʼ��
*/
int UnionIsInterfaceItemsGrpInited()
{
	if (pgunionUserInterfaceItemGrp != NULL)
		return(1);
	else
		return(0);
}


/*
����	
	��ȡ��ǰ�Ľ���������
�������
	��
�������
	��
����ֵ
	��ǰ����������ָ��
*/
PUnionTree UnionGetCurrentInterfaceItemGrp()
{
	return(pgunionUserInterfaceItemGrp);
}

/*
����	
	�ͷŽ��������ݽṹ
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
void UnionDeleteInterfaceItemGrp()
{
	UnionDeleteTree(pgunionUserInterfaceItemGrp);
	pgunionUserInterfaceItemGrp = NULL;
	return;
}

/*
����	
	��������д�뵽ָ���ļ���
�������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintInterfaceItemGrpToSpecFile(char *fileName)
{
	int	ret;
	int	treeIndex = 0;
	
	if ((ret = UnionLogTreeToSpecFile(&treeIndex,pgunionUserInterfaceItemGrp,fileName)) < 0)
	{
		UnionUserErrLog("in UnionPrintInterfaceItemGrpToSpecFile::UnionLogTreeToSpecFile [%s]!\n",fileName);
		return(ret);
	}
	return(ret);
}	

/*
����	
	������Ľ�����д�뵽ָ���ļ���
�������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintErrorInterfaceItemGrpToSpecFile(char *fileName)
{
	int	ret;
	int	treeIndex = 0;
	
	if ((ret = UnionLogTreeWithRemarkToSpecFile(&treeIndex,pgunionUserInterfaceItemGrp,fileName)) < 0)
	{
		UnionUserErrLog("in UnionPrintInterfaceItemGrpToSpecFile::UnionLogTreeWithRemarkToSpecFile [%s]!\n",fileName);
		return(ret);
	}
	return(ret);
}	

/*
����	
	��ʼ�����������ݽṹ
�������
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitInterfaceItemGrp(char *mainMenuName)
{
	UnionDeleteTree(pgunionUserInterfaceItemGrp);
	
	if ((pgunionUserInterfaceItemGrp = UnionNewTree(mainMenuName)) == NULL)
	{
		UnionUserErrLog("in UnionInitInterfaceItemGrp:: UnionNewTree [%s]!\n",mainMenuName);
		return(UnionGetUserDefinedErrorCode());
	}
	return(0);
}
	
/*
����	
	�ж�һ�������ǰ�Ƿ����ڱ�����
�������
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	1	���ڱ�����
	0	δ������
	<0	�������
*/
int UnionSetInterfaceItemDealCurrently(char *itemTag,char *fmt,...)
{
	va_list 	args;
	char		info[512+1];
	PUnionLeaf	ptr;
	int		ret;
	
	if (pgunionUserInterfaceItemGrp == NULL)
	{
		UnionUserErrLog("in UnionSetInterfaceItemDealCurrently:: pgunionUserInterfaceItemGrp is null!\n");
		return(errCodeNullPointer);
	}
	
	memset(info,0,sizeof(info));
	va_start(args,fmt);
	vsprintf(info,fmt,args);
	va_end(args);

	// ���ڱ�����
	if ((ptr = UnionFindLeafInTree(pgunionUserInterfaceItemGrp,itemTag,info)) != NULL)
	{
		ptr->referenceNum += 1;
		//UnionRealNullLog("[%s] already exists!\n",info);
		return(1);
	}
	// 
	if ((ret = UnionAddOneLeafToTree(pgunionUserInterfaceItemGrp,itemTag,info,"")) < 0)
	{
		UnionUserErrLog("in UnionSetInterfaceItemDealCurrently:: UnionAddOneLeafToTree [%s][%s]\n",itemTag,info);
		return(ret);
	}
	return(0);
}

/*
����	
	�ж�һ���������Ƿ����
�������
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	1	����
	0	������
	<0	�������
*/
int UnionIsInterfaceItemExists(char *itemTag,char *fmt,...)
{
	va_list 	args;
	char		info[512+1];
	PUnionLeaf	ptr;
	
	if (pgunionUserInterfaceItemGrp == NULL)
	{
		UnionUserErrLog("in UnionIsInterfaceItemExists:: pgunionUserInterfaceItemGrp is null!\n");
		return(errCodeNullPointer);
	}
	
	memset(info,0,sizeof(info));
	va_start(args,fmt);
	vsprintf(info,fmt,args);
	va_end(args);

	// ���ڱ�����
	if ((ptr = UnionFindLeafInTree(pgunionUserInterfaceItemGrp,itemTag,info)) != NULL)
	{
		ptr->referenceNum += 1;
		//UnionRealNullLog("[%s] already exists!\n",info);
		return(1);
	}
	return(0);
}

/*
����	
	�ж�һ��������ָ�����Ľ������Ƿ����
�������
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	1	����
	0	������
	<0	�������
*/
int UnionIsInterfaceItemContainSpecStrExists(char *itemTag,char *fmt,...)
{
	va_list 	args;
	char		info[512+1];
	PUnionBranch	ptr;
	
	if (pgunionUserInterfaceItemGrp == NULL)
	{
		UnionUserErrLog("in UnionIsInterfaceItemContainSpecStrExists:: pgunionUserInterfaceItemGrp is null!\n");
		return(errCodeNullPointer);
	}
	
	// ���ڱ�����
	if ((ptr = UnionFindBranchInTree(pgunionUserInterfaceItemGrp,itemTag)) == NULL)
		return(0);

	memset(info,0,sizeof(info));
	va_start(args,fmt);
	vsprintf(info,fmt,args);
	va_end(args);

	if (UnionFindFirstLeafContainSpecStrInLeafList(ptr->plastLeaf,info) != NULL)
	{
		//UnionRealNullLog("[%s] already exists!\n",info);
		return(1);
	}
	return(0);
}

/*
����	
	Ѱ��һ��������
�������
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	�ɹ�	������ָ��
	ʧ��	��ָ��
*/
PUnionLeaf UnionFindInterfaceItem(char *itemTag,char *fmt,...)
{
	va_list 	args;
	char		info[512+1];
	
	if (pgunionUserInterfaceItemGrp == NULL)
	{
		UnionUserErrLog("in UnionFindInterfaceItem:: pgunionUserInterfaceItemGrp is null!\n");
		return(NULL);
	}
	
	memset(info,0,sizeof(info));
	va_start(args,fmt);
	vsprintf(info,fmt,args);
	va_end(args);

	return(UnionFindLeafInTree(pgunionUserInterfaceItemGrp,itemTag,info));
}


/*
����	
	��һ����������뵽����������
�������
	remark	˵��
	itemTag	�������ʶ
	fmt	�������ʶ
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionAddInterfaceItemToItemGrp(char *remark,char *itemTag,char *fmt,...)
{
	va_list 	args;
	char		info[512+1];
	
	if (pgunionUserInterfaceItemGrp == NULL)
	{
		UnionUserErrLog("in UnionAddInterfaceItemToItemGrp:: pgunionUserInterfaceItemGrp is null!\n");
		return(errCodeNullPointer);
	}
	
	memset(info,0,sizeof(info));
	va_start(args,fmt);
	vsprintf(info,fmt,args);
	va_end(args);

	return(UnionAddOneLeafToTree(pgunionUserInterfaceItemGrp,itemTag,info,remark));
}

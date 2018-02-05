// author	Wolfgang Wang
// date		2010-5-5

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"

#include "unionTree.h"

/*
����
	�滻Ҷ�ӵ�˵��
���������
	pleaf		Ҷ��ָ��
	remark		Ҷ��˵��
�������
	��
����ֵ
	>=	�ɹ�
	<0	������
*/
int UnionReplaceLeafRemark(PUnionLeaf pleaf,char *remark)
{
	int	nameLen;
	
	if (pleaf == NULL)
		return(errCodeNullPointer);
	if (remark == NULL)
		return(0);
	if (strlen(remark) == 0)
		return(0);
	if (pleaf->remark != NULL)
		free(pleaf->remark);
	nameLen=strlen(remark);
	if ((pleaf->remark = (char *)malloc(nameLen+1)) == NULL)
	{
		UnionSystemErrLog("in UnionNewLeaf:: malloc [%d]!\n",nameLen);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	memcpy(pleaf->remark,remark,nameLen);
	pleaf->remark[nameLen] = 0;	
	return(0);
}
		
/*
����
	��ʼ��һƬҶ��
���������
	leafName	Ҷ������
	remark		Ҷ��˵��
�������
	��
����ֵ
	�ɹ�	Ҷ��ָ��
	ʧ��	NULL
*/
PUnionLeaf UnionNewLeaf(char *leafName,char *remark)
{
	PUnionLeaf	ptr;
	int		nameLen;
	
	if ((ptr = (PUnionLeaf)malloc(sizeof(*ptr))) == NULL)
	{
		UnionSystemErrLog("in UnionNewLeaf:: malloc [%zu]!\n",sizeof(*ptr));
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	nameLen=strlen(leafName);
	if ((ptr->leafName = (char *)malloc(nameLen+1)) == NULL)
	{
		free(ptr);
		UnionSystemErrLog("in UnionNewLeaf:: malloc [%d]!\n",nameLen);
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	memcpy(ptr->leafName,leafName,nameLen);
	ptr->leafName[nameLen] = 0;
	ptr->remark = NULL;
	ptr->referenceNum = 0;
	ptr->pnextLeaf = NULL;
	nameLen=strlen(remark);
	if ((remark != NULL) && (strlen(remark) > 0))
	{
		if ((ptr->remark = (char *)malloc(nameLen+1)) == NULL)
		{
			free(ptr);
			UnionSystemErrLog("in UnionNewLeaf:: malloc [%d]!\n",nameLen);
			UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
			return(NULL);
		}
		memcpy(ptr->remark,remark,nameLen);
		ptr->remark[nameLen] = 0;	
	}
	return(ptr);
}
	
/*
����
	�ͷ�һ��Ҷ��
���������
	ptr		Ҷ��ָ��
�������
	��
����ֵ
	��
*/
void UnionDeleteLeaf(PUnionLeaf ptr)
{
	if (ptr == NULL)
		return;
	UnionDeleteLeaf(ptr->pnextLeaf);
	free(ptr->leafName);
	if (ptr->remark)
		free(ptr->remark);
	free(ptr);
	return;
}

/*
����
	��һ��Ҷ��д�뵽��־��
���������
	leafIndex	Ҷ�����
	ptr		Ҷ��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogLeafWithRemark(int *leafIndex,PUnionLeaf ptr,FILE *fp)
{
	if (ptr == NULL)
		return;
	UnionLogLeafWithRemark(leafIndex,ptr->pnextLeaf,fp);
	if (ptr->remark == NULL)
		return;
	if (fp == NULL)
		UnionRealNullLog("leaf::index=%04d,referenceNum=%04d,name=%s",*leafIndex,ptr->referenceNum,ptr->leafName);
	else
		fprintf(fp,"leaf::index=%04d,referenceNum=%04d,name=%s",*leafIndex,ptr->referenceNum,ptr->leafName);
	fprintf(fp,",remark=%s\n",ptr->remark);
	*leafIndex += 1;
	return;
}

/*
����
	��һ��Ҷ��д�뵽��־��
���������
	leafIndex	Ҷ�����
	ptr		Ҷ��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogLeaf(int *leafIndex,PUnionLeaf ptr,FILE *fp)
{
	if (ptr == NULL)
		return;
	UnionLogLeaf(leafIndex,ptr->pnextLeaf,fp);
	if (fp == NULL)
		UnionRealNullLog("leaf::index=%04d,referenceNum=%04d,name=%s",*leafIndex,ptr->referenceNum,ptr->leafName);
	else
		fprintf(fp,"leaf::index=%04d,referenceNum=%04d,name=%s",*leafIndex,ptr->referenceNum,ptr->leafName);
	if (ptr->remark != NULL)
		if (fp == NULL)
			UnionRealNullLog(",remark=%s\n",ptr->remark);
		else
			fprintf(fp,",remark=%s\n",ptr->remark);
	else
		if (fp == NULL)
			UnionRealNullLog("\n");
		else
			fprintf(fp,"\n");
	*leafIndex += 1;
	return;
}

/*
����
	��ʼ��һ����֦
���������
	branchName	��֦����
�������
	��
����ֵ
	�ɹ�	��ָ֦��
	ʧ��	NULL
*/
PUnionBranch UnionNewBranch(char *branchName)
{
	PUnionBranch	ptr;
	int		nameLen;
	
	if ((ptr = (PUnionBranch)malloc(sizeof(*ptr))) == NULL)
	{
		UnionSystemErrLog("in UnionNewBranch:: malloc [%zu]!\n",sizeof(*ptr));
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	nameLen=strlen(branchName);
	if ((ptr->branchName = (char *)malloc(nameLen+1)) == NULL)
	{
		free(ptr);
		UnionSystemErrLog("in UnionNewBranch:: malloc [%d]!\n",nameLen);
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	memcpy(ptr->branchName,branchName,nameLen);
	ptr->branchName[nameLen] = 0;
	ptr->pnextBranch = NULL;
	ptr->plastLeaf = NULL;
	ptr->leafNum = 0;
	return(ptr);
}
	
/*
����
	�ͷ�һ����֦
���������
	ptr		��ָ֦��
�������
	��
����ֵ
	��
*/
void UnionDeleteBranch(PUnionBranch ptr)
{
	if (ptr == NULL)
		return;
	// ɾ������֦�ϵ�Ҷ��
	UnionDeleteLeaf(ptr->plastLeaf);
	// ɾ����һ����֦
	UnionDeleteBranch(ptr->pnextBranch);
	free(ptr->branchName);
	free(ptr);
	return;
}

/*
����
	��һ����֦д�뵽��־��
���������
	branchIndex	��֦���
	ptr		��ָ֦��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogBranch(int *branchIndex,PUnionBranch ptr,FILE *fp)
{
	int	leafNum = 0;
	
	if (ptr == NULL)
		return;
	UnionLogBranch(branchIndex,ptr->pnextBranch,fp);
	if (fp == NULL)
		UnionRealNullLog("**** branch::index=%04d,leafNum=%03d,name=%s\n",*branchIndex,ptr->leafNum,ptr->branchName);	
	else
		fprintf(fp,"**** branch::index=%04d,leafNum=%03d,name=%s\n",*branchIndex,ptr->leafNum,ptr->branchName);
	UnionLogLeaf(&leafNum,ptr->plastLeaf,fp);
	*branchIndex += 1;
	return;
}

/*
����
	��һ����֦д�뵽��־��
���������
	branchIndex	��֦���
	ptr		��ָ֦��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogBranchWithRemark(int *branchIndex,PUnionBranch ptr,FILE *fp)
{
	int	leafNum = 0;
	
	if (ptr == NULL)
		return;
	UnionLogBranchWithRemark(branchIndex,ptr->pnextBranch,fp);
	UnionLogLeafWithRemark(&leafNum,ptr->plastLeaf,fp);
	if (leafNum <= 0)
		return;
	if (fp == NULL)
		UnionRealNullLog("**** branch::index=%04d,leafNum=%03d,name=%s\n",*branchIndex,leafNum,ptr->branchName);	
	else
		fprintf(fp,"**** branch::index=%04d,leafNum=%03d,name=%s\n",*branchIndex,leafNum,ptr->branchName);
	*branchIndex += 1;
	return;
}

/*
����
	��ʼ��һ����
���������
	treeName	������
�������
	��
����ֵ
	�ɹ�	��ָ��
	ʧ��	NULL
*/
PUnionTree UnionNewTree(char *treeName)
{
	PUnionTree	ptr;
	int		nameLen;
	
	if ((ptr = (PUnionTree)malloc(sizeof(*ptr))) == NULL)
	{
		UnionSystemErrLog("in UnionNewTree:: malloc [%zu]!\n",sizeof(*ptr));
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	nameLen=strlen(treeName);
	if ((ptr->treeName = (char *)malloc(nameLen+1)) == NULL)
	{
		free(ptr);
		UnionSystemErrLog("in UnionNewTree:: malloc [%d]!\n",nameLen);
		UnionSetUserDefinedErrorCode(errCodeUseOSErrCode);
		return(NULL);
	}
	memcpy(ptr->treeName,treeName,nameLen);
	ptr->treeName[nameLen] = 0;
	ptr->plastBranch = NULL;
	ptr->branchNum = 0;
	return(ptr);
}
	
/*
����
	�ͷ�һ����
���������
	ptr		��ָ��
�������
	��
����ֵ
	��
*/
void UnionDeleteTree(PUnionTree ptr)
{
	if (ptr == NULL)
		return;
	// ɾ����һ����֦
	UnionDeleteBranch(ptr->plastBranch);
	free(ptr->treeName);
	free(ptr);
	return;
}

/*
����
	��һ����д�뵽��־��
���������
	treeIndex	�����
	ptr		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogTree(int *treeIndex,PUnionTree ptr,FILE *fp)
{
	int	branchNum = 0;
	
	if (ptr == NULL)
		return;
	if (fp == NULL)
		UnionRealNullLog("**** tree::index=%04d,branchNum=%03d,name=%s ****\n",*treeIndex,ptr->branchNum,ptr->treeName);	
	else
		fprintf(fp,"**** tree::index=%04d,branchNum=%03d,name=%s ****\n",*treeIndex,ptr->branchNum,ptr->treeName);
	UnionLogBranch(&branchNum,ptr->plastBranch,fp);
	*treeIndex += 1;
	return;
}

/*
����
	��һ����д�뵽��־��
���������
	treeIndex	�����
	ptr		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	��
*/
void UnionLogTreeWithRemark(int *treeIndex,PUnionTree ptr,FILE *fp)
{
	int	branchNum = 0;
	
	if (ptr == NULL)
		return;
	UnionLogBranchWithRemark(&branchNum,ptr->plastBranch,fp);
	if (branchNum <= 0)
		return;
	if (fp == NULL)
		UnionRealNullLog("**** tree::index=%04d,branchNum=%03d,name=%s ****\n",*treeIndex,ptr->branchNum,ptr->treeName);	
	else
		fprintf(fp,"**** tree::index=%04d,branchNum=%03d,name=%s ****\n",*treeIndex,ptr->branchNum,ptr->treeName);
	*treeIndex += 1;
	return;
}

/*
����
	��һ����д�뵽��־��
���������
	treeIndex	�����
	ptr		��ָ��
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	0	ʧ�ܣ�������
*/
int UnionLogTreeToSpecFile(int *treeIndex,PUnionTree ptr,char *fileName)
{
	FILE	*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionLogTreeToSpecFile:: fopen [%s]!\n",fileName);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	UnionLogTree(treeIndex,ptr,fp);
	fclose(fp);
	return(0);
}

/*
����
	��һ����д�뵽��־��
���������
	treeIndex	�����
	ptr		��ָ��
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	0	ʧ�ܣ�������
*/
int UnionLogTreeWithRemarkToSpecFile(int *treeIndex,PUnionTree ptr,char *fileName)
{
	FILE	*fp;
	
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionUserErrLog("in UnionLogTreeToSpecFile:: fopen [%s]!\n",fileName);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	UnionLogTreeWithRemark(treeIndex,ptr,fp);
	fclose(fp);
	return(0);
}

/*
����
	��һ�����ϲ�һ����֦
���������
	ptree		��
	branchName	��֦����
�������
	��
����ֵ
	�ɹ�	������ָ֦��
	ʧ��	NULL
*/
PUnionBranch UnionFindBranchInTree(PUnionTree ptree,char *branchName)
{
	if (ptree == NULL)
		return(NULL);
	return(UnionFindBranchInBranchList(ptree->plastBranch,branchName));
}

/*
����
	��һ����֦�б��в�һ����֦
���������
	pbranchList	��֦�б�
	branchName	��֦����
�������
	��
����ֵ
	�ɹ�	������ָ֦��
	ʧ��	NULL
*/
PUnionBranch UnionFindBranchInBranchList(PUnionBranch pbranchList,char *branchName)
{
	if (pbranchList == NULL)
	{
		UnionSetUserDefinedErrorCode(0);
		return(NULL);
	}
	if (branchName == NULL)
	{
		UnionUserErrLog("in UnionFindBranchInBranchList:: branchName is NULL!\n");
		UnionSetUserDefinedErrorCode(errCodeNullPointer);
		return(NULL);
	}
	if (strcmp(pbranchList->branchName,branchName) == 0)
		return(pbranchList);
	return(UnionFindBranchInBranchList(pbranchList->pnextBranch,branchName));
}

/*
����
	��һ����֦�ϲ�һ����Ҷ
���������
	pbranch		��֦
	leafName	��Ҷ����
�������
	��
����ֵ
	�ɹ�	������Ҷָ��
	ʧ��	NULL
*/
PUnionLeaf UnionFindLeafInBranch(PUnionBranch pbranch,char *leafName)
{
	if (pbranch == NULL)
		return(NULL);
	return(UnionFindLeafInLeafList(pbranch->plastLeaf,leafName));
}

/*
����
	��һ����Ҷ�б��в�һ����Ҷ
���������
	pleafList	��Ҷ�б�
	leafName	��Ҷ����
�������
	��
����ֵ
	�ɹ�	������Ҷָ��
	ʧ��	NULL
*/
PUnionLeaf UnionFindLeafInLeafList(PUnionLeaf pleafList,char *leafName)
{
	if (pleafList == NULL)
	{
		UnionSetUserDefinedErrorCode(0);
		return(NULL);
	}
	if (leafName == NULL)
	{
		UnionUserErrLog("in UnionFindBranchInBranchList:: leafName is NULL!\n");
		UnionSetUserDefinedErrorCode(errCodeNullPointer);
		return(NULL);
	}
	if (strcmp(pleafList->leafName,leafName) == 0)
		return(pleafList);
	return(UnionFindLeafInLeafList(pleafList->pnextLeaf,leafName));
}

/*
����
	��һ����Ҷ�б��в��һ��������ָ����ʶ����Ҷ
���������
	pleafList	��Ҷ�б�
	leafName	��Ҷ����
�������
	��
����ֵ
	�ɹ�	������Ҷָ��
	ʧ��	NULL
*/
PUnionLeaf UnionFindFirstLeafContainSpecStrInLeafList(PUnionLeaf pleafList,char *leafName)
{
	if (pleafList == NULL)
	{
		UnionSetUserDefinedErrorCode(0);
		return(NULL);
	}
	if (leafName == NULL)
	{
		UnionUserErrLog("in UnionFindFirstLeafContainSpecStrInLeafList:: leafName is NULL!\n");
		UnionSetUserDefinedErrorCode(errCodeNullPointer);
		return(NULL);
	}
	if (strstr(pleafList->leafName,leafName) != NULL)
		return(pleafList);
	return(UnionFindFirstLeafContainSpecStrInLeafList(pleafList->pnextLeaf,leafName));
}

/*
����
	��һ�����ϲ�һ����Ҷ
���������
	ptree		��
	branchName	��֦����
	leafName	��Ҷ����
�������
	��
����ֵ
	�ɹ�	������Ҷָ��
	ʧ��	NULL
*/
PUnionLeaf UnionFindLeafInTree(PUnionTree ptree,char *branchName,char *leafName)
{
	return(UnionFindLeafInBranch(UnionFindBranchInTree(ptree,branchName),leafName));
}

/*
����
	����������һƬҶ��
���������
	ptree		��
	branchName	��֦����
	leafName	Ҷ������
	remark		Ҷ��˵��
�������
	��
����ֵ
	>=0		�ɹ�����Ҷ����������
	<0		�������
*/
int UnionAddOneLeafToTree(PUnionTree ptree,char *branchName,char *leafName,char *remark)
{
	PUnionLeaf	ptrLeaf;
	PUnionBranch	ptrBranch;
	int		isNewLeaf = 0;
	
	if (ptree == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));

	// �������ϲ�����֦
	if ((ptrBranch = UnionFindBranchInTree(ptree,branchName)) == NULL)
	{
		// û���ҵ���֦
		if ((ptrBranch = UnionNewBranch(branchName)) == NULL)
		{
			UnionUserErrLog("in UnionAddOneLeafToTree:: UnionNewBranch! branchName = [%s]!\n",branchName);
			return(UnionGetUserDefinedErrorCode());
		}
		ptree->branchNum += 1;
		ptrBranch->pnextBranch = ptree->plastBranch;
		ptree->plastBranch = ptrBranch;
	}
	// �������ϲ�����Ҷ
	if ((ptrLeaf = UnionFindLeafInBranch(ptrBranch,leafName)) == NULL)
	{
		// û���ҵ�Ҷ��
		if ((ptrLeaf = UnionNewLeaf(leafName,remark)) == NULL)
		{
			UnionUserErrLog("in UnionAddOneLeafToTree:: UnionNewBranch! leafName = [%s]!\n",leafName);
			return(UnionGetUserDefinedErrorCode());
		}
		ptrBranch->leafNum += 1;
		ptrLeaf->pnextLeaf = ptrBranch->plastLeaf;
		ptrBranch->plastLeaf = ptrLeaf;
		isNewLeaf = 1;
	}
	if (!isNewLeaf)
		UnionReplaceLeafRemark(ptrLeaf,remark);
	ptrLeaf->referenceNum += 1;
	return(ptrLeaf->referenceNum);
}


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
功能
	替换叶子的说明
输入参数：
	pleaf		叶子指针
	remark		叶子说明
输出参数
	无
返回值
	>=	成功
	<0	错误码
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
功能
	初始化一片叶子
输入参数：
	leafName	叶子名称
	remark		叶子说明
输出参数
	无
返回值
	成功	叶子指针
	失败	NULL
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
功能
	释放一个叶子
输入参数：
	ptr		叶子指针
输出参数
	无
返回值
	无
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
功能
	将一个叶子写入到日志中
输入参数：
	leafIndex	叶子序号
	ptr		叶子指针
	fp		文件指针
输出参数
	无
返回值
	无
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
功能
	将一个叶子写入到日志中
输入参数：
	leafIndex	叶子序号
	ptr		叶子指针
	fp		文件指针
输出参数
	无
返回值
	无
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
功能
	初始化一个树枝
输入参数：
	branchName	树枝名称
输出参数
	无
返回值
	成功	树枝指针
	失败	NULL
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
功能
	释放一个树枝
输入参数：
	ptr		树枝指针
输出参数
	无
返回值
	无
*/
void UnionDeleteBranch(PUnionBranch ptr)
{
	if (ptr == NULL)
		return;
	// 删掉本树枝上的叶子
	UnionDeleteLeaf(ptr->plastLeaf);
	// 删掉下一个树枝
	UnionDeleteBranch(ptr->pnextBranch);
	free(ptr->branchName);
	free(ptr);
	return;
}

/*
功能
	将一个树枝写入到日志中
输入参数：
	branchIndex	树枝序号
	ptr		树枝指针
	fp		文件指针
输出参数
	无
返回值
	无
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
功能
	将一个树枝写入到日志中
输入参数：
	branchIndex	树枝序号
	ptr		树枝指针
	fp		文件指针
输出参数
	无
返回值
	无
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
功能
	初始化一个树
输入参数：
	treeName	树名称
输出参数
	无
返回值
	成功	树指针
	失败	NULL
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
功能
	释放一个树
输入参数：
	ptr		树指针
输出参数
	无
返回值
	无
*/
void UnionDeleteTree(PUnionTree ptr)
{
	if (ptr == NULL)
		return;
	// 删掉第一个树枝
	UnionDeleteBranch(ptr->plastBranch);
	free(ptr->treeName);
	free(ptr);
	return;
}

/*
功能
	将一个树写入到日志中
输入参数：
	treeIndex	树序号
	ptr		树指针
	fp		文件指针
输出参数
	无
返回值
	无
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
功能
	将一个树写入到日志中
输入参数：
	treeIndex	树序号
	ptr		树指针
	fp		文件指针
输出参数
	无
返回值
	无
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
功能
	将一个树写入到日志中
输入参数：
	treeIndex	树序号
	ptr		树指针
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	0	失败，错误码
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
功能
	将一个树写入到日志中
输入参数：
	treeIndex	树序号
	ptr		树指针
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	0	失败，错误码
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
功能
	在一棵树上查一个树枝
输入参数：
	ptree		树
	branchName	树枝名称
输出参数
	无
返回值
	成功	返回树枝指针
	失败	NULL
*/
PUnionBranch UnionFindBranchInTree(PUnionTree ptree,char *branchName)
{
	if (ptree == NULL)
		return(NULL);
	return(UnionFindBranchInBranchList(ptree->plastBranch,branchName));
}

/*
功能
	在一棵树枝列表中查一个树枝
输入参数：
	pbranchList	树枝列表
	branchName	树枝名称
输出参数
	无
返回值
	成功	返回树枝指针
	失败	NULL
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
功能
	在一个树枝上查一个树叶
输入参数：
	pbranch		树枝
	leafName	树叶名称
输出参数
	无
返回值
	成功	返回树叶指针
	失败	NULL
*/
PUnionLeaf UnionFindLeafInBranch(PUnionBranch pbranch,char *leafName)
{
	if (pbranch == NULL)
		return(NULL);
	return(UnionFindLeafInLeafList(pbranch->plastLeaf,leafName));
}

/*
功能
	在一个树叶列表中查一个树叶
输入参数：
	pleafList	树叶列表
	leafName	树叶名称
输出参数
	无
返回值
	成功	返回树叶指针
	失败	NULL
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
功能
	在一个树叶列表中查第一个包含了指定标识的树叶
输入参数：
	pleafList	树叶列表
	leafName	树叶名称
输出参数
	无
返回值
	成功	返回树叶指针
	失败	NULL
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
功能
	在一棵树上查一个树叶
输入参数：
	ptree		树
	branchName	树枝名称
	leafName	树叶名称
输出参数
	无
返回值
	成功	返回树叶指针
	失败	NULL
*/
PUnionLeaf UnionFindLeafInTree(PUnionTree ptree,char *branchName,char *leafName)
{
	return(UnionFindLeafInBranch(UnionFindBranchInTree(ptree,branchName),leafName));
}

/*
功能
	在树上增加一片叶子
输入参数：
	ptree		树
	branchName	树枝名称
	leafName	叶子名称
	remark		叶子说明
输出参数
	无
返回值
	>=0		成功，树叶的生长次数
	<0		出错代码
*/
int UnionAddOneLeafToTree(PUnionTree ptree,char *branchName,char *leafName,char *remark)
{
	PUnionLeaf	ptrLeaf;
	PUnionBranch	ptrBranch;
	int		isNewLeaf = 0;
	
	if (ptree == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));

	// 先在树上查找树枝
	if ((ptrBranch = UnionFindBranchInTree(ptree,branchName)) == NULL)
	{
		// 没有找到树枝
		if ((ptrBranch = UnionNewBranch(branchName)) == NULL)
		{
			UnionUserErrLog("in UnionAddOneLeafToTree:: UnionNewBranch! branchName = [%s]!\n",branchName);
			return(UnionGetUserDefinedErrorCode());
		}
		ptree->branchNum += 1;
		ptrBranch->pnextBranch = ptree->plastBranch;
		ptree->plastBranch = ptrBranch;
	}
	// 再在树上查找树叶
	if ((ptrLeaf = UnionFindLeafInBranch(ptrBranch,leafName)) == NULL)
	{
		// 没有找到叶子
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


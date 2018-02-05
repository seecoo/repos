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
功能	
	判断界面项数组是否已被初始化了
输入参数
	无
输出参数
	无
返回值
	1	已初始化
	0	未初始化
*/
int UnionIsInterfaceItemsGrpInited()
{
	if (pgunionUserInterfaceItemGrp != NULL)
		return(1);
	else
		return(0);
}


/*
功能	
	获取当前的界面项数组
输入参数
	无
输出参数
	无
返回值
	当前界面项数组指针
*/
PUnionTree UnionGetCurrentInterfaceItemGrp()
{
	return(pgunionUserInterfaceItemGrp);
}

/*
功能	
	释放界面项数据结构
输入参数
	无
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
void UnionDeleteInterfaceItemGrp()
{
	UnionDeleteTree(pgunionUserInterfaceItemGrp);
	pgunionUserInterfaceItemGrp = NULL;
	return;
}

/*
功能	
	将界面项写入到指定文件中
输入参数
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
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
功能	
	将错误的界面项写入到指定文件中
输入参数
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
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
功能	
	初始化界面项数据结构
输入参数
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
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
功能	
	判断一个界面项当前是否正在被处理
输入参数
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	1	正在被处理
	0	未被处理
	<0	出错代码
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

	// 正在被处理
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
功能	
	判断一个界面项是否存在
输入参数
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	1	存在
	0	不存在
	<0	出错代码
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

	// 正在被处理
	if ((ptr = UnionFindLeafInTree(pgunionUserInterfaceItemGrp,itemTag,info)) != NULL)
	{
		ptr->referenceNum += 1;
		//UnionRealNullLog("[%s] already exists!\n",info);
		return(1);
	}
	return(0);
}

/*
功能	
	判断一个包含了指定串的界面项是否存在
输入参数
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	1	存在
	0	不存在
	<0	出错代码
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
	
	// 正在被处理
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
功能	
	寻找一个界面项
输入参数
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	成功	界面项指针
	失败	空指针
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
功能	
	将一个界面项加入到界面项组中
输入参数
	remark	说明
	itemTag	界面项标识
	fmt	界面项标识
输出参数
	无
返回值
	>=0	成功
	<0	错误码
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

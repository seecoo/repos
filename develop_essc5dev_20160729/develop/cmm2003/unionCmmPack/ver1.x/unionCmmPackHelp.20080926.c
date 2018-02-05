//	Wolfgang Wang, 2008/9/28

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionServicePackage.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionProc.h"
#ifndef _WIN32
#include "unionREC.h"
#else
#include "unionRECVar.h"
#endif

// 获取服务说明
/*
输入参数
	serviceID	服务代码
输出参数
	无
返回值
	服务说明的指针
	出错，空指针
*/
char *UnionFindCmmPackServiceRemark(int serviceID)
{
	char	varName[128];
	
	sprintf(varName,"cmmPackServiceNameOf%03d",serviceID);
	return(UnionReadStringTypeRECVar(varName));
}
			
// 获取域标识说明
/*
输入参数
	tag	域标识
输出参数
	无
返回值
	说明的指针
	出错，空指针
*/
char *UnionFindCmmPackFldTagRemark(int tag)
{
	char	varName[128];
	
	sprintf(varName,"cmmPackFldTagNameOf%03d",tag);
	return(UnionReadStringTypeRECVar(varName));
}


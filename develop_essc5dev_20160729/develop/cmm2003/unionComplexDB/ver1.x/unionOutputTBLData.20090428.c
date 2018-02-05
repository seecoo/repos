#include <stdlib.h>

#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "UnionLog.h"			// 使用3.x版本
#include "unionWorkingDir.h"
#include "unionResID.h"
#include "unionREC.h"

#include "unionComplexDBCommon.h"
#include "unionComplexDBObject.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBObjectDef.h"

#include "unionDatabaseCmd.h"

#include "unionTBLData.h"

// 获取 TBL 数据文件名
int UnionGetTBLDataFileName(char *fileName, char *resName)
{
	char		mainDir[512];
	char		*ptr;
	
	if ((NULL == fileName) || (NULL == resName))
	{
		UnionUserErrLog("in UnionGetTBLDataFileName, bad parameter.\n");
		return errCodeParameter;
	}
	
	memset(mainDir, 0, sizeof(mainDir));
#ifdef _WIN32
	UnionGetMainWorkingDir(mainDir);
	strcat(mainDir, "\\tmp");
#else
	if ((ptr = getenv("UNIONTEMP")) != NULL)
	{
		strcpy(mainDir, ptr);
	}
	else
	{
		UnionNullLog("in UnionGetTBLDataFileName, the env UNIONTEMP not defined.\n");
	}
#endif
	return sprintf(fileName, "%s/%s.txt", mainDir, resName);
}

/* 
功能：
	从complexDB中导出数据
参数:
	[IN]:
		resName			: 对象名[表名]
	[OUT]:
		NULL
返回值:
	[0, +d)				: 成功, 导出数据记录的条数
	(-d, 0)				: 失败
*/
int UnionOutputTBLData(char *resName)
{
	int		ret;
	char		tblFileName[512];
	
	if (NULL == resName)
	{
		UnionUserErrLog("in UnionOutputTBLData, bad parameter.\n");
		return errCodeParameter;
	}
	
	memset(tblFileName, 0, sizeof(tblFileName));
	if ((ret = UnionGetTBLDataFileName(tblFileName, resName)) < 0)
	{
		UnionUserErrLog("in UnionOutputTBLData:: UnionGetTBLDataFileName error, resName: [%s], ret: [%d].\n", resName, ret);
		return ret;
	}
	
	if ((ret = UnionBatchSelectUnionObjectRecord(resName, "", tblFileName)) < 0)
	{
		UnionUserErrLog("in UnionOutputTBLData:: UnionBatchSelectUnionObjectRecord error, resName: [%s], reqStr: [], tblFileName: [%s], ret: [%d].\n", resName, tblFileName);
		return ret;
	}
	
	return ret;
}


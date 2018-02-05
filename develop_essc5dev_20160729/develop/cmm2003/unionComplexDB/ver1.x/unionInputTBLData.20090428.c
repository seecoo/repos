#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif

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

#include "UnionEnv.h"
#include "unionDatabaseCmd.h"

#include "unionTBLData.h"

// 将env数据转换为complexDB数据
int unionTranslateEnvDataToComplexDBData(char *desStr, int sizeOfDesStr, char *oriStr, int lenOfOriStr, char *idOfObject)
{
	TUnionObject		sObject;
	PUnionEnviVariable 	pvar;
	char			caValue[512];
	int			ret, iRet;
	int			i, offset;
	
	if ((NULL == desStr) || (NULL == oriStr) || (0 >= sizeOfDesStr) || (0 >= lenOfOriStr))
	{
		UnionUserErrLog("in unionTranslateEnvDataToComplexDBData, bad parameter.\n");
		return errCodeParameter;
	}
	
	// 读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in unionTranslateEnvDataToComplexDBData:: UnionReadObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	if ((pvar = ReadUnionEnviVarFromStr(oriStr)) == NULL)
	{
		UnionUserErrLog("in unionTranslateEnvDataToComplexDBData:: ReadUnionEnviVarFromStr, read env var from: [%s] error.\n", oriStr);
		return errCodeEnviMDL_NullLine;
	}
	
	for (i = 0, offset = 0; i < sObject.fldNum; i++)
	{
		if (NULL == pvar->pVariableValue[i])
		{
			UnionUserErrLog("in unionTranslateEnvDataToComplexDBData, the fld: [%s]'s value is null, please check the data is you need.\n", sObject.fldDefGrp[i].name);
			break;
		}
		
		memset(caValue, 0, sizeof(caValue));
		strcpy(caValue, (char *)pvar->pVariableValue[i]);
		if ((ret = UnionPutRecFldIntoRecStr(sObject.fldDefGrp[i].name, caValue, strlen(caValue), desStr + offset, sizeOfDesStr - offset)) < 0)
		{
			UnionUserErrLog("in unionTranslateEnvDataToComplexDBData:: UnionPutRecFldIntoRecStr error, ret: [%d].\n", ret);
			UnionFreeEnviVar(pvar);
			return ret;
		}
		offset	+= ret;
	}
	UnionFreeEnviVar(pvar);
	
	return offset;
}

/* 
功能：
	从 tblFileName 文件中,将数据导入 resName 中
参数:
	[IN]:
		tblFileName		: 需要被导入数据的文件名,要求为全路径文件名
		resName			: 对象名[表名]
		isEnvData		: 是否为 env 数据
	[OUT]:
		NULL
返回值:
	[0, +d)				: 成功, 导入数据记录的条数
	(-d, 0)				: 失败
*/
int UnionInputTBLData(char *tblFileName, char *resName, int isEnvData)
{
	char			recStr[2048 + 1];
	char			desStr[2048 + 1];
	int			lenOfRecStr;
	int 			ret, nums, len;
	TUnionRecFileHDL        hdl;
	
	if ((NULL == tblFileName) || (NULL == resName))
	{
		UnionUserErrLog("in UnionInputTBLData, bad parameter.\n");
		return errCodeParameter;
	}
	
	// 判断数据文件是否存在
	if (UnionExistsFile(tblFileName) == 0)
	{
		UnionUserErrLog("in UnionInputTBLData, not find the file: [%s].\n", tblFileName);
		return errCodeParameter;
	}
	
	if ((hdl = UnionOpenRecFileHDL(tblFileName)) == NULL)
	{
		UnionUserErrLog("in UnionInputTBLData, open file: [%s] failed.\n", tblFileName);
		return errCodeUseOSErrCode;
	}
	
	nums			= 0;
	while (1)
	{
		memset(recStr, 0, sizeof(recStr));
                if ((lenOfRecStr = UnionReadNextRecFromFile(hdl,recStr,sizeof(recStr))) == 0)	// 已无记录
			break;
		
		if (lenOfRecStr < 0)	// 出错
                {
                        UnionCloseRecFileHDL(hdl);
                        return(lenOfRecStr);
                }
                
                memset(desStr, 0, sizeof(desStr));
                if (1 == isEnvData)
                {
                	if ((ret = unionTranslateEnvDataToComplexDBData(desStr, sizeof(desStr), recStr, lenOfRecStr, resName)) < 0)
                	{
                		UnionUserErrLog("in UnionInputTBLData:: unionTranslateEnvDataToComplexDBData error, recStr: [%s], resName: [%s], ret: [%d].\n", recStr, resName, ret);
                		continue;
                	}
                	len	= ret;
                }
                else
                {
                	memcpy(desStr, recStr, lenOfRecStr);
                	len	= lenOfRecStr;
                }
                
                if ((ret = UnionInsertObjectRecord(resName, desStr, len)) < 0)
                {
                	UnionUserErrLog("in UnionInputTBLData:: UnionInsertObjectRecord error, resName: [%s], desStr: [%s], ret: [%d].\n", resName, desStr, ret);
                	continue;
                }
                nums++;
	}
	UnionCloseRecFileHDL(hdl);
	
	return nums;
}


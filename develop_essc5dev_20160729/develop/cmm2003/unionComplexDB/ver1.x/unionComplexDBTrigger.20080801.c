//	Author: Wolfgang Wang
//	Date: 2008-10-23

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionRec0.h"
#include "unionErrCode.h"
#include "simuMngSvrLocally.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBTrigger.h"
#include "unionComplexDBOperationDef.h"
#include "unionDatabaseCmd.h"

#ifndef conMenuDefinedQueryConditionFldSeperator
#define conMenuDefinedQueryConditionFldSeperator	','     // 菜单定义的查询条件中的分隔符
#endif
#ifndef conMenuDefinedQueryConditionReadThisFld
#define conMenuDefinedQueryConditionReadThisFld		"this."   // 定义读取当前记录的该域
#endif

int gunionIsOperationOnTrigger = 0;

/*
功能	打开触发器触发的动作
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionOpenOperationOnTrigger()
{
	gunionIsOperationOnTrigger = 1;
}

/*
功能	关闭触发器触发的动作
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionCloseOperationOnTrigger()
{
	gunionIsOperationOnTrigger = 0;
}

/*
功能	判断是否是触发器触发的动作
输入参数
	无
输出参数
	无
返回值
	1	是
	0	不是
*/
int UnionIsOperationOnTrigger()
{
	return(gunionIsOperationOnTrigger);
}

//---------------------------------------------------------------------------
/*
功能	根据定义的trigger条件，以及操作条件，拼成trigger需要的操作条件
输入参数
	oriCondition		trigger定义的赋值方法
	lenOfOriCondition	trigger定义的赋值方法长度
	recStr			操作使用的实际条件
	lenOfRecStr		操作使用的实际条件的长度
	sizeOfDesCondition	目标条件缓冲的大小
输出参数
	desCondition		目标条件
返回值
	>= 0			目标条件的长度
	<0			出错代码
*/
int UnionFormConditionFromTriggerDefinedStr(char *oriCondition,int lenOfOriCondition,char *recStr,int lenOfRecStr,char *desCondition,int sizeOfDesCondition)
{
	int		index;
	int		offset;
	int		ret;
	TUnionRec	recAsignDef;
	TUnionRec	recValue;
	int		asignFldNum;
	char		fldNameInRecStr[128+1];
	
	if ((oriCondition == NULL) || (desCondition == NULL) || (lenOfOriCondition < 0) || (recStr == NULL) || (lenOfRecStr < 0))
	{
		UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: parameter error!\n");
		return(errCodeParameter);
	}
	UnionDebugLog("in UnionFormConditionFromTriggerDefinedStr::\noriCondition=[%04d][%s]\n",lenOfOriCondition,oriCondition);
	UnionDebugNullLog("recStr=[%04d][%s]\n",lenOfRecStr,recStr);
	if (lenOfOriCondition == 0)	// 源条件为空
	{
		strcpy(desCondition,"");
		return(0);
	}
	// 将源条件中的域分隔符转换为通用分隔符
	if ((ret = UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator(oriCondition,lenOfOriCondition,conMenuDefinedQueryConditionFldSeperator,oriCondition,lenOfOriCondition+1)) < 0)
	{
		UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator!\n");
		return(ret);
	}
	// 从源条件中读取赋值方法
	memset(&recAsignDef,0,sizeof(recAsignDef));
	if ((asignFldNum = UnionReadRecFromRecStr(oriCondition,lenOfOriCondition,&recAsignDef)) < 0)
	{
		UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: UnionReadRecFromRecStr!\n");
		return(asignFldNum);
	}
	// 根据赋值条件赋值
	memset(&recValue,0,sizeof(recValue));
	for (index = 0; index < recAsignDef.fldNum; index++)
	{
		strcpy(recValue.fldName[index],recAsignDef.fldName[index]);
		if (strstr(recAsignDef.fldValue[index],conMenuDefinedQueryConditionReadThisFld) == NULL)	// 定义成了固定值
		{
			strcpy(recValue.fldValue[index],recAsignDef.fldValue[index]);
			continue;
		}
		// 定义成了取可变值
		if (strlen(recAsignDef.fldValue[index]+strlen(conMenuDefinedQueryConditionReadThisFld)) >= sizeof(fldNameInRecStr))
		{
			UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: fld alais name [%s] too long!\n",recAsignDef.fldValue[index]+strlen(conMenuDefinedQueryConditionReadThisFld));
			return(errCodeParameter);
		}
                // 从记录窗口读赋值数据
		strcpy(fldNameInRecStr,recAsignDef.fldValue[index]+strlen(conMenuDefinedQueryConditionReadThisFld));
		if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,fldNameInRecStr,recValue.fldValue[index],sizeof(recValue.fldValue[index]))) < 0)
		{
			UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: UnionReadRecFldFromRecStr [%s]\n",fldNameInRecStr);
			return(ret);
		}
	}
	recValue.fldNum = recAsignDef.fldNum;
	if ((ret = UnionPutRecIntoRecStr(&recValue,desCondition,sizeOfDesCondition)) < 0)
	{
		UnionUserErrLog("in UnionFormConditionFromTriggerDefinedStr:: UnionPutRecIntoRecStr!\n");
		return(ret);
	}
        desCondition[ret] = 0;
	UnionDebugLog("in UnionFormConditionFromTriggerDefinedStr::\ndesCondition=[%04d][%s]\n",ret,desCondition);
	return(ret);
}

/*
功能：
	执行触发器动作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
	fileRecved	是否收到了文件
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationInStr(char *triggerStr,int lenOfTriggerStr,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	resName[128+1];
	char	oriCondition[1024+1];
	char	desCondition[1024+1];
	int	operationID;
	int	lenOfDesCondition;

	if (UnionIsOperationOnTrigger())	// 当前动作是触发器触发的动作
		return(0);
		
	// 读取一行触发器操作
	memset(oriCondition,0,sizeof(oriCondition));
	memset(resName,0,sizeof(resName));
	if ((ret = UnionReadComplexDBOperationDefFromStr(triggerStr,lenOfTriggerStr,resName,sizeof(resName),&operationID,oriCondition,sizeof(oriCondition))) < 0)
	{
		UnionUserErrLog("in UnionExcuteTriggerOperationInStr:: UnionReadComplexDBOperationDefFromStr from [%s]\n",triggerStr);
		return(ret);
	}
	// 转换获得操作条件
	memset(desCondition,0,sizeof(desCondition));
	if ((lenOfDesCondition = UnionFormConditionFromTriggerDefinedStr(oriCondition,strlen(oriCondition),recStr,lenOfRecStr,desCondition,sizeof(desCondition))) < 0)
	{
		UnionUserErrLog("in UnionExcuteTriggerOperationInStr:: UnionFormConditionFromTriggerDefinedStr from [%s] and from [%s]\n",triggerStr,recStr);
		return(lenOfDesCondition);
	}
	// 执行操作
	UnionOpenOperationOnTrigger();
	if ((ret = UnionExcuteDBSvrOperation(resName,operationID,desCondition,lenOfDesCondition,resStr,sizeOfResStr,fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionExcuteTriggerOperationInStr:: UnionExcuteDBSvrOperation [%s] on [%s]\n",desCondition,resName);
		UnionCloseOperationOnTrigger();
		return(ret);
	}
	UnionCloseOperationOnTrigger();
	return(ret);
}

/*
功能：
	执行一个文件中的触发器操作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
	fileRecved	是否收到了文件
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationInFile(char *fileName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	lineStr[2048+1];
	FILE	*fp;
	int	lineLen;
	int	offset = 0;

	if (UnionIsOperationOnTrigger())	// 当前动作是触发器触发的动作
		return(0);
	
	// UnionLog("in UnionExcuteTriggerOperationInFile, fileName: [%s], recStr: [%s].\n", fileName, recStr);
	/* Mary delete, 20081211
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionExcuteTriggerOperationInFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	*/
	// Mary add begin, 20081211
	if ((fp = UnionOpenRecFileHDL(fileName)) == NULL)
	{
		UnionUserErrLog("in UnionExcuteTriggerOperationInFile:: UnionOpenRecFileHDL for [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// Mary add end, 20081211
	
	// 逐行读触发器定义
	while (!feof(fp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionUserErrLog("in UnionExcuteTriggerOperationInFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			ret = lineLen;
			goto errExit;
		}
		if (lineLen == 0)	// 空行
			continue;
		// 执行触发器动作
		if ((ret = UnionExcuteTriggerOperationInStr(lineStr,lineLen,recStr,lenOfRecStr,resStr+offset,sizeOfResStr-offset,fileRecved)) < 0)
		{
			UnionUserErrLog("in UnionExcuteTriggerOperationInFile:: UnionExcuteTriggerOperationInStr from [%s] in [%s]\n",lineStr,fileName);
			goto errExit;
		}
		offset += ret;
	}
	ret = offset;
errExit:
	//fclose(fp);	Mary modify, 20081211
	UnionCloseRecFileHDLOfFileName(fp,fileName);
	return(ret);
}

/*
功能：
	执行一个表的触发器操作
输入参数：
	resName		对象名称
	operationTag	操作标识
	beforeOrAfter	是操作之前执行还是之后执行
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
	fileRecved	是否收到了文件
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationOnObject(char *resName,char *operationTag,char *beforeOrAfter,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	fileName[512+1];
		
	// 判断触发器定义件是否存在
	if ((ret = UnionGetObjectTriggerFileName(resName,operationTag,beforeOrAfter,fileName)) <= 0)
		return(ret);
	
	// UnionLog("in UnionExcuteTriggerOperationOnObject, fileName is: [%s].\n", fileName);
	
	return(UnionExcuteTriggerOperationInFile(fileName,recStr,lenOfRecStr,recStr,sizeOfResStr,fileRecved));
}

/*
功能：
	执行一个表的触发器操作
输入参数：
	resName		对象名称
	operationID	操作标识
	isBeforeOperation	是操作之前执行还是之后执行，1 表示操作之前
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr)
{
	char	operationTag[64+1];
	char	beforeOrAfter[20];
	char	resStr[4096+1];
	int	fileRecved;
	
	if (UnionIsOperationOnTrigger())	// 当前动作是触发器触发的动作
		return(0);
	if (isBeforeOperation)
		strcpy(beforeOrAfter,"before");
	else
		strcpy(beforeOrAfter,"after");
	
	// UnionLog("in UnionExcuteTriggerOperationOnTable, beforeOrAfter: [%s].\n", beforeOrAfter);
	
	switch (operationID)
	{
		case	conDatabaseCmdInsert:	// 插入记录
			strcpy(operationTag,"insert");
			break;
		case	conDatabaseCmdDelete:	// 删除记录
			strcpy(operationTag,"delete");
			break;
		case	conDatabaseCmdConditionDelete:	// 条件删除记录
			strcpy(operationTag,"deleteAll");
			break;
		case	conDatabaseCmdUpdate:	// 修改记录
			strcpy(operationTag,"update");
			break;
		case	conDatabaseCmdConditionUpdate:	// 条件修改记录
			strcpy(operationTag,"updateAll");
			break;
		default:
			return(0);
	}
	
	// UnionLog("in UnionExcuteTriggerOperationOnTable, operationTag is: [%s].\n", operationTag);
	memset(resStr,0,sizeof(resStr));
	return(UnionExcuteTriggerOperationOnObject(resName,operationTag,beforeOrAfter,recStr,lenOfRecStr,resStr,sizeof(resStr),&fileRecved));
}

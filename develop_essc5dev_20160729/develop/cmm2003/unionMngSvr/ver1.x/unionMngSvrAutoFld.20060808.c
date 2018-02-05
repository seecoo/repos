// 2008/7/26
// Wolfang Wang

#ifdef _WIN32
#include <vcl.h>
#include <windows.h>
#include "dir.h"
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "operationControl.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "UnionLog.h"			// 使用3.x版本
#include "unionWorkingDir.h"
#include "mngSvrCommProtocol.h"
#include "unionMngSvrOperationType.h"
#include "mngSvrFileSvr.h"
#include "unionResID.h"
#include "unionREC.h"
#include "mngSvrTeller.h"
#include "simuMngSvrLocally.h"

#ifdef _WIN32
#include "unionTBLQueryInterface.h"
#include "unionDBConf.h"
#endif

void UnionGetAutoAppendDBRecInputAttr(char *inputTeller,char *inputDate,char *inputTime)
{
	strcpy(inputTeller,UnionGetCurrentOperationTellerNo());
	strcpy(inputDate,UnionGetCurrentFullSystemDate());
	strcpy(inputTime,UnionGetCurrentSystemTime());
	//UnionLog("in UnionGetAutoAppendDBRecInputAttr:: [%s][%s][%s]\n",inputTeller,inputDate,inputTime);
}

void UnionGetAutoAppendDBRecUpdateAttr(char *updateTeller,char *updateDate,char *updateTime)
{
	strcpy(updateTeller,UnionGetCurrentOperationTellerNo());
	strcpy(updateDate,UnionGetCurrentFullSystemDate());
	strcpy(updateTime,UnionGetCurrentSystemTime());
	//UnionLog("in UnionGetAutoAppendDBRecUpdateAttr:: [%s][%s][%s]\n",updateTeller,updateDate,updateTime);
}

/*
功能
	自动在记录后附加记录输入属性
输入参数
	recStr		记录串
	lenOfRecStr	记录长度
	sizeOfRecStr	记录串缓冲大小
输入出数
	recStr	记录串
返回值
	>=0	附加了新串的值长度
	<0	错误码
*/

int UnionAutoAppendDBRecInputAttr(char *recStr,int lenOfRecStr,int sizeOfRecStr)
{
	int	ret;
	
	// 2010-1-22,wolfgang wang added
	if (UnionIsBackuperMngSvr())
		return(lenOfRecStr);
	// end of addition of 2010-1-22

	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "inputDate", UnionGetCurrentFullSystemDate())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecInputAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}
	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "inputTime", UnionGetCurrentSystemTime())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecInputAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}
	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "inputTeller", UnionGetCurrentOperationTellerNo())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecInputAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}
	return (ret);
}

/*
功能
	自动在记录后附加记录修改属性
输入参数
	recStr		记录串
	lenOfRecStr	记录长度
	sizeOfRecStr	记录串缓冲大小
输入出数
	recStr	记录串
返回值
	>=0	附加了新串的值长度
	<0	错误码
*/
int UnionAutoAppendDBRecUpdateAttr(char *recStr,int lenOfRecStr,int sizeOfRecStr)
{
	int	ret;
	ret = lenOfRecStr;

	// 2010-1-22,wolfgang wang added
	if (UnionIsBackuperMngSvr())
		return(lenOfRecStr);
	// end of addition of 2010-1-22

	if ( (ret = UnionDeleteSpecFldOnUnionRec(recStr, ret, "inputDate")) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionDeleteSpecFldOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionDeleteSpecFldOnUnionRec(recStr, ret, "inputTime")) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionDeleteSpecFldOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionDeleteSpecFldOnUnionRec(recStr, ret, "inputTeller")) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionDeleteSpecFldOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "updateDate", UnionGetCurrentFullSystemDate())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "updateTime", UnionGetCurrentSystemTime())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}

	if ( (ret = UnionUpdateSpecFldValueOnUnionRec(recStr, sizeOfRecStr, "updateTeller", UnionGetCurrentOperationTellerNo())) < 0)
	{
		UnionUserErrLog("in UnionAutoAppendDBRecUpdateAttr:: UnionUpdateSpecFldValueOnUnionRec!\n");
		return(ret);
	}

	return (ret);
}

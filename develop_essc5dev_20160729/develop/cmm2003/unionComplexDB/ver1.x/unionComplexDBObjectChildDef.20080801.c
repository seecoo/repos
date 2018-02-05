//	Author: ChenJiaMei
//	Date: 2008-7-31

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
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectChildDef.h"

/*
功能：检查一个对象的外部引用对象登记文件是否存在
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectForeignObjDefFile(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];

	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectForeignObjDefFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象的外部引用对象登记文件是否存在
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
功能：创建一个对象的外部引用对象登记文件
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionCreateObjectForeignObjDefFile(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectForeignObjDefFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的外部引用对象登记文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);
	
	// 检查对象的外部引用对象登记文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionCreateObjectForeignObjDefFile:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// 创建对象的外部引用对象登记文件
	iRet=UnionWriteRecStrIntoFile(caFileName,"",0);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectForeignObjDefFile:: create file [%s] fail!\n",caFileName);
		return(iRet);
	}

	return(0);
}

/*
功能：检查是否有其它对象引用了本对象
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	>0：有其它对象引用本对象
	0：没有其它对象引用本对象
	<0：失败，错误码
*/
int UnionExistForeignObjectInfo(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];
	int	iNum;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistForeignObjectInfo:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);
	
	iNum=UnionGetTotalRecNumInFile(caFileName);
	if (iNum < 0)
	{
		UnionUserErrLog("in UnionExistForeignObjectInfo:: UnionGetTotalRecNumInFile fail! return=[%d]\n",iNum);
		return(iNum);
	}
	
	return(iNum);
}

/*
功能：登记引用本对象的外部对象的引用信息
输入参数：
	idOfObject：本对象ID
	foreignObjectGrp：引用本对象的外部对象关键字定义
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionRegisterForeignObjectInfo(TUnionIDOfObject idOfObject,TUnionChildObjectGrp foreignObjectGrp)
{
	char	caFileName[512],caRecord[512],caBuf[512];
	int	iRet,i,iRecLen=0;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionRegisterForeignObjectInfo:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);

	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionAuditLog("in UnionRegisterForeignObjectInfo:: [%s] does not exists!\n",caFileName);
		if ((iRet = UnionExistAndCreateFile(caFileName,"")) < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: [%s] does not exists!\n",caFileName);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
		}
	}

	for (i=0;i<foreignObjectGrp.childNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=0;
		
		// 本地域组
                // UnionLog("In UnionRegisterForeignObjectInfo turn to UnionFormFieldNameStrFromFldGrp, and teh index is: [%d].\n", i);
                
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(foreignObjectGrp.childObjectGrp[i]).localFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionFormFieldNameStrFromFldGrp for localFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("localFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] localFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 外部对象名称
		iRet=UnionPutRecFldIntoRecStr("foreignObject",(foreignObjectGrp.childObjectGrp[i]).objectName,strlen((foreignObjectGrp.childObjectGrp[i]).objectName),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] foreignObject fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 外部域组
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(foreignObjectGrp.childObjectGrp[i]).foreignFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionFormFieldNameStrFromFldGrp for foreignFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("foreignFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] foreignFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		
		iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionRegisterForeignObjectInfo:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
	}
	
	return(0);
}

/*
功能：取消引用本对象的外部对象引用信息的登记
输入参数：
	idOfObject：本对象ID
	foreignObjectGrp：引用本对象的外部对象关键字定义
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCancelRegisterOfForeignObjectInfo(TUnionIDOfObject idOfObject,TUnionChildObjectGrp foreignObjectGrp)
{
	char	caFileName[512],caRecord[512],caBuf[512];
	int	iRet,i,iRecLen=0;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectChildrenDefFileName(idOfObject,caFileName);
	
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	for (i=0;i<foreignObjectGrp.childNum;i++)
	{
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=0;
		
		// 本地域组
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(foreignObjectGrp.childObjectGrp[i]).localFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionFormFieldNameStrFromFldGrp for localFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("localFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] localFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 外部对象名称
		iRet=UnionPutRecFldIntoRecStr("foreignObject",(foreignObjectGrp.childObjectGrp[i]).objectName,strlen((foreignObjectGrp.childObjectGrp[i]).objectName),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] foreignObject fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		// 外部域组
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(foreignObjectGrp.childObjectGrp[i]).foreignFldGrp,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionFormFieldNameStrFromFldGrp for foreignFldGrp fail! return=[%d]\n",iRet);
			return(iRet);
		}
		iRet=UnionPutRecFldIntoRecStr("foreignFldGrp",caBuf,iRet,&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionPutRecFldIntoRecStr for [%d] foreignFldGrp fail!\n",i);
			return(iRet);
		}
		iRecLen += iRet;
		
		iRet = UnionDeleteRecStrFromFile(caFileName,caRecord,iRecLen);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCancelRegisterOfForeignObjectInfo:: UnionDeleteRecStrFromFile for [%s] fail! return=[%d]\n",caRecord,iRet);
			return(iRet);
		}
	}
	
	return(0);
}

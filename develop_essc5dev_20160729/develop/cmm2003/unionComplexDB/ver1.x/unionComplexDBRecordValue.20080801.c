//	Author: ChenJiaMei
//	Date: 2008-8-6

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
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBRecordValue.h"
#include "unionRec0.h"
#include "unionDataImageInMemory.h"

/*
功能：检查一个对象实例的值文件是否存在
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectRecValueFile(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512];

	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectRecValueFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象实例的值文件是否存在
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
功能：新增一个对象实例的值文件
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	record：新记录，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：新记录的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionInsertObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL || primaryKey == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecordValue:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 创建对象实例的存储目录
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecDirName(idOfObject,primaryKey,caFileName);
	if (!UnionExistsDir(caFileName))
	{
		iRet=UnionCreateDir(caFileName);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionInsertObjectRecordValue:: UnionCreateDir for [%s] fail! return=[%d]\n",caFileName,iRet);
			return(iRet);
		}
	}
	
	// 获得对象实例的值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	// 检查对象实例的值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecordValue:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	iRet=UnionAppendRecStrToFile(caFileName,record,lenOfRecord);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionInsertObjectRecordValue:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",record,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：根据关键字删除一个对象实例的值文件
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512];
	int	iRet;

	// 从映象区删除数据,2009/1/8,王纯军增加
	if (UnionIsUseDataImageInMemory(idOfObject,primaryKey))
	{
		if ((iRet = UnionDeleteRecDataImageInMemory(idOfObject,primaryKey)) < 0)
		{
			UnionAuditLog("in UnionDeleteObjectRecordValue:: UnionDeleteRecDataImageInMemory [%s]\n",idOfObject,primaryKey);
			// 注意,出错时不返回,因为数据可能在映像区不存在
		}
	}
	// 2009/1/8,王纯军增加结束
	
	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteObjectRecordValue:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象实例的值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	// 检查对象实例的值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectRecordValue:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	iRet=UnionDeleteRecFile(caFileName);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectRecordValue:: UnionDeleteRecFile fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：修改一个对象实例的值文件
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	record：要修改的记录，格式为"域1=域值|域2=域值|…|域N=域值"，只包括要修改的域
	lenOfRecord：要修改的记录的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionUpdateObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord)
{
	char	caFileName[512],caRecStr[4096],caTmpVar[4096];
	int	iRet,iRecStrLen,i;
	TUnionRec	sOriRec,sNewRec;

	if (idOfObject == NULL || primaryKey == NULL || record == NULL || lenOfRecord <= 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象实例的值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	// 检查对象实例的值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	// 读原始记录
	memset(caRecStr,0,sizeof(caRecStr));
	iRecStrLen=UnionReadRecStrFromFile(caFileName,caRecStr,sizeof(caRecStr));
	if (iRecStrLen < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionReadRecStrFromFile fail! return=[%d]\n",iRecStrLen);
		return(iRecStrLen);
	}
	
	// 将原来的域读入到结构中
	memset(&sOriRec,0,sizeof(sOriRec));
	if ((iRet = UnionReadRecFromRecStr(caRecStr,iRecStrLen,&sOriRec)) < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionReadRecFromRecStr from [%d] [%s]\n",iRecStrLen,caRecStr);
		return(iRet);
	}
	
	// 将要更改的域读入到结构中
	memset(&sNewRec,0,sizeof(sNewRec));
	if ((iRet = UnionReadRecFromRecStr(record,lenOfRecord,&sNewRec)) < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionReadRecFromRecStr from [%d] [%s]\n",lenOfRecord,record);
		return(iRet);
	}
		
	// 将更改的域写入到串中
	memset(caRecStr,0,sizeof(caRecStr));	
	// deleted by Wolfgang Wang,2009/1/9
	/*
	if ((iRecStrLen = UnionPutRecIntoRecStr(&sNewRec,caRecStr,sizeof(caRecStr))) < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionPutRecIntoRecStr fail! return=[%d]\n",iRet);
		return(iRet);
	}
	*/
	
	// added by wolfgang wang 2009/1/9
	iRecStrLen = 0;
	for (i = 0; i < sNewRec.fldNum; i++)
	{
		//if (UnionIsPrimaryKeyFld(idOfObject, sNewRec.fldName[i]) == 1)
		if ((UnionIsPrimaryKeyFld(idOfObject, sNewRec.fldName[i]) == 1) || (UnionIsUniqueFld(idOfObject,sNewRec.fldName[i]) > 0))
			continue;
		if ((iRet = UnionPutRecFldIntoRecStr(sNewRec.fldName[i],sNewRec.fldValue[i],strlen(sNewRec.fldValue[i]),caRecStr+iRecStrLen,sizeof(caRecStr)-iRecStrLen)) < 0)
		{
			UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionPutRecFldIntoRecStr [%s]! return=[%d]\n",sNewRec.fldName[i],iRet);
			return(iRet);
		}
		iRecStrLen += iRet;
	}
	// end of addition of 2009/1/9
	
	// 将未改的域置入到记录串中
	for (i = 0; i < sOriRec.fldNum; i++)
	{
		if ((iRet = UnionReadRecFldFromRecStr(caRecStr,iRecStrLen,sOriRec.fldName[i],caTmpVar,sizeof(caTmpVar))) >= 0)
			continue;	// 是更改了的域
		if ((iRet = UnionPutRecFldIntoRecStr(sOriRec.fldName[i],sOriRec.fldValue[i],strlen(sOriRec.fldValue[i]),caRecStr+iRecStrLen,sizeof(caRecStr)-iRecStrLen)) < 0)
		{
			UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionPutRecFldIntoRecStr [%s]! return=[%d]\n",sOriRec.fldName[i],iRet);
			return(iRet);
		}
		iRecStrLen += iRet;
	}
	
	if ((iRet = UnionWriteRecStrIntoFile(caFileName,caRecStr,iRecStrLen)) < 0)
	{
		UnionUserErrLog("in UnionUpdateObjectRecordValue:: UnionWriteRecStrIntoFile [%s] to [%s] fail! return=[%d]\n",caRecStr,caFileName,iRet);
		return(iRet);
	}
	
	// 修改映象区中的数据,2009/1/8,王纯军增加
	if (UnionIsUseDataImageInMemory(idOfObject,primaryKey))
	{
		if ((iRet = UnionModifyRecDataImageInMemory(idOfObject,primaryKey,caRecStr,iRecStrLen)) < 0)
		{
			UnionAuditLog("in UnionUpdateObjectRecordValue:: UnionModifyRecDataImageInMemory [%s]\n",idOfObject,primaryKey);
			// 注意,出错时不返回,因为数据可以不在映像区中存在
			if (UnionExistRecDataImageInMemory(idOfObject,primaryKey))	// 映像区中存在数据
				UnionDeleteRecDataImageInMemory(idOfObject,primaryKey);	// 修改在映像区中的数据失败,删除在映像区中的数据
		}
	}
	// 2009/1/8,王纯军增加结束
	return(0);
}

/*
功能：根据关键字查询一个对象实例的值文件
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	sizeOfRecord：record的存储空间大小
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
*/
int UnionSelectObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord)
{
	char	caFileName[512];
	int	iRet;
	int	ret;
	
	if (idOfObject == NULL || primaryKey == NULL || record == NULL || sizeOfRecord <= 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordValue:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 先从映象区中读取数据,2009/1/8,王纯军增加
	if (UnionIsUseDataImageInMemory(idOfObject,primaryKey))
	{
		if ((iRet = UnionReadRecDataImageInMemory(idOfObject,primaryKey,record,sizeOfRecord)) >= 0)
			return(iRet);
	}
	// 2009/1/8,王纯军增加结束

	// 获得对象实例的值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecValueFileName(idOfObject,primaryKey,caFileName);
	
	// 检查对象实例的值文件是否存在
	iRet = UnionExistsFile(caFileName);
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordValue:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	iRet=UnionReadRecStrFromFile(caFileName,record,sizeOfRecord);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionSelectObjectRecordValue:: UnionReadRecStrFromFile fail! return=[%d]\n",iRet);
		return(iRet);
	}

	// 加载到映象区,2009/1/8,王纯军增加
	if (UnionIsUseDataImageInMemory(idOfObject,primaryKey))
	{	
		if (UnionExistRecDataImageInMemory(idOfObject,primaryKey))	// 映像区中存在数据
		{
			if ((ret = UnionModifyRecDataImageInMemory(idOfObject,primaryKey,record,iRet)) < 0)	// 修改失败
			{
				UnionAuditLog("in UnionSelectObjectRecordValue:: UnionModifyRecDataImageInMemory fail! return=[%d]\n",ret);
				UnionDeleteRecDataImageInMemory(idOfObject,primaryKey);	// 修改在映像区中的数据失败,删除在映像区中的数据
			}
		}
		else
		{
			if ((ret = UnionInsertRecDataImageInMemory(idOfObject,primaryKey,record,iRet)) < 0)	// 增加失败
				UnionAuditLog("in UnionSelectObjectRecordValue:: UnionInsertRecDataImageInMemory fail! return=[%d]\n",ret);
		}
	}
	// 2009/1/8,王纯军增加结束
	return(iRet);
}

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
#include "unionComplexDBCommon.h"
#include "unionComplexDBUniqueKey.h"

/*
功能：检查一个对象的某唯一值域组文件是否存在
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectUniqueKeyIndexFile(TUnionIDOfObject idOfObject,char *fldGrpName)
{
	char	caFileName[512];

	if (idOfObject == NULL || fldGrpName == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectUniqueKeyIndexFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象的某唯一值域组文件是否存在
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
功能：创建一个对象的某唯一值域组文件
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionCreateObjectUniqueKeyIndexFile(TUnionIDOfObject idOfObject,char *fldGrpName)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL || fldGrpName == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectUniqueKeyIndexFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的某唯一值域组文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// 检查对象的某唯一值域组文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionCreateObjectUniqueKeyIndexFile:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// 创建对象的某唯一值域组文件
	iRet=UnionWriteRecStrIntoFile(caFileName,"",0);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectUniqueKeyIndexFile:: create file [%s] fail!\n",caFileName);
		return(iRet);
	}

	return(0);
}

/*
功能：在一个对象的某唯一值域组文件中增加一条新记录的唯一值
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	uniqueKey：新记录的唯一值，格式为"唯一值域1的值.唯一值域2的值.唯一值域3的值…唯一值域N的值"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	storePosition：新记录的存储位置
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionAddObjectUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey,char *storePosition)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0;

	if (idOfObject == NULL || fldGrpName == NULL || uniqueKey == NULL || storePosition == NULL)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的某唯一值域组文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// 检查对象的某唯一值域组文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	memset(caRecord,0,sizeof(caRecord));
	iRet=iRet=UnionPutRecFldIntoRecStr("fldGrp",uniqueKey,strlen(uniqueKey),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: UnionPutRecFldIntoRecStr for uniqueKey [%s] fail!\n",uniqueKey);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=iRet=UnionPutRecFldIntoRecStr("fileName",storePosition,strlen(storePosition),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: UnionPutRecFldIntoRecStr for storePosition [%s] fail!\n",storePosition);
		return(iRet);
	}
	iRecLen += iRet;
	
	iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectUniqueKey:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：在一个对象的某唯一值域组文件中删除一条记录的唯一值
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	uniqueKey：要删除记录的唯一值，格式为"唯一值域1的值.唯一值域2的值.唯一值域3的值…唯一值域N的值"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteObjectUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey)
{
	char	caFileName[512],caBuf[1024];
	int	iRet;

	if (idOfObject == NULL || fldGrpName == NULL || uniqueKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的某唯一值域组文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// 检查对象的某唯一值域组文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// 检查对象的某唯一值域组文件中是否有该记录
	iRet=UnionObjectRecUniqueKeyIsRepeat(idOfObject,fldGrpName,uniqueKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: UnionObjectRecUniqueKeyIsRepeat fail! reutnr=[%d]\n",iRet);
		return(iRet);
	}
	else if (iRet == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: [%s] does not exists!\n",uniqueKey);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	memset(caBuf,0,sizeof(caBuf));
	sprintf(caBuf,"fldGrp=%s",uniqueKey);
	//iRet = UnionDeleteRecStrFromFile(caFileName,caBuf,strlen(caBuf));	// Mary delete, 20081105
	iRet = UnionDeleteRecStrFromFileByFld(caFileName,caBuf,strlen(caBuf),"fldGrp");	// Mary add, 20081105
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectUniqueKey:: UnionDeleteRecStrFromFileByFld for unique key [%s] fail! return=[%d]\n",uniqueKey,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：根据对象实例的唯一值获得实例的存储位置
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	uniqueKey：记录的唯一值，格式为"唯一值域1的值.唯一值域2的值.唯一值域3的值…唯一值域N的值"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	sizeofStorePosition：storePosition的存储空间大小
输出参数：
	storePosition：记录的存储位置
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionGetObjectRecStoreStationByUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey,char *storePosition,int sizeofStorePosition)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0,iLen;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || fldGrpName == NULL || uniqueKey == NULL || storePosition == NULL || sizeofStorePosition <= 0)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// 获得对象的某唯一值域组文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// 检查对象的某唯一值域组文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// 读取文件中的一行，已经去掉了注释行
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// 读文件结束
			break;
		
		// Mary add begin, 20081125
		for (iLen=0;iLen<iRecLen;iLen++)
		{
			if (caRecord[iLen+7] == '|')
				break;
		}
		if (iLen != strlen(uniqueKey))
			continue;			
		// Mary add end, 20081125
		
		if (strncmp(&caRecord[7],uniqueKey,strlen(uniqueKey)) == 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			iRet=UnionReadRecFldFromRecStr(caRecord,iRecLen,"fileName",storePosition,sizeofStorePosition);
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
				return(iRet);
			}
			return(0);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	UnionUserErrLog("in UnionGetObjectRecStoreStationByUniqueKey:: uniqueKey [%s] is not found!\n",uniqueKey);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectRecordNotExist));
}

/*
功能：判断对象实例的唯一值是否重复
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	uniqueKey：记录的唯一值，格式为"唯一值域1的值.唯一值域2的值.唯一值域3的值…唯一值域N的值"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
输出参数：
	无
返回值：
	1：重复
	0：不重复
	<0：失败，错误码
*/
int UnionObjectRecUniqueKeyIsRepeat(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0,iLen;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || fldGrpName == NULL || uniqueKey == NULL)
	{
		UnionUserErrLog("in UnionObjectRecUniqueKeyIsRepeat:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的某唯一值域组文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecUniqueKeyIndexFileName(idOfObject,fldGrpName,caFileName);
	
	// 检查对象的某唯一值域组文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionObjectRecUniqueKeyIsRepeat:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionObjectRecUniqueKeyIsRepeat:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// 读取文件中的一行，已经去掉了注释行
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionObjectRecUniqueKeyIsRepeat:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// 读文件结束
			break;
		
		// Mary add begin, 20081201
		for (iLen=0;iLen<iRecLen;iLen++)
		{
			if (caRecord[iLen+7] == '|')
				break;
		}
		if (iLen != strlen(uniqueKey))
			continue;			
		// Mary add end, 20081201
		
		if (strncmp(&caRecord[7],uniqueKey,strlen(uniqueKey)) == 0)
		{
			UnionAuditLog("in UnionObjectRecUniqueKeyIsRepeat:: uniqueKey is repeat! record=[%s]\n",caRecord);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(1);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	return(0);
}

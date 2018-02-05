//	Author: ChenJiaMei
//	Date: 2008-8-7

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
#include "unionComplexDBRecordParents.h"

/*
功能：检查一个对象实例的内部引用文件是否存在
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
int UnionExistsObjectRecParentsFile(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512];

	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectRecParentsFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象实例的内部引用文件是否存在
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfObject,primaryKey,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
功能：创建一个对象实例的内部引用文件
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
int UnionCreateObjectRecParentsFile(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectRecParentsFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象实例的内部引用文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfObject,primaryKey,caFileName);
	
	// 检查对象实例的内部引用文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionCreateObjectRecParentsFile:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// 创建对象实例的内部引用文件
	iRet=UnionWriteRecStrIntoFile(caFileName,"",0);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectRecParentsFile:: create file [%s] fail!\n",caFileName);
		return(iRet);
	}

	return(0);
}

/*
功能：在一个对象实例的内部引用文件中删除一条记录
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	localFldGrp：本对象的本地域组的域名，格式为"本地域名1.本地域名2.本地域名3…本地域名N"，
		本地域名的排列顺序与本对象定义文件中定义的本地域组顺序一致
	idOfForeignObject：外部对象的ID
	foreignPrimaryKey：本对象引用的外部对象实例的关键字域值，
		格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDelObjectRecParentsFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0;
	
	if (idOfLocalObject == NULL || localPrimaryKey == NULL || localFldGrp == NULL || idOfForeignObject == NULL || foreignPrimaryKey == NULL)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象实例的内部引用文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfLocalObject,localPrimaryKey,caFileName);
	
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=0;
	iRet=UnionPutRecFldIntoRecStr("localfldGrp",localFldGrp,strlen(localFldGrp),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: UnionPutRecFldIntoRecStr for local field group [%s] fail!\n",localFldGrp);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignObject",idOfForeignObject,strlen(idOfForeignObject),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: UnionPutRecFldIntoRecStr for foreign object name [%s] fail!\n",idOfForeignObject);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignPrimaryKey",foreignPrimaryKey,strlen(foreignPrimaryKey),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: UnionPutRecFldIntoRecStr for foreign field value [%s] fail!\n",foreignPrimaryKey);
		return(iRet);
	}
	iRecLen += iRet;

        UnionProgramerLog("in UnionDelObjectRecParentsFile, try to delete file: [%s] record: [%s].\n", caFileName, caRecord);
        
	// iRet = UnionDeleteRecStrFromFile(caFileName, caRecord, iRecLen);
        iRet = UnionDeleteRecStrFromFileByFld(caFileName, caRecord, iRecLen, "foreignPrimaryKey");
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDelObjectRecParentsFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：在一个对象实例的内部引用文件中增加一条记录
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	localFldGrp：本对象的本地域组的域名，格式为"本地域名1.本地域名2.本地域名3…本地域名N"，
		本地域名的排列顺序与本对象定义文件中定义的本地域组顺序一致
	idOfForeignObject：外部对象的ID
	foreignPrimaryKey：本对象引用的外部对象实例的关键字域值，
		格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionAddObjectRecParentsFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0;
	
	if (idOfLocalObject == NULL || localPrimaryKey == NULL || localFldGrp == NULL || idOfForeignObject == NULL || foreignPrimaryKey == NULL)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象实例的内部引用文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfLocalObject,localPrimaryKey,caFileName);
	
	memset(caRecord,0,sizeof(caRecord));
	iRecLen=0;
	iRet=UnionPutRecFldIntoRecStr("localfldGrp",localFldGrp,strlen(localFldGrp),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: UnionPutRecFldIntoRecStr for local field group [%s] fail!\n",localFldGrp);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignObject",idOfForeignObject,strlen(idOfForeignObject),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: UnionPutRecFldIntoRecStr for foreign object name [%s] fail!\n",idOfForeignObject);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignPrimaryKey",foreignPrimaryKey,strlen(foreignPrimaryKey),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: UnionPutRecFldIntoRecStr for foreign field value [%s] fail!\n",foreignPrimaryKey);
		return(iRet);
	}
	iRecLen += iRet;

        UnionProgramerLog("in UnionAddObjectRecParentsFile, try to add record: [%s] to file: [%s].\n", caRecord, caFileName);
        
	iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecParentsFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：取得一个实例引用的对象实例的关键字
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	localFldGrp：本地域组名称，格式为"域1的名称.域2的名称.域3的名称…域N的名称"，
		域的排列顺序与对象定义文件中外部关键字的本地域组中域顺序一致
	idOfForeignObject：外部对象的名称
	sizeOfPrimaryKey：foreignPrimaryKey的存储空间大小
输出参数：
	foreignPrimaryKey：本对象引用的外部对象实例的关键字域值，
		格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
返回值：
	>=0：成功，返回foreignPrimaryKey的长度
	<0：失败，错误码
*/
int UnionObtainForeignObjectRecPrimaryKey(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey,int sizeOfPrimaryKey)
{
	char	caFileName[512],caRecord[1024],caBuf[1024];
	int	iRet,iRecLen=0,iBufLen=0;
	TUnionRecFileHDL	hFileHdl;

	if (idOfLocalObject == NULL || localPrimaryKey == NULL || localFldGrp == NULL || idOfForeignObject == NULL || foreignPrimaryKey == NULL || sizeOfPrimaryKey <= 0)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象实例的内部引用文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecParentsListFileName(idOfLocalObject,localPrimaryKey,caFileName);
	
	// 检查对象实例的内部引用文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	memset(caBuf,0,sizeof(caBuf));
	iBufLen=0;
	iRet=UnionPutRecFldIntoRecStr("localfldGrp",localFldGrp,strlen(localFldGrp),&caBuf[iBufLen],sizeof(caBuf)-iBufLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionPutRecFldIntoRecStr for local field group [%s] fail!\n",localFldGrp);
		return(iRet);
	}
	iBufLen += iRet;
	iRet=UnionPutRecFldIntoRecStr("foreignObject",idOfForeignObject,strlen(idOfForeignObject),&caBuf[iBufLen],sizeof(caBuf)-iBufLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionPutRecFldIntoRecStr for foreign object name [%s] fail!\n",idOfForeignObject);
		return(iRet);
	}
	iBufLen += iRet;
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// 读取文件中的一行，已经去掉了注释行
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// 读文件结束
			break;
		
		if (strncmp(caRecord,caBuf,iBufLen) == 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			iRet=UnionReadRecFldFromRecStr(caRecord,iRecLen,"foreignPrimaryKey",foreignPrimaryKey,sizeOfPrimaryKey);
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: UnionReadRecFldFromRecStr for foreignPrimaryKey fail! return=[%d]\n",iRet);
				return(iRet);
			}
			return(0);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	UnionUserErrLog("in UnionObtainForeignObjectRecPrimaryKey:: idOfForeignObject [%s] is not found!\n",idOfForeignObject);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectRecordNotExist));
}

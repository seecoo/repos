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
#include "unionComplexDBPrimaryKey.h"

/*
功能：检查一个对象的键值文件是否存在
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectPrimaryKeyIndexFile(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];

	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectPrimaryKeyIndexFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象的键值文件是否存在
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
功能：创建一个对象的键值文件
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionCreateObjectPrimaryKeyIndexFile(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectPrimaryKeyIndexFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// 检查对象的键值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) > 0)
	{
		UnionUserErrLog("in UnionCreateObjectPrimaryKeyIndexFile:: [%s] is already exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// 创建对象的键值文件
	iRet=UnionWriteRecStrIntoFile(caFileName,"",0);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectPrimaryKeyIndexFile:: create file [%s] fail!\n",caFileName);
		return(iRet);
	}

	return(0);
}

/*
功能：在一个对象的键值文件中增加一条新记录的键值
输入参数：
	idOfObject：对象ID
	primaryKey：新记录的键值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	storePosition：新记录的存储位置
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionAddObjectPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *storePosition)
{
	char	caFileName[512],caRecord[4096];
	int	iRet,iRecLen=0;

	if (idOfObject == NULL || primaryKey == NULL || storePosition == NULL)
	{
		UnionUserErrLog("in UnionAddObjectPaimaryKey:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// 检查对象的键值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionAddObjectPaimaryKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	memset(caRecord,0,sizeof(caRecord));
	iRet=iRet=UnionPutRecFldIntoRecStr("fldGrp",primaryKey,strlen(primaryKey),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectPaimaryKey:: UnionPutRecFldIntoRecStr for primary [%s] fail!\n",primaryKey);
		return(iRet);
	}
	iRecLen += iRet;
	iRet=iRet=UnionPutRecFldIntoRecStr("fileName",storePosition,strlen(storePosition),&caRecord[iRecLen],sizeof(caRecord)-iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectPaimaryKey:: UnionPutRecFldIntoRecStr for storePosition [%s] fail!\n",storePosition);
		return(iRet);
	}
	iRecLen += iRet;

        // modify by chenliang, 2008-10-10
	// iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
        // iRet = UnionInsertRecStrToFileInAscOrder(caFileName,caRecord,iRecLen);	// deleted by Wolfgang Wang, 2008/10/27
        // modify end.
        iRet = UnionInsertRecStrToFileInOrderByFld(caFileName,caRecord,iRecLen,"fldGrp",1);	// added by Wolfgang Wang, 2008/10/27
	if (iRet < 0)
	{
		// UnionUserErrLog("in UnionAddObjectPaimaryKey:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
                UnionUserErrLog("in UnionAddObjectPaimaryKey:: UnionInsertRecStrToFileInOrderByFld for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：在一个对象的键值文件中删除一条记录的键值
输入参数：
	idOfObject：对象ID
	primaryKey：要删除记录的键值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteObjectPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512],caBuf[4096];
	int	iRet;

	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// 检查对象的键值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// 检查对象的键值文件中是否有该记录
	iRet=UnionObjectRecPaimaryKeyIsRepeat(idOfObject,primaryKey);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: UnionObjectRecPaimaryKeyIsRepeat fail! reutnr=[%d]\n",iRet);
		return(iRet);
	}
	else if (iRet == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: [%s] does not exists!\n",primaryKey);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	memset(caBuf,0,sizeof(caBuf));
	sprintf(caBuf,"fldGrp=%s",primaryKey);

	//iRet = UnionDeleteRecStrFromFile(caFileName,caBuf,strlen(caBuf));	// 2008/10/27,wolfgang deleted
	iRet = UnionDeleteRecStrFromFileByFld(caFileName,caBuf,strlen(caBuf),"fldGrp");	// 2008/10/27,wolfgang added
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectPaimaryKey:: UnionDeleteRecStrFromFile for primary key [%s] fail! return=[%d]\n",primaryKey,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：根据对象实例的键值获得实例的存储位置
输入参数：
	idOfObject：对象ID
	primaryKey：记录的键值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	sizeofStorePosition：storePosition的存储空间大小
输出参数：
	storePosition：记录的存储位置
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionGetObjectRecStoreStationByPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *storePosition,int sizeofStorePosition)
{
	char	caFileName[512],caRecord[4096];
	int	iRet,iRecLen=0,iLen;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || primaryKey == NULL || storePosition == NULL || sizeofStorePosition <= 0)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// 检查对象的键值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// 读取文件中的一行，已经去掉了注释行
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
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
		if (iLen != strlen(primaryKey))
			continue;			
		// Mary add end, 20081125
		
		if (strncmp(&caRecord[7],primaryKey,strlen(primaryKey)) == 0)
		{
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			iRet=UnionReadRecFldFromRecStr(caRecord,iRecLen,"fileName",storePosition,sizeofStorePosition);
			if (iRet <= 0)
			{
				UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: UnionReadRecFldFromRecStr for fileName fail! return=[%d]\n",iRet);
				return(iRet);
			}
			return(0);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	UnionUserErrLog("in UnionGetObjectRecStoreStationByPaimaryKey:: primaryKey [%s] is not found!\n",primaryKey);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectRecordNotExist));
}

/*
功能：判断对象实例的键值是否重复
输入参数：
	idOfObject：对象ID
	primaryKey：记录的键值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	1：重复
	0：不重复
	<0：失败，错误码
*/
int UnionObjectRecPaimaryKeyIsRepeat(TUnionIDOfObject idOfObject,char *primaryKey)
{
	char	caFileName[512],caRecord[4096];
	int	iRet,iRecLen=0,iLen=0;
	TUnionRecFileHDL	hFileHdl;

	if (idOfObject == NULL || primaryKey == NULL)
	{
		UnionUserErrLog("in UnionObjectRecPaimaryKeyIsRepeat:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// 检查对象的键值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		if ((iRet = UnionExistAndCreateFile(caFileName,"")) < 0)
		{
			UnionUserErrLog("in UnionObjectRecPaimaryKeyIsRepeat:: [%s] does not exists!\n",caFileName);
			return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
		}
	}
	
	hFileHdl=UnionOpenRecFileHDL(caFileName);
	if (hFileHdl == NULL)
	{
		UnionUserErrLog("in UnionObjectRecPaimaryKeyIsRepeat:: UnionOpenRecFileHDL fail!\n");
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	while(1)
	{
		// 读取文件中的一行，已经去掉了注释行
		memset(caRecord,0,sizeof(caRecord));
		iRecLen=UnionReadNextRecFromFile(hFileHdl,caRecord,sizeof(caRecord));
		if (iRecLen < 0)
		{
			UnionUserErrLog("in UnionObjectRecPaimaryKeyIsRepeat:: UnionReadNextRecFromFile fail! return=[%d]\n",iRecLen);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(iRecLen);
		}
		if (iRecLen == 0)	// 读文件结束
			break;
		
		
		// Mary add begin, 20081027
		for (iLen=0;iLen<iRecLen;iLen++)
		{
			if (caRecord[iLen+7] == '|')
				break;
		}
		if (iLen != strlen(primaryKey))
			continue;			
		// Mary add end, 20081027
		
		if (strncmp(&caRecord[7],primaryKey,strlen(primaryKey)) == 0)
		{
			UnionProgramerLog("in UnionObjectRecPaimaryKeyIsRepeat:: primaryKey is repeat! primaryKey = [%s] record=[%s]\n",primaryKey,caRecord);
			UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
			return(1);
		}
	}
	UnionCloseRecFileHDLOfFileName(hFileHdl,caFileName);
	
	return(0);
}

/*
功能：获得对象的实例数目
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	>=0：实例数
	<0：失败，错误码
*/
int UnionGetObjectRecordNumber(TUnionIDOfObject idOfObject)
{
	char	caFileName[512];
	int	iRet;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionGetObjectRecordNumber:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(idOfObject,caFileName);
	
	// 检查对象的键值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionGetObjectRecordNumber:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// 获得对象的键值文件中的记录数
	iRet=UnionGetTotalRecNumInFile(caFileName);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionGetObjectRecordNumber:: UnionGetTotalRecNumInFile fail! return=[%d]\n",iRet);
		return(iRet);
	}

	return(iRet);
}

// Mary add begin, 20081112
/*
功能：读取指定表的所有记录
输入参数：
	tableName：	表名
	maxRecNum	缓冲可以缓冲的最大记录数量
输出参数：
	recKey		关键字，如一个关键字由多个域构成，使用.分开两个域
返回值：
	>=0：		记录数
	<0：		错误代码
*/
int UnionReadAllRecOfSpecTBL(char *tableName,char recKey[][128+1],int maxRecNum)
{
	char	caFileName[512];
	int	iRet;
        char 	tmpBuf[128+1];
        int	iTmpRet;
	int	i;
	
	if (tableName == NULL || maxRecNum < 0)
	{
		UnionUserErrLog("in UnionReadAllRecOfSpecTBL:: parameters error!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	if (maxRecNum == 0)
	{
		UnionAuditLog("in UnionReadAllRecOfSpecTBL:: maxRecNum is 0!\n");
		return(0);
	}
	
	// 获得对象的键值文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjecRecPrimaryKeyIndexFileName(tableName,caFileName);
	
	// 检查对象的键值文件是否存在
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionReadAllRecOfSpecTBL:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// 获得键值文件中所有记录的键值
	iRet=UnionReadAllRecFldInFile(caFileName,"fldGrp",recKey,maxRecNum);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionReadAllRecOfSpecTBL:: UnionReadAllRecFldInFile fail! return=[%d]\n",iRet);
		return(iRet);
	}

        // Begin Add By HuangBaoxin 2008/11/14
        for (i = 0; i<iRet; i++)
        {
                memset(tmpBuf, 0, sizeof(tmpBuf));
                iTmpRet = UnionTranslateRecFldStrIntoStr(recKey[i], strlen(recKey[i]), tmpBuf, sizeof(tmpBuf));
                if (iTmpRet < 0)
                {
                	UnionUserErrLog("in UnionReadAllRecOfSpecTBL:: UnionTranslateRecFldStrIntoStr fail! return=[%d]\n",iTmpRet);
                	return(iTmpRet);		
                }
                memset(recKey[i], 0, sizeof(recKey[i]));
                memcpy(recKey[i], tmpBuf, iTmpRet);
        }
        // End Add By HuangBaoxin 2008/11/14
        
	return(iRet);
}
// Mary add end, 20081112

/*
function:
        判断是否是对象中的关键字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是关键字段
        0               不是关键字段
*/
int UnionIsPrimaryKeyFld(TUnionIDOfObject idOfObject, char *fldName)
{
        int             ret;
        TUnionObject    obj;
        //int             i;

        if(NULL == fldName || strlen(fldName) == 0)
                return 0;

        memset(&obj,0,sizeof(obj));
        // 根据对象名，读取该对象的定义
        if ((ret = UnionReadObjectDef(idOfObject,&obj)) < 0)
        {
                UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadObjectDef from [%s] errCode = [%d]\n",idOfObject,ret);
                return(ret);
        }
        /*
        for(i = 0; i < obj.primaryKey.fldNum; i++)
                if(strcmp(obj.primaryKey.fldNameGrp[i], fldName) == 0)
                        return 1;

        return 0;
        */
        return(UnionIsPrimaryKeyFldOfObject(&obj,fldName));
}

/*
function:
        判断是否是对象中的关键字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是关键字段
        0               不是关键字段
*/
int UnionIsPrimaryKeyFldOfObject(PUnionObject pobject, char *fldName)
{
        int             ret;
        int             i;

        if (NULL == fldName || pobject == 0)
                return 0;

        for(i = 0; i < pobject->primaryKey.fldNum; i++)
                if(strcmp(pobject->primaryKey.fldNameGrp[i], fldName) == 0)
                        return 1;

        return 0;
}

/*
function:
        判断是否是对象中的唯一值字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是唯一值字段
        0               不是唯一值字段
*/
int UnionIsUniqueFld(TUnionIDOfObject idOfObject, char *fldName)
{
        int             ret;
        TUnionObject    obj;

        if(NULL == fldName || strlen(fldName) == 0)
                return 0;

        memset(&obj,0,sizeof(obj));
        // 根据对象名，读取该对象的定义
        if ((ret = UnionReadObjectDef(idOfObject,&obj)) < 0)
        {
                UnionUserErrLog("in UnionGetAllowModifyFldFromObjectRecord:: UnionReadObjectDef from [%s] errCode = [%d]\n",idOfObject,ret);
                return(ret);
        }

        return(UnionIsUniqueFldOfObject(&obj,fldName));
}

/*
function:
        判断是否是对象中的唯一字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是唯一值字段
        0               不是唯一值字段
*/
int UnionIsUniqueFldOfObject(PUnionObject pobject, char *fldName)
{
        int             ret;
        int             index1,index2;
        PUnionObjectFldGrp	pfldGrp;

        if (NULL == fldName || pobject == 0)
                return 0;

        for(index1 = 0; index1 < pobject->uniqueFldGrpNum; index1++)
        {
        	pfldGrp = &(pobject->uniqueFldGrp[index1]);
        	for (index2 = 0; index2 < pfldGrp->fldNum; index2++)
        	{
                	if(strcmp(pfldGrp->fldNameGrp[index2], fldName) == 0)
                        	return(1);
		}
	}
        return(0);
}

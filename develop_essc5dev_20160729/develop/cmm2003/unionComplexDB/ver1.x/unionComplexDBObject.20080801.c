//	Author: ChenJiaMei
//	Date: 2008-8-7

#include <stdio.h>
#include <string.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectSql.h"
#include "unionComplexDBObjectDef.h"
#include "unionComplexDBObjectChildDef.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBUniqueKey.h"
#include "unionComplexDBObject.h"

/*
功能：根据预先定义的SQL文件创建一个对象
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCreateObject(TUnionIDOfObject idOfObject)
{
	int	iRet,i;
	TUnionObject		sObject;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionCreateObject:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象定义文件是否存在
	iRet=UnionExistsObjectDef(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObject:: UnionExistsObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionCreateObject:: object [%s] defined file already exists!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionAlreadyExists));
	}
	
	// 从对象创建文件中读取对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionCreateObjectDefFromCreateSqlFile(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObject:: UnionCreateObjectDefFromCreateSqlFile fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	iRet=UnionCreateObjectAnyway(&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObject:: UnionCreateObjectAnyway fail! return=[%d]\n",iRet);
		return(iRet);
	}
	return(iRet);
}

/*
功能：删除一个对象
输入参数：
	idOfObject：对象的ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionDropObject(TUnionIDOfObject idOfObject)
{
	int	iRet,i;
	char	caBuf[512];
	TUnionObject		sObject;
	TUnionChildObjectGrp	sChildObjGrp;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionDropObject:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象定义文件是否存在
	iRet=UnionExistsObjectDef(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionExistsObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionDropObject:: object [%s] defined file does not exists!\n",idOfObject);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// 读对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionReadObjectDef for [%s] fail! return=[%d]\n",idOfObject,iRet);
		return(iRet);
	}
	
	// 检查是否有对象实例
	iRet=UnionGetObjectRecordNumber(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionGetObjectRecordNumber fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDropObject:: object [%s] has [%d] record already!\n",idOfObject,iRet);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_RecordExistAlready));
	}
	
	// 检查是否有其它对象域引用了本对象的域
	iRet=UnionExistForeignObjectInfo(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionExistForeignObjectInfo fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDropObject:: object [%s] has been referenced by [%d] foreign objects already!\n",idOfObject,iRet);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectIsReferenced));
	}
	
	// 删除对象记录的存储目录
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecMainDirName(idOfObject,caBuf);
	iRet=UnionRemoveDir(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionRemoveDir for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	// 删除本对象定义对其它对象域的引用关系
	memset(&sChildObjGrp,0,sizeof(TUnionChildObjectGrp));
	sChildObjGrp.childNum=sObject.foreignFldGrpNum;
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		strcpy(sChildObjGrp.childObjectGrp[i].objectName,idOfObject);
		memcpy(&(sChildObjGrp.childObjectGrp[i].localFldGrp),&(sObject.foreignFldGrp[i].foreignFldGrp),sizeof(TUnionObjectFldGrp));
		memcpy(&(sChildObjGrp.childObjectGrp[i].foreignFldGrp),&(sObject.foreignFldGrp[i].localFldGrp),sizeof(TUnionObjectFldGrp));
		iRet=UnionCancelRegisterOfForeignObjectInfo(sObject.foreignFldGrp[i].objectName,sChildObjGrp);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObject:: UnionCancelRegisterOfForeignObjectInfo fail! return=[%d]\n",iRet);
			return(iRet);
		}
	}
	
	// 删除本对象的定义文件
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	// 删除本对象的外部关键字定义清单文件
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectChildrenDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObject:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
功能：
	创建一个对象，如果已存在，先删除
输入参数：
	prec	对象
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCreateObjectAnyway(PUnionObject prec)
{
	int	iRet,i;
	char	caBuf[512];
	TUnionObject		sObject;
	TUnionChildObjectGrp	sChildObjGrp;
	
	if (prec == NULL)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	if ((iRet = UnionBatchDeleteObjectRecord(prec->name,"")) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionBatchDeleteObjectRecord! ret = %d!\n",iRet);
		//return(iRet);
	}
	if ((iRet = UnionDropObjectAnyway(prec->name)) < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionDropObjectAnyway! ret = %d!\n",iRet);
		//return(iRet);
	}
	
	// 检查对象定义是否合法
	iRet=UnionIsValidObjectDef(*prec);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionIsValidObjectDef fail! return=[%d]\n",iRet);
		return(iRet);
	}
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: object [%s] is invalid!\n",prec->name);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectIsInvalid));
	}
	
	// 在外部对象登记外键定义
	memset(&sChildObjGrp,0,sizeof(TUnionChildObjectGrp));
	// sChildObjGrp.childNum=prec->foreignFldGrpNum;
        sChildObjGrp.childNum=1;       // modify by chenliang, 2008-09-11

	for (i=0;i<prec->foreignFldGrpNum;i++)
	{
		strcpy(sChildObjGrp.childObjectGrp[i].objectName,prec->name);
		memcpy(&(sChildObjGrp.childObjectGrp[i].localFldGrp),&(prec->foreignFldGrp[i].foreignFldGrp),sizeof(TUnionObjectFldGrp));
		memcpy(&(sChildObjGrp.childObjectGrp[i].foreignFldGrp),&(prec->foreignFldGrp[i].localFldGrp),sizeof(TUnionObjectFldGrp));
		iRet=UnionRegisterForeignObjectInfo(prec->foreignFldGrp[i].objectName,sChildObjGrp);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionRegisterForeignObjectInfo fail! return=[%d]\n",iRet);
			return(iRet);
		}
	}
	
	// 创建对象定义的存储目录
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectDefDirName(prec->name,caBuf);
	if (!UnionExistsDir(caBuf))
	{
		iRet=UnionCreateDir(caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateDir for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
	}
	
	// 创建对象定义文件
	iRet=UnionStoreObjectDefAnyway(prec);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionStoreObjectDefAnyway fail! return=[%d]\n",iRet);
		return(iRet);
	}
	
	// 创建外部引用对象登记文件
	iRet=UnionCreateObjectForeignObjDefFile(prec->name);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateObjectForeignObjDefFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		return(iRet);
	}
	
	// 创建对象实例的存储目录
	memset(caBuf,0,sizeof(caBuf));
	UnionGetAllObjectRecDirName(caBuf);
	if (!UnionExistsDir(caBuf))
	{
		iRet=UnionCreateDir(caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateDir for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
	}
	
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecMainDirName(prec->name,caBuf);
	if (!UnionExistsDir(caBuf))
	{
		iRet=UnionCreateDir(caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateDir for [%s] fail! return=[%d]\n",caBuf,iRet);
			return(iRet);
		}
	}
	
	// 创建键值文件和唯一值文件
	iRet=UnionCreateObjectPrimaryKeyIndexFile(prec->name);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateObjectPrimaryKeyIndexFile for [%s] fail! return=[%d]\n",prec->name,iRet);
		return(iRet);
	}
	
	for (i=0;i<prec->uniqueFldGrpNum;i++)
	{
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionFormFieldNameStrFromFldGrp(&(prec->uniqueFldGrp[i]),caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionFormFieldNameStrFromFldGrp for [%d] fail! return=[%d]\n",i,iRet);
			return(iRet);
		}

		iRet=UnionCreateObjectUniqueKeyIndexFile(prec->name,caBuf);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObjectAnyway:: UnionCreateObjectUniqueKeyIndexFile for [%s] fail! return=[%d]\n",prec->name,iRet);
			return(iRet);
		}
	}
	
	return(0);
}

/*
功能：删除一个对象
输入参数：
	idOfObject：对象的ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionDropObjectAnyway(TUnionIDOfObject idOfObject)
{
	int	iRet,i;
	char	caBuf[512];
	TUnionObject		sObject;
	TUnionChildObjectGrp	sChildObjGrp;
	
	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象定义文件是否存在
	iRet=UnionExistsObjectDef(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionExistsObjectDef fail! return=[%d]\n",iRet);
		//return(iRet);
	}
	if (iRet == 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: object [%s] defined file does not exists!\n",idOfObject);
		//return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}
	
	// 读对象定义
	memset(&sObject,0,sizeof(TUnionObject));
	iRet=UnionReadObjectDef(idOfObject,&sObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionReadObjectDef for [%s] fail! return=[%d]\n",idOfObject,iRet);
		//return(iRet);
	}
	
	// 检查是否有对象实例
	iRet=UnionGetObjectRecordNumber(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionGetObjectRecordNumber fail! return=[%d]\n",iRet);
		//return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: object [%s] has [%d] record already!\n",idOfObject,iRet);
		//return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_RecordExistAlready));
	}
	
	// 检查是否有其它对象域引用了本对象的域
	iRet=UnionExistForeignObjectInfo(idOfObject);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionExistForeignObjectInfo fail! return=[%d]\n",iRet);
		//return(iRet);
	}
	if (iRet > 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: object [%s] has been referenced by [%d] foreign objects already!\n",idOfObject,iRet);
		//return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectIsReferenced));
	}
	
	// 删除对象记录的存储目录
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectRecMainDirName(idOfObject,caBuf);
	iRet=UnionRemoveDir(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionRemoveDir for [%s] fail! return=[%d]\n",caBuf,iRet);
		//return(iRet);
	}
	
	// 删除本对象定义对其它对象域的引用关系
	memset(&sChildObjGrp,0,sizeof(TUnionChildObjectGrp));
	sChildObjGrp.childNum=sObject.foreignFldGrpNum;
	for (i=0;i<sObject.foreignFldGrpNum;i++)
	{
		strcpy(sChildObjGrp.childObjectGrp[i].objectName,idOfObject);
		memcpy(&(sChildObjGrp.childObjectGrp[i].localFldGrp),&(sObject.foreignFldGrp[i].foreignFldGrp),sizeof(TUnionObjectFldGrp));
		memcpy(&(sChildObjGrp.childObjectGrp[i].foreignFldGrp),&(sObject.foreignFldGrp[i].localFldGrp),sizeof(TUnionObjectFldGrp));
		iRet=UnionCancelRegisterOfForeignObjectInfo(sObject.foreignFldGrp[i].objectName,sChildObjGrp);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionCreateObject:: UnionCancelRegisterOfForeignObjectInfo fail! return=[%d]\n",iRet);
			//return(iRet);
		}
	}
	
	// 删除本对象的定义文件
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		//return(iRet);
	}
	
	// 删除本对象的外部关键字定义清单文件
	memset(caBuf,0,sizeof(caBuf));
	UnionGetObjectChildrenDefFileName(idOfObject,caBuf);
	iRet=UnionDeleteRecFile(caBuf);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDropObjectAnyway:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caBuf,iRet);
		//return(iRet);
	}
	
	return(0);
}

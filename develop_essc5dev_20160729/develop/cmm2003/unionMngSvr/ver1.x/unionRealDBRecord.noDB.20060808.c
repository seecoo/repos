#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"unionComplexDBObjectDef.h"
//#include"unionFileDoc.h"
#include"unionComplexDBObjectDef.h"
#include "unionFileManager.h"

int UnionConnectDatabase()
{
	return 0;
}

int UnionCloseDatabase()
{
	return 0;
}

int UnionSelectObjectRecordByRealSQL(TUnionIDOfObject idOfObject,char *condition,char *fileName)
{
	return 0;
}

int UnionSelectObjectRecordBySpecFieldListOnObject(TUnionIDOfObject idOfObject,char *reqStr,int lenOfReqStr, char *fileName)
{
	return 0;
}

/*
int UnionInsertFileDocRec(PUnionFileDoc prec)
{
	return 0;
}
*/

int UnionSelectSpecFldOfObjectByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *fldName,char *fldValue,int sizeOfBuf)
{
	return 0;
}

int UnionFormPrimaryKeyOfFileManagerRecFromRecStr(char *recStr,int lenOfRecStr,PUnionFileManager prec)
{
	return 0;
}

int UnionReadFileManagerRec(char *tableName,int fileType,PUnionFileManager prec)
{
	return 0;
}

int UnionGetAllFldNameFromFldIDList(char *fldList,int lenOfFldList,char fldNameGrp[][128+1],int maxNum)
{
	return 0;
}

int UnionReadTableFldNameByFldID(char *fldID,char *fldName)
{
	return 0;
}

int UnionGetMngSvrOperationType(int resID,int resCmd)
{
	return 0;
}

int UnionExcuteOtherSvrService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	return 0;
}


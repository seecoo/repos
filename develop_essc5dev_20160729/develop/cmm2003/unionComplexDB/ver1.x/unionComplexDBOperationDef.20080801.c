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

#include "unionComplexDBOperationDef.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionLog.h"			// ʹ��3.x�汾

/*
����	
	��һ������ڴ��ж�ȡһ�����ݿ��������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	sizeOfTblName	��������Ĵ�С
	sizeOfCondition	������������Ĵ�С
�������
	tblName		����
	operationID	������ʶ
	condition	��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadComplexDBOperationDefFromStr(char *str,int lenOfStr,char *tblName,int sizeOfTblName,int *operationID,char *condition,int sizeOfCondition)
{
	return(UnionReadComplexDBSelectOperationDefFromStr(str,lenOfStr,tblName,sizeOfTblName,operationID,condition,sizeOfCondition,NULL,0));
}

/*
����	
	��һ������ڴ��ж�ȡһ�����ݿ�ѡ���������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	sizeOfTblName	��������Ĵ�С
	sizeOfOperationDef	������������Ĵ�С
	sizeOfFldList	���嵥����Ĵ�С
�������
	tblName		����
	operationID	������ʶ
	condition	��������
	fldList		Ҫѡ������嵥
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadComplexDBSelectOperationDefFromStr(char *str,int lenOfStr,char *tblName,int sizeOfTblName,int *operationID,char *condition,int sizeOfOperationDef,char *fldList,int sizeOfFldList)
{
	int			ret;
	int			index;
	char			var[128];
	
	if (str == NULL)
	{
		UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// ����
	if (tblName != NULL)
	{
		if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,"tableName",tblName,sizeOfTblName)) < 0)
		{
			UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n","tableName");
			return(ret);
		}
	}
	// ������ʶ
	if (operationID != NULL)
	{
		if ((ret = UnionReadIntTypeRecFldFromRecStr(str,lenOfStr,"operationID",operationID)) < 0)
		{
			UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: UnionReadIntTypeRecFldFromRecStr [%s]!\n","operationID");
			return(ret);
		}
	}
	// ��������
	if (condition != NULL)
	{
		if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,"condition",condition,sizeOfOperationDef)) < 0)
		{
			UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n","condition");
			return(ret);
		}
	}
	// ���嵥
	if (fldList != NULL)
	{
		if ((ret = UnionReadRecFldFromRecStr(str,lenOfStr,"fldList",fldList,sizeOfFldList)) < 0)
		{
			UnionUserErrLog("in UnionReadComplexDBSelectOperationDefFromStr:: UnionReadRecFldFromRecStr [%s]!\n","fldList");
			return(ret);
		}
	}
	return(0);
}


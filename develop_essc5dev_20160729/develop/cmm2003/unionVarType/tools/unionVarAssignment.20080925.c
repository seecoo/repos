//	Wolfgang Wang
//	2008/11/15

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionCommand.h"
#include "unionVarAssignment.h"
#include "UnionLog.h"
#include "unionGenFunListTestProgram.h"

/*
����	�ж��Ƿ��ǺϷ��ı�����ֵ����
�������
	method		����
�������
        ��
����ֵ
	1		�Ϸ�
	0		���Ϸ�
*/
int UnionIsValidVarAssignmentMethod(char *method)
{
	if (strcmp(method,conVarAssignmentMethodValue) == 0)
		return(1);
	if (strcmp(method,conVarAssignmentMethodFunVar) == 0)
		return(1);
	if (strcmp(method,conVarAssignmentMethodGolobalVar) == 0)
		return(1);
	if (strcmp(method,conVarAssignmentMethodInput) == 0)
		return(1);
	else
		return(0);
}

/*
����	��ָ�������ĸ�ֵ����
�������
	str         	��
        lenOfStr  	������
        sizeOfBuf	���ո�ֵ������ֵ�Ļ���Ĵ�С
�������
        varName		��������
	method		�����ĸ�ֵ����
	value		��ֵ����ʹ�õ�ֵ
����ֵ
	>=0		��ֵ�����ĳ���
	<0		�������
*/
int UnionReadVarValueAssignMethod(char *str,int lenOfStr,char *varName,char *method,char *value,int sizeOfBuf)
{
	char	*ptr;
	int	offset;
	int	len;
	
        // ����������
        if ((ptr = strstr(str,"=")) == NULL)
        {
        	UnionUserErrLog("in UnionReadVarValueAssignMethod:: no varName set in [%s]\n",str);
        	return(errCodeNoneVarDefined);
        }
        *ptr = 0;
        strcpy(varName,str);
        offset = strlen(varName) + 1;
        *ptr = '=';
        // ����ֵ����
        if (offset >= lenOfStr)
        	return(0);
        if ((ptr = strstr(str+offset,"::")) == NULL)
        {
        	strcpy(method,conVarAssignmentMethodValue);
        	goto readValue;
        }
        *ptr = 0;
        if (!UnionIsValidVarAssignmentMethod(str+offset))	// �ж��Ƿ��ǺϷ��ķ���
        {
        	*ptr = ':';
        	goto readValue;
        }
        strcpy(method,str+offset);
        offset += strlen(method);
        offset += 2;
        *ptr = ':';
        // ��ֵ
readValue:
        if ((len = lenOfStr - offset) < 0)
        	return(0);
        if (len >= sizeOfBuf)
        {
        	UnionAuditLog("in UnionReadVarValueAssignMethod:: sizeOfBuf [%d] is too small for buffer value [%s], this value is shortened!\n",sizeOfBuf,str+offset);
        	len = sizeOfBuf - 1;
        }
        memcpy(value,str+offset,len);
        return(len);
}


/*
����	
	��һ�����ж�ȡ�����ĸ�ֵ����
�������
	str		��
	lenOfStr	������
�������
	pdef		��ֵ����
����ֵ
	>=0		�ɹ�,ֵ�ĳ���
	<0		�������
*/
int UnionReadVarAssignmentDefFromStr(char *str,int lenOfStr,PUnionVarAssignmentDef pdef)
{
	int		ret;
	char		tmpBuf[4096+1];
	char		*ptr;
	int		funIndex;
	char		funVar[128+1];
	char		varPrefix[10];
	int		lenOfPar;
	
	if ((str == NULL) || (pdef == NULL))
		return(errCodeParameter);
		
	memset(pdef,0,sizeof(*pdef));
	memset(tmpBuf,0,sizeof(tmpBuf));
	// ��ȡ��ֵ�����͸�ֵ�������õĺ���
	if ((lenOfPar = UnionReadVarValueAssignMethod(str,lenOfStr,pdef->varName,pdef->method,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadVarAssignmentDefFromStr:: UnionReadVarValueAssignMethod! ret = [%d]\n",lenOfPar);
		return(lenOfPar);
	}
	//UnionLog("in UnionReadVarAssignmentDefFromStr:: method = [%s] varName = [%s] value = [%s]\n",pdef->method,pdef->varName,tmpBuf);
	pdef->isVarName = 0;
	// ûָ����ֵ������ֵ������ֱ��ȡֵ
	if ((strlen(pdef->method) == 0) || (strcmp(pdef->method,conVarAssignmentMethodValue) == 0))	
	{
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,lenOfPar,conVarAssignmentMethodFldVarValue,pdef->value,sizeof(pdef->value))) <= 0)
		{	
			if (lenOfPar >= (int)sizeof(pdef->value))
				lenOfPar = sizeof(pdef->value) - 1;
			if (lenOfPar != 0)
				memcpy(pdef->value,tmpBuf,lenOfPar);
			return(lenOfPar);
		}
	}
	else if (strcmp(pdef->method,conVarAssignmentMethodInput) == 0)	// ��������
	{
		if (lenOfPar == 0)
			ptr = UnionInput("���������ֵ::");
		else
			ptr = UnionInput("������%s::",tmpBuf);
		strcpy(pdef->value,ptr);
		if (UnionIsQuit(ptr))
			return(errCodeUserSelectExit);
		return(strlen(pdef->value));
	}
	else if (strcmp(pdef->method,conVarAssignmentMethodFunVar) == 0)	// ����ĳһ�������Ĳ���
	{
		pdef->isVarName = 1;
		funIndex = 0;
		if ((ret = UnionReadIntTypeRecFldFromRecStr(tmpBuf,lenOfPar,conVarAssignmentMethodFldVarFunIndex,&funIndex)) < 0)
		{
			UnionUserErrLog("in UnionReadVarAssignmentDefFromStr:: UnionReadIntTypeRecFldFromRecStr [%s] form [%s]\n",conVarAssignmentMethodFldVarFunIndex,tmpBuf);
			return(ret);
		}
		memset(funVar,0,sizeof(funVar));
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,lenOfPar,conVarAssignmentMethodFldVarName,funVar,sizeof(funVar))) < 0)
		{
			UnionUserErrLog("in UnionReadVarAssignmentDefFromStr:: UnionReadRecFldFromRecStr [%s] form [%s]\n",conVarAssignmentMethodFldVarName,tmpBuf);
			return(ret);
		}
		UnionGenerateFunVarNamePrefixOfFunGrp(funIndex,varPrefix);
		UnionFormCSentenceVarName(varPrefix,funVar,pdef->value);
	}
	else if (strcmp(pdef->method,conVarAssignmentMethodGolobalVar) == 0)	// ����ȫ�ֱ���
	{
		pdef->isVarName = 1;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,lenOfPar,conVarAssignmentMethodFldVarName,pdef->value,sizeof(pdef->value))) <= 0)
		{	
			if (lenOfPar != 0)
				memcpy(pdef->value,tmpBuf,lenOfPar);
			return(lenOfPar);
		}
	}
	else // ������ֵ����
	{
		if (lenOfPar != 0)
			memcpy(pdef->value,tmpBuf,lenOfPar);
		return(lenOfPar);
	}
	pdef->isBinary = 0;
	UnionReadRecFldFromRecStr(tmpBuf,lenOfPar,conVarAssignmentMethodFldVarLen,pdef->lenTag,sizeof(pdef->lenTag));
	UnionReadIntTypeRecFldFromRecStr(tmpBuf,lenOfPar,conVarAssignmentMethodFldVarIsBinary,&(pdef->isBinary));
	return(strlen(pdef->value));
}

/*
����	
	��һ����������д�뵽ָ���ļ���
�������
	pdef		��ֵ����
	fp		ָ���ļ����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarAssignmentDefToFile(PUnionVarAssignmentDef pdef,FILE *fp)
{
	FILE	*outFp = stdout;
	
	if (fp != NULL)
		outFp = fp;
	if (pdef == NULL)
		return(errCodeParameter);
	fprintf(outFp,"method=%s|varName=%s|value=%s|len=%s|isVarName=%d|isBinary=%d|\n",
		pdef->method,pdef->varName,pdef->value,pdef->lenTag,pdef->isVarName,pdef->isBinary);
	return(0);
}

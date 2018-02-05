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
功能	判断是否是合法的变量赋值方法
输入参数
	method		方法
输出参数
        无
返回值
	1		合法
	0		不合法
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
功能	读指定变量的赋值方法
输入参数
	str         	串
        lenOfStr  	串长度
        sizeOfBuf	接收赋值方法的值的缓冲的大小
输出参数
        varName		变量名称
	method		读出的赋值方法
	value		赋值方法使用的值
返回值
	>=0		域赋值方法的长度
	<0		出错代码
*/
int UnionReadVarValueAssignMethod(char *str,int lenOfStr,char *varName,char *method,char *value,int sizeOfBuf)
{
	char	*ptr;
	int	offset;
	int	len;
	
        // 读变量名称
        if ((ptr = strstr(str,"=")) == NULL)
        {
        	UnionUserErrLog("in UnionReadVarValueAssignMethod:: no varName set in [%s]\n",str);
        	return(errCodeNoneVarDefined);
        }
        *ptr = 0;
        strcpy(varName,str);
        offset = strlen(varName) + 1;
        *ptr = '=';
        // 读赋值方法
        if (offset >= lenOfStr)
        	return(0);
        if ((ptr = strstr(str+offset,"::")) == NULL)
        {
        	strcpy(method,conVarAssignmentMethodValue);
        	goto readValue;
        }
        *ptr = 0;
        if (!UnionIsValidVarAssignmentMethod(str+offset))	// 判断是否是合法的方法
        {
        	*ptr = ':';
        	goto readValue;
        }
        strcpy(method,str+offset);
        offset += strlen(method);
        offset += 2;
        *ptr = ':';
        // 读值
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
功能	
	从一个串中读取变量的赋值方法
输入参数
	str		串
	lenOfStr	串长度
输出参数
	pdef		赋值方法
返回值
	>=0		成功,值的长度
	<0		出错代码
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
	// 读取赋值方法和赋值方法采用的函数
	if ((lenOfPar = UnionReadVarValueAssignMethod(str,lenOfStr,pdef->varName,pdef->method,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionReadVarAssignmentDefFromStr:: UnionReadVarValueAssignMethod! ret = [%d]\n",lenOfPar);
		return(lenOfPar);
	}
	//UnionLog("in UnionReadVarAssignmentDefFromStr:: method = [%s] varName = [%s] value = [%s]\n",pdef->method,pdef->varName,tmpBuf);
	pdef->isVarName = 0;
	// 没指定赋值方法或赋值方法是直接取值
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
	else if (strcmp(pdef->method,conVarAssignmentMethodInput) == 0)	// 输入数据
	{
		if (lenOfPar == 0)
			ptr = UnionInput("请输入变量值::");
		else
			ptr = UnionInput("请输入%s::",tmpBuf);
		strcpy(pdef->value,ptr);
		if (UnionIsQuit(ptr))
			return(errCodeUserSelectExit);
		return(strlen(pdef->value));
	}
	else if (strcmp(pdef->method,conVarAssignmentMethodFunVar) == 0)	// 采用某一个函数的参数
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
	else if (strcmp(pdef->method,conVarAssignmentMethodGolobalVar) == 0)	// 采用全局变量
	{
		pdef->isVarName = 1;
		if ((ret = UnionReadRecFldFromRecStr(tmpBuf,lenOfPar,conVarAssignmentMethodFldVarName,pdef->value,sizeof(pdef->value))) <= 0)
		{	
			if (lenOfPar != 0)
				memcpy(pdef->value,tmpBuf,lenOfPar);
			return(lenOfPar);
		}
	}
	else // 不明赋值方法
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
功能	
	将一个变量定义写入到指定文件中
输入参数
	pdef		赋值方法
	fp		指定文件句柄
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
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

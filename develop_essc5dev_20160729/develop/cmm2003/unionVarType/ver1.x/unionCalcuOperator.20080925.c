//	Author: 王纯军
//	Date: 2008-11-29

// 定义数学运算符

#include <stdio.h>
#include <string.h>

#include "unionCalcuOperator.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

/*　
功能
	将一个运算符串转换为内部标识
输入参数
	specOperatorTag	运算符
输出参数
	无
返回值
	>=0		内部表示的运算符
	<0		出错代码
*/
int UnionConvertCalcuOperatorStrTagIntoIntTag(char *specOperatorTag)
{
	char	operatorTag[128+1];
	
	strcpy(operatorTag,specOperatorTag);
	UnionToUpperCase(operatorTag);
	if (strcmp(conCalcuOperatorTagNot,operatorTag) == 0)
		return(conCalcuOperatorNot);
	else if (strcmp(conCalcuOperatorTagNotOfCProgram,operatorTag) == 0)
		return(conCalcuOperatorNot);
	if (strcmp(conCalcuOperatorTagAnd,operatorTag) == 0)
		return(conCalcuOperatorAnd);
	else if (strcmp(conCalcuOperatorTagAndOfCProgram,operatorTag) == 0)
		return(conCalcuOperatorAnd);
	if (strcmp(conCalcuOperatorTagOr,operatorTag) == 0)
		return(conCalcuOperatorOr);
	else if (strcmp(conCalcuOperatorTagOrOfCProgram,operatorTag) == 0)
		return(conCalcuOperatorOr);
	else
	{
		UnionUserErrLog("in UnionConvertCalcuOperatorStrTagIntoIntTag:: invalid operator [%s]\n",specOperatorTag);
		return(errCodeCDPMDL_InvalidCalcuOperator);
	}
}

/*
功能	将一个关系类型转换为关系符号
输入参数
	compType	关系类型
输出参数
	compTag		关系符号
返回值
	>=0		成功
	<0		出错代码
*/
int UnionConvertSpecRelationCompTypeIntoDBRelationCompTag(int compType,char *compTag)
{
	switch (compType)
	{
		case	conCalcuOperatorAnd:
			strcpy(compTag," and ");
			break;
		case	conCalcuOperatorOr:
			strcpy(compTag," or ");
			break;
		case	conCalcuOperatorNot:
			strcpy(compTag," and ");
			break;
		default:
			strcpy(compTag," and ");
			break;
	}		
	return(strlen(compTag));	
}

/*
功能	将一个关系类型转换为赋值符号
输入参数
	compType	关系类型
输出参数
	compTag		关系符号
返回值
	>=0		成功
	<0		出错代码
*/
int UnionConvertSpecRelationCompTypeIntoDBValueAssignTag(int compType,char *compTag)
{
	switch (compType)
	{
		case	conCalcuOperatorAnd:
			strcpy(compTag,"=");
			break;
		case	conCalcuOperatorOr:
			strcpy(compTag,"=");
			break;
		case	conCalcuOperatorNot:
			strcpy(compTag,"<>");
			break;
		default:
			strcpy(compTag,"=");
			break;
	}
	return(strlen(compTag));	
}


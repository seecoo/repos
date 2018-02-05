//	Author: 王纯军
//	Date: 2008-11-29

// 定义数学运算符
#ifndef _unionCalcuOperator_
#define _unionCalcuOperator_

#define conCalcuOperatorTagNot			"NOT"
#define conCalcuOperatorTagNotOfCProgram	"!"
#define conCalcuOperatorTagAnd			"AND"
#define conCalcuOperatorTagAndOfCProgram	"&&"
#define conCalcuOperatorTagOr			"OR"
#define conCalcuOperatorTagOrOfCProgram		"||"

typedef enum
{
	conCalcuOperatorNot = 1,
	conCalcuOperatorAnd = 2,
	conCalcuOperatorOr = 3,
} TUnionCalcuOperator;

/*　
功能
	将一个运算符串转换为内部标识
输入参数
	operatorTag	运算符
输出参数
	无
返回值
	>=0		内部表示的运算符
	<0		出错代码
*/
int UnionConvertCalcuOperatorStrTagIntoIntTag(char *operatorTag);

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
int UnionConvertSpecRelationCompTypeIntoDBRelationCompTag(int compType,char *compTag);

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
int UnionConvertSpecRelationCompTypeIntoDBValueAssignTag(int compType,char *compTag);

#endif



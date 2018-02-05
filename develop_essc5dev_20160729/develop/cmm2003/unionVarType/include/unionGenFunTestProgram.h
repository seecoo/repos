//	Wolfgang Wang
//	2008/2/25

#ifndef _unionGenFunTestProgram_
#define _unionGenFunTestProgram_

#include "unionFunDef.h"

/*
功能	
	设置当前设置的变量是否是二进制
输入参数
	isBinary	变量是否是二进制的标志
输入出数
	无
返回值
	无
*/
void UnionSetCurrentVarValueAsBinary(int isBinary);

/*
功能	
	设置当前设置的变量的长度
输入参数
	varValueLen	变量的长度
输入出数
	无
返回值
	无
*/
void UnionSetCurrentVarValueLenName(char *varValueLen);

/*
功能	
	将固定的测试代码写入到测试程序中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	代码的数量
*/
int UnionAddFixedCodesToFile(FILE *fp);


/*
功能	
	写程序的生成信息
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	无
*/
void UnionAddAutoGenerateInfoToTestProgram(FILE *fp);

/*
功能	
	产生一个对字符串类型进行赋值的语句
输入参数
	localVarPrefix	变量前缀
	varValueTag	变量名称
	valueIsVarName	值是不是变量名称
	value		数据值
	varValueLenTag	变量值的长度
	isBinary	变量值是否是二进制,1,是,0,否
	fp		文件句柄
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateStrAssignmentCSentenceToFile(char *localVarPrefix,char *varValueTag,int valueIsVarName,char *value,char *varValueLenTag,int isBinary,FILE *fp);

/*
功能	
	产生一个对指针类型进行赋值的语句
输入参数
	localVarPrefix	变量前缀
	varValueTag	变量名称
	valueIsVarName	值是不是变量名称
	value		数据值
	varValueLenTag	变量值的长度
	isBinary	变量值是否是二进制,1,是,0,否
	fp		文件句柄
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGeneratePointerAssignmentCSentenceToFile(char *localVarPrefix,char *varValueTag,int valueIsVarName,char *value,char *varValueLenTag,int isBinary,FILE *fp);

/*
功能	
	产生一个对基本数据类型进行赋值的语句
输入参数
	varPrefix	变量前缀
	baseType	基本类型
	varValueTag	变量名称
	valueIsVarName	值是不是变量名称
	value		数据值
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateAssignmentCSentenceToFile(char *varPrefix,int baseType,char *varValueTag,int valueIsVarName,char *value,FILE *fp);

/*
功能	
	将头文件写入到测试程序中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	头文件的数量
*/
int UnionAddIncludeFileToTestProgram(FILE *fp);

/*
功能	
	产生一个为指针型函数变量分配空间的代码
输入参数
	pdef		函数定义指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMallocCodesForPointerFunVarToFp(PUnionFunDef pdef,FILE *fp);

/*
功能	
	产生一个为指针型函数变量分配空间的函数
输入参数
	pdef		函数定义指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMallocFunForPointerFunVarToTestProgram(PUnionFunDef pdef,FILE *fp);

/*
功能	
	产生一个为指针型函数变量释放空间的代码
输入参数
	pdef		函数定义指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFreeCodesForPointerFunVarToFp(PUnionFunDef pdef,FILE *fp);

/*
功能	
	产生一个为指针型函数变量释放空间的函数
输入参数
	pdef		函数定义指针
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFreeFunForPointerFunVarToTestProgram(PUnionFunDef pdef,FILE *fp);

/*
功能	
	产生一个为函数生成调用函数
输入参数
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCallFunOfFunToTestProgram(char *funName,FILE *fp);

/*
功能	
	产生一个读取函数值定义的索引文件
输入参数
	pdef		函数定义指针
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunVarValueTagListIndexFile(PUnionFunDef pdef);

/*
功能	
	将函数定义及为函数参数分配的变量定义写入到测试程序中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAddFunDefAndFunVarListToTestProgram(char *funName,FILE *fp);

/*
功能	
	产生一个对函数的输入参数进行赋值的函数
输入参数
	funName		函数名称
	testDataFileName	测试数据文件名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunInputVarAssignmentFun(char *funName,char *testDataFileName,FILE *fp);

/*
功能	
	产生一个对基本数据类型进行打印的语句
输入参数
	varPrefix	变量前缀
	baseType	基本类型
	varValueTag	变量名称
	value		数据值
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintOutputVarCSentenceToFp(char *varPrefix,int baseTypeTag,char *varValueTag,FILE *fp);

/*
功能	
	产生一个对函数的输出参数进行展示的函数
输入参数
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunOutputVarDisplayFun(char *funName,FILE *fp);

/*
功能	
	产生一个对函数的返回参数进行展示的函数
输入参数
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunReturnVarDisplayFun(char *funName,FILE *fp);

/*
功能	
	为一个主函数
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMainFunToTestProgram(FILE *fp);

/*
功能	
	为一个函数产生一个测试程序
输入参数
	funName		函数名称
	fileName	文件名称
	testDataFileName	测试数据文件名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateTestProgramForFun(char *funName,char *testDataFileName,char *fileName);

/*
功能	
	获取固定代码的定义文件
输入参数
	无
输入出数
	fileName	存入固定代码的定义文件
返回值
	>=0		正确
	<0		错误码
*/
int UnionFileNameOfFixedCodesConfFile(char *filename);

/*
功能	
	设置固定代码的定义文件
输入参数
	fileName	存入固定代码的定义文件
输入出数
	无
返回值
	>=0		正确
	<0		错误码
*/
int UnionSetFileNameOfFixedCodesConfFile(char *filename);

#endif

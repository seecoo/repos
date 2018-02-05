//	Wolfgang Wang
//	2008/10/6

#ifndef _unionTestConf_
#define _unionTestConf_

/*
功能	
	设置测试使用的库定义文件
输入参数
	fileName	文件名称
输入出数
	无
返回值
	无
*/
void UnionSetTestLibDefFileName(char *fileName);

/*
功能	
	获得生成的函数测试编译程序的名称
	这个文件名称为：$DIROFTEST/demo/mkfunName
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetTestFunMakeFileName(char *funName,char *fileName);

/*
功能	
	获得生成的函数测试程序的名称
	这个文件名称为：$DIROFTEST/demo/test-funName.c
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetTestFunCProgramFileName(char *funName,char *fileName);

/*
功能	
	获得生成的函数测试可执行程序的名称
	这个文件名称为：$DIROFTEST/demo/test-funName
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetTestFunExeProgramFileName(char *funName,char *fileName);

/*
功能	
	获得定义了库文件的文件的名称
	这个文件名称为：$DIROFTEST/conf/libFile.def
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetLibFileDefFileName(char *fileName);

/*
功能	
	获得定义了头文件目录定义文件的名称
	这个文件名称为：$DIROFTEST/conf/includeFileDir.def
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetIncludeFileDirDefFileName(char *fileName);

/*
功能	
	获得定义了头文件的文件的名称
	这个文件名称为：$DIROFAPITEST/conf/includeFile.def
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetIncludeFileDefFileName(char *fileName);

/*
功能	
	获得读函数输入参数值定义的索引文件
	这个文件名称为：$DIROFTEST/funVarValueListIndex/funName.input
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetFunVarInputValueListIndexFileName(char *funName,char *fileName);

/*
功能	
	获得读函数输出参数值定义的索引文件
	这个文件名称为：$DIROFTEST/funVarValueListIndex/funName.output
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetFunVarOutputValueListIndexFileName(char *funName,char *fileName);

/*
功能	
	获得读函数返回值参数值定义的索引文件
	这个文件名称为：$DIROFTEST/funVarValueListIndex/funName.return
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetFunVarReturnValueListIndexFileName(char *funName,char *fileName);

/*
功能	
	获得测试数据文件全名
	这个文件名称为：$DIROFTEST/testdata/fileName
输入参数
	无
输入出数
	文件名称
返回值
	无
*/
void UnionGetFullFileNameOfTestData(char *fileName,char *fullFileName);

#endif

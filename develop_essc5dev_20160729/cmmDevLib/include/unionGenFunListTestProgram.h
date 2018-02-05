//	Wolfgang Wang
//	2008/2/25

#ifndef _unionGenFunListTestProgram_
#define _unionGenFunListTestProgram_

/*
功能
	产生一个调用函数的函数名
输入参数
	funIndex	函数序号
	funName		函数名称
	outFp		文件名柄
输入出数
	无
返回值
	无
*/
void UnionGenerateCallFunName(int funIndex,char *funName,FILE *outFp);

/*
功能
	产生一个对输入参数赋值的函数名
输入参数
	funIndex	函数序号
	funName		函数名称
	outFp		文件名柄
输入出数
	无
返回值
	无
*/
void UnionGenerateInputVarSetFunName(int funIndex,char *funName,FILE *outFp);

/*
功能
	产生一个显示输出参数的函数名
输入参数
	funIndex	函数序号
	funName		函数名称
	outFp		文件名柄
输入出数
	无
返回值
	无
*/
void UnionGenerateOutputVarDisplayFunName(int funIndex,char *funName,FILE *outFp);

/*
功能
	产生一个显示返回值的函数名
输入参数
	funIndex	函数序号
	funName		函数名称
	outFp		文件名柄
输入出数
	无
返回值
	无
*/
void UnionGenerateReturnVarDisplayFunName(int funIndex,char *funName,FILE *outFp);

/*
功能	
	为一个函数变量生成前缀
输入参数
	funIndex	函数序号
输入出数
	funVarPrefix	变量前缀
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunVarNamePrefixOfFunGrp(int funIndex,char *funVarPrefix);

/*
功能	
	产生一个为一组函数的指针型函数变量释放空间的函数
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFreeFunForPointerFunVarOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp);

/*
功能	
	产生一个为指针型函数变量分配空间的函数
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMallocFunForPointerFunVarOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp);

/*
功能	
	将函数定义及为函数参数分配的变量定义写入到测试程序中
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAddFunDefAndFunVarListOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp);

/*
功能	
	产生一个读取函数值定义的索引文件
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunVarValueTagListIndexFileOfFunGrp(char funGrp[][128+1],int numOfFun);

/*
功能	
	产生一个对函数的输入参数进行赋值的函数
输入参数
	funIndex	函数在函数系列中的索引号
	funName		函数名称
	testDataFileName	测试数据文件名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunInputVarOfFunGrpAssignmentFun(int funIndex,char *funName,char *testDataFileName,FILE *fp);

/*
功能	
	产生一个对函数的输出参数进行展示的函数
输入参数
	funIndex	函数的序号
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunOutputVarOfFunGrpDisplayFun(int funIndex,char *funName,FILE *fp);

/*
功能	
	产生一个对函数的返回参数进行展示的函数
输入参数
	funIndex	函数的序号
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateFunReturnVarOfFunGrpDisplayFun(int funIndex,char *funName,FILE *fp);

/*
功能	
	产生一个为函数生成调用函数的函数
输入参数
	funIndex	函数序号
	funName		函数名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateCallFunOfFunGrpToTestProgram(int funIndex,char *funName,FILE *fp);

/*
功能	
	从文件中读取一个要测试的函数序列
输入参数
	testFileName	测试数据文件名称
	numOfFun	函数清单中可以放的函数的最大数目
输出参数
	funGrp		函数名称清单
返回值
	>=0		读出的函数的数目
	<0		出错代码
*/
int UnionGetFunGrpListFromFile(char *testDataFileName,char funGrp[][128+1],int numOfFun);

/*
功能	
	生成一个主函数
输入参数
	funGrp		函数名称清单
	numOfFun	函数清单中的函数的个数
	fp		文件句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMainFunOfFunGrpToTestProgram(char funGrp[][128+1],int numOfFun,FILE *fp);

/*
功能	
	为一组函数自动生成一组处理函数
输入参数
	testFileName	测试数据文件名称
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAutoGenerateFunForFunGrp(char *testFileName,FILE *fp);

/*
功能	
	为一个函数序列产生一个测试程序
输入参数
	cFileName	文件名称
	testDataFileName	测试数据文件名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAutoGenerateFunForFunGrpToSpecCFile(char *testDataFileName,char *cFileName);

#endif

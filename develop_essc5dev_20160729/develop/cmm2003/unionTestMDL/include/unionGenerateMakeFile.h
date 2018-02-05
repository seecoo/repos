//	Wolfgang Wang
//	2008/10/6

#ifndef _unionGenerateMakeFile_
#define _unionGenerateMakeFile_


/*
功能	
	将对头文件目录的定义增加到makefile中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAddIncludeDirDefToMakeFile(FILE *fp);

/*
功能	
	将库文件引用增加到makefile中
输入参数
	fp		测试程序句柄
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionAddLibDefToMakeFile(FILE *fp);


/*
功能	
	为一个测试程序产生一个编译文件
输入参数
	cFileName		测试程序文件名称
	exeFileName		生成的可执行程序名称
	makeFileName		编译文件名称
输入出数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateMakeFileForTestProgramForFun(char *cFileName,char *exeFileName,char *makeFileName);

#endif


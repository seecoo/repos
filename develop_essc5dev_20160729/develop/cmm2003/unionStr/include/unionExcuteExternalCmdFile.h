// Wolfgang Wang
// 2010-8-19

#ifndef _unionExcuteExternalCmdFile_
#define _unionExcuteExternalCmdFile_

// 定义外部命令函数名称
#define conMngSvrClientExternalCmdFunName		"UnionExcuteExternalCmd"

// 定义外部命令函数指针
#ifdef _WIN32_
typedef int (__stdcall *PUnionExternalCmdFun)(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
#else
typedef int (*PUnionExternalCmdFun)(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
#endif

/*
功能
	执行一个外部命令
输入参数
	reqStr		外部命令的请求串
	lenOfReqStr	外部命令的请求串长度
	sizeOfResStr	响应串的大小
输出参数
	resStr		命令的响应串
返回值
	>=0		响应串的长度
	<0		出错代码
*/
int UnionExcuteExternalCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

/*
功能
	执行一个命令文件
输入参数
	fileName	命令文件名称
	sizeOfResStr	响应串的大小
输出参数
	resStr		执行命令文件之后拼成的响应串
返回值
	>=0		响应串的长度
	<0		出错代码
*/
int UnionExecuteExternalCmdDefinedInSpecFile(char *fileName,char *resStr,int sizeOfResStr);

#endif

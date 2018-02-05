// Wolfgang Wang
// 2010-8-19

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionExcuteExternalCmdFile.h"

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
int UnionExecuteExternalCmdDefinedInSpecFile(char *fileName,char *resStr,int sizeOfResStr)
{
	FILE					*fp;
	char					cmdStr[2048+1];
	char					resDefStr[512+1];
	int					lenOfCmdReqStr;
	int					lenOfCmdResStr;
	int					lenOfResDefStr;
	int					ret;
	int					resOffset = 0;
	PUnionExternalCmdFun			pexternCmdFun;
	int					externalCmdDllConnected = 0;
#ifdef _WIN32
	HINSTANCE				externalCmdDll;
#endif
	
	// 打开命令文件
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	while (!feof(fp))
	{
		// 从文件中读取一个指令请求
		memset(cmdStr,0,sizeof(cmdStr));
		lenOfCmdReqStr = UnionReadOneLineFromTxtFile(fp,cmdStr,sizeof(cmdStr));
		if (strncmp(cmdStr,"dllName=",8) == 0) // 是动态库标识
		{
			UnionLog("dllName=%s",cmdStr+8);
#ifdef _WIN32
			// 加载动态库
			if((externalCmdDll = LoadLibrary(cmdStr+8)) == NULL)
			{
				UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: LoadLibrary [%s]!\n",cmdStr+8);
				UnionNotice("LoadLibrary,动库态[%s]装载失败",cmdStr+8);
				ret = -1;
				goto errExit;
			}
			externalCmdDllConnected = 1;
			// 获取外部命令函数的IP地址
			if ((pexternCmdFun = (PUnionExternalCmdFun)GetProcAddress(externalCmdDll,conMngSvrClientExternalCmdFunName)) == NULL)
			{
				UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: GetProcAddress [%s] 出错\n",conMngSvrClientExternalCmdFunName);
				FreeLibrary(externalCmdDll);
				UnionNotice("GetProcAddress获取函数[%s]地址出错\n",conMngSvrClientExternalCmdFunName);
				ret = -1;
				goto errExit;
			}
#endif
			continue;
		}
		if (UnionIsUnixShellRemarkLine(cmdStr))	// 是注释行
			continue;
		if ((lenOfCmdReqStr == 0) || (lenOfCmdReqStr == errCodeEnviMDL_NullLine))	// 空行
			continue;
		if (!externalCmdDllConnected)	// 没有连接动态库
			pexternCmdFun = UnionExcuteExternalCmd;
		// ***** 执行一个外部命令
		//UnionLog("cmdIndex=[%02d]externalReqStr=[%04d][%s]\n",++cmdIndex,lenOfCmdReqStr,cmdStr);
		if ((lenOfCmdResStr = pexternCmdFun(cmdStr,lenOfCmdReqStr,cmdStr,sizeof(cmdStr))) < 0)
		{
			UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: UnionExcuteExternalCmd [%s]!\n",cmdStr);
			ret = lenOfCmdResStr;
			goto errExit;
		}
		cmdStr[lenOfCmdResStr] = 0;
		//UnionLog("cmdIndex=[%02d]externalResStr=[%04d][%s]\n",cmdIndex,lenOfCmdResStr,cmdStr);
		// 从文件中读取响应拼装方法
		memset(resDefStr,0,sizeof(resDefStr));
		lenOfResDefStr = UnionReadOneLineFromTxtFile(fp,resDefStr,sizeof(resDefStr));
		if (UnionIsUnixShellRemarkLine(resDefStr))	// 是注释行,即不要响应
			continue;
		//UnionLog("cmdIndex=[%02d]resDefStr=[%04d][%s]\n",cmdIndex,lenOfResDefStr,resDefStr);
		if ((lenOfResDefStr == 0) || (lenOfResDefStr == errCodeEnviMDL_NullLine))	// 空行,即不要响应
			continue;
		// ***** 根据响应取值定义，将响应打入到响应串中
		if ((ret = UnionFormRecStrUnderSpecDef(resDefStr,lenOfResDefStr,cmdStr,lenOfCmdResStr,
			resStr+resOffset,sizeOfResStr-resOffset)) < 0)
		{
			UnionUserErrLog("in UnionExecuteExternalCmdDefinedInSpecFile:: UnionFormRecStrUnderSpecDef rules = [%s] resStr = [%s]!\n",resDefStr,cmdStr);
			goto errExit;
		}
		//UnionLog("ret = [%d] resOffset = [%d] res=[%04d][%s][%s]\n",ret,resOffset,strlen(resStr),resStr,resStr+resOffset);
		resOffset += ret;
		//UnionLog("cmdIndex=[%02d]resStr=[%04d][%s]\n",cmdIndex,resOffset,resStr);
	}
	ret = 0;
errExit:
#ifdef _WIN32
	// 释放动态库连接
	if (externalCmdDllConnected)
		FreeLibrary(externalCmdDll);	
#endif
	fclose(fp);
	if (ret == 0)
		return(resOffset);
	else
		return(ret);
}

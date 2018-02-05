// 2008/7/21
// Wolfgang Wang
//#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "unionWorkingDir.h"
#include "unionErrCode.h"
#include "UnionLog.h"

#ifdef _WIN32
#include <windows.h>
#include <vcl.h>
char	pgMngSvrTmpFileName[256+1] = "";
unsigned int gunionTmpFileSSN = 0;
// 获取主程序句柄
TApplication *UnionGetApplication();
#endif

// 获得一个暂时文件名
void UnionGetTempFileNameForThisProc(char *fileName)
{
	sprintf(fileName,"%s/%d",getenv("UNIONTEMP"),getpid());
}

/*
功能	获得主工作目录
	对于Unix，该目录为$UNIONMAINDIR
	对于Win，该目录为".\\"
输入参数
	无
输出参数
	workingDir	获取的主工作目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetMainWorkingDir(char *workingDir)
{
	if (workingDir == NULL)
		return(errCodeParameter);
#ifdef _WIN32
	memset(tmpBuf, 0, sizeof(tmpBuf));
        GetCurrentDirectory(sizeof(tmpBuf), tmpBuf);

        strcpy(workingDir, tmpBuf);
#else
	sprintf(workingDir,"%s",getenv("UNIONREC"));
#endif
        //UnionLog("in UnionGetMainWorkingDir:: workingDir = [%s]\n",workingDir);
        return(0);
}

/*
功能	获得暂时工作目录
	目录为"mainWorkingDir/tmp"
输入参数
	无
输出参数
	workingDir	获取的暂时工作目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTempWorkingDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;
	
	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetTempWorkingDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\tmp",mainDir);
#else
	sprintf(workingDir,"%s/tmp",mainDir);
#endif
	return(0);
}

/*
功能	获得仿真测试数据工作目录
	目录为"mainWorkingDir/simuData"
输入参数
	无
输出参数
	workingDir	获取的工作目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetSimuDataWorkingDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;

	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetSimuDataWorkingDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\simuData",mainDir);
#else
	sprintf(workingDir,"%s/simuData",mainDir);
#endif
	return(0);
}

/*
功能	获得tcpipSvr配置文件目录
	目录为"mainWorkingDir/server"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTcpipSvrConfDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;
	
	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetTcpipSvrConfDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\server",mainDir);
#else
	sprintf(workingDir,"%s/server",mainDir);
#endif
	return(0);
}

/*
功能	获得控制定义目录
	目录为"mainWorkingDir/control"
输入参数
	无
输出参数
	workingDir	获取的控制定义目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetControlDefWorkingDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;
	
	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetControlDefWorkingDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\control",mainDir);
#else
	sprintf(workingDir,"%s/control",mainDir);
#endif
	return(0);
}

#ifdef _WIN32
int UnionGetDefFileNameOfOperationController(char *fileName)
{
	char	dir[512+1];
	int	ret;

	if (fileName == NULL)
		return(errCodeParameter);
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetMainWorkingDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionGetMainWorkingDir:: UnionGetDefFileNameOfOperationController!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(fileName,"%s\\operation\\operationControl.Def",dir);
#else
	sprintf(fileName,"%s/operation/operationControl.Def",dir);
#endif
	return(0);
}
#endif
/*
功能	获得暂时文件名称
	该文件为"mainWorkingDir/tmp/x.tmp"
输入参数
        index           暂时文件序号
输出参数
	UnionGetIconDir	获取的文件名称
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTmpFileName(unsigned int index,char *fileName)
{
	char	dir[512+1];
	int	ret;

	if (fileName == NULL)
		return(errCodeParameter);
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetTempWorkingDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionGetTmpFileName:: UnionGetTempWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(fileName,"%s\\%05d.tmp",dir,index % 100 + 1);
#else
	sprintf(fileName,"%s/%05d.tmp",dir,index % 100 + 1);
#endif
	return(0);
}

#ifdef _WIN32
// 生成一个新的暂时文件名称
char *UnionGenerateTmpFileName()
{
        memset(pgMngSvrTmpFileName,0,sizeof(pgMngSvrTmpFileName));
        UnionGetTmpFileName(gunionTmpFileSSN,pgMngSvrTmpFileName);
        gunionTmpFileSSN++;
        return(pgMngSvrTmpFileName);
}
	
// 获得当前的暂时文件名称
char *UnionGetCurrentTmpFileName()
{
	return(pgMngSvrTmpFileName);
}

// 生成一个暂时文件，返回其文件名称
char *UnionGenerateMngSvrTempFile()
{
        memset(pgMngSvrTmpFileName,0,sizeof(pgMngSvrTmpFileName));
        UnionGetTmpFileName(gunionTmpFileSSN,pgMngSvrTmpFileName);
        gunionTmpFileSSN++;
        return(pgMngSvrTmpFileName);
}

int UnionDeleteAllMngSvrTempFile()
{
	int	i;
	char	tmpBuf[512];

        for (i = 0; i < 100; i++)
        {
                memset(tmpBuf,0,sizeof(tmpBuf));
                UnionGetTmpFileName(i,tmpBuf);
                DeleteFile(tmpBuf);
        }
	return(0);
}

// 获取当前暂时文件的名称
char *UnionGetCurrentMngSvrTempFileName()
{
	return(pgMngSvrTmpFileName);
}

// 删除暂时文件
int UnionDeleteMngSvrTempFile()
{
        DeleteFile(pgMngSvrTmpFileName);
	return(0);
}

// 程序设置暂时文件名
void UnionSetMngSvrTempFileName(char *fileName)
{
        if (fileName == NULL)
                return;
        else
                strcpy(pgMngSvrTmpFileName,fileName);
        return;
}
#endif

/*
功能	获得客户端表定义目录
	该目录为"mainWorkingDir/tblQuery"
输入参数
	无
输出参数
	workingDir	获取的客户端表定义工作目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTBLQueryConfDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;

	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetTBLQueryConfDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\tblQuery",mainDir);
#else
	sprintf(workingDir,"%s/tblQuery",mainDir);
#endif
	return(0);
}


/*
功能	获得表记录接口定义目录
	该目录为"mainWorkingDir/tblQuery"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTBLRecInterfaceDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;

	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetTBLRecInterfaceDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\tblQuery",mainDir);
#else
	sprintf(workingDir,"%s/tblQuery",mainDir);
#endif
	return(0);
}
/*
功能	获得菜单定义目录
	该目录为"mainWorkingDir/menu"
输入参数
	无
输出参数
	workingDir	获取的菜单定义目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetMenuDefDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;
	
	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetMenuDefDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\menu",mainDir);
#else
	sprintf(workingDir,"%s/menu",mainDir);
#endif
	return(0);
}

/*
功能	获得图标目录
	该目录为"mainWorkingDir/icon"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetIconDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;

	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetIconDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\icon",mainDir);
#else
	sprintf(workingDir,"%s/icon",mainDir);
#endif
	return(0);
}

/*
功能	获得图标文件名称
	该目录为"mainWorkingDir/icon"
输入参数
	无
输出参数
	UnionGetIconDir	获取的文件名称
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetIconFileName(char *iconName,char *fileName)
{
	char	dir[512+1];
	int	ret;

	if ((iconName == NULL) || (fileName == NULL))
		return(errCodeParameter);
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetIconDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionGetIconFileName:: UnionGetIconDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(fileName,"%s\\%s.ico",dir,iconName);
#else
	sprintf(fileName,"%s/%s.ico",dir,iconName);
#endif
	return(0);
}

/*
功能	获得域值定义目录
	该目录为"mainWorkingDir/enumValueDef"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetEnumValueDefDir(char *workingDir)
{
	char	mainDir[128+1];
	int	ret;

	if (workingDir == NULL)
		return(errCodeParameter);
	memset(mainDir,0,sizeof(mainDir));
	if ((ret = UnionGetMainWorkingDir(mainDir)) < 0)
	{
		UnionUserErrLog("in UnionGetEnumValueDefDir:: UnionGetMainWorkingDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(workingDir,"%s\\enumValueDef",mainDir);
#else
	sprintf(workingDir,"%s/enumValueDef",mainDir);
#endif
	return(0);
}

/*
功能	获得域值定义目录
	该目录为"mainWorkingDir/enumValueDef"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetEnumValueDefFileName(char *enumType,char *fileName)
{
	char	dir[512+1];
	int	ret;

	if ((enumType == NULL) || (fileName == NULL))
		return(errCodeParameter);
	memset(dir,0,sizeof(dir));
	if ((ret = UnionGetEnumValueDefDir(dir)) < 0)
	{
		UnionUserErrLog("in UnionGetEnumValueDefFileName:: UnionGetEnumValueDefDir!\n");
		return(ret);
	}
#ifdef _WIN32
	sprintf(fileName,"%s\\%s.conf",dir,enumType);
#else
	sprintf(fileName,"%s/%s.conf",dir,enumType);
#endif
	return(0);
}

//#pragma package(smart_init)

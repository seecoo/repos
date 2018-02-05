#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"

#include "batchComply.h"

int UnionGenerateHeaderOfBatchComplyFile(FILE *outFp)
{
	char	fullDateTime[14+1];
	
	memset(fullDateTime,0,sizeof(fullDateTime));
	UnionGetFullSystemDateTime(fullDateTime);
	fprintf(outFp,"Auto generated batch comply-link file\n");
	fprintf(outFp,"User:: %s	Time:: %s\n",getenv("LOGNAME"),fullDateTime);
	fprintf(outFp,"\n#!/bin/sh\n");
	return(0);
}

// 根据一个文件中的makefile写一个编译批处理
/*
输入参数
	inputFile	包含了makefile清单的文件
	outputFile	生成的编译批处理文件
	mainDir		目录，只对该目录下的makefile生成批处理
输出参数
	无
返回值
	>=0		包含的makefile的数量
	<0		出错代码
*/
int UnionWriteComplyAllSrcBatchFile(char *inputFile,char *outputFile,char *mainDir)
{
	FILE	*outFp;
	FILE	*inFp;
	char	tmpBuf[1024+1];
	char	dir[512+1],fileName[128+1];
	int	len;
	int	dirLen;
	char	lastDir[512+1] = "";
	char	tmpFileName[128+1];
	int	totalMakefileNum = 0;
	int	totalDirNum = 0;
	int	ret;
	
	// 批开文件
	if ((inFp = fopen(inputFile,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteComplyAllSrcBatchFile:: fopen [%s] error!\n",inputFile);
		return(errCodeUseOSErrCode);
	}
	if ((outFp = fopen(outputFile,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteComplyAllSrcBatchFile:: fopen [%s] error!\n",outputFile);
		fclose(inFp);
		return(errCodeUseOSErrCode);
	}
	UnionGenerateHeaderOfBatchComplyFile(outFp);
	while (!feof(inFp))
	{
		// 读一条记录
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(inFp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		// 判断是否是合法目录
		if ((ret = UnionThisDirIsFirstChildDirOfSpecDir(mainDir,tmpBuf)) <= 0)
		{
			if (ret < 0)
				UnionUserErrLog("in UnionWriteComplyAllSrcBatchFile:: [%s] [%s]!\n",mainDir,tmpBuf);
			continue;
		}
		// 折分获得文件名和目录
		memset(dir,0,sizeof(dir));
		memset(fileName,0,sizeof(fileName));
		if ((ret = UnionGetFileDirAndFileNameFromFullFileName(tmpBuf,len,dir,fileName)) < 0)
		{
			UnionUserErrLog("in UnionWriteComplyAllSrcBatchFile:: UnionGetFileDirAndFileNameFromFullFileName [%s]\n",tmpBuf);
			continue;
		}
		if ((dirLen = strlen(dir)) >= 9)
		{
			if (strcmp(dir+dirLen-9,"/makeBins") == 0)	// 是编译可执行程序
				continue;
		}
		if (strcmp(lastDir,dir) != 0)	// 换了目录
		{
			fprintf(outFp,"\n# 编译目录::%s\n",dir);
			fprintf(outFp,"	cd %s\n",dir);
			strcpy(lastDir,dir);
			totalDirNum++;
		}
		strcpy(tmpFileName,fileName);
		UnionToUpperCase(tmpFileName);
		totalMakefileNum++;
		if (strcmp(tmpFileName,"MAKEFILE") == 0)
			fprintf(outFp,"	make\n");
		else
			fprintf(outFp,"	make -f %s\n",fileName);
	}
	fprintf(outFp,"\n# 共有编译目录::%d个，makefile::%d个\n",totalDirNum,totalMakefileNum);
	printf("\n# 共有编译目录::%d个，makefile::%d个\n",totalDirNum,totalMakefileNum);
	fclose(inFp);
	fclose(outFp);
	return(totalMakefileNum);
}

// 根据一个文件中的makefile写一个编译批处理
/*
输入参数
	inputFile	包含了makefile清单的文件
	outputFile	生成的编译批处理文件
	mainDir		目录，只对该目录下的makefile生成批处理
输出参数
	无
返回值
	>=0		包含的makefile的数量
	<0		出错代码
*/
int UnionWriteLinkAllBinsBatchFile(char *inputFile,char *outputFile,char *mainDir)
{
	FILE	*outFp;
	FILE	*inFp;
	char	tmpBuf[1024+1];
	char	dir[512+1],fileName[128+1];
	int	len;
	int	dirLen;
	char	lastDir[512+1] = "";
	char	tmpFileName[128+1];
	int	totalMakefileNum = 0;
	int	totalDirNum = 0;
	int	ret;
	
	// 批开文件
	if ((inFp = fopen(inputFile,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteLinkAllBinsBatchFile:: fopen [%s] error!\n",inputFile);
		return(errCodeUseOSErrCode);
	}
	if ((outFp = fopen(outputFile,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteLinkAllBinsBatchFile:: fopen [%s] error!\n",outputFile);
		fclose(inFp);
		return(errCodeUseOSErrCode);
	}
	UnionGenerateHeaderOfBatchComplyFile(outFp);
	while (!feof(inFp))
	{
		// 读一条记录
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(inFp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		// 判断是否是合法目录
		if ((ret = UnionThisDirIsFirstChildDirOfSpecDir(mainDir,tmpBuf)) <= 0)
		{
			if (ret < 0)
				UnionUserErrLog("in UnionWriteLinkAllBinsBatchFile:: [%s] [%s]!\n",mainDir,tmpBuf);
			continue;
		}
		// 折分获得文件名和目录
		memset(dir,0,sizeof(dir));
		memset(fileName,0,sizeof(fileName));
		if ((ret = UnionGetFileDirAndFileNameFromFullFileName(tmpBuf,len,dir,fileName)) < 0)
		{
			UnionUserErrLog("in UnionWriteLinkAllBinsBatchFile:: UnionGetFileDirAndFileNameFromFullFileName [%s]\n",tmpBuf);
			continue;
		}
		if ((dirLen = strlen(dir)) >= 9)
		{
			if (strcmp(dir+dirLen-9,"/makeBins") != 0)	// 不是编译可执行程序
				continue;
		}
		else
			continue;
		if (strcmp(lastDir,dir) != 0)	// 换了目录
		{
			fprintf(outFp,"\n# 编译目录::%s\n",dir);
			fprintf(outFp,"	cd %s\n",dir);
			strcpy(lastDir,dir);
			totalDirNum++;
		}
		strcpy(tmpFileName,fileName);
		UnionToUpperCase(tmpFileName);
		totalMakefileNum++;
		if (strcmp(tmpFileName,"MAKEFILE") == 0)
			fprintf(outFp,"	make\n");
		else
			fprintf(outFp,"	make -f %s\n",fileName);
	}
	fprintf(outFp,"\n# 共有编译目录::%d个，makefile::%d个\n",totalDirNum,totalMakefileNum);
	printf("\n# 共有编译目录::%d个，makefile::%d个\n",totalDirNum,totalMakefileNum);
	fclose(inFp);
	fclose(outFp);
	return(totalMakefileNum);
}

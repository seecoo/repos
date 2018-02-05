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

// ����һ���ļ��е�makefileдһ������������
/*
�������
	inputFile	������makefile�嵥���ļ�
	outputFile	���ɵı����������ļ�
	mainDir		Ŀ¼��ֻ�Ը�Ŀ¼�µ�makefile����������
�������
	��
����ֵ
	>=0		������makefile������
	<0		�������
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
	
	// �����ļ�
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
		// ��һ����¼
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(inFp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		// �ж��Ƿ��ǺϷ�Ŀ¼
		if ((ret = UnionThisDirIsFirstChildDirOfSpecDir(mainDir,tmpBuf)) <= 0)
		{
			if (ret < 0)
				UnionUserErrLog("in UnionWriteComplyAllSrcBatchFile:: [%s] [%s]!\n",mainDir,tmpBuf);
			continue;
		}
		// �۷ֻ���ļ�����Ŀ¼
		memset(dir,0,sizeof(dir));
		memset(fileName,0,sizeof(fileName));
		if ((ret = UnionGetFileDirAndFileNameFromFullFileName(tmpBuf,len,dir,fileName)) < 0)
		{
			UnionUserErrLog("in UnionWriteComplyAllSrcBatchFile:: UnionGetFileDirAndFileNameFromFullFileName [%s]\n",tmpBuf);
			continue;
		}
		if ((dirLen = strlen(dir)) >= 9)
		{
			if (strcmp(dir+dirLen-9,"/makeBins") == 0)	// �Ǳ����ִ�г���
				continue;
		}
		if (strcmp(lastDir,dir) != 0)	// ����Ŀ¼
		{
			fprintf(outFp,"\n# ����Ŀ¼::%s\n",dir);
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
	fprintf(outFp,"\n# ���б���Ŀ¼::%d����makefile::%d��\n",totalDirNum,totalMakefileNum);
	printf("\n# ���б���Ŀ¼::%d����makefile::%d��\n",totalDirNum,totalMakefileNum);
	fclose(inFp);
	fclose(outFp);
	return(totalMakefileNum);
}

// ����һ���ļ��е�makefileдһ������������
/*
�������
	inputFile	������makefile�嵥���ļ�
	outputFile	���ɵı����������ļ�
	mainDir		Ŀ¼��ֻ�Ը�Ŀ¼�µ�makefile����������
�������
	��
����ֵ
	>=0		������makefile������
	<0		�������
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
	
	// �����ļ�
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
		// ��һ����¼
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((len = UnionReadOneLineFromTxtStr(inFp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		// �ж��Ƿ��ǺϷ�Ŀ¼
		if ((ret = UnionThisDirIsFirstChildDirOfSpecDir(mainDir,tmpBuf)) <= 0)
		{
			if (ret < 0)
				UnionUserErrLog("in UnionWriteLinkAllBinsBatchFile:: [%s] [%s]!\n",mainDir,tmpBuf);
			continue;
		}
		// �۷ֻ���ļ�����Ŀ¼
		memset(dir,0,sizeof(dir));
		memset(fileName,0,sizeof(fileName));
		if ((ret = UnionGetFileDirAndFileNameFromFullFileName(tmpBuf,len,dir,fileName)) < 0)
		{
			UnionUserErrLog("in UnionWriteLinkAllBinsBatchFile:: UnionGetFileDirAndFileNameFromFullFileName [%s]\n",tmpBuf);
			continue;
		}
		if ((dirLen = strlen(dir)) >= 9)
		{
			if (strcmp(dir+dirLen-9,"/makeBins") != 0)	// ���Ǳ����ִ�г���
				continue;
		}
		else
			continue;
		if (strcmp(lastDir,dir) != 0)	// ����Ŀ¼
		{
			fprintf(outFp,"\n# ����Ŀ¼::%s\n",dir);
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
	fprintf(outFp,"\n# ���б���Ŀ¼::%d����makefile::%d��\n",totalDirNum,totalMakefileNum);
	printf("\n# ���б���Ŀ¼::%d����makefile::%d��\n",totalDirNum,totalMakefileNum);
	fclose(inFp);
	fclose(outFp);
	return(totalMakefileNum);
}

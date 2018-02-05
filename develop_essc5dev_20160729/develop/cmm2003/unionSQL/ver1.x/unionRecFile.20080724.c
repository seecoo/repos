//---------------------------------------------------------------------------


//#pragma hdrstop

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef _WIN32
#include <unistd.h>
#include <libgen.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>
#include <io.h>
#include <dir.h>
#include <sys\stat.h>
#define F_OK 0
#endif

#include "unionRecFile.h"
#ifndef _WIN32
#include "unionREC.h"
#else
#include "unionRECVar.h"
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "unionWorkingDir.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionLockTBL.h"

//---------------------------------------------------------------------------
// 2010-12-13������������
/*
����	�Ե�һ���ļ�Ϊ��׼���Ƚ������ļ����������һ�µ���
�������
	firstFileDir	��һ���ļ���Ŀ¼
	firstFileName	��һ���ļ�����
	secondFileName	�ڶ����ļ�����
	unsameFileFp	��һ�µ�����
�������
	��
����ֵ
	>=0		��һ�µ�����
	<0		�������
*/
int UnionCompareFile_1stFileWithDir(char *firstFileDir,char *firstFileName,char *secondFileName,FILE *unsameFileFp)
{
	char	full1stFileName[512+1];
	
	sprintf(full1stFileName,"%s/%s",firstFileDir,firstFileName);
	return(UnionCompareFile(full1stFileName,secondFileName,unsameFileFp));
}


//---------------------------------------------------------------------------
// 2010-12-13������������
/*
����	�Ե�һ���ļ�Ϊ��׼���Ƚ������ļ����������һ�µ���
�������
	firstFileName	��һ���ļ�����
	secondFileDir	�ڶ����ļ���Ŀ¼
	secondFileName	�ڶ����ļ�����
	unsameFileFp	��һ�µ�����
�������
	��
����ֵ
	>=0		��һ�µ�����
	<0		�������
*/
int UnionCompareFile_2ndFileWithDir(char *firstFileName,char *secondFileDir,char *secondFileName,FILE *unsameFileFp)
{
	char	full2ndFileName[512+1];
	
	sprintf(full2ndFileName,"%s/%s",secondFileDir,secondFileName);
	return(UnionCompareFile(firstFileName,full2ndFileName,unsameFileFp));
}

//---------------------------------------------------------------------------
// 2010-12-13������������
/*
����	�Ե�һ���ļ�Ϊ��׼���Ƚ������ļ����������һ�µ���
�������
	firstFileDir	��һ���ļ���Ŀ¼
	firstFileName	��һ���ļ�����
	secondFileDir	�ڶ����ļ���Ŀ¼
	secondFileName	�ڶ����ļ�����
	unsameFileFp	��һ�µ�����
�������
	��
����ֵ
	>=0		��һ�µ�����
	<0		�������
*/
int UnionCompareFile_BothFileWithDir(char *firstFileDir,char *firstFileName,char *secondFileDir,char *secondFileName,FILE *unsameFileFp)
{
	char	full1stFileName[512+1];
	char	full2ndFileName[512+1];
	
	sprintf(full1stFileName,"%s/%s",firstFileDir,firstFileName);
	sprintf(full2ndFileName,"%s/%s",secondFileDir,secondFileName);
	return(UnionCompareFile(full1stFileName,full2ndFileName,unsameFileFp));
}

//---------------------------------------------------------------------------
// 2010-12-13������������
/*
����	�Ե�һ���ļ�Ϊ��׼���Ƚ������ļ����������һ�µ���
�������
	firstFileName	��һ���ļ�����
	secondFileName	�ڶ����ļ�����
	unsameFileFp	��һ�µ�����
�������
	��
����ֵ
	>=0		��һ�µ�����
	<0		�������
*/
int UnionCompareFile(char *firstFileName,char *secondFileName,FILE *unsameFileFp)
{
	char	tmpBuf1[4096+1],tmpBuf2[4096+1];
	FILE	*firstFp,*secondFp;
	int	ret;
	int	lineIndexOfSecondFile = 0;
	int	lineIndexOfFirstFile = 0;
	int	okLineIndexOfSecondFile = 0;
	int	unsameLines = 0;
	FILE	*outFp = stderr;
	int	isLastLineOK = 0;
	
	outFp = UnionGetValidFp(unsameFileFp);
	if ((firstFileName == NULL) || (secondFileName == NULL))
	{
		UnionUserErrLog("in UnionCompareFile:: null pointer!\n");
		return(errCodeParameter);
	}
	memset(tmpBuf1,0,sizeof(tmpBuf1));
	UnionReadDirFromStr(firstFileName,-1,tmpBuf1);
	if ((firstFp = fopen(tmpBuf1,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionCompareFile:: fopen [%s]\n",tmpBuf1);
		return(errCodeUseOSErrCode);
	}
	memset(tmpBuf1,0,sizeof(tmpBuf1));
	UnionReadDirFromStr(secondFileName,-1,tmpBuf1);
	if ((secondFp = fopen(tmpBuf1,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionCompareFile:: fopen [%s]\n",tmpBuf1);
		fclose(firstFp);
		return(errCodeUseOSErrCode);
	}
	while (!feof(firstFp))
	{
		// �ȴӵ�һ���ļ��ж�ȡһ��
		memset(tmpBuf1,0,sizeof(tmpBuf1));
		if ((ret = UnionReadOneDataLineFromTxtFile(firstFp,tmpBuf1,sizeof(tmpBuf1))) < 0)
			continue;
		lineIndexOfFirstFile++;
		//UnionRealNullLog("firstFile::[%04d][%s]\n",lineIndexOfFirstFile,tmpBuf1);
		if (ret == 0)
			continue;
		// �ٴӵڶ����ļ��ж�ȡһ��
		if (!isLastLineOK)
		{
			// ǰһ�в�һ��
			fseek(secondFp,0,SEEK_SET);	// ����Ϊ�ӵ�һ�ж�
			lineIndexOfSecondFile = 0;
		}
		isLastLineOK = 0;
		while (!feof(secondFp))
		{
			memset(tmpBuf2,0,sizeof(tmpBuf2));
			if ((ret = UnionReadOneDataLineFromTxtFile(secondFp,tmpBuf2,sizeof(tmpBuf2))) < 0)
				continue;
			lineIndexOfSecondFile++;
			//UnionRealNullLog("secondFile::[%04d][%s]\n",lineIndexOfSecondFile,tmpBuf2);
			if (ret == 0)
				continue;
			if (lineIndexOfSecondFile <= okLineIndexOfSecondFile)	// �ڶ����ļ��ĸ������ڵ�һ���ļ��д�����
				continue;
			// �Ƶ��ڶ����ļ��Ѿ��Ƚϳɹ�����֮���ٱȽ�
			if (strcmp(tmpBuf1,tmpBuf2) == 0)	// �����ļ��ĸ���һ��
			{
				okLineIndexOfSecondFile = lineIndexOfSecondFile;	// �õڶ����ļ���OK��Ϊ��ǰ��
				isLastLineOK = 1;
				break;
			}
		}
		if (isLastLineOK)	// ����һ��
			continue;
		unsameLines++;
		//fprintf(outFp,"��һ��::�ļ�[%s]�ĵ�[%04d]��[%s]!\n",firstFileName,lineIndexOfFirstFile,tmpBuf1);
		fprintf(outFp,"��һ��::��[%04d]��[%s]!\n",lineIndexOfFirstFile,tmpBuf1);
	}
	if (unsameLines == 0)
		fprintf(outFp,"��ȫһ��::һ������::[%04d]!\n",lineIndexOfFirstFile);
	else
		fprintf(outFp,"��һ��::��һ������::[%04d],һ������::[%04d],������[%04d]\n",unsameLines,lineIndexOfFirstFile-unsameLines,lineIndexOfFirstFile);
	return(unsameLines);
}

//---------------------------------------------------------------------------
/*
����	����һ���ļ�
�������
	oriFileName	�ļ�����
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionCreateFile(char *oriFileName)
{
	FILE	*fromFp;
	
	if (oriFileName == NULL)
	{
		UnionUserErrLog("in UnionCreateFile:: null pointer!\n");
		return(errCodeParameter);
	}
	if ((fromFp = fopen(oriFileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionCreateFile:: fopen [%s]\n",oriFileName);
		return(errCodeUseOSErrCode);
	}
	fclose(fromFp);
	return(0);
}

//---------------------------------------------------------------------------
/*
����	����һ���ļ�
�������
	oriFileName	Դ�ļ�����
	desDir		Ŀ��Ŀ¼
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionCopyFileToSpecDir(char *oriFileName,char *desDir,char *desFileName)
{
	char	fullFileName[512+1];
	int	offset = 0;
	char	desFileNamePtr[256+1];
	
	if (desDir == NULL)
	{
		if (desFileName == NULL)
			return(errCodeParameter);
	}
	else
		UnionReadDirFromStr(desDir,-1,fullFileName);
	offset = strlen(fullFileName);
	memset(desFileNamePtr,0,sizeof(desFileNamePtr));
	if ((desFileName == NULL) || (strlen(desFileName) == 0))
		UnionReadFileNameFromFullDir(oriFileName,strlen(oriFileName),desFileNamePtr);
	else
		strcpy(desFileNamePtr,desFileName);
	sprintf(fullFileName+offset,"/%s",desFileNamePtr);
	return(UnionCopyFile(oriFileName,fullFileName));
}

/*
����	����Ŀ¼
�������
	fullFileName	�ļ�ȫ����������·��
�������
	��
����ֵ
	>=0		������Ŀ¼��
	<0		�������
*/
int UnionCreateDirFromFullFileName(char *fullFileName)
{
	int	len;
	char	pathGrp[16][128];
	int	pathNum;
	int	index;
	char	tmpDir[1024];
	char	seprator;
	int	offset = 0;
	int	createDirNum = 0;
	int	ret;
	
#ifndef _WIN32
	seprator = '/';
#else
	seprator = '\\'
#endif
	// �۷ֳ���·��
	if ((pathNum = UnionSeprateVarStrIntoVarGrp(fullFileName,len=strlen(fullFileName),seprator,pathGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionCreateDirFromFullFileName:: UnionSeprateVarStrIntoVarGrp [%s]!\n",fullFileName);
		return(pathNum);
	}
	memset(tmpDir,0,sizeof(tmpDir));
	for (index = 0; index < pathNum - 1; index++)	// pathNum - 1,�Ǽ������һ�����ļ�����
	{
		if (index != 0)
		{
			tmpDir[offset] = seprator;
			offset++;
		}
		memcpy(tmpDir+offset,pathGrp[index],len=strlen(pathGrp[index]));
		offset += len;
		if (offset == 0)
			continue;
		if (UnionExistsDir(tmpDir))	// Ŀ¼�Ѵ���
			continue;
		if ((ret = UnionCreateDir(tmpDir)) < 0)
		{
			UnionUserErrLog("in UnionCreateDirFromFullFileName:: UnionCreateDir [%s]!\n",tmpDir);
			return(ret);
		}
		UnionLog("in UnionCreateDirFromFullFileName:: UnionCreateDir [%s] OK!\n",tmpDir);
		createDirNum++;
	}
	if (createDirNum > 0)
		UnionLog("in UnionCreateDirFromFullFileName:: createDirNum = [%d]\n",createDirNum);
	return(createDirNum);
}

/*
����	����һ���ļ�
�������
	oriFileName	Դ�ļ�����
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionBackupFileAutomatically(char *oriFileName)
{
	char	tmpFileName[512+1];
	char	dateTime[20+1];
	char	nullFileName[256+1];
	char	*ptr;
	int	ret;
	
	//UnionLog("in UnionBackupFileAutomatically:: now backuping %s ...\n",oriFileName);
	if (!UnionExistsFile(oriFileName))
		return(0); 
	memset(dateTime,0,sizeof(dateTime));
	UnionGetFullSystemDateTime(dateTime);
	memset(tmpFileName,0,sizeof(tmpFileName));
	sprintf(tmpFileName,"%s.%s.bak",oriFileName,dateTime);
	// ��ȡ���ļ�����
	memset(nullFileName,0,sizeof(nullFileName));
	if ((ret = UnionReadFullFileNameFromFullDir(tmpFileName,strlen(tmpFileName),nullFileName)) < 0)
	{
		UnionUserErrLog("in UnionBackupFileAutomatically:: UnionReadFullFileNameFromFullDir [%d][%s]!\n",strlen(tmpFileName),tmpFileName);
		return(ret);
	}
	if ((ptr = getenv("UNIONSRCBAKDIR")) != NULL)
	{
		sprintf(tmpFileName,"%s/%s",ptr,nullFileName);
	}
	UnionLog("in UnionBackupFileAutomatically:: backup %s to %s ...\n",oriFileName,tmpFileName);
	return(UnionCopyFile(oriFileName,tmpFileName));
}

//#include "unionRecFile.h"

//----------------------------------------------------------------------------
/*
        Author: HuangBaoxin Date: 2008\11\11
        ���ܣ���ȡ·����
        ���룺FullFileName���ļ���(��������·��)
        �����Dir(�ļ�����·����)
        ���أ�0
        ���磺�ļ� C:\aa\bb\c.txt, ·����C:\aa\bb
*/
int UnionGetDirExpFileName(char *FullFileName, char *Dir)
{
        int iIndex1, iIndex2 = 0;
        /*
        String strTmp1, strTmp2, strDir;
        String fileName;
        */
        char    strTmp1[512 + 1], strTmp2[512 + 1];
        char    fileName[512 + 1];
        char    *pos;

        memset(fileName, 0, sizeof(fileName));
        strcpy(fileName, FullFileName);
        
        memset(strTmp1, 0, sizeof(strTmp1));
        strcpy(strTmp1, FullFileName);

#ifdef _WIN32
        pos     = strstr(strTmp1, "\\");
#else
        pos     = strstr(strTmp1, "/");
#endif
        // fileName = FullFileName;
        // strTmp1 = FullFileName;
        // iIndex1 = strTmp1.Pos("\\");
        iIndex1 = pos - strTmp1;
        iIndex2 += iIndex1;

        while (iIndex1 > 0)
        {
                // strTmp2 = strTmp1.SubString(iIndex1+1, strlen(FullFileName) - iIndex1);
                // strTmp1 = strTmp2;

                memcpy(strTmp2, strTmp1 + iIndex1 + 1, strlen(FullFileName) - iIndex1);
                strcpy(strTmp1, strTmp2);

#ifdef _WIN32
                pos     = strstr(strTmp1, "\\");
#else
                pos     = strstr(strTmp1, "/");
#endif
                iIndex1 = pos - strTmp1;

                if (iIndex1 >= 0)
                        iIndex2 += iIndex1 + 1;

                // iIndex1 = strTmp1.Pos("\\");
                // iIndex2 += iIndex1;
        }
        /*
        strDir = fileName.SubString(1, iIndex2 - 1);
        memset(Dir, 0, sizeof(Dir));
        strcpy(Dir, strDir.c_str());
        */
        memcpy(Dir, fileName, iIndex2);
        
        return 0;
}
//----------------------------------------------------------------------------
/*
����	����·�������ļ���ƴװ���ļ�ȫ��
�������
	dirName	  ·����
	fileName  �ļ���
�������
	fullFileName  �ļ�ȫ��
����ֵ
	>=0		�ļ�ȫ������
	<0		�������
*/
int UnionFormFullFileName(char *dirName,char *fileName,char *fullFileName)
{
	char	tmpBuf[512];
	
	if (dirName == NULL || fileName == NULL)
	{
		UnionUserErrLog("in UnionFormFullFileName:: dirName or fileName is null!\n");
                return(errCodeParameter);
	}

	if (strlen(dirName) > 0)
		sprintf(tmpBuf,"%s/%s",dirName,fileName);
	else
		strcpy(tmpBuf,fileName);
	UnionReadDirFromStr(tmpBuf,-1,fullFileName);
	UnionLog("oriDir=[%s]oriFileName=[%s]fullFileName=[%s]!\n",dirName,fileName,fullFileName);
	return (strlen(fullFileName));
}

//----------------------------------------------------------------------------
/*
����	������Ŀ¼���ϲ���һ��Ŀ¼��
�������
	firstDir	��һ��Ŀ¼
	secondDir	�ڶ���Ŀ¼
�������
	newDirName  	��Ŀ¼��
����ֵ
	>=0		Ŀ¼����
	<0		�������
*/
int UnionFormDirName(char *firstDir,char *secondDir,char *newDirName)
{
	if (newDirName == NULL)
		return(errCodeParameter);
	strcpy(newDirName,firstDir);
	return(UnionAppendFirstDirToSecondDir(secondDir,newDirName));
}

//----------------------------------------------------------------------------
/*
����	����һ��Ŀ¼׷���ӵ��ڶ���Ŀ¼��
�������
	firstDir	��һ��Ŀ¼
	secondDir	�ڶ���Ŀ¼
�������
	secondDir  	�ڶ���Ŀ¼
����ֵ
	>=0		Ŀ¼����
	<0		�������
*/
int UnionAppendFirstDirToSecondDir(char *firstDir,char *secondDir)
{
	int	offset = 0;
	
	if (strlen(firstDir) == 0)
		return(strlen(secondDir));
	if ((offset = strlen(secondDir)) > 0)
	{
#ifdef _WIN32
		if (secondDir[offset] != '\\')
		{
			secondDir[offset] = '\\';
			offset++;
		}
#else
		if (secondDir[offset] != '/')
		{
			secondDir[offset] = '/';
			offset++;
		}
#endif
	}
	strcpy(secondDir+offset,firstDir);
	return(offset+strlen(firstDir));
}

/*
����	����ʱĿ¼�ָ��ļ�
�������
	oriFileName	Դ�ļ�����
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionRestoreFileFromTmpDir(char *oriFileName)
{
	char	nullFileName[256+1];
	char	tmpDir[256+1];
	char	tmpFileName[512+1];

	memset(tmpDir,0,sizeof(tmpDir));
	UnionGetTempWorkingDir(tmpDir);
	memset(nullFileName,0,sizeof(nullFileName));
	UnionReadFileNameFromFullDir(oriFileName,strlen(oriFileName),nullFileName);
	sprintf(tmpFileName,"%s/%s",tmpDir,nullFileName);
	return(UnionCopyFile(tmpFileName,oriFileName));
}


/*
����	����һ���ļ�����ʱĿ¼
�������
	oriFileName	Դ�ļ�����
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionBackupFileToTmpDir(char *oriFileName)
{
	char	nullFileName[256+1];
	char	tmpDir[256+1];
	char	tmpFileName[512+1];
	
	if (!UnionExistsFile(oriFileName))
		return(0); 
	memset(tmpDir,0,sizeof(tmpDir));
	UnionGetTempWorkingDir(tmpDir);
	memset(nullFileName,0,sizeof(nullFileName));
	UnionReadFileNameFromFullDir(oriFileName,strlen(oriFileName),nullFileName);
	sprintf(tmpFileName,"%s/%s",tmpDir,nullFileName);
	return(UnionCopyFile(oriFileName,tmpFileName));
}

//---------------------------------------------------------------------------
/*
����	����һ���ļ�
�������
	oriFileName	Դ�ļ�����
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionCopyFile(char *oriFileName,char *desFileName)
{
	char	tmpBuf[1024+1];
	FILE	*fromFp,*toFp;
	int	ret,writeLen;
	int	fileSize = 0;
	
	if ((oriFileName == NULL) || (desFileName == NULL))
	{
		UnionUserErrLog("in UnionCopyFile:: null pointer!\n");
		return(errCodeParameter);
	}
	if ((fromFp = fopen(oriFileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionCopyFile:: fopen [%s]\n",oriFileName);
		return(errCodeUseOSErrCode);
	}
	if ((toFp = fopen(desFileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionCopyFile:: fopen [%s]\n",desFileName);
		fclose(fromFp);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fromFp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = fread(tmpBuf,1,sizeof(tmpBuf)-1,fromFp)) < 0)
		{
			UnionSystemErrLog("in UnionCopyFile:: fread from [%s]!\n",oriFileName);
			fclose(fromFp);
			fclose(toFp);
			return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
			//continue;
		}
		if (ret == 0)
			continue;
		if ((writeLen = fwrite(tmpBuf,1,ret,toFp)) < 0)
		{
			UnionSystemErrLog("in UnionCopyFile:: fwrite to [%s]!\n",desFileName);
			fclose(fromFp);
			fclose(toFp);
			return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
			//continue;
		}
		fileSize += ret;
	}
	fflush(toFp);
	fclose(toFp);
	fclose(fromFp);
	//UnionLog("in UnionCopyFile:: fileSize = [%ld] of [%s][%s]!\n",fileSize,oriFileName,desFileName);
	return(fileSize);
}

//---------------------------------------------------------------------------
/*
����	����һ���ļ�׷�ӵ��ڶ����ļ���
�������
	oriFileName	Դ�ļ�����
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionAppend2ndFileTo1stFile(char *oriFileName,char *desFileName)
{
	char	tmpBuf[1024+1];
	FILE	*fp;
	int	fileSize = 0;
	
	if ((oriFileName == NULL) || (desFileName == NULL))
	{
		UnionUserErrLog("in UnionCopyFile:: null pointer!\n");
		return(errCodeParameter);
	}
	sprintf(tmpBuf,"cat %s >> %s",oriFileName,desFileName);
	system(tmpBuf);
	if ((fp = fopen(desFileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionCopyFile:: fopen [%s]\n",oriFileName);
		return(errCodeUseOSErrCode);
	}
	fseek(fp,0,SEEK_SET);
	fileSize = ftell(fp);
	fclose(fp);
	return(fileSize);
}

/*
����	������һ���ļ�
�������
	oriFileName	Դ�ļ�����
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionRenameFile(char *oriFileName,char *desFileName)
{
	int	ret;
	
	if ((ret = UnionCopyFile(oriFileName,desFileName)) < 0)
	{
		UnionUserErrLog("in UnionRenameFile:: UnionCopyFile!\n");
		return(ret);
	}
	UnionDeleteRecFile(oriFileName);
	return(ret);
}

//---------------------------------------------------------------------------
/*
����	������һ����¼�ļ�
�������
	oriFileName	Դ�ļ�����
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		��¼��
	<0		�������
*/
int UnionRenameRecFile(char *oriFileName,char *desFileName)
{
	char	tmpBuf[1024+1];
	FILE	*fromFp,*toFp;
	int	ret;
	int	recNum;
	
	if ((oriFileName == NULL) || (desFileName == NULL))
	{
		UnionUserErrLog("in UnionRenameRecFile:: null pointer!\n");
		return(errCodeParameter);
	}
	if ((fromFp = fopen(oriFileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionRenameRecFile:: fopen [%s]\n",oriFileName);
		return(errCodeUseOSErrCode);
	}
	if ((toFp = fopen(desFileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionRenameRecFile:: fopen [%s]\n",desFileName);
		fclose(fromFp);
		return(errCodeUseOSErrCode);
	}
	recNum = 0;
	while (!feof(fromFp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fromFp,tmpBuf,sizeof(tmpBuf))) < 0)
			continue;
		fprintf(toFp,"%s\n",tmpBuf);
		++recNum;
	}
	fflush(toFp);
	fclose(toFp);
	fclose(fromFp);
	UnionDeleteRecFile(oriFileName);
	return(recNum);
}

//---------------------------------------------------------------------------
/*
���ܣ�  ɾ��Ŀ¼�е��ļ�
������
        dir[IN]         ��Ҫ��ɾ����Ŀ¼��
return:
        -1              ɾ��ʧ��
        0               ɾ���ɹ�
*/
int UnionDelDirFiles(char *dir)
{        
#ifdef _WIN32
	char            curWorkDir[512 + 1];
        char            delDir[512 + 1];
        struct ffblk    fblk;
        int             done;
        
        if (!UnionExistsDir(dir))
        {
                return -1;
        }

        // ���浱ǰ����Ŀ¼
        memset(curWorkDir, 0, sizeof(curWorkDir));
        getcwd(curWorkDir, sizeof(curWorkDir));

        // ��ȡ��Ҫɾ����Ŀ¼·��
        memset(delDir, 0, sizeof(delDir));
        strcpy(delDir, dir);
        strcat(delDir, "\\*");

        done            = findfirst(delDir, &fblk, FA_RDONLY | FA_HIDDEN | FA_DIREC | FA_SYSTEM | FA_ARCH);
        while (!done)
        {
                if(16 != fblk.ff_attrib)
                {
                        chmod(fblk.ff_name, S_IREAD | S_IWRITE);
                        unlink(fblk.ff_name);
                }

                done    = findnext(&fblk);
        }

        findclose(&fblk);
        chdir(curWorkDir);
#endif
	return 0;
}
//----------------------------------------------------------------------------
// ��ȡһ���ļ��Ĵ�С
long UnionFileSize(char *fileName)
{
        FILE    *stream;
        long    curPos, size;
        
        if (UnionExistsFile(fileName) <= 0)
                return -1;
        
        stream  = fopen(fileName, "r");

        if(NULL == stream)
                return -1;


        curPos  = ftell(stream);

        fseek(stream, 0L, SEEK_END);
        size    = ftell(stream);

        fseek(stream, curPos, SEEK_SET);
        
        fclose(stream);

        return size;
}
//---------------------------------------------------------------------------
/*
����	��һ����¼�ļ��ж�ȡһ����¼��
�������
	fileName	��¼�ļ���
	sizeOfRecStr	���ռ�¼���Ļ���Ĵ�С
�������
	recStr		��ȡ�ļ�¼��
����ֵ
	>=0		��¼���ĳ���
	<0		�������
*/
int UnionReadRecStrFromFile(char *fileName,char *recStr,int sizeOfRecStr)
{
	int	ret;
        // modify by wangcj, 2008-10-14
	// char	tmpBuf[512+1];
        // to
        char	tmpBuf[3072+1];
        // modify end.
	FILE	*fp;
	
	if ((fileName == NULL) || (recStr == NULL))
		return(errCodeParameter);
	
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadRecStrFromFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fp,tmpBuf,sizeof(tmpBuf))) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;

		if ((ret = strlen(tmpBuf)) >= sizeOfRecStr)
		{
			UnionUserErrLog("in UnionReadRecStrFromFile:: small buffer!\n");
                        fclose(fp);
			return(errCodeSmallBuffer);
		}
		strcpy(recStr,tmpBuf);
		//UnionLog("*** [%s] [%s] [%d]\n",tmpBuf,recStr,ret);
		break;
	}
	fclose(fp);
	return(ret);
}

/*
����	��һ����¼��д�뵽��¼�ļ�
�������
	fileName	��¼�ļ���
	recStr		��¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionWriteRecStrIntoFile(char *fileName,char *recStr,int lenOfRecStr)
{
	FILE	*fp;
	
	if ((fileName == NULL) || (recStr == NULL))
		return(errCodeParameter);

	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionWriteRecStrIntoFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fwrite(recStr,1,lenOfRecStr,fp);
	fflush(fp);
	fclose(fp);
	return(0);
}

/*
����	��һ�������Ƽ�¼��׷�ӵ���¼�ļ�
�������
	fileName	��¼�ļ���
	recStr		��¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionAppendBinaryRecStrToFile(char *fileName,char *recStr,int lenOfRecStr)
{
	FILE	*fp;
	int	index;
	
	if ((fileName == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
	if ((fp = fopen(fileName,"a+")) == NULL)
	{
		UnionSystemErrLog("in UnionAppendRecStrToFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	for (index = 0; index < lenOfRecStr; index++)
	{
		//fprintf(fp,"%02X",recStr[index]);   //modify by hzh in 2011.8.9
		fprintf(fp,"%02X",0XFF&recStr[index]);
	}
	fflush(fp);
	fclose(fp);
	return(0);
}

/*
����	��һ����¼��׷�ӵ���¼�ļ�
�������
	fileName	��¼�ļ���
	recStr		��¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionAppendRecStrToFile(char *fileName,char *recStr,int lenOfRecStr)
{
	FILE	*fp;
	
	if ((fileName == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
	if ((fp = fopen(fileName,"a+")) == NULL)
	{
		UnionSystemErrLog("in UnionAppendRecStrToFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	fprintf(fp,"%s\n",recStr);
	fflush(fp);
	fclose(fp);
	return(0);
}

/*
����	��һ����׷�ӵ��ļ�
�������
	fileName	��¼�ļ���
	fmt		��
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionAppendToFile(char *fileName,char *fmt,...)
{
	FILE		*fp;
	va_list 	args;
	long		fileSize;
		
	if (fileName == NULL)
		return(errCodeParameter);
		
	if ((fp = fopen(fileName,"a+")) == NULL)
	{
		UnionSystemErrLog("in UnionAppendRecStrToFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	va_start(args,fmt);
	vfprintf(fp,fmt,args);
	va_end(args);
	fileSize = ftell(fp);
	fflush(fp);
	fclose(fp);
	return(fileSize);
}

/*
����	��һ����д�뵽�ļ�
�������
	fileName	��¼�ļ���
	fmt		��
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionWriteToFile(char *fileName,char *fmt,...)
{
	FILE		*fp;
	va_list 	args;
	long		fileSize;
		
	if (fileName == NULL)
		return(errCodeParameter);
		
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionAppendRecStrToFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	va_start(args,fmt);
	vfprintf(fp,fmt,args);
	va_end(args);
	fileSize = ftell(fp);
	fflush(fp);
	fclose(fp);
	return(fileSize);
}


// Mary add begin, 2008-10-27
/*
����	��һ����¼��׷�ӵ���¼�ļ���ʹ���ļ�ָ��
�������
	hdl		�ļ�ָ��
	recStr		��¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionAppendRecStrToFileUseFileHDL(TUnionRecFileHDL hdl,char *recStr,int lenOfRecStr)
{
	// FILE	*fp;
	
	if ((hdl == NULL) || (recStr == NULL))
		return(errCodeParameter);
	
	fseek(hdl,0,SEEK_END);
	fprintf(hdl,"%s\n",recStr);
	// fflush(fp);
        fflush(hdl);

	return(0);
}

/* 
����	Ϊд�ļ��򿪼�¼�ļ����
�������
	fileName	�ļ�����
�������
	��
����ֵ
	�ļ����
	NULL		����
*/
TUnionRecFileHDL UnionOpenRecFileHDLForWrite(char *fileName)
{
	FILE	*fp;
	
#ifdef _withFileLock_
	int	ret;
	if ((ret = UnionApplyRecWritingLockOfSpecRecWithTimeout("file",fileName,1)) < 0)
	{
		UnionUserErrLog("in UnionOpenRecFileHDLForWrite:: UnionApplyRecWritingLockOfSpecRecWithTimeout [%s] !\n",fileName);
		return(NULL);
	}
#endif
	fp=fopen(fileName,"w");
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionOpenRecFileHDLForWrite:: open [%s] for write fail!\n",fileName);
		return(NULL);
	}
	return(fp);
}
// Mary add end, 2008-10-27

/* 
����	Ϊд�ļ��򿪼�¼�ļ����
�������
	fileName	�ļ�����
�������
	��
����ֵ
	�ļ����
	NULL		����
*/
TUnionRecFileHDL UnionOpenRecFileHDLWithoutLock(char *fileName)
{
	FILE	*fp;
	
	fp=fopen(fileName,"w");
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionOpenRecFileHDLForWrite:: open [%s] for write fail!\n",fileName);
		return(NULL);
	}
	return(fp);
}
// Mary add end, 2008-10-27

/* 
����	�򿪼�¼�ļ����
�������
	fileName	�ļ�����
�������
	��
����ֵ
	�ļ����
	NULL		����
*/
TUnionRecFileHDL UnionOpenRecFileHDL(char *fileName)
{
	FILE	*fp;
	
#ifdef _withFileLock_
	int	ret;
	if ((ret = UnionApplyRecWritingLockOfSpecRecWithTimeout("file",fileName,1)) < 0)
	{
		UnionUserErrLog("in UnionOperRecFileHDL:: UnionApplyRecWritingLockOfSpecRecWithTimeout [%s] !\n",fileName);
		return(NULL);
	}
#endif
	fp=fopen(fileName,"r");
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionOperRecFileHDL:: open [%s] for read fail!\n",fileName);
		return(NULL);
	}
	return(fp);
}

/* 
����	�ر���¼�ļ����
�������
	hdl	�ļ����
�������
	��
����ֵ
	��
*/
void UnionCloseRecFileHDL(TUnionRecFileHDL hdl)
{
	if (hdl != NULL)
		fclose(hdl);
}

/* 
����	�ر���¼�ļ����
�������
	hdl	�ļ����
�������
	��
����ֵ
	��
*/
void UnionCloseRecFileHDLOfFileName(TUnionRecFileHDL hdl,char *fileName)
{
	if (hdl != NULL)
		fclose(hdl);
#ifdef _withFileLock_
	UnionFreeRecWritingLockOfSpecRec("file",fileName);
#endif
}

/* 
����	�ر���¼�ļ����
�������
	hdl	�ļ����
�������
	��
����ֵ
	��
*/
void UnionCloseRecFileHDLOfFileNameWithoutLock(TUnionRecFileHDL hdl,char *fileName)
{
	if (hdl != NULL)
		fclose(hdl);
}

/* 
����	����ļ��еļ�¼������
�������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0		�ļ��еļ�¼������
	<0		�������
*/
int UnionGetTotalRecNumInFile(char *fileName)
{
	FILE	*fp;
	int	iRet,iRecNum=0;
	char	caBuf[1024];
	
	if (fileName == NULL)
	{
		UnionUserErrLog("in UnionGetTotalRecNumInFile:: null pointer!\n");
		return(errCodeParameter);
	}
	
	fp=fopen(fileName,"r");
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionGetTotalRecNumInFile:: open [%s] for read fail!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	while(!feof(fp))
	{
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionReadOneLineFromTxtStr(fp,caBuf,sizeof(caBuf));
		if (iRet <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(caBuf))
			continue;
		iRecNum++;
	}
	
	fclose(fp);
	return(iRecNum);
}

/*
����	����ָ��ֵ���ض������е�λ��
�������
	caValue	        ָ��ֵ
	fldValue	�ض�
	maxNum		������������
�������
	��
����ֵ
	>=0		������λ��
	<0		�����ڻ�������
*/
int UnionIndexOfValueInSpecArray(char *caValue, char fldValue[][128+1],int maxNum)
{
        int             i;

        if (NULL == caValue)
        {
                UnionUserErrLog("in UnionIsExistsInSpecArray:: null pointer!\n");
		return(errCodeParameter);
        }

        for (i = 0; i < maxNum; i++)
        {
                if (strcmp(caValue, fldValue[i]) == 0)
                        return i;
        }

        return -1;
}

/*
����	����ļ������м�¼��ָ������
�������
	fileName	�ļ�����
	fldName		����
	maxNum		�������������
�������
	��
����ֵ
	>=0		������������
	<0		�������
*/
int UnionReadAllRecFldInFile(char *fileName,char *fldName,char fldValue[][128+1],int maxNum)
{
	FILE	*fp;
	int	iRet,iRecNum=0;
	char	caBuf[3072 + 1];
	int	fldLen;
	
	if (fileName == NULL)
	{
		UnionUserErrLog("in UnionReadAllRecFldInFile:: null pointer!\n");
		return(errCodeParameter);
	}
	
	fp=fopen(fileName,"r");
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionReadAllRecFldInFile:: open [%s] for read fail!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	while(!feof(fp))
	{
		if (iRecNum >= maxNum)
			break;
		memset(caBuf,0,sizeof(caBuf));
		iRet=UnionReadOneLineFromTxtStr(fp,caBuf,sizeof(caBuf));
		if (iRet <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(caBuf))
			continue;
		if ((fldLen = UnionReadRecFldFromRecStr(caBuf,iRet,fldName,fldValue[iRecNum],sizeof(fldValue[iRecNum]))) < 0)
			continue;
                // add by chenliang, 2009-01-08
                if ((iRet = UnionIndexOfValueInSpecArray(fldValue[iRecNum], fldValue, iRecNum)) >= 0)
                        continue;
                // add end.
		iRecNum++;
	}
	
	fclose(fp);
	return(iRecNum);
}

/*
����	���ļ��ж�ȡ��һ����¼
�������
	hdl		�ļ�ָ��
	sizeOfRecStr	���ռ�¼�Ļ���Ĵ�С
�������
	recStr		��ȡ�ļ�¼
����ֵ
	>=0		��ȡ�ļ�¼��С,=0,��ʾ�ļ������޺�����¼
	<0		�������
*/
int UnionReadNextRecFromFile(TUnionRecFileHDL hdl,char *recStr,int sizeOfRecStr)
{
	int	iLen;
	
	if ((hdl == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionReadNextRecFromFile:: null pointer!\n");
		return(errCodeParameter);
	}
	while(!feof(hdl))
	{
		memset(recStr,0,sizeOfRecStr);
		if ((iLen=UnionReadOneLineFromTxtStr(hdl,recStr,sizeOfRecStr)) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(recStr))
			continue;
		return(iLen);
	}
	return(0);
}

/* 
����	���ļ��ж�ȡָ�������ļ�¼
�������
	hdl		�ļ�ָ��
	index		��¼������
	sizeOfRecStr	���ռ�¼�Ļ���Ĵ�С
�������
	recStr		��ȡ�ļ�¼
����ֵ
	>=0		��ȡ�ļ�¼��С,=0,��ʾ�ļ������޼�¼
	<0		�������
*/
int UnionReadRecOfIndexFromFile(TUnionRecFileHDL hdl,unsigned int index,char *recStr,int sizeOfRecStr)
{
	int	iLen;
	int	recNum = 0;
	
	if ((hdl == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionReadNextRecFromFile:: null pointer!\n");
		return(errCodeParameter);
	}
	fseek(hdl,0,SEEK_SET);
	while(!feof(hdl))
	{
		memset(recStr,0,sizeOfRecStr);
		if ((iLen=UnionReadOneLineFromTxtStr(hdl,recStr,sizeOfRecStr)) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(recStr))
			continue;
		if (recNum == index)
			return(iLen);
		recNum++;
	}
	return(0);
}

/* 
����	���ļ��ж�ȡָ�������ļ�¼
�������
	fileName	�ļ�����
	index		��¼������
	sizeOfRecStr	���ռ�¼�Ļ���Ĵ�С
�������
	recStr		��ȡ�ļ�¼
����ֵ
	>=0		��ȡ�ļ�¼��С,=0,��ʾ�ļ������޺�����¼
	<0		�������
*/
int UnionReadRecOfIndexDirectlyFromFile(char *fileName,unsigned int index,char *recStr,int sizeOfRecStr)
{
	int	iLen;
	int	recNum = 0;
        FILE    *fp;
	
	if ((fileName == NULL) || (recStr == NULL))
	{
		UnionUserErrLog("in UnionReadRecOfIndexDirectlyFromFile:: null pointer!\n");
		return(errCodeParameter);
	}
	
	fp=fopen(fileName,"r");
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionReadRecOfIndexDirectlyFromFile:: open [%s] for read fail!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	while(!feof(fp))
	{
		memset(recStr,0,sizeOfRecStr);
		if ((iLen=UnionReadOneLineFromTxtStr(fp,recStr,sizeOfRecStr)) <= 0)
			continue;
		if (UnionIsUnixShellRemarkLine(recStr))
			continue;
		if (recNum == index)
                {       fclose(fp);
			return(iLen);
                }
		recNum++;
	}
        fclose(fp);
	return(0);
}
//---------------------------------------------------------------------------
/*
����    ���������һ���ļ������ݽ�������
�������
	fileName	��¼�ļ���
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionSortFileContentInAscOrder(char *fileName)
{
        int	ret;
	char	tmpBuf[512+1];
	char	tmpFileName[512+1];
	FILE	*fromFp,*toFp;

	if (fileName == NULL)
		return(errCodeParameter);

        // ���ļ���������
        if (UnionFileSize(fileName) <= 0)
                return 0;

        // ��Դ�ļ�д��һ����ʱ�ļ�
	if ((fromFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionSortFileContentInAscOrder:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

        // ����һ����ʱ�ļ������ڱ���Դ�ļ�����������
        sprintf(tmpFileName,"%s.tmp",fileName);
	if ((toFp = fopen(tmpFileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionSortFileContentInAscOrder:: fopen [%s]\n",tmpFileName);
		fclose(fromFp);
		return(errCodeUseOSErrCode);
	}
        fclose(toFp);

        while (!feof(fromFp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fromFp,tmpBuf,sizeof(tmpBuf))) < 0)
			continue;
		if (ret == 0)
			continue;
                if ((ret = UnionInsertRecStrToFileInAscOrder(tmpFileName, tmpBuf, strlen(tmpBuf))) < 0)
                {
                        UnionSystemErrLog("In UnionSortFileContentInAscOrder:: UnionInsertRecStrToFileInAscOrder error, ret is: [%d].\n", ret);
                        fclose(fromFp);
                        UnionDeleteRecFile(tmpFileName);
                        return ret;
                }
	}
        fclose(fromFp);

	// ����ʱ�ļ�д��Դ�ļ�
	if ((ret = UnionRenameRecFile(tmpFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionSortFileContentInAscOrder:: UnionRenameRecFile [%s] to [%s]\n",tmpFileName,fileName);
		return(ret);
	}
	return(ret);
}
//---------------------------------------------------------------------------
/*
����    ���ս����һ���ļ������ݽ�������
�������
	fileName	��¼�ļ���
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionSortFileContentInDesOrder(char *fileName)
{
        int	ret;
	char	tmpBuf[512+1];
	char	tmpFileName[512+1];
	FILE	*fromFp,*toFp;

	if (fileName == NULL)
		return(errCodeParameter);

        // ���ļ���������
        if (UnionFileSize(fileName) <= 0)
                return 0;

        // ��Դ�ļ�д��һ����ʱ�ļ�
	if ((fromFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionSortFileContentInDesOrder:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

        // ����һ����ʱ�ļ������ڱ���Դ�ļ�����������
        sprintf(tmpFileName,"%s.tmp",fileName);
	if ((toFp = fopen(tmpFileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionSortFileContentInDesOrder:: fopen [%s]\n",tmpFileName);
		fclose(fromFp);
		return(errCodeUseOSErrCode);
	}
        fclose(toFp);

        while (!feof(fromFp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fromFp,tmpBuf,sizeof(tmpBuf))) < 0)
			continue;
		if (ret == 0)
			continue;
                if ((ret = UnionInsertRecStrToFileInDesOrder(tmpFileName, tmpBuf, strlen(tmpBuf))) < 0)
                {
                        UnionSystemErrLog("In UnionSortFileContentInDesOrder:: UnionInsertRecStrToFileInAscOrder error, ret is: [%d].\n", ret);
                        fclose(fromFp);
                        UnionDeleteRecFile(tmpFileName);
                        return ret;
                }
	}
        fclose(fromFp);

	// ����ʱ�ļ�д��Դ�ļ�
	if ((ret = UnionRenameRecFile(tmpFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionSortFileContentInDesOrder:: UnionRenameRecFile [%s] to [%s]\n",tmpFileName,fileName);
		return(ret);
	}
	return(ret);	
}
//---------------------------------------------------------------------------

/*
����	���ս���ķ�ʽ��һ����¼�ļ��в���һ����¼��
����    ��¼�ļ��������һ�����ս���ʽ���е��ļ�
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionInsertRecStrToFileInDesOrder(char *fileName,char *recStr,int lenOfRecStr)
{
	return(UnionInsertRecStrToFileInOrderByFld(fileName,recStr,lenOfRecStr,"",0));
}
//---------------------------------------------------------------------------
/*
����	��������ķ�ʽ��һ����¼�ļ��в���һ����¼��
����    ��¼�ļ��������һ����������ʽ���е��ļ�
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionInsertRecStrToFileInAscOrder(char *fileName,char *recStr,int lenOfRecStr)
{
	return(UnionInsertRecStrToFileInOrderByFld(fileName,recStr,lenOfRecStr,"",1));
}

//---------------------------------------------------------------------------
/*
����	��������ķ�ʽ��һ����¼�ļ��в���һ����¼��
����    ��¼�ļ��������һ����������ʽ���е��ļ�
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
	fldName		���ݸ���������
	isAscOrder	����ʽ�������򣬣�����
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionInsertRecStrToFileInOrderByFld(char *fileName,char *recStr,int lenOfRecStr,char *fldName,int isAscOrder)
{
	int	ret;
	char	tmpBuf[512+1];
	char	tmpFileName[512+1];
	FILE	*fromFp,*toFp;
	int	isInserted = 0;
	
	if ((fileName == NULL) || (recStr == NULL))
		return(errCodeParameter);
		
        // ���ļ�
        if (UnionFileSize(fileName) <= 0)
        {
                return UnionWriteRecStrIntoFile(fileName, recStr, lenOfRecStr);
        }
          
	// ��Դ�ļ�д��һ����ʱ�ļ�
	if ((fromFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionInsertRecStrToFileInOrderByFld:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
        sprintf(tmpFileName,"%s.tmp",fileName);
	if ((toFp = fopen(tmpFileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionInsertRecStrToFileInOrderByFld:: fopen [%s]\n",tmpFileName);
		fclose(fromFp);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fromFp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fromFp,tmpBuf,sizeof(tmpBuf))) < 0)
			continue;
		if (ret == 0)
			continue;
        	if (isInserted)	// �Ѳ���
                {
                        fprintf(toFp,"%s\n",tmpBuf);
                        continue;
                }
		if ((ret = UnionCompare2RecStr(recStr,lenOfRecStr,tmpBuf,ret,fldName)) >= 0)	// Ҫ����ļ�¼���ڵ�ǰ��¼
		{
			if (!isAscOrder)	// ����
				goto insertNow;
			fprintf(toFp,"%s\n",tmpBuf);
		}
		else if (ret == -1)	// Ҫ����ļ�¼С�ڵ�ǰ��¼
		{
			if (isAscOrder)	// ����
				goto insertNow;
			fprintf(toFp,"%s\n",tmpBuf);
		}
		else
		{
			UnionSystemErrLog("in UnionInsertRecStrToFileInOrderByFld:: UnionCompare2RecStr [%s] [%s] on fldName [%s]\n",recStr,tmpBuf,fldName);
			fclose(fromFp);
			fclose(toFp);
			UnionDeleteRecFile(tmpFileName);
			return(ret);
		}
		continue;
insertNow:
		fprintf(toFp,"%s\n",recStr);
		fprintf(toFp,"%s\n",tmpBuf);
		isInserted = 1;
		continue;
	}
	// û�в����¼
        if (!isInserted)
                fprintf(toFp, "%s\n", recStr);
	fflush(toFp);
	fclose(toFp);
	fclose(fromFp);
	if ((ret = UnionRenameRecFile(tmpFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionInsertRecStrToFileInOrderByFld:: UnionRenameRecFile [%s] to [%s]\n",tmpFileName,fileName);
		return(ret);
	}
	return(ret);	
}

//---------------------------------------------------------------------------
/*
����	��һ����¼�ļ���ɾ��һ����¼��
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		ɾ���ļ�¼��
	<0		�������
*/
int UnionDeleteRecStrFromFile(char *fileName,char *recStr,int lenOfRecStr)
{
	return(UnionDeleteRecStrFromFileByFld(fileName,recStr,lenOfRecStr,""));
}

// �ж��Ƿ���ڼ�¼�ļ�
int UnionExistsFile(char *fileName)
{
#ifndef _WIN32
	if (access(fileName,F_OK) == 0)
#else
        if (FileExists(fileName))
#endif
		return(1);
	else
		return(0);
}

// ɾ���ļ�
int UnionDeleteFile(char *fileName)
{
	return(UnionDeleteRecFile(fileName));
}

// ɾ���ļ�
int UnionDeleteRecFile(char *fileName)
{
	// ɾ����¼�ļ�
	if (unlink(fileName) < 0)
	{
		UnionSystemErrLog("in UnionDeleteRecFile:: unlink %s fail!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	return(0);
}

// ����һ��Ŀ¼
int UnionCreateDir(char *dir)
{
	char	tmpBuf[512];
	
	if (dir == NULL)
	{
		UnionUserErrLog("in UnionCreateDir:: dir is null!\n");
		return(errCodeParameter);
	}
	if (strlen(dir) == 0)
		return(0);
#ifdef _WIN32
        if (CreateDir(dir))
        	return(0);
        else
        	return(errCodeUseOSErrCode);
#else
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"mkdir %s",dir);
#ifndef _LINUX_
	return(0-abs(system(tmpBuf)));
#else
	system(tmpBuf);
	return(0);
#endif
#endif
}

// ɾ��һ��Ŀ¼
int UnionRemoveDir(char *dir)
{
	char	tmpBuf[512];
	
	if (dir == NULL)
	{
		UnionUserErrLog("in UnionRemoveDir:: dir is null!\n");
		return(errCodeParameter);
	}
#ifdef _WIN32
        if (RemoveDir(dir))
        {
        	UnionLog("in UnionRemoveDir:: RemoveDir [%s] success!\n",dir);
        	return(0);
        }
        else
        {
        	UnionUserErrLog("in UnionRemoveDir:: RemoveDir [%s] success!\n",dir);
        	return(errCodeUseOSErrCode);
        }
#else
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"rm -r %s",dir);
#ifndef _LINUX_
	return(0-abs(system(tmpBuf)));
#else
	system(tmpBuf);
	return(0);
#endif
#endif
}

// ���һ��Ŀ¼�Ƿ����
// 1���ڣ�0������
int UnionExistsDir(char *dir)
{
	if (dir == NULL)
	{
		UnionUserErrLog("in UnionExistsDir:: dir is null!\n");
		return(0);
	}
#ifndef _WIN32
	if (access(dir,F_OK) == 0)
#else
        if (DirectoryExists(dir))
#endif
		return(1);
	else
		return(0);
}

/*
  ����: �ж�һ���ļ��Ƿ���ڣ�������������
  ����: fileName - �ļ���; dirName - �ļ�·��
  ���: ��
  ����: =0 - �ɹ�
        <0 - ʧ��
*/
int
UnionExistAndCreateFile(char *fileName, char *dirName)
{
        char   fullName[512+1];
        char   cmd[300+1];

        memset(fullName, 0, sizeof fullName);
        memset(cmd, 0, sizeof cmd);
        if (fileName == NULL || dirName == NULL)
        {
        	return(errCodeParameter);
        }

	UnionFormFullFileName(dirName, fileName, fullName);
        if ( !UnionExistsFile(fullName) ) /* �ļ������� */
        {
                /* ����Ŀ¼ */
                UnionExistAndCreateDir(dirName);
                strcpy(cmd,">");
                strcat(cmd,fullName);
                strcat(cmd," 1>/dev/null 2>&1");
		return(0-abs(system(cmd)));
        }
        return 0;
}

/*
  ����: �ж�һ��Ŀ¼�Ƿ���ڣ�������������,
        ���·���е�ĳ��Ŀ¼������һ����
  ����: dir - Ŀ¼·��
  ���: ��
  ����: =0 - �ɹ�
        <0 - ʧ��
*/
int
UnionExistAndCreateDir(char *dir)
{
        char cmd[300+1];
        char tmpdir[256+1];

        memset(cmd, 0, sizeof cmd);
        memset(tmpdir, 0, sizeof tmpdir);

        if ( (strcmp(dir,"/") !=0 && access(dir, F_OK)!=0) )
        {
#ifndef _WIN32
                strcpy(tmpdir, dirname(dir));
#else
                UnionGetDirExpFileName(dir, tmpdir);
#endif
                UnionExistAndCreateDir(tmpdir);
        }
        else
                return 0;
        strcpy(cmd, "mkdir ");
        strcat(cmd, dir);
	return(0-abs(system(cmd)));
}

/*
  ����: �ж�һ��Ŀ¼�Ƿ���ڣ�������������,
        ���·���е�ĳ��Ŀ¼������һ����
  ����: fullFileName - �ļ�ȫ��(��·��)
  ���: ��
  ����: =0 - �ɹ�
        <0 - ʧ��
*/
int
UnionExistAndCreateDirFromFullFileName(char *fullFileName)
{
        char dir[256+1];

        memset(dir, 0, sizeof dir);
        UnionGetDirExpFileName(fullFileName,dir);
	//strcpy(dir, dirname(fullFileName));
	UnionExistAndCreateDir(dir);
        return 0;
}



//---------------------------------------------------------------------------
/*
����	��һ����¼�ļ���ɾ��һ����¼��
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
	fldName		Ҫɾ������
�������
	��
����ֵ
	>=0		ɾ���ļ�¼��
	<0		�������
*/
int UnionDeleteRecStrFromFileByFld(char *fileName,char *recStr,int lenOfRecStr,char *fldName)
{
	int	ret;
	char	tmpBuf[512+1];
	char	tmpFileName[512+1];
	FILE	*fromFp,*toFp;

	if ((fileName == NULL) || (recStr == NULL))
		return(errCodeParameter);

	// ��Դ�ļ�д��һ����ʱ�ļ�
	if ((fromFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteRecStrFromFileByFld:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	sprintf(tmpFileName,"%s.tmp",fileName);
	if ((toFp = fopen(tmpFileName,"w")) == NULL)
	{
		UnionSystemErrLog("in UnionDeleteRecStrFromFileByFld:: fopen [%s]\n",tmpFileName);
		fclose(fromFp);
		return(errCodeUseOSErrCode);
	}
	while (!feof(fromFp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneLineFromTxtStr(fromFp,tmpBuf,sizeof(tmpBuf))) < 0)
			continue;
		if (ret == 0)
			continue;
		if ((ret = UnionCompare2RecStr(recStr,lenOfRecStr,tmpBuf,ret,fldName)) != 0)	// �뵱ǰֵ����
			fprintf(toFp,"%s\n",tmpBuf);
	}
	fflush(toFp);
	fclose(toFp);
	fclose(fromFp);
	
	if ((ret = UnionRenameRecFile(tmpFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteRecStrFromFileByFld:: UnionRenameRecFile [%s] to [%s]\n",tmpFileName,fileName);
		return(ret);
	}
	return(ret);	
}

//---------------------------------------------------------------------------
/*
����	�Ƚ�������¼�����Ƿ�һ��
�������
	recStr1		��¼��1
	lenOfRecStr1	��¼���ĳ���
	recStr2		��¼��2
	lenOfRecStr2	��¼���ĳ���
	fldName		Ҫ�Ƚϵ�����
�������
	��
����ֵ
	=0		��ͬ
	-1		��һ�����ȵڶ�����С
	=1		��һ�����ȵ���������
	<0		������루��-1)
*/
int UnionCompare2RecStr(char *recStr1,int lenOfRecStr1,char *recStr2,int lenOfRecStr2,char *fldName)
{
	int	ret;
	char	fld1[512+1],fld2[512+1];
	char	*fld1Str,*fld2Str;

	if ((recStr1 == NULL) || (recStr2 == NULL))
		return(errCodeParameter);

	if ((fldName != NULL) && (strlen(fldName) != 0))
	{
		memset(fld1,0,sizeof(fld1));
		if ((ret = UnionReadRecFldFromRecStr(recStr1,lenOfRecStr1,fldName,fld1,sizeof(fld1))) < 0)
		{
			UnionUserErrLog("in UnionCompare2RecStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",fldName,recStr1);
			return(ret);
		}
		fld1Str = fld1;
		memset(fld2,0,sizeof(fld2));
		if ((ret = UnionReadRecFldFromRecStr(recStr2,lenOfRecStr2,fldName,fld2,sizeof(fld2))) < 0)
		{
			UnionUserErrLog("in UnionCompare2RecStr:: UnionReadRecFldFromRecStr [%s] from [%s]\n",fldName,recStr2);
			return(ret);
		}
		fld2Str = fld2;
	}
	else
	{
		fld1Str = recStr1;
		fld2Str = recStr2;
	}
	if ((ret = strcmp(fld1Str,fld2Str)) < 0)
		return(-1);
	return(ret);
}

//---------------------------------------------------------------------------------
//#pragma package(smart_init)

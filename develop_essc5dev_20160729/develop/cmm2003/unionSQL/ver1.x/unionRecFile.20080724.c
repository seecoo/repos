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
// 2010-12-13，王纯军增加
/*
功能	以第一个文件为基准，比较两个文件，并输出不一致的行
输入参数
	firstFileDir	第一个文件的目录
	firstFileName	第一个文件名称
	secondFileName	第二个文件名称
	unsameFileFp	不一致的内容
输出参数
	无
返回值
	>=0		不一致的行数
	<0		出错代码
*/
int UnionCompareFile_1stFileWithDir(char *firstFileDir,char *firstFileName,char *secondFileName,FILE *unsameFileFp)
{
	char	full1stFileName[512+1];
	
	sprintf(full1stFileName,"%s/%s",firstFileDir,firstFileName);
	return(UnionCompareFile(full1stFileName,secondFileName,unsameFileFp));
}


//---------------------------------------------------------------------------
// 2010-12-13，王纯军增加
/*
功能	以第一个文件为基准，比较两个文件，并输出不一致的行
输入参数
	firstFileName	第一个文件名称
	secondFileDir	第二个文件的目录
	secondFileName	第二个文件名称
	unsameFileFp	不一致的内容
输出参数
	无
返回值
	>=0		不一致的行数
	<0		出错代码
*/
int UnionCompareFile_2ndFileWithDir(char *firstFileName,char *secondFileDir,char *secondFileName,FILE *unsameFileFp)
{
	char	full2ndFileName[512+1];
	
	sprintf(full2ndFileName,"%s/%s",secondFileDir,secondFileName);
	return(UnionCompareFile(firstFileName,full2ndFileName,unsameFileFp));
}

//---------------------------------------------------------------------------
// 2010-12-13，王纯军增加
/*
功能	以第一个文件为基准，比较两个文件，并输出不一致的行
输入参数
	firstFileDir	第一个文件的目录
	firstFileName	第一个文件名称
	secondFileDir	第二个文件的目录
	secondFileName	第二个文件名称
	unsameFileFp	不一致的内容
输出参数
	无
返回值
	>=0		不一致的行数
	<0		出错代码
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
// 2010-12-13，王纯军增加
/*
功能	以第一个文件为基准，比较两个文件，并输出不一致的行
输入参数
	firstFileName	第一个文件名称
	secondFileName	第二个文件名称
	unsameFileFp	不一致的内容
输出参数
	无
返回值
	>=0		不一致的行数
	<0		出错代码
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
		// 先从第一个文件中读取一行
		memset(tmpBuf1,0,sizeof(tmpBuf1));
		if ((ret = UnionReadOneDataLineFromTxtFile(firstFp,tmpBuf1,sizeof(tmpBuf1))) < 0)
			continue;
		lineIndexOfFirstFile++;
		//UnionRealNullLog("firstFile::[%04d][%s]\n",lineIndexOfFirstFile,tmpBuf1);
		if (ret == 0)
			continue;
		// 再从第二个文件中读取一行
		if (!isLastLineOK)
		{
			// 前一行不一致
			fseek(secondFp,0,SEEK_SET);	// 先置为从第一行读
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
			if (lineIndexOfSecondFile <= okLineIndexOfSecondFile)	// 第二个文件的该行已在第一个文件中存在了
				continue;
			// 移到第二个文件已经比较成功的行之后再比较
			if (strcmp(tmpBuf1,tmpBuf2) == 0)	// 两个文件的该行一致
			{
				okLineIndexOfSecondFile = lineIndexOfSecondFile;	// 置第二个文件的OK行为当前行
				isLastLineOK = 1;
				break;
			}
		}
		if (isLastLineOK)	// 两行一样
			continue;
		unsameLines++;
		//fprintf(outFp,"不一致::文件[%s]的第[%04d]行[%s]!\n",firstFileName,lineIndexOfFirstFile,tmpBuf1);
		fprintf(outFp,"不一致::第[%04d]行[%s]!\n",lineIndexOfFirstFile,tmpBuf1);
	}
	if (unsameLines == 0)
		fprintf(outFp,"完全一致::一致行数::[%04d]!\n",lineIndexOfFirstFile);
	else
		fprintf(outFp,"不一致::不一致行数::[%04d],一致行数::[%04d],总行数[%04d]\n",unsameLines,lineIndexOfFirstFile-unsameLines,lineIndexOfFirstFile);
	return(unsameLines);
}

//---------------------------------------------------------------------------
/*
功能	创建一个文件
输入参数
	oriFileName	文件名称
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
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
功能	拷贝一个文件
输入参数
	oriFileName	源文件名称
	desDir		目标目录
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
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
功能	创建目录
输入参数
	fullFileName	文件全名，包括了路径
输出参数
	无
返回值
	>=0		创建的目录数
	<0		出错代码
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
	// 折分出各路径
	if ((pathNum = UnionSeprateVarStrIntoVarGrp(fullFileName,len=strlen(fullFileName),seprator,pathGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionCreateDirFromFullFileName:: UnionSeprateVarStrIntoVarGrp [%s]!\n",fullFileName);
		return(pathNum);
	}
	memset(tmpDir,0,sizeof(tmpDir));
	for (index = 0; index < pathNum - 1; index++)	// pathNum - 1,是假设最后一个是文件名称
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
		if (UnionExistsDir(tmpDir))	// 目录已存在
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
功能	备份一个文件
输入参数
	oriFileName	源文件名称
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
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
	// 读取裸文件名称
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
        功能：获取路径名
        输入：FullFileName：文件名(包括绝对路径)
        输出：Dir(文件所在路径名)
        返回：0
        例如：文件 C:\aa\bb\c.txt, 路径名C:\aa\bb
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
功能	根据路径名和文件名拼装出文件全名
输入参数
	dirName	  路径名
	fileName  文件名
输出参数
	fullFileName  文件全名
返回值
	>=0		文件全名长度
	<0		出错代码
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
功能	将两个目录名合并成一个目录名
输入参数
	firstDir	第一个目录
	secondDir	第二个目录
输出参数
	newDirName  	新目录名
返回值
	>=0		目录长度
	<0		出错代码
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
功能	将第一个目录追到加到第二个目录上
输入参数
	firstDir	第一个目录
	secondDir	第二个目录
输出参数
	secondDir  	第二个目录
返回值
	>=0		目录长度
	<0		出错代码
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
功能	从暂时目录恢复文件
输入参数
	oriFileName	源文件名称
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
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
功能	备份一个文件到暂时目录
输入参数
	oriFileName	源文件名称
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
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
功能	拷贝一个文件
输入参数
	oriFileName	源文件名称
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
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
功能	将第一个文件追加到第二个文件中
输入参数
	oriFileName	源文件名称
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
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
功能	重命名一个文件
输入参数
	oriFileName	源文件名称
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
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
功能	重命名一个记录文件
输入参数
	oriFileName	源文件名称
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		记录数
	<0		出错代码
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
功能：  删除目录中的文件
参数：
        dir[IN]         需要被删除的目录名
return:
        -1              删除失败
        0               删除成功
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

        // 保存当前工作目录
        memset(curWorkDir, 0, sizeof(curWorkDir));
        getcwd(curWorkDir, sizeof(curWorkDir));

        // 获取需要删除的目录路径
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
// 获取一个文件的大小
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
功能	从一个记录文件中读取一个记录串
输入参数
	fileName	记录文件名
	sizeOfRecStr	接收记录串的缓冲的大小
输出参数
	recStr		读取的记录串
返回值
	>=0		记录串的长度
	<0		出错代码
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
功能	将一个记录串写入到记录文件
输入参数
	fileName	记录文件名
	recStr		记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
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
功能	将一个二进制记录串追加到记录文件
输入参数
	fileName	记录文件名
	recStr		记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
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
功能	将一个记录串追加到记录文件
输入参数
	fileName	记录文件名
	recStr		记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
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
功能	将一个串追加到文件
输入参数
	fileName	记录文件名
	fmt		串
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
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
功能	将一个串写入到文件
输入参数
	fileName	记录文件名
	fmt		串
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
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
功能	将一个记录串追加到记录文件，使用文件指针
输入参数
	hdl		文件指针
	recStr		记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
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
功能	为写文件打开记录文件句柄
输入参数
	fileName	文件名称
输出参数
	无
返回值
	文件句柄
	NULL		出错
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
功能	为写文件打开记录文件句柄
输入参数
	fileName	文件名称
输出参数
	无
返回值
	文件句柄
	NULL		出错
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
功能	打开记录文件句柄
输入参数
	fileName	文件名称
输出参数
	无
返回值
	文件句柄
	NULL		出错
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
功能	关柄记录文件句柄
输入参数
	hdl	文件句柄
输出参数
	无
返回值
	无
*/
void UnionCloseRecFileHDL(TUnionRecFileHDL hdl)
{
	if (hdl != NULL)
		fclose(hdl);
}

/* 
功能	关柄记录文件句柄
输入参数
	hdl	文件句柄
输出参数
	无
返回值
	无
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
功能	关柄记录文件句柄
输入参数
	hdl	文件句柄
输出参数
	无
返回值
	无
*/
void UnionCloseRecFileHDLOfFileNameWithoutLock(TUnionRecFileHDL hdl,char *fileName)
{
	if (hdl != NULL)
		fclose(hdl);
}

/* 
功能	获得文件中的记录总数量
输入参数
	fileName	文件名称
输出参数
	无
返回值
	>=0		文件中的记录总数量
	<0		出错代码
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
功能	查找指定值在特定数组中的位置
输入参数
	caValue	        指定值
	fldValue	特定
	maxNum		数组的最大数量
输出参数
	无
返回值
	>=0		读出的位置
	<0		不存在或出错代码
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
功能	获得文件中所有记录的指定的域
输入参数
	fileName	文件名称
	fldName		域名
	maxNum		读出的最大数量
输出参数
	无
返回值
	>=0		读出的总数量
	<0		出错代码
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
功能	从文件中读取下一条记录
输入参数
	hdl		文件指针
	sizeOfRecStr	接收记录的缓冲的大小
输出参数
	recStr		读取的记录
返回值
	>=0		读取的记录大小,=0,表示文件中已无后续记录
	<0		出错代码
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
功能	从文件中读取指定索引的记录
输入参数
	hdl		文件指针
	index		记录索引号
	sizeOfRecStr	接收记录的缓冲的大小
输出参数
	recStr		读取的记录
返回值
	>=0		读取的记录大小,=0,表示文件中已无记录
	<0		出错代码
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
功能	从文件中读取指定索引的记录
输入参数
	fileName	文件名称
	index		记录索引号
	sizeOfRecStr	接收记录的缓冲的大小
输出参数
	recStr		读取的记录
返回值
	>=0		读取的记录大小,=0,表示文件中已无后续记录
	<0		出错代码
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
功能    按照升序对一个文件的内容进行排序
输入参数
	fileName	记录文件名
输出参数
	无
返回值
	>=0		排序成功
	<0		出错代码
*/
int UnionSortFileContentInAscOrder(char *fileName)
{
        int	ret;
	char	tmpBuf[512+1];
	char	tmpFileName[512+1];
	FILE	*fromFp,*toFp;

	if (fileName == NULL)
		return(errCodeParameter);

        // 空文件，无内容
        if (UnionFileSize(fileName) <= 0)
                return 0;

        // 从源文件写到一个暂时文件
	if ((fromFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionSortFileContentInAscOrder:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

        // 创建一个临时文件，用于保存源文件的有序内容
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

	// 从暂时文件写回源文件
	if ((ret = UnionRenameRecFile(tmpFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionSortFileContentInAscOrder:: UnionRenameRecFile [%s] to [%s]\n",tmpFileName,fileName);
		return(ret);
	}
	return(ret);
}
//---------------------------------------------------------------------------
/*
功能    按照降序对一个文件的内容进行排序
输入参数
	fileName	记录文件名
输出参数
	无
返回值
	>=0		排序成功
	<0		出错代码
*/
int UnionSortFileContentInDesOrder(char *fileName)
{
        int	ret;
	char	tmpBuf[512+1];
	char	tmpFileName[512+1];
	FILE	*fromFp,*toFp;

	if (fileName == NULL)
		return(errCodeParameter);

        // 空文件，无内容
        if (UnionFileSize(fileName) <= 0)
                return 0;

        // 从源文件写到一个暂时文件
	if ((fromFp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionSortFileContentInDesOrder:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

        // 创建一个临时文件，用于保存源文件的有序内容
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

	// 从暂时文件写回源文件
	if ((ret = UnionRenameRecFile(tmpFileName,fileName)) < 0)
	{
		UnionUserErrLog("in UnionSortFileContentInDesOrder:: UnionRenameRecFile [%s] to [%s]\n",tmpFileName,fileName);
		return(ret);
	}
	return(ret);	
}
//---------------------------------------------------------------------------

/*
功能	按照降序的方式在一个记录文件中插入一个记录串
条件    记录文件本身就是一个按照降序方式排列的文件
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		插入成功
	<0		出错代码
*/
int UnionInsertRecStrToFileInDesOrder(char *fileName,char *recStr,int lenOfRecStr)
{
	return(UnionInsertRecStrToFileInOrderByFld(fileName,recStr,lenOfRecStr,"",0));
}
//---------------------------------------------------------------------------
/*
功能	按照升序的方式在一个记录文件中插入一个记录串
条件    记录文件本身就是一个按照升序方式排列的文件
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		插入成功
	<0		出错代码
*/
int UnionInsertRecStrToFileInAscOrder(char *fileName,char *recStr,int lenOfRecStr)
{
	return(UnionInsertRecStrToFileInOrderByFld(fileName,recStr,lenOfRecStr,"",1));
}

//---------------------------------------------------------------------------
/*
功能	按照升序的方式在一个记录文件中插入一个记录串
条件    记录文件本身就是一个按照升序方式排列的文件
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
	fldName		根据该域来排序
	isAscOrder	排序方式，１升序，０除序
输出参数
	无
返回值
	>=0		插入成功
	<0		出错代码
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
		
        // 空文件
        if (UnionFileSize(fileName) <= 0)
        {
                return UnionWriteRecStrIntoFile(fileName, recStr, lenOfRecStr);
        }
          
	// 从源文件写到一个暂时文件
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
        	if (isInserted)	// 已插入
                {
                        fprintf(toFp,"%s\n",tmpBuf);
                        continue;
                }
		if ((ret = UnionCompare2RecStr(recStr,lenOfRecStr,tmpBuf,ret,fldName)) >= 0)	// 要插入的记录大于当前记录
		{
			if (!isAscOrder)	// 降序
				goto insertNow;
			fprintf(toFp,"%s\n",tmpBuf);
		}
		else if (ret == -1)	// 要插入的记录小于当前记录
		{
			if (isAscOrder)	// 升序
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
	// 没有插入记录
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
功能	从一个记录文件中删除一个记录串
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		删除的记录数
	<0		出错代码
*/
int UnionDeleteRecStrFromFile(char *fileName,char *recStr,int lenOfRecStr)
{
	return(UnionDeleteRecStrFromFileByFld(fileName,recStr,lenOfRecStr,""));
}

// 判断是否存在记录文件
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

// 删除文件
int UnionDeleteFile(char *fileName)
{
	return(UnionDeleteRecFile(fileName));
}

// 删除文件
int UnionDeleteRecFile(char *fileName)
{
	// 删除记录文件
	if (unlink(fileName) < 0)
	{
		UnionSystemErrLog("in UnionDeleteRecFile:: unlink %s fail!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	return(0);
}

// 创建一个目录
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

// 删除一个目录
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

// 检查一个目录是否存在
// 1存在，0不存在
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
  描述: 判断一个文件是否存在，不存在则建立它
  输入: fileName - 文件名; dirName - 文件路径
  输出: 无
  返回: =0 - 成功
        <0 - 失败
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
        if ( !UnionExistsFile(fullName) ) /* 文件不存在 */
        {
                /* 建立目录 */
                UnionExistAndCreateDir(dirName);
                strcpy(cmd,">");
                strcat(cmd,fullName);
                strcat(cmd," 1>/dev/null 2>&1");
		return(0-abs(system(cmd)));
        }
        return 0;
}

/*
  描述: 判断一个目录是否存在，不存在则建立它,
        如果路径中的某个目录不存在一起建立
  输入: dir - 目录路径
  输出: 无
  返回: =0 - 成功
        <0 - 失败
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
  描述: 判断一个目录是否存在，不存在则建立它,
        如果路径中的某个目录不存在一起建立
  输入: fullFileName - 文件全名(带路径)
  输出: 无
  返回: =0 - 成功
        <0 - 失败
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
功能	从一个记录文件中删除一个记录串
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
	fldName		要删除的域
输出参数
	无
返回值
	>=0		删除的记录数
	<0		出错代码
*/
int UnionDeleteRecStrFromFileByFld(char *fileName,char *recStr,int lenOfRecStr,char *fldName)
{
	int	ret;
	char	tmpBuf[512+1];
	char	tmpFileName[512+1];
	FILE	*fromFp,*toFp;

	if ((fileName == NULL) || (recStr == NULL))
		return(errCodeParameter);

	// 从源文件写到一个暂时文件
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
		if ((ret = UnionCompare2RecStr(recStr,lenOfRecStr,tmpBuf,ret,fldName)) != 0)	// 与当前值不符
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
功能	比较两个记录串的是否一致
输入参数
	recStr1		记录串1
	lenOfRecStr1	记录串的长度
	recStr2		记录串2
	lenOfRecStr2	记录串的长度
	fldName		要比较的域名
输出参数
	无
返回值
	=0		相同
	-1		第一个串比第二个串小
	=1		第一个串比第三个串大
	<0		出错代码（非-1)
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

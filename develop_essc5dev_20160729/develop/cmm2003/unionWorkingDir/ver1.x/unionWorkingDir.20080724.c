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
// ��ȡ��������
TApplication *UnionGetApplication();
#endif

// ���һ����ʱ�ļ���
void UnionGetTempFileNameForThisProc(char *fileName)
{
	sprintf(fileName,"%s/%d",getenv("UNIONTEMP"),getpid());
}

/*
����	���������Ŀ¼
	����Unix����Ŀ¼Ϊ$UNIONMAINDIR
	����Win����Ŀ¼Ϊ".\\"
�������
	��
�������
	workingDir	��ȡ��������Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	�����ʱ����Ŀ¼
	Ŀ¼Ϊ"mainWorkingDir/tmp"
�������
	��
�������
	workingDir	��ȡ����ʱ����Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	��÷���������ݹ���Ŀ¼
	Ŀ¼Ϊ"mainWorkingDir/simuData"
�������
	��
�������
	workingDir	��ȡ�Ĺ���Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	���tcpipSvr�����ļ�Ŀ¼
	Ŀ¼Ϊ"mainWorkingDir/server"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	��ÿ��ƶ���Ŀ¼
	Ŀ¼Ϊ"mainWorkingDir/control"
�������
	��
�������
	workingDir	��ȡ�Ŀ��ƶ���Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	�����ʱ�ļ�����
	���ļ�Ϊ"mainWorkingDir/tmp/x.tmp"
�������
        index           ��ʱ�ļ����
�������
	UnionGetIconDir	��ȡ���ļ�����
����ֵ
	>=0		��ȷ
	<0		�������
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
// ����һ���µ���ʱ�ļ�����
char *UnionGenerateTmpFileName()
{
        memset(pgMngSvrTmpFileName,0,sizeof(pgMngSvrTmpFileName));
        UnionGetTmpFileName(gunionTmpFileSSN,pgMngSvrTmpFileName);
        gunionTmpFileSSN++;
        return(pgMngSvrTmpFileName);
}
	
// ��õ�ǰ����ʱ�ļ�����
char *UnionGetCurrentTmpFileName()
{
	return(pgMngSvrTmpFileName);
}

// ����һ����ʱ�ļ����������ļ�����
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

// ��ȡ��ǰ��ʱ�ļ�������
char *UnionGetCurrentMngSvrTempFileName()
{
	return(pgMngSvrTmpFileName);
}

// ɾ����ʱ�ļ�
int UnionDeleteMngSvrTempFile()
{
        DeleteFile(pgMngSvrTmpFileName);
	return(0);
}

// ����������ʱ�ļ���
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
����	��ÿͻ��˱���Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/tblQuery"
�������
	��
�������
	workingDir	��ȡ�Ŀͻ��˱��幤��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	��ñ��¼�ӿڶ���Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/tblQuery"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	��ò˵�����Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/menu"
�������
	��
�������
	workingDir	��ȡ�Ĳ˵�����Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	���ͼ��Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/icon"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	���ͼ���ļ�����
	��Ŀ¼Ϊ"mainWorkingDir/icon"
�������
	��
�������
	UnionGetIconDir	��ȡ���ļ�����
����ֵ
	>=0		��ȷ
	<0		�������
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
����	�����ֵ����Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/enumValueDef"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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
����	�����ֵ����Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/enumValueDef"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
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

//	Author: ChenJiaMei
//	Date: 2008-8-7

#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#include <vcl.h>
#include <Filectrl.hpp>

#include <dir.h>
#include <io.h>
#include <sys\stat.h>

#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionErrCode.h"
#include "unionComplexDBObjectFileName.h"
#include "unionComplexDBCommon.h"
#include "unionComplexDBRecordChild.h"

/*
���ܣ����һ������ʵ�����ⲿ�����ļ��Ƿ����
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	idOfForeignObject���ⲿ����ID
���������
	��
����ֵ��
	1������
	0��������
	<0��ʧ�ܣ�������
*/
int UnionExistsObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject)
{
	char	caFileName[512];

	if (idOfLocalObject == NULL || localPrimaryKey == NULL || idOfForeignObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectRecChildFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ������ʵ�����ⲿ�����ļ��Ƿ����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecChildListFileName(idOfLocalObject,localPrimaryKey,idOfForeignObject,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
���ܣ���һ������ʵ�����ⲿ�����ļ�������һ����¼
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	idOfForeignObject���ⲿ����ID
	foreignPrimaryKey���ⲿ����ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionAddObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject, char *foreignPrimaryKey)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecLen=0;

	if (idOfLocalObject == NULL || localPrimaryKey == NULL || idOfForeignObject == NULL || foreignPrimaryKey == NULL)
	{
		UnionUserErrLog("in UnionAddObjectRecChildFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ��ö���ʵ�����ⲿ�����ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecChildListFileName(idOfLocalObject,localPrimaryKey,idOfForeignObject,caFileName);
	
	memset(caRecord,0,sizeof(caRecord));
	strcpy(caRecord,"foreignRecKey=");
	strcat(caRecord,foreignPrimaryKey);
	iRecLen=strlen(caRecord);

        UnionProgramerLog("in UnionAddObjectRecChildFile, try to add record: [%s] to file: [%s].\n", caRecord, caFileName);
        
	iRet=UnionAppendRecStrToFile(caFileName,caRecord,iRecLen);
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionAddObjectRecChildFile:: UnionAppendRecStrToFile for [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
	
	return(0);
}

/*
���ܣ���һ������ʵ�����ⲿ�����ļ���ɾ��һ����¼
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��
	idOfForeignObject���ⲿ����ID
	foreignPrimaryKey���ⲿ����ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳�����ⲿ�������ļ��ж���ļ�ֵ��˳��һ��
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionDeleteObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject, char *foreignPrimaryKey)
{
	char	caFileName[512],caRecord[1024];
	int	iRet,iRecNum=0;

	if (idOfLocalObject == NULL || localPrimaryKey == NULL || idOfForeignObject == NULL || foreignPrimaryKey == NULL)
	{
		UnionUserErrLog("in UnionDeleteObjectRecChildFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	// ��ö���ʵ�����ⲿ�����ļ�����
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecChildListFileName(idOfLocalObject,localPrimaryKey,idOfForeignObject,caFileName);

        // ������ʵ�����ⲿ�����ļ��Ƿ����
	if ((iRet = UnionExistsFile(caFileName)) == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectRecChildFile:: [%s] does not exists!\n",caFileName);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists));
	}

	iRecNum=UnionGetTotalRecNumInFile(caFileName);
	if (iRecNum < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectRecChildFile:: UnionGetTotalRecNumInFile fail! return=[%d]\n",iRecNum);
		return(iRecNum);
	}
	if (iRecNum == 0)
	{
		UnionUserErrLog("in UnionDeleteObjectRecChildFile:: no record [%d]!\n",iRecNum);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ForeignRecordNotExist));
	}
	
	memset(caRecord,0,sizeof(caRecord));
	strcpy(caRecord,"foreignRecKey=");
	strcat(caRecord,foreignPrimaryKey);

        // iRet = UnionDeleteRecStrFromFile(caFileName,caRecord,strlen(caRecord));
        iRet    = UnionDeleteRecStrFromFileByFld(caFileName,caRecord,strlen(caRecord), "foreignRecKey");
	if (iRet < 0)
	{
		UnionUserErrLog("in UnionDeleteObjectRecChildFile:: UnionDeleteRecStrFromFile for foreign key [%s] fail! return=[%d]\n",caRecord,iRet);
		return(iRet);
	}
        
        UnionProgramerLog("in UnionDeleteObjectRecChildFile:: [%s]'s record nums: [%d], after deleted [%s], the record nums: [%d].\n", caFileName, iRecNum, caRecord, iRet);
	
	if (iRecNum == iRet)
	{
		UnionUserErrLog("in UnionDeleteObjectRecChildFile:: [%s] does not exists!\n",foreignPrimaryKey);
		return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_ForeignRecordNotExist));
	}
	
	// �ⲿ�����ļ�����û�м�¼��ɾ�����ļ�
	if (iRet == 0)
	{
		iRet=UnionDeleteRecFile(caFileName);
		if (iRet < 0)
		{
			UnionUserErrLog("in UnionDeleteObjectRecChildFile:: UnionDeleteRecFile for [%s] fail! return=[%d]\n",caFileName,iRet);
			return(iRet);
		}
                else
                {
                        UnionProgramerLog("in UnionDeleteObjectRecChildFile:: delete file [%s].\n", caFileName);
                }
	}
	
	return(0);
}

#ifdef _WIN32
// ������ȡһ���ļ��Ĵ�С
// long fileSize(const char *fileName)
long fileSize(char *fileName)
{
        FILE    *stream;
        //stream  = fopen(fileName, "r");	Mary modify, 20081211
        stream=UnionOpenRecFileHDL(fileName);

        if(NULL == stream)
                return -1;

        long    curPos, size;

        curPos  = ftell(stream);

        fseek(stream, 0L, SEEK_END);
        size    = ftell(stream);

        fseek(stream, curPos, SEEK_SET);
        
        //fclose(stream);		Mary modify, 20081211
        UnionCloseRecFileHDLOfFileName(stream,fileName);

        return size;
}
#endif

#ifdef _WIN32
// �ж��ǲ���Ŀ¼
int isDir(const char *dir)
{
    char        curWorkDir[MAXDIR];
    memset(curWorkDir, 0, MAXDIR);
    getcwd(curWorkDir, MAXDIR);

    if(!chdir(dir))
    {
        chdir(curWorkDir);
        return 0;
    }
    else
        return -1;
}
#endif

#ifdef _WIN32
// ������ȡ�ļ����У��ļ����к���ĳ���ַ������ļ�����
int getMatchFileNums(char *path, char *partName)
{
        if(NULL == path || NULL == partName)
                return 0;

        if(isDir(path))
                return 0;

        char            curWorkDir[MAXDIR];
        memset(curWorkDir, 0, sizeof(char) * MAXDIR);
        getcwd(curWorkDir, MAXDIR);

        chdir(path);

        char            delDir[MAXDIR];
        memset(delDir, 0, sizeof(char) * MAXDIR);
        strcpy(delDir, path);
        strcat(delDir, "\\*");

        char            *pos;
        int             nums = 0;

        struct ffblk    fblk;
        int             done;

        done            = findfirst(delDir, &fblk, FA_RDONLY | FA_HIDDEN | FA_DIREC | FA_SYSTEM | FA_ARCH);
        while(!done)
        {
                if(16 != fblk.ff_attrib)
                {
                        chmod(fblk.ff_name, S_IREAD | S_IWRITE);
                        pos = strstr(fblk.ff_name, partName);
                        if((pos - fblk.ff_name) >= 0)
                        {
                                if(fileSize(fblk.ff_name) > 0)
                                {
                                        nums += 1;
                                }
                        }
                }
                done    = findnext(&fblk);
        }
        findclose(&fblk);
        chdir(curWorkDir);
        return nums;
}
#endif
/*
���ܣ����һ�������ʵ���Ƿ����������ʵ������
���������
	idOfLocalObject��������ID
	localPrimaryKey��������ʵ���Ĺؼ�����ֵ����ʽΪ"��ֵ��1��ֵ.��ֵ��2��ֵ.��ֵ��3��ֵ����ֵ��N��ֵ"��
		��ֵ�������˳���뱾�������ļ��ж���ļ�ֵ��˳��һ��	
���������
	��
����ֵ��
	1����
	0������
	<0��ʧ�ܣ�������
*/
int UnionExistForeignObjectRecordDef(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey)
{
	char	caDirName[512],caBuf[512],caTmpDir[512];
	int	iResult;
        int     nums;
	char	*ptr;

	if (idOfLocalObject == NULL || localPrimaryKey == NULL)
	{
		UnionUserErrLog("in UnionExistForeignObjectRecordDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// ���һ������ʵ���Ĵ洢Ŀ¼
	memset(caDirName,0,sizeof(caDirName));
	UnionGetObjectRecDirName(idOfLocalObject,localPrimaryKey,caDirName);
	
	// ����ʵ���Ĵ洢Ŀ¼���ⲿ�����ļ�������
#ifndef _WIN32
	memset(caTmpDir,0,sizeof(caTmpDir));
	if ((ptr = (char *)getenv("UNIONTEMP")) == NULL)
		strcpy(caTmpDir,".");
	else
		strcpy(caTmpDir,ptr);
		
	memset(caBuf,0,sizeof(caBuf));
	sprintf(caBuf,"ls -l %s/child* 1>%s/tmpFile 2>%s/tmpErr",caDirName,caTmpDir,caTmpDir);
	iResult=system(caBuf);
	memset(caBuf,0,sizeof(caBuf));
	sprintf(caBuf,"%s/tmpFile",caTmpDir);
	UnionDeleteRecFile(caBuf);
	memset(caBuf,0,sizeof(caBuf));
	sprintf(caBuf,"%s/tmpErr",caTmpDir);
	UnionDeleteRecFile(caBuf);
	if (iResult == 0)
		return(1);
	else
		return(0);
#else
        nums = getMatchFileNums(caDirName, "child");
        if(nums > 0)
                return 1;
        
	return(0);
#endif
}

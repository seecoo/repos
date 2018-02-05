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
功能：检查一个对象实例的外部引用文件是否存在
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	idOfForeignObject：外部对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject)
{
	char	caFileName[512];

	if (idOfLocalObject == NULL || localPrimaryKey == NULL || idOfForeignObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectRecChildFile:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	// 检查对象实例的外部引用文件是否存在
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecChildListFileName(idOfLocalObject,localPrimaryKey,idOfForeignObject,caFileName);
	
	return(UnionExistsFile(caFileName));
}

/*
功能：在一个对象实例的外部引用文件中增加一条记录
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	idOfForeignObject：外部对象ID
	foreignPrimaryKey：外部对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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
	
	// 获得对象实例的外部引用文件名称
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
功能：在一个对象实例的外部引用文件中删除一条记录
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	idOfForeignObject：外部对象ID
	foreignPrimaryKey：外部对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
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

	// 获得对象实例的外部引用文件名称
	memset(caFileName,0,sizeof(caFileName));
	UnionGetObjectRecChildListFileName(idOfLocalObject,localPrimaryKey,idOfForeignObject,caFileName);

        // 检查对象实例的外部引用文件是否存在
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
	
	// 外部引用文件中已没有记录，删除该文件
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
// 用来获取一个文件的大小
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
// 判断是不是目录
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
// 用来获取文件夹中，文件名中含有某个字符串的文件数量
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
功能：检查一个对象的实例是否被其它对象的实例引用
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致	
输出参数：
	无
返回值：
	1：是
	0：不是
	<0：失败，错误码
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
	
	// 获得一个对象实例的存储目录
	memset(caDirName,0,sizeof(caDirName));
	UnionGetObjectRecDirName(idOfLocalObject,localPrimaryKey,caDirName);
	
	// 对象实例的存储目录下外部引用文件的数量
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

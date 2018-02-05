//	hzh创建，DesKeyDB库备份程序 keyDBLocalBackuper.20060814.c
//	2009/09/23

#define _UnionTask_3_x_
#define _UnionDesKeyDB_2_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionModule.h"

#include "unionDesKeyDB.h"
#include "unionREC.h"

PUnionTaskInstance	ptaskInstance = NULL;
PUnionSharedMemoryModule	pgunionDesKeyDBMDL = NULL;
PUnionDesKeyDB			pgunionDesKeyDB = NULL;
PUnionDesKey			pgunionDesKey = NULL;	

long		gNumOfDBBak = 0;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionLocalDisconnectDesKeyDB();
	exit(0);	//return(exit(0));
}

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	return 0;
}

int UnionLocalIsDesKeyDBConnected()
{
	if ((!UnionIsSharedMemoryInited(conMDLNameOfUnionDesKeyDB)) || (pgunionDesKeyDB == NULL) || (pgunionDesKey == NULL))
		return(0);
	else
		return(1);
}

// Module Layer Functions
int UnionLocalConnectExistingDesKeyDB()
{
	
	int	ret;

	if (UnionLocalIsDesKeyDBConnected())
		return(0);

	if ((pgunionDesKeyDBMDL = UnionConnectExistingSharedMemoryModule(conMDLNameOfUnionDesKeyDB,
			sizeof(TUnionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionLocalConnectExistingDesKeyDB:: UnionConnectExistingSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionDesKeyDB = (PUnionDesKeyDB)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionDesKeyDBMDL)) == NULL)
	{
		UnionUserErrLog("in UnionLocalConnectExistingDesKeyDB:: PUnionDesKeyDB!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionDesKey = (PUnionDesKey)((unsigned char *)pgunionDesKeyDB + sizeof(*pgunionDesKeyDB))) == NULL)
	{
		UnionUserErrLog("in UnionLocalConnectExistingDesKeyDB:: PUnionDesKey!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionDesKeyDB->pkey = pgunionDesKey;
	
	return(0);
}

int UnionLocalDisconnectDesKeyDB()
{
	pgunionDesKey = NULL;
	pgunionDesKeyDB = NULL;
	return(UnionDisconnectShareModule(pgunionDesKeyDBMDL));
}

long UnionFsize(char *filename)
{
	long fsize = 0;
 	struct stat st;
 	if (access(filename,0))
 	  return -1;
	stat(filename,&st);
	fsize = st.st_size;
	return fsize;
}

// 检查密钥库文件最后修改的时间
int UnionGetDBBakTime(char *fileName,char *lastUpdatedTime)
{
	int	ret;
	FILE	*fp;
	TUnionDesKeyDB	keyDB;
	long fsize = 0;
	
	fsize = UnionFsize(fileName);
	
	if (fsize <= (int)(sizeof(TUnionDesKeyDB)))
	{
		// 不存在，或文件长度为小于库文件头长度
		return(0);
	}
	
	// 取文件大小
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionCheckDesKeyDB:: fsize[%d] fopen [%s]\n",fsize,fileName);
		fclose(fp);
		return(errCodeUseOSErrCode);
	}
	
	memset(&keyDB,0,sizeof(keyDB));	
	fread(&keyDB,sizeof(keyDB),1,fp);
	
	fclose(fp);
	strncpy(lastUpdatedTime,keyDB.lastUpdatedTime,14);
	
	return 1;
}

// 功能：取可以生成的备份文件。
// 不存在的文件，或在所有备份的文件中取最旧的备份文件
int UnionGetOldNameOfDesKeyDBOfBak(char *bakFileName)
{
	int i = 0;
	char tmpfilename[512];
	char lastUpdatedTime[15];
	char tmpUpdatedTime[15];
	int ret = 0;
	
	sprintf(bakFileName,"%s/keyFile/unionDesKeyDB.KDB.0.bak",getenv("UNIONETC"));
	ret = UnionGetDBBakTime(bakFileName,lastUpdatedTime);
	if (ret <= 0)
	{
			return 0;
	}
		
	for(i=1;i<gNumOfDBBak;i++)
	{
		sprintf(tmpfilename,"%s/keyFile/unionDesKeyDB.KDB.%d.bak",getenv("UNIONETC"),i);
		memset(tmpUpdatedTime,0,sizeof(tmpUpdatedTime));
		ret = UnionGetDBBakTime(tmpfilename,tmpUpdatedTime);
		if (ret <= 0)
		{
			strcpy(bakFileName,tmpfilename);
			return 0;
		}
		if (strcmp(tmpUpdatedTime,lastUpdatedTime) < 0)
		{
			strcpy(bakFileName,tmpfilename);
			strcpy(lastUpdatedTime,tmpUpdatedTime);
		}
	}
	
	return(0);
}

int UnionMirrorDesKeyDBLocalBackup()
{
	int	ret;
	
	if ((ret = UnionLocalConnectExistingDesKeyDB()) < 0)
	{
		UnionUserErrLog("in UnionLocalConnectExistingDesKeyDB:: UnionConnectExistingDesKeyDB!\n");
		return(ret);
	}

	return(UnionMirrorDesKeyDBLocalBackupToDisk(pgunionDesKeyDB,pgunionDesKey));
}

int UnionMirrorDesKeyDBLocalBackupToDisk(PUnionDesKeyDB pdesKeyDB,PUnionDesKey pkey)
{
	FILE			*fp;
	char			fileName[256];

	if (pdesKeyDB == NULL)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBLocalBackupToDisk:: NullPointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	// 取备份文件名
	UnionGetOldNameOfDesKeyDBOfBak(fileName);
	
	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionMirrorDesKeyDBLocalBackupToDisk:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	fwrite(pdesKeyDB,sizeof(*pdesKeyDB),1,fp);
	if (pdesKeyDB->num < 0)
	{
		UnionUserErrLog("in UnionMirrorDesKeyDBLocalBackupToDisk:: pdesKeyDB->num [%d] Error!\n",pdesKeyDB->num);
		fclose(fp);
		return(errCodeDesKeyDBMDL_KeyNum);
	}
	
	if ((pdesKeyDB->num == 0) || (pkey == NULL))
	{
		fclose(fp);
		return(0);
	}
	
	fwrite(pkey,sizeof(TUnionDesKey),pdesKeyDB->num,fp);	
	
	fclose(fp);

	return(0);
}


int main(int argc,char **argv)
{
	int	ret;
	int	i;
	long	etime = 0;
	long	stime = 0;	
	long  intervalTimeOfDBBak = 0;   
	
	if (UnionCreateProcess() > 0)
		return(0);
		
	UnionSetApplicationName(argv[0]);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionMirrorDesKeyDBLocalBackup()) < 0)
			printf("UnionMirrorDesKeyDBLocalBackup Error! ret = [%d]\n",ret);
	
	time(&stime);
	
	while(1){
		time(&etime);
		gNumOfDBBak = UnionReadIntTypeRECVar("numberOfDBBak");
		if (gNumOfDBBak <= 0)
			gNumOfDBBak = 10;
		
		intervalTimeOfDBBak = UnionReadIntTypeRECVar("intervalTimeOfDBBak");	
		if (intervalTimeOfDBBak <= 0)
			intervalTimeOfDBBak = 7200;
		
		if (etime - stime < intervalTimeOfDBBak)
		{
				sleep(5);
				continue;
    }
		if ((ret = UnionMirrorDesKeyDBLocalBackup()) < 0)
			printf("UnionMirrorDesKeyDBLocalBackup Error! ret = [%d]\n",ret);
		
		time(&stime);
	}
	
	return(UnionTaskActionBeforeExit());
}


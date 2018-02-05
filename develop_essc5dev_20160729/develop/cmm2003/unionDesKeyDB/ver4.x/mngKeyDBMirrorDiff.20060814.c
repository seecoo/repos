//	hzh创建，比较DesKeyDB与内存映象的差异 mngKeyDBMirrorDiff.20060814.c
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
int 							gDiffFlag = 0;

int UnionLocalDisconnectDesKeyDB()
{
	pgunionDesKey = NULL;
	pgunionDesKeyDB = NULL;
	return(UnionDisconnectShareModule(pgunionDesKeyDBMDL));
}

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

int UnionGetNameOfDesKeyDB(char *fileName)
{
	sprintf(fileName,"%s/keyFile/unionDesKeyDB.KDB",getenv("UNIONETC"));
	return(0);
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

// 比较内存中的key与DB库中的
int UnionKeyDiff(FILE *fp,PUnionDesKey pDesKey1,PUnionDesKey pDesKey2)
{
	if (strcmp(pDesKey1->fullName,pDesKey2->fullName) !=0 || 
		strcmp(pDesKey1->value,pDesKey2->value) !=0	||
		strcmp(pDesKey1->checkValue,pDesKey2->checkValue) !=0
		)
	{
	if (((fp == stdout) || (fp == stderr)) && (gDiffFlag % 7 == 0) && (gDiffFlag != 0))
	{
			if (UnionIsQuit(UnionPressAnyKey("Press enter to continue or exit/quit to exit")))
				return(-1);
	}
	if (gDiffFlag == 0)
		fprintf(fp,"                   diff DBMirror DBFile\n");
	fprintf(fp,"-------------------fullName|value|checkValue--------------------------\n");
	fprintf(fp,"%40s |%s |%s\n",pDesKey1->fullName,pDesKey1->value,pDesKey1->checkValue);
	fprintf(fp,"%40s |%s |%s\n",pDesKey2->fullName,pDesKey2->value,pDesKey2->checkValue);
	gDiffFlag++;
	}
	return 0;
}

int UnionDBMirrorDiff(char *diffOutFileName)
{
	int ret = 0;
	char fileName[512];
	FILE *fp = NULL;
	FILE *diffFp = NULL;
	long	fsize = 0,presize = 0;
	long	i = 0;
	long	num = 0;
	TUnionDesKeyDB	keyDB;
	TUnionDesKey tDesKey;
	
	// 取内存DB
	if ((ret = UnionLocalConnectExistingDesKeyDB()) < 0)
	{
		printf("in UnionDBMirrorDiff:: UnionConnectExistingDesKeyDB!\n");
		return(ret);
	}
	// 读密钥库文件
	UnionGetNameOfDesKeyDB(fileName);
	fsize = UnionFsize(fileName);
	if (fsize <= sizeof(TUnionDesKeyDB))
	{
		printf("in UnionDBMirrorDiff:: UnionFsize [%s],size[%ld]\n",fileName,fsize);
		return(errCodeUseOSErrCode);
	}
	
	// 取文件大小
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		printf("in UnionDBMirrorDiff:: fopen [%s]\n",fileName);
		fclose(fp);
		return(errCodeUseOSErrCode);
	}
	memset(&keyDB,0,sizeof(keyDB));	
	fread(&keyDB,sizeof(keyDB),1,fp);
	
	num = keyDB.num;
	if (pgunionDesKeyDB->num != num)
	{
		fclose(fp);
		printf("UnionDBMirrorDiff:: DBMirrorNum[%ld] != DBNum[%ld]\n",pgunionDesKeyDB->num,num);
		return -1;
	}
	
	presize = sizeof(TUnionDesKeyDB) + sizeof(TUnionDesKey) * num;
	if (fsize != presize)
	{
		printf("in UnionCheckDesKeyDB:: file[%s] is err! fsize[%ld],presize[%ld] \n",fileName,fsize,presize);
		return(errCodeUseOSErrCode);
	}
	
	if (diffOutFileName != NULL && strlen(diffOutFileName) != 0 )
	{
		sprintf(fileName,"%s.diff",diffOutFileName);
		printf("gen diff file [%s]\n",fileName);
		diffFp = fopen(fileName,"w");
	}
	
	if (diffFp == NULL)
		diffFp = stdout;
		
	for(i=0;i<num;i++)
	{
		memset(&tDesKey,0,sizeof(TUnionDesKey));
		fread(&tDesKey,sizeof(TUnionDesKey),1,fp);
		if (UnionKeyDiff(diffFp,pgunionDesKey+i,&tDesKey) < 0)
			break;
	}
	fclose(fp);
	if ( diffFp != stdout)
		fclose(diffFp);
	if (gDiffFlag == 0)
		printf("DBMirror and DBFile are identical!\n");

	return(0);
}

int main(int argc,char **argv)
{
	char 	diffFileName[512];
	
	UnionSetApplicationName(argv[0]);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

/*
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	*/
	
	memset(diffFileName,0,sizeof(diffFileName));
	if (argc > 1) {
		strcpy(diffFileName,argv[1]);
	}
	
	UnionDBMirrorDiff(diffFileName);
	
	return(UnionTaskActionBeforeExit());
}


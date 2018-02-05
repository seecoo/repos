//	hzh����, DesKeyDB��ָ����� mngKeyDBLocalReloader.20060814.c
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

long		gNumOfDBBak = 0;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);	//return(exit(0));
}

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	return 0;
}

int UnionGetNameOfDesKeyDB(char *fileName)
{
	sprintf(fileName,"%s/keyFile/unionDesKeyDB.KDB",getenv("UNIONETC"));
	return(0);
}

int UnionGetNameOfDesKeyDBOfBak(int num,char *bakFileName)
{
	sprintf(bakFileName,"%s/keyFile/unionDesKeyDB.KDB.%d.bak",getenv("UNIONETC"),num);
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

// �����Կ���ļ��Ƿ�����, ������ʱ���������޸ĵ�ʱ��
int UnionCheckDesKeyDB(char *fileName,char *lastUpdatedTime)
{
	FILE	*fp=NULL;
	TUnionDesKeyDB	keyDB;
	long 	num = 0;
	long	fsize = 0;
	long	presize = 0;
	
	fsize = UnionFsize(fileName);
	
	if (fsize <= sizeof(TUnionDesKeyDB))
	{
		UnionSystemErrLog("in UnionCheckDesKeyDB:: UnionFsize [%s],size[%ld]\n",fileName,fsize);
		return(errCodeUseOSErrCode);
	}
	
	// ȡ�ļ���С
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionCheckDesKeyDB:: fopen [%s]\n",fileName);
		//fclose(fp);
		return(errCodeUseOSErrCode);
	}
	
	memset(&keyDB,0,sizeof(keyDB));	
	fread(&keyDB,sizeof(keyDB),1,fp);
	
	fclose(fp);
	num = keyDB.num;
	strncpy(lastUpdatedTime,keyDB.lastUpdatedTime,14);
	presize = sizeof(TUnionDesKeyDB) + sizeof(TUnionDesKey) * num;
	if (fsize != presize)
	{
		UnionSystemErrLog("in UnionCheckDesKeyDB:: file[%s] is err! fsize[%ld],presize[%ld] \n",fileName,fsize,presize);
		return(errCodePKDBMDL_KeyIndexFileNotExists);  //mofify by hzh in 2012.1.10
	}
	return 0;
}

// ���DB���Ƿ�����
int UnionIsDesKeyDBOK()
{
	int ret = 0;
	char fileName[512];
	char lastUpdatedTime[15];
	memset(fileName,0,sizeof(fileName));
	memset(lastUpdatedTime,0,sizeof(lastUpdatedTime));
	UnionGetNameOfDesKeyDB(fileName);
	if ((ret = UnionCheckDesKeyDB(fileName,lastUpdatedTime)) < 0)
	{
		printf("DesKeyDB[%s]���쳣!\n",fileName);
		return ret;
	}
	return 0;
}

// ȡ����ģ��õı��ݿ�
int UnionGetDesKeyDBOfBak(char *filename)
{
	int i = 0;
	char tmpFileName[512];
	char tmpLastUpdatedTime[15];
	char lastUpdatedTime[15];
	int  findGoodBak = 0;
	
	memset(lastUpdatedTime,'0',sizeof(lastUpdatedTime));
	lastUpdatedTime[sizeof(lastUpdatedTime)-1] = '\0';
	
	for(i=0;i<gNumOfDBBak;i++)
	{
		memset(tmpFileName,0,sizeof(tmpFileName));
		memset(tmpLastUpdatedTime,0,sizeof(tmpLastUpdatedTime));
		UnionGetNameOfDesKeyDBOfBak(i,tmpFileName);
		if (UnionCheckDesKeyDB(tmpFileName,tmpLastUpdatedTime) < 0)
			continue;
		if (strcmp(tmpLastUpdatedTime,lastUpdatedTime) > 0)
		{
			findGoodBak = 1;
			strcpy(lastUpdatedTime,tmpLastUpdatedTime);
			strcpy(filename,tmpFileName);
		}
	}
	if (!findGoodBak)
		return -1;
		 
	return 0;
}

// ��DB���ñ��ݿ��滻
int UnionReplaceKeyDB(char *filename)
{
	char DesDBFileName[512];
	char cmd[512];
	memset(DesDBFileName,0,sizeof(DesDBFileName));
	UnionGetNameOfDesKeyDB(DesDBFileName);
	sprintf(cmd,"cp %s %s",filename,DesDBFileName);
	system(cmd);
	return 0;
}


int main(int argc,char **argv)
{
	int	ret;
	char	fileName[512];
	
	UnionSetApplicationName(argv[0]);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
		
	//���DB���������򷵻�	
	if ((ret = UnionIsDesKeyDBOK()) >= 0){
			printf("check DesKeyDB file OK!\n");
			return(UnionTaskActionBeforeExit());
	}
			
	gNumOfDBBak = UnionReadIntTypeRECVar("numberOfDBBak");
	if (gNumOfDBBak <= 0)
		gNumOfDBBak = 10;
		
	// DB�ⲻ������ȡһ�����ļ�
	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionGetDesKeyDBOfBak(fileName)) < 0)
	{
		printf("not found valid DBbak file!\n");
		return(UnionTaskActionBeforeExit());
	}
	
	if (!UnionConfirm("DesKeyDB DBFile is err! Are you reload DesKeyDB from back File[%s]\n",fileName))
	{
		  printf("DesKeyDB not reloaded!\n");
		  return(UnionTaskActionBeforeExit());
	}
	// �ָ�DB���ļ�
	if ((ret = UnionReplaceKeyDB(fileName)) < 0)
	{
		return(UnionTaskActionBeforeExit());
	}
	
	printf("Replaced DesKeyDB by %s !\n",fileName);
	return(UnionTaskActionBeforeExit());
	
}



#define _UnionTask_3_x_
#define _UnionEnv_3_x_		// ʹ��3.x�汾��UnionEnvģ��
#define _UnionLogMDL_3_x_

#define _UnionTask_3_2_

//#include <stdio.h>
#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <stdarg.h>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#else
#include "unionSimuUnixSharedMemory.h"
#include "simuUnixSystemCall.h"
#endif


#include "UnionProc.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionTask.h"
#include "UnionEnv.h"
#include "unionModule.h"

#include "unionVersion.h"
#ifndef _WIN32
#include "unionCommand.h"
#endif



#include "unionErrCode.h"
#include "UnionStr.h"
#include "UnionLog.h"

extern PUnionSharedMemoryModule	pgunionTaskMDL;
extern PUnionTaskTBL			pgunionTaskTBL;
extern PUnionTaskClass			pgunionTaskClass;
extern PUnionTaskInstance		pgunionTaskInstance;
extern PUnionTaskInstance		pgthisInstance;

int UnionGetNameOfTaskTBL(char *fileName)
{
	sprintf(fileName,"%s/unionTaskTBL.CFG",getenv("UNIONETC"));
	return(0);
}

int UnionGetMaxNumOfTaskClass()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		maxClassNum;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxClassNum")) == NULL)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionGetEnviVarByName for [%s]\n!","maxClassNum");
		UnionClearEnvi();
		return(errCodeTaskMDL_ConfFile);
	}
	maxClassNum = atoi(p);
	UnionClearEnvi();
	return(maxClassNum);
}

int UnionGetMaxNumOfTaskInstance()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		maxInstanceNum;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxInstanceNum")) == NULL)
	{
		UnionUserErrLog("in UnionGetMaxNumOfTaskClass:: UnionGetEnviVarByName for [%s]\n!","maxInstanceNum");
		UnionClearEnvi();
		return(errCodeTaskMDL_ConfFile);
	}
	maxInstanceNum = atoi(p);
	UnionClearEnvi();
	return(maxInstanceNum);
}

int UnionReloadTaskTBL()
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	PUnionTaskClass	ptaskClass;
	int		firstNullPos;
	char		tmpBuf[128+1];
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionConnectTaskTBL!\n");
		return(ret);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxClassNum")) == NULL)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarByName for [%s]\n!","maxClassNum");
		UnionClearEnvi();
		return(errCodeTaskMDL_ConfFile);
	}
	pgunionTaskTBL->maxClassNum = atoi(p);
	if ((p = UnionGetEnviVarByName("maxInstanceNum")) == NULL)
	{
		UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarByName for [%s]\n!","maxInstanceNum");
		UnionClearEnvi();
		return(errCodeTaskMDL_ConfFile);
	}
	pgunionTaskTBL->maxInstanceNum = atoi(p);
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)
			continue;
		if ((strcmp((pgunionTaskClass+i)->name,"maxClassNum") == 0) || 
			(strcmp((pgunionTaskClass+i)->name,"maxInstanceNum") == 0))
		{
			memset(pgunionTaskClass+i,0,sizeof(*ptaskClass));
			continue;
		}
		if ((p = UnionGetEnviVarByName((pgunionTaskClass+i)->name)) == NULL)
		{
			if ((pgunionTaskClass+i)->currentNum > 0)
				continue;
			memset(pgunionTaskClass+i,0,sizeof(*ptaskClass));
		}
	}
	
	for (i = 0,firstNullPos = 0; (i < UnionGetEnviVarNum()) && (firstNullPos < pgunionTaskTBL->maxClassNum); i++)
	{
		// ��ȡ����
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if ((strcmp(p,"maxClassNum") == 0) || (strcmp(p,"maxInstanceNum") == 0))
			continue;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionFilterRubbisBlank(p,strlen(p),tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionFilterRubbisBlank [%s]\n",p);
			continue;
		}
		if ((ptaskClass = UnionFindTaskClass(tmpBuf)) == NULL)
		{
			for (;firstNullPos < pgunionTaskTBL->maxClassNum;firstNullPos++)
			{
				ptaskClass = pgunionTaskClass + firstNullPos;
				if (strlen(ptaskClass->name) == 0)
					break;
				else
					continue;
			}
			if (firstNullPos >=  pgunionTaskTBL->maxClassNum)
			{
				UnionUserErrLog("in UnionReloadTaskTBL:: taskTBL is full! maxClassNum = [%d]\n",pgunionTaskTBL->maxClassNum);
				break;
			}
			ptaskClass->currentNum = 0;
		}
		if (strlen(tmpBuf) >= sizeof(ptaskClass->name))
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: taskName [%s] too long!\n",tmpBuf);
			continue;
		}
		strcpy(ptaskClass->name,tmpBuf);
		// ��ȡstartCmd
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		if (strlen(p) > sizeof(ptaskClass->startCmd) - 1)
			continue;
		strcpy(ptaskClass->startCmd,p);
			
		// ��ȡminNum
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		if ((ptaskClass->minNum = atoi(p)) < 0)
			ptaskClass->minNum = 1;
		
		// ��ȡlogFileName
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,3)) == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskTBL:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,3);
			continue;
		}
		if (strlen(p) > sizeof(ptaskClass->logFileName) - 1)
			continue;
		strcpy(ptaskClass->logFileName,p);
		/*UnionNullLog("[%s][%s][%d][%d][%s]\n",
			ptaskClass->name,
			ptaskClass->startCmd,
			ptaskClass->minNum,
			ptaskClass->currentNum,
			ptaskClass->logFileName);
		*/
		firstNullPos++;
	}
	
	UnionClearEnvi();
		
	return(0);
}

int UnionReloadTaskClass(char *taskName)
{
	char			taskDefLine[512+1];
	char			fileName[512+1];
	int			ret;
	PUnionEnviVariable	penviVar;
	TUnionTaskClass		taskClass;
	int			index;
	PUnionTaskClass		ptaskClass;
	
	if (taskName == NULL)
		return(UnionReloadTaskTBL());
	if (strlen(taskName) >= sizeof(taskClass.name))
	{
		UnionUserErrLog("in UnionReloadTaskClass:: taskName [%s] too long!\n",taskName);
		return(errCodeParameter);
	}
	// ���ļ��ж�ȡ������
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	memset(taskDefLine,0,sizeof(taskDefLine));
	if ((ret = UnionReadEnviVarDefLineInFile(fileName,taskName,taskDefLine)) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: UnionReadEnviVarDefLineInFile [%s]!\n",taskName);
		return(ret);
	}
	if ((penviVar = ReadUnionEnviVarFromStr(taskDefLine)) == NULL)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: ReadUnionEnviVarFromStr [%s]!\n",taskDefLine);
		return(errCodeTaskMDL_TaskClassDefLineError);
	}
	for (index = 0; index < 4; index++)
	{
		if (penviVar->pVariableValue[index] == NULL)
		{
			UnionUserErrLog("in UnionReloadTaskClass:: ReadUnionEnviVarFromStr [%s]!\n",taskDefLine);
			UnionFreeEnviVar(penviVar);
			return(errCodeTaskMDL_TaskClassDefLineError);
		}
	}
	// ƴװ����
	memset(&taskClass,0,sizeof(taskClass));
	strcpy(taskClass.name,taskName);
	if (strlen(penviVar->pVariableValue[1]) < sizeof(taskClass.startCmd))
		strcpy(taskClass.startCmd,penviVar->pVariableValue[1]);
	if (strlen(taskClass.startCmd) != 0)
		taskClass.minNum = atoi(penviVar->pVariableValue[2]);
	if (strlen(penviVar->pVariableValue[3]) < sizeof(taskClass.logFileName))
		strcpy(taskClass.logFileName,penviVar->pVariableValue[3]);
	else
		memcpy(taskClass.logFileName,penviVar->pVariableValue[3],sizeof(taskClass.logFileName)-1);
	UnionFreeEnviVar(penviVar);
	// ���ڴ��в����������Ƿ����
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionReloadTaskClass:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	if ((ptaskClass	= UnionFindTaskClass(taskName)) == NULL)	// ������
	{
		for (index = 0; index < pgunionTaskTBL->maxClassNum; index++)
		{
			if (strlen((pgunionTaskClass+index)->name) == 0)	// �����ͱ��в��ҵ���һ����λ��
			{
				memcpy(pgunionTaskClass+index,&taskClass,sizeof(taskClass));
				return(0);
			}
		}
		UnionUserErrLog("in UnionReloadTaskClass:: table is full!\n");
		return(errCodeTaskMDL_TaskClassTableIsFull);
	}
	else
	{
		strcpy(ptaskClass->startCmd,taskClass.startCmd);
		ptaskClass->minNum = taskClass.minNum;
		strcpy(ptaskClass->logFileName,taskClass.logFileName);
		return(0);
	}
}

// ����һ���µ��������Ͷ���
int UnionAddNewTaskClass(char *taskName,char *command,int num,char *logFileName)
{
	int		i;
	int		ret;
	PUnionTaskClass	ptaskClass;
	char		fileName[512];
	//char		dateTime[100];
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionAddNewTaskClass:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	if (UnionFindTaskClass(taskName) != NULL)
	{
		UnionUserErrLog("in UnionAddNewTaskClass:: task class [%s] already exists!\n",taskName);
		return(errCodeTaskMDL_TaskClassAlreadyExists);
	}
	
	for (i = 0; i < pgunionTaskTBL->maxClassNum; i++)
	{
		if (strlen((pgunionTaskClass+i)->name) == 0)	// �����ͱ��в��ҵ���һ����λ��
		{
			// ������������
			ptaskClass = pgunionTaskClass+i;
			if (taskName == NULL)
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: null task class name!\n");
				return(errCodeTaskMDL_TaskClassNameIsNull);
			}
			if (strlen(taskName) >= sizeof(ptaskClass->name))
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: task class name [%s] too long!\n",taskName);
				return(errCodeTaskMDL_TaskClassNameTooLong);
			}
			strcpy(ptaskClass->name,taskName);
			if ((command == NULL) || (strlen(command) == 0))
				strcpy(ptaskClass->startCmd,"null");
			else
			{
				if (strlen(command) >= sizeof(ptaskClass->startCmd))
				{
					UnionUserErrLog("in UnionAddNewTaskClass:: task class start command [%s] too long!\n",command);
					return(errCodeTaskMDL_TaskClassStartCmdTooLong);
				}
				strcpy(ptaskClass->startCmd,command);
			}
			ptaskClass->minNum = num;
			ptaskClass->currentNum = 0;
			if ((logFileName == NULL) || (strlen(logFileName) == 0))
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: null task class logfile name!\n");
				return(errCodeTaskMDL_TaskClassLogFileNameIsNull);
			}
			if (strlen(logFileName) >= sizeof(ptaskClass->logFileName))
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: task log file name [%s] too long!\n",logFileName);
				return(errCodeTaskMDL_TaskClassLogFileNameTooLong);
			}
			strcpy(ptaskClass->logFileName,logFileName);
			
			// ���������ļ������Ӽ�¼
			memset(fileName,0,sizeof(fileName));
			UnionGetNameOfTaskTBL(fileName);
			if ((command == NULL) || (strlen(command) == 0))
				ret = UnionInsertEnviVar(fileName,taskName,"[null]	[%d]	[%s]",num,logFileName);
			else
				ret = UnionInsertEnviVar(fileName,taskName,"[%s]	[%d]	[%s]",command,num,logFileName);
			if (ret < 0)
			{
				UnionUserErrLog("in UnionAddNewTaskClass:: UnionInsertEnviVar [%s]\n",taskName);
				return(ret);
			}
			return(0);
			/*
			if ((fp = fopen(fileName,"a")) == NULL)
			{
				UnionSystemErrLog("in UnionAddNewTaskClass:: fopen [%s]\n",fileName);
				return(errCodeUseOSErrCode);
			}
			fprintf(fp,"\n");
			memset(dateTime,0,sizeof(dateTime));
			UnionGetFullSystemDateTime(dateTime);
			//fprintf(fp,"# %s auto added by ESSC\n",dateTime);
			fprintf(fp,"[%s]		[%s]		[%d]	[%s]\n",
				ptaskClass->name,ptaskClass->startCmd,ptaskClass->minNum,ptaskClass->logFileName);
			fflush(fp);
			fclose(fp);
			return(0);
			*/
		}
	}
	UnionUserErrLog("in UnionAddNewTaskClass:: task class table is full!\n");
	return(errCodeTaskMDL_TaskClassTableIsFull);
}

// ɾ��һ���������Ͷ���
int UnionDeleteTaskClass(char *taskName)
{
	int		ret;
	PUnionTaskClass	ptaskClass;
	char		fileName[512];
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionDeleteTaskClass:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	if ((ptaskClass = UnionFindTaskClass(taskName)) != NULL)
	{
		if (ptaskClass->currentNum > 0)	// ����������ʹ�ô���
		{
			UnionUserErrLog("in UnionDeleteTaskClass:: taskClass [%s] is used now!\n",taskName);
			return(errCodeTaskMDL_TaskClassIsUsed);
		}
		memset(ptaskClass->name,0,sizeof(ptaskClass->name));	// ���ڴ���ɾ������
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionDeleteEnviVar(fileName,taskName)) < 0)
	{
		UnionUserErrLog("in UnionDeleteTaskClass:: UnionDeleteEnviVar!\n");
		return(ret);
	}
	return(ret);
}
// ����һ���������Ͷ���
// ���command,logFileNameΪ�գ������£����numΪ��ֵ��������
int UnionUpdateTaskClass(char *taskName,char *command,int num,char *logFileName)
{
	int		ret;
	PUnionTaskClass	ptaskClass;
	char		fileName[512];
	int		offset = 0;
	char		tmpBuf[512];
		
	if ((ret = UnionConnectTaskTBL()) < 0)
	{
		UnionUserErrLog("in UnionUpdateTaskClass:: UnionConnectTaskTBL!\n");
		return(ret);
	}
	
	if ((ptaskClass = UnionFindTaskClass(taskName)) == NULL)
	{
		UnionUserErrLog("in UnionUpdateTaskClass:: UnionFindTaskClass [%s]!\n",taskName);
		return(ret);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf+offset,"[",1);
	offset = 1;
	if ((command != NULL) || (strlen(command) != 0))	// �޸���������
	{
		if (strlen(command) >= sizeof(ptaskClass->startCmd))
		{
			memcpy(ptaskClass->startCmd,command,sizeof(ptaskClass->startCmd)-1);
			ptaskClass->startCmd[sizeof(ptaskClass->startCmd)-1] = 0;
			memcpy(tmpBuf+offset,command,sizeof(ptaskClass->startCmd)-1);
			tmpBuf[offset+sizeof(ptaskClass->startCmd)-1] = ']';
		}
		else
		{
			strcpy(ptaskClass->startCmd,command);
			sprintf(tmpBuf+offset,"%s]	",command);
		}
	}
	else	// ���޸���������
	{
		sprintf(tmpBuf+offset,"%s]	",ptaskClass->startCmd);
	}
	offset = strlen(tmpBuf);
	if (num < 0)	// ������С����
		sprintf(tmpBuf+offset,"[%d]	",ptaskClass->minNum);
	else		// ����С����
	{
		sprintf(tmpBuf+offset,"[%d]	",num);
		ptaskClass->minNum = num;
	}
	offset = strlen(tmpBuf);
	memcpy(tmpBuf+offset,"[",1);
	offset += 1;
	if ((logFileName != NULL) || (strlen(logFileName) != 0))	// �޸���־�ļ�
	{
		if (strlen(logFileName) >= sizeof(ptaskClass->logFileName))
		{
			memcpy(ptaskClass->logFileName,logFileName,sizeof(ptaskClass->logFileName)-1);
			ptaskClass->logFileName[sizeof(ptaskClass->logFileName)-1] = 0;
			memcpy(tmpBuf+offset,logFileName,sizeof(ptaskClass->logFileName)-1);
			tmpBuf[offset+sizeof(ptaskClass->logFileName)-1] = ']';
		}
		else
		{
			strcpy(ptaskClass->logFileName,logFileName);
			sprintf(tmpBuf+offset,"%s]",logFileName);
		}
	}
	else	// ���޸���־�ļ�
		sprintf(tmpBuf+offset,"%s]",ptaskClass->logFileName);
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfTaskTBL(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,taskName,tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionUpdateTaskClass:: UnionUpdateEnviVar [%s]!\n",taskName);
		return(ret);
	}
	return(ret);
}


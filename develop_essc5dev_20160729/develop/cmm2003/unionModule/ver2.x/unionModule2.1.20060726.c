//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0

/*
	本模块操作一个名为unionMDLTBL.CFG的文件，该文件存储在$UNIONETC目录，每一行定义了一个共享模块
	其结构如下：
		[nameOfMDL]	[idOfMDL]	[reloadCommandOfMDL]
*/

// 2006/7/26 在unionModule2.1.20060414基础上修改
// 修改了UnionRemoveAllSharedMemoryModule，在这个函数中，删除的如果是消息交换区，会同步删除交换区对应的对列

#ifndef _unionModule_2_x_above_
#define _unionModule_2_x_above_
#endif

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "unionModule.h"
#include "UnionEnv.h"
#include "unionErrCode.h"
#include "unionIndexTBL.h"
#include "UnionLog.h"
#include "UnionStr.h"

// 2007/10/26增加
int UnionIsSharedMemoryInited(char *mdlName __attribute__((unused)))
{
	return(1);
}

PUnionSharedMemoryModule UnionConnectExistingSharedMemoryModule(char *mdlName,long sizeOfUserSpace)
{
	int				ret;
	TUnionSharedMemoryModule	mdl;
	int				resID;
	PUnionSharedMemoryModule	pmdl;
	
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UnionReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		UnionUserErrLog("in UnionConnectExistingSharedMemoryModule:: UnionReadSharedMemoryModuleDef [%s]!\n",mdlName);
		return(NULL);
	}
	
	if (mdl.id <= 0)
	{
		UnionUserErrLog("in UnionConnectExistingSharedMemoryModule:: mdl.id = [%d] Error!\n",mdl.id);
		return(NULL);
	}
	
	if (sizeOfUserSpace < 0)
	{
		UnionUserErrLog("in UnionConnectExistingSharedMemoryModule:: sizeOfUserSpace = [%ld] Error!\n",sizeOfUserSpace);
		return(NULL);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule) + sizeOfUserSpace,0666)) == -1)
	{
		UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmget [%d] [%ld]!\n",mdl.id,sizeOfUserSpace);
		return(NULL);
	}
		
	//UnionLog("in UnionConnectExistingSharedMemoryModule:: shmget [%d] OK newCreated = [%d]!\n",mdl.id,mdl.newCreated);
	
	if ((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL)
	{
		UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
		return(NULL);
	}

	
	++(pmdl->users);
	pmdl->newCreated = 0;
	
	if (sizeOfUserSpace == 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));

	return(pmdl);
}
int UnionGetNameOfSharedMemoryModuleTBL(char *fileName)
{
	sprintf(fileName,"%s/unionSharedMemoryMDLTBL.CFG",getenv("UNIONETC"));
	return(0);
}

int UnionReadSharedMemoryModuleDef(char *mdlName,PUnionSharedMemoryModule pmdl)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	
	if ((mdlName == NULL) || (pmdl == NULL))
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: mdlName or pmdl is NULL!\n");
		return(errCodeParameter);
	}
	if (strlen(mdlName) > sizeof(pmdl->name) - 1)
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: mdlName [%s] longer than expected [%zu]!\n",mdlName,sizeof(pmdl->name)-1);
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: UnionInitEnvi!\n");
		return(ret);
	}

	if ((i = UnionGetVarIndexOfTheVarName(mdlName)) < 0)
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: UnionGetVarIndexOfTheVarName for [%s]\n",mdlName);
		goto abnormalExit;
	}
	
	// 找到了该模块的定义
	memset(pmdl,0,sizeof(*pmdl));
	strcpy(pmdl->name,mdlName);
	// 读取ID号
	if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
		goto abnormalExit;
	}
	pmdl->id = atoi(p);
	// 读取自动加载命令
	if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
		goto abnormalExit;
	}
	if (strlen(p) > sizeof(pmdl->reloadCommand) - 1)
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: reloadCommand [%s] longer than expected [%zu]!\n",p,sizeof(pmdl->reloadCommand)-1);
		goto abnormalExit;
	}
	strcpy(pmdl->reloadCommand,p);
	pmdl->index = i;
	pmdl->sizeOfUserSpace = 0;
	pmdl->puserSpace = NULL;
	pmdl->users = 0;
	pmdl->newCreated = 0;
	pmdl->readingLocks = 0;
	pmdl->writingLocks = 0;
	
	//UnionLog("in UnionReadSharedMemoryModuleDef:: [%s] [%d] [%s] [%d]\n",pmdl->name,pmdl->id,pmdl->reloadCommand,pmdl->index);
		
	UnionClearEnvi();
	return(0);

abnormalExit:
	UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: [%s] not defined in file [%s]\n",mdlName,fileName);
	
	UnionClearEnvi();
	return(errCodeSharedMemoryMDL_MDLNotDefined);
}

int UnionIsNewCreatedSharedMemoryModule(PUnionSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(0);
	return(pmdl->newCreated);
}

unsigned char *UnionGetAddrOfSharedMemoryModuleUserSpace(PUnionSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(NULL);
	
	// modify by hzh in 2010.1.20 pmdl->puserSpace可能被其他进程修改
	//return(pmdl->puserSpace);
	return((unsigned char *)pmdl+sizeof(TUnionSharedMemoryModule));
}
	
int UnionPrintSharedMemoryModuleToFile(PUnionSharedMemoryModule pmdl,FILE *fp)
{
	if ((fp == NULL) || (pmdl == NULL))
	{
		UnionUserErrLog("in UnionPrintSharedMemoryModuleToFile:: fp or pmdl is NULL!\n");
		return(errCodeParameter);
	}
	
	fprintf(fp,"\nThis SharedMemoryModule\n");
	fprintf(fp,"[name]			[%s]\n",pmdl->name);
	fprintf(fp,"[id]			[%d]\n",pmdl->id);
	fprintf(fp,"[reloadCommand]		[%s]\n",pmdl->reloadCommand);
	fprintf(fp,"[index]			[%d]\n",pmdl->index);
	fprintf(fp,"[sizeOfUserSpace]	[%ld]\n",pmdl->sizeOfUserSpace);
	if (pmdl->puserSpace)
		fprintf(fp,"[userSpaceAddress]	[%s]\n",pmdl->puserSpace);
	else
		fprintf(fp,"[userSpaceAddress]	[NULL]\n");
	fprintf(fp,"[users]			[%d]\n",pmdl->users);
	fprintf(fp,"[newCreated]		[%d]\n",pmdl->newCreated);
	fprintf(fp,"[readingLocks]		[%d]\n",pmdl->readingLocks);
	fprintf(fp,"[writingLocks]		[%d]\n",pmdl->writingLocks);
	fflush(fp);
	
	return(0);
}
		
int UnionPrintSharedMemoryModule(PUnionSharedMemoryModule pmdl)
{
	return(UnionPrintSharedMemoryModuleToFile(pmdl,stdout));
}

int UnionRemoveAllSharedMemoryModule()
{
	int			ret;
	struct shmid_ds		buf;
	int 			resID;
	char			fileName[512];
	char			*p;
	int			id;
	int			i;
	int			num;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionRemoveAllSharedMemoryModule:: UnionInitEnvi!\n");
		return(ret);
	}
	
	for (i = 0,num = 0; i < UnionGetEnviVarNum(); i++)
	{
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
			continue;
		UnionNullLog("i = %d	idOfShareMemoryID = [%d]\n",i,atoi(p));
		if ((id = atoi(p)) <= 0)
			continue;
		if ((resID = shmget(id,sizeof(TUnionSharedMemoryModule),0666)) == -1)
		{
			UnionSystemErrLog("in UnionRemoveAllSharedMemoryModule:: shmget [%d]\n",id);
			continue;
		}
	
		if (shmctl(resID,IPC_RMID,&buf) != 0)
		{
			UnionSystemErrLog("in UnionRemoveAllSharedMemoryModule:: shmctl IPC_RMID resID = [%d]\n",id);
			continue;
		}
		num++;
		// 读取模块名称,以下是20060726增加
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
			continue;
		if (strcmp(p,"UnionMsgBufMDL") != 0)	// 不是消息交换区模块
			continue;
		// 是消息交换区模块，删除对应的队列
		if ((ret = UnionRemoveIndexStatusTBL(id)) < 0)
		{
			UnionUserErrLog("in UnionRemoveAllSharedMemoryModule:: UnionRemoveIndexStatusTBL id = [%d]\n",id);
		}
	}

	UnionClearEnvi();
	
	return(num);
}

PUnionSharedMemoryModule UnionConnectSharedMemoryModule(char *mdlName,long sizeOfUserSpace)
{
	int				ret;
	TUnionSharedMemoryModule	mdl;
	int				resID;
	PUnionSharedMemoryModule	pmdl;
	unsigned char 			*pmUserSpace = NULL;
	
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UnionReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		UnionUserErrLog("in UnionConnectSharedMemoryModule:: UnionReadSharedMemoryModuleDef [%s]!\n",mdlName);
		return(NULL);
	}
	
	if (mdl.id <= 0)
	{
		UnionUserErrLog("in UnionConnectSharedMemoryModule:: mdl.id = [%d] Error!\n",mdl.id);
		return(NULL);
	}
	
	if (sizeOfUserSpace < 0)
	{
		UnionUserErrLog("in UnionConnectSharedMemoryModule:: sizeOfUserSpace = [%ld] Error!\n",sizeOfUserSpace);
		return(NULL);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule) + sizeOfUserSpace,0666)) == -1)
	{
		if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule) + sizeOfUserSpace,IPC_CREAT|0666)) == -1)
		{
			UnionSystemErrLog("in UnionConnectSharedMemoryModule:: shmget [%d] [%ld]!\n",mdl.id,sizeOfUserSpace);
			return(NULL);
		}
		mdl.newCreated = 1;
	}
	else
		mdl.newCreated = 0;
		
	//UnionLog("in UnionConnectSharedMemoryModule:: shmget [%d] OK newCreated = [%d]!\n",mdl.id,mdl.newCreated);
	
	if ((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL)
	{
		UnionSystemErrLog("in UnionConnectSharedMemoryModule:: shmat [%d]!\n",mdl.id);
		return(NULL);
	}

	
	if (!mdl.newCreated)
	{
		++(pmdl->users);
		pmdl->newCreated = 0;
	}
	else	// 新建
	{		
		memcpy(pmdl,&mdl,sizeof(mdl));
		pmdl->users = 1;
		pmdl->readingLocks = 0;
		pmdl->writingLocks = 0;
		pmdl->sizeOfUserSpace = sizeOfUserSpace;
	}
	if (sizeOfUserSpace == 0) {
		pmdl->puserSpace = NULL;
		pmUserSpace = NULL;
	}
	else {
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
		pmUserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	}
	
	// modify by hzh in 2010.1.20 pmdl->puserSpace可能被其他进程修改
	/*
	if ((pmdl->newCreated) && (pmdl->puserSpace)) 
		memset(pmdl->puserSpace,0,sizeOfUserSpace);
	*/
	if ((pmdl->newCreated) && (pmUserSpace)) 
		memset(pmUserSpace,0,sizeOfUserSpace);
	
	return(pmdl);
}
	
int UnionDisconnectShareModule(PUnionSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(0);
	if (pmdl->users >= 0)
		--pmdl->users;
	pmdl = NULL;
	return(0);
}

int UnionRemoveSharedMemoryModule(char *mdlName)
{
	int				ret;
	struct shmid_ds			buf;
	int 				resID;
	TUnionSharedMemoryModule	mdl;
	
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UnionReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		UnionUserErrLog("in UnionRemoveSharedMemoryModule:: UnionReadSharedMemoryModuleDef for [%s]\n",mdlName);
		return(ret);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule),0666)) == -1)
	{
		UnionSystemErrLog("in UnionRemoveAllSharedMemoryModule:: shmget [%d]\n",mdl.id);
		return(errCodeUseOSErrCode);
	}
	
	if (shmctl(resID,IPC_RMID,&buf) != 0)
	{
		UnionSystemErrLog("in UnionRemoveAllSharedMemoryModule:: shmctl IPC_RMID resID = [%d]\n",mdl.id);
		return(errCodeUseOSErrCode);
	}

	return(0);
}	

int UnionPrintSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp)
{
	PUnionSharedMemoryModule	pmdl;

	if ((fp == NULL) || (mdlName == NULL))
		return(errCodeParameter);
	
	if ((pmdl = UnionConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		UnionUserErrLog("in UnionPrintSharedMemoryModuleToFileByModuleName:: UnionConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	UnionPrintSharedMemoryModuleToFile(pmdl,fp);
	
	if (UnionIsNewCreatedSharedMemoryModule(pmdl))
	{
		UnionDisconnectShareModule(pmdl);
		UnionRemoveSharedMemoryModule(mdlName);
		return(0);
	}
	else
	{	
		UnionDisconnectShareModule(pmdl);
		return(0);
	}
}

int UnionApplyWritingLocks(PUnionSharedMemoryModule pmdl)
{
	int	i;

	if (pmdl == NULL)
	{
		UnionUserErrLog("in UnionApplyWritingLocks:: null Pointer!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < 100; i++)
	{
		//if ((pmdl->readingLocks <= 0) && (pmdl->writingLocks <= 0))
		if (pmdl->writingLocks <= 0)
		{
			pmdl->writingLocks++;
			if (pmdl->writingLocks > 0)
				return(0);
			pmdl->writingLocks = 1;
			return(0);
		}
		else
		{
			usleep(1000);
			continue;
		}
	}
	
	return(errCodeSharedMemoryMDL_LockMDL);
}

int UnionReleaseWritingLocks(PUnionSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
	{
		UnionUserErrLog("in UnionReleaseWritingLocks:: null Pointer!\n");
		return(errCodeParameter);
	}

	pmdl->writingLocks--;
	if (pmdl->writingLocks >= 0)
		return(0);
		
	pmdl->writingLocks = 0;
	return(0);
}

int UnionApplyReadingLocks(PUnionSharedMemoryModule pmdl)
{
	int	i;

	if (pmdl == NULL)
	{
		UnionUserErrLog("in UnionApplyReadingLocks:: null Pointer!\n");
		return(errCodeParameter);
	}

	for (i = 0; i < 100; i++)
	{
		if (pmdl->writingLocks <= 0)
		{
			pmdl->readingLocks++;
			if (pmdl->readingLocks > 0)
				return(0);
			pmdl->readingLocks = 1;
			return(0);
		}
		else
		{
			usleep(1000);
			continue;
		}
	}
	
	return(errCodeSharedMemoryMDL_LockMDL);
}

int UnionReleaseReadingLocks(PUnionSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
	{
		UnionUserErrLog("in UnionReleaseReadingLocks:: null Pointer!\n");
		return(errCodeParameter);
	}

	pmdl->readingLocks--;
	if (pmdl->readingLocks >= 0)
		return(0);
	pmdl->readingLocks = 0;
	return(0);
}

int UnionResetWritingLocks(char *mdlName)
{
	PUnionSharedMemoryModule	pmdl;

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UnionConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		UnionUserErrLog("in UnionResetWritingLocks:: UnionConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	pmdl->writingLocks = 0;
	
	if (UnionIsNewCreatedSharedMemoryModule(pmdl))
	{
		UnionDisconnectShareModule(pmdl);
		UnionRemoveSharedMemoryModule(mdlName);
		return(0);
	}
	else
	{	
		UnionDisconnectShareModule(pmdl);
		return(0);
	}
}

int UnionResetReadingLocks(char *mdlName)
{
	PUnionSharedMemoryModule	pmdl;

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UnionConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		UnionUserErrLog("in UnionResetReadingLocks:: UnionConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	pmdl->readingLocks = 0;
	
	if (UnionIsNewCreatedSharedMemoryModule(pmdl))
	{
		UnionDisconnectShareModule(pmdl);
		UnionRemoveSharedMemoryModule(mdlName);
		return(0);
	}
	else
	{	
		UnionDisconnectShareModule(pmdl);
		return(0);
	}
}

int UnionGetUserIDOfSharedMemoryModule(PUnionSharedMemoryModule pmdl)
{
	if (pmdl == NULL)
		return(errCodeParameter);
	return(pmdl->id);
}

int UnionReloadSharedMemoryModule(char *mdlName)
{
	int				ret;
	TUnionSharedMemoryModule	mdl;
	char				command[512];
	
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UnionReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		UnionUserErrLog("in UnionReloadSharedMemoryModule:: UnionReadSharedMemoryModuleDef for [%s]\n",mdlName);
		return(ret);
	}
	if (strlen(mdl.reloadCommand) == 0)
		return(0);
	strcpy(command,mdl.reloadCommand);
	UnionToUpperCase(command);
	if (strcmp(command,"NULL") == 0)
		return(0);
	sprintf(command,"%s -reload",mdl.reloadCommand);
	UnionLog("in UnionReloadAllSharedMemoryModule:: command = [%s]\n",command);
	return(system(command));
}	

int UnionReloadAllSharedMemoryModule(char *anywayOrNot)
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i;
	int			num;
	char			command[256];
	int			reloadWithoutConfirm = 0;
	
	if (anywayOrNot != NULL)
	{
		UnionToUpperCase(anywayOrNot);
		if (strcmp(anywayOrNot,"-RELOADALL") == 0)
			reloadWithoutConfirm = 0;
		else if (strcmp(anywayOrNot,"-RELOADALLANYWAY") == 0)
			reloadWithoutConfirm = 1;
		else
		{
			UnionUserErrLog("in UnionReloadAllSharedMemoryModule:: wrong command [%s]\n",anywayOrNot);
			return(0);
		}
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReloadAllSharedMemoryModule:: UnionInitEnvi!\n");
		return(ret);
	}
	
	for (i = 0,num = 0; i < UnionGetEnviVarNum(); i++)
	{
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
			continue;
		strcpy(command,p);
		if (strlen(p) == 0)
			continue;
		UnionToUpperCase(command);
		if (strcmp(command,"NULL") == 0)
			continue;		
		if (strstr(p,"-reload") == NULL)
		{
			if (reloadWithoutConfirm)
				sprintf(command,"%s -reloadanyway",p);
			else
				sprintf(command,"%s -reload",p);
		}
		else
			sprintf(command,"%s",p);
		UnionLog("in UnionReloadAllSharedMemoryModule:: command = [%s]\n",command);
		system(command);
		num++;
	}

	UnionClearEnvi();
	
	return(num);
}

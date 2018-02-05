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

// 2006/12/27 在2.1.20060726基础上升级，增加了一组打印共享内存状态的函数

// 2007/10/26 在2.1.20061227基础上升级
// 增加了一个数组，用于标识一个进程连接的共享内存。
// 这个组用于解决在连接的共享内存超过最大限制数目时，释放不必要的连接。

#ifndef _unionModule_2_x_above_
#define _unionModule_2_x_above_
#endif

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#include "unionIndexTBL.h"
#else
#include <windows.h>
#include "unionSimuUnixSharedMemory.h"
#include "unionWorkingDir.h"
#endif

#include "UnionStr.h"
#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionErrCode.h"


// 以下变量2007/12/26增加
TUnionSharedMemoryGroup		gunionSharedMemoryGrp;
int				gunionIsSharedMemoryGrpInited = 0;

// 2007/10/26增加
int UnionOutputAllRegisteredShareMemory(char *buf,int sizeOfBuf)
{
	int	index;
	int	ret;
	int	offset = 0;
	PUnionSharedMemoryModule	pmdl;

#ifdef _AIX_	
	UnionInitSharedMemoryGroup();
	
	sprintf(buf,"num=%04d|",gunionSharedMemoryGrp.num);
	offset = strlen(buf);
	for (index = 0; index < gunionSharedMemoryGrp.num; index++)
	{
		pmdl = gunionSharedMemoryGrp.pshmPtrGrp[index];
		if ((pmdl == NULL) || ((int)pmdl == -1))
			continue;
		if (offset + strlen(pmdl->name) + 8 >= sizeOfBuf)
			break;
		sprintf(buf+offset,"name%02d=%s|",index,pmdl->name);
		offset += (strlen(pmdl->name) + 8);
	}
	return(offset);
#else
	return(0);
#endif
}

// 2007/10/26增加
int UnionIsSharedMemoryGroupInited()
{
#ifdef _AIX_
	return(gunionIsSharedMemoryGrpInited);
#else
	return(1);
#endif
}

// 2007/10/26增加
void UnionInitSharedMemoryGroup()
{
	int	index;
#ifdef _AIX_
	if (gunionIsSharedMemoryGrpInited)
		return;
		
	for (index = 0; index < conMaxNumOfSharedMemoryPerProcess; index++)
		gunionSharedMemoryGrp.pshmPtrGrp[index] = NULL;
	gunionSharedMemoryGrp.num = 0;
	gunionIsSharedMemoryGrpInited = 1;
#endif
	return;
}

// 2007/10/26增加
void UnionAddIntoSharedMemoryGroup(PUnionSharedMemoryModule pmdl)
{
#ifdef _AIX_
	if ((pmdl == NULL) || ((int)pmdl == -1))
		return;
	
	UnionInitSharedMemoryGroup();
	if (gunionSharedMemoryGrp.num >= conMaxNumOfSharedMemoryPerProcess)
	{
		UnionUserErrLog("in UnionAddIntoSharedMemoryGroup:: shared memory group is full! [%x] not added\n",pmdl);
		return;
	}
	gunionSharedMemoryGrp.pshmPtrGrp[gunionSharedMemoryGrp.num] = pmdl;
	gunionSharedMemoryGrp.num += 1;
	//UnionProgramerLog("in UnionAddIntoSharedMemoryGroup:: [%x] of [%s] added ok!\n",pmdl,pmdl->name);
#endif
	return;
}

// 2007/10/26增加
void UnionDeleteFromSharedMemoryGroup(PUnionSharedMemoryModule pmdl)
{
	int	index;
#ifdef _AIX_	
	if ((pmdl == NULL) || ((int)pmdl == -1))
		return;
	
	UnionInitSharedMemoryGroup();
	for (index = 0; index < gunionSharedMemoryGrp.num; index++)
	{
		if (gunionSharedMemoryGrp.pshmPtrGrp[index] == pmdl)
		{
			gunionSharedMemoryGrp.pshmPtrGrp[index] = gunionSharedMemoryGrp.pshmPtrGrp[gunionSharedMemoryGrp.num-1];
			gunionSharedMemoryGrp.num -= 1;
			shmdt(pmdl);
			pmdl = NULL;
			return;
		}
	}
	//UnionUserErrLog("in UnionDeleteFromSharedMemoryGroup:: shared memory [%x] not exists!\n",pmdl);
#endif
	return;
}

// 2007/10/26增加
int UnionIsSharedMemoryGroupIsFull()
{
#ifdef _AIX_
	UnionInitSharedMemoryGroup();
	if (gunionSharedMemoryGrp.num >= conMaxNumOfSharedMemoryPerProcess)
		return(1);
	else
		return(0);
#else
	return(0);
#endif
}

// 2007/10/26增加
int UnionIsSharedMemoryInited(char *mdlName)
{
	int	index;
	
#ifdef _AIX_
	UnionInitSharedMemoryGroup();
	for (index = 0; index < gunionSharedMemoryGrp.num; index++)
	{
		if ((gunionSharedMemoryGrp.pshmPtrGrp[index] == NULL) || ((int)(gunionSharedMemoryGrp.pshmPtrGrp[index]) == -1))
			continue;
		if (strcmp(gunionSharedMemoryGrp.pshmPtrGrp[index]->name,mdlName) == 0)
			return(1);
	}
	return(0);
#else
	return(1);
#endif
}

// 2007/10/26增加
void UnionFreeUnnecessarySharedMemory()
{
	int				index;
	PUnionSharedMemoryModule	pmdl;

#ifdef _AIX_	
	UnionInitSharedMemoryGroup();	
	for (index = 0; index < gunionSharedMemoryGrp.num; index++)
	{
		pmdl = gunionSharedMemoryGrp.pshmPtrGrp[index];
		if ((pmdl == NULL) || ((int)pmdl == -1))
			continue;
		if (strcmp(pmdl->name,"UnionTaskTBLMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UnionLogTBLMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UnionSJL06TBLMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UnionRECMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UnionDesKeyDBMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UnionPKDBMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UnionMsgBufMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UnionKDBSvrMDL") == 0)
			continue;
		if (strcmp(pmdl->name,"UnionTransSpierBufMDL") == 0)
			continue;
		gunionSharedMemoryGrp.pshmPtrGrp[index] = gunionSharedMemoryGrp.pshmPtrGrp[gunionSharedMemoryGrp.num-1];
		gunionSharedMemoryGrp.num -= 1;
		UnionProgramerLog("in UnionFreeUnnecessarySharedMemory:: [%s] [%x] freed!\n",pmdl->name,pmdl);
		shmdt(pmdl);
		pmdl = NULL;
		return;
	}
#endif		
	return;
}
	
PUnionSharedMemoryModule UnionConnectExistingSharedMemoryModule(char *mdlName,long sizeOfUserSpace)
{
	int				ret;
	TUnionSharedMemoryModule	mdl;
	int				resID;
	PUnionSharedMemoryModule	pmdl;
	int				i;

	// 20071026增加	
	if (UnionIsSharedMemoryGroupIsFull())
	{
		UnionFreeUnnecessarySharedMemory();
		if (UnionIsSharedMemoryGroupIsFull())
		{
			UnionUserErrLog("in UnionConnectExistingSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026增加结束

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
		UnionUserErrLog("in UnionConnectExistingSharedMemoryModule:: sizeOfUserSpace = [%d] Error!\n",sizeOfUserSpace);
		return(NULL);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule) + sizeOfUserSpace,0666)) == -1)
	{
		UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmget [%d] [%ld]!\n",mdl.id,sizeOfUserSpace);
		return(NULL);
	}
		
	//UnionLog("in UnionConnectExistingSharedMemoryModule:: shmget [%d] OK newCreated = [%d]!\n",mdl.id,mdl.newCreated);
	
	// 2007/10/26修改以下代码
	if (((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || ((int)pmdl == -1))
	{
		if (errno == 24)
		{
			UnionFreeUnnecessarySharedMemory();
			pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || ((int)pmdl == -1))
		{
			UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
			return(NULL);
		}
	}
	// 2007/10/26修改结束

	
	++(pmdl->users);
	pmdl->newCreated = 0;
	
	if (sizeOfUserSpace == 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));

	UnionAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 增加

	return(pmdl);
}

int UnionGetNameOfSharedMemoryModuleTBL(char *fileName)
{
#ifdef _WIN32
        char    mainDir[512+1];
        memset(mainDir,0,sizeof(mainDir));
        UnionGetMainWorkingDir(mainDir);
	sprintf(fileName,"%s/unionSharedMemoryMDLTBL.CFG",mainDir);
#else
	sprintf(fileName,"%s/unionSharedMemoryMDLTBL.CFG",getenv("UNIONETC"));
#endif
	return(0);
}

// 2008/12/16,读取用户的共享内存id号
int UnionReadUserIDOfSharedMemoryModule(char *mdlName)
{
	TUnionSharedMemoryModule	mdl;
	int				ret;
	
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UnionReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		UnionUserErrLog("in UnionReadUserIDOfSharedMemoryModule:: UnionReadSharedMemoryModuleDef!\n");
		return(ret);
	}
	return(mdl.id);
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
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: mdlName [%s] longer than expected [%d]!\n",mdlName,sizeof(pmdl->name)-1);
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
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: reloadCommand [%s] longer than expected [%d]!\n",p,sizeof(pmdl->reloadCommand)-1);
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
	fprintf(fp,"[userSpaceAddress]	[%0x]\n",pmdl->puserSpace);
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
	char			tmpBuf[100];
	char			*ptr;

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
		// 2010/1/21,wolfgang wang
		sprintf(tmpBuf,"reservedSHMID%d",id);
		if ((ptr=getenv(tmpBuf)) != NULL)
		{
			if (atoi(ptr) > 0)
				continue;
		}
		// 2010/1/21, end of addition
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
#ifndef _WIN32
		if ((ret = UnionRemoveIndexStatusTBL(id)) < 0)
		{
			UnionUserErrLog("in UnionRemoveAllSharedMemoryModule:: UnionRemoveIndexStatusTBL id = [%d]\n",id);
		}
#endif
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
	int				i;
	unsigned char 			*pmUserSpace = NULL;
	
	// 20071026增加	
	if (UnionIsSharedMemoryGroupIsFull())
	{
		UnionFreeUnnecessarySharedMemory();
		if (UnionIsSharedMemoryGroupIsFull())
		{
			UnionUserErrLog("in UnionConnectSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026增加结束
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
		UnionUserErrLog("in UnionConnectSharedMemoryModule:: sizeOfUserSpace = [%d] Error!\n",sizeOfUserSpace);
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
	
	// 2007/10/26修改以下代码
	if (((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || ((int)pmdl == -1))
	{
		if (errno == 24)
		{
			UnionFreeUnnecessarySharedMemory();
			pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || ((int)pmdl == -1))
		{
			UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
			return(NULL);
		}
	}
	// 2007/10/26修改结束
	
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
	
	UnionAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 增加
		
	return(pmdl);
}
	
int UnionDisconnectShareModule(PUnionSharedMemoryModule pmdl)
{
	if ((pmdl == NULL) || ((int)pmdl == -1))
		return(0);
/**del by xusj 20091225 for core**
	if (pmdl->users >= 0)
		--pmdl->users;
**del by xusj end**/
	UnionDeleteFromSharedMemoryGroup(pmdl);	// 2007/10/26 增加
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
#ifndef _WIN32
			usleep(1000);
#else
                        Sleep(1000);
#endif
			continue;
		}
	}
	
	return(errCodeSharedMemoryMDL_LockMDL);
}

int UnionReleaseWritingLocks(PUnionSharedMemoryModule pmdl)
{
	int	i;

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
#ifndef _WIN32
			usleep(1000);
#else
                        Sleep(1000);
#endif
			continue;
		}
	}
	
	return(errCodeSharedMemoryMDL_LockMDL);
}

int UnionReleaseReadingLocks(PUnionSharedMemoryModule pmdl)
{
	int	i;

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
	struct shmid_ds		buf;
	int 			resID;
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
// 2006/12/27 增加以下函数
PUnionSharedMemoryModule UnionConnectExistedSharedMemoryModule(char *mdlName)
{
	int				ret;
	TUnionSharedMemoryModule	mdl;
	int				resID;
	PUnionSharedMemoryModule	pmdl;
	int				i;
	
	// 20071026增加	
	if (UnionIsSharedMemoryGroupIsFull())
	{
		UnionFreeUnnecessarySharedMemory();
		if (UnionIsSharedMemoryGroupIsFull())
		{
			UnionUserErrLog("in UnionConnectSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026增加结束

	memset(&mdl,0,sizeof(mdl));
	if ((ret = UnionReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		UnionUserErrLog("in UnionConnectExistedSharedMemoryModule:: UnionReadSharedMemoryModuleDef [%s]!\n",mdlName);
		return(NULL);
	}
	
	if (mdl.id <= 0)
	{
		UnionUserErrLog("in UnionConnectExistedSharedMemoryModule:: mdl.id = [%d] Error!\n",mdl.id);
		return(NULL);
	}
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule),0666)) == -1)
	{
		//UnionAuditLog("in UnionConnectExistedSharedMemoryModule:: shmget [%d]!\n",mdl.id);
		return(NULL);
	}
	mdl.newCreated = 0;
		
	// 2007/10/26修改以下代码
	if (((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || ((int)pmdl == -1))
	{
		if (errno == 24)
		{
			UnionFreeUnnecessarySharedMemory();
			pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || ((int)pmdl == -1))
		{
			UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
			return(NULL);
		}
	}
	// 2007/10/26修改结束
	
	if (pmdl->sizeOfUserSpace <= 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	
	UnionAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 增加

	return(pmdl);
}

// 2006/12/27 增加
int UnionPrintExistedSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp)
{
	PUnionSharedMemoryModule	pmdl;
	int				index;
	char				indexBuf[40+1];
	char				binDataBuf[100+1];
	char				ascDataBuf[100+1];
	int				offset = 0;
	
	if ((fp == NULL) || (mdlName == NULL))
		return(errCodeParameter);
	
	if ((pmdl = UnionConnectExistedSharedMemoryModule(mdlName)) == NULL)
	{
		UnionUserErrLog("in UnionPrintExistedSharedMemoryModuleToFileByModuleName:: UnionConnectExistedSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	fprintf(fp,"\n********************************************************************************\n");	
	UnionPrintSharedMemoryModuleToFile(pmdl,fp);
	
	memset(ascDataBuf,0,sizeof(ascDataBuf));
	memset(binDataBuf,0,sizeof(binDataBuf));
	sprintf(indexBuf,"%08Xh:",0);
	for (index = 0,offset = 0; index < pmdl->sizeOfUserSpace; index++)
	{
		sprintf(binDataBuf+offset*3,"%02X ",pmdl->puserSpace[index]);
		if ((pmdl->puserSpace[index] <= 0) || (pmdl->puserSpace[index] >= 128))
			ascDataBuf[offset] = '.';
		else
			ascDataBuf[offset] = pmdl->puserSpace[index];
		offset++;
		if ((index+1) % 16 == 0)
		{
			fprintf(fp,"%s %s; %s\n",indexBuf,binDataBuf,ascDataBuf);
			memset(ascDataBuf,0,sizeof(ascDataBuf));
			memset(binDataBuf,0,sizeof(binDataBuf));
			sprintf(indexBuf,"%08Xh:",index+1);
			offset = 0;
		}
	}
	if (index % 16 != 0)
	{
		for (; offset < 16; offset++)
			memcpy(binDataBuf+offset*3,"   ",3);
		fprintf(fp,"%s %s; %s\n",indexBuf,binDataBuf,ascDataBuf);
	}
	fprintf(fp,"********************************************************************************\n");	
	return(0);
}

// 2006/12/27增加
int UnionPrintAllExistedSharedMemoryModule(char *outputFile)
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i;
	FILE			*fp;
	char			mdlNameGrp[200][64+1];
	int			mdlNum;
	int			num = 0;
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionPrintAllExistedSharedMemoryModule:: UnionInitEnvi!\n");
		return(ret);
	}
	for (i = 0,mdlNum = 0; (i < UnionGetEnviVarNum()) && (mdlNum < sizeof(mdlNameGrp)); i++)
	{
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
			continue;
		memset(mdlNameGrp[mdlNum],0,sizeof(mdlNameGrp[mdlNum]));
		strcpy(mdlNameGrp[mdlNum],p);
		mdlNum++;
	}
	UnionClearEnvi();
	if (strcmp(outputFile,"stderr") == 0)
		fp = stderr;
	else if (strcmp(outputFile,"stdout") == 0)
		fp = stdout;
	else
	{
		if ((fp = fopen(outputFile,"w")) == NULL)
		{
			UnionUserErrLog("in UnionPrintAllExistedSharedMemoryModule:: UnionInitEnvi!\n");
			return(errCodeUseOSErrCode);
		}
	}
		
	for (i = 0; i < mdlNum; i++)
	{
		if ((ret = UnionPrintExistedSharedMemoryModuleToFileByModuleName(mdlNameGrp[i],fp)) < 0)
			printf("Print [%s] Failure! ret = [%d]\n",mdlNameGrp[i],ret);
		else
		{
			printf("Print [%s] OK!\n",mdlNameGrp[i]);
			num++;
		}
	}
	fclose(fp);
	return(num);
}
int UnionReadingLockModuleByModuleName(char *mdlName)
{
	PUnionSharedMemoryModule	pmdl;
	int				ret;

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UnionConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		UnionUserErrLog("in UnionReadingLockModuleByModuleName:: UnionConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	return(UnionApplyReadingLocks(pmdl));
}

int UnionWritingLockModuleByModuleName(char *mdlName)
{
	PUnionSharedMemoryModule	pmdl;
	int				ret;

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UnionConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		UnionUserErrLog("in UnionWritingLockModuleByModuleName:: UnionConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	return(UnionApplyWritingLocks(pmdl));
}


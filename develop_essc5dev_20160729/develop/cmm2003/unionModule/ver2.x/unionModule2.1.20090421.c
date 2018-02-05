//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0

/*
	��ģ�����һ����ΪunionMDLTBL.CFG���ļ������ļ��洢��$UNIONETCĿ¼��ÿһ�ж�����һ������ģ��
	��ṹ���£�
		[nameOfMDL]	[idOfMDL]	[reloadCommandOfMDL]
*/

// 2006/7/26 ��unionModule2.1.20060414�������޸�
// �޸���UnionRemoveAllSharedMemoryModule������������У�ɾ�����������Ϣ����������ͬ��ɾ����������Ӧ�Ķ���

// 2006/12/27 ��2.1.20060726������������������һ���ӡ�����ڴ�״̬�ĺ���

// 2007/10/26 ��2.1.20061227����������
// ������һ�����飬���ڱ�ʶһ���������ӵĹ����ڴ档
// ��������ڽ�������ӵĹ����ڴ泬�����������Ŀʱ���ͷŲ���Ҫ�����ӡ�

#ifndef _unionModule_2_x_above_
#define _unionModule_2_x_above_
#endif

#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif

#ifndef _unionModule_2_x_above_
#define _unionModule_2_x_above_
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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

// added 2012-06-01
#define SHARED_MEMORY_PERM_MODE 0600
// ended of 2012-06-01

// ���±���2007/12/26����
TUnionSharedMemoryGroup		gunionSharedMemoryGrp;
int				gunionIsSharedMemoryGrpInited = 0;

// 2007/10/26����
int UnionOutputAllRegisteredShareMemory(char *buf __attribute__((unused)),int sizeOfBuf __attribute__((unused)))
{
#ifdef _AIX_	
	int	index;
	int	ret;
	int	offset = 0;
	PUnionSharedMemoryModule	pmdl;

	UnionInitSharedMemoryGroup();
	
	sprintf(buf,"num=%04d|",gunionSharedMemoryGrp.num);
	offset = strlen(buf);
	for (index = 0; index < gunionSharedMemoryGrp.num; index++)
	{
		pmdl = gunionSharedMemoryGrp.pshmPtrGrp[index];
		if ((pmdl == NULL) || ((long)pmdl == -1))
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

// 2007/10/26����
int UnionIsSharedMemoryGroupInited()
{
#ifdef _AIX_
	return(gunionIsSharedMemoryGrpInited);
#else
	return(1);
#endif
}

// 2007/10/26����
void UnionInitSharedMemoryGroup()
{
#ifdef _AIX_
	int	index;
	if (gunionIsSharedMemoryGrpInited)
		return;
		
	for (index = 0; index < conMaxNumOfSharedMemoryPerProcess; index++)
		gunionSharedMemoryGrp.pshmPtrGrp[index] = NULL;
	gunionSharedMemoryGrp.num = 0;
	gunionIsSharedMemoryGrpInited = 1;
#endif
	return;
}

// 2007/10/26����
void UnionAddIntoSharedMemoryGroup(PUnionSharedMemoryModule pmdl __attribute__((unused)))
{
#ifdef _AIX_
	if ((pmdl == NULL) || ((long)pmdl == -1))
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

// 2007/10/26����
void UnionDeleteFromSharedMemoryGroup(PUnionSharedMemoryModule pmdl __attribute__((unused)))
{
#ifdef _AIX_	
	int	index;
	if ((pmdl == NULL) || ((long)pmdl == -1))
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
	UnionUserErrLog("in UnionDeleteFromSharedMemoryGroup:: shared memory [%x] not exists!\n",pmdl);
#endif
	return;
}

// 2007/10/26����
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

// 2007/10/26����
int UnionIsSharedMemoryInited(char *mdlName __attribute__((unused)))
{
#ifdef _AIX_
	int	index;
	
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

// 2007/10/26����
void UnionFreeUnnecessarySharedMemory()
{
#ifdef _AIX_	
	int				index;
	PUnionSharedMemoryModule	pmdl;

	UnionInitSharedMemoryGroup();	
	for (index = 0; index < gunionSharedMemoryGrp.num; index++)
	{
		pmdl = gunionSharedMemoryGrp.pshmPtrGrp[index];
		if ((pmdl == NULL) || ((long)pmdl == -1))
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

	// 20071026����	
	if (UnionIsSharedMemoryGroupIsFull())
	{
		UnionFreeUnnecessarySharedMemory();
		if (UnionIsSharedMemoryGroupIsFull())
		{
			UnionUserErrLog("in UnionConnectExistingSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026���ӽ���

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
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule) + sizeOfUserSpace,SHARED_MEMORY_PERM_MODE)) == -1)
	{
		UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmget [%d] [%ld]!\n",mdl.id,sizeOfUserSpace);
		return(NULL);
	}
		
	//UnionLog("in UnionConnectExistingSharedMemoryModule:: shmget [%d] OK newCreated = [%d]!\n",mdl.id,mdl.newCreated);
	
	// 2007/10/26�޸����´���
	if (((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || ((long)pmdl == -1))
	{
		if (errno == 24)
		{
			UnionFreeUnnecessarySharedMemory();
			pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || ((long)pmdl == -1))
		{
			UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
			return(NULL);
		}
	}
	// 2007/10/26�޸Ľ���

	
	++(pmdl->users);
	pmdl->newCreated = 0;
	
	if (sizeOfUserSpace == 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));

	UnionAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 ����

	return(pmdl);
}

// 2008/12/16,��ȡ�û��Ĺ����ڴ�id��
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
	// modify by hzh in 2010.1.20 pmdl->puserSpace���ܱ����������޸�
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


PUnionSharedMemoryModule UnionConnectSharedMemoryModule(char *mdlName,long sizeOfUserSpace)
{
	int				ret;
	TUnionSharedMemoryModule	mdl;
	int				resID;
	PUnionSharedMemoryModule	pmdl;
	unsigned char 			*pmUserSpace = NULL;
	
	// 20071026����	
	if (UnionIsSharedMemoryGroupIsFull())
	{
		UnionFreeUnnecessarySharedMemory();
		if (UnionIsSharedMemoryGroupIsFull())
		{
			UnionUserErrLog("in UnionConnectSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026���ӽ���
	memset(&mdl,0,sizeof(mdl));
	if ((ret = UnionReadSharedMemoryModuleDef(mdlName,&mdl)) < 0)
	{
		UnionLog("in UnionConnectSharedMemoryModule:: UnionReadSharedMemoryModuleDef [%s]!\n",mdlName);
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
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule) + sizeOfUserSpace,SHARED_MEMORY_PERM_MODE)) == -1)
	{
		if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule) + sizeOfUserSpace,IPC_CREAT|SHARED_MEMORY_PERM_MODE)) == -1)
		{
			UnionSystemErrLog("in UnionConnectSharedMemoryModule:: shmget [%d] [%ld] errno = %d!\n",mdl.id,sizeOfUserSpace, errno);
			return(NULL);
		}
		mdl.newCreated = 1;
	}
	else
		mdl.newCreated = 0;
		
	//UnionLog("in UnionConnectSharedMemoryModule:: shmget [%d] OK newCreated = [%d]!\n",mdl.id,mdl.newCreated);
	
	// 2007/10/26�޸����´���
	if (((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || ((long)pmdl == -1))
	{
		if (errno == 24)
		{
			UnionFreeUnnecessarySharedMemory();
			pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || ((long)pmdl == -1))
		{
			UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
			return(NULL);
		}
	}
	// 2007/10/26�޸Ľ���

	if (!mdl.newCreated)
	{
		++(pmdl->users);
		pmdl->newCreated = 0;
	}
	else	// �½�
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
		// pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
		pmUserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	}
	
	// modify by hzh in 2010.1.20 pmdl->puserSpace���ܱ����������޸�
	/*
	if ((pmdl->newCreated) && (pmdl->puserSpace)) 
		memset(pmdl->puserSpace,0,sizeOfUserSpace);
	*/
	if ((pmdl->newCreated) && (pmUserSpace)) 
		memset(pmUserSpace,0,sizeOfUserSpace);

	UnionAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 ����
		
	return(pmdl);
}
	
int UnionDisconnectShareModule(PUnionSharedMemoryModule pmdl)
{
	if ((pmdl == NULL) || ((long)pmdl == -1))
		return(0);
/**************20100422********
	if (pmdl->users >= 0)
		--pmdl->users;
******************************/
	UnionDeleteFromSharedMemoryGroup(pmdl);	// 2007/10/26 ����
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
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule),SHARED_MEMORY_PERM_MODE)) == -1)
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

// 2006/12/27 �������º���
PUnionSharedMemoryModule UnionConnectExistedSharedMemoryModule(char *mdlName)
{
	int				ret;
	TUnionSharedMemoryModule	mdl;
	int				resID;
	PUnionSharedMemoryModule	pmdl;
	
	// 20071026����	
	if (UnionIsSharedMemoryGroupIsFull())
	{
		UnionFreeUnnecessarySharedMemory();
		if (UnionIsSharedMemoryGroupIsFull())
		{
			UnionUserErrLog("in UnionConnectSharedMemoryModule:: shared memory group is full!\n");
			return(NULL);
		}
	}
	// 20071026���ӽ���

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
	
	if ((resID = shmget(mdl.id,sizeof(TUnionSharedMemoryModule),SHARED_MEMORY_PERM_MODE)) == -1)
	{
		//UnionAuditLog("in UnionConnectExistedSharedMemoryModule:: shmget [%d]!\n",mdl.id);
		return(NULL);
	}
	mdl.newCreated = 0;
		
	// 2007/10/26�޸����´���
	if (((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL) || ((long)pmdl == -1))
	{
		if (errno == 24)
		{
			UnionFreeUnnecessarySharedMemory();
			pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND);
		}
		if ((pmdl == NULL) || ((long)pmdl == -1))
		{
			UnionSystemErrLog("in UnionConnectExistingSharedMemoryModule:: shmat [%d]!\n",mdl.id);
			return(NULL);
		}
	}
	// 2007/10/26�޸Ľ���
	
	if (pmdl->sizeOfUserSpace <= 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	
	UnionAddIntoSharedMemoryGroup(pmdl);	// 2007/10/26 ����

	return(pmdl);
}

// 2006/12/27 ����
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


int UnionReadingLockModuleByModuleName(char *mdlName)
{
	PUnionSharedMemoryModule	pmdl;
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

	if (mdlName == NULL)
		return(errCodeParameter);
	
	if ((pmdl = UnionConnectSharedMemoryModule(mdlName,0)) == NULL)
	{
		UnionUserErrLog("in UnionWritingLockModuleByModuleName:: UnionConnectSharedMemoryModule [%s]!\n",mdlName);
		return(errCodeSharedMemoryModule);
	}
	
	return(UnionApplyWritingLocks(pmdl));
}

int UnionReadSharedMemoryModuleDef(char *mdlName,PUnionSharedMemoryModule pmdl)
{
	int	ret = 0;
	
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

	if( (ret = UnionReadSharedMemoryMDLTBLRec(mdlName, pmdl)) < 0 )
	{
		UnionLog("in UnionReadSharedMemoryModuleDef:: UnionReadSharedMemoryMDLTBLRec mdlName[%s]\n", mdlName);
		return (ret);
	}
	
	return(0);
}

int UnionRemoveAllSharedMemoryModule()
{
	struct shmid_ds	buf;
	int	resID = 0;
	int	i = 0;
	int	num = 0;
	TUnionSharedMemoryModule recGrp[128];
	int iCnt = 0;

	memset(recGrp, 0,sizeof(recGrp));
	if( (iCnt = UnionSelectAllSharedMemoryMDLTBLRec(recGrp, 128)) < 0 )
	{
		UnionUserErrLog("in UnionRemoveAllSharedMemoryModule:: UnionSelectAllSharedMemoryMDLTBLRec\n");
		return (iCnt);
	}

	for(i = 0, num = 0; i < iCnt; i++)
	{
		UnionNullLog("i = %d	idOfShareMemoryID = [%d]\n",i,recGrp[i].id);
		if ((resID = shmget(recGrp[i].id, sizeof(TUnionSharedMemoryModule), SHARED_MEMORY_PERM_MODE)) == -1)
		{
			UnionSystemErrLog("in UnionRemoveAllSharedMemoryModule:: shmget [%d]\n",recGrp[i].id);
			continue;
		}
		if (shmctl(resID, IPC_RMID, &buf) != 0)
		{
			UnionSystemErrLog("in UnionRemoveAllSharedMemoryModule:: shmctl IPC_RMID resID = [%d]\n",recGrp[i].id);
			continue;
		}
		num++;
	}
	return(num);
}

int UnionReloadAllSharedMemoryModule(char *anywayOrNot)
{
	int			i = 0;
	int			num = 0;
	char			command[256];
	int			reloadWithoutConfirm = 0;
	TUnionSharedMemoryModule rec[128];
	int iCnt = 0;

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

	memset(rec,0,sizeof(rec));
	if( (iCnt = UnionSelectAllSharedMemoryMDLTBLRec(rec, 128)) < 0 )
	{
		UnionUserErrLog("in UnionReloadAllSharedMemoryModule:: UnionSelectAllSharedMemoryMDLTBLRec\n");
		return (iCnt);
	}

	for (i = 0, num = 0; i < iCnt; i++)
	{
		if( strlen(rec[i].reloadCommand) == 0 )
			continue;
		strcpy(command, rec[i].reloadCommand);
		UnionToUpperCase(command);
		if( strcmp(command, "NULL") == 0 )
			continue;
		if(strstr(rec[i].reloadCommand, "-reload") == NULL )
		{
			if (reloadWithoutConfirm)
				sprintf(command,"%s -reloadanyway", rec[i].reloadCommand);
			else
				sprintf(command,"%s -reload", rec[i].reloadCommand);
		}
		else
			sprintf(command, "%s", rec[i].reloadCommand);
		UnionLog("in UnionReloadAllSharedMemoryModule:: command = [%s]\n", rec[i].reloadCommand);
		system(command);
		num++;
	}
	return(num);
}

int UnionPrintAllExistedSharedMemoryModule(char *outputFile)
{
	int				ret = 0;
	int				i = 0;
	FILE				*fp;
	int				num = 0;
	TUnionSharedMemoryModule	rec[128];
	int 				iCnt = 0;

	memset(rec, 0, sizeof(rec));
	if( (iCnt = UnionSelectAllSharedMemoryMDLTBLRec(rec, 128)) < 0 )
	{
		UnionUserErrLog("in UnionRemoveAllSharedMemoryModule:: UnionSelectAllSharedMemoryMDLTBLRec\n");
		return (iCnt);
	}

	if (strcmp(outputFile,"stderr") == 0)
		fp = stderr;
	else if (strcmp(outputFile,"stdout") == 0)
		fp = stdout;
	else
	{
		if ((fp = fopen(outputFile,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintAllExistedSharedMemoryModule:: fopen [%s]!\n",outputFile);
			return(errCodeUseOSErrCode);
		}
	}
		
	for (i = 0; i < iCnt; i++)
	{
		if ((ret = UnionPrintExistedSharedMemoryModuleToFileByModuleName(rec[i].name,fp)) < 0)
			printf("Print [%s] Failure! ret = [%d]\n",rec[i].name,ret);
		else
		{
			printf("Print [%s] OK!\n",rec[i].name);
			num++;
		}
	}
	fclose(fp);
	return(num);
}

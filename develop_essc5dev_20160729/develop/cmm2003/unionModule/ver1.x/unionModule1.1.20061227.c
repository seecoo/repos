//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0

/*
	本模块操作一个名为unionMDLTBL.CFG的文件，该文件存储在$UNIONETC目录，每一行定义了一个共享模块
	其结构如下：
		[nameOfMDL]	[idOfMDL]	[versionOfMDL]
*/


#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "unionModule.h"
#include "UnionEnv.h"
#include "unionErrCode.h"
#include "UnionLog.h"

// 2007/10/26增加
int UnionIsSharedMemoryInited(char *mdlName __attribute__((unused)))
{
	return(1);
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
	// 读取版本号
	if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
		goto abnormalExit;
	}
	if (strlen(p) > sizeof(pmdl->version) - 1)
	{
		UnionUserErrLog("in UnionReadSharedMemoryModuleDef:: version [%s] longer than expected [%d]!\n",p,sizeof(pmdl->version)-1);
		goto abnormalExit;
	}
	strcpy(pmdl->version,p);
	pmdl->index = i;
	pmdl->sizeOfUserSpace = 0;
	pmdl->puserSpace = NULL;
	pmdl->users = 0;
	pmdl->newCreated = 0;
	pmdl->readingLocks = 0;
	pmdl->writingLocks = 0;
	
	//UnionLog("in UnionReadSharedMemoryModuleDef:: [%s] [%d] [%s] [%d]\n",pmdl->name,pmdl->id,pmdl->version,pmdl->index);
		
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
	return(pmdl->puserSpace);
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
	fprintf(fp,"[version]		[%s]\n",pmdl->version);
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
		if (atoi(p) <= 0)
			continue;
		if ((resID = shmget(atoi(p),sizeof(TUnionSharedMemoryModule),0666)) == -1)
		{
			UnionSystemErrLog("in UnionRemoveAllSharedMemoryModule:: shmget [%d]\n",atoi(p));
			continue;
		}
	
		if (shmctl(resID,IPC_RMID,&buf) != 0)
		{
			UnionSystemErrLog("in UnionRemoveAllSharedMemoryModule:: shmctl IPC_RMID resID = [%d]\n",atoi(p));
			continue;
		}
		num++;
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
	
	if ((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL)
	{
		UnionSystemErrLog("in UnionConnectSharedMemoryModule:: shmat [%d]!\n",mdl.id);
		return(NULL);
	}

	
	if (!mdl.newCreated)
	{
		++pmdl->users;
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
	if (sizeOfUserSpace == 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	if ((pmdl->newCreated) && (pmdl->puserSpace))
		memset(pmdl->puserSpace,0,sizeOfUserSpace);
	
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

// 2006/12/27 增加以下函数
PUnionSharedMemoryModule UnionConnectExistedSharedMemoryModule(char *mdlName)
{
	int				ret;
	TUnionSharedMemoryModule	mdl;
	int				resID;
	PUnionSharedMemoryModule	pmdl;
	
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
		UnionAuditLog("in UnionConnectExistedSharedMemoryModule:: shmget [%d]!\n",mdl.id);
		return(NULL);
	}
	mdl.newCreated = 0;
		
	if ((pmdl = (PUnionSharedMemoryModule)shmat(resID,0,SHM_RND)) == NULL)
	{
		UnionSystemErrLog("in UnionConnectExistedSharedMemoryModule:: shmat [%d]!\n",mdl.id);
		return(NULL);
	}
	if (pmdl->sizeOfUserSpace <= 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	
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
	for (i = 0,mdlNum = 0; (i < UnionGetEnviVarNum()) && (mdlNum < (int)sizeof(mdlNameGrp)); i++)
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
		UnionUserErrLog("in UnionConnectExistingSharedMemoryModule:: sizeOfUserSpace = [%d] Error!\n",sizeOfUserSpace);
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

	++pmdl->users;
	pmdl->newCreated = 0;

	if (sizeOfUserSpace == 0)
		pmdl->puserSpace = NULL;
	else
		pmdl->puserSpace = (unsigned char *)((unsigned char *)pmdl + sizeof(*pmdl));
	
	return(pmdl);
}

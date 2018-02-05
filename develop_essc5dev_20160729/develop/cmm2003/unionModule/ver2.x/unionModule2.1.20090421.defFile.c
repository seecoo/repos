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


// 以下变量2007/12/26增加
extern TUnionSharedMemoryGroup	gunionSharedMemoryGrp;
extern int			gunionIsSharedMemoryGrpInited;


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

/*
功能
	读取指定名称的共享内存的配置参数
输入参数
	mdlName	共享内存名称
输出参数
	prec	共享内存记录
返回值
	>=0	成功
	<0	出错代码
*/
int UnionReadSharedMemoryMDLTBLRec(char *mdlName,PUnionSharedMemoryModule pmdl)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	int		firstMDLID = -1;
	char		tmpBuf[32+1];
	
	if ((mdlName == NULL) || (pmdl == NULL))
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: mdlName or pmdl is NULL!\n");
		return(errCodeParameter);
	}
	if (strlen(mdlName) > sizeof(pmdl->name) - 1)
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: mdlName [%s] longer than expected [%zu]!\n",mdlName,sizeof(pmdl->name)-1);
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionInitEnvi!\n");
		return(ret);
	}

	// 2012-06-01 张永定增加
	if ((p = UnionGetEnviVarByName("prefixOfSharedMemoryID")) != NULL)
	{
		//UnionLog("in UnionReadSharedMemoryMDLTBLRec:: prefixOfSharedMemoryID = [%s]\n",p);
		firstMDLID = atoi(p) % 10;
	}
	// 2012-06-01 

	if ((i = UnionGetVarIndexOfTheVarName(mdlName)) < 0)
	{
		UnionLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetVarIndexOfTheVarName for [%s]\n",mdlName);
		goto abnormalExit;
	}
	
	// 找到了该模块的定义
	memset(pmdl,0,sizeof(*pmdl));
	strcpy(pmdl->name,mdlName);
	// 读取ID号
	// 2012-06-01 张永定增加
	if (firstMDLID > 0)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d%d%02d",firstMDLID,getuid() % 100000,i+1);
		p = tmpBuf;
	}
	// 2012-06-01
	else
	{
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			goto abnormalExit;
		}
	}
	pmdl->id = atoi(p);
	// 读取自动加载命令
	if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
		goto abnormalExit;
	}
	if (strlen(p) > sizeof(pmdl->reloadCommand) - 1)
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: reloadCommand [%s] longer than expected [%zu]!\n",p,sizeof(pmdl->reloadCommand)-1);
		goto abnormalExit;
	}
	strcpy(pmdl->reloadCommand,p);
	pmdl->index = i;
	
	//UnionLog("in UnionReadSharedMemoryMDLTBLRec:: [%s] [%d] [%s] [%d]\n",pmdl->name,pmdl->id,pmdl->reloadCommand,pmdl->index);
		
	UnionClearEnvi();
	return(0);

abnormalExit:
	UnionLog("in UnionReadSharedMemoryMDLTBLRec:: [%s] not defined in file [%s]\n",mdlName,fileName);
	
	UnionClearEnvi();
	return(errCodeSharedMemoryMDL_MDLNotDefined);
}

/*
功能
	读出所有记录
输入参数
	maxNum		可以读出的最大数量
输出参数
	recGrp		读出的记录
返回值
	>=0	读出的记录数
	<0	出错代码
*/
long UnionSelectAllSharedMemoryMDLTBLRec(TUnionSharedMemoryModule recGrp[],int maxNum)
{
	int				ret;
	char				fileName[512];
	int				i;
	int				num = 0;
	char				*p;
	int				firstMDLID = -1;
	char				tmpBuf[32+1];
	PUnionSharedMemoryModule	pmdl;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionSelectAllSharedMemoryMDLTBLRec:: UnionInitEnvi!\n");
		return(ret);
	}
	
	// 2012-06-01 张永定增加
	if ((p = UnionGetEnviVarByName("prefixOfSharedMemoryID")) != NULL)
	{
		firstMDLID = atoi(p) % 10;
	}
	// 2012-06-01 
	
	for (i = 0,num = 0; (i < UnionGetEnviVarNum()) && (num < maxNum); i++)
	{
		pmdl = &(recGrp[num]);
		memset(pmdl,0,sizeof(*pmdl));
		// 读取名称
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strlen(p) >= sizeof(pmdl->name))
		{
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: name [%s] longer than expected [%zu]!\n",p,sizeof(pmdl->name));
			continue;
		}
		strcpy(pmdl->name,p);
		// 读取ID号
		// 2012-06-01 张永定增加
		if (firstMDLID > 0)
		{
			memset(tmpBuf,0,sizeof(tmpBuf));
			sprintf(tmpBuf,"%d%d%02d",firstMDLID,getuid() % 100000,num+1);
			p = tmpBuf;
		}
		// 2012-06-01
		else
		{
			if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
			{
				UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
				continue;
			}
		}
		pmdl->id = atoi(p);
		//UnionLog("in UnionReadSharedMemoryMDLTBLRec:: pmdl->id[%d]\n",pmdl->id);
		// 读取自动加载命令
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		if (strlen(p) > sizeof(pmdl->reloadCommand) - 1)
		{
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: reloadCommand [%s] longer than expected [%zu]!\n",p,sizeof(pmdl->reloadCommand)-1);
			continue;
		}
		strcpy(pmdl->reloadCommand,p);
		pmdl->index = i;
		num++;
	}

	UnionClearEnvi();
	
	return(num);
}

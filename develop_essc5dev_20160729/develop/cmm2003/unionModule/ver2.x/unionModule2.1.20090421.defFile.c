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


// ���±���2007/12/26����
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
����
	��ȡָ�����ƵĹ����ڴ�����ò���
�������
	mdlName	�����ڴ�����
�������
	prec	�����ڴ��¼
����ֵ
	>=0	�ɹ�
	<0	�������
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

	// 2012-06-01 ����������
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
	
	// �ҵ��˸�ģ��Ķ���
	memset(pmdl,0,sizeof(*pmdl));
	strcpy(pmdl->name,mdlName);
	// ��ȡID��
	// 2012-06-01 ����������
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
	// ��ȡ�Զ���������
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
����
	�������м�¼
�������
	maxNum		���Զ������������
�������
	recGrp		�����ļ�¼
����ֵ
	>=0	�����ļ�¼��
	<0	�������
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
	
	// 2012-06-01 ����������
	if ((p = UnionGetEnviVarByName("prefixOfSharedMemoryID")) != NULL)
	{
		firstMDLID = atoi(p) % 10;
	}
	// 2012-06-01 
	
	for (i = 0,num = 0; (i < UnionGetEnviVarNum()) && (num < maxNum); i++)
	{
		pmdl = &(recGrp[num]);
		memset(pmdl,0,sizeof(*pmdl));
		// ��ȡ����
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
		// ��ȡID��
		// 2012-06-01 ����������
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
		// ��ȡ�Զ���������
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

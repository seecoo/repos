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
	
	if ((mdlName == NULL) || (pmdl == NULL))
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: mdlName or pmdl is NULL!\n");
		return(errCodeParameter);
	}
	if (strlen(mdlName) > sizeof(pmdl->name) - 1)
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: mdlName [%s] longer than expected [%d]!\n",mdlName,sizeof(pmdl->name)-1);
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionInitEnvi!\n");
		return(ret);
	}

	if ((i = UnionGetVarIndexOfTheVarName(mdlName)) < 0)
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetVarIndexOfTheVarName for [%s]\n",mdlName);
		goto abnormalExit;
	}
	
	// �ҵ��˸�ģ��Ķ���
	memset(pmdl,0,sizeof(*pmdl));
	strcpy(pmdl->name,mdlName);
	// ��ȡID��
	if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
	{
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
		goto abnormalExit;
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
		UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: reloadCommand [%s] longer than expected [%d]!\n",p,sizeof(pmdl->reloadCommand)-1);
		goto abnormalExit;
	}
	strcpy(pmdl->reloadCommand,p);
	pmdl->index = i;
	
	//UnionLog("in UnionReadSharedMemoryMDLTBLRec:: [%s] [%d] [%s] [%d]\n",pmdl->name,pmdl->id,pmdl->reloadCommand,pmdl->index);
		
	UnionClearEnvi();
	return(0);

abnormalExit:
	UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: [%s] not defined in file [%s]\n",mdlName,fileName);
	
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
	PUnionSharedMemoryModule	pmdl;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionSelectAllSharedMemoryMDLTBLRec:: UnionInitEnvi!\n");
		return(ret);
	}
	
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
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: name [%s] longer than expected [%d]!\n",p,sizeof(pmdl->name));
			continue;
		}
		strcpy(pmdl->name,p);
		// ��ȡID��
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		pmdl->id = atoi(p);
		// ��ȡ�Զ���������
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) == NULL)
		{
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,2);
			continue;
		}
		if (strlen(p) > sizeof(pmdl->reloadCommand) - 1)
		{
			UnionUserErrLog("in UnionReadSharedMemoryMDLTBLRec:: reloadCommand [%s] longer than expected [%d]!\n",p,sizeof(pmdl->reloadCommand)-1);
			continue;
		}
		strcpy(pmdl->reloadCommand,p);
		pmdl->index = i;
		num++;
	}

	UnionClearEnvi();
	
	return(num);
}

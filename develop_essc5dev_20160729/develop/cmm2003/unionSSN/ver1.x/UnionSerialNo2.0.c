//	Author:		Wolfgang Wang
//	Date:		2002/8/17

// 2003/08/11，Wolfgang Wang 升级为2.0
// 1.0，直接保持内存中的数值和文件中的数值同步，2.0改为滞后同步。

#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "UnionSerialNo.h"
#include "UnionLog.h"

PUnionSerialNoMDL UnionConnectSerialNoMDL(int IDOfSerialNoMDL,long LargestSN,long LowestSN)
{
	int 			ResID;
	int			NewCreated = 0;
	PUnionSerialNoMDL	pSerialNoMDL;
	
	if ((ResID = shmget(IDOfSerialNoMDL,sizeof(TUnionSerialNoMDL),0666)) == -1)
	{
		if ((ResID = shmget(IDOfSerialNoMDL,sizeof(TUnionSerialNoMDL),IPC_CREAT|0666)) == -1)
		{
			UnionSystemErrLog("in UnionConnectSerialNoMDL:: shmget!\n");
			return(NULL);
		}
		NewCreated = 1;
	}

	if ((pSerialNoMDL = (PUnionSerialNoMDL)shmat(ResID,0,SHM_RND)) == NULL)
	{
		UnionSystemErrLog("in UnionConnectSerialNoMDL:: shmat!\n");
		return(NULL);
	}
	
	if (!NewCreated)
		return(pSerialNoMDL);

	pSerialNoMDL->Locked = 0;
	pSerialNoMDL->IDOfSerialNoMDL = IDOfSerialNoMDL;
	pSerialNoMDL->IDOfSharedMemory = ResID;
	pSerialNoMDL->LargestSN = LargestSN;
	pSerialNoMDL->LowestSN = LowestSN;
	UnionGetSerialNoFromMirrorOfSerialNoMDL(pSerialNoMDL);
	
	return(pSerialNoMDL);
}

int UnionDisconnectSerialNoMDL(PUnionSerialNoMDL pSerialNoMDL)
{
	if (pSerialNoMDL != NULL)
	{
		if (shmdt(pSerialNoMDL) != 0)
		{
			UnionSystemErrLog("in UnionDisonnectTransSSNRes:: shmdt!");
			return(-1);
		}
		else
			return(0);
	}
	else
		return(-1);
}

int UnionRemoveSerialNoMDL(int IDOfSerialNoMDL)
{
	struct shmid_ds		buf;
	int 			ResID;
	
	if ((ResID = shmget(IDOfSerialNoMDL,sizeof(TUnionSerialNoMDL),0666)) == -1)
	{
		UnionSystemErrLog("in UnionRemoveSerialNoMDL:: shmget [%d]\n",IDOfSerialNoMDL);
		return(-1);
	}
	if (shmctl(ResID,IPC_RMID,&buf) != 0)
	{
		UnionSystemErrLog("in UnionRemoveSerialNoMDL:: shmctl IPC_RMID ResID = [%d]\n",ResID);
		return(-1);
	}
	
	return(0);
}

int UnionPrintStatusOfSerialNoMDL(PUnionSerialNoMDL pSerialNoMDL)
{
	if (pSerialNoMDL == NULL)
	{
		printf("SerialNoMDL [%d] is not initialized!\n");
		return(-1);
	}
	
	printf("Status Of SerialNoMDL::\n");
	printf("[IDOfSerialNoMDL]			[%d]\n",pSerialNoMDL->IDOfSerialNoMDL);
	printf("[IDOfSharedMemory]			[%d]\n",pSerialNoMDL->IDOfSharedMemory);
	printf("[SerialNo]				[%ld]\n",pSerialNoMDL->SerialNo);
	printf("[Locked]				[%d]\n",pSerialNoMDL->Locked);
	printf("[LargestSN]				[%ld]\n",pSerialNoMDL->LargestSN);
	printf("[LowestSN]				[%ld]\n",pSerialNoMDL->LowestSN);

	printf("\n");
	
	return(0);
}

long UnionApplyNewSerialNo(PUnionSerialNoMDL pSerialNoMDL)
{
	int		i;
	long		SerialNo;

	if (pSerialNoMDL == NULL)
	{
		UnionUserErrLog("in UnionApplyNewSerialNo:: SerialNoMDL is not initialized!\n");
		return(-1);
	}
	
	for (i = 0; i < 100; i++)
	{
		if (!(pSerialNoMDL->Locked))
			pSerialNoMDL->Locked = 1;
		else
		{
			usleep(10);
			continue;
		}
		SerialNo = pSerialNoMDL->SerialNo;
		pSerialNoMDL->SerialNo += 1;
		if (pSerialNoMDL->SerialNo > pSerialNoMDL->LargestSN)
			pSerialNoMDL->SerialNo = pSerialNoMDL->LowestSN;
		/*if (UnionRefreshMirrorOfSerialNoMDL(pSerialNoMDL) != 0)
		{
			UnionUserErrLog("in UnionApplyNewSerialNo:: UnionRefreshMirrorOfSerialNoMDL!\n");
		}
		*/
		pSerialNoMDL->Locked = 0;
		return(SerialNo);
	}
	UnionUserErrLog("in UnionApplyNewSerialNo:: System is Busy!\n");
	return(-1);
}

long UnionReadCurrentSerialNo(PUnionSerialNoMDL pSerialNoMDL)
{
	int		i;
	long		SerialNo;

	if (pSerialNoMDL == NULL)
	{
		UnionUserErrLog("in UnionReadCurrentSerialNo:: SerialNoMDL is not initialized!\n");
		return(-1);
	}
	
	for (i = 0; i < 100; i++)
	{
		if (!(pSerialNoMDL->Locked))
			pSerialNoMDL->Locked = 1;
		else
		{
			usleep(10);
			continue;
		}
		SerialNo = pSerialNoMDL->SerialNo;
		pSerialNoMDL->Locked = 0;
		return(SerialNo);
	}
	UnionUserErrLog("in UnionReadCurrentSerialNo:: System is Busy!\n");
	return(-1);
}

int UnionUnlockSerialNoMDL(PUnionSerialNoMDL pSerialNoMDL)
{
	if (pSerialNoMDL == NULL)
	{
		UnionUserErrLog("in UnionUnlockSerialNoMDL:: SerialNoMDL is not initialized!\n");
		return(-1);
	}

	if (pSerialNoMDL->Locked)
		pSerialNoMDL->Locked = 0;

	return(0);
}

int UnionLockSerialNoMDL(PUnionSerialNoMDL pSerialNoMDL)
{
	if (pSerialNoMDL == NULL)
	{
		UnionUserErrLog("in UnionLockSerialNoMDL:: SerialNoMDL is not initialized!\n");
		return(-1);
	}

	if (!pSerialNoMDL->Locked)
		pSerialNoMDL->Locked = 1;

	return(0);
}


int UnionGetNameOfMirrorFileOfSerialNoMDL(PUnionSerialNoMDL pSerialNoMDL,char *NameOfMirrorFileOfSerialNoMDL)
{
	sprintf(NameOfMirrorFileOfSerialNoMDL,"%s/etc/USNM%d.Def",getenv("HOME"),pSerialNoMDL->IDOfSerialNoMDL);
	return(0);
}

int UnionGetSerialNoFromMirrorOfSerialNoMDL(PUnionSerialNoMDL pSerialNoMDL)
{
	char	NameOfMirrorFileOfSerialNoMDL[256];
	FILE	*Fp;
	int	i;
	long	SerialNo;
	int	MirrorFileNonExist = 0;
	
	if (pSerialNoMDL == NULL)
	{
		UnionUserErrLog("in UnionGetSerialNoFromMirrorOfSerialNoMDL:: SerialNoMDL is not initialized!\n");
		return(-1);
	}

	memset(NameOfMirrorFileOfSerialNoMDL,0,sizeof(NameOfMirrorFileOfSerialNoMDL));
	UnionGetNameOfMirrorFileOfSerialNoMDL(pSerialNoMDL,NameOfMirrorFileOfSerialNoMDL);
	if ((Fp = fopen(NameOfMirrorFileOfSerialNoMDL,"r")) == NULL)
	{
		UnionSystemErrLog("UnionGetSerialNoFromMirrorOfSerialNoMDL:: fopen [%s]!\n",NameOfMirrorFileOfSerialNoMDL);
		SerialNo = pSerialNoMDL->LowestSN;
		MirrorFileNonExist = 1;
	}
	else
	{
		fscanf(Fp,"%ld",&SerialNo);
		fclose(Fp);
	}
	
	for (i = 0; i < 100; i++)
	{
		if (pSerialNoMDL->Locked)
		{
			usleep(10);
			continue;
		}
		else
		{
			pSerialNoMDL->Locked = 1;
			pSerialNoMDL->SerialNo = SerialNo;
			pSerialNoMDL->Locked = 0;
			break;
		}
	}
	if (MirrorFileNonExist)
	{
		if (UnionRefreshMirrorOfSerialNoMDL(pSerialNoMDL) < 0)
		{
			UnionUserErrLog("in UnionGetSerialNoFromMirrorOfSerialNoMDL:: UnionRefreshMirrorOfSerialNoMDL!\n");
		}
		else
			UnionSuccessLog("in UnionGetSerialNoFromMirrorOfSerialNoMDL:: Create MirrorFile [%s] OK!\n",NameOfMirrorFileOfSerialNoMDL);
	}
	if (i == 100)
		return(-1);
	else
		return(0);
}

int UnionRefreshMirrorOfSerialNoMDL(PUnionSerialNoMDL pSerialNoMDL)
{
	char	NameOfMirrorFileOfSerialNoMDL[256];
	FILE	*Fp;
	
	if (pSerialNoMDL == NULL)
	{
		UnionUserErrLog("in UnionRefreshMirrorOfSerialNoMDL:: SerialNoMDL is not initialized!\n");
		return(-1);
	}

	memset(NameOfMirrorFileOfSerialNoMDL,0,sizeof(NameOfMirrorFileOfSerialNoMDL));
	UnionGetNameOfMirrorFileOfSerialNoMDL(pSerialNoMDL,NameOfMirrorFileOfSerialNoMDL);
	if ((Fp = fopen(NameOfMirrorFileOfSerialNoMDL,"w")) == NULL)
	{
		UnionSystemErrLog("UnionRefreshMirrorOfSerialNoMDL:: fopen [%s]!\n",NameOfMirrorFileOfSerialNoMDL);
		return(-1);
	}
	fprintf(Fp,"%ld",pSerialNoMDL->SerialNo);
	fclose(Fp);
	
	return(0);
}


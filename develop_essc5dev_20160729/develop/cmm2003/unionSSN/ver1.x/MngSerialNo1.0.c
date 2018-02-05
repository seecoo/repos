//	Author:		Wolfgang Wang
//	Date:		2002/8/17

#include <stdio.h>
#include <string.h>

#include "UnionSerialNo.h"

PUnionSerialNoMDL pSerialNoMDL;

int UnionHelp()
{
	printf("Usage:: MngSerialNo1.x IDOfSerialNoMDL\n");
	return(0);
}

int main(int argc,char **argv)
{
	long		SerialNo;
	int		Choice;
	int		Ret;
	long		LargestSN = 100000000;
	long		LowestSN = 100;
	long		TmpSN;
	int		Confirm;
	
	if (argc < 2)
		return(UnionHelp());

	if ((pSerialNoMDL = UnionConnectSerialNoMDL(atoi(argv[1]),LargestSN,LowestSN)) == NULL)
	{
		printf("UnionConnectSerialNoMDL Failure!\n");
		return(-1);
	}
	
	for (;;)
	{
		printf("\n\n");
		printf("1. Apply a New SerialNo\n");
		printf("2. Lock SerialNoMDL\n");
		printf("3. Unlock SerialNoMDL\n");
		printf("4. Print Status of SerialNoMDL\n");
		printf("0. Exit\n");
		printf("100. Remove SerialNoMDL and Exit\n");
		scanf("%d",&Choice);
		switch (Choice)
		{
			case 100:
				printf("Sure? If yes, please reinput 100 once again, otherwise any other digits.");
				scanf("%d",&Confirm);
				if (Confirm == 100)
					goto RemoveExit;
				else
					continue;
			case 0:
				goto MainExit;
			case 1:
				if ((SerialNo = UnionApplyNewSerialNo(pSerialNoMDL)) < 0)
					printf("UnionApplyNewSerialNo Failure!\n");
				else
					printf("New SerialNo = [%ld]\n",SerialNo);
				break;
			case 2:
				if ((Ret = UnionLockSerialNoMDL(pSerialNoMDL)) < 0)
					printf("UnionLockSerialNoMDL Failure!\n");
				else
					printf("UnionLockSerialNoMDL OK\n");
				break;
			case 3:
				if ((Ret = UnionUnlockSerialNoMDL(pSerialNoMDL)) < 0)
					printf("UnionUnlockSerialNoMDL Failure!\n");
				else
					printf("UnionUnlockSerialNoMDL OK\n");
				break;
			case 4:
				if ((Ret = UnionPrintStatusOfSerialNoMDL(pSerialNoMDL)) < 0)
					printf("UnionPrintStatusOfSerialNoMDL Failure!\n");
				else
					printf("UnionPrintStatusOfSerialNoMDL OK\n");
				break;
			default:
				break;
		}
	}
				
MainExit:
	if ((Ret = UnionDisconnectSerialNoMDL(pSerialNoMDL)) < 0)
		printf("UnionDisconnectSerialNoMDL Failure!\n");
	else
		printf("UnionDisconnectSerialNoMDL OK!\n");
	
	return(Ret);

RemoveExit:
	if ((Ret = UnionRemoveSerialNoMDL(atoi(argv[1]))) < 0)
		printf("UnionRemoveSerialNoMDL Failure!\n");
	else
		printf("UnionRemoveSerialNoMDL OK!\n");
	
	return(Ret);
}
		
int UnionIsDebug()
{
	return(1);
}

int UnionGetSizeOfLogFile()
{
	return(10000000);
}

int UnionGetNameOfLogFile(char *NameOfLogFile)
{
	sprintf(NameOfLogFile,"%s/log/MngSerialNo1.x.log",getenv("HOME"));
	return(0);
}
	

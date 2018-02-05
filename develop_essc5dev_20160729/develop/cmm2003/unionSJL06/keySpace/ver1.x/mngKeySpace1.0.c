//	Wolfgang Wang
//	2003/09/09
// 	Version	1.0

#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "sjl06KeySpace.h"
#include "UnionStr.h"
#include "unionCommand.h"
#include "sjl06LMK.h"

TUnionKeyPosStatus	gkeyPosStatus;
PUnionSJL06KeyPosSpace	pgKeyPosSpace = NULL;
char			ghsmGrp[50];
char			gname[50];

#include "UnionTask.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectKeyPosSpace(pgKeyPosSpace);
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s keySpaceName\n",UnionGetApplicationName());
	printf(" Command\n");
	printf("	create hsmGrpID spaceName keyPosNum 	Create KeyPosSpace\n");
	printf("	drop hsmGrpID spaceName ...		Drop KeyPosSpace\n");
	
	printf("	read hsmGrpID spaceName keyPos ...	Read status of keyPos\n");
	printf("	available hsmGrpID spaceName		Read all available keyPos\n");
	printf("	occupied hsmGrpID spaceName		Read all occupied keyPos\n");
	printf("	allinvalid hsmGrpID spaceName		Print all invalid keyPos\n");
	printf("	puser hsmGrpID spaceName userName	Print all occupieed keyPos by user\n");
	
	printf("	selectavailable hsmGrpID spaceName	Read the minimum available keyPos\n");
	printf("	isavailable     hsmGrpID spaceName	decide whether a keyPos is available\n");
	
	printf("	occupy hsmGrpID spaceName keyPos userName ...	Set keyPos occupied\n");
	printf("	free hsmGrpID spaceName keyPos ...	Set keyPos available\n");
	printf("	freeUser hsmGrpID spaceName userName	Free all keyPos occupied by User\n");
	printf("	invalid hsmGrpID spaceName startKeyPos [endKeyPos] Set keyPos invalid\n");
	printf("	initickeyspace hsmGrp spaceName versionNum groupNum indexNum\n");
	
	printf("	applyalllmkkeyspace hsmGrp		Set LMK KeySpace for hsmGrp\n");
	
	printf("	status hsmGrp spaceName\n");
	return(0);
}

int main(int argc,char **argv)
{
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngKeySpace")) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (strcasecmp(argv[1],"CREATE") == 0)
		return(CreateKeyPosSpace(argc-2,&argv[2]));
	if (strcasecmp(argv[1],"DROP") == 0)
		return(DropKeyPosSpace(argc-2,&argv[2]));
	
	memset(ghsmGrp,0,sizeof(ghsmGrp));
	
	if (argc < 3)
	{
loopInputHsmGrpID:
		strcpy(ghsmGrp,UnionInput("Input hsmGrpID or exit/quit to exit(%d digits)::",3));
		if (UnionIsQuit(ghsmGrp))
			return(UnionTaskActionBeforeExit());
	}
	else
		strcpy(ghsmGrp,argv[2]);
	
	if (argc < 4)
	{
		if (strcasecmp(argv[1],"APPLYALLLMKKEYSPACE") == 0)
			strcpy(gname,"JK");
		else
		{
loopInputKeySpaceName:
			strcpy(gname,UnionInput("Input KeySpaceName(JK/IC/RSA/Quit/Exit)::"));
			if (UnionIsQuit(gname))
				return(UnionTaskActionBeforeExit());
		}
	}
	else
		strcpy(gname,argv[3]);
	
	if ((pgKeyPosSpace = UnionConnectKeyPosSpace(ghsmGrp,gname)) == NULL)
	{
		printf("UnionConnectKeyPosSpace [%s] [%s] Error!\n",ghsmGrp,gname);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"READ") == 0)
		return(ReadKeyPos(argc-4,&argv[4]));
	if (strcasecmp(argv[1],"AVAILABLE") == 0)
	{
		UnionPrintAllAvailableKeyPos(pgKeyPosSpace);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"OCCUPIED") == 0)
	{
		UnionPrintAllOccupiedKeyPos(pgKeyPosSpace);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"ALLINVALID") == 0)
	{
		UnionPrintAllInvalidKeyPos(pgKeyPosSpace);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"SELECTAVAILABLE") == 0)
	{
		printf("Current Available KeyPos = [%06ld]\n",UnionSelectAvailableKeyPos(pgKeyPosSpace));
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"ISAVAILABLE") == 0)
		return(IsAvailableKeyPos(argc-4,&argv[4]));
		
	if (strcasecmp(argv[1],"OCCUPY") == 0)
		return(OccupyKeyPos(argc-4,&argv[4]));
		
	if (strcasecmp(argv[1],"FREE") == 0)
		return(FreeKeyPos(argc-4,&argv[4]));
		
	if (strcasecmp(argv[1],"INVALID") == 0)
		return(InvalidKeyPos(argc-4,&argv[4]));
	
	if (strcasecmp(argv[1],"INITICKEYSPACE") == 0)
		return(InitICKeySpace(argc-4,&argv[4]));
	
	if (strcasecmp(argv[1],"PUSER") == 0)
		return(PrintOccupiedByUser(argc-4,&argv[4]));
	
	if (strcasecmp(argv[1],"FREEUSER") == 0)
		return(FreeOccupiedByUser(argc-4,&argv[4]));
		
	if (strcasecmp(argv[1],"STATUS") == 0)
	{
		UnionPrintStatusOfKeyPosSpaceToFile(pgKeyPosSpace,stdout);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"APPLYALLLMKKEYSPACE") == 0)
		return(OccupyKeySpaceForAllLMK(ghsmGrp));
	
	UnionHelp();
	
	return(UnionTaskActionBeforeExit());
}

//Create
int CreateKeyPosSpace(int argc,char *argv[])
{
	char	*p;
	int	ret;
	long	keyPosNum = 0;
	char	tmpBuf[100];

	memset(ghsmGrp,0,sizeof(ghsmGrp));
	if (argc > 0)
	{
		strcpy(ghsmGrp,argv[0]);
	}
	else
		goto loopCreate;
	memset(gname,0,sizeof(gname));
	if (argc > 1)
	{
		strcpy(gname,argv[1]);
	}
	else
	{
		if (UnionIsQuit(p = UnionInput("Input KeySpaceName (JK/IC/RSA/Exit)::")))
			return(UnionTaskActionBeforeExit());
		strcpy(gname,p);
	}
	if (argc > 2)
		keyPosNum = atol(argv[2]);
	else
	{
		if (UnionIsQuit(p = UnionInput("Input keyPosNum (Exit to exit)::")))
			return(UnionTaskActionBeforeExit());
		keyPosNum = atol(p);
	}
	if (argc > 0)
		goto createKeyPosSpace;
		
loopCreate:
	p = UnionInput("CreateSpace(hsmGrpID/JK/RSA/IC/keyPosNum/create/exit)>");
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,p);
	
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	if (strcasecmp("KEYPOSNUM",p) == 0)
		goto loopInputKeyPosNum;
	if ((strcasecmp(p,conSJL06JKKeySpaceName) == 0) || (strcasecmp(p,conSJL06ICKeySpaceName) == 0) || (strcasecmp(p,conSJL06RSAKeySpaceName) == 0))
	{
		memset(gname,0,sizeof(gname));
		strcpy(gname,p);
		goto loopCreate;
	}
	if (strcasecmp(p,"CREATE") == 0)
		goto createKeyPosSpace;
	memset(ghsmGrp,0,sizeof(ghsmGrp));
	strcpy(ghsmGrp,p);
	printf("\nhsmGrpID	[%s]\n",ghsmGrp);
	printf("keySpaceName	[%s]\n",gname);
	printf("keyPosNum	[%ld]\n",keyPosNum);
	goto loopCreate;
		
loopInputKeyPosNum:
	if ((keyPosNum = atol(UnionInput("Input keyPosNum::"))) <= 0)
	{
		printf("Invalid keyPosNum!\n");
		goto loopInputKeyPosNum;
	}
	goto loopCreate;

createKeyPosSpace:
	if (!UnionConfirm("Are you sure of create a keyPosSpace named [%s.%s] and keyPosNum [%06ld] ",ghsmGrp,gname,keyPosNum))
	{
		if (argc > 0)
			return(UnionTaskActionBeforeExit());
		goto loopCreate;
	}
	if ((ret = UnionCreateKeyPosSpace(ghsmGrp,gname,keyPosNum)) < 0)
		printf("UnionCreateKeyPosSpace Error! ret = [%d]\n",ret);
	else
		printf("UnionCreateKeyPosSpace OK!\n");
	
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

	goto loopCreate;
}

// Drop
int DropKeyPosSpace(int argc,char *argv[])
{
	int	ret;
	int	i;
	char	*p;
	
	memset(ghsmGrp,0,sizeof(ghsmGrp));
	if (argc > 0)
	{
		strcpy(ghsmGrp,argv[0]);
	}
	else
		goto loopDrop;
	memset(gname,0,sizeof(gname));
	if (argc > 1)
	{
		strcpy(gname,argv[1]);
	}
	else
	{
		if (UnionIsQuit(p = UnionInput("Input KeySpaceName (JK/IC/RSA/Exit)::")))
			return(UnionTaskActionBeforeExit());
		strcpy(gname,p);
	}
	goto dropKeySpace;

loopDrop:
	if (UnionIsQuit(p = UnionInput("DropSpace (hsmGrp/JK/IC/RSA/Drop/Exit)>")))
		return(UnionTaskActionBeforeExit());
	if ((strcasecmp(p,conSJL06JKKeySpaceName) == 0) || (strcasecmp(p,conSJL06ICKeySpaceName) == 0) || (strcasecmp(p,conSJL06RSAKeySpaceName) == 0))
	{
		memset(gname,0,sizeof(gname));
		strcpy(gname,p);
		goto loopDrop;
	}
	if (strcasecmp(p,"DROP") == 0)
		goto dropKeySpace;
	memset(ghsmGrp,0,sizeof(ghsmGrp));
	strcpy(ghsmGrp,p);
	printf("\nhsmGrpID	[%s]\n",ghsmGrp);
	printf("keySpaceName	[%s]\n",gname);
	goto loopDrop;

dropKeySpace:
	if (UnionConfirm("Remove the keyPosSpace [%s.%s]?",ghsmGrp,gname))
	{
		if ((ret = UnionDeleteKeyPosSpace(ghsmGrp,gname)) < 0)
			printf("UnionDeleteKeyPosSpace Error! ret = [%d]\n",ret);
		else
			printf("UnionDeleteKeyPosSpace OK!\n");
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
	goto loopDrop;
}

// Read
int ReadKeyPos(int argc,char *argv[])
{
	int	ret;
	long	min,max,i;
	char	*p;
	
	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionPrintKeyPosStatus(pgKeyPosSpace,atol(argv[i]))) < 0)
			printf("UnionPrintKeyPosStatus [%06ld] Error! ret = [%d]\n",atol(argv[i]),ret);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
				
	if (UnionIsQuit(p=UnionInput("start keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	min = atol(p);
	if (UnionIsQuit(p=UnionInput("end keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	max = atol(p);
	
	for (i = min; i <= max; i++)
		if ((ret = UnionPrintKeyPosStatus(pgKeyPosSpace,i)) < 0)
			printf("UnionPrintKeyPosStatus [%06ld] Error! ret = [%d]\n",i,ret);
	
	return(UnionTaskActionBeforeExit());
}		

// IsAvailable
int IsAvailableKeyPos(int argc,char *argv[])
{
	int	ret;
	long	min,max,i;
	char	*p;

	for (i = 0; i < argc; i++)
	{
		if (UnionIsKeyPosAvailable(pgKeyPosSpace,atol(argv[i])))
			printf("[%0l6d] available.\n",atol(argv[i]));
		else
			printf("[%0l6d] inavailable.\n",atol(argv[i]));
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
				
	if (UnionIsQuit(p=UnionInput("start keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	min = atol(p);
	if (UnionIsQuit(p=UnionInput("end keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	max = atol(p);
	
	
	for (i = min; i <= max; i++)
		if (UnionIsKeyPosAvailable(pgKeyPosSpace,i))
			printf("[%0l6d] available.\n",i);
		else
			printf("[%0l6d] inavailable.\n",i);

	return(UnionTaskActionBeforeExit());
}	

// Occupy
int OccupyKeyPos(int argc,char *argv[])
{
	int	ret;
	long	min,max,i;
	char	userName[80+1];
	char	*p;
	
	for (i = 0; i < argc; i = i+2)
	{
		memset(userName,0,sizeof(userName));
		if (i + 1 >= argc)
			strcpy(userName,UnionInput("Input Username who occupy this pos [%ld]::",atol(argv[i])));
		else
			strcpy(userName,argv[i+1]);
		if ((ret = UnionUseKeyPos(pgKeyPosSpace,atol(argv[i]),userName)) < 0)
			printf("UnionUseKeyPos [%06ld] Error! ret = [%d]\n",atol(argv[i]),ret);
		else
			printf("UnionUseKeyPos [%06ld] for [%s] OK\n",atol(argv[i]),userName);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
				
	if (UnionIsQuit(p=UnionInput("start keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	min = atol(p);
	if (UnionIsQuit(p=UnionInput("end keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	max = atol(p);
	
	memset(userName,0,sizeof(userName));
	strcpy(userName,p = UnionInput("Input Username who occupy these poses or exit/quit to exit::"));
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	
	for (i = min; i <= max; i++)
	{
		if ((ret = UnionUseKeyPos(pgKeyPosSpace,i,userName)) < 0)
			printf("UnionUseKeyPos [%06ld] Error! ret = [%d]\n",i,ret);
		else
			printf("UnionUseKeyPos [%06ld] for [%s] OK\n",i,userName);
	}
	
	return(UnionTaskActionBeforeExit());
}		

// Free
int FreeKeyPos(int argc,char *argv[])
{
	int	ret;
	long	min,max,i;
	char	*p;

	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionUnuseKeyPos(pgKeyPosSpace,atol(argv[i]))) < 0)
			printf("UnionUnuseKeyPos [%06ld] Error! ret = [%d]\n",atol(argv[i]),ret);
		else
			printf("UnionUnuseKeyPos [%06ld] OK!\n",atol(argv[i]));
		
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
				
	if (UnionIsQuit(p=UnionInput("start keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	min = atol(p);
	if (UnionIsQuit(p=UnionInput("end keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	max = atol(p);
		
	for (i = min; i <= max; i++)
	{
		if ((ret = UnionUnuseKeyPos(pgKeyPosSpace,i)) < 0)
			printf("UnionUnuseKeyPos [%06ld] Error! ret = [%d]\n",i,ret);
		else
			printf("UnionUnuseKeyPos [%06ld] OK!\n",i);
	}
	return(UnionTaskActionBeforeExit());
}

int InvalidKeyPos(int argc,char *argv[])
{
	int	ret;
	long	min,max,i;
	char	*p;
	
	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionSetKeyPosInvalid(pgKeyPosSpace,atol(argv[i]))) < 0)
			printf("UnionSetKeyPosInvalid [%06ld] Error! ret = [%d]\n",atol(argv[i]),ret);
		else
			printf("UnionSetKeyPosInvalid [%06ld] OK!\n",atol(argv[i]));
		
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());
				
	if (UnionIsQuit(p=UnionInput("start keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	min = atol(p);
	if (UnionIsQuit(p=UnionInput("end keyPos (exit/quit to exit)::")))
		return(UnionTaskActionBeforeExit());
	max = atol(p);
		
	for (i = min; i <= max; i++)
	{
		if ((ret = UnionSetKeyPosInvalid(pgKeyPosSpace,i)) < 0)
			printf("UnionSetKeyPosInvalid [%06ld] Error! ret = [%d]\n",i,ret);
		else
			printf("UnionSetKeyPosInvalid [%06ld] OK!\n",i);
	}
	return(UnionTaskActionBeforeExit());
}

int InitICKeySpace(int argc,char *argv[])
{
	int	ret;
	long	i,j,k;
	long	version;
	long	group;
	long	index;
	long	totalKeyNum;
	
	if (argc > 0)
		version = atol(argv[0]);
	else
		version = atol(UnionInput("Input Version Num::"));
	if (argc > 1)
		group = atol(argv[1]);
	else
		group = atol(UnionInput("Input Group Num::"));
	if (argc > 2)
		index = atol(argv[2]);
	else
		index = atol(UnionInput("Input Index Num::"));
	
	if (!UnionConfirm("Are you sure to initialize ic key pos space?"))
		return(UnionTaskActionBeforeExit());
		
	printf("First set all keypos invalid...\n");
	for (i = 0,totalKeyNum = UnionGetTotalKeyPosNum(pgKeyPosSpace); i < totalKeyNum; i++)
		if ((ret = UnionSetKeyPosInvalid(pgKeyPosSpace,i)) < 0)
			printf("UnionSetKeyPosInvalid [%06ld] Error! ret = [%d]\n",i,ret);

	printf("Set all keypos invalid ok!\n");
	
	printf("Now set these available key pos...\n");	
	for (i = 0; i < version; i++)
		for (j = 0; j < group; j++)
			for (k = 1; k <= index; k++)
			{
				if ((ret = UnionUnuseKeyPos(pgKeyPosSpace,i*10000+j*100+k)) < 0)
					printf("UnionUnuseKeyPos [%06ld] Error! ret = [%d]\n",i*10000+j*100+k,ret);
				printf(".");
			}
	printf("Set available keypos ok!\n");
	return(UnionTaskActionBeforeExit());
}

int PrintOccupiedByUser(int argc,char *argv[])
{
	int	ret;
	long	min,max,i;
	char	*p;
	char	userName[80];
	
	for (i = 0; i < argc; i++)
	{
		if ((ret = UnionPrintAllOccupiedKeyPosByUser(pgKeyPosSpace,argv[i])) < 0)
			printf("UnionPrintAllOccupiedKeyPosByUser [%s] Error! ret = [%d]\n",argv[i]);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	p=UnionInput("InputUserName (exit/quit to exit)::");
	if (strlen(p) > sizeof(userName))
	{
		printf("userName too long!\n");
		goto loop;
	}
	memset(userName,0,sizeof(userName));
	strcpy(userName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionPrintAllOccupiedKeyPosByUser(pgKeyPosSpace,userName)) < 0)
		printf("UnionPrintAllOccupiedKeyPosByUser [%s] Error! ret = [%d]\n",userName);
	goto loop;
}

int FreeOccupiedByUser(int argc,char *argv[])
{
	int	ret;
	long	min,max,i;
	char	*p;
	char	userName[80];
	
	for (i = 0; i < argc; i++)
	{
		if (!UnionConfirm("Are you sure of free pos occupied by [%s]?",argv[i]))
			continue;
		if ((ret = UnionUnuseAllKeyPosUsedByUser(pgKeyPosSpace,argv[i])) < 0)
			printf("UnionUnuseAllKeyPosUsedByUser [%s] Error! ret = [%d]\n",argv[i]);
		else
			printf("UnionUnuseAllKeyPosUsedByUser [%d] for [%s] OK!\n",ret,argv[i]);
	}
	if (argc > 0)
		return(UnionTaskActionBeforeExit());

loop:
	p=UnionInput("InputUserName (exit/quit to exit)::");
	if (strlen(p) > sizeof(userName))
	{
		printf("userName too long!\n");
		goto loop;
	}
	memset(userName,0,sizeof(userName));
	strcpy(userName,p);
	if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	
	if ((ret = UnionUnuseAllKeyPosUsedByUser(pgKeyPosSpace,userName)) < 0)
		printf("UnionUnuseAllKeyPosUsedByUser [%s] Error! ret = [%d]\n",userName);
	else
		printf("UnionUnuseAllKeyPosUsedByUser [%d] for [%s] OK!\n",ret,userName);
	goto loop;
}

int OccupyKeySpaceForAllLMK(char *hsmGrpID)
{
	TUnionSJL06LMKPairIndex	lmkIndex;
	int			ret;
	char			*bmk;
		
	if ((ret = UnionConnectSJL06LMKTBL(hsmGrpID)) < 0)
	{
		printf("UnionConnectSJL06LMKTBL Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	
	for (lmkIndex = conLMK0001; lmkIndex <= conLMK3839; lmkIndex++)
	{
		if ((bmk = UnionGetSJL06LMKPair(lmkIndex)) == NULL)
		{
			printf("UnionGetSJL06LMKPair for lmkPair [%02d%02d] error!\n",lmkIndex * 2,lmkIndex * 2 + 1);
			continue;
		}
		if (strlen(bmk) == 0)
		{
			printf("lmkPair [%02d%02d] not defined!\n",lmkIndex*2,lmkIndex*2+1);
			continue;
		}
		if ((ret = UnionUseKeyPos(pgKeyPosSpace,atol(bmk),UnionGetNameOfLMKPair(lmkIndex))) < 0)
			printf("UnionUseKeyPos for [%02d%02d] error!\n",lmkIndex*2,lmkIndex*2+1);
		else
			printf("UnionUseKeyPos for [%02d%02d] OK!\n",lmkIndex*2,lmkIndex*2+1);
	}
	
	UnionDisconnectSJL06LMKTBL(hsmGrpID);
	
	return(UnionTaskActionBeforeExit());
}
		

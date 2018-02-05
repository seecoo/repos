// Author:	Wolfgang Wang
// Date:	2003/09/10
// Version	1.0

#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>

#include "sjl06KeySpace.h"
#include "sjl06SharedKeySpace.h"
#include "unionErrCode.h"
#include "UnionLog.h"

int UnionExistSJL06KeySpaceFile(char *hsmGrp,char *name)
{
	char	fileName[512];
	FILE	*fp;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(hsmGrp,name,fileName);
	
	if ((fp = fopen(fileName,"r")) == NULL)
		return(0);
	else
	{
		fclose(fp);
		return(1);
	}
}

int UnionIsValidSJL06KeySpaceName(char *hsmGrp,char *name)
{
	if ((hsmGrp == NULL) || (name == NULL))
		return(0);
	if (strlen(hsmGrp) != 3)
		return(0);
	if (!isdigit(hsmGrp[0]) || !isdigit(hsmGrp[1]) || !isdigit(hsmGrp[2]))
		return(0);
	if (strcmp(name,conSJL06JKKeySpaceName) == 0)
		return(1);
	if (strcmp(name,conSJL06ICKeySpaceName) == 0)
		return(1);
	if (strcmp(name,conSJL06RSAKeySpaceName) == 0)
		return(1);
	return(0);
}
	
int UnionGetNameOfKeyPosSpace(char *hsmGrp,char *name,char *fileName)
{
	sprintf(fileName,"%s/keyPos/%s.%s.KPS",getenv("UNIONETC"),hsmGrp,name);
	return(0);
}

int UnionCreateKeyPosSpace(char *hsmGrp,char *name,long totalKeyPosNum)
{
	FILE			*fp;
	char			fileName[256];
	int			ret;
	TUnionKeyPosStatus	status;
	long			num;
	TUnionSJL06KeyPosSpace	keyPosSpace;
		
	if (!UnionIsValidSJL06KeySpaceName(hsmGrp,name))
	{
		UnionUserErrLog("in UnionCreateKeyPosSpace:: UnionIsValidSJL06KeySpaceName [%s] [%s]!\n",hsmGrp,name);
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(hsmGrp,name,fileName);
	
	//UnionLog("in UnionCreateKeyPosSpace:: name = [%s] fileName = [%s]\n",name,fileName);
	
	if ((fp = fopen(fileName,"wb")) == NULL)
	{
		UnionSystemErrLog("in UnionCreateKeyPosSpace:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	memset(&keyPosSpace,0,sizeof(keyPosSpace));
	strcpy(keyPosSpace.hsmGrpID,hsmGrp);
	strcpy(keyPosSpace.name,name);
	keyPosSpace.availablePos = keyPosSpace.totalPos = totalKeyPosNum;
	keyPosSpace.invalidPos = keyPosSpace.occupiedPos = 0;
	fwrite(&keyPosSpace,sizeof(keyPosSpace),1,fp);
	
	memset(&status,0,sizeof(status));
	status.status = conAvailableKeyPos;
	UnionLog("in UnionCreateKeyPosSpace:: name = [%s] totalKeyPosNum = [%ld]\n",name,totalKeyPosNum);
	for (num = 0; num < totalKeyPosNum; num++)
		fwrite(&status,sizeof(status),1,fp);
	
	fclose(fp);

	return(0);
}

int UnionDeleteKeyPosSpace(char *hsmGrp,char *name)
{
	char			fileName[256];
	char			cmd[256];
	
	if (!UnionIsValidSJL06KeySpaceName(hsmGrp,name))
	{
		UnionUserErrLog("in UnionDeleteKeyPosSpace:: UnionIsValidSJL06KeySpaceName [%s] [%s]!\n",hsmGrp,name);
		return(errCodeParameter);
	}

	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(hsmGrp,name,fileName);
	
	memset(cmd,0,sizeof(cmd));
	sprintf(cmd,"rm %s",fileName);
	
	return(system(cmd));
}	

PUnionSJL06KeyPosSpace UnionConnectKeyPosSpace(char *hsmGrp,char *name)
{
	PUnionSJL06KeyPosSpace	pkeyPosSpace;
	FILE			*fp;
	char			fileName[256];
	long			totalKeyPosNum;
	TUnionSJL06KeyPosSpace	keyPosSpace;
	
	if (!UnionIsValidSJL06KeySpaceName(hsmGrp,name))
	{
		UnionUserErrLog("in UnionConnectKeyPosSpace:: UnionIsValidSJL06KeySpaceName [%s] [%s]!\n",hsmGrp,name);
		return(NULL);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(hsmGrp,name,fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionConnectKeyPosSpace:: fopen [%s]\n",fileName);
		return(NULL);
	}
	

	if (fread(&keyPosSpace,sizeof(keyPosSpace),1,fp) < 0)
	{
		UnionSystemErrLog("in UnionConnectKeyPosSpace:: fread!\n");
		fclose(fp);
		return(NULL);
	}
	
	fseek(fp,0,SEEK_END);
	if ((totalKeyPosNum = (ftell(fp) - sizeof(keyPosSpace)) / sizeof(TUnionKeyPosStatus)) < keyPosSpace.totalPos)
	{
		UnionUserErrLog("in UnionConnectKeyPosSpace:: define posNum = [%ld] != calPosNum = [%ld]!\n",keyPosSpace.totalPos,totalKeyPosNum);
		fclose(fp);
		return(NULL);
	}
	fclose(fp);
	
	UnionLog("in UnionConnectKeyPosSpace:: totalKeyPosNum = [%ld]\n",keyPosSpace.totalPos);
	
	if ((keyPosSpace.totalPos <= 0) || (keyPosSpace.availablePos + keyPosSpace.occupiedPos + keyPosSpace.invalidPos != keyPosSpace.totalPos))
	{
		fclose(fp);
		UnionUserErrLog("in UnionConnectKeyPosSpace:: [%ld][%ld][%ld][%ld]\n",
		 	keyPosSpace.totalPos,keyPosSpace.availablePos,keyPosSpace.occupiedPos,keyPosSpace.invalidPos);
		return(NULL);
	}
	if ((strcmp(keyPosSpace.hsmGrpID,hsmGrp) != 0) || (strcmp(keyPosSpace.name,name) != 0))
	{
		fclose(fp);
		UnionUserErrLog("in UnionConnectKeyPosSpace:: [%s][%s] != [%s][%s]\n",keyPosSpace.hsmGrpID,keyPosSpace.name,hsmGrp,name);
		return(NULL);
	}
		
	if ((pkeyPosSpace = (PUnionSJL06KeyPosSpace)malloc(sizeof(TUnionSJL06KeyPosSpace))) == NULL)
	{
		UnionSystemErrLog("in UnionConnectKeyPosSpace:: malloc!\n");
		return(NULL);
	}
	
	memcpy(pkeyPosSpace,&keyPosSpace,sizeof(keyPosSpace));
	
	return(pkeyPosSpace);
}
 
int UnionDisconnectKeyPosSpace(PUnionSJL06KeyPosSpace pkeyPosSpace)
{
	if (pkeyPosSpace == NULL)
	{
		//UnionUserErrLog("in UnionDisconnectKeyPosSpace:: null pointer!\n");
		return(errCodeParameter);
	}
	free(pkeyPosSpace);
	return(0);
}

int UnionModifyKeyPosStatus(PUnionSJL06KeyPosSpace pkeyPosSpace,PUnionKeyPosStatus pRec,long keyPosIndex)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	oldStatus;
	
	if ((pRec == NULL) || (pkeyPosSpace == NULL))
	{
		UnionUserErrLog("in UnionModifyKeyPosStatus:: null pointer!\n");
		return(errCodeParameter);
	}
	
	if ((keyPosIndex < 0) || (keyPosIndex >= pkeyPosSpace->totalPos))
	{
		UnionUserErrLog("in UnionModifyKeyPosStatus:: keyPosIndex [%ld] Error!\n",keyPosIndex);
		return(errCodeParameter);
	}
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb+")) == NULL)
	{
		UnionSystemErrLog("in UnionModifyKeyPosStatus:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	if (fseek(fp,sizeof(*pkeyPosSpace) + sizeof(*pRec) * keyPosIndex,SEEK_SET) < 0)
	{
		UnionSystemErrLog("in UnionModifyKeyPosStatus:: fseek!\n");
		goto abnormalExit;
	}
	if (fread(&oldStatus,sizeof(oldStatus),1,fp) < 0)
	{
		UnionSystemErrLog("in UnionModifyKeyPosStatus:: fread!\n");
		goto abnormalExit;
	}
	if (fseek(fp,sizeof(*pkeyPosSpace) + sizeof(*pRec) * keyPosIndex,SEEK_SET) < 0)
	{
		UnionSystemErrLog("in UnionModifyKeyPosStatus:: fseek!\n");
		goto abnormalExit;
	}
	if (fwrite(pRec,sizeof(*pRec),1,fp) < 0)
	{
		UnionSystemErrLog("in UnionModifyKeyPosStatus:: fwrite!\n");
		goto abnormalExit;
	}
	switch (oldStatus.status)
	{
		case	conAvailableKeyPos:
			--pkeyPosSpace->availablePos;
			break;
		case	conOccupiedKeyPos:
			--pkeyPosSpace->occupiedPos;
			break;
		default:
			--pkeyPosSpace->invalidPos;
			break;
	}
	switch (pRec->status)
	{
		case	conAvailableKeyPos:
			++pkeyPosSpace->availablePos;
			break;
		case	conOccupiedKeyPos:
			++pkeyPosSpace->occupiedPos;
			break;
		default:
			++pkeyPosSpace->invalidPos;
			break;
	}
	fseek(fp,0,SEEK_SET);
	if (fwrite(pkeyPosSpace,sizeof(*pkeyPosSpace),1,fp) < 0)
	{
		UnionSystemErrLog("in UnionModifyKeyPosStatus:: fwrite 2!\n");
		goto abnormalExit;
	}
	
	fclose(fp);
	return(0);

abnormalExit:
	fclose(fp);
	return(errCodeSJL06MDL_ModifyKeyPosStatus);
}

int UnionPrintKeyPosStatus(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;

	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionPrintKeyPosStatus:: null pointer!\n");
		return(errCodeParameter);
	}
	
	if ((keyPosIndex < 0) || (keyPosIndex >= pkeyPosSpace->totalPos))
	{
		UnionUserErrLog("in UnionPrintKeyPosStatus:: keyPosIndex [%ld] Error!\n",keyPosIndex);
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintKeyPosStatus:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	if (fseek(fp,sizeof(*pkeyPosSpace) + sizeof(rec) * keyPosIndex,SEEK_SET) < 0)
	{
		UnionSystemErrLog("in UnionPrintKeyPosStatus:: fseek!\n");
		goto abnormalExit;
	}
			
	memset(&rec,0,sizeof(rec));
	if (fread(&rec,sizeof(rec),1,fp) < 0)
	{
		UnionSystemErrLog("in UnionPrintKeyPosStatus:: fwrite!\n");
		goto abnormalExit;
	}
	
	fclose(fp);

	printf("(PosIndex,%06ld); ",keyPosIndex);
	switch (rec.status)
	{
		case	conAvailableKeyPos:
			printf("(Available Pos)\n");
			break;
		case	conOccupiedKeyPos:
			printf("(Occupied  Pos);");
			printf("(Date,%8s);",rec.date);
			printf("(User,%s)\n",rec.user);
			break;
		case	conInvalidKeyPos:
			printf("(Invalid   Pos)\n");
			break;
	}
	
	return(0);
			
abnormalExit:
	fclose(fp);
	return(errCodeUseOSErrCode);
}

int UnionPrintAllAvailableKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;
	long			i;
	int			availableNum = 0;
	
	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionPrintAllAvailableKeyPos:: null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintAllAvailableKeyPos:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	fseek(fp,sizeof(*pkeyPosSpace),SEEK_SET);
	printf("Available KeyPos for [%s]::\n",pkeyPosSpace->name);
	for (i = 0; i < pkeyPosSpace->totalPos; i++)
	{
		fread(&rec,sizeof(rec),1,fp);
		if (rec.status == conAvailableKeyPos)
		{
			printf("[%06ld]\n",i);
			if ((availableNum != 0) && (availableNum % 22 == 0))
			{
				if (UnionIsQuit(UnionPressAnyKey(("Press any key to continue or exit/quit to exit..."))))
				{
					break;
				}
			}
			availableNum++;
		}
	}
	printf("AvailableNum = [%ld] for [%s.%s]\n",availableNum,pkeyPosSpace->hsmGrpID,pkeyPosSpace->name);
	
	fclose(fp);
	return(0);
			
abnormalExit:
	fclose(fp);
	return(errCodeUseOSErrCode);
}

int UnionPrintAllInvalidKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;
	long			i;
	int			invalidNum = 0;
	
	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionPrintAllInvalidKeyPos:: null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintAllInvalidKeyPos:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	fseek(fp,sizeof(*pkeyPosSpace),SEEK_SET);
	printf("Invalid KeyPos for [%s]::\n",pkeyPosSpace->name);
	for (i = 0; i < pkeyPosSpace->totalPos; i++)
	{
		fread(&rec,sizeof(rec),1,fp);
		if (rec.status == conInvalidKeyPos)
		{
			printf("[%06ld]\n",i);
			if ((invalidNum != 0) && (invalidNum % 22 == 0))
			{
				if (UnionIsQuit(UnionPressAnyKey(("Press any key to continue or exit/quit to exit..."))))
				{
					break;
				}
			}
			invalidNum++;
		}
	}
	printf("InvalidNum = [%ld] for [%s.%s]\n",invalidNum,pkeyPosSpace->hsmGrpID,pkeyPosSpace->name);
	
	fclose(fp);
	return(0);
			
abnormalExit:
	fclose(fp);
	return(errCodeUseOSErrCode);
}

int UnionPrintAllOccupiedKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;
	long			i;
	int			occupiedNum = 0;
	
	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionPrintAllOccupiedKeyPos:: null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintAllOccupiedKeyPos:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	fseek(fp,sizeof(*pkeyPosSpace),SEEK_SET);
	printf("Occupied KeyPos for [%s]::\n",pkeyPosSpace->name);
	for (i = 0; i < pkeyPosSpace->totalPos; i++)
	{
		fread(&rec,sizeof(rec),1,fp);
		if (rec.status == conOccupiedKeyPos)
		{
			printf("[%06ld]	[%40s] [%8s]\n",i,rec.user,rec.date);
			if ((occupiedNum != 0) && (occupiedNum % 22 == 0))
			{
				if (UnionIsQuit(UnionPressAnyKey(("Press any key to continue or exit/quit to exit..."))))
				{
					break;
				}
			}
			occupiedNum++;
		}
	}
	printf("OccupiedNum = [%06ld] for [%s.%s]\n",occupiedNum,pkeyPosSpace->hsmGrpID,pkeyPosSpace->name);
	
	fclose(fp);
	return(0);
			
abnormalExit:
	fclose(fp);
	return(errCodeUseOSErrCode);
}

int UnionIsKeyPosAvailable(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;

	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionIsKeyPosAvailable:: null pointer!\n");
		return(0);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionIsKeyPosAvailable:: fopen [%s]\n",fileName);
		return(0);
	}
	
	if (fseek(fp,sizeof(*pkeyPosSpace) + sizeof(rec) * keyPosIndex,SEEK_SET) < 0)
	{
		UnionSystemErrLog("in UnionIsKeyPosAvailable:: fseek!\n");
		goto abnormalExit;
	}
			
	memset(&rec,0,sizeof(rec));
	if (fread(&rec,sizeof(rec),1,fp) < 0)
	{
		UnionSystemErrLog("in UnionIsKeyPosAvailable:: fwrite!\n");
		goto abnormalExit;
	}
	
	fclose(fp);

	if (rec.status == conAvailableKeyPos)
		return(1);
	else
		return(0);
		
abnormalExit:
	fclose(fp);
	return(0);
}

int UnionUseKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex,char *userName)
{
	TUnionKeyPosStatus       rec;
		
	if ((pkeyPosSpace == NULL) || (userName == NULL))
	{
		UnionUserErrLog("in UnionUseKeyPos:: null pointer!\n");
		return(0);
	}
	
	if (strlen(userName) > sizeof(rec.user) - 1)
	{
		UnionUserErrLog("in UnionUseKeyPos:: userName [%s] longer than expected [%d]\n",userName,sizeof(rec.user)-1);
		return(errCodeParameter);
	}
	
	memset(&rec,0,sizeof(rec));
	strcpy(rec.user,userName);
	rec.status = conOccupiedKeyPos;
	UnionGetFullSystemDate(rec.date);
	
	if (!UnionIsKeyPosAvailable(pkeyPosSpace,keyPosIndex))
	{
		UnionUserErrLog("in UnionUseKeyPos: [%ld] is not available!\n",keyPosIndex);
		return(errCodeParameter);
	}
	
	return(UnionModifyKeyPosStatus(pkeyPosSpace,&rec,keyPosIndex));
}

int UnionUnuseKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex)
{
	TUnionKeyPosStatus       rec;
		
	memset(&rec,0,sizeof(rec));
	rec.status = conAvailableKeyPos;

	return(UnionModifyKeyPosStatus(pkeyPosSpace,&rec,keyPosIndex));
}

int UnionSetKeyPosInvalid(PUnionSJL06KeyPosSpace pkeyPosSpace,long keyPosIndex)
{
	TUnionKeyPosStatus       rec;
		
	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionSetKeyPosInvalid:: null pointer!\n");
		return(0);
	}
	
	memset(&rec,0,sizeof(rec));
	rec.status = conInvalidKeyPos;
	
	return(UnionModifyKeyPosStatus(pkeyPosSpace,&rec,keyPosIndex));
}

long UnionSelectAvailableKeyPos(PUnionSJL06KeyPosSpace pkeyPosSpace)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;
	long			i;
	
	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionSelectAvailableKeyPos:: null pointer!\n");
		return(0);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionSelectAvailableKeyPos:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	for (i = 0; i < pkeyPosSpace->totalPos; i++)
	{
		if (fseek(fp,sizeof(*pkeyPosSpace) + i * sizeof(rec),SEEK_SET) < 0)
		{
			UnionSystemErrLog("in UnionSelectAvailableKeyPos:: fseek [%ld]!\n",i);
			goto abnormalExit;
		}
		fread(&rec,sizeof(rec),1,fp);
		if (rec.status == conAvailableKeyPos)
		{
			fclose(fp);
			return(i);
		}
	}
	
abnormalExit:
	fclose(fp);
	return(errCodeUseOSErrCode);
}

long UnionGetTotalKeyPosNum(PUnionSJL06KeyPosSpace pkeyPosSpace)
{
	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionGetTotalKeyPosNum:: pkeyPosSpace is null!\n");
		return(errCodeParameter);
	}
	
	return(pkeyPosSpace->totalPos);
}

int UnionPrintStatusOfKeyPosSpaceToFile(PUnionSJL06KeyPosSpace pkeyPosSpace,FILE *fp)
{
	if ((pkeyPosSpace == NULL) || (fp == NULL))
		return(errCodeParameter);
	fprintf(fp,"\nStatus of keyPosSpace::\n");	
	fprintf(fp,"[hsmGrp]	[%s]\n",pkeyPosSpace->hsmGrpID);
	fprintf(fp,"[name]		[%s]\n",pkeyPosSpace->name);
	fprintf(fp,"[totalPos]	[%ld]\n",pkeyPosSpace->totalPos);
	fprintf(fp,"[availablePos]	[%ld]\n",pkeyPosSpace->availablePos);
	fprintf(fp,"[occupiedPos]	[%ld]\n",pkeyPosSpace->occupiedPos);
	fprintf(fp,"[invalidPos]	[%ld]\n",pkeyPosSpace->invalidPos);
	
	return(0);
}

int UnionUnuseAllKeyPosUsedByUser(PUnionSJL06KeyPosSpace pkeyPosSpace,char *userName)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;
	long			i;
	int			occupiedNum = 0;
	
	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionUnuseAllKeyPosUsedByUser:: null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb+")) == NULL)
	{
		UnionSystemErrLog("in UnionUnuseAllKeyPosUsedByUser:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	for (i = 0; i < pkeyPosSpace->totalPos; i++)
	{
		fseek(fp,sizeof(*pkeyPosSpace) + i * sizeof(rec),SEEK_SET);
		fread(&rec,sizeof(rec),1,fp);
		if ((rec.status != conOccupiedKeyPos) || (strcmp(rec.user,userName) != 0))
			continue;
		rec.status = conAvailableKeyPos;
		memset(rec.user,0,sizeof(rec.user));
		fseek(fp,sizeof(*pkeyPosSpace) + i * sizeof(rec),SEEK_SET);
		fwrite(&rec,sizeof(rec),1,fp);
		++pkeyPosSpace->availablePos;
		--pkeyPosSpace->occupiedPos;
		occupiedNum++;
	}
	fseek(fp,0,SEEK_SET);
	fwrite(pkeyPosSpace,sizeof(*pkeyPosSpace),1,fp);
		
	fclose(fp);
	return(occupiedNum);
			
abnormalExit:
	fclose(fp);
	return(errCodeUseOSErrCode);
}

int UnionPrintAllOccupiedKeyPosByUser(PUnionSJL06KeyPosSpace pkeyPosSpace,char *userName)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;
	long			i;
	int			occupiedNum = 0;
	
	if (pkeyPosSpace == NULL)
	{
		UnionUserErrLog("in UnionPrintAllOccupiedKeyPosByUser:: null pointer!\n");
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,fileName);
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionPrintAllOccupiedKeyPosByUser:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	
	fseek(fp,sizeof(*pkeyPosSpace),SEEK_SET);
	printf("Occupied KeyPos for [%s]::\n",pkeyPosSpace->name);
	for (i = 0; i < pkeyPosSpace->totalPos; i++)
	{
		fread(&rec,sizeof(rec),1,fp);
		if ((rec.status != conOccupiedKeyPos) || (strcmp(rec.user,userName) != 0))
			continue;
		printf("[%06ld]	[%40s] [%8s]\n",i,rec.user,rec.date);
		if ((occupiedNum != 0) && (occupiedNum % 22 == 0))
		{
			if (UnionIsQuit(UnionPressAnyKey(("Press any key to continue or exit/quit to exit..."))))
			{
				break;
			}
		}
		occupiedNum++;
	}
	printf("OccupiedNum = [%06ld] for [%s.%s] by user [%s]\n",occupiedNum,pkeyPosSpace->hsmGrpID,pkeyPosSpace->name,userName);
	
	fclose(fp);
	return(0);
			
abnormalExit:
	fclose(fp);
	return(errCodeUseOSErrCode);
}


int UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace(char *hsmGrp,char *name)
{
	FILE			*fp;
	char			fileName[256];
	TUnionKeyPosStatus	rec;
	long			i;
	int			loadNum = 0;
	PUnionSJL06KeyPosSpace	pkeyPosSpace;
	int			ret;
	
	if ((pkeyPosSpace = UnionConnectKeyPosSpace(hsmGrp,name)) == NULL)
	{
		UnionUserErrLog("in UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace:: UnionConnectKeyPosSpace [%s][%s]\n",hsmGrp,name);
		return(errCodeParameter);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyPosSpace(hsmGrp,name,fileName);
	
	if ((fp = fopen(fileName,"rb+")) == NULL)
	{
		UnionSystemErrLog("in UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace:: fopen [%s]\n",fileName);
		UnionDisconnectKeyPosSpace(pkeyPosSpace);
		return(errCodeUseOSErrCode);
	}
	
	for (i = 0; i < pkeyPosSpace->totalPos; i++)
	{
		fseek(fp,sizeof(*pkeyPosSpace) + i * sizeof(rec),SEEK_SET);
		fread(&rec,sizeof(rec),1,fp);
		if ((rec.status != conOccupiedKeyPos) || (strcmp(rec.user,conKeyPosUserNameOfSharedKeySpace) != 0))
			continue;
		if ((ret = UnionAddSJL06SharedKeyPos(hsmGrp,name,i)) < 0)
		{
			UnionUserErrLog("in UnionLoadSharedKeyPosIntoSJL06ShareKeyPosSpace:: UnionAddSJL06SharedKeyPos [%s][%s][%ld]!\n",hsmGrp,name,i);
			continue;
		}
		loadNum++;
	}
	fclose(fp);
	return(loadNum);
}

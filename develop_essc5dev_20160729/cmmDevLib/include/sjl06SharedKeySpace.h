// Author: Wolfgang Wang
// Copyright: Union Tech. Guangzhou.
// Date: 2003/09/30
// Version1.0

#ifndef _SJL06TmpKeySpace_
#define _SJL06TmpKeySpace_

#include "sjl06KeySpace.h"

#define conMDLNameOfSJL06SharedKeySpace		"SJL06SharedKeySpaceMDL"
#define conKeyPosUserNameOfSharedKeySpace	"shared key pos"

typedef struct
{
	long		keyPos;
	char		hsmGrpID[3+1];
	char		name[3+1];
	short		locked;
} TUnionSJL06SharedKeyPos;
typedef TUnionSJL06SharedKeyPos		*PUnionSJL06SharedKeyPos;

typedef struct
{
	int				maxNumOfSharedKeyPos;
	int				currentNumOfSharedKeyPos;
	PUnionSJL06SharedKeyPos		psjl06SharedKeyPos;
} TUnionSJL06SharedKeySpace;
typedef TUnionSJL06SharedKeySpace	*PUnionSJL06SharedKeySpace;

int UnionGetNameOfSharedKeyPosSpaceDef(char *fileName);
int UnionGetMaxNumOfSJL06SharedKeyPos();

int UnionLockSJL06SharedKeyPos(PUnionSJL06SharedKeyPos	pkeyPos);
int UnionUnlockSJL06SharedKeyPos(PUnionSJL06SharedKeyPos pkeyPos);

int UnionConnectSJL06SharedKeySpace();
int UnionDisconnectSJL06SharedKeySpace();
int UnionRemoveSJL06SharedKeySpace();

int UnionAddSJL06SharedKeyPos(char *hsmGrp,char *name,long keyPos);
long UnionLockUseAvailableSJL06SharedKeyPos(char *hsmGrp,char *name);
int UnionFreeLockUsedSJL06SharedKeyPos(char *hsmGrp,char *name,long index);
int UnionSetSJL06SharedKeyPosSpace(char *hsmGrp,char *name,long num);

int UnionReloadSJL06SharedKeyPosSpace();
int UnionPrintSJL06SharedKeySpaceToFile(FILE *fp);
long UnionClearSJL06SharedKeyPos();
long UnionForceUnlockAllSJL06SharedKeyPos();

#endif

//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/07/26

#ifndef _unionSJL06API_
#define _unionSJL06API_

#include <stdio.h>
#include <string.h>
#include <signal.h>

//#include "UnionStr.h"

#include "sjl06.h"
#include "sjl06Grp.h"
#include "sjl06Protocol.h"

typedef struct
{
	PUnionSJL06	psjl06;
	int		sckHDL;
} TUnionSJL06Conn;
typedef TUnionSJL06Conn	*PUnionSJL06Conn;

typedef enum
{
	conUnionEnumHsmSvr,
	conUnionESSCSvr
} TUnionSJL06ServerType;

#define conMaxNumOfSJL06Conn	10
typedef struct
{
	char		hsmGrpID[3+1];
	PUnionSJL06Conn	phsmConn[conMaxNumOfSJL06Conn];
	int		currentConnNum;
	TUnionSJL06ServerType	serverType;
} TUnionSJL06Server;
typedef TUnionSJL06Server	*PUnionSJL06Server;

PUnionSJL06Server UnionConnectSJL06Server(char *hsmGrpID,TUnionSJL06ServerType serverType);
int UnionDisconnectSJL06Server(PUnionSJL06Server);

PUnionSJL06Conn UnionCreateSJL06Conn(PUnionSJL06 psj06);
int UnionCloseSJL06Conn(PUnionSJL06Conn psjl06Conn);

int UnionSJL06ServerService(PUnionSJL06Server psjl06Server,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
	
#endif

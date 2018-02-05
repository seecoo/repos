#ifndef _AccessCheck_H_
#define _AccessCheck_H_

typedef struct
{
	char	sysID[16+1];
	char	sysName[128+1];
	char	ipAddrList[1024+1];
	char	appIDList[1024+1];
	int	enabled;
}TUnionExternalSystemTable;
typedef TUnionExternalSystemTable		*PUnionExternalSystemTable;

typedef struct
{
	char	appID[16+1];
	char	appName[128+1];
	char	serviceCodeList[1024+1];
	int	timeout;
	int	enabled;
}TUnionAppTable;
typedef TUnionAppTable		*PUnionAppTable;

typedef struct
{
	char	serviceCode[4+1];
	char	serviceType[128+1];
	int	codeEnabled;
	int	typeEnabled;
}TUnionServiceCodeAndTypeTable;
typedef TUnionServiceCodeAndTypeTable		*PUnionServiceCodeAndTypeTable;

int UnionCheckExternalSystemDef(int isHsmCmd,char *sysID,char *appID,char *cliIPAddr,int port,char *clientIPAddr);

int UnionCheckAppDef(char *appID,char *serviceCode,int *timeout);

int UnionCheckServiceCodeAndServiceTypeDef(char *serviceCode);

int UnionAccessCheck(char *cliIPAddr,int port,int *timeout,int *isUI);

int UnionCheckLoginFlagOfUser(char *sysID,char *userID,char *serviceCode);

int UnionReadHsmGroupIDListByAppID(char *appID,char *hsmGroupIDList,int sizeofBuf);

int UnionReadHsmGroupIDByAppID(char *appID,PUnionHsmGroupRec phsmGroupRec);

#endif

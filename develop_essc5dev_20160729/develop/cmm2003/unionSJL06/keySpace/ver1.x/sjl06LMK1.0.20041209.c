//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2003/10/08
//	Version:	1.0

#define _UnionEnv_3_x_
#define _UnionLog_3_x_

#include <stdio.h>
#include <string.h>

#include "UnionEnv.h"

#include "sjl06LMK.h"
#include "unionErrCode.h"
#include "UnionLog.h"

int	pgsjl06LMKTBLConnected = 0;

char	pgsjl06LMKPair[50][3+1];
char	pgthisHsmGrpID[20];

int UnionGetNameOfLMKDefFileOfHsmGrp(char *hsmGrpID,char *fileName)
{
	sprintf(fileName,"%s/keyPos/hsmGrp%s.LMK",getenv("UNIONETC"),hsmGrpID);
	return(0);
}

int UnionConnectSJL06LMKTBL(char *hsmGrpID)
{
	char	fileName[512];
	int	ret;
	char	tmpBuf[10];
	int	i;
	char	*p;

	if (pgsjl06LMKTBLConnected)
		return(0);
			
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/keyPos/hsmGrp%s.LMK",getenv("UNIONETC"),hsmGrpID);
	
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionConnectSJL06LMKTBL:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}
	
	for (i = 0; i < 50; i++)
	{
		memset(pgsjl06LMKPair[i],0,sizeof(pgsjl06LMKPair[i]));
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%02d%02d",2*i,2*i+1);
		if ((p = UnionGetEnviVarByName(tmpBuf)) == NULL)
			continue;
		if (strlen(p) != sizeof(pgsjl06LMKPair[i]) - 1)
			continue;
		strcpy(pgsjl06LMKPair[i],p);
	}
	
	UnionClearEnvi();
	
	pgsjl06LMKTBLConnected = 1;
	memset(pgthisHsmGrpID,0,sizeof(pgthisHsmGrpID));
	strcpy(pgthisHsmGrpID,hsmGrpID);
		
	return(0);
}

int UnionDisconnectSJL06LMKTBL(char *hsmGrpID)
{
	return(0);
}

	
char *UnionGetSJL06LMKPair(TUnionSJL06LMKPairIndex lmkPairIndex)
{
	int	ret;
	
	if ((ret = UnionConnectSJL06LMKTBL(pgthisHsmGrpID)) < 0)
		return(NULL);
		
	if ((lmkPairIndex >= 50) || (lmkPairIndex < 0))
		return(NULL);
	else
		return(pgsjl06LMKPair[lmkPairIndex]);
}

char *UnionGetSJL06LMKPairOfSpecifiedHsmGrp(char *hsmGrpID,TUnionSJL06LMKPairIndex lmkPairIndex)
{
	int	ret;
	
	if ((ret = UnionConnectSJL06LMKTBL(hsmGrpID)) < 0)
		return(NULL);
		
	if ((lmkPairIndex >= 50) || (lmkPairIndex < 0))
		return(NULL);
	else
		return(pgsjl06LMKPair[lmkPairIndex]);
}

TUnionSJL06LMKPairIndex UnionConvertSJL06PairIndex(char *lmkPairIndex)
{
	UnionToUpperCase(lmkPairIndex);
	if ((strcmp(lmkPairIndex,"LMKPAIR0001") == 0) ||
	    (strcmp(lmkPairIndex,"LMK0001") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR0001") == 0) ||
	    (strcmp(lmkPairIndex,"0001") == 0))
	    return(conLMK0001);
	if ((strcmp(lmkPairIndex,"LMKPAIR0203") == 0) ||
	    (strcmp(lmkPairIndex,"LMK0203") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR0203") == 0) ||
	    (strcmp(lmkPairIndex,"0203") == 0))
	    return(conLMK0203);
	if ((strcmp(lmkPairIndex,"LMKPAIR0405") == 0) ||
	    (strcmp(lmkPairIndex,"LMK0405") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR0405") == 0) ||
	    (strcmp(lmkPairIndex,"0405") == 0))
	    return(conLMK0405);
	if ((strcmp(lmkPairIndex,"LMKPAIR0607") == 0) ||
	    (strcmp(lmkPairIndex,"LMK0607") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR0607") == 0) ||
	    (strcmp(lmkPairIndex,"0607") == 0))
	    return(conLMK0607);
	if ((strcmp(lmkPairIndex,"LMKPAIR0809") == 0) ||
	    (strcmp(lmkPairIndex,"LMK0809") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR0809") == 0) ||
	    (strcmp(lmkPairIndex,"0809") == 0))
	    return(conLMK0809);
	if ((strcmp(lmkPairIndex,"LMKPAIR1011") == 0) ||
	    (strcmp(lmkPairIndex,"LMK1011") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR1011") == 0) ||
	    (strcmp(lmkPairIndex,"1011") == 0))
	    return(conLMK1011);
	if ((strcmp(lmkPairIndex,"LMKPAIR1213") == 0) ||
	    (strcmp(lmkPairIndex,"LMK1213") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR1213") == 0) ||
	    (strcmp(lmkPairIndex,"1213") == 0))
	    return(conLMK1213);
	if ((strcmp(lmkPairIndex,"LMKPAIR1415") == 0) ||
	    (strcmp(lmkPairIndex,"LMK1415") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR1415") == 0) ||
	    (strcmp(lmkPairIndex,"1415") == 0))
	    return(conLMK1415);
	if ((strcmp(lmkPairIndex,"LMKPAIR1617") == 0) ||
	    (strcmp(lmkPairIndex,"LMK1617") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR1617") == 0) ||
	    (strcmp(lmkPairIndex,"1617") == 0))
	    return(conLMK1617);
	if ((strcmp(lmkPairIndex,"LMKPAIR1819") == 0) ||
	    (strcmp(lmkPairIndex,"LMK1819") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR1819") == 0) ||
	    (strcmp(lmkPairIndex,"1819") == 0))
	    return(conLMK1819);
	if ((strcmp(lmkPairIndex,"LMKPAIR2021") == 0) ||
	    (strcmp(lmkPairIndex,"LMK2021") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR2021") == 0) ||
	    (strcmp(lmkPairIndex,"2021") == 0))
	    return(conLMK2021);
	if ((strcmp(lmkPairIndex,"LMKPAIR2223") == 0) ||
	    (strcmp(lmkPairIndex,"LMK2223") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR2223") == 0) ||
	    (strcmp(lmkPairIndex,"2223") == 0))
	    return(conLMK2223);
	if ((strcmp(lmkPairIndex,"LMKPAIR2425") == 0) ||
	    (strcmp(lmkPairIndex,"LMK2425") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR2425") == 0) ||
	    (strcmp(lmkPairIndex,"2425") == 0))
	    return(conLMK2425);
	if ((strcmp(lmkPairIndex,"LMKPAIR2627") == 0) ||
	    (strcmp(lmkPairIndex,"LMK2627") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR2627") == 0) ||
	    (strcmp(lmkPairIndex,"2627") == 0))
	    return(conLMK2627);
	if ((strcmp(lmkPairIndex,"LMKPAIR2829") == 0) ||
	    (strcmp(lmkPairIndex,"LMK2829") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR2829") == 0) ||
	    (strcmp(lmkPairIndex,"2829") == 0))
	    return(conLMK2829);
	if ((strcmp(lmkPairIndex,"LMKPAIR3031") == 0) ||
	    (strcmp(lmkPairIndex,"LMK3031") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR3031") == 0) ||
	    (strcmp(lmkPairIndex,"3031") == 0))
	    return(conLMK3031);
	if ((strcmp(lmkPairIndex,"LMKPAIR3233") == 0) ||
	    (strcmp(lmkPairIndex,"LMK3233") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR3233") == 0) ||
	    (strcmp(lmkPairIndex,"3233") == 0))
	    return(conLMK3233);
	if ((strcmp(lmkPairIndex,"LMKPAIR3435") == 0) ||
	    (strcmp(lmkPairIndex,"LMK3435") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR3435") == 0) ||
	    (strcmp(lmkPairIndex,"3435") == 0))
	    return(conLMK3435);
	if ((strcmp(lmkPairIndex,"LMKPAIR3637") == 0) ||
	    (strcmp(lmkPairIndex,"LMK3637") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR3637") == 0) ||
	    (strcmp(lmkPairIndex,"3637") == 0))
	    return(conLMK3637);
	if ((strcmp(lmkPairIndex,"LMKPAIR3839") == 0) ||
	    (strcmp(lmkPairIndex,"LMK3839") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR3839") == 0) ||
	    (strcmp(lmkPairIndex,"3839") == 0))
	    return(conLMK3839);
	if ((strcmp(lmkPairIndex,"LMKPAIR4041") == 0) ||
	    (strcmp(lmkPairIndex,"LMK4041") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR4041") == 0) ||
	    (strcmp(lmkPairIndex,"4041") == 0))
	    return(conLMK4041);
	if ((strcmp(lmkPairIndex,"LMKPAIR4243") == 0) ||
	    (strcmp(lmkPairIndex,"LMK4243") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR4243") == 0) ||
	    (strcmp(lmkPairIndex,"4243") == 0))
	    return(conLMK4243);
	if ((strcmp(lmkPairIndex,"LMKPAIR4445") == 0) ||
	    (strcmp(lmkPairIndex,"LMK4445") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR4445") == 0) ||
	    (strcmp(lmkPairIndex,"4445") == 0))
	    return(conLMK4445);
	if ((strcmp(lmkPairIndex,"LMKPAIR4647") == 0) ||
	    (strcmp(lmkPairIndex,"LMK4647") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR4647") == 0) ||
	    (strcmp(lmkPairIndex,"4647") == 0))
	    return(conLMK4647);
	if ((strcmp(lmkPairIndex,"LMKPAIR4849") == 0) ||
	    (strcmp(lmkPairIndex,"LMK4849") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR4849") == 0) ||
	    (strcmp(lmkPairIndex,"4849") == 0))
	    return(conLMK4849);
	if ((strcmp(lmkPairIndex,"LMKPAIR5051") == 0) ||
	    (strcmp(lmkPairIndex,"LMK5051") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR5051") == 0) ||
	    (strcmp(lmkPairIndex,"5051") == 0))
	    return(conLMK5051);
	if ((strcmp(lmkPairIndex,"LMKPAIR5253") == 0) ||
	    (strcmp(lmkPairIndex,"LMK5253") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR5253") == 0) ||
	    (strcmp(lmkPairIndex,"5253") == 0))
	    return(conLMK5253);
	if ((strcmp(lmkPairIndex,"LMKPAIR5455") == 0) ||
	    (strcmp(lmkPairIndex,"LMK5455") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR5455") == 0) ||
	    (strcmp(lmkPairIndex,"5455") == 0))
	    return(conLMK5455);
	if ((strcmp(lmkPairIndex,"LMKPAIR5657") == 0) ||
	    (strcmp(lmkPairIndex,"LMK5657") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR5657") == 0) ||
	    (strcmp(lmkPairIndex,"5657") == 0))
	    return(conLMK5657);
	if ((strcmp(lmkPairIndex,"LMKPAIR5859") == 0) ||
	    (strcmp(lmkPairIndex,"LMK5859") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR5859") == 0) ||
	    (strcmp(lmkPairIndex,"5859") == 0))
	    return(conLMK5859);
	if ((strcmp(lmkPairIndex,"LMKPAIR6061") == 0) ||
	    (strcmp(lmkPairIndex,"LMK6061") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR6061") == 0) ||
	    (strcmp(lmkPairIndex,"6061") == 0))
	    return(conLMK6061);
	if ((strcmp(lmkPairIndex,"LMKPAIR6263") == 0) ||
	    (strcmp(lmkPairIndex,"LMK6263") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR6263") == 0) ||
	    (strcmp(lmkPairIndex,"6263") == 0))
	    return(conLMK6263);
	if ((strcmp(lmkPairIndex,"LMKPAIR6465") == 0) ||
	    (strcmp(lmkPairIndex,"LMK6465") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR6465") == 0) ||
	    (strcmp(lmkPairIndex,"6465") == 0))
	    return(conLMK6465);
	if ((strcmp(lmkPairIndex,"LMKPAIR6667") == 0) ||
	    (strcmp(lmkPairIndex,"LMK6667") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR6667") == 0) ||
	    (strcmp(lmkPairIndex,"6667") == 0))
	    return(conLMK6667);
	if ((strcmp(lmkPairIndex,"LMKPAIR6869") == 0) ||
	    (strcmp(lmkPairIndex,"LMK6869") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR6869") == 0) ||
	    (strcmp(lmkPairIndex,"6869") == 0))
	    return(conLMK6869);
	if ((strcmp(lmkPairIndex,"LMKPAIR7071") == 0) ||
	    (strcmp(lmkPairIndex,"LMK7071") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR7071") == 0) ||
	    (strcmp(lmkPairIndex,"7071") == 0))
	    return(conLMK7071);
	if ((strcmp(lmkPairIndex,"LMKPAIR7273") == 0) ||
	    (strcmp(lmkPairIndex,"LMK7273") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR7273") == 0) ||
	    (strcmp(lmkPairIndex,"7273") == 0))
	    return(conLMK7273);
	if ((strcmp(lmkPairIndex,"LMKPAIR7475") == 0) ||
	    (strcmp(lmkPairIndex,"LMK7475") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR7475") == 0) ||
	    (strcmp(lmkPairIndex,"7475") == 0))
	    return(conLMK7475);
	if ((strcmp(lmkPairIndex,"LMKPAIR7677") == 0) ||
	    (strcmp(lmkPairIndex,"LMK7677") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR7677") == 0) ||
	    (strcmp(lmkPairIndex,"7677") == 0))
	    return(conLMK7677);
	if ((strcmp(lmkPairIndex,"LMKPAIR7879") == 0) ||
	    (strcmp(lmkPairIndex,"LMK7879") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR7879") == 0) ||
	    (strcmp(lmkPairIndex,"7879") == 0))
	    return(conLMK7879);
	if ((strcmp(lmkPairIndex,"LMKPAIR8081") == 0) ||
	    (strcmp(lmkPairIndex,"LMK8081") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR8081") == 0) ||
	    (strcmp(lmkPairIndex,"8081") == 0))
	    return(conLMK8081);
	if ((strcmp(lmkPairIndex,"LMKPAIR8283") == 0) ||
	    (strcmp(lmkPairIndex,"LMK8283") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR8283") == 0) ||
	    (strcmp(lmkPairIndex,"8283") == 0))
	    return(conLMK8283);
	if ((strcmp(lmkPairIndex,"LMKPAIR8485") == 0) ||
	    (strcmp(lmkPairIndex,"LMK8485") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR8485") == 0) ||
	    (strcmp(lmkPairIndex,"8485") == 0))
	    return(conLMK8485);
	if ((strcmp(lmkPairIndex,"LMKPAIR8687") == 0) ||
	    (strcmp(lmkPairIndex,"LMK8687") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR8687") == 0) ||
	    (strcmp(lmkPairIndex,"8687") == 0))
	    return(conLMK8687);
	if ((strcmp(lmkPairIndex,"LMKPAIR8889") == 0) ||
	    (strcmp(lmkPairIndex,"LMK8889") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR8889") == 0) ||
	    (strcmp(lmkPairIndex,"8889") == 0))
	    return(conLMK8889);
	if ((strcmp(lmkPairIndex,"LMKPAIR9091") == 0) ||
	    (strcmp(lmkPairIndex,"LMK9091") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR9091") == 0) ||
	    (strcmp(lmkPairIndex,"9091") == 0))
	    return(conLMK9091);
	if ((strcmp(lmkPairIndex,"LMKPAIR9293") == 0) ||
	    (strcmp(lmkPairIndex,"LMK9293") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR9293") == 0) ||
	    (strcmp(lmkPairIndex,"9293") == 0))
	    return(conLMK9293);
	if ((strcmp(lmkPairIndex,"LMKPAIR9495") == 0) ||
	    (strcmp(lmkPairIndex,"LMK9495") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR9495") == 0) ||
	    (strcmp(lmkPairIndex,"9495") == 0))
	    return(conLMK9495);
	if ((strcmp(lmkPairIndex,"LMKPAIR9697") == 0) ||
	    (strcmp(lmkPairIndex,"LMK9697") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR9697") == 0) ||
	    (strcmp(lmkPairIndex,"9697") == 0))
	    return(conLMK9697);
	if ((strcmp(lmkPairIndex,"LMKPAIR9899") == 0) ||
	    (strcmp(lmkPairIndex,"LMK9899") == 0) ||
	    (strcmp(lmkPairIndex,"PAIR9899") == 0) ||
	    (strcmp(lmkPairIndex,"9899") == 0))
	    return(conLMK9899);
	if ((strcmp(lmkPairIndex,"ALLLMK") == 0) ||
	    (strcmp(lmkPairIndex,"LMKALL") == 0) ||
	    (strcmp(lmkPairIndex,"ALL") == 0))
	    return(conLMKAll);
	return(errCodeSJL06MDL_InvalidLMKPaire);
}

char *UnionGetNameOfLMKPair(TUnionSJL06LMKPairIndex lmkPair)
{
	switch (lmkPair)
	{
		case conLMK0001:
			return("LMKPAIR0001");
		case conLMK0203:
			return("LMKPAIR0203");
		case conLMK0405:
			return("保护ZMK的密钥对");
		case conLMK0607:
			return("保护ZPK的LMK对0607");
		case conLMK0809:
			return("LMKPAIR0809");
		case conLMK1011:
			return("LMKPAIR1011");
		case conLMK1213:
			return("LMKPAIR1213");
		case conLMK1415:
			return("保护PVK/TMK/TAK的LMK对1415");
		case conLMK1617:
			return("保护TAK的密钥对");
		case conLMK1819:
			return("LMKPAIR1819");
		case conLMK2021:
			return("LMKPAIR2021");
		case conLMK2223:
			return("保护WWK的密钥对");
		case conLMK2425:
			return("LMKPAIR2425");
		case conLMK2627:
			return("保护ZAK的LMK对2627");
		case conLMK2829:
			return("保护BDK的密钥对");
		case conLMK3031:
			return("LMKPAIR3031");
		case conLMK3233:
			return("保护ZEK的密钥对");
		case conLMK3435:
			return("LMKPAIR3435");
		case conLMK3637:
			return("LMKPAIR3637");
		case conLMK3839:
			return("LMKPAIR3839");
		default:
			return("UnknownLMK");
	}
}

	

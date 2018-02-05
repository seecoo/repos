// Author:	Wolfgang Wang
// Date:	2006/07/26

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "centerREC.h"

int	gunionNumOfEssc = 0;
int	gunionIsCenterRECConnected = 0;
char	gunionIDOfEsscAPI[10+1] = "DA";
char	gunionIPAddrOfEssc[conMaxNumOfEssc][15+1];
// 本机的IP地址
char	gunionIPAddrOfMyself[15+1] = "192.1.2.202";
int	gunionPortOfEssc[conMaxNumOfEssc];
int	gunionTimeoutOfEssc = 5;
int	gunionIsDebug = 0;
int	gunionIsShortConnUsed = 0;
int	gunionTypeOfAutoSign = 100;

int UnionIsDebug()
{
	return(gunionIsDebug);
}	

int UnionGetRealNumOfEssc()
{
	return(gunionNumOfEssc);
}

char *UnionGetIPAddrOfMyself()
{
	return(gunionIPAddrOfMyself);
}

char *UnionGetIPAddrOfCenterSecuSvr(int index)
{
	if (index <= 0)
		index = 1;
	return(gunionIPAddrOfEssc[(index-1)%gunionNumOfEssc]);
}

int UnionGetPortOfCenterSecuSvr(int index)
{
	if (index <= 0)
		index = 1;
	return(gunionPortOfEssc[(index-1)%gunionNumOfEssc]);
}

int UnionGetTimeoutOfCenterSecuSvr()
{
	return(gunionTimeoutOfEssc);
}

char *UnionGetIDOfEsscAPI()
{
	return(gunionIDOfEsscAPI);
}

int UnionIsShortConnectionUsed()
{
	return(gunionIsShortConnUsed);
}

int UnionGetAutoAppendSignType()
{
	return(gunionTypeOfAutoSign);
}


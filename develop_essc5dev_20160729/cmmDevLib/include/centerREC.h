// Author:	Wolfgang Wang
// Date:	2006/07/26

#ifndef _centerREC_
#define _centerREC_

#define conMaxNumOfEssc	2

int UnionGetRealNumOfEssc();

int UnionConnectCenterREC();

char *UnionGetIPAddrOfMyself();

int UnionIsDebug();

char *UnionGetIPAddrOfCenterSecuSvr(int hsmIndex);

int UnionGetPortOfCenterSecuSvr(int hsmIndex);

int UnionGetTimeoutOfCenterSecuSvr();

char *UnionGetIDOfEsscAPI();

int UnionIsShortConnectionUsed();

int UnionGetAutoAppendSignType();

#endif

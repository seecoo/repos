//	Author:		’≈”¿∂®
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _CommWithHsm_
#define _CommWithHsm_

void UnionDealSJL06Timeout();

void UnionSetTimeoutOfHsm(int timeout);

void UnionSetLenOfHsmHeader(int len);

int UnionConnectHsm(char *hsmIPAddr,int hsmPort);

int UnionCloseHsm();

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);



#endif


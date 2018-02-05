//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2006/7/26

#ifndef _commWithEsscSvr_
#define _commWithEsscSvr_

void UnionReleaseCommWithHsmSvr(unsigned char *connTag);

int UnionCommWithHsmSvrForThread(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,unsigned char *connTag);

#endif

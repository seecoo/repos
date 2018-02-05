#ifndef _SmartCard_H_
#define _SmartCard_H_
#include <winscard.h>
#include <stdbool.h>

bool UnionCardConnect(char *CardReaderName,char *pATR);
bool UnionCardDisconnect();
int UnionRunAPDU(char *pRequestStr, int lenOfReq,char *pResponseStr,int sizeofBuf);

//int UnionGetCardListReaders(char pReaders[][128+1],int maxNum);
int UnionGetCardListReaders(LPTSTR pReaders[],int maxNum);
void UnionCardReleaseContex(void);

void UnionReadErrorMsg(long ErrorCode,char *errorerrorMsg);

#endif

//	Date:	2002/6/20
/* Modified by Mary, 2000-7-23 */

#ifndef _UnionMAC

int UnionGenerateANSIX99MAC(char *pPlainKey,char *pMacData,int nLenOfMacData,char *pMAC);

int UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail(char *key,int lenOfKey,char *macData,int lenOfMacData,char *mac);

#endif

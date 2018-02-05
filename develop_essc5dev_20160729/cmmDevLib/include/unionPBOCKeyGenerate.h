// Wolfgang Wang
// 2004/11/17

#ifndef _unionPBOCKeyGenerate_
#define _unionPBOCKeyGenerate_

int UnionGeneratePBOCDPKLeftPart(char *MPK,char *inputData,char *leftDPK);
int UnionGeneratePBOCDPKRightPart(char *MPK,char *inputData,char *rightDPK);
int UnionGeneratePBOCDPK(char *MPK,char *inputData,char *DPK);
int UnionGeneratePBOCSessionKey(char *DPK,char *inputData,char *sessionKey);

#endif

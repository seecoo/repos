//	Author:		Wolfgang Wang
//	Date:		2003/11/04
//	Version:	5.0

//	5.1 2005/05/30����5.0����������
//	�޸���	UnionApplyNewMsgIndexOfTransSpierBuf

#ifndef _commWithTransSpier_
#define _commWithTransSpier_

// 2009/9/19������������
void UnionIncreaseSpierSSN();

// 2009/9/19������������
long UnionGetSpierSSN();

// 2009/9/19������������
char *UnionGetClientIPAddrToldTransSpier();

// 2009/9/19������������
int UnionGetClientPortToldTransSpier();

// 2009/9/19������������
void UnionSetClientAttrToldTransSpier(char *ipAddr,int port);

// 2009/9/19������������
int UnionSendRequestInfoToTransSpierAlways(int len,char *data);

// 2009/9/19������������
int UnionSendRequestInfoToTransSpier(int len,char *data);

// 2009/9/19������������
int UnionSendResponseInfoToTransSpier(int len,char *data);

// 2009/9/19������������
int UnionSendHsmCmdRequestInfoToTransSpier(int len,char *data);

// 2009/9/19������������
int UnionSendHsmCmdResponseInfoToTransSpier(int len,char *data);

int UnionSendInfoToTransSpier(int len,char *data);

int UnionSendInfoToTransSpierWithSpecResID(int len,char *data,int resID);

void UnionSetMyResIDToTransSpier(int resID);

// 2009/9/19������������
int UnionSendKDBServiceInfoToTransSpier(int len,char *data);

// 2009/9/19������������
int UnionSendKeyDBSynchronizerInfoToTransSpier(int len,char *data);

#endif

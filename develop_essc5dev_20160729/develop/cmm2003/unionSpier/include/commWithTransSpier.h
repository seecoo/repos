//	Author:		Wolfgang Wang
//	Date:		2003/11/04
//	Version:	5.0

//	5.1 2005/05/30，在5.0基础上升级
//	修改了	UnionApplyNewMsgIndexOfTransSpierBuf

#ifndef _commWithTransSpier_
#define _commWithTransSpier_

// 2009/9/19，王纯军增加
void UnionIncreaseSpierSSN();

// 2009/9/19，王纯军增加
long UnionGetSpierSSN();

// 2009/9/19，王纯军增加
char *UnionGetClientIPAddrToldTransSpier();

// 2009/9/19，王纯军增加
int UnionGetClientPortToldTransSpier();

// 2009/9/19，王纯军增加
void UnionSetClientAttrToldTransSpier(char *ipAddr,int port);

// 2009/9/19，王纯军增加
int UnionSendRequestInfoToTransSpierAlways(int len,char *data);

// 2009/9/19，王纯军增加
int UnionSendRequestInfoToTransSpier(int len,char *data);

// 2009/9/19，王纯军增加
int UnionSendResponseInfoToTransSpier(int len,char *data);

// 2009/9/19，王纯军增加
int UnionSendHsmCmdRequestInfoToTransSpier(int len,char *data);

// 2009/9/19，王纯军增加
int UnionSendHsmCmdResponseInfoToTransSpier(int len,char *data);

int UnionSendInfoToTransSpier(int len,char *data);

int UnionSendInfoToTransSpierWithSpecResID(int len,char *data,int resID);

void UnionSetMyResIDToTransSpier(int resID);

// 2009/9/19，王纯军增加
int UnionSendKDBServiceInfoToTransSpier(int len,char *data);

// 2009/9/19，王纯军增加
int UnionSendKeyDBSynchronizerInfoToTransSpier(int len,char *data);

#endif

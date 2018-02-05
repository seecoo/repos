#ifndef _UnionMsgBufGroup_        
#define _UnionMsgBufGroup_ 

// 消息分组
// # 第0组:UI通讯
// # 第1组:APP通讯
// # 第2组:dbSvr
// # 第3组:hsmSvr
// # 第4组:监控信息
// # 第5组:同步
#define defMsgBufGroupOfUI		0
#define defMsgBufGroupOfAPP		1
#define defMsgBufGroupOfDBSvr		2
#define defMsgBufGroupOfHsmSvr		3
#define defMsgBufGroupOfMon		4
#define defMsgBufGroupOfDataSync	5
//#define defMsgBufGroupOfHsmThrough      6
//#define defMsgBufGroupOfHsmCmd          7

// 设置组ID,从0开始计数
void UnionSetGroupIDOfMsgBuf(int groupID);

#endif

// Author:	Wolfgang Wang
// Date:	2006/07/26

#ifndef _centerREC_
#define _centerREC_

#define conMaxNumOfEssc	2

// 密码机的实际数据
extern int	gunionNumOfEssc;
// 客户端API使用的应用编号
extern char	gunionIDOfEsscAPI[];
// 中心的IP地址
extern char	gunionIPAddrOfEssc[][15+1];
// 中心的端口号
extern int	gunionPortOfEssc[];
// 与中心通讯使用的超时
extern int	gunionTimeoutOfEssc;
// 日志是否打开
extern int	gunionIsDebug;
// 是否使用短连接
extern int	gunionIsShortConnUsed;

extern int	gunionIDOfMsgBuf;

extern int	gunionIsCenterRECConnected;

extern int	gunionTypeOfAutoSign;

int UnionConnectCenterREC();

int UnionIsDebug();

char *UnionGetIPAddrOfCenterSecuSvr(int hsmIndex);

int UnionGetPortOfCenterSecuSvr(int hsmIndex);

int UnionGetTimeoutOfCenterSecuSvr();

char *UnionGetIDOfEsscAPI();

int UnionIsShortConnectionUsed();

int UnionGetAutoAppendSignType();

#endif

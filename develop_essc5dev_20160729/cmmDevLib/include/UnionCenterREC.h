// Author:	Wolfgang Wang
// Date:	2006/07/26

#ifndef _centerREC_
#define _centerREC_

#define conMaxNumOfEssc	2

// �������ʵ������
extern int	gunionNumOfEssc;
// �ͻ���APIʹ�õ�Ӧ�ñ��
extern char	gunionIDOfEsscAPI[];
// ���ĵ�IP��ַ
extern char	gunionIPAddrOfEssc[][15+1];
// ���ĵĶ˿ں�
extern int	gunionPortOfEssc[];
// ������ͨѶʹ�õĳ�ʱ
extern int	gunionTimeoutOfEssc;
// ��־�Ƿ��
extern int	gunionIsDebug;
// �Ƿ�ʹ�ö�����
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

//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2002/10/31

//	��������˵������������0����ʾ��ȷִ�У�����<0��ʾ������롣

/*	SJL06��ָ���ʽ:
	����
		lenOfLenField + MsgHeader(lenOfMsgHeader) + reqStr
	��Ӧ��
		lenofLenField + MsgHeader(lenOfMsgHeader) + resStr
*/
#ifndef _UnionSJL06Protocol_
#define _UnionSJL06Protocol_

#include "sjl06.h"

// ��鵱ǰ�����Ƿ�����
int UnionIsThisSJL06LongConnAbnormal();
// ���õ�ǰ��������
int UnionSetThisSJL06LongConnOK();
// ���õ�ǰ�����쳣
int UnionSetThisSJL06LongConnAbnormal();

// ��resStr�⣬ȫ�������������
// ����HSM��Ӧ���ĳ��ȣ�ȥ�����������Ϣͷ��
// �����ش�����
// pSJL06��ָ�������
// reqStr���������
// lenOfReqStr������ĳ���
// resStr����Ӧ�ִ�
// sizeOfResStr����Ӧ�ִ��Ĵ�С
// sckHDL��ָ�������������
int UnionShortConnSJL06Cmd(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
int UnionLongConnSJL06Cmd(int sckHDL,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
int UnionLongConn2HexLenSJL06Cmd(int sckConn,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
int UnionShortConn2HexLenSJL06Cmd(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

// the following two functions added by wolfgang wang, 20040730
int UnionShortConnSJL06CmdAnyway(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
int UnionLongConnSJL06CmdAnyway(int sckHDL,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

#endif

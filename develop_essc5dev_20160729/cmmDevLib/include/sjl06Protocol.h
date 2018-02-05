//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2002/10/31

//	除非特殊说明，函数返回0，表示正确执行，返回<0表示出错代码。

/*	SJL06的指令格式:
	请求：
		lenOfLenField + MsgHeader(lenOfMsgHeader) + reqStr
	响应：
		lenofLenField + MsgHeader(lenOfMsgHeader) + resStr
*/
#ifndef _UnionSJL06Protocol_
#define _UnionSJL06Protocol_

#include "sjl06.h"

// 检查当前连接是否正常
int UnionIsThisSJL06LongConnAbnormal();
// 设置当前连接正常
int UnionSetThisSJL06LongConnOK();
// 设置当前连接异常
int UnionSetThisSJL06LongConnAbnormal();

// 除resStr外，全部是输入参数。
// 返回HSM响应串的长度（去掉长度域和消息头）
// 出错返回错误码
// pSJL06，指向密码机
// reqStr，请求命令串
// lenOfReqStr，命令串的长度
// resStr，响应字串
// sizeOfResStr，响应字串的大小
// sckHDL，指向密码机的连接
int UnionShortConnSJL06Cmd(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
int UnionLongConnSJL06Cmd(int sckHDL,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
int UnionLongConn2HexLenSJL06Cmd(int sckConn,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
int UnionShortConn2HexLenSJL06Cmd(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

// the following two functions added by wolfgang wang, 20040730
int UnionShortConnSJL06CmdAnyway(PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);
int UnionLongConnSJL06CmdAnyway(int sckHDL,PUnionSJL06 pSJL06,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

#endif

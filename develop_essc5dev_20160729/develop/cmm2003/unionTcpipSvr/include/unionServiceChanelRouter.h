// Author:	Wolfgang Wang
// Date:	2011/1/23


#ifndef _unionServiceChanelRouter_
#define _unionServiceChanelRouter_

/*
功能
	获取服务通道标识
输入参数
	reqStr		服务请求
	lenOfReqStr	请求串长度
输出参数
	无
返回值
	>=0		服务通道的标识
	<0		错误代码
*/
long UnionGetServiceChanelID(char *reqStr,int lenOfReqStr);

#endif

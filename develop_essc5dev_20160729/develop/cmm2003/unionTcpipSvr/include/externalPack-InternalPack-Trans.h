// author	Wolfgang Wang
// date		2010-10-25

#ifndef _externalPack_InternalPack_Trans_
#define _externalPack_InternalPack_Trans_

/*
功能
	将外部请求报文转换为内部请求报文
输入参数
	exteranlReqPack		外部请求报文
	lenOfExternalReqPack	外部请求报文长度
	sizeOfInteranlReqPack	内部请求报文缓冲大小
输出参数
	interanlReqPack		内部请求报文
返回值
	>=0			内部请求报文长度
	<0			错误代码
*/
int UnionTransExternalReqPackIntoInternalReqPack(unsigned char *exteranlReqPack,int lenOfExternalReqPack,
		unsigned char *interanlReqPack,int sizeOfInteranlReqPack);

/*
功能
	将内部响应报文转换为外部响应报文
输入参数
	interanlResPack		内部响应报文
	lenOfInternalResPack	内部响应报文长度
	sizeOfExteranlResPack	外部响应报文缓冲大小
输出参数
	exteranlResPack		外部响应报文
返回值
	>=0			外部响应报文长度
	<0			错误代码
*/
int UnionTransInternalResPackIntoExternalResPack(unsigned char *interanlResPack,int lenOfInternalResPack,unsigned char *exteranlResPack,
		int sizeOfExteranlResPack);

#endif

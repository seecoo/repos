#ifndef _UnionCertErrCode_
#define _UnionCertErrCode_

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifndef errCodeOffsetOfCertMDL	
#define errCodeOffsetOfCertMDL -69000	
#endif

#define errCodeOffsetOfCertMDL_FileNotExist			(errCodeOffsetOfCertMDL-1)	 //文件不存在
#define errCodeOffsetOfCertMDL_ReadFile					(errCodeOffsetOfCertMDL-2)		//读文件失败
#define errCodeOffsetOfCertMDL_WriteFile				(errCodeOffsetOfCertMDL-3)		//写文件失败
#define errCodeOffsetOfCertMDL_FileFormat				(errCodeOffsetOfCertMDL-4)		//文件格式不匹配
#define errCodeOffsetOfCertMDL_VerifyCertReq		(errCodeOffsetOfCertMDL-5)		//验证证书请求失败
#define errCodeOffsetOfCertMDL_VerifyCert				(errCodeOffsetOfCertMDL-6)		//验证证书失败
#define errCodeOffsetOfCertMDL_Passwd						(errCodeOffsetOfCertMDL-7)	  //密码不正确
#define	errCodeOffsetOfCertMDL_Arithmetic				(errCodeOffsetOfCertMDL-8)		//运算过程失败
#define	errCodeOffsetOfCertMDL_SmallBuffer			(errCodeOffsetOfCertMDL-9)		//太小的缓冲区
#define errCodeOffsetOfCertMDL_CodeParameter		(errCodeOffsetOfCertMDL-10)		//参数错
#define errCodeOffsetOfCertMDL_Sign							(errCodeOffsetOfCertMDL-11)		//签名失败
#define errCodeOffsetOfCertMDL_VerifySign				(errCodeOffsetOfCertMDL-12)		//验证签名失败
#define errCodeOffsetOfCertMDL_OpenSSl					(errCodeOffsetOfCertMDL-13)		//openssl调用出错
#define errCodeOffsetOfCertMDL_PKNotMatchVK			(errCodeOffsetOfCertMDL-14)		//rsa公私钥不匹配

#ifndef errCodeNullPointer
#define errCodeNullPointer			(errCodeOffsetOfCertMDL-15)		//空指针
#endif
#endif


#ifndef _UnionCertErrCode_
#define _UnionCertErrCode_

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifndef errCodeOffsetOfCertMDL	
#define errCodeOffsetOfCertMDL -69000	
#endif

#define errCodeOffsetOfCertMDL_FileNotExist			(errCodeOffsetOfCertMDL-1)	 //�ļ�������
#define errCodeOffsetOfCertMDL_ReadFile					(errCodeOffsetOfCertMDL-2)		//���ļ�ʧ��
#define errCodeOffsetOfCertMDL_WriteFile				(errCodeOffsetOfCertMDL-3)		//д�ļ�ʧ��
#define errCodeOffsetOfCertMDL_FileFormat				(errCodeOffsetOfCertMDL-4)		//�ļ���ʽ��ƥ��
#define errCodeOffsetOfCertMDL_VerifyCertReq		(errCodeOffsetOfCertMDL-5)		//��֤֤������ʧ��
#define errCodeOffsetOfCertMDL_VerifyCert				(errCodeOffsetOfCertMDL-6)		//��֤֤��ʧ��
#define errCodeOffsetOfCertMDL_Passwd						(errCodeOffsetOfCertMDL-7)	  //���벻��ȷ
#define	errCodeOffsetOfCertMDL_Arithmetic				(errCodeOffsetOfCertMDL-8)		//�������ʧ��
#define	errCodeOffsetOfCertMDL_SmallBuffer			(errCodeOffsetOfCertMDL-9)		//̫С�Ļ�����
#define errCodeOffsetOfCertMDL_CodeParameter		(errCodeOffsetOfCertMDL-10)		//������
#define errCodeOffsetOfCertMDL_Sign							(errCodeOffsetOfCertMDL-11)		//ǩ��ʧ��
#define errCodeOffsetOfCertMDL_VerifySign				(errCodeOffsetOfCertMDL-12)		//��֤ǩ��ʧ��
#define errCodeOffsetOfCertMDL_OpenSSl					(errCodeOffsetOfCertMDL-13)		//openssl���ó���
#define errCodeOffsetOfCertMDL_PKNotMatchVK			(errCodeOffsetOfCertMDL-14)		//rsa��˽Կ��ƥ��

#ifndef errCodeNullPointer
#define errCodeNullPointer			(errCodeOffsetOfCertMDL-15)		//��ָ��
#endif
#endif


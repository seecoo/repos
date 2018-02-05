//	Author:		Wolfgang Wang
//	Date:		2006/8/9
//	Version:	1.0

#ifndef _mngSvrPassword_
#define _mngSvrPassword_

int UnionEncryptPassword(char *plainPassword,char *encryptPassword);

int UnionDecryptPassword(char *encryptPassword,char *plainPassword);

#endif


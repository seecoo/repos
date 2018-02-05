//	Author:		Wolfgang Wang
//	Date:		2006/8/9
//	Version:	1.0

#include <string.h>
#include <stdio.h>
#include "UnionDes.h"
#include "UnionStr.h"
#include "mngSvrPassword.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionDataExch.h"
#include <stdlib.h>

char gunionPasswordKey[] = "B4011F49527A00964CE3A961D82A9B40";

int UnionEncryptPassword(char *plainPassword,char *encryptPassword)
{
	char	plainData[128+1];
        int     passwordLen;
        int     len,i;

	if ((passwordLen = strlen(plainPassword)) > 64 - 2)
        {
                UnionUserErrLog("in UnionEncryptPassword:: password too long [%d]!\n",passwordLen);
                return(errCodeParameter);
        }
        sprintf(plainData,"%02d",passwordLen);
        memset(plainData+2,'0',sizeof(plainData)-1-2);
        plainData[sizeof(plainData)-1] = 0;
        UnionUncompressBitsIntoBCD((unsigned char *)plainPassword,passwordLen,plainData+2);
        if ((len = passwordLen * 2 + 2) % 16 != 0)
                len = len / 16 * 16 + 16;
        for (i = 0; i < len; i = i + 16)
        {
	        Union3DesEncrypt64BitsText(gunionPasswordKey,plainData+i,encryptPassword+i);
        }
	//strcpy(encryptPassword,plainPassword);
	return(len);
}

int UnionDecryptPassword(char *encryptPassword,char *plainPassword)
{
	char	plainData[128+1];
	int     len,i;
        char    tmpChar;
        int     passwordLen;

	if (((len = strlen(encryptPassword)) % 16 != 0) || (len >= sizeof(plainData)))
	{
		UnionUserErrLog("in UnionDecryptPassword:: encryptPassword = [%s] length error!\n",encryptPassword);
		return(errCodeEsscMDL_WrongPasswordLength);
	}
        for (i = 0; i < len; i = i + 16)
        {
        	Union3DesDecrypt64BitsText(gunionPasswordKey,encryptPassword+i,plainData+i);
        }
        tmpChar = plainData[2];
        plainData[2] = 0;
        passwordLen = atoi(plainData);
        if (passwordLen * 2 > len - 2)
	{
		UnionUserErrLog("in UnionDecryptPassword:: passwordLen = [%d] length error!\n",passwordLen);
		return(errCodeEsscMDL_WrongPasswordLength);
	}
        plainData[2] = tmpChar;
        UnionCompressBCDIntoBits(plainData+2,passwordLen * 2,(unsigned char *)plainPassword);
        return(passwordLen);
}


/*
Author:	zhangyongding
Date:	20081223
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

char gunionKeyEncryptDBName[] = "22605AF8436F2756FB32D94447B97FB3";


int UnionEncryptDatabasePassword(char *plainPassword,char *ciperPassword)
{
	char	tmpBuf[136];
	int	pinLen;
	int	i = 0;
	int	blockNum = 0;
	
	//if ((pinLen = strlen(plainPassword)) > 15)
	//	pinLen = 15;
	pinLen = strlen(plainPassword);
	if (pinLen > 63)
		return(errCodeParameter);

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%02d",pinLen);
	bcdhex_to_aschex(plainPassword,pinLen,tmpBuf+2);
	//memset(tmpBuf+2+pinLen*2,'F',32-2-pinLen*2);

	// modify by leipp 20150414
	if (pinLen <= 15)
	{
		blockNum = 2;	
	}
	else if (pinLen > 15 && pinLen < 24)
	{
		blockNum = 3;
	}
	else
	{
		blockNum = 4;
	}

	memset(tmpBuf+2+pinLen*2,'F',blockNum*16*2-2-pinLen*2);

	for (i = 0; i < blockNum; i++)
		Union3DesEncrypt64BitsText(gunionKeyEncryptDBName,tmpBuf+i*16,ciperPassword+i*16);
	ciperPassword[blockNum*16] = 0;
	// modify end

	//Union3DesEncrypt64BitsText(gunionKeyEncryptDBName,tmpBuf,ciperPassword);
	//Union3DesEncrypt64BitsText(gunionKeyEncryptDBName,tmpBuf+16,ciperPassword+16);
	//printf("[%s][%s][%s]\n",plainPassword,tmpBuf,ciperPassword);
	return(0);
}

int UnionDecryptDatabasePassword(char *ciperPassword,char *plainPassword)
{
	// 最大支持63个密码明文
	char	tmpBuf[136];
	int	pinLen;
	int     i = 0;
	int     blockNum = 0;

	// modify by leipp 20150414 begin
	blockNum = strlen(ciperPassword) / 16;

	memset(tmpBuf,0,sizeof(tmpBuf));
	for (i = 0; i < blockNum; i++)
	{
		Union3DesDecrypt64BitsText(gunionKeyEncryptDBName,ciperPassword+i*16,tmpBuf+i*16);
	}
	// modify by leipp end

	//memset(tmpBuf,0,sizeof(tmpBuf));
	//Union3DesDecrypt64BitsText(gunionKeyEncryptDBName,ciperPassword,tmpBuf);
	//Union3DesDecrypt64BitsText(gunionKeyEncryptDBName,ciperPassword+16,tmpBuf+16);
	pinLen = (tmpBuf[0] - '0') * 10 + tmpBuf[1] - '0';
	aschex_to_bcdhex(tmpBuf+2,pinLen*2,plainPassword);
	plainPassword[pinLen] = 0;
	//printf("[%s][%s][%s]\n",ciperPassword,tmpBuf,plainPassword);
	return(0);
}

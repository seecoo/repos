//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionDesKey.h"
#include "unionSJJ1127Cmd.h"
#include "commWithHsmSvr.h"

// 算法标识，1，DES算法，2，SM1算法
int gunionAlgorithmFlag = 1;

// 设置使用DES算法
int UnionSetUseDesAlgorithm()
{
	gunionAlgorithmFlag = 1;
	return(0);
}

// 设置使用SM1算法
int UnionSetUseSM1Algorithm()
{
	gunionAlgorithmFlag = 2;
	return(0);
}

// 转换密钥长度，返回密钥的长度
int UnionTranslateSJJ1127HsmKeyLength(TUnionDesKeyLength length,char *lenStr)
{       
	switch (length)
	{
		case    con64BitsDesKey:
		case    64:
		case    16:
			memcpy(lenStr,"01",2);
			return(16);
		case    con128BitsDesKey:
		case    128:
		case    32:
			memcpy(lenStr,"02",2);
			return(32);
		case    con192BitsDesKey:
		case    192:
		case    48:
			memcpy(lenStr,"03",2);
			return(64);
		default:
			UnionUserErrLog("in UnionTranslateSJJ1127HsmKeyLength:: invalid key length [%d]!\n",length);
			return(errCodeParameter);
	}
}

// 填充加密机指令代码
int UnionFillSJJ1127HsmCommandHeader(char *oldCmd,int lenOfOldCmd,char *newCmd)
{
	int	offset = 0;
		
	if (gunionAlgorithmFlag == 1)
	{
		// 如果第一个字符是"!"，则不做填充，并去掉"!"
		if (memcmp(oldCmd,"!",1) == 0)
		{
			memcpy(newCmd,oldCmd+1,lenOfOldCmd - 1);
			return(lenOfOldCmd - 1);
		}

		offset = 2;
		if (memcmp(oldCmd,"50",2) == 0)
			memcpy(newCmd,"A0",2);
		else if (memcmp(oldCmd,"10",2) == 0)
			memcpy(newCmd,"B0",2);
		else if (memcmp(oldCmd,"12",2) == 0)
			memcpy(newCmd,"B0",2);
		else
			memcpy(newCmd,"00",2);
	}
	else if (gunionAlgorithmFlag == 2)
	{
		// 如果第一个字符是"!"，则不做填充，并去掉"!"
		if (memcmp(oldCmd,"!",1) == 0)
		{
			memcpy(newCmd,oldCmd+1,lenOfOldCmd - 1);
			return(lenOfOldCmd - 1);
		}

		offset = 2;
		if ((memcmp(oldCmd,"00",2) == 0) ||
			(memcmp(oldCmd,"02",2) == 0) ||
			(memcmp(oldCmd,"04",2) == 0) ||
			(memcmp(oldCmd,"62",2) == 0))
		{
			memcpy(newCmd,"00",2);
		}
		else if (memcmp(oldCmd,"10",2) == 0)
			memcpy(newCmd,"B0",2);
		else if (memcmp(oldCmd,"12",2) == 0)
			memcpy(newCmd,"B0",2);
		else
			memcpy(newCmd,"S0",2);
	}

	// 设置加密机指令头的长度为4
	UnionSetLenOfHsmCmdHeader(4);
	
	memcpy(newCmd + offset, oldCmd, lenOfOldCmd);
	return(lenOfOldCmd + offset);
}

/*      
函数功能：
	18指令，对指定密钥进行转加密
输入参数：      
	transformID：转换方式
	oriPKIndex: 源保护密钥索引
	lenOfOriPK: 源保护密钥长度
	oriPKValue: 源保护密钥
	desPKIndex: 目的保护密钥索引
	lenOfDesPK: 目的保护密钥长度
	desPKValue: 目的保护密钥
	lenOfOriKey：待转加密的密钥长度
	oriKeyValue：待转加密的密钥密文
	oriKeyCheckValue：密钥校验值
		
输出参数：	      
	desKeyValue：转加密后密文
	desKeyCheckValue：密钥校验值
*/	      
int UnionSJJ1127Cmd18(char *transformID,int oriPKIndex,TUnionDesKeyLength lenOfOriPK,char *oriPKValue,int desPKIndex,TUnionDesKeyLength lenOfDesPK,char *desPKValue,TUnionDesKeyLength lenOfOriKey,char *oriKeyValue,char *oriKeyCheckValue,char *desKeyValue,char *desKeyCheckValue)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	char    tmpBuf[128+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
	if ((ret = UnionFillSJJ1127HsmCommandHeader("18",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd18:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// 转换方式
	memcpy(hsmCmdBuf+hsmCmdLen, transformID, 2);
	hsmCmdLen += 2;

	if ((memcmp(transformID,"02",2) == 0) ||
		(memcmp(transformID,"03",2) == 0))
	{
		// 源保护密钥索引
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", oriPKIndex);
		memset(tmpBuf,0,sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
		hsmCmdLen += 4;

		if (memcmp(tmpBuf,"FFFF",4) == 0)
		{
			// 源保护密钥长度
			len = UnionTranslateSJJ1127HsmKeyLength(lenOfOriPK,hsmCmdBuf+hsmCmdLen);
			hsmCmdLen += 2;
			// 源保护密钥
			memcpy(hsmCmdBuf+hsmCmdLen, oriPKValue, len);
			hsmCmdLen += len;
		}
	}

	if ((memcmp(transformID,"01",2) == 0) ||
		(memcmp(transformID,"03",2) == 0))
	{
		// 目的保护密钥索引
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", desPKIndex);
		memset(tmpBuf,0,sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
		hsmCmdLen += 4;

		if (memcmp(tmpBuf,"FFFF",4) == 0)
		{
			// 目的保护密钥长度
			len = UnionTranslateSJJ1127HsmKeyLength(lenOfDesPK,hsmCmdBuf+hsmCmdLen);
			hsmCmdLen += 2;
			// 目的保护密钥
			memcpy(hsmCmdBuf+hsmCmdLen, desPKValue, len);
			hsmCmdLen += len;
		}
	}

	// 待转加密的密钥长度
	len = UnionTranslateSJJ1127HsmKeyLength(lenOfOriKey,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 2;

	// 待转加密的密钥密文
	memcpy(hsmCmdBuf+hsmCmdLen, oriKeyValue, len);
	hsmCmdLen += len;

	// 密钥校验值
	memcpy(hsmCmdBuf+hsmCmdLen, oriKeyCheckValue, 16);
	hsmCmdLen += 16;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd18:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	// 转加密后密文
	memcpy(desKeyValue, hsmCmdBuf+offset, len);
	offset += len;

	// 密钥校验值
	memcpy(desKeyCheckValue, hsmCmdBuf+offset, 16);
	offset += 16;

	return(0);
}


/*
函数功能：
	50指令，用指定密钥对指定数据进行加解密
输入参数：
	algorithmID：算法标识
		值00- SM1
		值 01- DES ECB
		值 02- DES CBC
		值03- DES CFB
		值04- DES OFB
	encryptID：加/解密标志，1：加密；0：解密；
	indexOfKey: 密钥索引号
	lenOfKey: 密钥长度
	keyValue: 密钥密文
	iv：初始向量
	lenOfData: 数据块长度
	data：数据块
输出参数：
	lenOfResData: 数据块长度
	resData: 数据块
*/
int UnionSJJ1127Cmd50(char *algorithmID,char *encryptID,int indexOfKey,TUnionDesKeyLength lenOfKey,char *keyValue,char *iv,int lenOfData,char *data,int *lenOfResData,char *resData)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	char    tmpBuf[128+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
	if ((ret = UnionFillSJJ1127HsmCommandHeader("50",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd50:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// 算法标识
	memcpy(hsmCmdBuf+hsmCmdLen, algorithmID, 2);
	hsmCmdLen += 2;

	// 算法标识
	memcpy(hsmCmdBuf+hsmCmdLen, encryptID, 1);
	hsmCmdLen += 1;

	// 密钥索引号
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", indexOfKey);
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
	hsmCmdLen += 4;

	// 密钥密文
	if (memcmp(tmpBuf,"FFFF",4) == 0)
	{
		// 密钥长度
		len = UnionTranslateSJJ1127HsmKeyLength(lenOfKey,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += 2;

		memcpy(hsmCmdBuf+hsmCmdLen, keyValue, len);
		hsmCmdLen += len;
	}

	// 初始向量
	if ((memcmp(algorithmID,"02",2) == 0) ||		// DES CBC
		(memcmp(algorithmID,"03",2) == 0) ||	    // DES CFB
		(memcmp(algorithmID,"04",2) == 0))	      // DES OFB
	{
		memcpy(hsmCmdBuf+hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	// 数据块长度
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", lenOfData / 2);
	hsmCmdLen += 4;

	// 数据块
	memcpy(hsmCmdBuf+hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;
	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd50:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	// 随机密钥
	memcpy(tmpBuf, hsmCmdBuf+offset, 4);
	offset += 4;
	tmpBuf[4] = 0;
	sscanf(tmpBuf, "%X", lenOfResData);
	ret = (*lenOfResData) *2;
	// 数据块
	memcpy(resData, hsmCmdBuf+offset, ret);
	return(ret);
}



/*
函数功能：
	60指令，生成并打印密钥
输入参数：
	lenOfKey: 密钥长度标志
	numOfComponent: 成份数
	keyName：密钥名称
输出参数：
	keyValue: 随机密钥
	keyCheckValue: 密钥的校验值
*/
int UnionSJJ1127Cmd60(TUnionDesKeyLength lenOfKey,int numOfComponent,char *keyName,char *keyValue,char *keyCheckValue)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
	if ((ret = UnionFillSJJ1127HsmCommandHeader("60",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd60:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// 密钥长度
	len = UnionTranslateSJJ1127HsmKeyLength(lenOfKey,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 2;

	// 成份数
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",numOfComponent);
	hsmCmdLen += 1;

	// 附加信息
	memcpy(hsmCmdBuf+hsmCmdLen, keyName, strlen(keyName));
	hsmCmdLen += strlen(keyName);

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd60:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	// 随机密钥
	memcpy(keyValue, hsmCmdBuf+offset, len);
	offset += len;

	// 密钥的校验值
	memcpy(keyCheckValue, hsmCmdBuf+offset, 16);
	offset += 16;
	return(len);
}


/*
函数功能：
	62指令，装载打印格式
输入参数：
	formatType: 格式类型
	format: 打印格式
输出参数：
	无
*/
int UnionSJJ1127Cmd62(char *formatType,char *format)
{
	int     ret;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
	if ((ret = UnionFillSJJ1127HsmCommandHeader("62",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd62:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// 格式类型
	memcpy(hsmCmdBuf+hsmCmdLen, formatType, 1);
	hsmCmdLen += 1;

	// 格式长度
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",(int)strlen(format));
	hsmCmdLen += 4;

	// 密钥校验值
	memcpy(hsmCmdBuf+hsmCmdLen, format, strlen(format));
	hsmCmdLen += strlen(format);

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd62:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}


/*
函数功能：
	16指令，读取指定密钥索引的密钥
输入参数：
	indexOfKey: 密钥索引号
输出参数：
	keyValue: 随机密钥
	keyCheckValue: 密钥的校验值
返回值：
	<0，函数执行失败
	=0，成功
*/
int UnionSJJ1127Cmd16(int indexOfKey,char *keyValue,char *keyCheckValue)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	char    tmpBuf[128+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
	if ((ret = UnionFillSJJ1127HsmCommandHeader("16",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd16:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// 密钥索引号
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X",indexOfKey);
	hsmCmdLen += 4;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd16:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;

	// 密钥长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+offset, 2);
	len = atoi(tmpBuf) * 16;
	offset += 2;

	// 随机密钥
	memcpy(keyValue, hsmCmdBuf+offset, len);
	offset += len;

	// 密钥的校验值
	memcpy(keyCheckValue, hsmCmdBuf+offset, 16);
	offset += 16;

	return(len);
}

/*      
函数功能：
        64指令，用指定密钥对离散数据衍生密钥
输入参数：
        mkIndex:	根密钥索引
		FFFF：使用指令中带入的根密钥   
		0000-03E8：密钥索引    
		其他：非法
        lenOfMK:	根密钥长度
		仅当密钥索引为FFFF时有此域
		值01- 64bits   
		值02- 128bits    
		值03- 192bits
        mk:		根密钥
		仅当密钥索引为FFFF时有此域
		MK加密下的根密钥的密文
        mkDvsNum:	离散次数
        mkDvsData:	离散数据
	
输出参数： 
	
        criperData:	输出的密文数据 
		MK加密下的过程密钥/子密钥密文
        keyCheckValue:	密钥的校验值
		过程密钥/子密钥的校验值
*/     
int UnionSJJ1127Cmd64(int mkIndex,TUnionDesKeyLength lenOfMK,char *mk, int mkDvsNum, char *mkDvsData, char *criperData, char *keyCheckValue)
{
        int     ret;
        int     lenOfDvsData = 16;
        int     len = 0;
        char    hsmCmdBuf[1024*8+1];
        int     hsmCmdLen = 0;
        char    tmpBuf[4+1];
        int     offset = 0;

        memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
        if ((ret = UnionFillSJJ1127HsmCommandHeader("64",2,hsmCmdBuf)) < 0)
	{
	        UnionUserErrLog("in UnionSJJ1127Cmd64:: UnionFillSJJ1127HsmCommandHeader!\n");
	        return(ret);
	}
        hsmCmdLen += 4;

	// 根密钥索引
        sprintf(hsmCmdBuf+hsmCmdLen, "%04X", mkIndex);        
        memset(tmpBuf,0,sizeof(tmpBuf));
        memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);  
        hsmCmdLen += 4;      

        if (memcmp(tmpBuf,"FFFF",4) == 0)
	{
		// 根密钥长度
	        len = UnionTranslateSJJ1127HsmKeyLength(lenOfMK,hsmCmdBuf+hsmCmdLen);
	        hsmCmdLen += 2;
		// 根密钥
	        memcpy(hsmCmdBuf+hsmCmdLen, mk, len);
	        hsmCmdLen += len;
	}
	
	// 离散次数
        sprintf(hsmCmdBuf+hsmCmdLen, "%02d", mkDvsNum);
        hsmCmdLen += 2;

	// 离散数据
        memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
        hsmCmdLen += mkDvsNum*lenOfDvsData;
        
	// 与密码机通讯
        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
	        UnionUserErrLog("in UnionSJJ1127Cmd64:: UnionDirectHsmCmd!\n");
	        return(ret);
	}
        offset = 6;
	
	// 密文数据
	memcpy(criperData, hsmCmdBuf+offset, 32);

	// 密钥的校验值
	offset += 32;
        memcpy(keyCheckValue, hsmCmdBuf+offset, 16);        
        return(0);
}

/*
函数功能：
	14指令，将密钥密文写入指定密钥索引
输入参数：
	lenOfKey: 密钥长度标志
	keyValue: 随机密钥
	keyCheckValue: 密钥的校验值
	indexOfKey: 密钥索引号
输出参数：
	无
*/
int UnionSJJ1127Cmd14(TUnionDesKeyLength lenOfKey,char *keyValue,char *keyCheckValue,int indexOfKey)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
	if ((ret = UnionFillSJJ1127HsmCommandHeader("14",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd14:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// 密钥长度
	len = UnionTranslateSJJ1127HsmKeyLength(lenOfKey,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 2;

	// 密钥密文
	memcpy(hsmCmdBuf+hsmCmdLen, keyValue, len);
	hsmCmdLen += len;

	// 密钥校验值
	memcpy(hsmCmdBuf+hsmCmdLen, keyCheckValue, 16);
	hsmCmdLen += 16;

	// 密钥索引号
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X",indexOfKey);
	hsmCmdLen += 4;

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd14:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}


/*
函数功能：
	90指令，生成随机的密钥
输入参数：
	lenOfKey: 密钥长度标志
	indexOfKey: 密钥索引号
输出参数：
	keyValue: 随机密钥
	keyCheckValue: 密钥的校验值
*/
int UnionSJJ1127Cmd90(TUnionDesKeyLength lenOfKey,int indexOfKey,char *keyValue,char *keyCheckValue)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
	if ((ret = UnionFillSJJ1127HsmCommandHeader("90",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd90:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// 密钥长度
	len = UnionTranslateSJJ1127HsmKeyLength(lenOfKey,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 2;

	// 密钥索引号
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", indexOfKey);
	hsmCmdLen += 4;
	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd90:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	// 随机密钥
	memcpy(keyValue, hsmCmdBuf+offset, len);
	offset += len;

	// 密钥的校验值
	memcpy(keyCheckValue, hsmCmdBuf+offset, 16);
	offset += 16;

	return(len);
}


/*
函数功能：
	U2指令，使用指定的应用主密钥进行2次离散得到卡片应用子密钥，
		使用指定控制密钥子密钥进行加密保护输出并进行MAC计算。
输入参数：
	securityMech: 安全机制(S：单DES加密和MAC, T：三DES加密和MAC)
	mode: 模式标志, 0-仅加密 1-加密并计算MAC
	id: 方案ID, 0=M/Chip4(CBC模式，强制填充X80) 1=VISA/PBOC(带长度指引的ECB) 2=PBOC1.0模式(ECB模式，外带填充数据)

	mkIndex:	FFFF：使用指令中带入的根密钥
			0000-03E8：密钥索引
			其他：非法
	lenOfMK:	仅当根密钥索引为FFFF时有此域。
			值01- 64bits
			值02- 128bits
			值03- 192bits
	mk:	     MK加密下的根密钥的密文
	mkDvsNum: 根密钥离散次数, 1-3次
	mkDvsData: 根密钥离散数据, n*16H, n代表离散次数

	pkIndex:	FFFF：使用指令中带入的保护密钥
			0000-03E8：密钥索引
			其他：非法
	lenOfPK:	仅当保护密钥索引为FFFF时有此域。
			值01- 64bits
			值02- 128bits
			值03- 192bits
	pk:	     MK加密下的保护密钥的密文
	pkDvsNum: 保护密钥离散次数,仅当“保护密钥类型”为1时有,从MK-SMC离散得到DK-SMC的离散次数,范围为1-3
	pkDvsData: 保护密钥离散数据,n*16H仅当“保护密钥类型”为1时有,保护密钥的离散数据，其中n为“保护密钥离散次数”

	proKeyFlag:过程密钥标识，Y:计算过程密钥　N:不计算过程密钥　可选项:当没有该域时缺省为N
	proFactor: 过程因子(16H),可选项:仅当过程密钥标志为Y时有

	ivCbc: IV-CBC,8H 仅当“方案ID”为0时有

	encryptFillDataLen: 加密填充数据长度(4H),仅当“方案ID”为2时有（不大于1024）
		和密钥明文一起进行加密的数据长度
	encryptFillData: 加密填充数据 nB 仅当“方案ID”为2时有,和密钥明文一起进行加密
	encryptFillOffset: 加密填充数据偏移量 4H 仅当“方案ID”为2时有
		将密钥明文插入到加密填充数据的位置, 数值必须在0到加密填充数据长度之间

	ivMac: IV-MAC,16H 仅当“模式标志”为1时有
	macDataLen: MAC填充数据长度 4H 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据长度
	macData: MAC填充数据 nB 仅当“模式标志”为1时有,和密钥密文一起进行MAC计算的数据
	macOffset: 偏移量 4H 仅当“模式标志”为1时有,将密钥密文插入到MAC填充数据的位置
		数值必须在0到MAC填充数据长度之间
输出参数：
	mac: MAC值 8B 仅当“模式标志”为1时有
	criperDataLen: 密文数据长度 4H 密文数据长度(必须是8的倍数，并等于前缀长度、后缀长度、密钥长度的和)
	criperData: nB 输出的密文数据
返回值：
	<0，函数执行失败
	=0，成功
*/
int UnionSJJ1127CmdU2(char *securityMech, char *mode, char *id, int mkIndex,TUnionDesKeyLength lenOfMK,char *mk,
	int mkDvsNum, char *mkDvsData, int pkIndex, TUnionDesKeyLength lenOfPK, char *pk,
	int pkDvsNum, char *pkDvsData, char *proKeyFlag, char *proFactor, char *ivCbc,
	int encryptFillDataLen, char *encryptFillData, int encryptFillOffset,
	char *ivMac, int macDataLen, char *macData, int macOffset,
	char *mac, int *criperDataLen, char *criperData)
{
	int     ret;
	int     lenOfDvsData = 16;
	int     len = 0;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	char    tmpBuf[4+1];
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// 命令代码
	if ((ret = UnionFillSJJ1127HsmCommandHeader("U2",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127CmdU2:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// 安全机制
	memcpy(hsmCmdBuf+hsmCmdLen, securityMech, 1);
	hsmCmdLen += 1;

	// 模式标识
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// 方案ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// 根密钥索引
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", mkIndex);
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
	hsmCmdLen += 4;

	if (memcmp(tmpBuf,"FFFF",4) == 0)
	{
		// 根密钥长度
		len = UnionTranslateSJJ1127HsmKeyLength(lenOfMK,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += 2;
		// 根密钥
		memcpy(hsmCmdBuf+hsmCmdLen, mk, len);
		hsmCmdLen += len;
	}

	// 离散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// 离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
	hsmCmdLen += mkDvsNum*lenOfDvsData;

	// 保护根密钥索引
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", pkIndex);
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
	hsmCmdLen += 4;

	if (memcmp(tmpBuf,"FFFF",4) == 0)
	{
		// 保护根密钥长度
		len = UnionTranslateSJJ1127HsmKeyLength(lenOfPK,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += 2;
		// 保护根密钥
		memcpy(hsmCmdBuf+hsmCmdLen, pk, len);
		hsmCmdLen += len;
	}

	// 保护密钥离散次数
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
	hsmCmdLen += 1;

	// 保护密钥离散数据
	memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*lenOfDvsData);
	hsmCmdLen += pkDvsNum*lenOfDvsData;

	// 过程密钥标识
	if( (proKeyFlag != NULL) && (strlen(proKeyFlag) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, proKeyFlag, 1);
		hsmCmdLen += 1;
		// 过程因子
		if (proKeyFlag[0] == 'Y')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, proFactor, lenOfDvsData);
			hsmCmdLen += lenOfDvsData;
		}
	}

	// IV-CBC
	if (id[0] == '0')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 8);
		hsmCmdLen += 8;
	}

	// 加密填充数据长度、加密填充数据、加密填充数据偏移量
	if (id[0] == '2')
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, encryptFillData, encryptFillDataLen);
		hsmCmdLen += encryptFillDataLen;

		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillOffset/2);
		hsmCmdLen += 4;
	}

	// IV-MAC, MAC填充数据长度, MAC填充数据, 偏移量
	if (mode[0] == '1')
	{
		if (strlen(ivMac) > 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, ivMac, 16);
			hsmCmdLen += 16;
		}

		if (macDataLen > 8000)
		{
			UnionUserErrLog("in UnionSJJ1127CmdU2::macDataLen[%d] too large\n", macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, macData, macDataLen);
		hsmCmdLen += macDataLen;

		if ((macOffset < 0) || (macOffset > macDataLen))
		{
			UnionUserErrLog("in UnionSJJ1127CmdU2::macOffset[%d] <0 or >macDataLen[%d]\n", macOffset, macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macOffset/2);
		hsmCmdLen += 4;
	}

	// 与密码机通讯
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127CmdU2:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	if (mode[0] == '1') // MAC值
	{
		memcpy(mac, hsmCmdBuf+offset, 16);
		offset += 16;
	}

	// 密文数据长度
	offset += 4;
	*criperDataLen = ret - offset;

	// 密文数据
	memcpy(criperData, hsmCmdBuf+offset, *criperDataLen);
	UnionLog("in UnionSJJ1127CmdU2::*criperDataLen=[%d]\n",*criperDataLen);
	return(0);
}




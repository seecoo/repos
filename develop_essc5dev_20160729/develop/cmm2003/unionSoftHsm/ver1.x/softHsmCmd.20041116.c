//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#include <stdio.h>
#include <string.h>

#include "UnionAlgorithm.h"
#include "softHsmKeySpace.h"
#include "UnionLog.h"
#include "UnionStr.h"

/* 60指令的格式
输  入  域	长  度	类  型	内      容
命      令	2	A	值"60"
PIN  类型	1	N	'1'-'7'
银行主密钥索引	3	N	银行主密钥索引
PIK  密钥	16	H	用银行主密钥加密的PIN密钥
明    文	16	H	要加密的PIN 明文
PAN	16	H	仅当PIN类型为3或7时有此域


输  出  消  息  格  式

输  出  域	长  度	类  型	内      容
应  答  码	2	A	"61"
错  误  码	2	A	错  误  码
加密后的PIN	16	H	用PIK密钥加密后的PIN块
*/

int SoftHsmCmd60(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	bmkIndex[3+1];
	char	*bmk;
	char	wk[16+1];
	char	pinAfterXOR[16+1];
	char	pinBlock[16+1];
	char	errCode[2+1];
	
	if (lenOfReqStr < 2 + 1 + 3 + 16 + 16 + 16)
	{
		UnionUserErrLog("in SoftHsmCmd60:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if (lenOfReqStr > 2 + 1 + 3 + 16 + 16 + 16)
	{
		UnionUserErrLog("in SoftHsmCmd60:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"60",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmd60:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"60",2);
		goto exitNormally;	
	}
	memcpy(bmkIndex,reqStr+2+1,3);
	bmkIndex[3] = 0;
	if (((bmk = UnionReadSoftHsmBMK(hsmGrp,bmkIndex)) == NULL) || (strlen(bmk) != 32))
	{
		UnionUserErrLog("in SoftHsmCmd60:: bmkIndex not defined [%s]\n",bmkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	memset(wk,0,sizeof(wk));
	Union3DesDecrypt64BitsText(bmk,reqStr+2+1+3,wk);
	memset(pinAfterXOR,0,sizeof(pinAfterXOR));
	UnionXOR(reqStr+2+1+3+16,reqStr+2+1+3+16+16,16,pinAfterXOR);
	memset(pinBlock,0,sizeof(pinBlock));
	Union1DesEncrypt64BitsText(wk,pinAfterXOR,pinBlock);
	UnionLog("in SoftHsmCmd60:: \nbmk = [%s]\nwk=[%s]\npinAfterXOR=[%s]\npinBlock=[%s]\n",
		bmk,wk,pinAfterXOR,pinBlock);
	memcpy(errCode,"00",2);
exitNormally:
	memcpy(resStr,"61",2);
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	memcpy(resStr+2+2,pinBlock,16);
	return(20);
}
/*
1.3  用PIK密钥解密一个PIN 68

输  入  消  息  格  式

输  入  域	长  度	类  型	内      容
命    令	2	A	值"68"
银行索引号	3	N	银行主密钥索引号
PIK	16	H	用它解密PIN
PIN 的密文	16	H	将要解密的PIN 的密文
附加信息	16	H	附加信息

输  出  消  息  格  式

输  出  域	长  度	类  型	内      容
应  答  码	2	A	"69"
错  误  码	2	A	错  误  码
PIN明码	16	H	


*/
int SoftHsmCmd68(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	bmkIndex[3+1];
	char	*bmk;
	char	wk[16+1];
	char	pinAfterXOR[16+1];
	char	pinText[16+1];
	char	errCode[2+1];
	
	if (lenOfReqStr < 2+3+16+16+16)
	{
		UnionUserErrLog("in SoftHsmCmd68:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if (lenOfReqStr > 2 + 3 + 16 + 16 + 16)
	{
		UnionUserErrLog("in SoftHsmCmd68:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"68",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmd68:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"60",2);
		goto exitNormally;	
	}
	memcpy(bmkIndex,reqStr+2,3);
	bmkIndex[3] = 0;
	if (((bmk = UnionReadSoftHsmBMK(hsmGrp,bmkIndex)) == NULL) || (strlen(bmk) != 32))
	{
		UnionUserErrLog("in SoftHsmCmd68:: bmkIndex not defined [%s]\n",bmkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	memset(wk,0,sizeof(wk));
	Union3DesDecrypt64BitsText(bmk,reqStr+2+3,wk);
	memset(pinAfterXOR,0,sizeof(pinAfterXOR));
	Union1DesDecrypt64BitsText(wk,reqStr+2+3+16,pinAfterXOR);
	memset(pinText,0,sizeof(pinText));
	UnionXOR(reqStr+2+3+16+16,pinAfterXOR,16,pinText);
	UnionLog("in SoftHsmCmd68:: \nbmk = [%s]\nwk=[%s]\npinAfterXOR=[%s]\npinText=[%s]\n",
		bmk,wk,pinAfterXOR,pinText);
	memcpy(errCode,"00",2);
exitNormally:
	memcpy(resStr,"69",2);
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	memcpy(resStr+2+2,pinText,16);
	return(20);
}
/*
1.4  用MAK密钥产生一个MAC 80
输  入  消  息  格  式

输  入  域	长  度	类  型	内      容
命    令	2	A	值"80"
银行索引号	3	N	加密MAK的银行主密钥索引号
MAK密码	16	H	用它产生MAC
MAC数据的长度	3	N	输入的MAC数据的字节数
输入MAC 数据	N	A	用来计算MAC的数据

输  出  消  息  格  式

输  出  域	长  度	类  型	内      容
应  答  码	2   	A	"81"
错  误  码	2	A	错  误  码
MAC	16	H	MAC
*/

int SoftHsmCmd80(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	bmkIndex[3+1];
	char	*bmk;
	char	wk[16+1];
	int	lenOfMacData;
	char	mac[16+1];
	char	errCode[2+1];
	char	macData[2048];
	
	if (lenOfReqStr < 2+3+16+3)
	{
		UnionUserErrLog("in SoftHsmCmd80:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if ((lenOfMacData = UnionConvertIntStringToInt(reqStr+2+3+16,3)) <= 0)
	{
		UnionUserErrLog("in SoftHsmCmd80:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"77",2);
		goto exitNormally;	
	}	
	if (lenOfReqStr < 2+3+16+3+lenOfMacData)
	{
		UnionUserErrLog("in SoftHsmCmd80:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if (lenOfReqStr > 2+3+16+3+lenOfMacData)
	{
		UnionUserErrLog("in SoftHsmCmd80:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"80",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmd80:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"80",2);
		goto exitNormally;	
	}
	memcpy(bmkIndex,reqStr+2,3);
	bmkIndex[3] = 0;
	if (((bmk = UnionReadSoftHsmBMK(hsmGrp,bmkIndex)) == NULL) || (strlen(bmk) != 32))
	{
		UnionUserErrLog("in SoftHsmCmd80:: bmkIndex not defined [%s]\n",bmkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	memset(wk,0,sizeof(wk));
	Union3DesDecrypt64BitsText(bmk,reqStr+2+3,wk);
	memset(mac,0,sizeof(mac));
        if (lenOfMacData * 2 > sizeof(macData))
	{
		UnionUserErrLog("in SoftHsmCmd80:: lenOfMacData [%d] too long\n",lenOfMacData);
		memcpy(errCode,"62",2);
		goto exitNormally;
	}
        bcdhex_to_aschex(reqStr+2+3+16+3,lenOfMacData,macData);
	//UnionGenerateANSIX99MAC(wk,reqStr+2+3+16+3,lenOfMacData,mac);
	UnionGenerateANSIX99MAC(wk,macData,lenOfMacData*2,mac);
	UnionLog("in SoftHsmCmd80:: \nbmk = [%s]\nwk=[%s]\nmac=[%s]\n",
		bmk,wk,mac);
	memcpy(errCode,"00",2);
exitNormally:
	memcpy(resStr,"81",2);
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	memcpy(resStr+2+2,mac,16);
	return(20);
}
/*
输入域	长度	类型	说　明
命令代码	2	A	值""
版本号	1	N	
组号	2	N	
索引号	2	N	
随机数A	16	H	
随机数B	16	H	
PIN密文	16	H	


输出域	长度	类型	说　明
响应代码	2	A	""
错误代码	2	H	
PIN明文	16	H	

1.	使用{版本、组号、索引号}指定的卡片主密钥对随机数A进行分散（分散采用PBOC算法），生成离散密钥K1（128位）。
2.	使用离散密钥K1加密随机数B，获得工作密钥K2（64位）。
3.	使用K2解密PIN密文，获得PIN明文。
*/
int SoftHsmCmdW1(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	*mpk;
	char	mpkIndex[10+1];
	char	dpk[32+1];
	char	sessionKey[16+1];
	char	pinBlock[16+1];
	char	errCode[2+1];
	
	//UnionLog("in SoftHsmCmdW1:: reqStr = [%s]\n",reqStr);
	if (lenOfReqStr < 2+1+2+2+16+16+16)
	{
		UnionUserErrLog("in SoftHsmCmdW1:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if (lenOfReqStr > 2+1+2+2+16+16+16)
	{
		UnionUserErrLog("in SoftHsmCmdW1:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"W1",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmdW1:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"60",2);
		goto exitNormally;	
	}
	//UnionLog("1.\n");
	if (((mpk = UnionReadSoftHsmICCardTransKey(hsmGrp,reqStr+2,reqStr+2+1,reqStr+2+1+2)) == NULL) || (strlen(mpk) != 32))
	{
		memset(mpkIndex,0,sizeof(mpkIndex));
		memcpy(mpkIndex,reqStr+2,5);
		UnionUserErrLog("in SoftHsmCmdW1:: mpkIndex not defined [%s]\n",mpkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	//UnionLog("2.\n");
	memset(dpk,0,sizeof(dpk));
	UnionGeneratePBOCDPK(mpk,reqStr+2+1+2+2,dpk);
	//UnionLog("3.\n");
	memset(sessionKey,0,sizeof(sessionKey));
	UnionGeneratePBOCSessionKey(dpk,reqStr+2+1+2+2+16,sessionKey);
	//UnionLog("4.\n");
	memset(pinBlock,0,sizeof(pinBlock));
	Union1DesDecrypt64BitsText(sessionKey,reqStr+2+1+2+2+16+16,pinBlock);
	UnionLog("in SoftHsmCmdW1:: \ndpk= [%s]\nsessionKey=[%s]\npinBlock=[%s]\n",
		dpk,sessionKey,pinBlock);
	memcpy(errCode,"00",2);
exitNormally:
	resStr[0] = reqStr[0];
	resStr[1] = reqStr[1] + 1;
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	memcpy(resStr+2+2,pinBlock,16);
	return(20);
}

/*
3  生成ANSI X9.9 MAC指令
输入域	长度	类型	说　明
命令代码	2	A	值"W2"
版本号	1	N	
组号	2	N	
索引号	2	N	
随机数A	16	H	
TAC数据长度	3	N	
TAC数据		A	

输出域	长度	类型	说　明
响应代码	2	A	""
错误代码	2	H	
TAC	16	H	

指令过程描述
1.	使用{版本、组号、索引号}指定的卡片主密钥对随机数A进行分散（分散采用PBOC算法），生成离散密钥K1（128位）。
2.	将离散密钥K1的左、右两部分异或，获得工作密钥K2（64位）。
3.	使用K2对TAC数据生成TAC（ANSI X9.9）。

测试数据：
输入参数：
卡片主密钥的明文：0F1345069DBA1650873B7594DBD0F906
随机数A：5882008000111111
TAC数据长度：048
TAC数据：00000064063030303130320002097420031212121110W200
输出参数：
TAC：53539467C2F5DFCA

*/
int SoftHsmCmdW2(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	*mpk;
	char	mpkIndex[10+1];
	char	dpk[32+1];
	char	sessionKey[16+1];
	char	mac[16+1];
	char	errCode[2+1];
	int	lenOfMacData;	
	
	//UnionLog("in SoftHsmCmdW2:: reqStr = [%s]\n",reqStr);
	if (lenOfReqStr < 2+1+2+2+16+3)
	{
		UnionUserErrLog("in SoftHsmCmdW2:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if ((lenOfMacData = UnionConvertIntStringToInt(reqStr+2+1+2+2+16,3)) <= 0)
	{
		UnionUserErrLog("in SoftHsmCmdW2:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"77",2);
		goto exitNormally;	
	}	
	if (lenOfReqStr < 2+1+2+2+16+3+lenOfMacData)
	{
		UnionUserErrLog("in SoftHsmCmdW2:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if (lenOfReqStr > 2+1+2+2+16+3+lenOfMacData)
	{
		UnionUserErrLog("in SoftHsmCmdW2:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"W2",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmdW2:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"60",2);
		goto exitNormally;	
	}
	if (((mpk = UnionReadSoftHsmICCardTransKey(hsmGrp,reqStr+2,reqStr+2+1,reqStr+2+1+2)) == NULL) || (strlen(mpk) != 32))
	{
		memset(mpkIndex,0,sizeof(mpkIndex));
		memcpy(mpkIndex,reqStr+2,5);
		UnionUserErrLog("in SoftHsmCmdW2:: mpkIndex not defined [%s]\n",mpkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	memset(dpk,0,sizeof(dpk));
	UnionGeneratePBOCDPK(mpk,reqStr+2+1+2+2,dpk);
	memset(sessionKey,0,sizeof(sessionKey));
	UnionXOR(dpk,dpk+16,16,sessionKey);
	memset(mac,0,sizeof(mac));
	UnionGenerateANSIX99MAC(sessionKey,reqStr+2+1+2+2+16+3,lenOfMacData,mac);
	UnionLog("in SoftHsmCmdW2:: \nmpk = [%s]\ndpk= [%s]\nsessionKey=[%s]\nmac=[%s]\n",
		mpk,dpk,sessionKey,mac);
	memcpy(errCode,"00",2);
exitNormally:
	resStr[0] = reqStr[0];
	resStr[1] = reqStr[1] + 1;
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	memcpy(resStr+2+2,mac,16);
	return(20);
}

/*
4  使用二次生成的密钥生成Ansi X9.9 MAC指令
输入域	长度	类型	说　明
命令代码	2	A	值"W3"
版本号	1	N	
组号	2	N	
索引号	2	N	
随机数A	16	H	
随机数B	16	H	
MAC数据长度	3	N	
MAC数据		A	


输出域	长度	类型	说　明
响应代码	2	A	""
错误代码	2	H	
MAC	16	H	

指令过程描述
1.	使用{版本、组号、索引号}指定的卡片主密钥对随机数A进行分散（分散采用PBOC算法），生成离散密钥K1（128位）。
2.	使用离散密钥K1加密随机数B，获得工作密钥K2（64位）。
3.	使用K2对MAC数据生成ANSIX9.9MAC。
测试数据一：
卡片主密钥的明文：0CCD4468BEF1C0CE66BC0E4C78CAFB34
随机数A：3030313330313030
随机数B：0656789013244880
MAC数据：3834567890132448
MAC数据长度：016
MAC：98753F731FD969D7
测试数据二：
卡片主密钥的明文：D804931E76AB6A1D912468991FA55252
随机数A：3838383838383838
随机数B：3838383838383838
MAC数据：
165883402880000088000009C400000070199901250856493030303130310594
MAC数据长度：064
MAC：B8369C60C82A03CB

*/
int SoftHsmCmdW3(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	*mpk;
	char	mpkIndex[10+1];
	char	dpk[32+1];
	char	sessionKey[16+1];
	char	mac[16+1];
	char	errCode[2+1];
	int	lenOfMacData;	
	
	//UnionLog("in SoftHsmCmdW3:: reqStr = [%s]\n",reqStr);
	if (lenOfReqStr < 2+1+2+2+16+16+3)
	{
		UnionUserErrLog("in SoftHsmCmdW3:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if ((lenOfMacData = UnionConvertIntStringToInt(reqStr+2+1+2+2+16+16,3)) <= 0)
	{
		UnionUserErrLog("in SoftHsmCmdW3:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"77",2);
		goto exitNormally;	
	}	
	if (lenOfReqStr < 2+1+2+2+16+16+3+lenOfMacData)
	{
		UnionUserErrLog("in SoftHsmCmdW3:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if (lenOfReqStr > 2+1+2+2+16+16+3+lenOfMacData)
	{
		UnionUserErrLog("in SoftHsmCmdW3:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"W3",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmdW3:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"60",2);
		goto exitNormally;	
	}
	if (((mpk = UnionReadSoftHsmICCardTransKey(hsmGrp,reqStr+2,reqStr+2+1,reqStr+2+1+2)) == NULL) || (strlen(mpk) != 32))
	{
		memset(mpkIndex,0,sizeof(mpkIndex));
		memcpy(mpkIndex,reqStr+2,5);
		UnionUserErrLog("in SoftHsmCmdW3:: mpkIndex not defined [%s]\n",mpkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	memset(dpk,0,sizeof(dpk));
	UnionGeneratePBOCDPK(mpk,reqStr+2+1+2+2,dpk);
	memset(sessionKey,0,sizeof(sessionKey));
	UnionGeneratePBOCSessionKey(dpk,reqStr+2+1+2+2+16,sessionKey);
	memset(mac,0,sizeof(mac));
	UnionGenerateANSIX99MAC(sessionKey,reqStr+2+1+2+2+16+16+3,lenOfMacData,mac);
	UnionLog("in SoftHsmCmdW3:: \nmpk = [%s]\ndpk= [%s]\nsessionKey=[%s]\nmac=[%s]\n",
		mpk,dpk,sessionKey,mac);
	memcpy(errCode,"00",2);
exitNormally:
	resStr[0] = reqStr[0];
	resStr[1] = reqStr[1] + 1;
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	memcpy(resStr+2+2,mac,16);
	return(20);
}
/*
5  一次分散的密钥加密数据指令
输入域	长度	类型	说　明
命令代码	2	A	值"W4"
版本号	1	N	
组号	2	N	
索引号	2	N	
随机数A	16	H	
数据长度	4	N	
数据		H	

输出域	长度	类型	说　明
响应代码	2	A	""
错误代码	2	H	
数据长度	4	N	
数据密文			

指令过程描述
1.	使用{版本、组号、索引号}指定的卡片主密钥对随机数A进行分散（分散采用PBOC算法），生成离散密钥K1（128位）。
2.	将离散密钥K1对数据进行3DES加密。
*/
int SoftHsmCmdW4(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	*mpk;
	char	mpkIndex[10+1];
	char	dpk[32+1];
	char	tmpBuf[2048+1];
	int	lenOfData = 0;
	char	errCode[2+1];
	int	i,num;
	
	//UnionLog("in SoftHsmCmdW4:: reqStr = [%s]\n",reqStr);
	if (lenOfReqStr < 2+1+2+2+16+4)
	{
		UnionUserErrLog("in SoftHsmCmdW4:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if ((lenOfData = UnionConvertIntStringToInt(reqStr+2+1+2+2+16,4)) <= 0)
	{
		UnionUserErrLog("in SoftHsmCmdW4:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"77",2);
		goto exitNormally;	
	}	
	if (lenOfReqStr < 2+1+2+2+16+4+lenOfData)
	{
		UnionUserErrLog("in SoftHsmCmdW4:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if ((lenOfReqStr > 2+1+2+2+16+4+lenOfData) || (lenOfData > sizeof(tmpBuf)))
	{
		UnionUserErrLog("in SoftHsmCmdW4:: lenOfReqStr [%d]! lenOfData = [%d]\n",lenOfReqStr,lenOfData);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"W4",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmdW4:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"60",2);
		goto exitNormally;	
	}
	//UnionLog("1.\n");
	if (((mpk = UnionReadSoftHsmICCardTransKey(hsmGrp,reqStr+2,reqStr+2+1,reqStr+2+1+2)) == NULL) || (strlen(mpk) != 32))
	{
		memset(mpkIndex,0,sizeof(mpkIndex));
		memcpy(mpkIndex,reqStr+2,5);
		UnionUserErrLog("in SoftHsmCmdW4:: mpkIndex not defined [%s]\n",mpkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	//UnionLog("2.\n");
	memset(dpk,0,sizeof(dpk));
	UnionGeneratePBOCDPK(mpk,reqStr+2+1+2+2,dpk);
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,reqStr+2+1+2+2+16+4,lenOfData);
	//UnionLog("in SoftHsmCmdW4:: lenOfData = [%d]\ndata = [%s]\n",lenOfData,tmpBuf);
	if (lenOfData % 16 != 0)
	{
		if ((lenOfData / 16) * 16 + 16 > sizeof(tmpBuf))
		{
			UnionUserErrLog("in SoftHsmCmdW4:: lenOfReqStr [%d]! lenOfData = [%d]\n",lenOfReqStr,lenOfData);
			memcpy(errCode,"62",2);
			goto exitNormally;	
		}
		memset(tmpBuf+lenOfData,'0',16 - lenOfData % 16);
		lenOfData = (lenOfData / 16) * 16 + 16;
	}
	UnionLog("in SoftHsmCmdW4:: lenOfData = [%d]\ndata = [%s]\n",lenOfData,tmpBuf);
	for (i = 0,num = lenOfData / 16; i < num; i++)
	{
		Union3DesEncrypt64BitsText(dpk,tmpBuf+i*16,tmpBuf+i*16);
	}
	UnionLog("in SoftHsmCmdW4:: \ndpk= [%s]\nlenOfData=[%d]\nData=[%s]\n",
		dpk,lenOfData,tmpBuf);
	memcpy(errCode,"00",2);
exitNormally:
	resStr[0] = reqStr[0];
	resStr[1] = reqStr[1] + 1;
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	if (2+2+4+lenOfData > sizeOfResStr)
	{
		UnionUserErrLog("in SoftHsmCmdW4:: lenOfData = [%d]\n",lenOfData);
		memcpy(resStr+2,"62",2);
		return(4);
	}
	sprintf(resStr+4,"%04d",lenOfData);
	memcpy(resStr+2+2+4,tmpBuf,lenOfData);
	return(2+2+4+lenOfData);
}

/*
输  入  消  息  格  式

输  入  域	长  度	类  型	内      容
命      令	2	A	50
算法选择	1	N	
Version	1	N	卡片密钥的版本号
Group_Index	2	N	卡片密钥的组号
M_Number	2	N	卡片密钥的索引号
Key_Flag	1	N	0：LSKEY；1：GCKEY
LS_Data	16	H	用于产生LSKEY的数据
GC_Data	16	H	用于产生GCKEY的数据，仅当Key_Flag为1时有此域
Length	3	N	Input Data的字节数/2
Input_Data	n	H	输入的MAC数据


输  出  消  息  格  式

输  出  域	长  度	类  型	内      容
应  答  码	2	A	51
错 误 代 码	2	N	
MAC	8	H	

处理步骤：
1．	用指定的卡片密钥加密LS_Data，得离散子密钥的LSKEY的左半部。
2．	用指定的卡片密钥加密取反后的LS_Data，得离散子密钥的LSKEY的右半部。
3．	用LSKEY加密GC_Data，得过程密钥GCKEY。
4．	如果Key_Flag等于0，则用离散子密钥LSKEY对输入的数据计算3_DES_MAC,                如果Key_Flag等于1，则用过程密钥GCKEY对输入的数据计算1_DES_MAC。
 
*/
int SoftHsmCmd50(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	*mpk;
	char	mpkIndex[10+1];
	char	dpk[32+1],sessionKey[16+1];
	char	tmpBuf[2048+1];
	int	lenOfData;
	char	errCode[2+1];
	char	keyFlag;
	int	lenOfHeader;
	char	mac[16+1];
	
	//UnionLog("in SoftHsmCmd50:: reqStr = [%s]\n",reqStr);
	lenOfHeader = 2+1+1+2+2;
	if (lenOfReqStr < lenOfHeader+1+16)
	{
		UnionUserErrLog("in SoftHsmCmd50:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if (((mpk = UnionReadSoftHsmICCardTransKey(hsmGrp,reqStr+2+1,reqStr+2+1+1,reqStr+2+1+1+2)) == NULL) || (strlen(mpk) != 32))
	{
		memset(mpkIndex,0,sizeof(mpkIndex));
		memcpy(mpkIndex,reqStr+2,5);
		UnionUserErrLog("in SoftHsmCmd50:: mpkIndex not defined [%s]\n",mpkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	memset(dpk,0,sizeof(dpk));
	UnionGeneratePBOCDPK(mpk,reqStr+lenOfHeader+1,dpk);
	memset(sessionKey,0,sizeof(sessionKey));
	if ((keyFlag = reqStr[lenOfHeader]) == '1')
	{
		UnionGeneratePBOCSessionKey(dpk,reqStr+lenOfHeader+1+16,sessionKey);
		lenOfHeader = lenOfHeader+1+16+16;
	}
	else if (keyFlag == '0')
		lenOfHeader = lenOfHeader+1+16;
	else
	{
		UnionUserErrLog("in SoftHsmCmd50:: invalid key selection [%c]!\n",reqStr[lenOfHeader]);
		memcpy(errCode,"77",2);
		goto exitNormally;	
	}
	UnionLog("in SoftHsmCmd50:: \ndpk=[%s]\nsessionKey=[%s]\nlenOfHeader=[%d]\n",dpk,sessionKey,lenOfHeader);
	if ((lenOfData = UnionConvertIntStringToInt(reqStr+lenOfHeader,3)) <= 0)
	{
		UnionUserErrLog("in SoftHsmCmd50:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"77",2);
		goto exitNormally;	
	}
	lenOfData *= 2;	
	if (lenOfReqStr < lenOfHeader+3+lenOfData)
	{
		UnionUserErrLog("in SoftHsmCmd50:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if ((lenOfReqStr > lenOfHeader+3+lenOfData) || (lenOfData > sizeof(tmpBuf)))
	{
		UnionUserErrLog("in SoftHsmCmd50:: lenOfReqStr [%d]! lenOfData = [%d]\n",lenOfReqStr,lenOfData);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"50",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmd50:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"60",2);
		goto exitNormally;	
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,reqStr+lenOfHeader+3,lenOfData);
	UnionLog("in SoftHsmCmd50:: lenOfMacData = [%d]\nmacData = [%s]\n",lenOfData,tmpBuf);
	memset(mac,0,sizeof(mac));
	if (keyFlag == '1')
		UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail(sessionKey,16,tmpBuf,lenOfData,mac);
	else
		UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail(dpk,32,tmpBuf,lenOfData,mac);
	UnionLog("in SoftHsmCmd50:: mac = [%s]\n",mac);
	memcpy(errCode,"00",2);
exitNormally:
	resStr[0] = reqStr[0];
	resStr[1] = reqStr[1] + 1;
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	memcpy(resStr+2+2,mac,8);
	return(2+2+8);
}
/*
6  使用一次分散生成的密钥生成MAC指令（带初始向量）
输入域	长度	类型	说　明
命令代码	2	A	值"W5"
版本号	1	N	
组号	2	N	
索引号	2	N	
随机数A	16	H	
初始向量	16	H	
MAC数据长度	3	N	
MAC数据		A	


输出域	长度	类型	说　明
响应代码	2	A	""
错误代码	2	H	
MAC	16	H	

指令过程描述
1.	使用{版本、组号、索引号}指定的卡片主密钥对随机数A进行分散（分散采用PBOC算法），生成离散密钥K1（128位）。
2.	使用K1对MAC数据生成MAC。
测试数据一：
卡片主密钥的明文：A0DBB32E7A6E7DA88581F514657048ED
随机数A：5882338010333398
初始向量：2222222200000000
MAC数据：04D695180CFB7FD46251ECFAE3
MAC数据长度：026
MAC：0105E931209CDB23
 
*/
int SoftHsmCmdW5(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	*mpk;
	char	mpkIndex[10+1];
	char	dpk[32+1];
	char	tmpBuf[2048+1];
	int	lenOfData;
	char	errCode[2+1];
	int	lenOfHeader;
	char	mac[16+1];
	
	//UnionLog("in SoftHsmCmdW5:: reqStr = [%s]\n",reqStr);
	lenOfHeader = 2+1+2+2+16+16+3;
	if (lenOfReqStr < lenOfHeader)
	{
		UnionUserErrLog("in SoftHsmCmdW5:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if (((mpk = UnionReadSoftHsmICCardTransKey(hsmGrp,reqStr+2,reqStr+2+1,reqStr+2+1+2)) == NULL) || (strlen(mpk) != 32))
	{
		memset(mpkIndex,0,sizeof(mpkIndex));
		memcpy(mpkIndex,reqStr+2,5);
		UnionUserErrLog("in SoftHsmCmdW5:: mpkIndex not defined [%s]\n",mpkIndex);
		memcpy(errCode,"91",2);
		goto exitNormally;	
	}
	memset(dpk,0,sizeof(dpk));
	UnionGeneratePBOCDPK(mpk,reqStr+2+1+2+2,dpk);

	UnionLog("in SoftHsmCmdW5:: \ndpk=[%s]\nlenOfHeader=[%d]\n",dpk,lenOfHeader);
	if ((lenOfData = UnionConvertIntStringToInt(reqStr+lenOfHeader-3,3)) <= 0)
	{
		UnionUserErrLog("in SoftHsmCmdW5:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"77",2);
		goto exitNormally;	
	}
	if (lenOfReqStr < lenOfHeader+lenOfData)
	{
		UnionUserErrLog("in SoftHsmCmdW5:: lenOfReqStr [%d]!\n",lenOfReqStr);
		memcpy(errCode,"61",2);
		goto exitNormally;	
	}
	if ((lenOfReqStr > lenOfHeader+lenOfData) || (lenOfData+16 > sizeof(tmpBuf)))
	{
		UnionUserErrLog("in SoftHsmCmdW5:: lenOfReqStr [%d]! lenOfData = [%d]\n",lenOfReqStr,lenOfData);
		memcpy(errCode,"62",2);
		goto exitNormally;	
	}
	if (strncmp(reqStr,"W5",2) != 0)
	{
		UnionUserErrLog("in SoftHsmCmdW5:: reqStr = [%s]\n",reqStr);
		memcpy(errCode,"60",2);
		goto exitNormally;	
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,reqStr+2+1+2+2+16,16);
	memcpy(tmpBuf+16,reqStr+lenOfHeader,lenOfData);
	lenOfData += 16;
	UnionLog("in SoftHsmCmdW5:: lenOfMacData = [%d]\nmacData = [%s]\n",lenOfData,tmpBuf);
	memset(mac,0,sizeof(mac));
	UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail(dpk,32,tmpBuf,lenOfData,mac);
	UnionLog("in SoftHsmCmdW5:: mac = [%s]\n",mac);
	memcpy(errCode,"00",2);
exitNormally:
	resStr[0] = reqStr[0];
	resStr[1] = reqStr[1] + 1;
	memcpy(resStr+2,errCode,2);
	if (strncmp(errCode,"00",2) != 0)
		return(4);
	memcpy(resStr+2+2,mac,8);
	return(2+2+8);
}

int SoftHsmCmd38(char *hsmGrp,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	memcpy(resStr,"3900",4);
	return(4);
}

int UnionSoftHsmCmdInterprotor(char *hsmGrpID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	char	cmdStr[2+1];
	
	//UnionLog("in UnionSoftHsmCmdInterprotor:: lenOfReqStr = [%d]\nreqStr = [%s]\n",lenOfReqStr,reqStr);
	if (lenOfReqStr < 2)
	{
		UnionUserErrLog("in UnionSoftHsmCmdInterprotor:: lenOfReqStr [%d] too short!\n",lenOfReqStr);
		return(-1);
	}
	memcpy(cmdStr,reqStr,2);
	cmdStr[2] = 0;
	if (strncmp(cmdStr,"01",2) == 0)
	{
		memcpy(resStr,"0200",4);
		return(4);
	}
	else if (strncmp(cmdStr,"60",2) == 0)
		return(SoftHsmCmd60(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"68",2) == 0)
		return(SoftHsmCmd68(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"80",2) == 0)
		return(SoftHsmCmd80(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"W1",2) == 0)
		return(SoftHsmCmdW1(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"W2",2) == 0)
		return(SoftHsmCmdW2(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"W3",2) == 0)
		return(SoftHsmCmdW3(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"W4",2) == 0)
		return(SoftHsmCmdW4(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"W5",2) == 0)
		return(SoftHsmCmdW5(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"50",2) == 0)
		return(SoftHsmCmd50(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	else if (strncmp(cmdStr,"38",2) == 0)
		return(SoftHsmCmd38(hsmGrpID,reqStr,lenOfReqStr,resStr,sizeOfResStr));
	resStr[0] = reqStr[0];
	resStr[1] = reqStr[1] + 1;
	memcpy(resStr+2,"60",2);
	return(4);
}


/****************ges指令组装********************

author:  lics
date:    2014-4-29

***********************************************/
#include "unionHsmCmdGes.h"








/*
功能：相互认证

输入参数:
	zakByzmk: 认证密钥，用于计算mac
	lenOfrandData: 随机数长度
	randData: 随机数， 用于计算mac
	mac: 用户端计算出的mac

输出参数：
	lenOfrandData2: 随机数长度
	randData2: 随机数
	mac2: 加密机产生的mac

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(const char* zakByzmk,  const int* lenOfrandData, const char* randData, const char* mac,  int* lenOfrandData2, char* randData2, char* mac2)
{
	int ret;
	char hsmCmdBuf[1024+1];
	int hsmCmdLen;
	

	if ((zakByzmk == NULL) || (randData == NULL) || (mac == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

	memcpy(hsmCmdBuf,"MM",2);
	hsmCmdLen = 2;

        if ((ret = UnionGenerateX917RacalKeyString(strlen(zakByzmk) ,zakByzmk ,hsmCmdBuf+hsmCmdLen)) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zakByzmk, strlen(zakByzmk));
                return(ret);
        }
        hsmCmdLen += ret;
	
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", lenOfrandData);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, randData, lenOfrandData);
	hsmCmdLen += lenOfrandData;

	memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
	hsmCmdLen += 16;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
		return(ret);
	}
       
        hsmCmdBuf += 4;

	memcpy(lenOfrandData2, hsmCmdBuf, 2);
	hsmCmdBuf += 2;

	memcpy(randData2, hsmCmdBuf, lenOfrandData2);
	hsmCmdBuf += lenOfrandData2;

	memcpy(mac2, hsmCmdBuf, 16);

	return 0;
	
	
}



/*
功能：退出认证

输入参数：

输出参数：

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM()
{
	int ret;
	char hsmCmdBuf[32+1];

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }


	return 0;
}



/*
功能： 写配额到加密机中

输入参数：
	quotaByzmk：由zmk加密的配额密文，  明文格式为：产品号,机构号,时间批次,配额值
	mac：用户端计算出的mac

输出参数：

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac)
{
        int ret;
        char hsmCmdBuf[128+1];
        int hsmCmdLen;


        if ((quotaByzmk == NULL) || (mac == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;

        memcpy(hsmCmdBuf+hsmCmdLen, quotaByzmk, 32);
        hsmCmdLen += 32;

        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

	return 0;

}



/*
功能：从加密机中读取指定配额

输入参数：
	quotaByzmk：由zmk加密的配额信息密文， 明文格式为： 产品号,机构号,时间批次
	mac:  用户端计算出的mac

输出参数：
	quotaByzmk2: 由zmk加密的配额密文，  明文格式为：产品号,机构号,时间批次,配额值

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac, char* quotaByzmk2)
{
        int ret;
        char hsmCmdBuf[128+1];
        int hsmCmdLen;


        if ((quotaByzmk == NULL) || (mac == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;

        memcpy(hsmCmdBuf+hsmCmdLen, quotaByzmk, 32);
        hsmCmdLen += 32;

        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf += 4;

        memcpy(quotaByzmk2, hsmCmdBuf, 32);

        return 0;
}



/*
功能：删除指定配额

输入参数：
	quotaByzmk: 由zmk加密的配额信息密文， 明文格式为： 产品号,机构号,时间批次
	mac: 用户端计算出的mac

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac)
{
        int ret;
        char hsmCmdBuf[128+1];
        int hsmCmdLen;


        if ((quotaByzmk == NULL) || (mac == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;

        memcpy(hsmCmdBuf+hsmCmdLen, quotaByzmk, 32);
        hsmCmdLen += 32;

        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        return 0;
}



/*
功能：删除加密机内所有配额

输入参数：
	lenOfrandData：随机数长度
	randData：随机数
	mac：用户端计算出的mac

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(const int lenOfrandData, const char* randData, const char* mac)
{
        int ret;
        char hsmCmdBuf[128+1];
        int hsmCmdLen;


        if ((randData == NULL) || (mac == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;
	
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", lenOfrandData);
        hsmCmdLen += 2;

        memcpy(hsmCmdBuf+hsmCmdLen, randData, lenOfrandData);
        hsmCmdLen += lenOfrandData;

        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        return 0;
}



/*
功能：读取全部配额数据

输入参数：
	batchNoBegin： 起始批次号
	batchNoEnd： 结束批次号
	mac：用户端计算出的mac

输出参数：
	numOfquota： 配额条数
	listOfquotaByzmk：格式：单条配额密文*配额条数，  单条配额明文：产品号,机构号,时间批次,配额值

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(const int batchNoBegin, const int batchNoEnd, const char* mac, int* numOfquota, char* listOfquotaByzmk)
{
        int ret;
        char hsmCmdBuf[1024+1];
        int hsmCmdLen;


        if ((mac == NULL) || (batchNoBegin >= batchNoEnd))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;

        sprintf(hsmCmdBuf+hsmCmdLen, "%02d", batchNoBegin);
        hsmCmdLen += 2;


        sprintf(hsmCmdBuf+hsmCmdLen, "%02d", batchNoEnd);
        hsmCmdLen += 2;


        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf += 4;

        memcpy(numOfquota, 2, hsmCmdBuf);
        hsmCmdBuf += 2;

        memcpy(randData2, hsmCmdBuf, numOfquota);

        return 0;
}



/*
功能：读取批次总条数

输入参数：

输出参数：
	numOfquota: 配额总条数

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(int * numOfquota)
{
	int ret;
        char hsmCmdBuf[32+1];


        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);


        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf += 4;

        memcpy(numOfquota, 2, hsmCmdBuf);
        
        return 0;
}




/****************ges指令组装********************

author:  lics
date:    2014-4-29

***********************************************/









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
int UnionHsmCmdMM(const char* zakByzmk,  const int* lenOfrandData, const char* randData, const char* mac,  int* lenOfrandData2, char* randData2, char* mac2);



/*
功能：退出认证

输入参数：

输出参数：

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM();



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
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac);



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
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac, char* quotaByzmk2);



/*
功能：删除指定配额

输入参数：
	quotaByzmk: 由zmk加密的配额信息密文， 明文格式为： 产品号,机构号,时间批次
	mac: 用户端计算出的mac

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac);



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
int UnionHsmCmdMM(const int lenOfrandData, const char* randData, const char* mac);



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
int UnionHsmCmdMM(const int batchNoBegin, const int batchNoEnd, const char* mac, int* numOfquota, char* listOfquotaByzmk);



/*
功能：读取批次总条数

输入参数：

输出参数：
	numOfquota: 配额总条数

返回值
        >=0 成功
        <0 失败
*/
int UnionHsmCmdMM(int * numOfquota);




#ifndef UNION_GENKEY_BYPK
#define UNION_GENKEY_BYPK

//随机产生单倍长des密钥
char *UnionGenDESKey(char *outKeybuf);

//随机产生双倍长des密钥
char *UnionGen2DESKey(char *outKeybuf);

//随机产生3倍长des密钥
char *UnionGen3DESKey(char *outKeybuf);

/*随机生成2倍长des,并由强度为1024的PK加密输出*/
/*
输入：1024bits的PK裸公钥,扩展的可见字符串
输出：DesKey, 随即产生的des密钥明文
输出: DesKeyByPK,被由PK加密的des密钥,扩展的可见字符串
返回：
    >= 0  成功
    <0   失败
*/
int UnionGen2DesKeyBy1024PK(char *PK,char *DesKey,char *DesKeyByPK);

//用PK加密DES密钥
/*
输入：1024bits的PK裸公钥,扩展的可见字符串
输入：DesKey, 2des密钥明文
输出: DesKeyByPK,被由PK加密的des密钥,扩展的可见字符串
返回：
    >= 0  成功
    <0   失败
*/
int UnionEncDesKeyBy1024PK(char *PK,char *pDesKey,char *DesKeyByPK);

#endif


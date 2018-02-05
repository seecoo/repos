
#ifndef _UnionTASSCmd_
#define	_UnionTASSCmd_ 

//生成验签的指令(国密)
int UnionTASSHsmCmdK5(int vkIndex, char *hashID, int lenOfUsrID, char *usrID, int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal);

/*
   函数功能:
   TJ指令，公钥解密
   输入参数:
encryMode: 1H 算法标识 1－解密数据 2－加密数据 3－验证签名 4－加密密钥 5－解密密钥
pkLength: 送入公钥长度
pk: nB/1A+3H DER编码的公钥或公钥在HSM安全存储区内的位置
signLength: 待验证签名值长度，encryMode = 3时该域存在
sign: nB 待验证签名值，encryMode = 3时该域存在
keyLength: 1H 待加/解密密钥长度，0 = 单长度密钥，1 = 双长度密钥；encryMode = 4 或 encryMode = 5时该域存在
inputDataLength: 加解密数据长度
inputData: nB/1A+3H 待加解密数据或待加解密密钥在HSM安全存储区内的索引
输出参数:
outputData: nB 加解密/加解密密钥数据
返回值:
>0      ouputData的长度
<0      失败
 */     
int UnionTASSHsmCmdTJ(char encryMode, int pkLength, char *pk, int signLength, char *sign, char keyLength, int inputDataLength, char *inputData, char *outputData);

#endif

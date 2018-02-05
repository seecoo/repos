
#ifndef _UnionSJJ1127Cmd_
#define _UnionSJJ1127Cmd_

#include "unionDesKey.h"

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

// 转换密钥长度，返回密钥的长度
int UnionTranslateSJJ1127HsmKeyLength(TUnionDesKeyLength length,char *lenStr);

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
int UnionSJJ1127Cmd18(char *transformID,int oriPKIndex,TUnionDesKeyLength lenOfOriPK,char *oriPKValue,int desPKIndex,TUnionDesKeyLength lenOfDesPK,char *desPKValue,TUnionDesKeyLength lenOfOriKey,char *oriKeyValue,char *oriKeyCheckValue,char *desKeyValue,char *desKeyCheckValue);

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
int UnionSJJ1127Cmd50(char *algorithmID,char *encryptID,int indexOfKey,TUnionDesKeyLength lenOfKey,char *keyValue,char *iv,int lenOfData,char *data,int *lenOfResData,char *resData);

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
int UnionSJJ1127Cmd60(TUnionDesKeyLength lenOfKey,int numOfComponent,char *keyName,char *keyValue,char *keyCheckValue);

/*
函数功能：
        62指令，装载打印格式
输入参数：
        formatType: 格式类型
        format: 打印格式
输出参数：
        无
*/
int UnionSJJ1127Cmd62(char *formatType,char *format);

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
int UnionSJJ1127Cmd16(int indexOfKey,char *keyValue,char *keyCheckValue);

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
int UnionSJJ1127Cmd14(TUnionDesKeyLength lenOfKey,char *keyValue,char *keyCheckValue,int indexOfKey);

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
int UnionSJJ1127Cmd90(TUnionDesKeyLength lenOfKey,int indexOfKey,char *keyValue,char *keyCheckValue);

/*
函数功能：
        U2指令，使用指定的应用主密钥进行2次离散得到卡片应用子密钥，
                使用指定控制密钥子密钥进行加密保护输出并进行MAC计算。
输入参数：
        securityMech: 安全机制(S：单DES加密和MAC, T：三DES加密和MAC)
        mode: 模式标志, 0-仅加密 1-加密并计算MAC
        id: 方案ID, 0=M/Chip4(CBC模式，强制填充X80) 1=VISA/PBOC(带长度指引的ECB) 2=PBOC1.0模式(ECB模式，外带填充数据)

        mkIndex:        FFFF：使用指令中带入的根密钥
                        0000-03E8：密钥索引
                        其他：非法
        lenOfMK:        仅当根密钥索引为FFFF时有此域。
                        值01- 64bits
                        值02- 128bits
                        值03- 192bits
        mk:             MK加密下的根密钥的密文
        mkDvsNum: 根密钥离散次数, 1-3次
        mkDvsData: 根密钥离散数据, n*16H, n代表离散次数

        pkIndex:        FFFF：使用指令中带入的保护密钥
                        0000-03E8：密钥索引
                        其他：非法
        lenOfPK:        仅当保护密钥索引为FFFF时有此域。
                        值01- 64bits
                        值02- 128bits
                        值03- 192bits
        pk:             MK加密下的保护密钥的密文
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
int UnionSJJ1127CmdU2(char *securityMech, char *mode, char *id, int mkIndex,TUnionDesKeyLength lenOfMK,char *mk,int mkDvsNum, char *mkDvsData, int pkIndex, TUnionDesKeyLength lenOfPK, char *pk,int pkDvsNum, char *pkDvsData, char *proKeyFlag, char *proFactor, char *ivCbc,int encryptFillDataLen, char *encryptFillData, int encryptFillOffset,char *ivMac, int macDataLen, char *macData, int macOffset,char *mac, int *criperDataLen, char *criperData);

#endif


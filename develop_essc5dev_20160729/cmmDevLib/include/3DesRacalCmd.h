/* Copyright:	Union Tech.
   Author:	Wolfgang Wang
   Date:	2003/05/02
*/

#ifndef _Union3DesRacalCmd_
#define _Union3DesRacalCmd_

#include "unionDesKey.h"

// 将一个密钥Key从ZMK保护转换为受LMK保护
// pZMK是保护Key的ZMK对象
// 输入时，pKeyObject中存放的是pZMK加密的Key密文
// 输出时，pKeyObject中存放的是LMK加密的Key密文，checkValue存储了其校验值
// Key的保护属性、强度都由pKeyObject定义
// 当密码机转换失败时，errCode存放的是错误码，此时返回值为0
// 只有当返回值为0，且errCode为00时，才转换成功
int UnionTranslateKeyUnderZMKToLMK(int hsmSckHDL,PUnionSJL06 pSJL06,PUnionDesKey pZMK,PUnionDesKey pKeyObject,char *errCode);

// 将一个密钥Key从LMK保护转换为受ZMK保护
// pZMK是保护Key的ZMK对象
// 输入时，pKeyObject中存放的是LMK加密的Key密文
// 输出时，pKeyObject中存放的是ZMK加密的Key密文，checkValue存储了其校验值
// Key的保护属性、强度都由pKeyObject定义
// 当密码机转换失败时，errCode存放的是错误码，此时返回值为0
// 只有当返回值为0，且errCode为00时，才转换成功
int UnionTranslateKeyUnderLMKToZMK(int hsmSckHDL,PUnionSJL06 pSJL06,PUnionDesKey pZMK,PUnionDesKey pKeyObject,char *errCode);

// 根据密钥对象的定义，形成指令字,返回命令字的长度
int UnionFormCmdStringForTranslateKeyUnderLMKToZMK(PUnionDesKey pDesKey,char *cmdString);

// 根据密钥对象的定义，判断响应指令字是否正确
int UnionIsValidCmdResponseStringForTranslateKeyUnderLMKToZMK(PUnionDesKey pDesKey,char *cmdResString);

#endif

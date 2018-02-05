#ifndef _Essc5Interface_H_
#define _Essc5Interface_H_

#include "commWithHsmSvr.h"

// 测试密码机状态
int UnionDealServiceCodeE100(PUnionHsmGroupRec phsmGroupRec);
// 执行密码机指令
int UnionDealServiceCodeE101(PUnionHsmGroupRec phsmGroupRec);
// 生成对称密钥
int UnionDealServiceCodeE110(PUnionHsmGroupRec phsmGroupRec);
// 更新对称密钥
int UnionDealServiceCodeE111(PUnionHsmGroupRec phsmGroupRec);
// 导入对称密钥
int UnionDealServiceCodeE112(PUnionHsmGroupRec phsmGroupRec);
// 导出对称密钥
int UnionDealServiceCodeE113(PUnionHsmGroupRec phsmGroupRec);
// 申请对称密钥
int UnionDealServiceCodeE114(PUnionHsmGroupRec phsmGroupRec);
// 分发对称密钥
int UnionDealServiceCodeE115(PUnionHsmGroupRec phsmGroupRec);
// 销毁对称密钥
int UnionDealServiceCodeE116(PUnionHsmGroupRec phsmGroupRec);
// 获取对称密钥信息
int UnionDealServiceCodeE117(PUnionHsmGroupRec phsmGroupRec);
// 打印密钥
int UnionDealServiceCodeE118(PUnionHsmGroupRec phsmGroupRec);
// 启用/挂起对称密钥
int UnionDealServiceCodeE119(PUnionHsmGroupRec phsmGroupRec);
// 生成非对称密钥
int UnionDealServiceCodeE120(PUnionHsmGroupRec phsmGroupRec);
// 更新非对称密钥
int UnionDealServiceCodeE121(PUnionHsmGroupRec phsmGroupRec);
// 导入公钥
int UnionDealServiceCodeE122(PUnionHsmGroupRec phsmGroupRec);
// 导出公钥
int UnionDealServiceCodeE123(PUnionHsmGroupRec phsmGroupRec);
// 申请非对称密钥
int UnionDealServiceCodeE124(PUnionHsmGroupRec phsmGroupRec);
// 分发非对称密钥
int UnionDealServiceCodeE125(PUnionHsmGroupRec phsmGroupRec);
// 销毁非对称密钥
int UnionDealServiceCodeE126(PUnionHsmGroupRec phsmGroupRec);
// 获取非对称密钥信息
int UnionDealServiceCodeE127(PUnionHsmGroupRec phsmGroupRec);
// 生成PIN
int UnionDealServiceCodeE130(PUnionHsmGroupRec phsmGroupRec);
// 生成PVV
int UnionDealServiceCodeE131(PUnionHsmGroupRec phsmGroupRec);
// 生成PIN Offset
int UnionDealServiceCodeE132(PUnionHsmGroupRec phsmGroupRec);
// 生成CVV
int UnionDealServiceCodeE133(PUnionHsmGroupRec phsmGroupRec);
// 加密PIN
int UnionDealServiceCodeE140(PUnionHsmGroupRec phsmGroupRec);
// 解密PIN
int UnionDealServiceCodeE141(PUnionHsmGroupRec phsmGroupRec);
// 转加密PIN
int UnionDealServiceCodeE142(PUnionHsmGroupRec phsmGroupRec);
// 验证PIN
int UnionDealServiceCodeE143(PUnionHsmGroupRec phsmGroupRec);
// 转加密PIN Offset
int UnionDealServiceCodeE144(PUnionHsmGroupRec phsmGroupRec);
// 验证CVV
int UnionDealServiceCodeE145(PUnionHsmGroupRec phsmGroupRec);
// 生成MAC
int UnionDealServiceCodeE150(PUnionHsmGroupRec phsmGroupRec);
// 验证MAC
int UnionDealServiceCodeE151(PUnionHsmGroupRec phsmGroupRec);
// 验证并生成MAC
int UnionDealServiceCodeE152(PUnionHsmGroupRec phsmGroupRec);
// 加密数据
int UnionDealServiceCodeE160(PUnionHsmGroupRec phsmGroupRec);
// 解密数据
int UnionDealServiceCodeE161(PUnionHsmGroupRec phsmGroupRec);
// 私钥签名
int UnionDealServiceCodeE170(PUnionHsmGroupRec phsmGroupRec);
// 公钥验签
int UnionDealServiceCodeE171(PUnionHsmGroupRec phsmGroupRec);
// 公钥加密
int UnionDealServiceCodeE172(PUnionHsmGroupRec phsmGroupRec);
// 私钥解密
int UnionDealServiceCodeE173(PUnionHsmGroupRec phsmGroupRec);

/***************************************
服务代码:       E162
服务名:         RC4 加密数据
功能描述:       RC4 加密数据
add by zhangbs 20140627
***************************************/
int UnionDealServiceCodeE162(PUnionHsmGroupRec phsmGroupRec);

//测试密码机状态
int UnionDealServiceCodeE300(PUnionHsmGroupRec phsmGroupRec);
//校验ARQC
int UnionDealServiceCodeE301(PUnionHsmGroupRec phsmGroupRec);
//生成ARPC
int UnionDealServiceCodeE302(PUnionHsmGroupRec phsmGroupRec);
//加密脚本数据
int UnionDealServiceCodeE303(PUnionHsmGroupRec phsmGroupRec);
//脚本数据计算mac
int UnionDealServiceCodeE304(PUnionHsmGroupRec phsmGroupRec);

//测试密码机状态
int UnionDealServiceCodeE200(PUnionHsmGroupRec phsmGroupRec);
//数字PIN转换(PK->ZPK)
int UnionDealServiceCodeE201(PUnionHsmGroupRec phsmGroupRec);
//字符PIN转换(PK->ZPK)
int UnionDealServiceCodeE202(PUnionHsmGroupRec phsmGroupRec);
//字符PIN转换(ZPK->ZPK)
int UnionDealServiceCodeE203(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeE204(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeE205(PUnionHsmGroupRec phsmGroupRec);

//PKCS7签名
int UnionDealServiceCodeE501(PUnionHsmGroupRec phsmGroupRec);
//PKCS7验证签名
int UnionDealServiceCodeE502(PUnionHsmGroupRec phsmGroupRec);
//PKCS7加密
int UnionDealServiceCodeE503(PUnionHsmGroupRec phsmGroupRec);
//PKCS7解密
int UnionDealServiceCodeE504(PUnionHsmGroupRec phsmGroupRec);

// 计算安全报文
int UnionDealServiceCodeE601(PUnionHsmGroupRec phsmGroupRec);
// 使用离散密钥加解密数据或计算MAC
int UnionDealServiceCodeE602(PUnionHsmGroupRec phsmGroupRec);
// 使用离散密钥转加密PIN
int UnionDealServiceCodeE603(PUnionHsmGroupRec phsmGroupRec);


// 生成本行报文
int UnionDealServiceCodeE401(PUnionHsmGroupRec phsmGroupRec);
// 解析他行绑定报文
int UnionDealServiceCodeE402(PUnionHsmGroupRec phsmGroupRec);
// 数字签名加签
int UnionDealServiceCodeE403(PUnionHsmGroupRec phsmGroupRec);
// 数字签名核签
int UnionDealServiceCodeE404(PUnionHsmGroupRec phsmGroupRec);

// 以下是广发银行CGB客户化服务
// 测试密码机状态
int UnionDealServiceCodeEEA0(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEA1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEA2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEA3(PUnionHsmGroupRec phsmGroupRec);

// 以下是阿里云服务接口
int UnionDealServiceCodeEEB0(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB3(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB4(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB5(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB6(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB7(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB8(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB9(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBA(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBB(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBC(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBD(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBE(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBF(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBG(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBH(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBI(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBJ(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBK(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBL(PUnionHsmGroupRec phsmGroupRec);

// 内蒙古社保定制
int UnionDealServiceCodeEEC1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC3(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC4(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC5(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC6(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC7(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC8(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC9(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEECA(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEECB(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEECC(PUnionHsmGroupRec phsmGroupRec);

//中原银行定制
int UnionDealServiceCodeEED1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEED2(PUnionHsmGroupRec phsmGroupRec);

//广西农信定制

int UnionDealServiceCodeEEF1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEF2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEF3(PUnionHsmGroupRec phsmGroupRec);

//江苏交通厅定制
int UnionDealServiceCodeJ001(PUnionHsmGroupRec phsmGroupRec);

// 交通部定制
int UnionDealServiceCodeEEG1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG3(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG4(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG5(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG6(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG7(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG8(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG9(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGA(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGB(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGC(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGD(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGE(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGF(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGG(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGH(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGI(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGJ(PUnionHsmGroupRec phsmGroupRec);

// 广州农商行
int UnionDealServiceCodeEEZ1(PUnionHsmGroupRec phsmGroupRec);

#endif //_Essc5Interface_H_

//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#ifndef _CommWithHsmSvr_
#define _CommWithHsmSvr_

// 定义对称密钥容器
typedef struct
{
	char	hsmGroupID[8+1];	// 密码机组ID
	char	lmkProtectMode[16+1];	// LMK保护方式
	int	hsmCmdVersionID;	// 密码机指令类型
} TUnionHsmGroupRec;
typedef TUnionHsmGroupRec		*PUnionHsmGroupRec;

// 使用BCD扩展打印HSM请求指令
void UnionSetBCDPrintTypeForHSMReqCmd();
// 使用BCD扩展打印HSM响应指令
void UnionSetBCDPrintTypeForHSMResCmd();
// 使用BCD扩展打印HSM指令
void UnionSetBCDPrintTypeForHSMCmd();

// 使用***打印HSM请求指令报文体
void UnionSetMaskPrintTypeForHSMReqCmd();
// 使用***打印HSM响应指令报文体
void UnionSetMaskPrintTypeForHSMResCmd();
// 使用***打印HSM指令报文体
void UnionSetMaskPrintTypeForHSMCmd();

void UnionResetPrintTypeForHSMCmd();
char *UnionGetHsmCmdPrintTypeStr();

// 设置加密机指令头的长度
void UnionSetLenOfHsmCmdHeader(int len);

// 设置不需要检查加密机响应码
void UnionSetIsNotCheckHsmResCode();

// 设置需要检查加密机响应码
void UnionSetIsCheckHsmResCode();

// 设置使用的加密机组ID
void UnionSetHsmGroupIDForHsmSvr(char *hsmGrpID);

// 读取使用的加密机组ID
char *UnionGetHsmGroupIDForHsmSvr();

// 设置访问指定IP地址的加密机
// 使用这个函数，每个指令只能生效一次，下个指令恢复使用组内加密机
int UnionSetUseSpecHsmIPAddrForOneCmd(char *hsmIPAddr);

// 设置访问指定组的加密机
// 使用这个函数，每个指令只能生效一次，下个指令恢复使用默认组内加密机
int UnionSetUseSpecHsmGroupForOneCmd(char *hsmGrpID);

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

int UnionGetHsmGroupRecByHsmGroupID(char *hsmGroupID,PUnionHsmGroupRec phsmGroupRec);

#endif

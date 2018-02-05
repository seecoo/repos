#ifndef	_UnionMDLIDDef_
#define _UnionMDLIDDef_

#include "unionErrCode.h"

typedef long			TUnionModuleID;
typedef TUnionModuleID		*PUnionModuleID;

typedef int			TUnionMDLType;

#define conMDLTypeOffsetOfTcpAndTask		100000000
#define conMDLTypeOffsetOfHsmSvr		200000000
#define conMDLTypeOffsetOfDbSvr			300000000
#define conMDLTypeOffsetOfMonSvr		400000000
#define conMDLTypeOffsetOfDataSync		500000000
//#define conMDLTypeOffsetOfHsmThrough            600000000
//#define conMDLTypeOffsetOfHsmCmd                700000000

#define conMDLTypeDefault			1
#define conMDLTypeUnionMngSvr			2
#define conMDLTypeUnionLongConnTcpipSvr		3
#define conMDLTypeUnionShortConnTcpipSvr	4
#define conMDLTypeUnionFunSvr			5
#define conMDLTypeUnionLogSvr			6
#define conMDLTypeUnionDBSvr			7
#define conMDLTypeUnionHsmSvr			8
#define conMDLTypeUnionMonSvr			9
#define conMDLTypeUnionDataSync			10
#define conMDLTypeUnionMon_MaxConn		11
#define conMDLTypeUnionMon_HsmCmd               12

#define conMaxNumOfMDL		100

// 获取指定密码机的处理模块ID号
TUnionModuleID UnionGetFixedMDLIDOfTaskOfHsm(char *hsmIPAddr);

// 获取指定密码机工作组的处理模块ID号
TUnionModuleID UnionGetFixedMDLIDOfTaskOfHsmGrp(char *hsmGrpID);

int UnionGetHsmGrpIDOutOfPort(int port,char *hsmGrpID);

// 生成动态模块标识号
long UnionGenerateDynamicMDLNum();

// 设置模块类型
int UnionSetMyModuleType(TUnionMDLType myMDLType);

// 获取模块类型
TUnionMDLType UnionGetMyModuleType();

// 从模块标识号中获取模块类型
TUnionMDLType UnionGetMDLTypeOutOfMDLID(TUnionModuleID id);

// 从模块标识号中获取进程号
int UnionGetPIDOutOfMDLID(TUnionModuleID id);

// 从模块标识号中获取动态模块标识号
int UnionGetDynamicIDOutOfMDLID(TUnionModuleID id);

// 产生模块的动态标识号
TUnionModuleID UnionGenerateMyDynamicMDLID();

// 获取当前的模块的动态标识号
TUnionModuleID UnionGetMyCurrentDynamicMDLID();

// 获取模块的静态标识号
TUnionModuleID UnionGetMyFixedMDLID();

// 获取模块的带进程号的静态标识号
TUnionModuleID UnionGetMySpecifiedFixedMDLID();

// 获取指定类型的模块的静态标识号
TUnionModuleID UnionGetFixedMDLIDOfMDLType(TUnionMDLType mdlType);

// 获取指定模块的ID号
TUnionModuleID UnionGetFixedMDLIDOfSpecSvr(int specID);

#endif

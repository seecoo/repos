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

// ��ȡָ��������Ĵ���ģ��ID��
TUnionModuleID UnionGetFixedMDLIDOfTaskOfHsm(char *hsmIPAddr);

// ��ȡָ�������������Ĵ���ģ��ID��
TUnionModuleID UnionGetFixedMDLIDOfTaskOfHsmGrp(char *hsmGrpID);

int UnionGetHsmGrpIDOutOfPort(int port,char *hsmGrpID);

// ���ɶ�̬ģ���ʶ��
long UnionGenerateDynamicMDLNum();

// ����ģ������
int UnionSetMyModuleType(TUnionMDLType myMDLType);

// ��ȡģ������
TUnionMDLType UnionGetMyModuleType();

// ��ģ���ʶ���л�ȡģ������
TUnionMDLType UnionGetMDLTypeOutOfMDLID(TUnionModuleID id);

// ��ģ���ʶ���л�ȡ���̺�
int UnionGetPIDOutOfMDLID(TUnionModuleID id);

// ��ģ���ʶ���л�ȡ��̬ģ���ʶ��
int UnionGetDynamicIDOutOfMDLID(TUnionModuleID id);

// ����ģ��Ķ�̬��ʶ��
TUnionModuleID UnionGenerateMyDynamicMDLID();

// ��ȡ��ǰ��ģ��Ķ�̬��ʶ��
TUnionModuleID UnionGetMyCurrentDynamicMDLID();

// ��ȡģ��ľ�̬��ʶ��
TUnionModuleID UnionGetMyFixedMDLID();

// ��ȡģ��Ĵ����̺ŵľ�̬��ʶ��
TUnionModuleID UnionGetMySpecifiedFixedMDLID();

// ��ȡָ�����͵�ģ��ľ�̬��ʶ��
TUnionModuleID UnionGetFixedMDLIDOfMDLType(TUnionMDLType mdlType);

// ��ȡָ��ģ���ID��
TUnionModuleID UnionGetFixedMDLIDOfSpecSvr(int specID);

#endif

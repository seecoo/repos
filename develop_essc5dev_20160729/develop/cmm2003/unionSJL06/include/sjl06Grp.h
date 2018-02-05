//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2002/10/31

#ifndef _UnionSJL06Group_1_x_
#define _UnionSJL06Group_1_x_

#include "sjl06.h"

#define conMaxNumOfSJL06		100
#define conMDLNameOfUnionSJL06TBL	"UnionSJL06TBLMDL"
#define conMaxNumOfHsmCheckCmd		32

// 2008/6/6����
// ����һ��ָ�������������ļ����ָ��������Ƿ�����
// ���checkAnyway��0����ֻ����쳣����������������쳣�������ļ��ܻ�
int UnionIsSpecHsmNormal(char *ipAddr,int checkAnyway);
// 2008/6/3�����ӣ���һ��ָ�������ļ��ж�ȡ��������
int UnionReadTestCmdGrpOfHsmGrp(char *hsmGrpID,char cmdReq[][256+1],char cmdRes[][256+1],int maxExpectedNum);
// 2008/6/3����
// ����һ��ָ�������������ļ�����������״̬
int UnionCheckStatusOfWorkingSJL06WithCmdGrp();

// 2006/08/08����
PUnionSJL06 UnionGetCurrentSJL06Grp();
// 2006/08/08����
int UnionGetMaxNumOfSJL06InGrp();

int UnionAutoLoadSJL06IntoSJL06MDL();

int UnionConnectWorkingSJL06MDL();
int UnionDisconnectWorkingSJL06MDL();
int UnionRemoveWorkingSJL06MDL();

PUnionSJL06 UnionFindWorkingSJL06(char *ipAddr);
int UnionExistWorkingSJL06(PUnionSJL06 pSJL06);
int UnionAddWorkingSJL06(char *ipAddr);
int UnionDeleteWorkingSJL06(char *ipAddr);
int UnionSetWorkingSJL06Abnormal(PUnionSJL06 pSJL06);
int UnionSetWorkingSJL06Online(PUnionSJL06 pSJL06);
int UnionSetWorkingSJL06ColdBackup(PUnionSJL06 pSJL06);
int UnionIsOnlineWorkingSJL06(char *ipAddr);
int UnionSetSpecWorkingSJL06Abnormal(char *ipAddr);
int UnionSetSpecWorkingSJL06Online(char *ipAddr);
int UnionSetSpecWorkingSJL06ColdBackup(char *ipAddr);

int UnionPrintStatusOfWorkingSJL06(PUnionSJL06 pSJL06);
int UnionPrintWorkingSJL06(char *ipAddr);
int UnionPrintAllWorkingSJL06();
int UnionPrintAllWorkingSJL06ToFile(FILE *fp);
int UnionPrintAllWorkingSJL06ToSpecFile(char *fileName);

int UnionRefreshDynamicAttrsOfAllWorkingSJL06();

int UnionOutputWorkingSJL06(PUnionSJL06 pSJL06,int expectedNum);

int UnionFindAllWorkingSJL06InGroup(char *hsmGrpID,PUnionSJL06 pSJL06[],int expectedNum);

int UnionCheckStatusOfWorkingSJL06();

PUnionSJL06 UnionFindIdleWorkingSJL06(char *hsmGrpID);

int UnionResetWorkingSJL06CmdTimes(char *ipAddr);
int UnionResetAllWorkingSJL06CmdTimes();

int UnionReloadWorkingSJL06(char *ipAddr);

int UnionExistSJL06InHsmGrpConfFile(char *ipAddr);

int UnionAddSJL06IntoHsmGrpConfFile(char *ipAddr,int connNum);

int UnionDeleteSJL06FromHsmGrpConfFile(char *ipAddr);

int UnionDeleteSJL06FromHsmGrpConfFile(char *ipAddr);

int UnionDeleteWorkingSJL06Anyway(char *ipAddr);

int UnionGetSjl06GrpTraceStr(PUnionSJL06 pSJL06, char *str);

// Add By HuangBaoxin, 2010-11-30
int UnionClearWorkingSJL06(int index);
int UnionChangeWorkingSJL06(int index, char *ipAddr);
// Add End

#endif

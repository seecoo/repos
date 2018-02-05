//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2002/10/31
// 	Version:	1.0

//	2004/06/22,Wolfgang Wang, �޸���TUnionSJL06StaticAttr�Ľṹ

//	2006/08/08,Wolfgang Wang, �޸���TUnionSJL06DynamicAttr�Ľṹ

#ifndef _UnionSJL06_1_x_
#define _UnionSJL06_1_x_

#define errSJL06Abnormal		-101
#define errSJL06CmdResultError		-102

#define conAbnormalSJL06		'0'
#define conOnlineSJL06			'1'
#define conColdBackupSJL06		'2'

#define conHsmCmdVersionSJL06		"SJL06"
#define conHsmCmdVersionRacal		"RACAL"
#define conHsmCmdVersionSJL05		"SJL05"
#define conHsmCmdVersionSJJ11270	"11270"
#define conHsmCmdVersionSJJ1127S	"1127S"

typedef enum 
{
	conHsmCmdOfSJL06,
	conHsmCmdOfRacal,
	conHsmCmdOfSJL05,
	conHsmCmdOfSJJ11270,
	conHsmCmdOfSJJ1127S,
	conHsmCmdUnknown
} TUnionHsmCmdVersion;
typedef TUnionHsmCmdVersion		*PUnionHsmCmdVersion;

typedef struct
{
	char	hsmGrpID[3+1];		// ������Hsm�����������
	char	hsmCmdVersion[6+1];	// ָ��İ汾
	char	ipAddr[16];		// IP��ַ��Ωһ��ʶһ̨HSM��Ωһֵ������
	int	port;			// �˿ںţ�ȱʡΪ8
	int	lenOfLenFld;		// ���ȵ���Ķȣ�Ϊ{0,2}
	int	lenOfMsgHeader;		// ��Ϣͷ��ĳ��ȣ�Ϊ[0~12]
	char	msgHeader[12+1];	// ���ڹ̶�ֵ����Ϣͷ����ֵ
	char	remark[20+1];		// ��ע
	int	maxConLongConn;		// ����ͬʱ���ӵ��������������ȱʡΪ1
	char	registerDate[8+1];	// ����ʱ�䣬�Զ�ȡϵͳ����
	// Added by Wolfgang Wang, 2004/06/22
#ifdef _UnionSJL06_2_x_Above_
	char	testCmdReq[128];	// ����ָ��
	char	testCmdSuccessRes[128];	// ����ָ����Ӧ�ɹ���־
#endif
	// End of Addition Of 2004/06/22
} TUnionSJL06StaticAttr;

typedef struct
{
	// ����������HSM����״̬�ĸı���ı�
	// ָ��ִ�е��ܴ��� = ���¸�Times�ĺ�
	long	timeoutTimes;		// ��ʱ�Ĵ��������ܻ���ʱ����Ӧ
	long	connFailTimes;		// �����쳣�Ĵ������޷�����ܻ����ͻ�Ӽ��ܻ�����
	long	abnormalCmdTimes;	// ָ���쳣ִ�еĴ���������ֵ�Ƿ�00
	long	normalCmdTimes;		// ָ������ִ�еĴ���,����ֵ��00
	// ���˼���abnormalCmdTimes���⣬��������Times���Զ����뱾����
	long	continueFailTimes;	// ����ָ�������ִ�еĴ���
	int	downTimes;		// Down���Ĵ���
	char	lastDownDate[8+6+1];	// ���һ���쳣ʱ��
	char	status;			// �������ǰ״̬
					/*
					��0�������ϣ�������
					��1��������ʹ��״̬
					��2�����䱸�ã�����ǰδʹ��
					*/
	int	activeLongConn;		// ��ǰ���ߵĳ���������
} TUnionSJL06DynamicAttr;

typedef struct
{
	TUnionSJL06StaticAttr	staticAttr;
	TUnionSJL06DynamicAttr	dynamicAttr;
} TUnionSJL06;
typedef TUnionSJL06	*PUnionSJL06;

int UnionGetFileNameOfDefaultHsmDef(char *fileName);

int UnionReadDefaultAttrOfSJL06(PUnionSJL06 psjl06);
int UnionEditSJL06Conf();
int UnionCreateSJL06Conf(char *ipAddr);
int UnionDeleteSJL06Conf(char *ipAddr);
int UnionPrintSJL06Conf(char *ipAddr);

int UnionInsertSJL06Rec(PUnionSJL06 pSJL06);
int UnionDeleteSJL06Rec(char *pipAddr);
int UnionSelectSJL06Rec(PUnionSJL06 pSJL06Grp);
int UnionPrintSJL06Rec(PUnionSJL06 pSJL06Grp);

int UnionWriteSJL06RecToFile(FILE *fp,PUnionSJL06 pSJL06);
int UnionRewriteSJL06Rec(PUnionSJL06 pSJL06);

int UnionExistSJL06Def(char *ipAddr);

int UnionUpdateSJL06StaticAttr(PUnionSJL06 pSJL06);
int UnionUpdateSJL06DynamicAttr(PUnionSJL06 pSJL06);

int UnionIsValidHsmCmdVersion(char *version);
int UnionGetHsmCmdVersion(PUnionSJL06 pSJL06);

// 2006/7/26���ӣ�����һ������������ļ�������������ļ�������
// ����$UNIONETC/HsmCfg/default.CFG������
int UnionCreateSJL06RecWhenNotExist(char *ipAddr);

int UnionModifySJL06StaticAttr(PUnionSJL06 psjl06);

int UnionPrintAllSJL06RecToSpecFile(char *fileName);

#endif

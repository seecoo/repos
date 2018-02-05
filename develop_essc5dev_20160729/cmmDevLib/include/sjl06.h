//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2002/10/31
// 	Version:	1.0

//	2004/06/22,Wolfgang Wang, 修改了TUnionSJL06StaticAttr的结构

//	2006/08/08,Wolfgang Wang, 修改了TUnionSJL06DynamicAttr的结构

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
	char	hsmGrpID[3+1];		// 所属的Hsm工作组的名字
	char	hsmCmdVersion[6+1];	// 指令集的版本
	char	ipAddr[16];		// IP地址，惟一标识一台HSM，惟一值，主键
	int	port;			// 端口号，缺省为8
	int	lenOfLenFld;		// 长度的域的度，为{0,2}
	int	lenOfMsgHeader;		// 消息头域的长度，为[0~12]
	char	msgHeader[12+1];	// 对于固定值的消息头，其值
	char	remark[20+1];		// 备注
	int	maxConLongConn;		// 允许同时连接的最大长连接数量，缺省为1
	char	registerDate[8+1];	// 创建时间，自动取系统日期
	// Added by Wolfgang Wang, 2004/06/22
#ifdef _UnionSJL06_2_x_Above_
	char	testCmdReq[128];	// 测试指令
	char	testCmdSuccessRes[128];	// 测试指令响应成功标志
#endif
	// End of Addition Of 2004/06/22
} TUnionSJL06StaticAttr;

typedef struct
{
	// 以下域随着HSM工作状态的改变而改变
	// 指令执行的总次数 = 以下各Times的和
	long	timeoutTimes;		// 超时的次数，加密机超时无响应
	long	connFailTimes;		// 链接异常的次数，无法向加密机发送或从加密机接收
	long	abnormalCmdTimes;	// 指令异常执行的次数，返回值是非00
	long	normalCmdTimes;		// 指令正常执行的次数,返回值是00
	// 除了计入abnormalCmdTimes的外，计入其它Times的自动计入本变量
	long	continueFailTimes;	// 连续指令非正常执行的次数
	int	downTimes;		// Down机的次数
	char	lastDownDate[8+6+1];	// 最近一次异常时间
	char	status;			// 密码机当前状态
					/*
					‘0’，故障，不可用
					‘1’，正在使用状态
					‘2’，冷备用，但当前未使用
					*/
	int	activeLongConn;		// 当前在线的长连接数量
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

// 2006/7/26增加，创建一个密码机配置文件，如果其配置文件不存在
// 根据$UNIONETC/HsmCfg/default.CFG来创建
int UnionCreateSJL06RecWhenNotExist(char *ipAddr);

int UnionModifySJL06StaticAttr(PUnionSJL06 psjl06);

int UnionPrintAllSJL06RecToSpecFile(char *fileName);

#endif

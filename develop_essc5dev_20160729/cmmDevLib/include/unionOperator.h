// Wolfgang Wang
// 2004/12/08

#ifndef _UnionOperator_
#define _UnionOperator_

#define conMDLNameOfOperatorTable	"operatorTBLMDL"

/*
#define errOperatorAlreadyLogon			-1001
#define errOperatorNotLogon			-1002
#define errOperatorTooMuchWrongPasswordTimes	-1003
#define errOperatorWrongPassword		-1004
#define errOperatorTooLongTimeWithoutOperation	-1005
#define errOperatorPasswordLocked		-1006
#define errOperatorNotRegister			-1007
*/

typedef struct
{
	char		id[8+1];			// 柜员编号
	char		name[20+1];			// 姓名
	char		level;				// '1'~'9'
	unsigned char	password[16+1];			// 
	char		passwordLastUpdatedDate[8+1];	// 密码最近修改日期
	char		registerDate[8+1];		// 登记时期
	char		lastLogonDateTime[14+1];	// 最近一次登录时间
	long		logonTimes;			// 登录次数
	long		lastOperationTime;		// 最近一次操作时间
	char		lastOperationDateTime[14+1];	// 最近一次操作时间
	long		operationTimes;			// 操作的次数
	int		passwordInputWrongTimes;	// 密码连续出错的时间
	short		isCurrentLogon;			// 当前是否登录
} TUnionOperator;
typedef TUnionOperator	*PUnionOperator;

typedef struct
{
	long		maxNumOfOperator;			// 最大的柜员数
	long		passwordMaxEffectiveDays;		// 密码的最大有效日期
	long		maxIdleTimeBetweenTwoOperations;	// 一次登录允许的两次操作间的最大空闲
	PUnionOperator	poperatorList;
} TUnionOperatorTable;
typedef  TUnionOperatorTable	*PUnionOperatorTable;

PUnionOperator UnionConnectCurrentOperatorTable();
int UnionGetCurrentMaxNumOfOperator();

int UnionEncryptOperatorPassword(char *tellerNo,char *passwd,unsigned char *epasswd);
int UnionFormOperator(char *tellerNo,char *name,char level,char *passwd,PUnionOperator poper);
int UnionIsOperatorTableConnected();
int UnionGetConfFileNameOfOperatorTable(char *fileName);
int UnionCreateOperatorTable(long maxNumOfOperator,long passwordMaxEffectiveDays,long maxIdleTimeBetweenTwoOperations);
int UnionDeleteOperatorTable();
long UnionGetMaxVarNumOfOperatorTable();
int UnionConnectOperatorTable();
int UnionDisconnectOperatorTable();
int UnionRemoveOperatorTable();
int UnionReloadOperatorTable();
int UnionPrintOperatorToFile(PUnionOperator poperator,FILE *fp);
int UnionPrintOperatorTableAttrToFile(FILE *fp);
int UnionPrintOperatorTableToFile(FILE *fp);
PUnionOperator UnionReadOperator(char *tellerNo);
int UnionUpdateOperatorPasswordDirectly(char *tellerNo,unsigned char *newpasswdCrytogram);
int UnionUpdateOperatorPassword(char *tellerNo,char *newpasswd);
int UnionUpdateOperatorName(char *tellerNo,char *name);
int UnionUpdateOperatorLevel(char *tellerNo,char level);
int UnionMirrorOperatorTable();
int UnionUpdateOperatorPasswordMaxEffectiveDays(long passwordMaxEffectiveDays);
int UnionUpdateOperatorMaxIdleTimeBetweenTwoOperations(long maxIdleTimeBetweenTwoOperations);
int UnionInsertOperator(char *tellerNo,char *name,char level,char *passwd);
int UnionDeleteOperator(char *tellerNo);
char UnionGetOperatorLevel(char *tellerNo);

int UnionOperatorLogon(char *tellerNo,char *password);
int UnionOperatorLogoff(char *tellerNo);
int UnionUnlockOperatorPassword(char *tellerNo);
int UnionLockOperatorPassword(char *tellerNo);
int UnionApplyOperatorOperation(char *tellerNo);
long UnionGetOperatorDaysBeforePasswordOutdate(char *tellerNo);
int UnionIsOperatorLogon(char *tellerNo);
// 将指定的域输出到文件，如果fldList为NULL或为空，将所有域输出到文件
// 合法的域名称为ID/NAME/LEVEL/passwordInputWrongTimes/LastLogonDateTime/IsCurrentLogon
// 分别对应标识/姓名/级别/密码连接出错次数/最后一次登录时间/当前是否登录
// 域串中可以包括多个域名
int UnionPrintOperatorTableToSpecFile(char *fileName,char *fldList);

int UnionConvertOperatorTableError(int errno,char *str);

int UnionIsTellerStillActiveLogon(char *tellerNo);

int UnionPrintOperator(char *tellerNo);

#endif

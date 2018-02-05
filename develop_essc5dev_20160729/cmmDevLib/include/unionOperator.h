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
	char		id[8+1];			// ��Ա���
	char		name[20+1];			// ����
	char		level;				// '1'~'9'
	unsigned char	password[16+1];			// 
	char		passwordLastUpdatedDate[8+1];	// ��������޸�����
	char		registerDate[8+1];		// �Ǽ�ʱ��
	char		lastLogonDateTime[14+1];	// ���һ�ε�¼ʱ��
	long		logonTimes;			// ��¼����
	long		lastOperationTime;		// ���һ�β���ʱ��
	char		lastOperationDateTime[14+1];	// ���һ�β���ʱ��
	long		operationTimes;			// �����Ĵ���
	int		passwordInputWrongTimes;	// �������������ʱ��
	short		isCurrentLogon;			// ��ǰ�Ƿ��¼
} TUnionOperator;
typedef TUnionOperator	*PUnionOperator;

typedef struct
{
	long		maxNumOfOperator;			// ���Ĺ�Ա��
	long		passwordMaxEffectiveDays;		// ����������Ч����
	long		maxIdleTimeBetweenTwoOperations;	// һ�ε�¼��������β������������
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
// ��ָ������������ļ������fldListΪNULL��Ϊ�գ���������������ļ�
// �Ϸ���������ΪID/NAME/LEVEL/passwordInputWrongTimes/LastLogonDateTime/IsCurrentLogon
// �ֱ��Ӧ��ʶ/����/����/�������ӳ������/���һ�ε�¼ʱ��/��ǰ�Ƿ��¼
// ���п��԰����������
int UnionPrintOperatorTableToSpecFile(char *fileName,char *fldList);

int UnionConvertOperatorTableError(int errno,char *str);

int UnionIsTellerStillActiveLogon(char *tellerNo);

int UnionPrintOperator(char *tellerNo);

#endif

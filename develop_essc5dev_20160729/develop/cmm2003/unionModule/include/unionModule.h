//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0

#ifndef _UnionSharedMemoryModuleMDL_
#define _UnionSharedMemoryModuleMDL_

#define conMDLNameOfUnionSharedMemoryModuleTBL	"UnionSharedMemoryModuleTBLMDL"

#ifdef _unionModule_2_x_above_
int UnionReloadSharedMemoryModule(char *mdlName);
int UnionReloadAllSharedMemoryModule(char *anywayOrNot);
#endif

typedef struct
{
	char	name[40+1];		// ����
	int	id;			// id��
#ifdef _unionModule_2_x_above_
	char	reloadCommand[80+1];	// �Զ���������
#else
	char	version[10+1];		// �汾��
#endif
	int	index;			// ģ����ģ����е�������,�Զ�����
	long	sizeOfUserSpace;	// �û��Զ������ݿռ�Ĵ�С
	unsigned char *puserSpace;	// ָ���û��Զ������ݿռ��ָ��
	int	users;			// ��ǰʹ�ñ�ģ����û���Ŀ
	int	newCreated;		// ģ���Ƿ��½���ʶ
	int	writingLocks;		// ������Ŀ
	int	readingLocks;		// д����Ŀ
} TUnionSharedMemoryModule;
typedef TUnionSharedMemoryModule			*PUnionSharedMemoryModule;

// 2008/12/16,��ȡ�û��Ĺ����ڴ�id��
int UnionReadUserIDOfSharedMemoryModule(char *mdlName);

int UnionReadSharedMemoryModuleDef(char *mdlName,PUnionSharedMemoryModule pmdl);
int UnionPrintSharedMemoryModuleToFile(PUnionSharedMemoryModule pmdl,FILE *fp);
int UnionPrintSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp);
int UnionPrintSharedMemoryModule(PUnionSharedMemoryModule pmdl);

int UnionGetNameOfSharedMemoryModuleTBL(char *fileName);

int UnionRemoveAllSharedMemoryModule();

PUnionSharedMemoryModule UnionConnectSharedMemoryModule(char *mdlName,long sizeOfUserSpace);
PUnionSharedMemoryModule UnionConnectExistingSharedMemoryModule(char *mdlName,long sizeOfUserSpace);
int UnionDisconnectShareModule(PUnionSharedMemoryModule);
int UnionRemoveSharedMemoryModule(char *mdlName);

int UnionIsNewCreatedSharedMemoryModule(PUnionSharedMemoryModule pmdl);
unsigned char *UnionGetAddrOfSharedMemoryModuleUserSpace(PUnionSharedMemoryModule pmdl);

int UnionApplyWritingLocks(PUnionSharedMemoryModule pmdl);
int UnionReleaseWritingLocks(PUnionSharedMemoryModule pmdl);
int UnionApplyReadingLocks(PUnionSharedMemoryModule pmdl);
int UnionReleaseReadingLocks(PUnionSharedMemoryModule pmdl);

int UnionResetWritingLocks(char *mdlName);
int UnionResetReadingLocks(char *mdlName);

int UnionGetUserIDOfSharedMemoryModule(PUnionSharedMemoryModule pmdl);

// 2006/12/27 ����
int UnionPrintExistedSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp);
// 2006/12/27 �������º���
PUnionSharedMemoryModule UnionConnectExistedSharedMemoryModule(char *mdlName);

int UnionReadingLockModuleByModuleName(char *mdlName);

int UnionWritingLockModuleByModuleName(char *mdlName);

// 2007/10/26 ����
#define conMaxNumOfSharedMemoryPerProcess	32
typedef struct
{
	int				num;
	PUnionSharedMemoryModule	pshmPtrGrp[conMaxNumOfSharedMemoryPerProcess];
} TUnionSharedMemoryGroup;
typedef TUnionSharedMemoryGroup		*PUnionSharedMemoryGroup;

// 2007/10/26����
int UnionIsSharedMemoryGroupInited();

// 2007/10/26����
void UnionInitSharedMemoryGroup();

// 2007/10/26����
void UnionAddIntoSharedMemoryGroup(PUnionSharedMemoryModule pmdl);

// 2007/10/26����
void UnionDeleteFromSharedMemoryGroup(PUnionSharedMemoryModule pmdl);

// 2007/10/26����
int UnionIsSharedMemoryGroupIsFull();

// 2007/10/26����
int UnionIsSharedMemoryInited(char *mdlName);

// 2007/10/26����
int UnionOutputAllRegisteredShareMemory(char *buf,int sizeOfBuf);

// 2007/10/26����
void UnionFreeUnnecessarySharedMemory();

/*
����
	��ȡָ�����ƵĹ����ڴ�����ò���
�������
	mdlName	�����ڴ�����
�������
	prec	�����ڴ��¼
����ֵ
	>=0	�ɹ�
	<0	�������
*/
int UnionReadSharedMemoryMDLTBLRec(char *mdlName,PUnionSharedMemoryModule prec);

/*
����
	�������м�¼
�������
	maxNum		���Զ������������
�������
	recGrp		�����ļ�¼
����ֵ
	>=0	�����ļ�¼��
	<0	�������
*/
long UnionSelectAllSharedMemoryMDLTBLRec(TUnionSharedMemoryModule recGrp[],int maxNum);

int UnionPrintAllExistedSharedMemoryModule(char *outputFile);

#endif

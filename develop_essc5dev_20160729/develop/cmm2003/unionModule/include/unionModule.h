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
	char	name[40+1];		// 名称
	int	id;			// id号
#ifdef _unionModule_2_x_above_
	char	reloadCommand[80+1];	// 自动加载命令
#else
	char	version[10+1];		// 版本号
#endif
	int	index;			// 模块在模块表中的索引号,自动分配
	long	sizeOfUserSpace;	// 用户自定义数据空间的大小
	unsigned char *puserSpace;	// 指向用户自定义数据空间的指针
	int	users;			// 当前使用本模块的用户数目
	int	newCreated;		// 模块是否新建标识
	int	writingLocks;		// 读锁数目
	int	readingLocks;		// 写锁数目
} TUnionSharedMemoryModule;
typedef TUnionSharedMemoryModule			*PUnionSharedMemoryModule;

// 2008/12/16,读取用户的共享内存id号
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

// 2006/12/27 增加
int UnionPrintExistedSharedMemoryModuleToFileByModuleName(char *mdlName,FILE *fp);
// 2006/12/27 增加以下函数
PUnionSharedMemoryModule UnionConnectExistedSharedMemoryModule(char *mdlName);

int UnionReadingLockModuleByModuleName(char *mdlName);

int UnionWritingLockModuleByModuleName(char *mdlName);

// 2007/10/26 增加
#define conMaxNumOfSharedMemoryPerProcess	32
typedef struct
{
	int				num;
	PUnionSharedMemoryModule	pshmPtrGrp[conMaxNumOfSharedMemoryPerProcess];
} TUnionSharedMemoryGroup;
typedef TUnionSharedMemoryGroup		*PUnionSharedMemoryGroup;

// 2007/10/26增加
int UnionIsSharedMemoryGroupInited();

// 2007/10/26增加
void UnionInitSharedMemoryGroup();

// 2007/10/26增加
void UnionAddIntoSharedMemoryGroup(PUnionSharedMemoryModule pmdl);

// 2007/10/26增加
void UnionDeleteFromSharedMemoryGroup(PUnionSharedMemoryModule pmdl);

// 2007/10/26增加
int UnionIsSharedMemoryGroupIsFull();

// 2007/10/26增加
int UnionIsSharedMemoryInited(char *mdlName);

// 2007/10/26增加
int UnionOutputAllRegisteredShareMemory(char *buf,int sizeOfBuf);

// 2007/10/26增加
void UnionFreeUnnecessarySharedMemory();

/*
功能
	读取指定名称的共享内存的配置参数
输入参数
	mdlName	共享内存名称
输出参数
	prec	共享内存记录
返回值
	>=0	成功
	<0	出错代码
*/
int UnionReadSharedMemoryMDLTBLRec(char *mdlName,PUnionSharedMemoryModule prec);

/*
功能
	读出所有记录
输入参数
	maxNum		可以读出的最大数量
输出参数
	recGrp		读出的记录
返回值
	>=0	读出的记录数
	<0	出错代码
*/
long UnionSelectAllSharedMemoryMDLTBLRec(TUnionSharedMemoryModule recGrp[],int maxNum);

int UnionPrintAllExistedSharedMemoryModule(char *outputFile);

#endif

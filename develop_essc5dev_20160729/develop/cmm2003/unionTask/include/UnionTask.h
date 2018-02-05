//	Author:		Wolfgang Wang
//	Date:		2002/1/18
//	Version:	3.0

/*	
	2003/09/19 Wolfgang Wang升级为3.0增加了 _UnionTask_3_x_ 相关代码
*/

// 2004/06/18 升级为3.2

#ifndef _UnionTaskMDL
#define _UnionTaskMDL

int UnionCreateProcess();
int UnionIgnoreSignals();

#ifndef _UnionTask_3_x_
// 2003/03/05, 王纯军，将该值由200改为1000
#define MaxTasks		1000

typedef struct
{
        char    TaskName[40+1];
        int     ProcID;
        char    RegisterTime[14+1];
} TUnionTask;
typedef TUnionTask      *PUnionTask;

typedef struct
{
	int	ProcID[MaxTasks];
	char	TaskName[MaxTasks][40+1];
	char	RegisterTime[MaxTasks][14+1];
} TUnionTaskRegistry;
typedef TUnionTaskRegistry	*PUnionTaskRegistry;

// Anytime before calling functions of the UnionTaskMDL the first time,
// please call the following fucntion.
int UnionConnectTaskMDL(char *,int IDOfTaskMDL);

// Anytime when no use of the functions of the UnionTaskMDL, please call
// one of the following functions.
// Difference of the two functions:
// UnionDisconnectTaskMDL will not remove the TaskRegistry, so other application
// still could use the TaskRegistry.
// UnionRemoveTaskMDL will remove the TaskRegistry.
int UnionDisconnectTaskMDL();
int UnionRemoveTaskMDL(int IDOfTaskMDL);

int UnionKillAllTask();
int UnionKillTaskByName(char *TaskName);
int UnionKillTaskByProcID(int ProcID);

#ifdef _UnionTask_2_x_
int UnionRegisterTask(char *,...);
#else
int UnionRegisterTask(char *TaskName);
#endif

int UnionUnregisterTask();

int UnionPrintTaskRegistry();
int UnionMaintainTaskRegistry();

long UnionOutputTasks(unsigned char *TaskBuf,int BufSize);

int UnionKillProcess(int ProcID);
int UnionTaskExist(char *TaskName);

// 2002/12/17, 王纯军增加
int UnionStartTaskRegistryMaintainer(int Seconds);
int UnionTerminateTaskRegistryMaintainer(int Seconds);
void UnionDealTaskRegistryMaintainerExit();

#endif // ifndef _UnionTask_3_x_

// Added by Wolfgang Wang, 2003/09/19
#ifdef _UnionTask_3_x_

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif
#include "UnionLog.h"

#define _UnionEnv_3_x_
#include "UnionEnv.h"

// 定义任务类型
typedef struct
{
	char			name[40+1];			// 任务名
	char			startCmd[128+1];		// 创建任务实例的命令
	int			minNum;				// 必须同时存在的最小的实例数目
	int			currentNum;			// 实例的个数
	char			logFileName[40+1];		// 日志文件名
} TUnionTaskClass;
typedef TUnionTaskClass		*PUnionTaskClass;

// 定义任务实例的个性属性
typedef struct
{
	PUnionTaskClass		pclass;				// 任务类
	int			procID;				// 进程号
	char			startTime[14+1];		// 启动时间
	PUnionLogFile		plogFile;			// 日志文件
#ifdef _UnionTask_3_2_
	char			name[40+1];			// 任务名称
#endif
} TUnionTaskInstance;
typedef TUnionTaskInstance	*PUnionTaskInstance;

// 定义任务列表

// 定义任务模块的缺省名
#define conMDLNameOfUnionTaskTBL		"UnionTaskTBLMDL"

// 定义任务模块
typedef struct
{
	int			maxClassNum;			// 模块管理的任务类的最大数目
	int			maxInstanceNum;			// 模块管理的任务实例的最大数目
	PUnionTaskClass		pclassTBL;			// 任务定义列表
	PUnionTaskInstance	pinstanceTBL;			// 任务实例列表
} TUnionTaskTBL;
typedef TUnionTaskTBL		*PUnionTaskTBL;

// 从一个配置文件初始化 UnionTaskTBL
/*
	配置文件名称为：	$HOME/etc/UnionTaskTBL.CFG
	这个文件定义以下变量：
		变量				变量说明/对应PUnionTaskMDL的域
		maxClassNum			maxClassNum
		maxInstanceNum			maxInstanceNum
		和任务定义，每个任务定义占据一行，每行包括
		[name]	[startCmd]  [minNum]  [logFileName]		
*/
int UnionConnectTaskTBL();
int UnionDisconnectTaskTBL();
int UnionRemoveTaskTBL();
int UnionReloadTaskTBL();
int UnionPrintTaskTBL();

// 20060808 增加
PUnionTaskClass UnionGetCurrentTaskClassGrp();
// 20060808 增加
PUnionTaskInstance UnionGetCurrentTaskInstanceGrp();
// 20060808 增加
int UnionGetCurrentTaskClassNum();
// 20060808 增加
int UnionGetCurrentTaskInstanceNum();

int UnionPrintTaskClassInTBL();
int UnionPrintTaskInstanceInTBL();
int UnionPrintStatusOfTaskTBL();
int UnionPrintTaskClassInTBLToFile(FILE *fp);
int UnionPrintTaskInstanceInTBLToFile(FILE *fp);
int UnionPrintStatusOfTaskTBLToFile(FILE *fp);
int UnionPrintTaskClassToFile(PUnionTaskClass ptaskClass,FILE *fp);

int UnionPrintTaskClassInFormatToFile(PUnionTaskClass ptaskClass,FILE *fp);
int UnionPrintTaskClassInTBLInFormatToFile(FILE *fp);

PUnionTaskInstance UnionFindTaskInstance(int procID);
PUnionTaskClass UnionFindTaskClass(char *taskName);

PUnionTaskInstance UnionCreateTaskInstance(int (*UnionUserDefinedTaskExitFun)(),char *fmt,...);
PUnionTaskInstance UnionCreateTaskInstanceOfName(int (*UnionUserDefinedTaskExitFun)(),char *fmt,...);

PUnionTaskInstance UnionResetTaskInstanceLogFile(int (*UnionUserDefinedTaskExitFun)(),char *fmt,...);
int UnionUnregisterTaskInstance(PUnionTaskInstance pinstance);
int UnionUnregisterThisTaskInstance();

int UnionKillTaskInstanceByName(char *taskName);
int UnionKillTaskInstanceByAlikeName(char *taskName);
int UnionKillAllTaskInstance();
int UnionKillTaskInstanceByProcID(int taskProcID);
int UnionPrintTaskInstanceToFile(PUnionTaskInstance ptaskInstance,FILE *fp);

int UnionStartTaskGuard(int seconds);
int UnionClearRubbishTaskInstance(int seconds);

PUnionLogFile UnionGetLogFile();

int UnionGetMaxNumOfTaskClass();
int UnionGetMaxNumOfTaskInstance();

#endif // define _UnionTask_3_x_

int UnionExistsTaskOfName(char *taskName);
int UnionExistsAnotherTaskOfName(char *taskName);

// End of Addition of 2003/09/19

// added by Wolfgang Wang, 2005/09/08
int UnionPrintTaskInstanceByAlikeNameToFile(char *taskName,FILE *fp);
int UnionCloseTaskInstanceByAlikeName(char *taskName);	// 这个函数和Kill函数的区别在于，他还置这类函数的启动数量为0。
// 这个函数和Kill函数的区别在于，他还置这类函数的启动数量为0。
int UnionCloseTaskInstanceByName(char *taskName);

// added by Wolfgang Wang,2005/10/09
int UnionRenameTaskNameOfMyself(char *fmt,...);

// 2006/7/26 增加以下函数
// 检查是否有名字中含有taskName的任务类型定义,返回数量
int UnionExistsTaskClassOfAlikeName(char *taskName);
// 检查是否有名字为taskName的任务类型定义,返回数量
int UnionExistsTaskClassOfName(char *taskName);
// 增加一个新的任务类型定义
int UnionAddNewTaskClass(char *taskName,char *command,int num,char *logFileName);
// 删除一个任务类型定义
int UnionDeleteTaskClass(char *taskName);
// 更新一个任务类型定义
// 如果command,logFileName为空，不更新，如果num为负值，不更新
int UnionUpdateTaskClass(char *taskName,char *command,int num,char *logFileName);

int UnionGetNameOfTaskTBL(char *fileName);

// 将必须在线的任务的指定的域输出到文件中
// 合法的域名
// minnum/currentnum/name/logfile/startcmd
int UnionPrintMustOnlineTaskClassToSpecFile(char *fileName,char *fldList);

// 将指定的域输出到文件中
// 合法的域名
// minnum/currentnum/name/logfile/startcmd
// activeNum指明输出的任务的当前启动数量必须达到的数目
int UnionPrintTaskClassInTBLToSpecFile(char *fileName,char *fldList,int activeNum);

// 将异常任务输出到文件中
// 合法的域名
// minnum/currentnum/name/logfile/startcmd
int UnionPrintAbnormalTaskClassToSpecFile(char *fileName,char *fldList);
// 将任务实例输出到文件中
// 合法的域名
// pid/starttime/logfile
int UnionPrintTaskInstanceToSpecFile(char *fileName,char *fldList);

// 2006/9/8增加
int UnionReloadTaskClass(char *taskName);

// 2006/11/01增加
// 重置任务的当前数量值为真正值
int UnionResetTaskNumOfName(char *taskName);
// 设置任务的当前数量为指定值
int UnionSetTaskNumOfName(char *taskName,int num);

// 2008/10/24，王纯军增加
int UnionExistsTaskOfAlikeName(char *taskName);

// wangk add 2009-9-24
int UnionPrintTaskClassInTBLInRecStrToSpecFile(char *fileName);
// wangk add end 2009-9-24
// wangk add 2009-9-29
int UnionPrintTaskInstanceInRecStrFormatToSpecFile(char *fileName);
// wangk add end 2009-9-29

// added 2015-06-08
// 更新类表的实例的子进程类数目
int UnionUpdateClientClassNum(char *className, char *logName, int num);
// 统计各个实例名的数目
int UnionCountTaskInstanceNumByNameGrp(int grpNum, char instanceNameGrp[][64], int *numGrp);
// 20150605 zhangyd增加，用于子进程创建任务实例(不创建类)
PUnionTaskInstance UnionCreateTaskInstanceForClient(int (*UnionTaskActionBeforeExit)(),char *fmt,...);
// 用于子进程创建任务实例, 如果与实例同名的类不存在则创建类记录
PUnionTaskInstance UnionCreateTaskInstanceForSub(char *parentLog, int (*UnionTaskActionBeforeExit)(), char *fmt,...);
// end of addition 2015-06-08

#endif

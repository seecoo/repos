//	Author:		Wolfgang Wang
//	Date:		2002/1/18
//	Version:	3.0

/*	
	2003/09/19 Wolfgang Wang����Ϊ3.0������ _UnionTask_3_x_ ��ش���
*/

// 2004/06/18 ����Ϊ3.2

#ifndef _UnionTaskMDL
#define _UnionTaskMDL

int UnionCreateProcess();
int UnionIgnoreSignals();

#ifndef _UnionTask_3_x_
// 2003/03/05, ������������ֵ��200��Ϊ1000
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

// 2002/12/17, ����������
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

// ������������
typedef struct
{
	char			name[40+1];			// ������
	char			startCmd[128+1];		// ��������ʵ��������
	int			minNum;				// ����ͬʱ���ڵ���С��ʵ����Ŀ
	int			currentNum;			// ʵ���ĸ���
	char			logFileName[40+1];		// ��־�ļ���
} TUnionTaskClass;
typedef TUnionTaskClass		*PUnionTaskClass;

// ��������ʵ���ĸ�������
typedef struct
{
	PUnionTaskClass		pclass;				// ������
	int			procID;				// ���̺�
	char			startTime[14+1];		// ����ʱ��
	PUnionLogFile		plogFile;			// ��־�ļ�
#ifdef _UnionTask_3_2_
	char			name[40+1];			// ��������
#endif
} TUnionTaskInstance;
typedef TUnionTaskInstance	*PUnionTaskInstance;

// ���������б�

// ��������ģ���ȱʡ��
#define conMDLNameOfUnionTaskTBL		"UnionTaskTBLMDL"

// ��������ģ��
typedef struct
{
	int			maxClassNum;			// ģ������������������Ŀ
	int			maxInstanceNum;			// ģ����������ʵ���������Ŀ
	PUnionTaskClass		pclassTBL;			// �������б�
	PUnionTaskInstance	pinstanceTBL;			// ����ʵ���б�
} TUnionTaskTBL;
typedef TUnionTaskTBL		*PUnionTaskTBL;

// ��һ�������ļ���ʼ�� UnionTaskTBL
/*
	�����ļ�����Ϊ��	$HOME/etc/UnionTaskTBL.CFG
	����ļ��������±�����
		����				����˵��/��ӦPUnionTaskMDL����
		maxClassNum			maxClassNum
		maxInstanceNum			maxInstanceNum
		�������壬ÿ��������ռ��һ�У�ÿ�а���
		[name]	[startCmd]  [minNum]  [logFileName]		
*/
int UnionConnectTaskTBL();
int UnionDisconnectTaskTBL();
int UnionRemoveTaskTBL();
int UnionReloadTaskTBL();
int UnionPrintTaskTBL();

// 20060808 ����
PUnionTaskClass UnionGetCurrentTaskClassGrp();
// 20060808 ����
PUnionTaskInstance UnionGetCurrentTaskInstanceGrp();
// 20060808 ����
int UnionGetCurrentTaskClassNum();
// 20060808 ����
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
int UnionCloseTaskInstanceByAlikeName(char *taskName);	// ���������Kill�������������ڣ����������ຯ������������Ϊ0��
// ���������Kill�������������ڣ����������ຯ������������Ϊ0��
int UnionCloseTaskInstanceByName(char *taskName);

// added by Wolfgang Wang,2005/10/09
int UnionRenameTaskNameOfMyself(char *fmt,...);

// 2006/7/26 �������º���
// ����Ƿ��������к���taskName���������Ͷ���,��������
int UnionExistsTaskClassOfAlikeName(char *taskName);
// ����Ƿ�������ΪtaskName���������Ͷ���,��������
int UnionExistsTaskClassOfName(char *taskName);
// ����һ���µ��������Ͷ���
int UnionAddNewTaskClass(char *taskName,char *command,int num,char *logFileName);
// ɾ��һ���������Ͷ���
int UnionDeleteTaskClass(char *taskName);
// ����һ���������Ͷ���
// ���command,logFileNameΪ�գ������£����numΪ��ֵ��������
int UnionUpdateTaskClass(char *taskName,char *command,int num,char *logFileName);

int UnionGetNameOfTaskTBL(char *fileName);

// ���������ߵ������ָ������������ļ���
// �Ϸ�������
// minnum/currentnum/name/logfile/startcmd
int UnionPrintMustOnlineTaskClassToSpecFile(char *fileName,char *fldList);

// ��ָ������������ļ���
// �Ϸ�������
// minnum/currentnum/name/logfile/startcmd
// activeNumָ�����������ĵ�ǰ������������ﵽ����Ŀ
int UnionPrintTaskClassInTBLToSpecFile(char *fileName,char *fldList,int activeNum);

// ���쳣����������ļ���
// �Ϸ�������
// minnum/currentnum/name/logfile/startcmd
int UnionPrintAbnormalTaskClassToSpecFile(char *fileName,char *fldList);
// ������ʵ��������ļ���
// �Ϸ�������
// pid/starttime/logfile
int UnionPrintTaskInstanceToSpecFile(char *fileName,char *fldList);

// 2006/9/8����
int UnionReloadTaskClass(char *taskName);

// 2006/11/01����
// ��������ĵ�ǰ����ֵΪ����ֵ
int UnionResetTaskNumOfName(char *taskName);
// ��������ĵ�ǰ����Ϊָ��ֵ
int UnionSetTaskNumOfName(char *taskName,int num);

// 2008/10/24������������
int UnionExistsTaskOfAlikeName(char *taskName);

// wangk add 2009-9-24
int UnionPrintTaskClassInTBLInRecStrToSpecFile(char *fileName);
// wangk add end 2009-9-24
// wangk add 2009-9-29
int UnionPrintTaskInstanceInRecStrFormatToSpecFile(char *fileName);
// wangk add end 2009-9-29

// added 2015-06-08
// ��������ʵ�����ӽ�������Ŀ
int UnionUpdateClientClassNum(char *className, char *logName, int num);
// ͳ�Ƹ���ʵ��������Ŀ
int UnionCountTaskInstanceNumByNameGrp(int grpNum, char instanceNameGrp[][64], int *numGrp);
// 20150605 zhangyd���ӣ������ӽ��̴�������ʵ��(��������)
PUnionTaskInstance UnionCreateTaskInstanceForClient(int (*UnionTaskActionBeforeExit)(),char *fmt,...);
// �����ӽ��̴�������ʵ��, �����ʵ��ͬ�����಻�����򴴽����¼
PUnionTaskInstance UnionCreateTaskInstanceForSub(char *parentLog, int (*UnionTaskActionBeforeExit)(), char *fmt,...);
// end of addition 2015-06-08

#endif

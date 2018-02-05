// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/8/10
// Version:	1.0

#ifndef _operationControl_
#define _operationControl_

typedef struct
{
	int	resID;		// 资源ID
	int	resCmd;		// 资源命令
	long	roles;		// 可以操作的角色，每一位的数字代表一个角色。
				// 值为0时，表示不限制操作。为负数时，表示禁止任何角色操作。
				// 321，表示角色3、2、1可以操作。149，表示角色1、4、9可操作
	char	remark[40+1];	// 操作说明
} TUnionOperationControlRec;
typedef TUnionOperationControlRec	*PUnionOperationControlRec;

// 获得命令的类型
int UnionGetMngSvrOperationType(int resID,int resCmd);

int UnionGetDefFileNameOfOperationController(char *fileName);

// 读取资源操作的说明
char *UnionGetOperationRemark(int resID,int operationID);

// 从字符串中读出操作定义
int UnionReadOperationFromDefStr(char *str,PUnionOperationControlRec prec);
// 从文件中读取操作
int UnionReadSpecOperationFromDefFile(int resID,int resCmd,PUnionOperationControlRec prec);
// 从文件中读取操作
int UnionReadSpecOperationFromSpecDefFile(char *fileName,int resID,int resCmd,PUnionOperationControlRec prec);

// 判断角色级别是不是授权了
int UnionIsAuthorizedRoleLevel(long roles,char roleLevel);

// 验证角色级别能否进行这个操作
int UnionVerifyOperation(int resID,int resCmd,char roleLevel);

// 验证角色级别能否进行这个操作
int UnionVerifyOperationInSpecFile(char *fileName,int resID,int resCmd,char roleLevel);

int UnionPrintOperationToFile(PUnionOperationControlRec prec,FILE *fp);

int UnionPrintAllSpecOperationToFile(FILE *outputFP,int resID,int resCmd,char roleLevel);

int UnionPrintAllSpecOperationToSpecFile(char *fileName,int resID,int resCmd,char roleLevel);

#endif

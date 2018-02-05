#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionLogonMode.h"
#include "unionTeller.h"
//#include "kmcDefaultHsm.h"
#include "sjl06Cmd.h"
#include "UnionStr.h"

char *UnionReadResMngClientIPAddr();

// 验证操作员登陆
int UnionVerifyOperatorLogin(char *tellerNo, int lenOfReqStr, char *reqStr)
{
	return 0;
}

/*
功能：
	判断一个柜员是否登录
输入参数:
	idOfOperator 用户定义的，用于唯一识别一条记录的名称
输出参数:
	无
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionIsTellerStillLogon(char *idOfOperator)
{
	return(0);
}

// 判断密码是否过期
// 输入：passwdMTime - 密码最近修改日期
// 返回：>=0 - 还剩下的天数；<0 - 已过期
int UnionIsPasswdOverTime(char *passwdMTime)
{
	return (0);
}

// 更新操作员的登陆次数和最近登陆时间
// 输入：tellerNo - 操作员ID
// 返回：=0 - 成功；<0 - 失败
int UnionUpdateOperationLogTimes(char *tellerNo)
{
	return 0;
}

// 验证操作员的空闲时间是否超限
// 输入：tellerNo - 操作员ID
// 返回：=0 - 成功；<0 - 失败
int UnionVerifyOperatorFreeTime(char *tellerNo)
{
	return 0;
}

// 验证合法客户端
// 输入：tellerNo - 操作员ID
// 返回：=0 - 成功；<0 - 失败
int UnionVerifyLegitimateClient(char *tellerNo)
{
	return 0;
}

// 判断操作员是否被锁
// 1-被锁；0-没有被锁
int UnionIsOperatorLocked(char *tellerNo)
{
        return 0;
}

// 判断密码是不是弱密码, 0-不是; 1-是
int UnionIsPasswdTooSimple(char *tellerNo, char *passwd)
{
	return 0;
}

int UnionSetOperatorPasswordInputWrongTimes(char *tellerNo)
{
	return 0;
}

int UnionReSetOperatorPasswordInputWrongTimes(char *tellerNo)
{
	return 0;
}

int UnionVerifyAuthOperator(int lenOfReqStr, char *reqStr)
{
	return 0;
}

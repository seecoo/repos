//	Wolfgang Wang, 2006/08/08

#ifndef _teller_
#define _teller_

/*
功能：
	读取操作员级别标识
输入参数:
	level		操作员级别
	sizeOfBuf	域值缓冲大小
输出参数:
	operatorTypeID	操作员级别标识
返回值
	>=0		成功
	<0		失败,错误码
*/
int UnionReadOperatorTypemacroValueByLevel(int level,char *operatorTypeID,int sizeOfBuf);

int UnionGetTellerLevelTypeID(char *tellerNo,char *tellerTypeID);

// 获得登录状态
int UnionIsCurrentTellerLogonOK();

// 设置当前柜员登录成功
void UnionSetCurrentTellerLogonOK();

// 获得级别
char UnionGetTellerLevel();

/*
功能	获得当前柜员号
输入参数
	无
输出参数
	无
返回值
	指向柜员员的指针
*/
char *UnionGetTellerNo();

/*
功能	设置当前柜员信息
输入参数
	tellerNo	当前柜员号
	tellerName	柜员名
	level           级别
输出参数
	无
返回值
	>=0	正确
	<0	失败
*/
void UnionSetTellerInfo(char *tellerNo,char *tellerName,char level);

#ifdef _WIN32_
/*
功能	柜员登录
输入参数
	id	柜员号
	passwd	柜员密码
输出参数
	无
返回值
	true	成功
        false   失败
*/
bool UnionOperatorLogon(char *id,char *passwd);

// 退出登录
void UnionOperatorLogoff();
#endif

#endif

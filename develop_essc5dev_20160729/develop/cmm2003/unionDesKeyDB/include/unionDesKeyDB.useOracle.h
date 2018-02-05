#ifndef _unionDesKeyDB_useOracle_
#define _unionDesKeyDB_useOracle_

#include "unionDesKey.h"
#include "unionDesKeyDB.h"

/*
	函数功能：
		设置数据库用户和密码
	输入参数：无
	输出参数：无
	函数返回：0
*/
int UnionInitDBOnOracle();

/*
	函数功能：
		连接数据库
	输入参数：
		timeout：超时(秒)，>=0超时时间，<0不设置超时
	输出参数：无
	函数返回：
		0成功，<0失败
*/
int UnionConnectDBOnOracle(int timeout);

/*
	函数功能：
		提交事务，断开连接
	输入参数：无
	输出参数：无
	函数返回：sqlcode
*/
int UnionCommitAndDisconnectOnOracle();

/*
	函数功能：
		记录数据库错误信息日志
	输入参数：
	输出参数：
	函数返回：
*/
void UnionLogErrMsgOnOracle();

/*
	函数功能：
		回滚事务，断开连接
	输入参数：无
	输出参数：无
	函数返回：sqlcode
*/
int UnionRollbackAndDisconnectOnOracle();

/*
	函数功能：
		deskey初始化全局变量
	输入参数：
	输出参数：
	函数返回：
*/
void UnionInitDesKeyGlobalVar();

/*
	函数功能：
		打印deskey全局变量
	输入参数：
	输出参数：
	函数返回：	
*/
void UnionLogDesKeyGlobalVar();

/*
	函数功能：
		deskey给全局变量赋值
	输入参数：
	输出参数：
	函数返回：
*/
void UnionSetDesKeyGlobalVar(PUnionDesKey pkey);

/*
	函数功能：
		全局变量给deskey赋值
	输入参数：
	输出参数：
	函数返回：
*/
void UnionSetGlobalVarToDesKey(PUnionDesKey pkey);

/*
	函数功能：
		插入密钥库
	输入参数：
		pkey：密钥定义
	输出参数：无
	函数返回：
		=0成功，<0失败
*/
int UnionInsertDesKeyOnOracle(PUnionDesKey pkey);

/*
	函数功能：
		更新密钥
	输入参数：
		pkey：密钥定义
	输出参数：
	函数返回：
		>0成功，=0未找到记录，<0失败
*/
int UnionUpdateDesKeyOnOracle(PUnionDesKey pkey);

/*
	函数功能：
		删除密钥
	输入参数：
		pkey：密钥定义
	输出参数：
	函数返回：
		>0成功，=0未找到记录，<0失败	
*/
int UnionDeleteDesKeyOnOracle(char *fullName);

/*
	函数功能：
		查询参数值
	输入参数：
		paramName：参数名
		sizeOfValue: 参数paramValue的大小
	输出参数：
		paramValue:参数值
	函数返回：
		>0输出参数的长度，=0未查询到数据，<0失败		
*/
int UnionGetParamValueOnOracle(char *paramName, char *paramValue, int sizeOfValue);

/*
	函数功能：
		插入参数值
	输入参数：
		paramName：参数名
		paramValue: 参数值
		paramRemark: 备注
	输出参数：
	函数返回：
		=0成功，<0失败		
*/
int UnionInsertParamValueOnOracle(char *paramName, char *paramValue, char *paramRemark);

/*
	函数功能：
		更新参数值
	输入参数：
		paramName：参数名
		paramValue: 参数值
		paramRemark: 备注
	输出参数：
	函数返回：
		>0成功，=0未找到记录，<0失败		
*/
int UnionUpdateUnionParamOnOracle(char *paramName, char *paramValue, char *paramRemark);

/*
	函数功能：
		从密钥库查询所有密钥，保存在共享内存区
	输入参数：
	输出参数：
	函数返回：
		=0成功，<0失败	
*/
int UnionLoadDesKeyDBIntoMemoryOnOracle();

/*
	函数功能：
		获取maxKeyNum
	输入参数：
	输出参数：
	函数返回：
		>0成功返回maxKeyNum，<=0失败	
*/
long UnionGetMaxKeyNumFromKeyDBOnOracle();

/*
	函数功能：
		获取num
	输入参数：
	输出参数：
	函数返回：
		>0成功返回num，<=0失败	
*/
long UnionGetNumFromKeyDBOnOracle();

/*
	函数功能：
		获取LASTUPDATEDTIME
	输入参数：
	输出参数：
	函数返回：
		char* 指针，NULL失败	
*/
char *UnionGetDesKeyDBLastUpdatedTimeOnOracle();

/*
	函数功能：
		创建UnionDesKeyParam表和UnionDesKeyDB表,并插入参数MAXKEYNUM、NUM和LASTUPDATEDTIME
	输入参数：
	输出参数：
	函数返回：
		=0成功，<0失败	
*/
int UnionCreateDesKeyDBOnOracle(PUnionDesKeyDB pDesKeyDB);

/*
	函数功能：
		删除UnionDesKeyParam表和UnionDesKeyDB表
	输入参数：
	输出参数：
	函数返回：
		=0成功，<0失败	
*/
int UnionDeleteDesKeyDBOnOracle();


/*
	函数功能：
		更新num
	输入参数：
	输出参数：
	函数返回：
		=0成功，<0失败	
*/
int UnionUpdateNumFromKeyDBOnOracle();

#endif

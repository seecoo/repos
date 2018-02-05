// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _database_
#define _database_

// 获得当前数据库的名称
char *UnionGetCurrentDatabaseName();

// 获得数据库的目录
int UnionGetDatabaseDir(char *databaseName,char *dir);

/*
功能：创建一个数据库
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionCreateDatabase(char *databaseName);

/*
功能：删除一个数据库
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDropDatabase(char *databaseName);

/*
功能：判断一个数据库是否存在
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	>0：数据库存在
	0：数据库不存在
	<0：失败
*/
int UnionExistDatabase(char *databaseName);

/*
功能：建立与数据库的连接
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionConnectDatabase(char *databaseName);
/*
功能：断开与数据库的连接
输入参数：
	无
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDisconnectDatabase();

#endif

// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _table_
#define _table_

int UnionGetDirOfTable(char *tableName,char *dir);

int UnionGetDirOfTable(char *tableName,char *dir);

/*
功能：创建一张表
输入参数：
	tableName：表名
	primaryKeyStr	表的关键字域名，如果关键字由多个域组成，则各个域名之间用'|'分隔
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionCreateTable(char *tableName,char *primaryKeyStr);

/*
功能：删除一张表
输入参数：
	tableName：表名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDropTable(char *tableName);

/*
功能：判断一张表是否存在
输入参数：
	tableName：表名
输出参数：
	无
返回值：
	>0：表存在
	0：表不存在
	<0：失败
*/
int UnionExistTable(char *tableName);

#endif

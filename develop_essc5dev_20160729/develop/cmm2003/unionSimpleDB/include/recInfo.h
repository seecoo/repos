// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _recInfo_
#define _recInfo_

// 获得记录信息文件
int UnionGetFileNameOfRecInfo(char *tableName,char *fileName);

/*
功能：创建一个表的记录信息文件，文件中只有两行，第一行为“0”，第2行为表的关键字
输入参数：
	tableName：对应的表名
	keyStr：表的关键字域名，如果关键字由多个域组成，则各个域之间用'|'分隔
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionCreateRecInfo(char *tableName,char *keyStr);

/*
功能：在记录信息文件中增加一条记录的记录文件名称，同时修改记录总数
输入参数：
	tableName：对应的表名
	recFileName：要增加的记录文件名称
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionAddToRecInfo(char *tableName,char *recFileName);

/*
功能：从记录信息文件中删除一条记录的记录文件名称，同时修改记录总数
输入参数：
	tableName：对应的表名
	recFileName：要删除的记录文件名称
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDeleteFromRecInfo(char *tableName,char *recFileName);

/*
功能：从记录信息文件获取一张表的记录总数
输入参数：
	tableName：对应的表名
输出参数：
	无
返回值：
	>=0：成功，返回表的记录总数
	<0：失败
*/
int UnionGetTableRecordNumber(char *tableName);

/*
功能：从记录信息文件获取一张表的记录总数和关键字
输入参数：
	tableName：对应的表名
	sizeOfPrimaryKey	关键字缓冲的大小
输出参数：
	primaryKey      关键字
返回值：
	>=0：成功，返回表的记录总数
	<0：失败
*/
int UnionGetTableRecInfo(char *tableName,char *primaryKey,int sizeOfPrimaryKey);

/*
功能：从记录信息文件获取一张表的记录总数和关键字
输入参数：
	tableName：对应的表名
输出参数：
	primaryKey      关键字
返回值：
	>=0：		关键字的长度
	<0：失败
*/
int UnionGetPrimaryKeyFromTableRecInfo(char *tableName,char *primaryKey,int sizeOfPrimaryKey);

#endif

// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _allTableInfo_
#define _allTableInfo_

// 获得文件名称
int UnionGetAllTableInfoFileName(char *fileName);

/*
功能：创建一个新的表信息文件，文件中只有一个值"0"
输入参数：
	databaseName：数据库名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionCreateAllTableInfo();

/*
功能：在表信息文件中增加一张表的名称，同时修改表的总数
输入参数：
	databaseName：要增加的表名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionAddToAllTableInfo(char *tableName);
/*
功能：从表信息文件中删除一张表的名称，同时修改表的总数
输入参数：
	databaseName：要删除的表名
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDeleteFromAllTableInfo(char *tableName);

/*
功能：从表信息文件获取一个数据库中表的总数
输入参数：
	无
输出参数：
	无
返回值：
	>=0：成功，返回表的总数
	<0：失败
*/
int UnionGetAllTableNumber();

/*
将所有的表信息写入到文件中
*/
int UnionPrintAllTableNameToFile(char *fileName);

#endif

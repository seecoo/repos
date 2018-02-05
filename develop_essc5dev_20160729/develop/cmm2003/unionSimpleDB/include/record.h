// Author:	ChenJiaMei
// Date:	2008-7-16

#ifndef _record_
#define _record_

/*
功能：	从一个关键字定义串中读出各个字段
输入参数：
	defStr		关键字定义串
	lenOfDefStr	关键字定义串长度
        maxFldNum       关键字数组的大小
输出参数：
	primaryKeyGrp	关键字数组
返回值：
	>=0：		关键字包括的域数目
	<0：		错误代码
*/
int UnionReadPrimaryKeyFldGrpFromDefStr(char *defStr,int lenOfDefStr,char primaryKeyGrp[][128+1],int maxFldNum);

// 根据表格，记录条件，获得记录文件的名称
int UnionGetRecordFileNameFromCondition(char *tableName,char *condition,int lenOfCondition,char *fileName,char *keyStr,int sizeOfKeyStr);

/*
功能：	从记录信息文件中读出关键字各字段
输入参数：
	tableName	表名
        maxFldNum       关键字数组的大小
输出参数：
	primaryKeyGrp	关键字数组
返回值：
	>=0：		关键字包括的域数目
	<0：		错误代码
*/
int UnionReadPrimaryKeyFldGrpFromRecInfoFile(char *tableName,char primaryKeyGrp[][128+1],int maxFldNum);

// 根据表格，记录名，获得记录文件的名称
int UnionGetRecordFileName(char *tableName,char *recNullFileName,char *fileName);

// 判断是否存在记录文件
int UnionExistsRecordFile(char *recordFileName);

// 根据记录和键值组成记录的文件名称
int UnionFormRecordFileName(char *tableName,char *record,int lenOfRecord,char *fileName,int sizeOfFileName);
/*
功能：在表中插入一条记录
输入参数：
	tableName：表名
	record：要插入的记录，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：记录的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionInsertRecord(char *tableName,char *record,int lenOfRecord);

/*
功能：删除表中的一条记录
输入参数：
	tableName：表名
	key：要删除记录的键值，格式为"关键字域1=域值|关键字域2=域值|…|关键字域N=域值"
	lenOfKey：键值的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionDeleteRecord(char *tableName,char *key,int lenOfKey);

/*
功能：修改表中的一条记录，关键字不能修改
输入参数：
	tableName：表名
	record：要修改的记录，格式为"域1=域值|域2=域值|…|域N=域值"，
		必须带关键字域且关键字域不能修改，其余域可以只包括要修改的域
	lenOfRecord：记录的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败
*/
int UnionUpdateRecord(char *tableName,char *record,int lenOfRecord);

/*
功能：根据关键字查询一条记录
输入参数：
	tableName：表名
	key：记录关键字，格式为"关键字域1=域值|关键字域2=域值|…|关键字域N=域值"
	lenOfKey：关键字的长度
        sizeOfRecord    记录缓冲的大小
输出参数：
	record：查找出的记录
返回值：
	>=0：成功，返回记录的长度
	<0：失败
*/
int UnionSelectRecord(char *tableName,char *key,int lenOfKey,char *record,int sizeOfRecord);

/*
功能：将所有记录写入到文件中
输入参数：
	tableName：表名
	fileName	写入到的文件名称
输出参数：
	无
返回值：
	>=0：		记录数
	<0：		错误代码
*/
int UnionPrintAllRecordToFile(char *tableName,char *condition,char *fileName);

#endif

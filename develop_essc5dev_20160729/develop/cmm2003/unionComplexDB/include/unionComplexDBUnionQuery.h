//	Author: Wolfgang Wang
//	Date: 2008-10-23

#ifndef _unionComplexDBUnionQuery_
#define _unionComplexDBUnionQuery_

/*
功能：
	执行联合查询动作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteUnionSelectSpecRecInStr(char *triggerStr,int lenOfTriggerStr,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr);

/*
功能：
	执行一个文件中的联合查询操作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteUnionSelectSpecRecInFile(char *fileName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr);

/*
功能：
	执行一个表的联合查询操作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteUnionSelectSpecRecOnObject(char *resName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr);

/*
功能：批量查询一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：查询条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	fileName：存储了查询出来的实例的值的文件名
返回值：
	>=0：成功，返回查询出的记录的数目
	<0：失败，错误码
*/
int UnionBatchSelectUnionObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName);

#endif


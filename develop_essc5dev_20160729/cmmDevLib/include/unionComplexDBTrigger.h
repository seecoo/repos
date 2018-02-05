//	Author: Wolfgang Wang
//	Date: 2008-10-23

#ifndef _unionComplexDBTrigger_
#define _unionComplexDBTrigger_

/*
功能	打开触发器触发的动作
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionOpenOperationOnTrigger();

/*
功能	关闭触发器触发的动作
输入参数
	无
输出参数
	无
返回值
	无
*/
void UnionCloseOperationOnTrigger();

/*
功能	判断是否是触发器触发的动作
输入参数
	无
输出参数
	无
返回值
	1	是
	0	不是
*/
int UnionIsOperationOnTrigger();

//---------------------------------------------------------------------------
/*
功能	根据定义的trigger条件，以及操作条件，拼成trigger需要的操作条件
输入参数
	oriCondition		trigger定义的赋值方法
	lenOfOriCondition	trigger定义的赋值方法长度
	recStr			操作使用的实际条件
	lenOfRecStr		操作使用的实际条件的长度
	sizeOfDesCondition	目标条件缓冲的大小
输出参数
	desCondition		目标条件
返回值
	>= 0			目标条件的长度
	<0			出错代码
*/
int UnionFormConditionFromTriggerDefinedStr(char *oriCondition,int lenOfOriCondition,char *recStr,int lenOfRecStr,char *desCondition,int sizeOfDesCondition);

/*
功能：
	执行触发器动作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
	fileRecved	是否收到了文件
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationInStr(char *triggerStr,int lenOfTriggerStr,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved);

/*
功能：
	执行一个文件中的触发器操作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
	fileRecved	是否收到了文件
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationInFile(char *fileName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved);

/*
功能：
	执行一个表的触发器操作
输入参数：
	resName		对象名称
	operationTag	操作标识
	beforeOrAfter	是操作之前执行还是之后执行
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
	fileRecved	是否收到了文件
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationOnObject(char *resName,char *operationTag,char *beforeOrAfter,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved);

/*
功能：
	执行一个表的触发器操作
输入参数：
	resName		对象名称
	operationID	操作标识
	isBeforeOperation	是操作之前执行还是之后执行，1 表示操作之前
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr);

#endif
 


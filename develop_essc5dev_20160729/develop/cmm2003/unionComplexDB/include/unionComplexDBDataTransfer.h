//	Author: Wolfgang Wang
//	Date: 2008-10-23

#ifndef _unionComplexDBDataTransfer_
#define _unionComplexDBDataTransfer_

/*
功能：
	导出表中的所有符合条件的记录
输入参数：
	resName		对象名称
	condition	条件
	outputFileName	文件名称，记录写入到该文件中
输出参数：
	无
返回值：
	0：		导出的记录数
	<0：		失败，错误码
*/
int UnionOutputAllRecFromSpecTBL(char *resName,char *condition,char *outputFileName);

/*
功能：
	将记录导入表中
输入参数：
	resName		对象名称
	inputFileName	文件名称，记录保存在该文件中
输出参数：
	无
返回值：
	0：		导入的记录数
	<0：		失败，错误码
*/
int UnionInputAllRecIntoSpecTBL(char *resName,char *inputFileName);

#endif

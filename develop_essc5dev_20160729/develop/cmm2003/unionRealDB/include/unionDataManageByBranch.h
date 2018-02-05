//	Author: Wolfgang Wang
//	Date: 2008-12-3

#ifndef _unionDataManageByBranch_
#define _unionDataManageByBranch_

/*
功能：
	读取操作员的授权
输入参数:
	idOfOperator 	用户定义的，用于唯一识别一条记录的名称
输出参数:
	branchID	操作员所属机构
	dataReadAuth	操作员操作权限
返回值
	>=0		成功
	<0		失败,错误码
*/
int UnionReadAuthDefOfOperator(char *idOfOperator,char *branchID,int *dataReadAuth);

/*
功能：
	判断是否分机构管理数据
输入参数：
	无
输出参数：
	无
返回值：
	1：		是
	0：		否
*/
int UnionIsManageDataByBranch();

/*
功能：
	读机构域的名称
输入参数：
	idOfObject：	对象ID
输出参数：
	branchFldName	机构域的名称
返回值：
	>=0：		成功，串长度
	<0：		失败，错误码
*/
int UnionReadBranchFldNameOfSpecTBL(char *tblName,char *branchFldName);
	
/*
功能：
	自动赋加查询条件
输入参数：
	idOfObject：对象ID
	oriConLen:	源条件的长度
	condition：查询条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
	sizeOfBuf	条件串缓冲的大小
输出参数：
	condition	赋加了自动查询域的条件串
返回值：
	>=0：		成功，串长度
	<0：		失败，错误码
*/
int UnionAutoAppendBranchIDToQueryCondition(char *tblName,int oriConLen,char *condition,int sizeOfBuf);

/*
功能：
	自动赋加数据串
输入参数：
	idOfObject：	对象ID
	record		记录串，"域1=域值|域2=域值|域3=域值|…域N=域值|"
	lenOfRecord	记录串长度
输出参数：
	record		赋加了自动域的记录串
返回值：
	>=0：		成功，串长度
	<0：		失败，错误码
*/
int UnionAutoAppendFldOfSpecTBL(char *idOfObject,char *record,int lenOfRecord);

#endif

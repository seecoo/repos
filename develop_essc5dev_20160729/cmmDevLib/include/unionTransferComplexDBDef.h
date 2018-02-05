//	Wolfgang Wang
//	2009/4/29

#ifndef _unionTransferComplexDBDef_
#define unionTransferComplexDBDef

#include "unionComplexDBCommon.h"
#include "unionComplexDBObjectDef.h"
#include "unionTableField.h"
#include "unionTableList.h"

/*
功能	
	判断一个域定义是否存在
输入参数
	pfldRec	tableField的记录定义
输出参数
	无
返回值
	errCodeNameIsUsed	同名的存在，但定义不同
	1	存在
	其它	不存在
*/
int UnionExistSpecTableFieldRec(PUnionTableField pfldRec);

/*
功能	
	获得一个表域定义的标识
输入参数
	tableName	表名
	fldName		域名称
输出参数
	fldID		域标识
返回值
	>=0	成功
	<0	错误码
*/
int UnionFormTableFieldRecFldID(char *tableName,char *fldName,char *fldID);

/*
功能	
	将一个complexDB定义的域转换为tableField的记录定义
输入参数
	tableName	表名
	pfldDef		complexDB定义的域
输出参数
	pfldRec		tableField的记录定义
返回值
	>=0	成功
	<0	错误码
*/
int UnionTransferComplexDBFldDefToTableFieldRec(char *tableName,PUnionObjectFldDef pfldDef,PUnionTableField pfldRec);

/*
功能	
	将一个fldGrp定义写入到清单串中
输入参数
	tableName	表名
	pfldGrpDef	域定义
输出参数
	fldListStr	域清单串
返回值
	>=0	成功,串长度
	<0	错误码
*/
int UnionPutObjectFldGrpDefIntoStr(char *tableName,PUnionObjectFldGrp pfldGrpDef,char *fldListStr);

/*
功能	
	根据资源名称，读以complexDB定义的资源ID号
输入参数
	resName		资源名称
输出参数
	无
返回值
	>=0	资源ID
	<0	错误码
*/
int UnionReadResIDOfSpecResNameForComplexDB(char *resName);

/*
功能	
	将一个complexDB定义的对象转换为tableList的记录定义
输入参数
	
	pobjDef		complexDB定义的对象
输出参数
	ptblRec		tableList的记录定义
返回值
	>=0	成功
	<0	错误码
*/
int UnionTransferComplexDBObjectDefToTableListRec(PUnionObject pobjDef,PUnionTableList ptblRec);

/*
功能	
	将一个complexDB定义的对象插入到tabelList中
输入参数
	tableName	表名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInsertComplexDBObjectDefIntoTableList(char *tableName);

#endif

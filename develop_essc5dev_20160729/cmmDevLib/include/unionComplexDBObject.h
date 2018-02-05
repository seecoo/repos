//	Author: ChenJiaMei
//	Date: 2008-8-7

#ifndef _unionComplexDBObject_
#define _unionComplexDBObject_

#include "unionComplexDBObjectDef.h"

int UnionConnectDatabase();

int UnionCloseDatabase();

/*
功能：根据预先定义的SQL文件创建一个对象
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCreateObject(TUnionIDOfObject idOfObject);

/*
功能：
	创建一个对象，如果已存在，先删除
输入参数：
	prec	对象
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCreateObjectAnyway(PUnionObject prec);

/*
功能：删除一个对象
输入参数：
	idOfObject：对象的ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionDropObject(TUnionIDOfObject idOfObject);

#endif

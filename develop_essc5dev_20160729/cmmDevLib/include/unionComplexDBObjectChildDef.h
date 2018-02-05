//	Author: ChenJiaMei
//	Date: 2008-8-1

#ifndef _unionComplexDBObjectChildDef_
#define _unionComplexDBObjectChildDef_

/*
功能：检查一个对象的外部引用对象登记文件是否存在
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectForeignObjDefFile(TUnionIDOfObject idOfObject);

/*
功能：创建一个对象的外部引用对象登记文件
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionCreateObjectForeignObjDefFile(TUnionIDOfObject idOfObject);

/*
功能：检查是否有其它对象引用了本对象
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	>0：有其它对象引用本对象
	0：没有其它对象引用本对象
	<0：失败，错误码
*/
int UnionExistForeignObjectInfo(TUnionIDOfObject idOfObject);

/*
功能：登记引用本对象的外部对象的引用信息
输入参数：
	idOfObject：本对象ID
	foreignObjectGrp：引用本对象的外部对象关键字定义
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionRegisterForeignObjectInfo(TUnionIDOfObject idOfObject,TUnionChildObjectGrp foreignObjectGrp);

/*
功能：取消引用本对象的外部对象引用信息的登记
输入参数：
	idOfObject：本对象ID
	foreignObjectGrp：引用本对象的外部对象关键字定义
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCancelRegisterOfForeignObjectInfo(TUnionIDOfObject idOfObject,TUnionChildObjectGrp foreignObjectGrp);

#endif

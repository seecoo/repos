//	Author: ChenJiaMei
//	Date: 2008-8-6

#ifndef _recordValue_
#define _recordValue_

/*
功能：检查一个对象实例的值文件是否存在
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectRecValueFile(TUnionIDOfObject idOfObject,char *primaryKey);

/*
功能：新增一个对象实例的值文件
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	record：新记录，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：新记录的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionInsertObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord);

/*
功能：根据关键字删除一个对象实例的值文件
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey);

/*
功能：修改一个对象实例的值文件
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	record：要修改的记录，格式为"域1=域值|域2=域值|…|域N=域值"，只包括要修改的域
	lenOfRecord：要修改的记录的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionUpdateObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord);

/*
功能：根据关键字查询一个对象实例的值文件
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	sizeOfRecord：record的存储空间大小
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
*/
int UnionSelectObjectRecordValue(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord);

#endif

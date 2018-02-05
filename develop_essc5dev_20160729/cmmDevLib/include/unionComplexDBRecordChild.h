//	Author: ChenJiaMei
//	Date: 2008-8-7

#ifndef _recordChild_
#define _recordChild_

/*
功能：检查一个对象实例的外部引用文件是否存在
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	idOfForeignObject：外部对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject);

/*
功能：在一个对象实例的外部引用文件中增加一条记录
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	idOfForeignObject：外部对象ID
	foreignPrimaryKey：外部对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionAddObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject, char *foreignPrimaryKey);

/*
功能：在一个对象实例的外部引用文件中删除一条记录
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	idOfForeignObject：外部对象ID
	foreignPrimaryKey：外部对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteObjectRecChildFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,TUnionIDOfObject idOfForeignObject, char *foreignPrimaryKey);

/*
功能：检查一个对象的实例是否被其它对象的实例引用
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致	
输出参数：
	无
返回值：
	1：是
	0：不是
	<0：失败，错误码
*/
int UnionExistForeignObjectRecordDef(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey);

#endif

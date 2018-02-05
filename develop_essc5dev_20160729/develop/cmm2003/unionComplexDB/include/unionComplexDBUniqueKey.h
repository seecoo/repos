//	Author: ChenJiaMei
//	Date: 2008-8-6

#ifndef _uniqueKey_
#define _uniqueKey_

/*
功能：检查一个对象的某唯一值域组文件是否存在
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectUniqueKeyIndexFile(TUnionIDOfObject idOfObject,char *fldGrpName);

/*
功能：创建一个对象的某唯一值域组文件
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionCreateObjectUniqueKeyIndexFile(TUnionIDOfObject idOfObject,char *fldGrpName);

/*
功能：在一个对象的某唯一值域组文件中增加一条新记录的唯一值
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	uniqueKey：新记录的唯一值，格式为"唯一值域1的值.唯一值域2的值.唯一值域3的值…唯一值域N的值"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	storePosition：新记录的存储位置
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionAddObjectUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey,char *storePosition);

/*
功能：在一个对象的某唯一值域组文件中删除一条记录的唯一值
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	uniqueKey：要删除记录的唯一值，格式为"唯一值域1的值.唯一值域2的值.唯一值域3的值…唯一值域N的值"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteObjectUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey);

/*
功能：根据对象实例的唯一值获得实例的存储位置
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	uniqueKey：记录的唯一值，格式为"唯一值域1的值.唯一值域2的值.唯一值域3的值…唯一值域N的值"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	sizeofStorePosition：storePosition的存储空间大小
输出参数：
	storePosition：记录的存储位置
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionGetObjectRecStoreStationByUniqueKey(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey,char *storePosition,int sizeofStorePosition);

/*
功能：判断对象实例的唯一值是否重复
输入参数：
	idOfObject：对象ID
	fldGrpName：唯一值域名称，格式为"唯一值域1的名称.唯一值域2的名称.唯一值域3的名称…唯一值域N的名称"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
	uniqueKey：记录的唯一值，格式为"唯一值域1的值.唯一值域2的值.唯一值域3的值…唯一值域N的值"，
		唯一值域的排列顺序与对象定义文件中定义的唯一值域顺序一致
输出参数：
	无
返回值：
	1：重复
	0：不重复
	<0：失败，错误码
*/
int UnionObjectRecUniqueKeyIsRepeat(TUnionIDOfObject idOfObject,char *fldGrpName,char *uniqueKey);

#endif

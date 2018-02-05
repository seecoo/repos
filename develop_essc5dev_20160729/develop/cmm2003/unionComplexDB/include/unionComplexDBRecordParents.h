//	Author: ChenJiaMei
//	Date: 2008-8-7

#ifndef _recordParents_
#define _recordParents_

/*
功能：检查一个对象实例的内部引用文件是否存在
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
int UnionExistsObjectRecParentsFile(TUnionIDOfObject idOfObject,char *primaryKey);

/*
功能：创建一个对象实例的内部引用文件
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
int UnionCreateObjectRecParentsFile(TUnionIDOfObject idOfObject,char *primaryKey);

/*
功能：在一个对象实例的内部引用文件中删除一条记录
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	localFldGrp：本对象的本地域组的域名，格式为"本地域名1.本地域名2.本地域名3…本地域名N"，
		本地域名的排列顺序与本对象定义文件中定义的本地域组顺序一致
	idOfForeignObject：外部对象的ID
	foreignPrimaryKey：本对象引用的外部对象实例的关键字域值，
		格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDelObjectRecParentsFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey);

/*
功能：在一个对象实例的内部引用文件中增加一条记录
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	localFldGrp：本对象的本地域组的域名，格式为"本地域名1.本地域名2.本地域名3…本地域名N"，
		本地域名的排列顺序与本对象定义文件中定义的本地域组顺序一致
	idOfForeignObject：外部对象的ID
	foreignPrimaryKey：本对象引用的外部对象实例的关键字域值，
		格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionAddObjectRecParentsFile(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey);

/*
功能：取得一个实例引用的对象实例的关键字
输入参数：
	idOfLocalObject：本对象ID
	localPrimaryKey：本对象实例的关键字域值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与本对象定义文件中定义的键值域顺序一致
	localFldGrp：本地域组名称，格式为"域1的名称.域2的名称.域3的名称…域N的名称"，
		域的排列顺序与对象定义文件中外部关键字的本地域组中域顺序一致
	idOfForeignObject：外部对象的名称
	sizeOfPrimaryKey：foreignPrimaryKey的存储空间大小
输出参数：
	foreignPrimaryKey：本对象引用的外部对象实例的关键字域值，
		格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与外部对象定义文件中定义的键值域顺序一致
返回值：
	>=0：成功，返回foreignPrimaryKey的长度
	<0：失败，错误码
*/
int UnionObtainForeignObjectRecPrimaryKey(TUnionIDOfObject idOfLocalObject,char *localPrimaryKey,char *localFldGrp,TUnionIDOfObject idOfForeignObject,char *foreignPrimaryKey,int sizeOfPrimaryKey);

#endif

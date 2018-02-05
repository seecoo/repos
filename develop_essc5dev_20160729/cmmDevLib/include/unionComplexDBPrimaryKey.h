//	Author: ChenJiaMei
//	Date: 2008-8-6

#ifndef _primaryKey_
#define _primaryKey_

/*
功能：检查一个对象的键值文件是否存在
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectPrimaryKeyIndexFile(TUnionIDOfObject idOfObject);

/*
功能：创建一个对象的键值文件
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionCreateObjectPrimaryKeyIndexFile(TUnionIDOfObject idOfObject);

/*
功能：在一个对象的键值文件中增加一条新记录的键值
输入参数：
	idOfObject：对象ID
	primaryKey：新记录的键值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	storePosition：新记录的存储位置
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionAddObjectPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *storePosition);

/*
功能：在一个对象的键值文件中删除一条记录的键值
输入参数：
	idOfObject：对象ID
	primaryKey：要删除记录的键值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteObjectPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey);

/*
功能：根据对象实例的键值获得实例的存储位置
输入参数：
	idOfObject：对象ID
	primaryKey：记录的键值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
	sizeofStorePosition：storePosition的存储空间大小
输出参数：
	storePosition：记录的存储位置
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionGetObjectRecStoreStationByPaimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *storePosition,int sizeofStorePosition);

/*
功能：判断对象实例的键值是否重复
输入参数：
	idOfObject：对象ID
	primaryKey：记录的键值，格式为"键值域1的值.键值域2的值.键值域3的值…键值域N的值"，
		键值域的排列顺序与对象定义文件中定义的键值域顺序一致
输出参数：
	无
返回值：
	1：重复
	0：不重复
	<0：失败，错误码
*/
int UnionObjectRecPaimaryKeyIsRepeat(TUnionIDOfObject idOfObject,char *primaryKey);

/*
功能：获得对象的实例数目
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	>=0：实例数
	<0：失败，错误码
*/
int UnionGetObjectRecordNumber(TUnionIDOfObject idOfObject);

// Mary add begin, 20081112
/*
功能：读取指定表的所有记录
输入参数：
	tableName：	表名
	maxRecNum	缓冲可以缓冲的最大记录数量
输出参数：
	recKey		关键字，如一个关键字由多个域构成，使用.分开两个域
返回值：
	>=0：		记录数
	<0：		错误代码
*/
int UnionReadAllRecOfSpecTBL(char *tableName,char recKey[][128+1],int maxRecNum);
// Mary add end, 20081112

/*
function:
        判断是否是对象中的关键字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是关键字段
        0               不是关键字段
*/
int UnionIsPrimaryKeyFld(TUnionIDOfObject idOfObject, char *fldName);

/*
function:
        判断是否是对象中的关键字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是关键字段
        0               不是关键字段
*/
int UnionIsPrimaryKeyFldOfObject(PUnionObject pobject, char *fldName);

/*
function:
        判断是否是对象中的唯一值字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是唯一值字段
        0               不是唯一值字段
*/
int UnionIsUniqueFld(TUnionIDOfObject idOfObject, char *fldName);

/*
function:
        判断是否是对象中的唯一字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是唯一值字段
        0               不是唯一值字段
*/
int UnionIsUniqueFldOfObject(PUnionObject pobject, char *fldName);
#endif

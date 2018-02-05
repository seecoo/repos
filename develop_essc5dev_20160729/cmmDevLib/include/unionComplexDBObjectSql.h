//	Author: ChenJiaMei
//	Date: 2008-8-1

#ifndef _unionComplexDBObjectSql_
#define _unionComplexDBObjectSql_

#include "unionComplexDBObjectDef.h"

/*
功能：
	获得一个表域的缺省类型长度
输入参数：
	type	类型
输出参数：
	无
返回值：
	长度
*/
int UnionGetDefaultSizeOfTBLFieldType(int type);

/*
功能：
	获得一个对象定义所占内存大小
输入参数：
	无
输出参数：
	无
返回值：
	获得一个定义定义所占内存的大小
*/
int UnionGetSizeOfObjectDef();

// 功能：从指定行中读一个域组的定义
/*
输入参数：
	str		字符串
	lenOfStr	字符串长度
输出参数：
	pfldGrp		读出的域组定义，必须预先分配空间
返回值：
	成功：返回定义在字符串中占的字符数
	失败：返回错误码
*/
int UnionGetObjectFldGrpDefFromStr(char *str,int lenOfStr,PUnionObjectFldGrp pfldGrp);

// 功能：从指定字符串中读出域定义
/*
输入参数：
	fldStr		域定义串
	lenOfFldStr	域定义串长度
输出参数：
	pfldDef		读出的域定义，必须预先分配空间
返回值：
	成功：0
	失败：返回错误码
*/
int UnionGetObjectFldDefFromStr(char *fldStr,int lenOfFldStr,PUnionObjectFldDef pfldDef);

/*
功能：从对象创建文件获得一个对象的定义
输入参数：
	idOfObject：对象ID
输出参数：
	pobject：读出的对象定义，必须预先分配空间
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCreateObjectDefFromCreateSqlFile(TUnionIDOfObject idOfObject,PUnionObject pobject);

// 判断是否合法的域变量名
/*
输入参数：
	varName		域变量名
输出参数：
	无
返回值：
	1	合法变量名
	0	非法变量名
*/
int UnionIsValidObjectFldName(char *varName);

// 功能：检查域组中的域是否在对象中定义了
/*
输入参数：
	pfldGrp		要检查的域组
	pobject		在该对象中检查
输出参数：
	无
返回值：
	成功：>= 0
	失败：<0，错误码
*/
int UnionCheckFldGrpInObject(PUnionObjectFldGrp pfldGrp,PUnionObject pobject);

/*
功能：判断一个域值是否是另一个域值的子组
输入参数：
	pfldGrpChild：要检查的域组
	pfldGrpParent：在该域组中检查
输出参数：
	无
返回值：
	1：是
	0：不是
	<0：失败，返回错误码
*/
int UnionIsChildFldGrpOfFldGrp(PUnionObjectFldGrp pfldGrpChild,PUnionObjectFldGrp pfldGrpParent);

/*
功能：判断两个域组是否重复
输入参数：
	fldGrp1：域组1
	fldGrp2：域组2
输出参数：
	无
返回值：
	1：重复
	0：不重复
	<0：失败，返回错误码
*/
int UnionFldGrpIsRepeat(PUnionObjectFldGrp fldGrp1,PUnionObjectFldGrp fldGrp2);

/*
功能：判断两个域组是否完全一致，包括域名和域名的顺序判断
输入参数：
	fldGrp1：域组1
	fldGrp2：域组2
输出参数：
	无
返回值：
	1：重复
	0：不重复
	<0：失败，返回错误码
*/
int UnionFldGrpIsCompleteRepeat(PUnionObjectFldGrp fldGrp1,PUnionObjectFldGrp fldGrp2);

/*
功能：判断域定义是否合法
输入参数：
	fldDef：域定义
输出参数：
	无
返回值：
	1：合法
	0：不合法
	<0：失败，返回错误码
*/
int UnionIsValidObjectFieldDef(TUnionObjectFldDef fldDef);

// Mary add begin, 2008-9-5
/*
功能：将一个对象定义的串写入对象创建文件
输入参数：
	idOfObject：对象ID
	objSqlStr：对象定义串
	objSqlStrLen：对象定义串的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCreateObjectSqlFileByString(TUnionIDOfObject idOfObject,char *objSqlStr,int objSqlStrLen);
// Mary add end, 2008-9-5

/*
功能：将对象定义，写入到建表文件中
输入参数：
	pobject：指向对象定义的指针
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionCreateComplexDBTBLCreateSqlFile(PUnionObject pobject);

#endif

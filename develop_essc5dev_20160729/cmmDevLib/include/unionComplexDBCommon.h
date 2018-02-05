//	Author: ChenJiaMei
//	Date: 2008-8-1

#ifndef _unionComplexDBCommon_
#define _unionComplexDBCommon_

#include "unionComplexDBObjectDef.h"

#define SeparableSignOfFieldNameGroup		':'		// Mary add, 2008-9-9
#define SeparableSignOfFieldValueGroup		'.'		// Mary add, 2008-9-9
#define conSeparableSignOfFieldNameInFldList	','		// 王纯军增加，2009/4/21
#define conSeparableSignOfLineInMultiRow	'^'		// 王纯军增加，2009/5/2

// 对象域的值类型
typedef enum
{
	conObjectFldType_String = 1,	// 字符串
	conObjectFldType_Int = 2,	// 整型
	conObjectFldType_Double = 3,	// 有二个小数位的双精度数
	conObjectFldType_Bit = 4,	// 二进制
	conObjectFldType_Bool = 5,	// BOOL型
	conObjectFldType_Long = 6,	// 长整型
} TUnionObjectFldValueType;
typedef TUnionObjectFldValueType	*PUnionObjectFldValueType;

// 功能：将一个域类型转换为类型名称描述
/*
输入参数：
	type		域类型
输出参数：
	fldTypeName	域类型名称
返回值：
	成功：>=0
	失败：<0，错误码
*/
int UnionGetFldTypeName(TUnionObjectFldValueType type,char *fldTypeName);

/*
功能：去掉字符串中右边的多余字符
输入参数：
	Str：字符串
	lenOfStr：字符串的长度
	ch：要去掉的多余字符
输出参数：
	Str：去掉多余字符后的字符串
返回值：
	>=0：成功，返回新字符串的长度
	<0：失败，错误码
*/
int UnionFilterRightChar(char *Str,int lenOfStr,char ch);

/*
功能：去掉字符串中左边的多余字符
输入参数：
	Str：字符串
	lenOfStr：字符串的长度
	ch：要去掉的多余字符
输出参数：
	Str：去掉多余字符后的字符串
返回值：
	>=0：成功，返回新字符串的长度
	<0：失败，错误码
*/
int UnionFilterLeftChar(char *Str,int lenOfStr,char ch);

/*
功能：从"域1=域值|域2=域值|域3=域值|…"这种格式串中，拆分出域名组
输入参数：
	fieldNameStr：格式串，格式为"域1=域值|域2=域值|域3=域值|…"
	lenOfStr：格式串的长度
输出参数：
	fldGrp：域名组结构
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionFormFldGrpFromRecordStr(char *recordStr,int lenOfStr,PUnionObjectFldGrp fldGrp);

/*
功能：从"域名1.域名2.域名3…"这种格式串中，拆分出域名组
输入参数：
	fieldNameStr：格式串，格式为"域名1.域名2.域名3…"
	lenOfStr：格式串的长度
输出参数：
	fldGrp：域名组结构
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionFormFldGrpFromFieldNameStr(char *fieldNameStr,int lenOfStr,PUnionObjectFldGrp fldGrp);

/*
功能：从域名组结构拼成格式串"域名1.域名2.域名3…"
输入参数：
	fldGrp：域名组结构
输出参数：
	fieldNameStr：格式串，格式为"域名1.域名2.域名3…"
返回值：
	>=0：成功，返回格式串的长度
	<0：失败，错误码
*/
int UnionFormFieldNameStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *fieldNameStr);

/*
功能：从域名组结构和记录值拼成格式串"域值1.域值2.域值3…"
输入参数：
	fldGrp：域名组结构
	record：记录串，格式为"域1=域值|域2=域值|域3=域值|…"
	lenOfRecord：记录串的长度
输出参数：
	fieldValueStr：格式串，格式为"域值1.域值2.域值3…"
返回值：
	>=0：成功，返回格式串fieldValueStr的长度
	<0：失败，错误码
*/
int UnionFormFieldValueStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *record,int lenOfRecord,char *fieldValueStr);

/*
功能：从域名组结构和记录值拼成格式串"域1=域值|域2=域值|域3=域值|…"
输入参数：
	fldGrp：域名组结构
	record：记录串，格式为"域1=域值|域2=域值|域3=域值|…"
	lenOfRecord：记录串的长度
	sizeOfRecStr：fieldRecStr的存储空间大小
输出参数：
	fieldRecStr：格式串，格式为"域1=域值|域2=域值|域3=域值|…"
返回值：
	>=0：成功，返回格式串fieldRecStr的长度
	<0：失败，错误码
*/
int UnionFormFieldRecordStrFromFldGrp(PUnionObjectFldGrp fldGrp,char *record,int lenOfRecord,char *fieldRecStr,int sizeOfRecStr);

/*
功能：从"域1=域值|域2=域值|…"这种格式串拼成"域值1.域值2.域值3…"的格式串
输入参数：
	commonStr：域格式串，格式为"域1=域值|域2=域值|…"
	lenOfStr：域格式串的长度
输出参数：
	fieldValueStr：域值格式串，格式为"域值1.域值2.域值3…"
返回值：
	>=0：成功，返回域值格式串的长度
	<0：失败，错误码
*/
int UnionFormValueStrFromCommonStr(char *commonStr,int lenOfStr,char *fieldValueStr);

/*
功能：判断域的赋值是否合法
输入参数：
	fieldDef：域定义
	fieldValue：域值
	lenOfValue：域值的长度
输出参数：
	无
返回值：
	1：合法
	0：不合法
	<0：失败，返回错误码
*/
int UnionIsValidObjectFieldValue(TUnionObjectFldDef fieldDef,char *fieldValue,int lenOfValue);

/*
功能：从实例的存储位置得到实例的关键字
输入参数：
	storePosition：实例的存储位置
	lenOfPosition：storePosition的长度
输出参数：
	primaryKey：实例的关键字
返回值：
	>=0：成功，返回关键字的长度
	<0：失败，返回错误码
*/
int UnionGetRecordPrimaryKeyFromStorePosition(char *storePosition,int lenOfPosition,char *primaryKey);
#endif

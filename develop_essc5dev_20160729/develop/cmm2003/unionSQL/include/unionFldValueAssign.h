//---------------------------------------------------------------------------

#ifndef unionFldValueAssignH
#define unionFldValueAssignH
//---------------------------------------------------------------------------

#define conFldValueSetMethodTagAutoSetBySoftware        "auto"       // 系统自动赋值
#define conFldValueSetMethodTagUserInput                "null"       // 用户交互输入
#define conFldValueSetMethodDefaultSet                  "default::"  // 缺省值
#define conFldValueSetMethodRefrenceOtherTBLFld         "query::"    // 引用其它表域
#define conFldValueSetMethodEnumGrp                     "enum::"     // 枚举值

#include "unionFldGrp.h"

#define 	conMaxNumOfFldPerTBL		64

// 定义一个域的赋值方法组
typedef struct
{
	int		fldNum;
	TUnionFldName	fldName[conMaxNumOfFldPerTBL];			// 域名
	TUnionFldName	fldAlais[conMaxNumOfFldPerTBL];			// 域别名
	char		fldValue[conMaxNumOfFldPerTBL][128+1];		// 域赋值方法
} TUnionFldValueAssignMethodGrp;
typedef TUnionFldValueAssignMethodGrp	*PUnionFldValueAssignMethodGrp;

/*
功能	从文件中读取域赋值方法定义
输入参数
	fileName	文件名
输出参数
	prec		域赋值方法定义
返回值
	>=0		域赋值方法的数目
	<0		出错代码
*/
int UnionReadFldValueAssignMethodGrpDef(char *fileName,PUnionFldValueAssignMethodGrp prec);

/*
功能	读指定表的指定域的赋值方法
输入参数
	tblName         表别名
        isFldAlaisUsed  根据域名还是域别名来读  1根据域别名，0根据域名
        fldTag          域名或域别名
        sizeOfBuf       缓冲大小
输出参数
	method		读出的赋值方法
返回值
	>=0		域赋值方法的长度
	<0		出错代码
*/
int UnionReadFldValueAssignMethodOfSpecFldOfSpecTBL(char *tblName,int isFldAlaisUsed,char *fldTag,char *method,int sizeOfBuf);


//---------------------------------------------------------------------------
/*
功能	根据预定义的查询定义串获得表域值
输入参数
	conditionStr            查询定义串
        lenOfConditionStr       查询定义串长度
        maxValueNum             最多可读出值数目
输出参数
	varGrp		读出的值
返回值
	>=0		读出的值数目
	<0		出错代码
*/
int UnionGetAvailableValueGrpByQueryCondition(char *conditionStr,int lenOfConditionStr,char varGrp[][128+1],int maxValueNum);

//---------------------------------------------------------------------------
/*
功能	获得某表的某域的取值表
输入参数
	tblName		表名
        fldAlais        域别名
        maxValueNum     最多可读出的值数目
输出参数
	varGrp		读出的值
返回值
	>=0		读出的值数目
	<0		出错代码
*/
int UnionGetAvailableValueGrp(char *tblName,char *fldAlais,char varGrp[][128+1],int maxValueNum);

//---------------------------------------------------------------------------
/*
功能	根据域赋值方法将域值转换为显示方式
输入参数
	method          赋值方法
        value           原值
        lenOfValue      原值长度
        sizeOfBuf       接收缓冲大小
输出参数
	displayValue    显示值
返回值
	>=0		显示值长度
	<0		出错代码
*/
int UnionConvertFldValueIntoDisplayFormat(char *method,char *value,int lenOfValue,char *displayValue,int sizeOfBuf);

//---------------------------------------------------------------------------
/*
功能	根据域赋值方法将域值由显示方式转换为实际值
输入参数
	method          赋值方法
	displayValue    显示值
        lenOfDisplayValue      原值长度
        sizeOfBuf       接收缓冲大小
输出参数
        value           原值
返回值
	>=0		显示值长度
	<0		出错代码
*/
int UnionConvertFldValueFromDisplayFormat(char *method,char *displayValue,int lenOfDisplayValue,char *value,int sizeOfBuf);

#endif

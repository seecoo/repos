//---------------------------------------------------------------------------

#ifndef unionEnumValueDefH
#define unionEnumValueDefH
//---------------------------------------------------------------------------
typedef struct
{
        char    remark[40+1];   // 值说明
        char    value[48+1];    // 值
} TUnionEnumValueDef;
typedef TUnionEnumValueDef      *PUnionEnumValueDef;


#define conMaxNumOfEnumValuePerGrp      64
typedef struct
{
        int                     valueNum;
        TUnionEnumValueDef      valueDef[conMaxNumOfEnumValuePerGrp];
} TUnionEnumValueGrp;
typedef TUnionEnumValueGrp      *PUnionEnumValueGrp;

//---------------------------------------------------------------------------
/*
功能	从文件中读取枚举类型定义
输入参数
	enumType	枚举类型
输出参数
	prec		枚举类型定义
返回值
	>=0		枚举类型的值数目
	<0		出错代码
*/
int UnionReadEnumValueGrpDef(char *enumType,PUnionEnumValueGrp prec);

//---------------------------------------------------------------------------
/*
功能	读指定枚举类型的指定说明或值对应的值或类型
输入参数
	enumType         枚举类型名
        isReadValueByRemark  根据说明还是值来读  1根据说明，0根据值
        byTag           说明或值
输出参数
	readTag		读出的值或说明
返回值
	>=0		读出的长度
	<0		出错代码
*/
int UnionReadEnumValueOtherAttr(char *enumType,int isReadValueByRemark,char *byTag,char *readTag,int sizeOfBuf);

//---------------------------------------------------------------------------
/*
功能	读取枚举类型的所有值
输入参数
	enumType	枚举类型
输出参数
	prec		枚举类型定义
返回值
	>=0		枚举类型的值数目
	<0		出错代码
*/
int UnionReadAllValueOfEnumType(char *enumType,char value[][128+1],int maxNum);

//---------------------------------------------------------------------------
/*
功能	读取枚举类型的所有说明
输入参数
	enumType	枚举类型
输出参数
	prec		枚举类型定义
返回值
	>=0		枚举类型的说明数目
	<0		出错代码
*/
int UnionReadAllRemarkOfEnumType(char *enumType,char remark[][128+1],int maxNum);
#endif

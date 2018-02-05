//---------------------------------------------------------------------------

#ifndef unionTBLRecInterfaceH
#define unionTBLRecInterfaceH
//---------------------------------------------------------------------------
#include "unionWorkingDir.h"
#include "unionFldGrp.h"

#include "unionFldValueAssign.h"

// 定义一个表交互界面记录
typedef struct
{
	char			name[40+1];			// 表名称，表名称是唯一值，
	char			alais[40+1];			// 表别名，这个值用于显示在表界面的标题中
	int			id;				// 表标识，是表的唯一标识，这个值与平台定义的表的标识对应

	char			recIconFileName[40+1];		// 记录图标文件的名称
	TUnionFldValueAssignMethodGrp	fldAssignGrp;			// 赋的赋值方法定义
	TUnionFldGrp		primaryKeyFldGrp;		// 关键字域组
} TUnionTBLRecInterface;
typedef TUnionTBLRecInterface	*PUnionTBLRecInterface;

/*
功能	获得主表的配置文件的名称
输入参数
	tblAlais	表别名
输出参数
	fileName	配置文件名称
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTBLRecInterfaceFileName(char *tblAlais,char *fileName);
/*
功能	获得表配置记录
输入参数
	tblName		表名
输出参数
	prec		表配置记录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionReadTBLRecInterface(char *tblName,PUnionTBLRecInterface prec);

/*
功能	获得域的别名
输入参数
	prec            记录赋值方法
        fldName         域名
输出参数
	fldAlais        域别名
返回值
	>=0		正确
	<0		出错代码
*/
int UnionReadFldAlaisFromTBLRecInterface(PUnionTBLRecInterface prec,char *fldName,char *fldAlais);

/*
功能	判断域是否是关键字域
输入参数
	prec            记录赋值方法
        fldName         域名
输出参数
	无
返回值
	1		是关键字域
	0		不是关键字域
*/
int UnionFldOfTBLRecInterfaceIsPrimaryKeyFld(PUnionTBLRecInterface prec,char *fldName);

#endif


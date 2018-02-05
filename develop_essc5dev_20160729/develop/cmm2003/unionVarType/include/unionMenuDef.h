// Automatically generated codes
// Generated Date and Time 20091120173526
// Generated by ud090401

#ifndef _unionMenuDef_
#define _unionMenuDef_


// 定义表的域名常量
#define conMenuDefFldNameMainMenuName		"mainMenuName"
#define conMenuDefFldNameMenuGrpName		"menuGrpName"
#define conMenuDefFldNameDisplayName		"displayName"
#define conMenuDefFldNameTableMenuType		"tableMenuType"
#define conMenuDefFldNameMenuItemsList		"menuItemsList"
#define conMenuDefFldNameVisualLevelListOfMenu		"visualLevelListOfMenu"
#define conMenuDefFldNameRemark		"remark"
#define conMenuDefFldNameInputTeller		"inputTeller"
#define conMenuDefFldNameInputDate		"inputDate"
#define conMenuDefFldNameInputTime		"inputTime"
#define conMenuDefFldNameUpdateTeller		"updateTeller"
#define conMenuDefFldNameUpdateDate		"updateDate"
#define conMenuDefFldNameUpdateTime		"updateTime"

/*
*** 结构对应的表信息 ***
表名::          menuDef
表名宏定义::    conTBLNameMenuDef
表资源宏定义::  conResIDMenuDef
关键字::        mainMenuName,menuGrpName
*/

// 定义表记录对应的结构
typedef struct
{
	char	mainMenuName[128+1];	// 主菜单名称
	char	menuGrpName[128+1];	// 菜单组名
	char	displayName[128+1];	// 显示名称
	int	tableMenuType;	// 表菜单类型
	char	menuItemsList[1024+1];	// 菜单项清单
	char	visualLevelListOfMenu[1024+1];	// 菜单可视级别清单
	char	remark[128+1];	// 128个字符的说明
	char	inputTeller[20+1];	// 记录插入数据库时，系统自动附加的操作员标识
	char	inputDate[8+1];	// 记录插入数据库时，系统自动附加的日期
	char	inputTime[6+1];	// 记录插入数据库时，系统自动附加的时间
	char	updateTeller[20+1];	// 记录修改时，系统自动附加的操作员标识
	char	updateDate[8+1];	// 记录修改时，系统自动附加的日期
	char	updateTime[6+1];	// 记录修改时，系统自动附加的时间
} TUnionMenuDef;
typedef TUnionMenuDef	*PUnionMenuDef;

//***** 以下定义函数声明 *****


/*
功能：
	结构的缺省赋值函数
输入参数:
	prec	记录指针
输出参数:
	无
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormDefaultValueOfMenuDefRec(PUnionMenuDef prec);


/*
功能：
	将记录结构转换为记录字串的函数
输入参数:
	prec	记录指针
	sizeOfBuf	接收记录字串的缓冲的大小
输出参数:
	recStr	记录字串
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormRecStrFromMenuDefRec(PUnionMenuDef prec,char *recStr,int sizeOfBuf);


/*
功能：
	将记录结构转换为记录字串的函数
输入参数:
	prec	记录指针
	sizeOfBuf	接收记录字串的缓冲的大小
输出参数:
	recStr	记录字串
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormPrimaryKeyRecStrFromMenuDefRec(PUnionMenuDef prec,char *recStr,int sizeOfBuf);


/*
功能：
	将记录字串转换为记录结构的关键字的函数
输入参数:
	recStr	记录字串
	lenOfRecStr	记录串长度
输出参数:
	prec	记录指针
返回值
	>=0		读出的域数目
	<0		失败,错误码
*/
int UnionFormPrimaryKeyOfMenuDefRecFromRecStr(char *recStr,int lenOfRecStr,PUnionMenuDef prec);


/*
功能：
	将记录结构转换为记录字串的函数
输入参数:
	prec	记录指针
	sizeOfBuf	接收记录字串的缓冲的大小
输出参数:
	recStr	记录字串
返回值
	>=0		成功,拼装的记录串的长度
	<0		失败,错误码
*/
int UnionFormNonPrimaryKeyRecStrFromMenuDefRec(PUnionMenuDef prec,char *recStr,int sizeOfBuf);


/*
功能：
	将记录结构转换为记录字串的函数
输入参数:
	recStr	记录字串
	lenOfRecStr	记录串长度
输出参数:
	prec	记录指针
返回值
	>=0		读出的域数目
	<0		失败,错误码
*/
int UnionFormMenuDefRecFromRecStr(char *recStr,int lenOfRecStr,PUnionMenuDef prec);


/*
功能：
	插入一个记录
输入参数:
	prec	记录指针
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionInsertMenuDefRec(PUnionMenuDef prec);


/*
功能：
	删除一个记录
输入参数:
	mainMenuName 主菜单名称
	menuGrpName 菜单组名
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionDeleteMenuDefRec(char *mainMenuName,char *menuGrpName);
/*
函数功能:
	修改指定的表域:
输入参数:
	mainMenuName 主菜单名称
	menuGrpName 菜单组名
	fldName 域名称
	fldValue 域值
	fldValueLen 域值长度
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionUpdateSpecFldOfMenuDefRec(char *mainMenuName,char *menuGrpName,char *fldName,char *fldValue,int lenOfFldValue);
/*
函数功能:
	修改指定的表域:
输入参数:
	mainMenuName 主菜单名称
	menuGrpName 菜单组名
	fldName 域名称
	fldValue 域值
输出参数:
	无
返回值:
	>=0 成功
	<0 失败
*/
int UnionUpdateIntTypeSpecFldOfMenuDefRec(char *mainMenuName,char *menuGrpName,char *fldName,int fldValue);


/*
功能：
	修改一个记录
输入参数:
	prec	记录指针
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionUpdateMenuDefRec(PUnionMenuDef prec);


/*
功能：
	读取一个记录
输入参数:
	mainMenuName 主菜单名称
	menuGrpName 菜单组名
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionReadMenuDefRec(char *mainMenuName,char *menuGrpName,PUnionMenuDef prec);


/*
功能：
	读取一个记录
输入参数:
	mainMenuName 主菜单名称
	menuGrpName 菜单组名
	fldName		域名
	sizeOfBuf	域值缓冲大小
输出参数:
	fldValue	域值
返回值
	>=0		成功，返回域值的长度
	<0		失败,错误码
*/
int UnionReadMenuDefRecFld(char *mainMenuName,char *menuGrpName,char *fldName,char *fldValue,int sizeOfBuf);


/*
功能：
	读取一个记录
输入参数:
	mainMenuName 主菜单名称
	menuGrpName 菜单组名
	fldName		域名
输出参数:
	fldValue	域值
返回值
	>=0		成功，返回域值的长度
	<0		失败,错误码
*/
int UnionReadMenuDefRecIntTypeFld(char *mainMenuName,char *menuGrpName,char *fldName,int *fldValue);


/*
功能：
	根据条件，批量读取数据
输入参数:
	condition	条件字串
	maxRecNum		读取的最大记录数量
输出参数:
	recGrp		记录数组
返回值
	>=0		成功,读取的记录数量
	<0		失败,错误码
*/
int UnionBatchReadMenuDefRec(char *condition,TUnionMenuDef recGrp[],int maxRecNum);

//***** 函数声明结束 *****

#endif

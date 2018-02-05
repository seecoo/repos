// Automatically generated codes
// Generated Date and Time 20090503125417
// Generated by ud090401

#ifndef _unionTableList_
#define _unionTableList_


// 定义表的域名常量
#define conTableListFldNameTableName		"tableName"
#define conTableListFldNameTableTypeID		"tableTypeID"
#define conTableListFldNameTableAlais		"tableAlais"
#define conTableListFldNameResID		"resID"
#define conTableListFldNameRemark		"remark"
#define conTableListFldNameFieldList		"fieldList"
#define conTableListFldNamePrimaryKey		"primaryKey"
#define conTableListFldNameUnique1		"unique1"
#define conTableListFldNameUnique2		"unique2"
#define conTableListFldNameUnique3		"unique3"
#define conTableListFldNameUnique4		"unique4"
#define conTableListFldNameMyKeyGrp1		"myKeyGrp1"
#define conTableListFldNameForeignTable1		"foreignTable1"
#define conTableListFldNameForeignKey1		"foreignKey1"
#define conTableListFldNameMyKeyGrp2		"myKeyGrp2"
#define conTableListFldNameForeignTable2		"foreignTable2"
#define conTableListFldNameForeignKey2		"foreignKey2"
#define conTableListFldNameMyKeyGrp3		"myKeyGrp3"
#define conTableListFldNameForeignTable3		"foreignTable3"
#define conTableListFldNameForeignKey3		"foreignKey3"
#define conTableListFldNameMyKeyGrp4		"myKeyGrp4"
#define conTableListFldNameForeignTable4		"foreignTable4"
#define conTableListFldNameForeignKey4		"foreignKey4"
#define conTableListFldNameOperationList		"operationList"
#define conTableListFldNameInitValueList		"initValueList"
#define conTableListFldNameInputTeller		"inputTeller"
#define conTableListFldNameInputDate		"inputDate"
#define conTableListFldNameInputTime		"inputTime"
#define conTableListFldNameUpdateTeller		"updateTeller"
#define conTableListFldNameUpdateDate		"updateDate"
#define conTableListFldNameUpdateTime		"updateTime"


// 定义表记录对应的结构
typedef struct
{
	char	tableName[40+1];		// 表名称
	char	tableTypeID[50+1];	// 表所属类型
	char	tableAlais[40+1];	// 表别名
	int	resID;	// 资源ID号
	char	remark[128+1];
	char	fieldList[8192+1];	// 域清单
	char	primaryKey[1024+1];	// 关键字域
	char	unique1[1024+1];	// 唯一值域1
	char	unique2[1024+1];	// 唯一值域2
	char	unique3[1024+1];	// 唯一值域3
	char	unique4[1024+1];	// 唯一值域4
	char	myKeyGrp1[1024+1];	// 外键域1
	char	foreignTable1[40+1];	// 外表1
	char	foreignKey1[1024+1];	// 外键域1
	char	myKeyGrp2[1024+1];	// 外键域2
	char	foreignTable2[40+1];	// 外表2
	char	foreignKey2[1024+1];	// 外键域2
	char	myKeyGrp3[1024+1];	// 外键域3
	char	foreignTable3[40+1];	// 外表3
	char	foreignKey3[1024+1];	// 外键域3
	char	myKeyGrp4[1024+1];	// 外键域4
	char	foreignTable4[40+1];	// 外表4
	char	foreignKey4[1024+1];	// 外键域4
	char	operationList[1024+1];	// 支持的操作清单
	char	formMenuItemList[8192+1];	// 界面菜单项清单
	char	recMenuItemList[8192+1];	// 记录菜单项清单
	char	doubleClickMenuItemList[8192+1];	// 双击记录菜单项清单
	char	initValueList[8192+1];	// 初始记录清单
	char	inputTeller[20+1];
	char	inputDate[8+1];
	char	inputTime[6+1];
	char	updateTeller[20+1];
	char	updateDate[8+1];
	char	updateTime[6+1];
} TUnionTableList;
typedef TUnionTableList	*PUnionTableList;

//***** 以下定义函数声明 *****

/*
功能：
	读取一个记录域
输入参数:
	resID		资源ID
	fldName		域名称
	sizeOfFldValue	取值域的大小
输出参数:
	fldValue	读到的域值
返回值
	>=0		成功，返回域值的长度
	<0		失败,错误码
*/
int UnionReadTableListRecFldByResID(int resID,char *fldName,char *fldValue,int sizeOfFldValue);

/*
功能：
	读取一个记录域
输入参数:
	tableName	表名称
	fldName		域名称
	sizeOfFldValue	取值域的大小
输出参数:
	fldValue	读到的域值
返回值
	>=0		成功，返回域值的长度
	<0		失败,错误码
*/
int UnionReadTableListRecFld(char *tableName,char *fldName,char *fldValue,int sizeOfFldValue);

/*
功能：
	读取一个记录
输入参数:
	resID	资源ID
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionReadTableListRecByResID(int resID,PUnionTableList prec);

/*
功能：
	根据表名称读取表的版本
输入参数:
	tableName	表名称
	sizeOfBuf	接收缓冲的大小
输出参数:
	version
返回值
	>=0		成功，返回版本的长度
	<0		失败,错误码
*/
int UnionReadTableListRecVersionOfSpecTBLName(char *tableName,char *version,int sizeOfBuf);

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
int UnionFormDefaultValueOfTableListRec(PUnionTableList prec);


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
int UnionFormRecStrFromTableListRec(PUnionTableList prec,char *recStr,int sizeOfBuf);


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
int UnionFormTableListRecFromRecStr(char *recStr,int lenOfRecStr,PUnionTableList prec);


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
int UnionInsertTableListRec(PUnionTableList prec);


/*
功能：
	删除一个记录
输入参数:
	prec	记录指针
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionDeleteTableListRec(PUnionTableList prec);


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
int UnionUpdateTableListRec(PUnionTableList prec);


/*
功能：
	读取一个记录
输入参数:
	prec	记录指针，必须对关键字域赋值
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionReadTableListRec(PUnionTableList prec);

/*
功能：
	根据表名称读取一个记录
输入参数:
	tableName	表名称
输出参数:
	prec	记录指针
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionReadTableListRecOfSpecTBLName(char *tableName,PUnionTableList prec);

/*
功能：
	根据表名称读取一个表的RESID
输入参数:
	tableName	表名称
输出参数:
	无
返回值
	>=0		成功，返回记录的大小
	<0		失败,错误码
*/
int UnionReadResIDOfSpecTBLName(char *tableName);

/*
功能：
	根据表名称读取表的别名
输入参数:
	tableName	表名称
输出参数:
	tableAlais	表别名
返回值
	>=0		成功
	<0		失败,错误码
*/
int UnionReadTableAlaisOfSpecTBLName(char *tableName,char *tableAlais);



/*
功能：
	将一个操作，增加到表的菜单项中
输入参数:
	tableName	表名称
	menuItemName	菜单项名称
	menuType	菜单项类型
输出参数:
	无
返回值
	>=0		成功
	<0		失败,错误码
*/
int UnionAppendMenuItemToSpecTBLName(char *tableName,char *menuItemName,int menuType);


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
int UnionFormPrimaryKeyRecStrFromTableListRec(PUnionTableList prec,char *recStr,int sizeOfBuf);

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
int UnionFormNonePrimaryKeyRecStrFromTableListRec(PUnionTableList prec,char *recStr,int sizeOfBuf);
//***** 函数声明结束 *****

#endif

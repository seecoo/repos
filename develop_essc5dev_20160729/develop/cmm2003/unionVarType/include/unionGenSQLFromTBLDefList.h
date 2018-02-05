//	Wolfgang Wang
//	2009/4/29

#ifndef _unionGenSQLFromTBLDefList_
#define _unionGenSQLFromTBLDefList_

#define	conMaxNumOfChildrenTBL	32
#define	conMaxNumOfParentsTBL	32

typedef struct TUnionTBLDef
{
	int	created;	// 1,表示已建立，0表示未建立
	int	storeWay;	// 存储方式
	char	name[40+1];	// 表名
	int	childrenNum;	// 子表数目
	struct	TUnionTBLDef *children[conMaxNumOfChildrenTBL];	// 子表
	int	parentNum;	// 父表数目
	struct	TUnionTBLDef *parents[conMaxNumOfParentsTBL];	// 父表
} TUnionTBLDef;
typedef TUnionTBLDef	*PUnionTBLDef;


#define conMaxNumOfTBLDefPerGrp	1024
typedef struct TUnionTBLDefGrp
{
	int		num;
	int		relationsNum;
	int		dbTBLNum;
	PUnionTBLDef	ptblDefGrp[conMaxNumOfTBLDefPerGrp];
} TUnionTBLDefGrp;
typedef TUnionTBLDefGrp	*PUnionTBLDefGrp;

/*
功能	
	初始化表定义组
输入参数
	ptblDefGrp	表定义组指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionResetTBLDefGrp(PUnionTBLDefGrp ptblDefGrp);

/*
功能	
	释放表定义数组
输入参数
	ptblDefGrp	表定义组指针
输出参数
	无
返回值
	无
*/
void UnionFreeTBLDefGrp(PUnionTBLDefGrp ptblDefGrp);

/*
功能	
	初始化表定义
输入参数
	name	表名称
输出参数
	无
返回值
	成功	表指针
	NULL	空指针
*/
PUnionTBLDef UnionInitTBLDefFromDefFile(char *name);

/*
功能	
	从表定义清单初始化表定义组
输入参数
	ptblDefGrp	表定义组指针
输出参数
	无
返回值
	成功	返回指针
	失败	NULL
*/
PUnionTBLDefGrp UnionInitTBLDefGrpFromTBLDefTBL();

/*
功能	
	从表定义清单初始化表关系
输入参数
	ptblDefGrp	表定义组指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionInitAllTBLRelationsFromTBLDefList(PUnionTBLDefGrp ptblDefGrp);

/*
功能	
	读一个表的所有外表定义
输入参数
	name	表名
	maxNum	外表名数组可以缓冲的最大外表名数目
输出参数
	foreignTBLNameGrp	外表名数组
返回值
	>=0	成功，外表数目
	<0	错误码
*/
int UnionReadAllForeignTBLNames(char *name,char foreignTBLNameGrp[][40+1],int maxNum);

/*
功能	
	将一个表定义登记到父定义中
输入参数
	ptblDef		表定义
	ptblDefGrp	表定义组指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionRegisterSpecTBLToAllParents(PUnionTBLDef ptblDef,PUnionTBLDefGrp ptblDefGrp);

/*
功能	
	将一个表定义登记到父定义中
输入参数
	ptblDefGrp	表定义组指针
输出参数
	无
返回值
	成功	指针
	失改	NULL
*/
PUnionTBLDef UnionFindSpecTBLDefInTBLDefGrp(PUnionTBLDefGrp ptblDefGrp,char *name);

/*
功能	
	打印一个表的SQL语句
输入参数
	ptblDef	表定义组指针
	fp	文件指针
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateSQLForSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp,char *mainMenuName);

/*
功能	
	打印所有表的SQL语句
输入参数
	fp		文件指针
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenu(FILE *fp,char *mainMenuName);

/*
功能	
	打印所有表的SQL语句
输入参数
	fileName	文件名
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenuToSpecFile(char *fileName,char *mainMenuName);

/*
功能	
	判断一个表的父表是否已全部创建
输入参数
	ptblDef	表定义组指针
	fp	文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionIsAllParentsOfSpecTBLDefCreated(PUnionTBLDef ptblDef);

/*
功能	
	判断一个表的子表是否已全部创建
输入参数
	ptblDef	表定义组指针
	fp	文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionIsAllChildrenOfSpecTBLDefCreated(PUnionTBLDef ptblDef);

/*
功能	
	打印所有表的SQL语句
输入参数
	ptblDefGrp	表定义组指针
	fp		文件指针
	mainMenuName	主菜单名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateSQLForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,FILE *fp,char *mainMenuName);


/*
功能	
	打印所有表的SQL语句
输入参数
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateSQLForAllTBLDefInTableList(FILE *fp);

/*
功能	
	打印所有表的SQL语句
输入参数
	fileName	文件名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateSQLForAllTBLDefInTableListToSpecFile(char *fileName);


/*
功能	
	打印一个表名称
输入参数
	ptblDef	表定义组指针
	prefix		表名的前缀
	fp	文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintTBLNameForSpecTBLDef(PUnionTBLDef ptblDef,char *prefix,FILE *fp);

/*
功能	
	打印所有表名称
输入参数
	prefix		表名的前缀
	ptblDefGrp	表定义组指针
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintTBLNameForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,char *prefix,FILE *fp);

/*
功能	
	打印所有表名称
输入参数
	prefix		表名的前缀
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintTBLNameForAllTBLDefInTableList(char *prefix,FILE *fp);

/*
功能	
	打印所有表名称
输入参数
	prefix		表名的前缀
	fileName	文件名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintTBLNameForAllTBLDefInTableListToSpecFile(char *prefix,char *fileName);

/*
功能	
	导出所有表的数据
输入参数
	dir		导出的文件目录
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionOutputAllRecForAllTBLDef(char *dir);

/*
功能	
	显示所有表关系
输入参数
	ptblDefGrp	表定义组指针
	displayParents	1,显示父表，0,显示子表，2显示所有关系
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintRelationsForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,int displayParents,FILE *fp);

/*
功能	
	显示所有表关系
输入参数
	displayParents	1,显示父表，0,显示子表，2显示所有关系
	fileName	文件名称
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintRelationsForAllTBLDefToSpecFile(int displayParents,char *fileName);

/*
功能	
	显示表的子表
输入参数
	ptblDef		表指针
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintChildrenOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp);

/*
功能	
	显示表的父表
输入参数
	ptblDef		表指针
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintParentsOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp);

/*
功能	
	显示表的关系
输入参数
	ptblDef		表指针
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintRelationsOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp);

/*
功能	
	显示指定表的所有关系
输入参数
	ptblDefGrp	表定义组指针
	tableName	表名
	displayParents	1,显示父表，0,显示子表，2显示所有关系
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintRelationsForSpecTBLDef(PUnionTBLDefGrp ptblDefGrp,char *tableName,int displayParents,FILE *fp);

/*
功能	
	显示指定表的所有关系
输入参数
	tableName	表名
	displayParents	1,显示父表，0,显示子表，2显示所有关系
	fileName	文件名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionPrintRelationsForSpecTBLDefToSpecFile(char *tableName,int displayParents,char *fileName);

/*
功能	
	打印一个表的删除表的SQL语句
输入参数
	ptblDef	表定义组指针
	fp	文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateDropTBLSQLForSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp);

/*
功能	
	打印所有表的删除表的SQL语句
输入参数
	ptblDefGrp	表定义组指针
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateDropTBLSQLForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,FILE *fp);

/*
功能	
	打印所有表的删除表的SQL语句
输入参数
	fp		文件指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateDropTBLSQLForAllTBLDefInTableList(FILE *fp);

/*
功能	
	打印所有表的删除表的SQL语句
输入参数
	fileName	文件名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionCreateDropTBLSQLForAllTBLDefInTableListToSpecFile(char *fileName);

#endif

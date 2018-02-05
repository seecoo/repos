//	zhang yong ding
//	2012/12/07
//	Version 1.0

#ifndef _UnionTableDef_
#define _UnionTableDef_

#define conMDLNameOfTableDefTBL		"UnionTableDefMDL"
#define conMaxNumOfTableFieldDef	64
#define conMaxNumOfTableDef		128
#define conMaxFieldNumOfKeyGroup	10

typedef struct
{
	char				fieldName[40+1];			// 字段名
	char				fieldChnName[40+1];			// 字段中文名
	int				fieldType;				// 字段类型
	float				fieldSize;				// 字段大小
	char				defaultValue[128+1];			// 缺省值
	int				isPrimaryKey;				// 1：是，0：否
	int				isNull;					// 1：是，0：否
} TUnionTableFieldDef;
typedef TUnionTableFieldDef		*PUnionTableFieldDef;

typedef struct
{
	char				tableName[40+1];			// 表名
	char				tableChnName[40+1];			// 表中文名
	char				tableType[20+1];			// 表类型
	int				methodOfCached;				// 缓存方法,0：标准，1：个性化
	int				fieldNum;				// 字段数目
	TUnionTableFieldDef		fieldDef[conMaxNumOfTableFieldDef];	// 字段定义
} TUnionTableDef;
typedef TUnionTableDef			*PUnionTableDef;

typedef struct
{
	int				tableNum;				// 实际数量
	TUnionTableDef			rec[conMaxNumOfTableDef];		// 表的值
} TUnionTableDefTBL;
typedef TUnionTableDefTBL		*PUnionTableDefTBL;

typedef struct
{
	char				tableName[40+1];			// 表名
	int				dbIndex;				// 数据库索引
}TUnionDBIndex;
typedef TUnionDBIndex			*PUnionDBIndex;

typedef struct
{
	int				tableNum;				// 实际数量
	TUnionDBIndex			rec[conMaxNumOfTableDef];		// 表的值
} TUnionDBIndexTBL;
typedef TUnionDBIndexTBL		*PUnionDBIndexTBL;

int UnionGetFileDirOfTableDef(char *fileName);

int UnionGetFileNameOfTableDef(char *tableName,char *fileName);

int UnionGetTableNameFromFileName(char *fileName,char *tableName);

int UnionIsTableDefTBLConnected();

int UnionGetMaxNumOfTableDef();

int UnionConnectTableDefTBL();

int UnionDisconnectTableDefTBL();

int UnionRemoveTableDefTBL();

int UnionReloadTableDefTBL();

int UnionPrintTableDefTBLToFile(FILE *fp);

int UnionPrintTableDefToFile(PUnionTableDef pgunionTableDef,FILE *fp);

int UnionPrintTableFieldDefToFile(PUnionTableFieldDef pgunionTableFieldDef,FILE *fp);

int UnionPrintAllTableDefOfTableNameToFile(char *tableName,FILE *fp);

PUnionTableDef UnionFindTableDef(char *tableName);

PUnionTableDefTBL UnionGetTableDefTBL();

int UnionPrintRecOfTableDefToFile(char *tableName,FILE *fp);

int UnionTransferDBFieldNameToRealFieldName(char *tableName,char *dbFieldName,char *realFileName);

// 获取所有表字段清单
int UnionReadTableFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList);

// 获取所有真实表字段清单
int UnionReadTableRealFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList);

// 获取所有虚拟表字段清单
int UnionReadTableVirtualFieldListOfTableName(char *tableName,int tableFlag,char *tableFieldList);

// 判断是否是虚拟字段
int UnionIsVirtualFieldOfTableName(char *tableName,char *fieldName);

// 判断是否是主键
int UnionIsPrimaryKeyFieldOfTableName(char *tableName,char *fieldName);

// 判断字段是否存在
int UnionExistFieldOfTableName(char *tableName,char *fieldName);

// 根据表名读取缓存时长
int UnionReadMethodOfCachedOfTableName(char *tableName);

// 生成建表语句
int UnionGetCreateSQLByTableName(char *tableName,char *dbType,char *buf,int sizeOfBuf);

// 生成数据库脚本
int UnionGetSQLScript(char *tableName,char *database, char *flag, char *buf,int sizeOfBuf);

int UnionReadTablePrimaryKeyGrpFromImage(char *tableName, char primaryKeyGrp[][64]);

/* 
功能：	辨别字段是否为真
参数：	tableName[in]		表名
	fieldName[in]		字段名
返回值：1			是
	0			否
*/
int UnionCheckFieldNameIsTrue(char *tableName,char *fieldName);

/* 
功能：	转换表字段，过滤不显示的字段
参数：	useFlag[in]		用途，1：显示，2：菜单管理
	tableList[in]		表名
	fieldList[in|out]	字段名
返回值：1			是
	0			否
*/
int UnionTransformTableFileList(int useFlag,char *tableList,char *fieldList);

// 根据表名获取数据库索引位置
int UnionGetDBIndexByTableName(char *tableName);

#endif

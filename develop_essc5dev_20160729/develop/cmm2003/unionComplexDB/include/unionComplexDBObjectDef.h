//	Author: Wolfgang Wang
//	Date: 2008/3/6

#ifndef _unionComplexDBObjectDef_
#define _unionComplexDBObjectDef_

// 对象域名字
typedef char		TUnionObjectFldName[40+1];
// 对象名称
typedef char		TUnionObjectName[40+1];
// 对象域的缺省值
typedef char		TUnionObjectFldDefaultValue[48+1];
typedef char		TUnionIDOfObject[32+1];

#ifndef _boolDefinedAlready_
#define _boolDefinedAlready_
#ifndef _WIN32_
typedef int                     bool;
#endif
#endif

// 对象域定义
typedef struct
{
	TUnionObjectFldName		name;		// 域名称
	int				type;		// 域类型
	int				size;		// 大小
	//bool				isUnique;	// 是否唯一值
	bool				nullPermitted;	// 是否允许空值，1：允许空值，0：不允许空值
	TUnionObjectFldDefaultValue	defaultValue;	// 缺省值定义
	char				remark[128+1];	// 说明
} TUnionObjectFldDef;
typedef TUnionObjectFldDef		*PUnionObjectFldDef;

// 域组定义
#define conMaxFldNumPerFldGrp		8	// 一个唯一值域组可有的最大的域数目
typedef struct
{
	//int				maxValueLen;				// 域组值的最大长度
	//TUnionObjectName		fldGrpName;				// 域组名称
	unsigned int			fldNum;					// 域组中的域数目
	TUnionObjectFldName		fldNameGrp[conMaxFldNumPerFldGrp];	// 域组中的域名称
} TUnionObjectFldGrp;
typedef TUnionObjectFldGrp		*PUnionObjectFldGrp;

// 外部关键字定义
typedef struct
{
	TUnionObjectName		objectName;		// 外部对象名称
	TUnionObjectFldGrp		localFldGrp;		// 本地域组
	TUnionObjectFldGrp		foreignFldGrp;		// 外部域组
} TUnionDefOfRelatedObject;
typedef TUnionDefOfRelatedObject	*PUnionDefOfRelatedObject;

// 定义一个对象
#define conMaxUniqueFldGrpNumPerObject	8			// 定义一个对象可以有的最大的唯一值组
#define conMaxParentFldGrpNumPerObject	8			// 定义一个对象可以有的最大引用数，即本对象使用refrence或foreign key定义的指象其它对象的键值定义数
#define conMaxFldNumPerObject		64			// 定义一个对象可以有的最大的域数目
typedef struct
{
	TUnionObjectName		name;						// 对象名称
	unsigned int			fldNum;						// 域定义数目
	TUnionObjectFldDef		fldDefGrp[conMaxFldNumPerObject];		// 域定义
	TUnionObjectFldGrp		primaryKey;					// 关键字定义
	unsigned int			uniqueFldGrpNum;				// 唯一值组的数量
	TUnionObjectFldGrp		uniqueFldGrp[conMaxUniqueFldGrpNumPerObject];	// 唯一值定义数组
	unsigned int			foreignFldGrpNum;				// 外部关键字数据
	TUnionDefOfRelatedObject	foreignFldGrp[conMaxParentFldGrpNumPerObject];	// 外部关键字定义
} TUnionObject;
typedef TUnionObject			*PUnionObject;


// 定义使用一个对象的外部对象清单
#define conMaxChildFldGrpNumPerObject	32				// 定义一个对象可以有的最大被引用对象数，即其它对象使用refrence或foreign key指向的本对象的键值定义数
typedef struct
{
	int				childNum;					// 键值定义的数量
	TUnionDefOfRelatedObject	childObjectGrp[conMaxChildFldGrpNumPerObject];	// 引用本对象的其它对象关键字定义
} TUnionChildObjectGrp;
typedef TUnionChildObjectGrp		PUnionChildObjectGrp;

#define ObjectDefFlagOfObjectName		"objectName:"
#define ObjectDefFlagLenOfObjectName		11
#define ObjectDefFlagOfFldDefGrp		"fldDefGrp:"
#define ObjectDefFlagLenOfFldDefGrp		10
#define ObjectDefFlagOfPrimaryKey		"primaryKey:"
#define ObjectDefFlagLenOfPrimaryKey		11
#define ObjectDefFlagOfUniqueFldGrp		"uniqueFldGrp:"
#define ObjectDefFlagLenOfUniqueFldGrp		13
#define ObjectDefFlagOfForeignFldGrp		"foreignFldGrp:"
#define ObjectDefFlagLenOfForeignFldGrp		14

void UnionInitObjectDef(PUnionObject pdef);

/*
function:
        判断是否是对象中的字段
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
//int UnionIsFldOfObject(PUnionTableDef pobject, char *fldName);

/*
功能：
	将对象定义的域名称转换为实际数据库中的名称
输入参数：
	pobject：		指向对象定义的指针
	defFldNameGrp		域定义时采用的名称
输出参数：
	realFldNameGrpInDB	实际数据库中的名称	
返回值：
	>=0：	返回转换的域名称数量
	<0：	失败，返回错误码
*/
//int UnionConvertObjectFldNameInNameGrp(PUnionTableDef pobject,char defFldNameGrp[][128+1],int numOfFld,char realFldNameGrpInDB[][128+1]);

/*
功能：
	将对象定义的域名称转换为实际数据库中的名称
输入参数：
	idOfObject		对象名称
	defFldNameGrp		域定义时采用的名称
输出参数：
	realFldNameGrpInDB	实际数据库中的名称	
返回值：
	>=0：	返回转换的域名称数量
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldNameOfSpecObjectInNameGrp(TUnionIDOfObject idOfObject,char defFldNameGrp[][128],int numOfFld,char realFldNameGrpInDB[][128]);

/*
功能：从对象定义文件，读一个对象定义的关键字清单
输入参数：
	idOfObject：要读的对象ID	
	sizeOfBuf	关键字清单的大小
输出参数：
	primaryKeyList	关键字清单，两个域之间以,分开
返回值：
	0：关键字串的长度
	<0：失败，返回错误码
*/
int UnionReadObjectPrimarKeyList(TUnionIDOfObject idOfObject,char *primaryKeyList,int sizeOfBuf);

/*
功能	
	获得数据库域的最大长度
输入参数
	fldSize		域长度
输入出数
	无
返回值
	域的合法长度
*/
int UnionGetValidFldSizeOfDatabase(int fldSize);

// 函数定义
/*
功能：检查一个对象的定义是否存在
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectDef(TUnionIDOfObject idOfObject);

/*
功能：判断一个对象的定义是否合法
输入参数：
	objectDef：对象定义
输出参数：
	无
返回值：
	1：合法
	0：不合法
	<0：失败，错误码
*/
int UnionIsValidObjectDef(TUnionObject objectDef);

/*
功能：将对象定义，写入到对象定义文件中
输入参数：
	pobject：指向对象定义的指针
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
//int UnionStoreObjectDef(PUnionTableDef pobject);

/*
功能：将对象定义，写入到对象定义文件中
输入参数：
	pobject：指向对象定义的指针
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
//int UnionStoreObjectDefAnyway(PUnionTableDef pobject);

/*
功能：将对象定义，写入到指定文件中
输入参数：
	pobject：	指向对象定义的指针
	fileName	文件名
输出参数：
	无
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionPrintObjectDefToSpecFile(PUnionObject pobject,char *fileName);

/*
功能：从对象定义文件，读一个对象定义
输入参数：
	idOfObject：要读的对象ID
输出参数：
	pobject：指向对象定义的指针
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionReadObjectDef(TUnionIDOfObject idOfObject,PUnionObject pobject);

/*
功能：从对象定义文件，读一个对象定义
输入参数：
	idOfObject：要读的对象ID
	withLock	是否加读锁，1,加锁，0,不加锁
输出参数：
	pobject：指向对象定义的指针
返回值：
	0：成功
	<0：失败，返回错误码
*/
int UnionReadObjectDefWithLock(TUnionIDOfObject idOfObject,PUnionObject pobject,int withLock);

/*
功能	
	将一个域组写成串
输入参数
	flag		域组的标识
	pfldGrp		域组
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionCreateFldGrpIntoSQLToFp(char *flag,PUnionObjectFldGrp pfldGrp,FILE *fp);

/*
功能	
	从一个表定义创建SQL语句
输入参数
	fldList		域标识清单
	lenOfFldList	域标识清单的长度
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionCreateSQLFromObjectDefToFp(PUnionObject prec,FILE *fp);

/*
功能：
	获得一个对象的域定义的类型
输入参数：
	pobject：	指向对象定义的指针
	fldName		域名称
输出参数：
	fldTypeName	域的c误言类型
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
//int UnionGetTypeOfSpecFldFromObjectDef(PUnionTableDef pobject,char *fldName,char *fldTypeName);

/*
功能：
	转换一个对象域的名称
输入参数：
	pobject：	指向对象定义的指针
	oriFldName	域名称
输出参数：
	defName		域定义时采用的名称
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
//int UnionConvertObjectFldName(PUnionTableDef pobject,char *oriFldName,char *defName);

/*
功能：
	转换一个对象域的名称
输入参数：
	objectName	对象名称
	oriFldName	域名称
输出参数：
	defName		域定义时采用的名称
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldNameOfSpecObject(char *objectName,char *oriFldName,char *defName);

/*
功能	
	读出一张表的关键字
输入参数
	tblName	表名
输入出数
	primaryKey	读出的关键字串，2个关键字之间以,分隔
返回值
	>=0	成功,关键字串的长度
	<0	错误码
*/
int UnionReadPrimaryKeyOfSpecObject(char *tblName,char *primaryKey);

int UnionConvertTBLFldTypeIntoName(int type,char *typeName);

#endif

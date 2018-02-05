#ifndef _UnionComplexDBAPIOnUnionEnv_H_
#define _UnionComplexDBAPIOnUnionEnv_H_

#ifdef _WIN32_
#include "unionComplexDBCommon.h"
#include "unionComplexDBPrimaryKey.h"
#include "unionComplexDBRecord.h"
#include "unionComplexDBObjectFileName.h"
#else

#include "unionComplexDBObjectDef.h"	// added by wolfgang wang,2009/5/29
/*
// 对象域名字
typedef char		TUnionObjectFldName[40+1];
// 对象名称
typedef char		TUnionObjectName[40+1];
// 对象域的缺省值
typedef char		TUnionObjectFldDefaultValue[48+1];
typedef char		TUnionIDOfObject[32+1];

typedef int                     bool;

// 对象域定义
typedef struct
{
	TUnionObjectFldName		name;		// 域名称
	int				type;		// 域类型
	int				size;		// 大小
	//bool				isUnique;	// 是否唯一值
	bool				nullPermitted;	// 是否允许空值，1：允许空值，0：不允许空值
	TUnionObjectFldDefaultValue	defaultValue;	// 缺省值定义
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
*/

/*
功能：插入一个对象的实例
输入参数：
	idOfObject：表名
	record：格式1："域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionInsertObjectRecord(TUnionIDOfObject idOfObject,char *record,int lenOfRecord);

/*
功能：根据关键字删除一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey："fld1=域值|"
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionDeleteUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey);

/*
功能：修改一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	record：要修改的实例的值，格式为"域1=域值|域2=域值|…|域N=域值"，只包括要修改的域
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionUpdateUniqueObjectRecord(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int lenOfRecord);

/*
功能：根据关键字查询一个对象的实例
输入参数：
	idOfObject：对象ID
	primaryKey：对象实例的关键字，格式为"键值域1=域值|键值域2=域值|…键值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
输出参数：
	record：查找出来的记录，格式为"fld1=域值|fld2=域值|…|fldN=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
*/
int UnionSelectUniqueObjectRecordByPrimaryKey(TUnionIDOfObject idOfObject,char *primaryKey,char *record,int sizeOfRecord);


/*
功能：根据唯一值查询一个对象的实例
输入参数：
	idOfObject：对象ID
	uniqueKey：对象实例的唯一值，格式为"唯一值域1=域值|唯一值域2=域值|…唯一值域N=域值|"
	sizeOfRecord：接受查询记录的record的大小
输出参数：
	record：查找出来的记录，格式为"域1=域值|域2=域值|…|域N=域值"
返回值：
	>=0：成功，返回记录的长度
	<0：失败，错误码
*/
int UnionSelectUniqueObjectRecordByUniqueKey(TUnionIDOfObject idOfObject,char *uniqueKey,char *record,int sizeOfRecord);

/*
功能：批量删除一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：删除条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	无
返回值：
	>0：成功，返回被删除的实例数目
	<0：失败，错误码
*/
int UnionBatchDeleteObjectRecord(TUnionIDOfObject idOfObject,char *condition);

/*
功能：批量修改一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：修改条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
	record：要修改的实例的值，格式为"域1=域值|域2=域值|…|域N=域值|"，只包括要修改的域
	lenOfRecord：record的长度
输出参数：
	无
返回值：
	>=0：成功，返回被修改的实例数目
	<0：失败，错误码
*/
int UnionBatchUpdateObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *record,int lenOfRecord);

/*
功能：批量查询一个对象的实例
输入参数：
	idOfObject：对象ID
	condition：查询条件，格式为"域1=域值|域2=域值|域3=域值|…域N=域值|"
输出参数：
	fileName：存储了查询出来的实例的值的文件名
返回值：
	>=0：成功，返回查询出的记录的数目
	<0：失败，错误码
*/
int UnionBatchSelectObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName);

/*
功能：拼装一个关键字条件串
输入参数：
	idOfObject：	要读的对象ID	
	primaryKeyValueList	关键值字串,两个域值以.分开
	lenOfValueList		关键值字串长度
	sizeOfBuf	关键字清单的大小
输出参数：
	recStr		关键字条件串
返回值：
	0：	关键字条件串的长度
	<0：	失败，返回错误码
*/
int UnionFormObjectPrimarKeyCondition(TUnionIDOfObject idOfObject,char *primaryKeyValueList,int lenOfValueList,char *recStr,int sizeOfBuf);

/*
功能：
	从一个记录串拼装关键字
输入参数：
	idOfObject：		对象ID
	record：		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecord：		record的长度
	sizeOfPrimaryKey	关键值串缓冲大小
输出参数：
	primaryKey		关键值串，以.分开
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionFormPrimaryKeyStrOfSpecObjectRec(TUnionIDOfObject idOfObject,char *record,int lenOfRecord,char *primaryKey,int sizeOfPrimaryKey);

/*
功能：获得对象的实例数目
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	>=0：实例数
	<0：失败，错误码
*/
int UnionGetObjectRecordNumber(TUnionIDOfObject idOfObject);

int UnionReadObjectDef(TUnionIDOfObject idOfObject,PUnionObject pobject);

int UnionExistsObjectDef(TUnionIDOfObject idOfObject);

int UnionCreateObject(TUnionIDOfObject idOfObject);

int UnionDropObject(TUnionIDOfObject idOfObject);

/*
功能：
	执行一个表的触发器操作
输入参数：
	resName		对象名称
	operationID	操作标识
	isBeforeOperation	是操作之前执行还是之后执行，1 表示操作之前
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
输出参数：
	无
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr);

/*
功能：
	执行一个表的联合查询操作
输入参数：
	resName		对象名称
	recStr		对象实例的值，格式为"域1=域值|域2=域值|…|域N=域值"
	lenOfRecStr	record的长度
	sizeOfResStr	执行结果缓冲的大小
输出参数：
	resStr		执行结果
返回值：
	0：成功
	<0：失败，错误码
*/
int UnionExcuteUnionSelectSpecRecOnObject(char *resName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr);

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

int UnionBatchSelectUnionObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName);

#endif

#endif


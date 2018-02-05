//	HuangBaoxin
//	2009/07/22

#ifndef _UnionDesKey_1x_3x_
#define _UnionDesKey_1x_3x_

#ifndef _UnionDesKeyDB_2_x_
#define _UnionDesKeyDB_2_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif
#ifndef _UnionDesKey_4_x_
#define _UnionDesKey_4_x_
#endif


#include "unionDesKey.h"
#include "unionDesKeyDB.h"

typedef char		TUnionDate[8+1];	// YYYYMMDD

// 定义密钥类型
typedef struct
{
	char				fullName[40+1];		// 密钥全名，采用owner.keyApp.keyName
	char				value[48+1];		// 密钥密文
	char				checkValue[16+1];	// 密钥校验值
	TUnionDesKeyLength		length;			// 密钥强度
	//TUnionDesKeyProtectMethod	protectMethod;		// 密钥保护方法
	TUnionDesKeyType		type;			// 密钥类型
	TUnionDate			activeDate;		// 密钥有效日期
	TUnionDate			passiveDate;		// 密钥失效日期
} TUnionDesKey_1x;
typedef TUnionDesKey_1x			*PUnionDesKey_1x;

// 1.0 密钥结构体转换为 3.0
int UnionDesKey1xToDesKey3x(PUnionDesKey_1x pdesKey1x, PUnionDesKey_1x pdesKey1xOld, PUnionDesKey pdesKey3x);

// 3.0 密钥结构体转换为1.0
int UnionDesKey3xToDesKey1x(PUnionDesKey pdesKey3x, PUnionDesKey_1x pdesKey1x, PUnionDesKey_1x pdesKey1xOld);

// 从密钥文件读取一个密钥对象
int UnionReadDesKey1x(char *keyFileName, PUnionDesKey_1x pDesKey1x);

// 保存1.0密钥到密钥文件
int UnionSaveDesKey1x(PUnionDesKey_1x pDesKey1x);

// 删除1.0密钥的密钥文件
int UnionDeleteDesKey1x(PUnionDesKey_1x pDesKey1x);

// 保存3.x密钥到密钥文件
int UnionSaveDesKey3x(PUnionDesKey pdesKey3x);

// 保存3.x密钥到密钥文件
// 选择保存密钥或者old密钥
int UnionSaveDesKey3xByValueType(PUnionDesKey pdesKey3x, int valueType);

// 删除3.x密钥的密钥文件
int UnionDeleteDesKey3x(PUnionDesKey pdesKey3x);

/*
	从银联数据 1.0 的密钥库加载密钥到共享内存
	不包括OLD密钥
*/
int UnionLoadDesKey1xIntoMemory(PUnionDesKeyDB pdesKeyDB);

/*
	从银联数据 1.0 的密钥库把 OLD密钥 加载密钥到共享内存
*/
int UnionLoadOldDesKey1xIntoMemory(PUnionDesKeyDB pdesKeyDB);

/*
	查找密钥所在的位置
*/
int UnionFindDesKeyPosInKeyDB2(char *fullName, PUnionDesKeyDB pdesKeyDB);


/* 求日期tt与日期ss相差的天数  格式为YYYYMMDD */
int GetDaysBetween2Date(char *ss, char *tt);

/* 求某个日期ss加上i天后的日期, i是负数是表示ss之前的日期  格式为YYYYMMDD */
int AddDateByDatesYMD(int i, char *ss, char *tt);


// 初始化密钥库共享内存
int UnionInitDesKeyDB1x();

#endif

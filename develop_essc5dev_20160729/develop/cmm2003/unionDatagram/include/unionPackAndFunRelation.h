//	Wolfgang Wang
//	2008/10/08

#ifndef _unionPackAndFunRelation_
#define _unionPackAndFunRelation_

#define conPackFldAndFunVarRelationTagPackFldType	"fldType"
#define conPackFldAndFunVarRelationTagPackFldID		"packFldID"
#define conPackFldAndFunVarRelationTagVarValueTag	"varValueTag"
#define conPackFldAndFunVarRelationTagVarNameOfReaLen	"varNameOfRealLen"
#define conPackFldAndFunVarRelationTagRemark		"remark"
#define conPackFldAndFunVarRelationTagIsOptional	"isOptional"
#define conPackFldAndFunVarRelationTagIsPackToFun	"isPackToFun"
#define conPackAndFunRelatioTagFunAndPack		"funAndPack="	// 函数与报文对应关系
#define conPackAndFunRelatioTagPackUsedByFun		"packUsedByFun="	// 函数与报文对应关系
#define conPackAndFunRelatioTagServiceID		"serviceID"		

// 定义一个报文域与参数域的对应关系
typedef struct
{
	char	packFldID[64+1];	// 报文域的标识
	char	varValueTag[128+1];	// 参数值标识
	char	remark[128+1];		// 说明
	int	isPackToFun;		// 1,是报文域到函数输出参数;0是函数输入到报文域
	char	varNameOfRealLen[64+1];	// 指示实际长度的变量域
	char	fldType[128+1];		// 域的类型
	int	isOptional;		// 是否是可选域
} TUnionPackFldAndFunVarRelation;
typedef TUnionPackFldAndFunVarRelation	*PUnionPackFldAndFunVarRelation;

// 定义报文和函数的对应关系
#define conMaxNumOfPackFldAndFunVarRelation	64
typedef struct
{
	char				serviceID[3+1];				// 服务代码
	char				relationNum;				// 转换关系的数量
	TUnionPackFldAndFunVarRelation	relationGrp[conMaxNumOfPackFldAndFunVarRelation];
} TUnionPackAndFunRelation;
typedef TUnionPackAndFunRelation	*PUnionPackAndFunRelation;

/*
功能	
	获得一个报文和函数对应关系对应的服务代码
输入参数
	appID		报文的类别
	datagramID	报文的标识
输出参数
	fileName	获得的报文定义的文件名称
返回值
	>= 0	成功
	<0	出错代码
*/
int UnionGetServiceIDFromPackAndFunRelationDef(char *funName,char *serviceID);

/*
功能	
	获得一个报文定义的文件名
输入参数
	appID		报文的类别
	datagramID	报文的标识
输出参数
	fileName	获得的报文定义的文件名称
返回值
	无
*/
void UnionGetFileNameOfPackAndFunRelationDef(char *funName,char *fileName);

/*
功能	
	从一个数据串中读取一个报文域与函数参数对应关系定义
输入参数
	str		域定义串
	lenOfStr	串长度
输出参数
	pdef		读出的报文域与函数参数对应关系定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadPackFldAndFunVarRelationFromStr(char *str,int lenOfStr,PUnionPackFldAndFunVarRelation pdef);
/*
功能	
	从表中读取一个报文与函数的转换关系
输入参数
	funName		函数名称
	datagramID	报文的标识
输出参数
	pdef		读出的报文定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadSpecPackAndFunRelationFromFile(char *funName,PUnionPackAndFunRelation pdef);
/*
功能	
	打印一个报文域与函数参数转换关系
输入参数
	pdef		要打印的报文域与函数参数对应关系定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintPackFldAndFunVarRelationToFp(PUnionPackFldAndFunVarRelation pdef,FILE *fp);

/*
功能	
	打印一个报文与函数的转换关系
输入参数
	pdef		要打印的报文定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintPackAndFunRelationToFp(PUnionPackAndFunRelation pdef,FILE *fp);

/*
功能	
	打印一个报文与函数的转换关系
输入参数
	funName		函数名称
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecPackAndFunRelationToFp(char *funName,FILE *fp);

/*
功能	
	打印一个报文与函数的转换关系到指定文件
输入参数
	funName		函数名称
	fileName	文件名称
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintSpecPackAndFunRelationToSpecFile(char *funName,char *fileName);

#endif


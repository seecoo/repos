// wolfang wang
// 2008/10/3

#ifndef _unionVarDef_
#define _unionVarDef_

#define conVarDefTagTypePrefix		"typePrefix"
#define conVarDefTagVarName		"name"
#define conVarDefTagNameOfType		"nameOfType"
#define conVarDefTagIsPointer		"isPointer"
#define conVarDefTagDimisionNum		"dimisionNum"
#define conVarDefTagSizeOfDimision	"sizeOfDimision"
#define conVarDefTagRemark		"remark"

#define conMaxNumOfArrayDimision	4	// 数组的最大维度
// 报文域定义
typedef struct
{
	char		typePrefix[32+1];	// 变量类型前缀 union/struct等
	char		nameOfType[64+1];	// 变量类型
	int		isPointer;		// 1，是指针；0不是指针
	char		name[64+1];		// 变量名称
	int		dimisionNum;		// 数组的维度，0表示不是数组
	char		sizeOfDimision[conMaxNumOfArrayDimision][64+1];	// 每一维度的大小
	char		remark[80+1];
} TUnionVarDef;
typedef TUnionVarDef	*PUnionVarDef;

/*
功能	
	从一个定义口串中读取一个变量类型定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarDeclareTypeDefFromStr(char *str,int lenOfStr,PUnionVarDef pdef);


/*
功能	
	从一个定义口串中读取一个变量定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
输出参数
	pdef		变量定义
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadVarDefFromStr(char *str,int lenOfStr,PUnionVarDef pdef);

/*
功能	
	打印一个变量的类型定义
输入参数
	pdef			要打印的变量定义
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarTypeDeclareCProgramFormatToFp(PUnionVarDef pdef,FILE *fp);

/*
功能	
	打印一个变量定义，如果数组变量的大小未定义，则使用缺省大小
输入参数
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefSetArraySizeToFp(PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
功能	
	将函数的参数定义，以变量定义的形式打印到文件中
输入参数
	prefixBlankNum			变量名前缀的空格数
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefToFpOfCProgramWithPrefixBlank(int prefixBlankNum,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
功能	
	打印一个变量定义，在变量名前面增加前缀，如果数组变量的大小未定义，则使用缺省大小
输入参数
	prefix				变量前缀
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefWithPrefixSetArraySizeToFp(char *prefix,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
功能	
	打印一个变量定义
输入参数
	pdef			要打印的变量定义
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefToFp(PUnionVarDef pdef,int inCProgramFormat,FILE *fp);

/*
功能	
	将函数的参数定义，以变量定义的形式打印到文件中
输入参数
	prefix				变量前缀
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefToFpOfCProgram(PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
功能	
	将函数的参数定义，以变量定义的形式打印到文件中，在变量的名称前加前缀
输入参数
	pdef				要打印的变量定义
	defaultName			缺省的名字，如果未定义名字，则采用这个名字
	defaultArraySizeWhenSizeNotSet	缺省的数组大小，如果数组大小未定义
	fp		输出的文件指针
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefWithPrefixToFpOfCProgram(char *prefix,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
功能	
	打印一个变量定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
输出参数
	fp		输出的文件指针
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintVarDefInStrDefToFp(char *str,int lenOfStr,int inCProgramFormat,FILE *fp);

/*
功能	
	打印一个变量定义
输入参数
	str		定义串
	lenOfStr	定义串的长度
	inCProgramFormat	1，使用c语言定义格式输出，0，不使用c语言定义输出
输出参数
	无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputVarDefInStrDef(char *str,int lenOfStr,int inCProgramFormat);

#endif

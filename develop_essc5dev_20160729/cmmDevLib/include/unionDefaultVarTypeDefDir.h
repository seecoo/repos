// wolfang wang
// 2008/10/3

#ifndef _unionDefaultVarTypeDefDir_
#define _unionDefaultVarTypeDefDir_

// 关键字的缺省目录
#define conDefaultDesignDirOfBaseDef		"baseDef"
// 变量定义的缺省目录
#define conDefaultDesignDirOfDataStruct		"dataStruct"
// 常数定义的缺省目录
#define conDefaultDesignDirOfConst		"const"
// 函数定义的缺省目录
#define conDefaultDesignDirOfFun		"fun"
// 全局变量定义的缺省目录
#define conDefaultDesignDirOfVariable		"variable"
// 头文件定义的目录
#define conDefaultDesignDirOfIncludeConf	"includeConf"

/*
功能	
	获得指定模块的定义文件
输入参数
	moduleName	模块名称
输入出数
	fileName	文件名称
返回值
	无
*/
void UnionGetDefaultFileNameOfModuleDef(char *moduleName,char *fileName);
/*
功能	
	获得指定程序的定义文件
输入参数
	moduleName	程序名称
输入出数
	fileName	文件名称
返回值
	无
*/
void UnionGetDefaultFileNameOfProgramDef(char *programName,char *fileName);

/*
功能	
	获取完整的头文件定义文件
输入参数
	oriFileName	头文件定义文件
输入出数
	fullFileName	完整的头文件定义文件
返回值
	头文件的数量
*/
void UnionGetFullNameOfIncludeFileConf(char *oriFileName,char *fullFileName);

/*
功能	
	获取固定代码的定义文件
输入参数
	无
输入出数
	fileName	存入固定代码的定义文件
返回值
	>=0		正确
	<0		错误码
*/
int UnionFileNameOfFixedCodesConfFile(char *filename);

/*
功能	
	获得指定全局变量定义文件
输入参数
	funName		函数名称
输入出数
	fileName	文件名称
返回值
	函数定义文件名
*/
void UnionGetDefaultFileNameOfVariableDef(char *funName,char *fileName);

/*
功能	
	设置变量类型定义目录
输入参数
	dir		目录
输入出数
	无
返回值
	>=0		成功
	=		出错代码
*/
int UnionSetDefaultDesignDir(char *dir);

/*
功能	
	获得变量类型定义目录
输入参数
	无
输入出数
	无
返回值
	变量定义目录
*/
char *UnionGetDefaultDesignDir();

/*
功能	
	获得指定奕量类型标识定义文件
输入参数
	varTypeName	变量类型名称
输入出数
	fileName	文件名称
返回值
	变量定义目录
*/
void UnionGetDefaultFileNameOfVarTypeTagDef(char *varTypeName,char *fileName);

/*
功能	
	获得指定奕量类型定义文件
输入参数
	varTypeName	变量类型名称
输入出数
	fileName	文件名称
返回值
	变量定义目录
*/
void UnionGetDefaultFileNameOfVarTypeDef(char *varTypeName,char *fileName);

/*
功能	
	获得指定常量定义文件
输入参数
	constName	常量名称
输入出数
	fileName	文件名称
返回值
	变量定义文件名
*/
void UnionGetDefaultFileNameOfConstDef(char *constName,char *fileName);

#endif

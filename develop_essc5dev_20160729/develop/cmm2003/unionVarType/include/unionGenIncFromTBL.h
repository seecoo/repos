//	Wolfgang Wang
//	2009/4/29

#ifndef _unionGenIncFromTBL_
#define _unionGenIncFromTBL_

#define conDataTBLListFileName		"unionDataTBLList"
#define conDataTBLListTBLName		"tableList"

// 如果文件已存在的操作方法
#define conOverwriteModeWhenFileExistsOverwrite		1	//覆盖
#define conOverwriteModeWhenFileExistsReturn		0	//不写，返回
#define conOverwriteModeWhenFileExistsUserConfirm	3	//等待用户确认

#include "unionComplexDBCommon.h"

/*
功能	
	根据表定义，产生一个头文件
输入参数
	tblName			表名
	userSpecFileName	用户指定的文件名
	modeWhenFileExists	如果文件已存在的操作方法
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile(char *tblName,char *userSpecFileName,int modeWhenFileExists);

/*
功能	
	根据表名，获得表结构定义头文件路径
输入参数
	tblName		表名
输出参数
	fullPath	头文件路径
返回值
	无
*/
void UnionFormFullPathOfTableDefStructHeadFileName(char *tblName,char *fullPath);

/*
功能	
	根据表名，获得目标文件名
输入参数
	tblName		表名
输出参数
	fileName	文件名
返回值
	无
*/
void UnionFormObjFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName);

/*
功能	
	根据表名，获得目标文件名(含全路径)
输入参数
	tblName		表名
输出参数
	funName		c文件名
返回值
	无
*/
void UnionFormFullObjFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName);

/*
功能	
	根据表定义，产生一个makefile文件
输入参数
	tblName		表名
	fp		文件句柄
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateMakeFileFromTBLDefToFp(char *tblName,FILE *fp);

/*
功能	
	根据表名，获得makefile文件名
输入参数
	tblName		表名
输出参数
	funName		c文件名
返回值
	无
*/
void UnionFormFullMakeFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName);

/*
功能	
	根据表定义，产生一个makefile文件
输入参数
	tblName			表名
	fileName		文件名称
	modeWhenFileExists	文件存在时的写策略
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateMakeFileFromTBLDefToSpecFile(char *tblName,char *fileName,int modeWhenFileExists);

/*
功能	
	根据表名，获得makefile文件名
输入参数
	tblName		表名
输出参数
	funName		c文件名
返回值
	无
*/
void UnionFormMakeFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName);

/*
功能	
	根据表名，获得表对象定义
输入参数
	tblName		表名
输出参数
	pobjDef		表对象定义
返回值
	无
*/
int UnionReadObjectDefFromDesignDoc(char *tblName,PUnionObject pobjDef);

/*
功能	
	根据表名，获得表结构定义头文件全名（含路径)
输入参数
	tblName		表名
输出参数
	funName		头文件名
返回值
	无
*/
void UnionFormFullTableDefStructHeadFileName(char *tblName,char *incFileName);


/*
功能	
	根据表名，获得表结头文件名
输入参数
	tblName		表名
输出参数
	funName		头文件名
返回值
	无
*/
void UnionFormTableDefStructHeadFileName(char *tblName,char *incFileName);

/*
功能	
	根据表名，获得当前结构版本标识名
输入参数
	tblName		表名
	version		版本
输出参数
	funName		头文件名
返回值
	无
*/
void UnionFormStructTagNameOfCurrentVersionTBLDefFromTBLName(char *tblName,char *incFileName);

/*
功能	
	根据表名，获得结构版本标识名
输入参数
	tblName		表名
	version		版本
输出参数
	funName		头文件名
返回值
	无
*/
void UnionFormStructTagNameOfSpecVersionTBLDefFromTBLName(char *tblName,char *version,char *incFileName);

/*
功能	
	根据表名，和宏定义名，拼装一个宏定义名
输入参数
	tblName		表名
	macroTag	宏标识
输入出数
	macroName	宏名称
返回值
	无
*/
void UnionFormMacroNameFromTBLName(char *tblName,char *macroTag,char *macroName);

/*
功能	
	根据表名，获得程序名
输入参数
	tblName		表名
输入出数
	funName		程序名
返回值
	无
*/
void UnionFormTBLProgramNameFromTBLName(char *tblName,char *programName);

/*
功能	
	根据表名，获得头文件名
输入参数
	tblName		表名
输入出数
	funName		头文件名
返回值
	无
*/
void UnionFormIncFileNameOfTBLProgramFromTBLName(char *tblName,char *incFileName);

/*
功能	
	根据表名，获得头文件名(包括路径)
输入参数
	tblName		表名
输入出数
	funName		头文件名
返回值
	无
*/
void UnionFormFullIncFileNameOfTBLProgramFromTBLName(char *tblName,char *incFileName);


/*
功能	
	根据表名，获得C文件名(包括路径和版本)
输入参数
	tblName		表名
输入出数
	funName		c文件名
返回值
	无
*/
void UnionFormFullCFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName);

/*
功能	
	根据表名，获得表名对应的宏定义名称
输入参数
	tblName		表名
输入出数
	funName		宏定义名
返回值
	无
*/
void UnionFormTBLNameConstNameFromTBLName(char *tblName,char *constName);

/*
功能	
	根据表名，获得表资源对应的宏定义名称
输入参数
	tblName		表名
输入出数
	funName		宏定义名
返回值
	无
*/
void UnionFormTBLResIDConstNameFromTBLName(char *tblName,char *constName);

/*
功能	
	根据表名，拼装一个将记录结构转化为记录字串的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormConvertStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个将记录结构中的关键字转化为记录字串的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormConvertPrimaryKeyInStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个将记录结构中的非关键字转化为记录字串的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormConvertNonPrimaryKeyInStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个插入记录的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormInsertRecFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个读取记录的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormReadRecFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个修改记录的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormUpdateRecFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个删除记录的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormDeleteRecFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个修改记录指定域的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormUpdateSpecFldRecFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个修改记录指定整型域的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormUpdateIntTypeSpecFldRecFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个将记录字串转换为记录结构的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormConvertRecStrIntoStructFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个将记录字串转换为记录结构的关键字的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormConvertRecStrIntoStructPrimaryKeyFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个表的缺省赋值函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormStructDefaultValueSetFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个表的批量读取数据记录函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormBatchReadRecFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个表的域名称宏定义名
输入参数
	tblName		表名
	fldName		域名
输入出数
	fldDefName	域名称宏定义名
返回值
	无
*/
void UnionFormStructFldDefNameFromTBLName(char *tblName,char *fldName,char *fldDefName);

/*
功能	
	根据表名，拼装一个结构名称
输入参数
	tblName		表名
输入出数
	structName	结构名称
返回值
	无
*/
void UnionFormStructNameFromTBLName(char *tblName,char *structName);

/*
功能	
	根据表名，拼装一个结构名称
输入参数
	tblName		表名
	isPointer	是否定义为指针，1是，0否
输入出数
	structDefName	结构名称
返回值
	无
*/
void UnionFormStructDefNameFromTBLName(char *tblName,int isPointer,char *structDefName);

/*
功能	
	根据表定义，产生一个数据结构定义
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateStructDefFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，判断是否要为一个表定义宏定义
输入参数
	tblName		表名
输入出数
	无
返回值
	1	需要
	0	不需要
	<0	错误码
*/
int UnionExistsMacroDefInTBL(char *tblName);

/*
功能	
	根据表中的记录，定义宏，一个记录对应一个宏定义
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateConstDefFromTBL(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个数据结构缺省赋值方法的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateStructDefaultValueSetFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生批量读取记录的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateBatchReadRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个数据结构缺省赋值方法
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateStructDefaultValueSetFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个批量读取记录的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateBatchReadRecFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个将记录结构转换为记录字串的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateConvertStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个将记录结构中关键字转换为记录字串的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个将记录结构中非关键字转换为记录字串的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个将记录结构转换为记录字串的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateConvertStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个将记录串转换为记录结构的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateConvertRecStrIntoStructFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个将记录串转换为记录结构的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateConvertRecStrIntoStructFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个插入表中记录的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateInsertRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个将记录插入表中的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateInsertRecFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个删除表中记录的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateDeleteRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个将记录删除表中的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateDeleteRecFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个修改表中记录的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateUpdateRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个将记录修改表中的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateUpdateRecFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个读取表中记录的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateSelectRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个读取表中记录的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSelectRecFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	产生自动生成的代码标识
输入参数
	无
输入出数
	无
返回值
	无
*/
void UnionGenerateAutoCodesTagToFp(FILE *fp);
	
/*
功能	
	根据表定义，产生常量和结构定义的头文件
输入参数
	tblName		表名
	fp		文件句柄
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/

int UnionGenerateVersionSpecConstDefAndStructDefFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个头文件
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateIncFileFromTBLDefToFp(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个头文件
输入参数
	tblName			表名
	userSpecFileName	用户指定的文件名
	modeWhenFileExists	如果文件已存在的操作方法
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateIncFileFromTBLDef(char *tblName,char *userSpecFileName,int overwriteExists);

/*
功能	
	根据表定义，产生一个c文件
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateCFileFromTBLDefToFp(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个C文件
输入参数
	tblName			表名
	userSpecFileName	用户指定的文件名
	modeWhenFileExists	如果文件已存在的操作方法
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateCFileFromTBLDef(char *tblName,char *userSpecFileName,int overwriteExists);

/*
功能	
	根据表定义，产生一个C文件和头文件
输入参数
	tblName			表名
	modeWhenFileExists	如果文件已存在的操作方法
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateIncFileAndCFileFromTBLDef(char *tblName,int overwriteExists);

/*
功能	
	获得表清单定义头文件的名称
输入参数
	无
输入出数
	fileName	文件名
返回值
	>=0	成功
	<0	错误码
*/
void UnionGetIncFileNameOfTBLListDef(char *fileName);

/*
功能	
	根据表清单表中的记录，产生表清单定义头文件
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateTBLListDefFromTableListTBLToFp(FILE *fp);

/*
功能	
	根据表清单表中的记录，产生表清单定义头文件
输入参数
	无
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateTBLListDefFromTableListTBL();

/*
功能	
	根据表清单表中的记录，产生所有表的操作文件
输入参数
	modeWhenFileExists	如果文件已存在的操作方法
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateSrcForAllTBLFromTableListTBL(int modeWhenFileExists);

/*
功能	
	根据表中的记录，定义enum，一个记录对应一个enum值
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateEnumDefFromTBL(char *tblName,FILE *fp);

/*
功能	
	根据表中的记录，定义enum，一个记录对应一个enum值
输入参数
	tblName		表名
	fileName	文件名
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateEnumDefFromTBLToSpecFile(char *tblName,char *fileName);

/*
功能	
	根据表定义，产生一个修改指定域的函数的声明
输入参数
	tblName		表名
	isDeclaration	1表示是声明，0表示是函数体
	funName		函数名称
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateUpdateSpecFldFunDeclarationFromTBLDef(char *tblName,int isDeclaration,char *funName,FILE *fp);

/*
功能	
	根据表定义，产生一个数据结构缺省赋值方法
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateUpdateSpecFldFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个修改指定域的函数的声明
输入参数
	tblName		表名
	isDeclaration	1表示是声明，0表示是函数体
	funName		函数名称
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateUpdateIntTypeSpecFldFunDeclarationFromTBLDef(char *tblName,int isDeclaration,char *funName,FILE *fp);

/*
功能	
	根据表定义，产生一个数据结构缺省赋值方法
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateUpdateIntTypeSpecFldFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表名，拼装一个读取记录的域的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormReadRecFldFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表名，拼装一个读取记录的域的函数名称
输入参数
	tblName		表名
输入出数
	funName		函数名称
返回值
	无
*/
void UnionFormReadRecIntTypeFldFunNameFromTBLName(char *tblName,char *funName);

/*
功能	
	根据表定义，产生一个读取表中记录的域的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateSelectRecFldFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个读取表中记录的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSelectRecFldFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个读取表中记录的域的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateSelectRecIntTypeFldFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个读取表中记录的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateSelectRecIntTypeFldFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据操作清单表中的记录，产生操作定义头文件
输入参数
	fp		文件句柄
输入出数
	无
返回值
	>=0	域数目
	<0	错误码
*/
int UnionGenerateTBLCmdDefFromTBLDefToFp(FILE *fp);

/*
功能	
	获得表操作定义头文件的名称
输入参数
	无
输入出数
	fileName	文件名
返回值
	>=0	成功
	<0	错误码
*/
void UnionGetIncFileNameOfTBLCmdDef(char *fileName);

/*
功能	
	根据操作定义表中的记录，产生操作定义头文件
输入参数
	无
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateTBLCmdDefFromTBLDef();

/*
功能	
	根据表定义，产生一个将记录结构中关键字转换为记录字串的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个将记录结构中的非关键字转换为记录字串的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表定义，产生一个将记录串转换为记录结构的关键字的函数的声明
输入参数
	tblName		表名
	includeFunBody	是否包括函数体，1,包括，0不包括
	fp		文件句柄
输入出数
	funName		声明的函数名称
返回值
	无
*/
void UnionGenerateConvertRecStrIntoStructPrimaryKeyFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
功能	
	根据表定义，产生一个从记录串中读记录关键字的函数
输入参数
	tblName		表名
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateConvertRecStrIntoStructPrimaryKeyFunFromTBLDef(char *tblName,FILE *fp);

/*
功能	
	根据表名，获得makefile文件的执行脚本文件
输入参数
	tblName		表名
	fp		文件句柄
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateMakeFileExcuteScriptFromTBLDefToFp(char *tblName,FILE *fp);

/*
功能	
	根据表名，获得makefile文件的执行脚本文件名
输入参数
	tblName		表名
输出参数
	funName		c文件名
返回值
	无
*/
void UnionFormFullMakeFileExcuteScriptFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName);

/*
功能	
	根据表名，获得makefile文件的执行脚本文件名
输入参数
	tblName			表名
	specFileName		文件名称
	modeWhenFileExists	文件存在时的写策略
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile(char *tblName,char *specFileName,int modeWhenFileExists);

/*
功能	
	根据表名，获得makefile文件的执行脚本文件名
输入参数
	tblName		表名
输出参数
	fileName	文件名
返回值
	无
*/
void UnionFormMakeFileExcuteScriptFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName);

/*
功能	
	自动编译根据表自动生成的源代码
输入参数
	tblName			表名
输出参数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionComplyAutoGeneratedCFileFromTBLDef(char *tblName);

#endif

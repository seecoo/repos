// Wolfgang Wang
// 2002/08/26

#ifndef _UnionString_
#define _UnionString_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define conWordAsStr		1
#define conWordAsSeperator	2
#define conWordAsName		3
#define conWordAsCalculateChar	4

#define conStrValueFormatBCD			128	// 十六进制表示的字符串
#define	conIntValueFormatShiJinZhi		1	// 十进制
#define	conIntValueFormatXiaoXieShiLiuZhi	2	// 小写十六进制
#define	conIntValueFormatDaXieShiLiuZhi		3	// 大写十六进制

#define conStrCellAlignLeft			0	// 左对齐
#define	conStrCellAlignMiddle			1	// 位于中间
#define	conStrCellAlignRight			2	// 右对齐

#define conTestCaseTypeNormal			1	// 正常测试案例
#define conTestCaseTypeAbnormal			0	// 错误测试案例

// 2011-1-25,王纯军增加
/* 从目录中读取一个文件名、版本、后缀
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
输出参数
	nullFileName	读出的文件名
	version		版本
	suffix		后缀
返回值：
	>=0 	成功
	<0	出错代码

*/
int UnionReadSrcFileInfoFromFullDir(char *dir,int lenOfDir,char *nullFileName,char *version,char *suffix);

// 2011/1/1,王纯军增加
/* 将一个文件全名，折分成目录和文件名
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
输出参数
	onlyDir		拆分出的目录
	fileName	读出的文件名
返回值：
	>=0 	成功
	<0	出错代码

*/
int UnionSeperateDir(char *dir,int lenOfDir,char *onlyDir,char *fileName);

// 2010/12/10,王纯军
/*
功能	判断是否同一文件
输入参数
	firstFileName	第一个文件名称
	secondFileName	第二个文件名称
输出参数
        无
返回值
	1		同一文件
	0		不同文件
*/
int UnionIsSameFileName(char *firstFileName,char *secondFileName);

// 2010/12/10,王纯军
/*
功能	执行一个文件，并将结果输出到指定文件中
输入参数
	dir		文件目录
	excuteFileName	可执行文件名称
	outputFileName	输出文件名称
	overwrite	如果输出文件已存在，是否覆盖它，1，覆盖，0，不覆盖
输出参数
        无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionExecuteSpecFile(char *dir,char *excuteFileName,char *outputFileName,int overwrite);

// 2010/12/10,王纯军
/*
功能	写一个执行文件的脚本文件
输入参数
	dir		文件目录
	excuteFileName	可执行文件名称
	scriptFileName	脚本文件名称
输出参数
        无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionGenerateScriptFileForExecuteSpecFile(char *dir,char *excuteFileName,char *scriptFileName);

// 2010/12/10,王纯军
/*
功能	将一个unix文件的属性改为可执行
输入参数
	dir		文件目录
	fileName	文件名称
输出参数
        无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionChangeUnixFileToExecutable(char *dir,char *fileName);

// 2010/12/9,王纯军
/*
功能	将一个字符串写到文件中
输入参数
	str		字符串
	lenOfStr	字符串长度
	seperator	分隔符
	fileName	文件名
输出参数
        无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionOutputStrToSpecFile(char *str,int lenOfStr,char seperator,char *fileName);

// 2010/11/1,王纯军
/*
功能	打印指定数量的字符到文件中
输入参数
	num		空格数量
	specChar	指定的字符
	fp		文件指针
输出参数
        无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintNumOfSpecCharToFp(int num,char specChar,FILE *fp);

/*
功能	以指定的格式打印字符串到文件中
输入参数
	str		字符串
	lenOfStr	字符串长度
	fp		文件指针
	formatLen	字符串在文件中所占的长度，不足补空格
	DJFS		字符串对齐方式
			0	左对齐
			1	中间对齐
			2	右对齐
输出参数
        无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintStrToFpInFormat(char *str,int lenOfStr,FILE *fp,int formatLen,int DJFS);

// 2010/10/30,王纯军增加
/*
功能
	打开一个文件
输入参数
	fileName	文件名称
输出参数
	无
返回值
	成功		合法的文件句柄
	失败		空指针
*/
FILE *UnionOpenFile(char *fileName);

// 2010/10/30,王纯军增加
/*
功能
	打开一个文件
输入参数
	fileName	文件名称
输出参数
	无
返回值
	成功		合法的文件句柄
	失败		空指针
*/
FILE *UnionOpenFileForAppend(char *fileName);

// 2010/10/30,王纯军增加
/*
功能
	返回一个合法的文件句柄，如果fp不为空，则返回fp，否则返回stdout
输入参数
	fp		文件句柄
输出参数
	无
返回值
	成功		合法的文件句柄
	失败		空指针
*/
FILE *UnionGetValidFp(FILE *fp);

// 2010/10/30,王纯军增加
/*
功能
	关闭一个文件句柄
输入参数
	fp		文件句柄
输出参数
	无
返回值
	无
*/
void UnionCloseFp(FILE *fp);

// 2010/10/30,王纯军
/*
功能	拼成一个空格串
输入参数
	num		空格数量
	sizeOfStr	串大小
输出参数
        str		串指针
返回值
	>=0		成功，串长度
	<0		出错代码
*/
int UnionGenerateStrOfBlankOfNum(int num,char *str,int sizeOfStr);

// 2010/8/19,王纯军增加
/*
功能
	拆分一个域定义
	域定义格式为
	fldName=fldOffset[,fldLen]
输入参数
	fldDefStr		域定义串
	sizeOfFldName		域名缓冲大小
	sizeOfFldAlais		域别名缓冲大小
输出参数
	fldName			域名
	fldOffset		域偏移
	fldLen			域长度
	fldAlais		域别名
返回值
	>=0			成功
	<0			出错代码
*/
int UnionSeperateFldAssignmentDef(char *fldDefStr,char *fldName,int sizeOfFldName,int *fldOffset,int *fldLen,char *fldAlais,int sizeOfFldAlais);

// 2010/8/19,王纯军增加
/*
功能
	根据一个域选择定义，将相应的域写入到指定串中
输入参数
	fldDefStr		域定义串
	lenOfFldDefStr		域定义串长度
	oriDataStr		原始数据串
	lenOfOriDataStr		原始数据串长度
	sizeOfDesDataStr	目标数据串大小
输出参数
	desDataStr		目标数据串
返回值
	>=0			目标数据串的长度
	<0			出错代码
*/
int UnionFilterSpecRecFldUnderSpecDef(char *fldDefStr,int lenOfFldDefStr,char *oriDataStr,int lenOfOriDataStr,char *desDataStr,int sizeOfDesDataStr);

/*
功能
	获取一个月份的天数
输入参数
	month			月份
	year			年
输出参数
	无
返回值
	>=0			指定月份的天数
	<0			失败
*/
int UnionGetDaysOfSpecMonth(int month,int year);

/*
功能
	计算一个日期
输入参数
	firstYear		起如年份
	daysSinceFirstYear	从起始时间之后已过去的日
输出参数
	year			年
	month			月
	day			日
返回值
	>=0			成功
	<0			失败
*/
int UnionAnalysisAfterDate(int firstYear,long daysSinceFirstYear,long *year,long *month,long *day);

/*
功能
	分析特定时间
输入参数
	totalSeconds		从起始年过后的时间，以秒表
	firstYear		起始年
	timeZoneOffset		时区偏移，以格林威治时间为准
输出参数
	timeStr			日期时间串，格式"YYYY-MM-DD HH:MM:SS"
返回值
	>=0			成功
	<0			失败
*/
int UnionAnalysisSpecDateTime(long totalSeconds,int firstYear,int timeZoneOffset,char *timeStr);

/*
功能
	获取当前日期时间
输入参数
	无
输出参数
	timeStr			日期时间串，格式"YYYY-MM-DD HH:MM:SS MMM:MMM"
返回值
	>=0			成功
	<0			失败
*/
void UnionGetCurrentDateTimeInMacroFormat(char *timeStr);

// 读取当前设置时间
void UnionGetUserSetTime(struct timeval *userTime);

// 设置当前时间
void UnionSetUserSetTime(struct timeval *userTime);

// 重置当前时间
void UnionReInitUserSetTime();

// 获取当前运行时间,以微秒表示
long UnionGetRunningTimeInMicroSeconds();

// 获取运行时间,以毫秒表示
long UnionGetRunningTimeInMilliSeconds();

// 获取运行时间,以秒表示
long UnionGetRunningTimeInSeconds();

// 获取运行时间,以秒表示
long UnionGetRunningTime();

// 获取总运行时间,以微秒表示
long UnionGetTotalRunningTimeInMacroSeconds();

// 获取运行时间偏移量,以微秒表示
long UnionGetRunningTimeOffsetInMacroSeconds();

// 获取总运行时间,以毫秒表示
long UnionGetTotalRunningTimeInMilliSeconds();

// 获取总运行时间,以秒表示
long UnionGetTotalRunningTimeInSeconds();

// 获取总运行时间,以秒表示
long UnionGetTotalRunningTime();

// 获取毫秒表示的当前时间
// 返回值>0,当前时间.<0,出错了
long UnionGetCurrentTimeInMilliSeconds();

// 获取微秒表的当前时间
// 返回值>0,当前时间.<0,出错了
long UnionGetCurrentTimeInMicroSeconds();

// 2010/7/6,王纯军增加
/*
功能
	将一个整型值写入到字符串中
输入参数：
	intValue	整型值
	format		写入格式
输出参数
	value		字符串
返回值
	>=0	成功
	<0	错误码
*/
int UnionPutIntTypeValueIntoStr(int intValue,int format,char *value,int sizeOfBuf);

// 2010/5/12,王纯军增加
/*
功能
	复制一个串，如果源串比想复制的长度短，则左补位
输入参数：
	oriStr		源串
	expectedFldLen	期待复制的长度,如果该值<=0，则全部复制
	patchChar	补位字符
	sizeOfBuf	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>=0	成功,域的总数目
	<0	错误码
*/
int UnionCopyStrWithLeftPatch(char *oriStr,int expectedFldLen,char patchChar,char *desStr,int sizeOfBuf);

// 2010/5/12,王纯军增加
/*
功能
	复制一个串，如果源串比想复制的长度短，则右补位
输入参数：
	oriStr		源串
	expectedFldLen	期待复制的长度,如果该值<=0，则全部复制
	patchChar	补位字符
	sizeOfBuf	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>=0	成功,域的总数目
	<0	错误码
*/
int UnionCopyStrWithRightPatch(char *oriStr,int expectedFldLen,char patchChar,char *desStr,int sizeOfBuf);

// 2010/3/6
/* 读出文件名的后缀
输入参数
	fileName	文件全名
	lenOfFileName	文件全名的长度
	withDot		是否输出点
			1输出
			0不输出
输出参数
	suffix		读出的后缀
返回值：
	>=0 		后缀的长度
	<0		出错代码

*/
int UnionReadSuffixOfFileName(char *fileName,int lenOfFileName,int withDot,char *suffix);

/*
将一个SQL的where语句拆分成一个一个单词
*/
int UnionReadSQLWhereWordFromStr(char *str,int lenOfStr,char wordGrp[][128],int maxNum);

// 2008/10/13
/* 从目录中读取一个文件名
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
输出参数
	fileName	读出的文件名
返回值：
	>=0 	文件名的长度
	<0	出错代码

*/
int UnionReadFullFileNameFromFullDir(char *dir,int lenOfDir,char *fileName);

// 判断一个目录是否是另一个目录的子目录
/*
输入参数
	childDir		子目录
	fullDir			父目录
输出参数
	无
返回值
	1			是
	0			不是
	<0			错误码
*/
int UnionThisDirIsFirstChildDirOfSpecDir(char *childDir,char *fullDir);

// 从一个文件全名中折分出目录和文件名
/*
输入参数
	fullFileName		包含了路径的文件全名
	lenOfFullFileName	文件全名的长度
输出参数
	dir			文件目录，去掉了最后的/
	fileName		文件名
返回值
	>=0			成功
	<0			失败
*/
int UnionGetFileDirAndFileNameFromFullFileName(char *fullFileName,int lenOfFullFileName,char *dir,char *fileName);

// 2009/6/23，张永定增加
/*
功能
	把一个域名转换成带一个带前缀的域名。
输入参数
	fldName		转换前域名
输出参数
	prefixFldName	转换后增加前缀的域名
返回值
	>=0	成功
	<0	错误码
*/
int UnionChargeFieldNameToDBFieldName(char *fldName,char *prefixFldName);

// 2009/5/23,王纯军
/*
功能	恢复一个串中的指定字符
输入参数
	specChar	指定字符
	oriStr         	源串
        lenOfOriStr  	源串长度
        sizeOfDesStr	目标串缓冲大小
输出参数
        desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionRestoreSpecCharInStr(char specChar,char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 2009/5/23,王纯军
/*
功能	将一个串中的指定字符，作为特殊字符处理后，产生一个新串
输入参数
	specChar	指定字符
	oriStr         	源串
        lenOfOriStr  	源串长度
        sizeOfDesStr	目标串缓冲大小
输出参数
        desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionTranslateSpecCharInStr(char specChar,char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 2009/5/3，王纯军增加
/*
功能
	判断一个串，在不在字串中
输入参数
	str		串
	lenOfStr	串长度
	speratorTag	域分隔符
	specFld		要查的串
输出参数
	无
返回值
	>0	在串中
	==0	不在串在
	<0	错误码
*/
int UnionIsFldStrInUnionFldListStr(char *str,int lenOfStr,char speratorTag,char *specFld);

// 2009/4/30,王纯军
/*
功能	根据一个缺省值定义，获取缺省值
输入参数
	无
输出参数
        无
返回值
	1	是
	0	否
*/
int UnionConvertDefaultValue(char *defaultValueDef,char *defaultValue);

// 2009/4/29,王纯军增加，判断一个字符串是否是数学串
int UnionIsDecimalStr(char *str);

// 2008/12/17,王纯军
/*
功能	判断域名是否采用大写
输入参数
	无
输出参数
        无
返回值
	1	是
	0	否
*/
int UnionIsFldNameUseUpperCase();

// 2008/12/17,王纯军
/*
功能	设置一个域名采用大写字母
输入参数
	无
输出参数
        无
返回值
	无
*/
void UnionSetFldNameUseUpperCase();

// 2008/12/17,王纯军
/*
功能	设置一个域名不采用大写字母
输入参数
	无
输出参数
        无
返回值
	无
*/
void UnionSetFldNameUseNonUpperCase();

// 2008/11/24,王纯军
/*
功能	打印指定数量的空格
输入参数
	num		空格数量
	fp		文件指针
输出参数
        无
返回值
	>=0		成功
	<0		出错代码
*/
int UnionPrintBlankOfNumToFp(int num,FILE *fp);

// 2008/11/11,王纯军
/*
功能	将一个串拼装成科友的记录域串
	会将字符串中的:
		^^转换成.
		^*转换成*
输入参数
	oriStr         	源串
        lenOfOriStr  	源串长度
        sizeOfDesStr	目标串缓冲大小
输出参数
        desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionTranslateRecFldStrIntoStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 2008/11/11,王纯军
/*
功能	将一个串拼装成科友的记录域串
	会将字符串中的:
		.转换成^^
		^转换成^*

输入参数
	oriStr         	源串
        lenOfOriStr  	源串长度
        sizeOfDesStr	目标串缓冲大小
输出参数
        desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionTranslateStrIntoRecFldStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 2008/11/11,王纯军
/*
功能	拼装一个c语言的变量名，要将源变量名中的*移到变量最前面
输入参数
	prefix         	变量名的前缀
        oriVarName  	源变量名
输出参数
        desVarName	拼好的变量名
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionFormCSentenceVarName(char *prefix,char *oriVarName,char *desVarName);

// 2008/10/23,王纯军
/*
功能：
	拼装一个空值串
输入参数：
	fldGrp		要选择的域的清单，每个域之间以,分隔开
	lenOfFldGrp	域清单的长度
	sizeOfRecStr	域串缓冲的大小
输出参数：
	recStr		域串
返回值：
	>=0：成功，返回域串的长度
	<0：失败，错误码
*/
int UnionGenerateNullValueRecStr(char *fldGrp,int lenOfFldGrp,char *recStr,int sizeOfRecStr);

// 2008/10/23,王纯军
/*
功能：
	从记录串中读出指定域
输入参数：
	recStr		记录串
	lenOfRecStr	记录串长度
	fldGrp		要选择的域的清单，每个域之间以,分隔开
	lenOfFldGrp	域清单的长度
	sizeOfFldStr	域串缓冲的大小
输出参数：
	fldStr		域串
返回值：
	>=0：成功，返回域串的长度
	<0：失败，错误码
*/
int UnionReadFldGrpFromRecStr(char *recStr,int lenOfRecStr,char *fldGrp,int lenOfFldGrp,char *fldStr,int sizeOfFldStr);

// 2008/10/22
/* 过滤掉指定级别的目录
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
	dirNum		过滤掉的目录级数
输出参数
	desDir		目标目录
返回值：
	>=0 	文件名的长度
	<0	出错代码

*/
int UnionFilterDirFromDirStr(char *dir,int lenOfDir,int dirNum,char *desDir);

// 2008/10/20,王纯军
/*
函数功能：按分隔符拆串
输入参数：
	varStr：	字符串
	lenOfVarStr	字符串长度
	oper		分隔符
	maxNumOfVar	折分的字符串可有的最大数目
输出参数
	varGrp		折分的字符串变量
返回：
	>=0		拆串后个数
	<0		出错代码
*/
int UnionSeprateVarStrIntoVarGrp(char *varStr, int lenOfVarStr,char oper,char varGrp[][128],int maxNumOfVar);

// 2015/1/21,张永定
/*
函数功能：按分隔符拆串，用于替代UnionSeprateVarStrIntoVarGrp
输入参数：
	varStr：	字符串
	lenOfVarStr	字符串长度
	oper		分隔符
	numOfArray	数组的一维
	sizeOfArray	数组的二维
输出参数
	arrayPtr	数组的首地址
返回：
	>=0		拆串后个数
	<0		出错代码
*/
int UnionSeprateVarStrIntoArray(char *varStr, int lenOfVarStr,char oper,char *arrayPtr,int numOfArray,int sizeOfArray);

// 2015/1/21,张永定
/*
函数功能：按分隔符拆串，源串varStr会改变，
输入参数：
	varStr：	字符串
	lenOfVarStr	字符串长度
	oper		分隔符
	maxNumOfVar	折分的字符串可有的最大数目
输出参数
	varPtr		折分的字符串指针的指针
返回：
	>=0		拆串后个数
	<0		出错代码
*/
int UnionSeprateVarStrIntoPtr(char *varStr, int lenOfVarStr,char oper,char **varPtr,int maxNumOfVar);

// 2008/10/13
/* 从目录中读取一个文件名
输入参数
	dir		文件全名
	lenOfDir	文件全名的长度
输出参数
	fileName	读出的文件名
返回值：
	>=0 	文件名的长度
	<0	出错代码

*/
int UnionReadFileNameFromFullDir(char *dir,int lenOfDir,char *fileName);

// 2008/10/13
// 判断一个字符是否是数字
int UnionIsDigitChar(char ch);

// 判断一个字符串是否全是数字
int UnionIsDigitString(char *str);

// 2008/10/13
// 判断一个字符是否是字母
int UnionIsAlphaChar(char ch);

// 2008/10/13
/* 从一个字符串中读取一个C语言的变量名字
输入参数
	str		域定义串
	lenOfStr	域定义串的长度
输出参数
	varName		读出的名字
返回值：
	>=0 	返回名字在字符串中占的长度
	<0	出错代码

*/
int UnionReadCProgramVarNameFromStr(char *str,int lenOfStr,char *varName);

// 2008/10/13
/* 从一个字符串中读取所有C语言的变量名字
输入参数
	str		域定义串
	lenOfStr	域定义串的长度
	maxNumOfVar	最大的变量名字
输出参数
	varName		读出的名字
返回值：
	>=0 		读出的变量名字的数量
	<0		出错代码

*/
int UnionReadAllCProgramVarNameFromStr(char *str,int lenOfStr,char varName[][128],int maxNumOfVar);

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutCharTypeRecFldIntoRecStr(char *fldName,char value,char *recStr,int sizeOfRecStr);

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutDoubleTypeRecFldIntoRecStr(char *fldName,double value,char *recStr,int sizeOfRecStr);

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutLongTypeRecFldIntoRecStr(char *fldName,long value,char *recStr,int sizeOfRecStr);

int UnionPutLongLongTypeRecFldIntoRecStr(char *fldName,long long value,char *recStr,int sizeOfRecStr);

// 2008/7/18,增加
// 将一个二进制类型的记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutBitsTypeRecFldIntoRecStr(char *fldName,unsigned char *value,int lenOfValue,char *recStr,int sizeOfRecStr);

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutIntTypeRecFldIntoRecStr(char *fldName,int value,char *recStr,int sizeOfRecStr);

// 2008/10/8,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutStringTypeRecFldIntoRecStr(char *fldName,char *value,char *recStr,int sizeOfRecStr);

// add by chenliang, 2009-02-09
/*
功能:
	将unionRecStr中域名为fldName的值改为fldValue
参数:
	输入参数 [in]
		sizeOfRecStr	:需要修改的 unionRec 容量大小
		fldName		:需要修改的域名
		fldValue	:修改后的域值
	输出参数 [out]
		unionRecStr	:需要修改的 unionRec
返回值:
	>=0	修改后 unionRecStr 的长度
	< 0	出错代码
*/
int UnionUpdateSpecFldValueOnUnionRec(char *unionRecStr, int sizeOfRecStr, char *fldName, char *fldValue);
// add end, 2009-02-09



// UnionDeleteSpecFldOnUnionRec; add by LiangJie, 2009-05-13
/*
功能:
	删除记录串中指定的域
参数:
	输入参数 [in]
		unionRecStr	:修改前的记录串
		lenOfRecStr	:记录串的长度
		fldName		:需要删除的域名
	输出参数 [out]
		unionRecStr	:删除指定域后的记录串
返回值:
	>=0	修改后 unionRecStr 的长度
	< 0	出错代码
*/
int UnionDeleteSpecFldOnUnionRec(char *unionRecStr, int lenOfRecStr, char *fldName);


// 2008/10/5，增加
/*
功能	从文件中读取一个数据行，过滤掉注释行
输入参数
	fp		文件名柄
	sizeOfBuf	缓冲区大小
输出参数
	buf		读出的数据
返回值
	>= 0		读出的数据的长度
	<0		出错代码
*/


int UnionReadOneDataLineFromTxtFile(FILE *fp,char *buf,int sizeOfBuf);

// 2008/10/5，增加
/*
功能	判断一个名字是否是合法的c语言变量名字
输入参数
	name		名字
	lenOfName	名字长度
输出参数
	无
返回值
	1		是
	0		不是
*/
int UnionIsValidCProgramName(char *name,int lenOfName);

// 2008/10/3,增加
// 从文本文件读取一行
int UnionReadOneLineFromTxtFile(FILE *fp,char *buf,int sizeOfBuf);

// 2008/7/29，增加
/*
功能	使用通用的域分隔符替换指定的分隔符
输入参数
	oriStr		串
	lenOfOriStr	串长度
	oriFldSeperator	源分隔符
	desFldSeperator	目标分隔符
	sizeOfDesStr	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>= 0		目标串的长度
	<0		出错代码
*/
int UnionConvertOneFldSeperatorInRecStrIntoAnother(char *oriStr,int lenOfOriStr,char oriFldSeperator,char desFldSeperator,char *desStr,int sizeOfDesStr);

// 2008/7/24，增加
/*
功能	使用通用的域分隔符替换指定的分隔符
输入参数
	oriStr		串
	lenOfOriStr	串长度
	fldSeperator	分隔符
	sizeOfDesStr	目标串缓冲的大小
输出参数
	desStr		目标串
返回值
	>= 0		目标串的长度
	<0		出错代码
*/
int UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator(char *oriStr,int lenOfOriStr,char fldSeperator,char *desStr,int sizeOfDesStr);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadIntTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int *value);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,long *value);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadDoubleTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,double *value);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadCharTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value);

// 2008/7/18,增加
// 将一个记录域打包到字符串中
// 返回值是记录域在串中的长度
int UnionPutRecFldIntoRecStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadBitsTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,unsigned char *value,int sizeOfBuf);

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionReadRecFldFromRecStrForCommon(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf,int isBitsData);

// 2008/5/22，增加
/* 保留指定的IP地址级别
输入参数
	ipAddr	原始ip地址
	level	要保留的ip地址的级别
		4，保留4部分
		3，保留前3部分
		2，保留前2部分
		1，保留前1部分
		0，全部不保留
输出参数
	outIPAddr	加工后的ip地址
			不保留的部分以XXX替代
返回值
	0	成功
	负值	出错代码
*/
int UnionDealWithIPAddr(char *ipAddr,int level,char *outIPAddr);

// 2008/5/15增加
// 检查一个字符是否符合奇校验
int UnionIsOddParityChar(char chCheckChar);

// 2008/5/15增加
// 对一个字符串增加奇校验
int UnionMakeStrIntoOddParityStr(char *pData,int nLenOfData);

// 2008/5/15增加
// 检查一个字符串是否符合奇校验
// 符合奇校验返回值为1，否则返回值为0
int UnionCheckStrIsOddStr(char *pData,int nLenOfData);

/* 2008/3/31增加
功能：从一个字符串得到指定第几个域的值
输入参数:
	buf		输入字符串
	spaceMark	分隔符
	index		第几个值
输出参数：
	fld		第index的值

返回值：
	返回值的长度
*/
int UnionReadFldFromBuf(char *buf,char spaceMark,int index,char *fld);

// 去掉字符串中所有空格，2008-3-19
int UnionFilterAllBlankChars(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

// 2008/3/17增加
// 去掉字符串中的多余空格和tab，即将多个空格压缩为一个空格。去掉头和尾的空格和tab
int UnionFilterRubbisBlankAndTab(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

// 判断是否合法的变量名
int UnionIsValidAnsiCVarName(char *varName);

// 从文本文件读取一行
int UnionReadOneLineFromTxtStr(FILE *fp,char *buf,int sizeOfBuf);

// 从一个字符串中读取参数
/* 输入参数：
	str	字符串
   输出参数
   	par	放读出的参数
   	maxNum	要读出的最大参数数目
   返回值
   	返回读取的参数数目
*/
int UnionReadParFromStr(char *str,char par[][128],int maxNum);

// 将人民币小写金额转换为大写金额
int UnionSmallAmtToBigAmt(char *smallAmt, char *bigAmt);

// 从参数组中读取参数
// 参数组中
/* 输入参数：
	parGrp,参数组
	parNum，参数数目
	parID,参数名称
	sizeOfParValue,参数缓冲的大小
   输出参数
   	parValue,接受参数的缓冲
   返回值：
   	<0,错误码
   	>=0,参数长度
*/
int UnionReadParFromParGrp(char *parGrp[],int parNum,char *parID,char *parValue,int sizeOfParValue);

// 从命令串中读取参数
// 命令串的定义格式为:
//	-parID parValue
/* 输入参数：
	cmdStr,命令串
	parID,参数名称
	sizeOfParValue,参数缓冲的大小
   输出参数
   	parValue,接受参数的缓冲
   返回值：
   	<0,错误码
   	>=0,参数长度
*/
int UnionReadParFromCmdStr(char *cmdStr,char *parID,char *parValue,int sizeOfParValue);

// 从记录定义串中读取域值
// 记录串的定义格式为:
//	fldName=fldValue;fldName=fldValue;...fldName=fldValue;
/* 输入参数：
	recStr,记录串
	fldName,域名
	fldTypeName,类型名
		合法值	char/short/int/long/double/string
	sizeOfFldValue,域值缓冲的大小
   输出参数
   	fldValue,接受域值的缓冲
   返回值：
   	<0,错误码
   	>=0,域值长度
*/
int UnionReadFldFromRecStr(char *recStr,char *fldName,char *fldTypeName,unsigned char *fldValue,int sizeOfFldValue);

// 返回PK串的长度
int UnionGetPKOutOfRacalHsmCmdReturnStr(unsigned char *racalPKStr,int lenOfRacalPKStr,char *pk,int *lenOfPK,int sizeOfBuf);

int UnionFormFullDateFromMMDDDate(char *mmDDDate,char *fullDate);

// 判断串中是否含有.
int UnionIsStringHasDotChar(char *str);

// 去掉字符串中所有空格，2008-3-19
int UnionFilterAllSpecChars(char *sourStr,int lenOfSourStr,char specChar,char *destStr,int sizeOfBuf);

// 去掉字符串中的多余空格，即将多个空格压缩为一个空格。去掉头和尾的空格
int UnionFilterRubbisBlank(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

int UnionConvertLongStrIntoDoubleOf2TailStr(char *str);
int UnionConvertDoubleOf2TailStrIntoLongStr(char *str);

int UnionIsBCDStr(char *str1);	// Added by Wolfgang Wang, 2003/09/09
int UnionIsDigitStr(char *str);

int UnionConvertIntIntoStr(int data,int len,char *str);
int UnionConvertLongIntoStr(long data,int len,char *str);

int UnionConvertIntStringToInt(char *str,int lenOfStr);
long UnionConvertLongStringToLong(char *str,int lenOfStr);

int UnionConvertIntoLen(char *str,int intLen);

int UnionToUpper(char *lowerstr,char *upperstr);
int UnionUpperMySelf(char *str,int lenOfStr);
int UnionToUpperCase(char *str);
int UnionToLowerCase(char *str);
int UnionFilterRightBlank(char *Str);
int UnionFilterLeftBlank(char *Str);
int UnionVerifyDateStr(char *Str);
int UnionIsValidIPAddrStr(char *ipAddr);
int UnionCopyFilterHeadAndTailBlank(char *str1,char *str2);
int UnionFilterHeadAndTailBlank(char *str);
int UnionReadDirFromStr(char *str,int dirLevel,char *dir);

char hextoasc(int xxc);
char hexlowtoasc(int xxc);
char hexhightoasc(int xxc);
char asctohex(char ch1,char ch2);
int aschex_to_bcdhex(char aschex[],int len,char bcdhex[]);
int bcdhex_to_aschex(char bcdhex[],int len,char aschex[]);
int byte2int_to_bcd(int aa,char xx[]);
int IntToBcd(int aa,unsigned char xx[]);
int BcdToInt(char xx[]);

int UnionFormANSIX80LenString(int len,char *str,int sizeOfStr);
int UnionFormANSIDERRSAPK(char *pk,int lenOfPK,char *derPK,int sizeOfDERPK);
int UnionGetPKOutOfANSIDERRSAPK(char *derPK,int lenOfDERPK,char *pk,int sizeOfPK);
int UnionFilterRSASignNullChar(char *str,int len);

int UnionIsUnixShellRemarkLine(char *line);
int UnionConvertUnixShellStr(char *oldStr,int lenOfOldStr,char *newStr,int sizeOfNewStr);
int UnionReadUnixEnvVarFromStr(char *envVarStr,int lenOfEnvVarStr,char *envVar,int *envVarNameLen);
int UnionDeleteSuffixVersionFromStr(char *oldStr,char *newStr);
int UnionStrContainCharStar(char *str);

int UnionIsValidFullDateStr(char *date);

int UnionIsStringContainingOnlyZero(char *str,int lenOfStr);

// Mary add begin, 20080925
// 以下3个函数由昌子祥编写测试，陈家梅将函数声明拷贝到本程序中
int	unionIsDigit(char c);
int	unionIsHChar(char c);
int	unionStrToInt(char	*str,int Len);
// Mary add end, 20080925

// Mary add begin, 20081007
// 以下2个函数由昌子祥编写测试，陈家梅将函数声明拷贝到本程序中
/*
2008/10/06，昌子翔增加
功能：返回以分隔符分割的子串
输入参数
	srcStr：串
	Seperator：源分隔符
输出参数
	desStr：分割字符串
	CntOfDesStr：分割后子串个数
返回值
	>=0：串位移 offset
	<0：出错代码
*/
int UnionStrChr(char *srcStr,char Seperator,char desStr[][1024],int *CntOfDesStr);

/*
2008/10/06，昌子翔增加
功能
	返回以分隔符分割的子串
输入参数
	srcStr：串
	Seperator：分隔串
输出参数
	desStr：分割字符串
返回值
	>=0：分割后子串个数
	<0：出错代码
*/
int UnionStrToK(char *srcStr,char *Seperator,char desStr[][1024]);
// Mary add end, 20081007

// 2008/7/18,增加
// 从字符串中读取记录域
// 返回域值的长度
int UnionExistsRecFldInRecStr(char *recStr,int lenOfRecStr,char *fldName);

// Mary add begin, 20081124
/*
功能：根据转换表将十六进制字符串转换为十进制字符串
输入参数：
	hexStr：十六进制字符串
	hexStrLen：十六进制字符串的长度
	transTable：十六进制字符对应的数字表，例如："0123456789012345"，表示'A-F'转换为'0-5'
	digitStrLen：需要返回的数字串的长度
输出参数：
	digitStr：十进制字符串
	sizeOfDigitStr：digitStr的存储空间大小
返回值：
	>=0：成功，返回digitStr的长度
	<0：失败
算法过程：
	1、十六进制字符串中从左到右取'0'－'9'之间的数字，顺序组成一个串；
	2、十六进制字符串中从左到右取'A'－'F'之间的字母，根据十六进制字母转换表，
	   将字母转换成数字，按顺序组成一个串；
	3、将第1步和第2步的两个串相连，得到一个全数字串，按照长度取值返回
*/
int UnionTranslateHEXStringToDigitString(char *hexStr,int hexStrLen,char *transTable,int digitStrLen,char *digitStr,int sizeOfDigitStr);
// Mary add end, 20081124

// 陈家梅增加张永定写的函数, 20081124
// 将字符串类型的的时间值转换为time_t型，datetime必须为"yyyymmddhhmmss"
time_t UnionTranslateStringTimeToTime(char *datetime);

//计算以base为基底的n次方
int UnionPower(int base, int n);

//十六进制字符串转换为十进制整数
int UnionOxToD(char *hexStr);

// Mary add begin, 20090327, 函数由张永定编写
/*
功能：		对数字进行四舍五入

输入参数：	dNumber	输入的数字
		digit保留小数位的位数

返回值：	四舍五入后的数字
*/

double UnionChangeDigitOfNumber(double dNumber,int digit);
// Mary add end, 20090327

// 去除字符串的右空格
int UnionRTrim(char *str);

// 从一个mngSvr格式的请求串中删除某域
int UnionDeleteRecFldFromRecStr(char *str, char *fldName);

// 取数据库表字段前缀
char *UnionGetPrefixOfDBField();

/* 2010-3-23 wangk
功能	将数据库的字符串中的单引号替换为2个单引号，插入和更新时字符串中有单引号的时候会出错
输入参数
	oriStr         	源串
	lenOfOriStr  	源串长度
	sizeOfDesStr	目标串缓冲大小
输出参数
	desStr		目标串
返回值
	>=0		拼好的变量名的长度
	<0		出错代码
*/
int UnionTranslateSpecCharInDBStr(char *oriStr, int lenOfOriStr, char *desStr, int sizeOfDesStr);

// 检查日期是不是MMYY的格式
int UnionIsValidMMYYDateStr(char *mmyyDate);

// 按位取反
int UnionCPL(unsigned char *pData1, int nLenOfData, unsigned char *pData2);

/*
功能	写ini文件中的一个域
输入参数
	appName		字段名
	keyName		键名
	keyValue	键值
	fileName	ini文件名
输出参数
	无
返回值
	>=0	正确
	<0	错误
*/
int UnionINIWriteString(char *appName,char *keyName,char *keyValue,char *fileName);

/*
功能	读ini文件中的一个字符串域
输入参数
	appName		字段名
	keyName		键名
	defaultValue	默认值，如果没找到，返回这个值
	sizeOfBuf	键值缓冲区的大小
	fileName	ini文件名
输出参数
	keyValue	键值	
返回值
	>0	键值的长度
	=0	未找
	<0	错误
*/
int UnionINIReadString(char *appName,char *keyName,char *defaultValue,char *keyValue,int sizeOfBuf,char *fileName);

/*
功能	读ini文件中的一个整型域
输入参数
	appName		字段名
	keyName		键名
	defaultValue	默认值，如果没找到，返回这个值
	fileName	ini文件名
输出参数
返回值
	键值

*/
int UnionINIReadInt(char *appName,char *keyName,int defaultValue,char *fileName);

// 获得指定秒数之前的一个时间值agoTime，格式为"yyyymmddHHMMSS"
int UnionGetFullDateTimeSecondsBeforeNow(int secondsBefore, char *agoTime);

// 返回星期值:1~7
int UnionGetDays();

// added 2012-09-12
// 是否使用动态RSA批次号 
// 批次号:1NNNNYYDDD, NNNN为批次号,YY为年份,DDD为一年中的第几天
int UnionIsUseDynamicRsaBatchNo();

int UnionFormRsaBatchNoCondition(int inBatchNo, char *condition);


/*
 * UnionTransRsaBatchNo 返回转换后的批次号
 * 使用时间为前缀的批次号，以防止流程号重复使用以导致记录重复
 * 转换后的批次号格式为:1NNNNYYDDD,
 *   其中YY为年份，DDD为一年中的天数，NNNN为传进来的批次号
 */
int UnionTransRsaBatchNo(int inBatchNo);

int UnionTime(int *ctime);

//add by hzh in 2012.11.8设置当前rsa指数为3, 或65537
void UnionSetCurrentRsaExponent(int exponent);
int UnionGetCurrentRsaExponent();
//add end 

char *UnionStrCaseStr(const char *haystack, const char *needle);

char *UnionStrRCaseStr(const char *haystack, const char *needle);

int UnionGetTableNameAndFieldNameFromSQL(char *sql,char *tableName,char *fieldName);

int UnionFormRecStrUnderSpecDef(char *fldDefStr,int lenOfFldDefStr,char *oriDataStr,int lenOfOriDataStr,char *desDataStr,int sizeOfDesDataStr);

int UnionReadLongLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int64_t  *value);

// 把str2插入str1的指定pos位置,pos的下标为1
int UnionInsertString(char *str1,char *str2,int pos);

// 读取客户化标识
char *UnionGetIDOfCustomization();

// 判断是否是界面服务
int UnionIsUITrans(char *sysID);

// 2014-6-10 张永定增加
// 判断密码是否过期
// 输入：passwdUpdateTime - 密码最近修改日期
// 返回：>=0 - 还剩下的天数；<0 - 已过期
int UnionIsOverduePasswd(char *passwdUpdateTime,char *remark);

// 以下的函数生成一个长度为0x500（合10进制数：1280）的cryptTable[0x500]  
void UnionHashTablePrepareCryptTable();

// 以下函数计算hashData 字符串的hash值，其中dwHashType 为hash的类型，  
// 其可以取的值为0、1、2；该函数  
// 返回hashData 字符串的hash值；  
unsigned long UnionHashTableHashString( char *hashData, unsigned long dwHashType);

// 检查报文格式，并返回报文长度
int UnionIsCheckKMSPackage(char flag,char *package,int lenOfPackage,char *operator,char *appID,char *resID,char *serviceCode);

int UnionGetCurrentSecondTime();

// 是否显示敏感信息
int UnionIsDisplaySensitiveInfo();

/*
功能：	
	比较字符串，含通配符
输入：	
	s,不含有通配符
	t，含有通配符
返回:
	0表示不匹配，
	1表示匹配
*/
int UnionStrComp(char *s, char *t);

#endif

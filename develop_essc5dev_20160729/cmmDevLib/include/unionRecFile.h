//---------------------------------------------------------------------------

#ifndef unionRecFileH
#define unionRecFileH
//---------------------------------------------------------------------------

#include <stdio.h>

typedef FILE	*TUnionRecFileHDL;

/*
功能	将一个串写入到文件
输入参数
	fileName	记录文件名
	fmt		串
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
*/
int UnionWriteToFile(char *fileName,char *fmt,...);

//----------------------------------------------------------------------------
/*
功能	将两个目录名合并成一个目录名
输入参数
	firstDir	第一个目录
	secondDir	第二个目录
输出参数
	newDirName  	新目录名
返回值
	>=0		目录长度
	<0		出错代码
*/
int UnionFormDirName(char *firstDir,char *secondDir,char *newDirName);

//----------------------------------------------------------------------------
/*
功能	将第一个目录追到加到第二个目录上
输入参数
	firstDir	第一个目录
	secondDir	第二个目录
输出参数
	secondDir  	第二个目录
返回值
	>=0		目录长度
	<0		出错代码
*/
int UnionAppendFirstDirToSecondDir(char *firstDir,char *secondDir);

//---------------------------------------------------------------------------
/*
功能	将第一个文件追加到第二个文件中
输入参数
	oriFileName	源文件名称
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
*/
int UnionAppend2ndFileTo1stFile(char *oriFileName,char *desFileName);

/*
功能	将一个串追加到文件
输入参数
	fileName	记录文件名
	fmt		串
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
*/
int UnionAppendToFile(char *fileName,char *fmt,...);

//---------------------------------------------------------------------------
// 2010-12-13，王纯军增加
/*
功能	以第一个文件为基准，比较两个文件，并输出不一致的行
输入参数
	firstFileDir	第一个文件的目录
	firstFileName	第一个文件名称
	secondFileName	第二个文件名称
	unsameFileFp	不一致的内容
输出参数
	无
返回值
	>=0		不一致的行数
	<0		出错代码
*/
int UnionCompareFile_1stFileWithDir(char *firstFileDir,char *firstFileName,char *secondFileName,FILE *unsameFileFp);

//---------------------------------------------------------------------------
// 2010-12-13，王纯军增加
/*
功能	以第一个文件为基准，比较两个文件，并输出不一致的行
输入参数
	firstFileName	第一个文件名称
	secondFileDir	第二个文件的目录
	secondFileName	第二个文件名称
	unsameFileFp	不一致的内容
输出参数
	无
返回值
	>=0		不一致的行数
	<0		出错代码
*/
int UnionCompareFile_2ndFileWithDir(char *firstFileName,char *secondFileDir,char *secondFileName,FILE *unsameFileFp);

//---------------------------------------------------------------------------
// 2010-12-13，王纯军增加
/*
功能	以第一个文件为基准，比较两个文件，并输出不一致的行
输入参数
	firstFileDir	第一个文件的目录
	firstFileName	第一个文件名称
	secondFileDir	第二个文件的目录
	secondFileName	第二个文件名称
	unsameFileFp	不一致的内容
输出参数
	无
返回值
	>=0		不一致的行数
	<0		出错代码
*/
int UnionCompareFile_BothFileWithDir(char *firstFileDir,char *firstFileName,char *secondFileDir,char *secondFileName,FILE *unsameFileFp);

//---------------------------------------------------------------------------
// 2010-12-13，王纯军增加
/*
功能	以第一个文件为基准，比较两个文件，并输出不一致的行
输入参数
	firstFileName	第一个文件名称
	secondFileName	第二个文件名称
	unsameFileFp	不一致的内容
输出参数
	无
返回值
	>=0		不一致的行数
	<0		出错代码
*/
int UnionCompareFile(char *firstFileName,char *secondFileName,FILE *unsameFileFp);

//---------------------------------------------------------------------------
/*
功能	创建一个文件
输入参数
	oriFileName	文件名称
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
*/
int UnionCreateFile(char *oriFileName);

//---------------------------------------------------------------------------
/*
功能	拷贝一个文件
输入参数
	oriFileName	源文件名称
	desDir		目标目录
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
*/
int UnionCopyFileToSpecDir(char *oriFileName,char *desDir,char *desFileName);

/*
功能	创建目录
输入参数
	fullFileName	文件全名，包括了路径
输出参数
	无
返回值
	>=0		创建的目录数
	<0		出错代码
*/
int UnionCreateDirFromFullFileName(char *fullFileName);

/*
功能	备份一个文件
输入参数
	oriFileName	源文件名称
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
*/
int UnionBackupFileAutomatically(char *oriFileName);

//---------------------------------------------------------------------------
/* 
功能	关柄记录文件句柄
输入参数
	hdl	文件句柄
输出参数
	无
返回值
	无
*/
void UnionCloseRecFileHDLOfFileName(TUnionRecFileHDL hdl,char *fileName);
//---------------------------------------------------------------------------

/* 
功能	关柄记录文件句柄
输入参数
	hdl	文件句柄
输出参数
	无
返回值
	无
*/
void UnionCloseRecFileHDLOfFileNameWithoutLock(TUnionRecFileHDL hdl,char *fileName);

/*
功能：  删除目录中的文件
参数：
        dir[IN]         需要被删除的目录名
return:
        -1              删除失败
        0               删除成功
*/
int UnionDelDirFiles(char *dir);
//---------------------------------------------------------------------------
/*
功能	备份一个文件到暂时目录
输入参数
	oriFileName	源文件名称
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
*/
int UnionBackupFileToTmpDir(char *oriFileName);

/*
功能	从暂时目录恢复文件
输入参数
	oriFileName	源文件名称
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
*/
int UnionRestoreFileFromTmpDir(char *oriFileName);

/*
功能	拷贝一个文件
输入参数
	oriFileName	源文件名称
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
*/
int UnionCopyFile(char *oriFileName,char *desFileName);

/*
功能	重命名一个文件
输入参数
	oriFileName	源文件名称
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		文件的大小
	<0		出错代码
*/
int UnionRenameFile(char *oriFileName,char *desFileName);

//---------------------------------------------------------------------------
/*
功能	比较两个记录串的是否一致
输入参数
	recStr1		记录串1
	lenOfRecStr1	记录串的长度
	recStr2		记录串2
	lenOfRecStr2	记录串的长度
	fldName		要比较的域名
输出参数
	无
返回值
	=0		相同
	-1		第一个串比第二个串小
	=1		第一个串比第三个串大
	<0		出错代码（非-1)
*/
int UnionCompare2RecStr(char *recStr1,int lenOfRecStr1,char *recStr2,int lenOfRecStr2,char *fldName);

//---------------------------------------------------------------------------
/*
功能	从一个记录文件中删除一个记录串
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
	fldName		要删除的域
输出参数
	无
返回值
	>=0		删除的记录数
	<0		出错代码
*/
int UnionDeleteRecStrFromFileByFld(char *fileName,char *recStr,int lenOfRecStr,char *fldName);

//---------------------------------------------------------------------------
/*
功能	重命名一个记录文件
输入参数
	oriFileName	源文件名称
	desFileName	目标文件名
输出参数
	无
返回值
	>=0		记录数
	<0		出错代码
*/
int UnionRenameRecFile(char *oriFileName,char *desFileName);

//---------------------------------------------------------------------------
/*
功能	按照升序的方式在一个记录文件中插入一个记录串
条件    记录文件本身就是一个按照升序方式排列的文件
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
	fldName		根据该域来排序
	isAscOrder	排序方式，１升序，０除序
输出参数
	无
返回值
	>=0		插入成功
	<0		出错代码
*/
int UnionInsertRecStrToFileInOrderByFld(char *fileName,char *recStr,int lenOfRecStr,char *fldName,int isAscOrder);

/*
功能	从一个记录文件中读取一个记录串
输入参数
	fileName	记录文件名
	sizeOfRecStr	接收记录串的缓冲的大小
输出参数
	recStr		读取的记录串
返回值
	>=0		记录串的长度
	<0		出错代码
*/
int UnionReadRecStrFromFile(char *fileName,char *recStr,int sizeOfRecStr);

/*
功能	获得文件中所有记录的指定的域
输入参数
	fileName	文件名称
	fldName		域名
	maxNum		读出的最大数量
输出参数
	无
返回值
	>=0		读出的总数量
	<0		出错代码
*/
int UnionReadAllRecFldInFile(char *fileName,char *fldName,char fldValue[][128+1],int maxNum);

/*
功能	将一个记录串追加到记录文件
输入参数
	fileName	记录文件名
	recStr		记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
*/
#ifndef _WIN32_
int UnionAppendRecStrToFile(char *fileName,char *recStr,int lenOfRecStr);
#else
int UnionAppendRecStrToFile(char *fileName,char *recStr, int lenOfRecStr = 0);
#endif

/*
功能	将一个二进制记录串追加到记录文件
输入参数
	fileName	记录文件名
	recStr		记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
*/
int UnionAppendBinaryRecStrToFile(char *fileName,char *recStr,int lenOfRecStr);

/*
功能	将一个记录串写入到记录文件
输入参数
	fileName	记录文件名
	recStr		记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
*/
int UnionWriteRecStrIntoFile(char *fileName,char *recStr,int lenOfRecStr);

// Mary add begin, 2008-10-27
/*
功能	将一个记录串追加到记录文件，使用文件指针
输入参数
	hdl		文件指针
	recStr		记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		正确
	<0		出错代码
*/
int UnionAppendRecStrToFileUseFileHDL(TUnionRecFileHDL hdl,char *recStr,int lenOfRecStr);

/* 
功能	为写文件打开记录文件句柄
输入参数
	fileName	文件名称
输出参数
	无
返回值
	文件句柄
	NULL		出错
*/
TUnionRecFileHDL UnionOpenRecFileHDLForWrite(char *fileName);
// Mary add end, 2008-10-27

/* 
功能	为写文件打开记录文件句柄
输入参数
	fileName	文件名称
输出参数
	无
返回值
	文件句柄
	NULL		出错
*/
TUnionRecFileHDL UnionOpenRecFileHDLWithoutLock(char *fileName);

/* 
功能	打开记录文件句柄
输入参数
	fileName	文件名称
输出参数
	无
返回值
	文件句柄
	NULL		出错
*/
TUnionRecFileHDL UnionOpenRecFileHDL(char *fileName);

/* 
功能	关柄记录文件句柄
输入参数
	hdl	文件句柄
输出参数
	无
返回值
	无
*/
void UnionCloseRecFileHDL(TUnionRecFileHDL hdl);

/* 
功能	获得文件中的记录总数量
输入参数
	fileName	文件名称
输出参数
	无
返回值
	>=0		文件中的记录总数量
	<0		出错代码
*/
int UnionGetTotalRecNumInFile(char *fileName);

/* 
功能	从文件中读取下一条记录
输入参数
	hdl		文件指针
	sizeOfRecStr	接收记录的缓冲的大小
输出参数
	recStr		读取的记录
返回值
	>=0		读取的记录大小,=0,表示文件中已无后续记录
	<0		出错代码
*/
int UnionReadNextRecFromFile(TUnionRecFileHDL hdl,char *recStr,int sizeOfRecStr);

/* 
功能	从文件中读取指定索引的记录
输入参数
	hdl		文件指针
	index		记录索引号
	sizeOfRecStr	接收记录的缓冲的大小
输出参数
	recStr		读取的记录
返回值
	>=0		读取的记录大小,=0,表示文件中已无后续记录
	<0		出错代码
*/
int UnionReadRecOfIndexFromFile(TUnionRecFileHDL hdl,unsigned int index,char *recStr,int sizeOfRecStr);

//---------------------------------------------------------------------------
/* 
功能	从文件中读取指定索引的记录
输入参数
	fileName	文件名称
	index		记录索引号
	sizeOfRecStr	接收记录的缓冲的大小
输出参数
	recStr		读取的记录
返回值
	>=0		读取的记录大小,=0,表示文件中已无后续记录
	<0		出错代码
*/
int UnionReadRecOfIndexDirectlyFromFile(char *fileName,unsigned int index,char *recStr,int sizeOfRecStr);

//---------------------------------------------------------------------------
/*
功能	从一个记录文件中删除一个记录串
输入参数
	fileName	记录文件名
	recStr		读取的记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		删除的记录数
	<0		出错代码
*/
int UnionDeleteRecStrFromFile(char *fileName,char *recStr,int lenOfRecStr);

// 判断是否存在记录文件
int UnionExistsFile(char *fileName);

// 删除文件
int UnionDeleteRecFile(char *fileName);
// 删除文件
int UnionDeleteFile(char *fileName);

// 创建一个目录
int UnionCreateDir(char *dir);

// 删除一个目录
int UnionRemoveDir(char *dir);

// 检查一个目录是否存在
// 1存在，0不存在
int UnionExistsDir(char *dir);
//---------------------------------------------------------------------------
// add by chenliang, 2008-10-10
// 获取一个文件的大小
long UnionFileSize(char *fileName);

/*
  描述: 判断一个文件是否存在，不存在则建立它
  输入: fileName - 文件名; dirName - 文件路径
  输出: 无
  返回: =0 - 成功
        <0 - 失败
*/
int UnionExistAndCreateFile(char *fileName, char *dirName);

/*
  描述: 判断一个目录是否存在，不存在则建立它,
        如果路径中的某个目录不存在一起建立
  输入: dir - 目录路径
  输出: 无
  返回: =0 - 成功
        <0 - 失败
*/
int UnionExistAndCreateDir(char *dir);

/*
  描述: 判断一个目录是否存在，不存在则建立它,
        如果路径中的某个目录不存在一起建立
  输入: fullFileName - 文件全名(带路径)
  输出: 无
  返回: =0 - 成功
        <0 - 失败
*/
int UnionExistAndCreateDirFromFullFileName(char *fullFileName);


/*
功能	按照降序的方式在一个记录文件中插入一个记录串
条件    记录文件本身就是一个按照降序方式排列的文件
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		插入成功
	<0		出错代码
*/
int UnionInsertRecStrToFileInDesOrder(char *fileName,char *recStr,int lenOfRecStr);

/*
功能	按照升序的方式在一个记录文件中插入一个记录串
条件    记录文件本身就是一个按照升序方式排列的文件
输入参数
	fileName	记录文件名
	recStr		要删除的记录串
	lenOfRecStr	记录串的长度
输出参数
	无
返回值
	>=0		插入成功
	<0		出错代码
*/
int UnionInsertRecStrToFileInAscOrder(char *fileName,char *recStr,int lenOfRecStr);

/*
功能    按照升序对一个文件的内容进行排序
输入参数
	fileName	记录文件名
输出参数
	无
返回值
	>=0		排序成功
	<0		出错代码
*/
int UnionSortFileContentInAscOrder(char *fileName);

/*
功能    按照降序对一个文件的内容进行排序
输入参数
	fileName	记录文件名
输出参数
	无
返回值
	>=0		排序成功
	<0		出错代码
*/
int UnionSortFileContentInDesOrder(char *fileName);

int UnionFormFullFileName(char *dirName,char *fileName,char *fullFileName);

// add end.
//---------------------------------------------------------------------------
#endif

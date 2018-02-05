// 2008/7/21
// Wolfgang Wang

#ifndef unionWorkingDirH
#define unionWorkingDirH

// 获得一个暂时文件名
void UnionGetTempFileNameForThisProc(char *fileName);

/*
功能	获得主工作目录
	对于Unix，该目录为$UNIONMAINDIR
	对于Win，该目录为".\\"
输入参数
	无
输出参数
	workingDir	获取的主工作目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetMainWorkingDir(char *workingDir);

/*
功能	获得表记录接口定义目录
	该目录为"mainWorkingDir/tblQuery"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTBLRecInterfaceDir(char *workingDir);

/*
功能	获得tcpipSvr配置文件目录
	目录为"mainWorkingDir/server"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTcpipSvrConfDir(char *workingDir);

/*
功能	获得暂时工作目录
	目录为"mainWorkingDir/tmp"
输入参数
	无
输出参数
	workingDir	获取的暂时工作目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTempWorkingDir(char *workingDir);

/*
功能	获得仿真测试数据工作目录
	目录为"mainWorkingDir/simuData"
输入参数
	无
输出参数
	workingDir	获取的工作目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetSimuDataWorkingDir(char *workingDir);

/*
功能	获得控制定义目录
	目录为"mainWorkingDir/control"
输入参数
	无
输出参数
	workingDir	获取的控制定义目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetControlDefWorkingDir(char *workingDir);

/*
功能	获得暂时文件名称
	该文件为"mainWorkingDir/tmp/x.tmp"
输入参数
        index           暂时文件序号
输出参数
	UnionGetIconDir	获取的文件名称
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTmpFileName(unsigned int index,char *fileName);

/*
功能	获得客户端表定义目录
	该目录为"mainWorkingDir/tblConf"
输入参数
	无
输出参数
	workingDir	获取的客户端表定义工作目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetTBLQueryConfDir(char *workingDir);

/*
功能	获得菜单定义目录
	该目录为"mainWorkingDir/menu"
输入参数
	无
输出参数
	workingDir	获取的菜单定义目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetMenuDefDir(char *workingDir);

/*
功能	获得图标目录
	该目录为"mainWorkingDir/icon"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetIconDir(char *workingDir);

/*
功能	获得图标文件名称
	该目录为"mainWorkingDir/icon"
输入参数
	无
输出参数
	UnionGetIconDir	获取的文件名称
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetIconFileName(char *iconName,char *fileName);

// 程序设置暂时文件名
void UnionSetMngSvrTempFileName(char *fileName);

/*
功能	获得域值定义目录
	该目录为"mainWorkingDir/enumValueDef"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetEnumValueDefDir(char *workingDir);

/*
功能	获得域值定义目录
	该目录为"mainWorkingDir/enumValueDef"
输入参数
	无
输出参数
	workingDir	获取的目录
返回值
	>=0		正确
	<0		出错代码
*/
int UnionGetEnumValueDefFileName(char *enumType,char *fileName);

#endif

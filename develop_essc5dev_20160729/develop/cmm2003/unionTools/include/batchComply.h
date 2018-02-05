#ifndef _batchComply_
#define _batchComply_

int UnionGenerateHeaderOfBatchComplyFile(FILE *outFp);

// 根据一个文件中的makefile写一个编译批处理
/*
输入参数
	inputFile	包含了makefile清单的文件
	outputFile	生成的编译批处理文件
	mainDir		目录，只对该目录下的makefile生成批处理
输出参数
	无
返回值
	>=0		包含的makefile的数量
	<0		出错代码
*/

int UnionWriteComplyAllSrcBatchFile(char *inputFile,char *outputFile,char *mainDir);

// 根据一个文件中的makefile写一个编译批处理
/*
输入参数
	inputFile	包含了makefile清单的文件
	outputFile	生成的编译批处理文件
	mainDir		目录，只对该目录下的makefile生成批处理
输出参数
	无
返回值
	>=0		包含的makefile的数量
	<0		出错代码
*/
int UnionWriteLinkAllBinsBatchFile(char *inputFile,char *outputFile,char *mainDir);

#endif

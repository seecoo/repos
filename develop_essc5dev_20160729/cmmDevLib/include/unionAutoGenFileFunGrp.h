//	Wolfgang Wang
//	2008/2/25

#ifndef _unionAutoGenFileFunGrp_
#define _unionAutoGenFileFunGrp_

/*
功能	
	将头文件写入到测试程序中
输入参数
	incConfFile	配置了要包括的头文件的文件
	fp		测试程序句柄
输入出数
	无
返回值
	头文件的数量
*/
int UnionAddIncludeFileToFile(char *incConfFile,FILE *fp);

#endif


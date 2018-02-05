#ifndef UNION_ZIP_API
#define UNION_ZIP_API

/*
功能：将文件(可以是多个文件)压缩为指定的ZIP压缩文件
输入：
destZipFile: 压缩生成的文件名
srcFileNameStrs: 
	待压缩的文件。如果要压缩多个文件，要将每个文件用空格隔开，如有要压缩目录(下的所有文件),需在目录前加"-r"参数
	例子：UnionZip("aaa.zip","aaa bbb -r ccc"); 是将文件aaa,bbb及目录ccc(含ccc下的所有文件)压缩生成aaa.zip
*/
int UnionZip(char *destZipFile,char *srcFileNameStrs);

/*
功能: 将文件解压到指定路径
输入:
	ZipFileName: 待解压的文件名
	destDir： 解压后存放的路径
*/
int UnionUnZip(char *ZipFileName,char *destDir);

#endif


#ifndef _batchComply_
#define _batchComply_

int UnionGenerateHeaderOfBatchComplyFile(FILE *outFp);

// ����һ���ļ��е�makefileдһ������������
/*
�������
	inputFile	������makefile�嵥���ļ�
	outputFile	���ɵı����������ļ�
	mainDir		Ŀ¼��ֻ�Ը�Ŀ¼�µ�makefile����������
�������
	��
����ֵ
	>=0		������makefile������
	<0		�������
*/

int UnionWriteComplyAllSrcBatchFile(char *inputFile,char *outputFile,char *mainDir);

// ����һ���ļ��е�makefileдһ������������
/*
�������
	inputFile	������makefile�嵥���ļ�
	outputFile	���ɵı����������ļ�
	mainDir		Ŀ¼��ֻ�Ը�Ŀ¼�µ�makefile����������
�������
	��
����ֵ
	>=0		������makefile������
	<0		�������
*/
int UnionWriteLinkAllBinsBatchFile(char *inputFile,char *outputFile,char *mainDir);

#endif

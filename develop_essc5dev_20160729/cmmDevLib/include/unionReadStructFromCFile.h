//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionReadStructFromCFile_
#define _unionReadStructFromCFile_

// �ж�һ�������Ƿ���*
int UnionIsThisNameDefinedAsStarTag(char *varName);

/* ��һ���ַ����ж�ȡ�����Ķ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	prefix		������ǰ׺
	nameOfType	�����Ķ��������
	varName		�����ı���������
	isPointer	�����ı����Ƿ���ָ��
����ֵ��
	>=0 		�����Ĵ����ַ�����ռ�õĴ�С
	<0		�������	
	
*/
int UnionReadVarDefFromCProgramStr(char *str,int lenOfStr,char *prefix,char *nameOfType,char *varName,int *isPointer);

/* ��һ���ַ����ж�ȡ�ṹ��Ķ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pfldDef		����������
����ֵ��
	>=0 		�ɹ�
	<0		�������	
	
*/
int UnionReadStructFldDefFromStr(char *str,int lenOfStr,PUnionVarDef pfldDef);

/* ��ָ���ļ���ָ�����ƵĽṹ�Ķ���
�������
	fileName	�ļ�����
	structName	�ṹ����
	writeToDefFile	����д�������ļ��У�1д��0����д
	outFp		�������ļ�¼��������ļ���
�������
	pdef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������	
	
*/
int UnionReadSpecStructDefFromCHeaderFile(char *fileName,char *structName,PUnionStructDef pdef,int writeToDefFile,FILE *outFp);

/* ��ָ���ṹ�������ӡ���ļ���
�������
	fileName	�ļ�����
	structName	�ṹ����
	fp		�ļ����
	writeToDefFile	����д�������ļ��У�1д��0����д
�������
	��
����ֵ��
	>=0 	�ɹ�
	<0	�������	
	
*/
int UnionPrintSpecStructDefInCHeaderFileToFp(char *fileName,char *nameOfStructDef,FILE *fp,int writeToDefFile);

/* ��ָ���ṹ�������ӡ���ļ���
�������
	fileName	�ļ�����
	fp		�ļ����
�������
	��
����ֵ��
	>=0 	�ɹ�
	<0	�������	
	
*/
int UnionPrintUserSpecStructDefInCHeaderFileToFp(char *fileName,FILE *fp);

/* ��ָ���ļ���ָ�����ƵĽṹ�Ķ���
�������
	fileName	�ļ�����
	readName	�ṹ����
	writeToDefFile	����д�������ļ��У�1д��0����д
	outFp		�������ļ�¼��������ļ���
�������
	pdef	�����Ľṹ����
����ֵ��
	>=0 	�����Ľṹ�Ĵ�С
	<0	�������
*/
int UnionReadSpecTypeDefFromCHeaderFile(char *fileName,char *readName,int writeToDefFile,FILE *outFp);

/* ��ָ�������Ͷ����ӡ���ļ���
�������
	fileName	�ļ�����
	nameOfDef	�ṹ����
	fp		�ļ����
	writeToDefFile	����д�������ļ��У�1д��0����д
�������
	��
����ֵ��
	>=0 	�ɹ�
	<0	�������	
	
*/
int UnionPrintSpecTypeDefInCHeaderFileToFp(char *fileName,char *nameOfDef,FILE *fp,int writeToDefFile);

#endif

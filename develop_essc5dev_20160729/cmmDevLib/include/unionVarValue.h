// wolfang wang
// 2008/10/3

#ifndef _unionVarValue_
#define _unionVarValue_

#include "unionVarTypeDef.h"
#include "unionVarDef.h"

// ����ȱʡ�ַ�����������󳤶�
#ifndef conMaxDefaultSizeOfVarString	
#define conMaxDefaultSizeOfVarString	(8192+1)
#endif

#define conVarValueTagBaseTypeTag	"baseTypeTag"
#define conVarValueTagValueTag		"valueTag"

// ���������ȱʡ��С
void UnionSetDefaultArraySizeWhenNoSizeSet(int size);

// �����ӡ������ֵ��ʶ�б������
typedef enum
{
	conTagOfPrintValueTagListToFilePrintToFile = 10,			// ��������ȡֵ�嵥��ӡ���ļ���
	conTagOfPrintValueTagListToFileWriteMallocOfPointerToFile = 20,		// ������ָ������ռ������ӡ���ļ���
	conTagOfPrintValueTagListToFileWriteFreeOfPointerToFile = 21,		// ���ͷ�ָ������ռ������ӡ���ļ���
} TUnionTagOfPrintValueTagListToFile;

// ���������ֵ
typedef struct
{
	char				varValueTag[256+1];	// ������
	int				baseTypeTag;		// ����������ʶ
} TUnionVarValueTagDef;
typedef TUnionVarValueTagDef		*PUnionVarValueTagDef;

// ���ý�������ȡֵ�嵥��ӡ���ļ���ʱ����ӡ������
void UnionSetTagOfPrintValueTagListToFile(int tag);

/*
����	
	���ļ���ȡһ��ָ��ȡֵ��ʶ�ı�����ֵ
�������
	fileName	�ļ�����
�������
	pdef		�����ĺ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadValueOfSpecValueTagDefFromSpecFile(char *fileName,PUnionVarValueTagDef pdef,char *value,int sizeOfValue);

/*
����	
	��һ������ڴ��ж�ȡһ������ȡֵ��ʶ����
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadVarValueTagDefFromStr(char *str,int lenOfStr,PUnionVarValueTagDef pdef);

/*
����	
	��һ���򵥱���ȡֵд�뵽ָ���ļ���
�������
	varNamePrefix	�������Ƶ�ǰ׺
	pdef		�����Ķ���
	fp		����ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintValueTagListToFile(char *varNamePrefix,PUnionVarDef pdef,FILE *fp);

/*
����	
	��һ���������ȡֵд�뵽ָ���ļ���
�������
	varNamePrefix	�������Ƶ�ǰ׺
	pdef		�����Ķ���
	fp		����ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintArrayVarValueTagListToFile(char *varNamePrefix,PUnionVarDef pdef,int dimisionIndex,FILE *fp);

/*
����	
	��һ���������ȡֵд�뵽ָ���ļ���
�������
	varNamePrefix	�������Ƶ�ǰ׺
	isPointer	ָ���ʶ
	nameOfType	����������
	fp		����ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintArrayTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,int dimisionIndex,FILE *fp);

/*
����	
	��һ����c��������ȡֵ����д�뵽ָ���ļ���
�������
	varName		��������
	typeTag		����
	isPointer	ָ���ʶ��1��ʾ��ָ�룬0��ʾ����ָ��
	fp		����ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintCProgramSimpleTypeValueTagListToFile(char *varName,int isPointer,int typeTag,FILE *fp);

/*
����	
	��һ���ṹȡֵ����д�뵽ָ���ļ���
�������
	varNamePrefix	�������Ƶ�ǰ׺
	nameOfType	��������
	isPointer	ָ�������Ƿ���ָ��
	fp		����ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintStructTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp);

/*
����	
	��һ��������ȡֵ����д�뵽ָ���ļ���
�������
	varNamePrefix	�������Ƶ�ǰ׺
	nameOfType	��������
	fp		����ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSimpleTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp);

/*
����	
	��һ��ָ������ȡֵ����д�뵽ָ���ļ���
�������
	varNamePrefix	�������Ƶ�ǰ׺
	nameOfType	��������
	fp		����ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintPointerTypeValueTagListToFile(char *varNamePrefix,int isPointer,char *nameOfType,FILE *fp);

/*
����	
	��ӡһ����������ĸ�ֵ�б�
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintValueTagListDefinedInStrToFp(char *str,int lenOfStr,FILE *fp);

/*
����	
	��ӡһ����������ĸ�ֵ�б�
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionOutputValueTagListDefinedInStr(char *str,int lenOfStr);

/*
����	
	��һ��������ָ���������
�������
	specFldOfVar	ָ���ı�����
	varNamePrefix	�������Ƶ�ǰ׺
	pdef		�����Ķ���
�������	
	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,PUnionVarDef pdef);

/*
����	
	��һ���������ȡֵ������
�������
	specFldOfVar	ָ���ı�����
	varNamePrefix	�������Ƶ�ǰ׺
	pdef		�����Ķ���
�������	
	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadArrayVarTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,PUnionVarDef pdef,int dimisionIndex);

/*
����	
	��һ���������ȡֵ������
�������
	specFldOfVar	ָ���ı�����
	varNamePrefix	�������Ƶ�ǰ׺
	isPointer	ָ���ʶ
	nameOfType	����������
�������	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadArrayTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType,int dimisionIndex);

/*
����	
	��һ����c��������ȡֵ����������
�������
	specFldOfVar	ָ���ı�����
	varName		��������
	typeTag		����
	isPointer	ָ���ʶ��1��ʾ��ָ�룬0��ʾ����ָ��
�������	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadCProgramSimpleTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varName,int isPointer,int typeTag);

/*
����	
	��һ���ṹȡֵ����������
�������
	specFldOfVar	ָ���ı�����
	varNamePrefix	�������Ƶ�ǰ׺
	nameOfType	��������
	isPointer	ָ�������Ƿ���ָ��
�������	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadStructTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType);

/*
����	
	��һ��������ȡֵ����������
�������
	specFldOfVar	ָ���ı�����
	varNamePrefix	�������Ƶ�ǰ׺
	nameOfType	��������
�������	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadSimpleTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType);

/*
����	
	��һ��ָ������ȡֵ����������
�������
	specFldOfVar	ָ���ı�����
	varNamePrefix	�������Ƶ�ǰ׺
	nameOfType	��������
�������	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadPointerTypeTagOfSpecFldOfVar(char *specFldOfVar,char *varNamePrefix,int isPointer,char *nameOfType);

/*
����	
	��һ�������������
�������
	specFldOfVar	ָ���ı�����
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	��
����ֵ
	>=0		����
	<0		�������
*/
int UnionReadValueTagFromVarDefStrOfSpecFldOfVar(char *specFldOfVar,char *str,int lenOfStr);

#endif

// wolfang wang
// 2008/10/3

#ifndef _unionVarTypeDef_
#define _unionVarTypeDef_

#define conVarTypeDefTagTypeTag			"typeTag"
#define conVarTypeDefTagNameOfType		"nameOfType"
#define conVarTypeDefTagProgramName		"nameOfProgram"

#include "unionDesignKeyWord.h"

#ifndef conDesginKeyWordTagVarTypeOffset
#define conDesginKeyWordTagVarTypeOffset	100
#endif

#define conDesginKeyWordTagStrInt			"int"
#define conDesginKeyWordTagStrLong			"long"
#define conDesginKeyWordTagStrLong1			"long int"
#define conDesginKeyWordTagStrDouble			"double"
#define conDesginKeyWordTagStrFloat			"float"
#define conDesginKeyWordTagStrChar			"char"
#define conDesginKeyWordTagStrVoid			"void"
#define conDesginKeyWordTagStrFile			"file"
#define conDesginKeyWordTagStrFile1			"FILE"
#define conDesginKeyWordTagStrString			"string"
#define conDesginKeyWordTagStrEnum			"enum"
#define conDesginKeyWordTagStrSimpleType		"simple"
#define conDesginKeyWordTagStrStruct			"struct"
#define conDesginKeyWordTagStrUnion			"union"
#define conDesginKeyWordTagStrArray			"array"
#define conDesginKeyWordTagStrPointer			"pointer"
#define conDesginKeyWordTagStrUnsignedInt		"unsigned int"
#define conDesginKeyWordTagStrUnsignedLong		"unsigned long"
#define conDesginKeyWordTagStrUnsignedLong1		"unsigned long int"
#define conDesginKeyWordTagStrUnsignedChar		"unsigned char"
#define conDesginKeyWordTagStrFun			"fun"
#define conDesginKeyWordTagStrConst			"const"
#define conDesginKeyWordTagStrGlobalVar			"var"
#define conDesginKeyWordTagStrUnsignedString		"unsigned string"

// ��������
typedef enum
{
	conVarTypeTagInt = conDesginKeyWordTagVarTypeOffset + 1,
	conVarTypeTagLong = conDesginKeyWordTagVarTypeOffset + 2,
	conVarTypeTagDouble = conDesginKeyWordTagVarTypeOffset + 3,
	conVarTypeTagFloat = conDesginKeyWordTagVarTypeOffset + 4,
	conVarTypeTagChar = conDesginKeyWordTagVarTypeOffset + 5,
	conVarTypeTagVoid = conDesginKeyWordTagVarTypeOffset + 6,
	conVarTypeTagFile = conDesginKeyWordTagVarTypeOffset + 7,
	conVarTypeTagString = conDesginKeyWordTagVarTypeOffset + 8,
	conVarTypeTagEnum = conDesginKeyWordTagVarTypeOffset + 9,
	conVarTypeTagSimpleType = conDesginKeyWordTagVarTypeOffset + 10,
	conVarTypeTagStruct = conDesginKeyWordTagVarTypeOffset + 11,
	conVarTypeTagUnion = conDesginKeyWordTagVarTypeOffset + 12,
	conVarTypeTagArray = conDesginKeyWordTagVarTypeOffset + 13,
	conVarTypeTagPointer = conDesginKeyWordTagVarTypeOffset + 14,
	conVarTypeTagUnknown = conDesginKeyWordTagVarTypeOffset + 15,
	conVarTypeTagUnsignedInt = conDesginKeyWordTagVarTypeOffset + 16,
	conVarTypeTagUnsignedLong = conDesginKeyWordTagVarTypeOffset + 17,
	conVarTypeTagUnsignedChar = conDesginKeyWordTagVarTypeOffset + 18,
	conVarTypeTagUnsignedString = conDesginKeyWordTagVarTypeOffset + 19,
} TUnionVarTypeTag;
typedef TUnionVarTypeTag	TUnionBaseVarTypeTag;	// �������������ʶ

typedef struct
{
	char			nameOfType[64+1];	// ������������
	int			typeTag;		// �������ͱ�ʶ
	char			nameOfProgram[64+1];	// ģ������
} TUnionVarTypeDef;
typedef TUnionVarTypeDef	*PUnionVarTypeDef;

/*
����	
	��һ������ڴ��ж�ȡһ���������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadVarTypeDefFromStr(char *str,int lenOfStr,PUnionVarTypeDef pdef);

/*
����	
	��һ������ڴ��ж�ȡһ���������Ͷ���
�������
	pdef		��������
	sizeOfBuf	���崮�Ĵ�С
�������
	str		���崮
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPutVarTypeDefIntoStr(PUnionVarTypeDef pdef,char *str,int sizeOfBuf);

/*
����	
	���һ���ؼ������ڵĳ�������
�������
	keyWord		��������
�������
	programName	��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetProgramFileNameOfKeyWord(char *keyWord,char *nameOfProgram);

/*
����	
	��һ�������ڲ��ؼ��ֱ�ʶת��Ϊ��ʶֵ
�������
	keyWord		c�����Ͷ���Ĺؼ���
�������
	��
����ֵ
	>=0		ת�������ڲ���ʶ
	<0		�������
*/
int UnionConvertUnionKeyWordIntoTag(char *keyWord);

/*
����	
	��һ��c�����Ͷ���Ĺؼ���ת��Ϊ�����ڲ���ʶ
�������
	keyWord		c�����Ͷ���Ĺؼ���
�������
	��
����ֵ
	>=0		ת�������ڲ���ʶ
	<0		�������
*/
int UnionConvertCVarDefTypeIntoTag(char *keyWord);

/*
����	
	�ж�һ�������Ƿ���һ��������
�������
	nameOfType	��������
�������
	��
����ֵ
	1		��
	0		����
*/
int UnionIsBaseType(char *nameOfType);

/*
����	
	���ָ�����͵ı������ڲ���ʶ
�������
	nameOfType	��������
�������
	��
����ֵ
	�������͵��ڲ���ʶ
*/
int UnionGetTypeTagOfSpecNameOfType(char *nameOfType);

/*
����	
	���ָ�����͵ı�������ԭʼ����
�������
	oriNameOfType	��������
�������
	finalNameOfType	������������
����ֵ
	>= 0		�ɹ�
	<0		�������
*/
int UnionGetFinalTypeNameOfSpecNameOfType(char *oriNameOfType,char *finalNameOfType);

/*
����	
	�ж��Ƿ���һ��������
�������
	keyTag		���ͱ�ʶ
�������
	��
����ֵ
	1		��
	0		����
*/
int UnionIsBaseTypeTag(TUnionVarTypeTag keyTag);

/*
����	
	��һ�������ڲ���ʶת��Ϊc�����Ͷ���Ĺؼ���
�������
	keyTag		���ͱ�ʶ
�������
	keyWord		c�����Ͷ���Ĺؼ���
����ֵ
	>=0		ת�������ڲ���ʶ
	<0		�������
*/
int UnionConvertTypeTagIntoCVarDefKeyWord(TUnionVarTypeTag keyTag,char *keyWord);

/*
����	
	��һ��c�����Ͷ���Ĺؼ���ת��Ϊ�����ڲ���ʶ
�������
	keyWord		c�����Ͷ���Ĺؼ���
�������
	��
����ֵ
	>=0		ת�������ڲ���ʶ
	<0		�������
*/
int UnionConvertCVarDefKeyWordIntoTag(char *keyWord);

/*
����	
	��һ������ڴ��ж�ȡһ���������Ͷ���
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadVarTypeDefFromStr(char *str,int lenOfStr,PUnionVarTypeDef pdef);

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
	nameOfType	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadVarTypeDefFromSpecFile(char *fileName,char *nameOfType,PUnionVarTypeDef pdef);

/* ��ȱʡ�ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	nameOfType	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadVarTypeDefFromDefaultFile(char *nameOfType,PUnionVarTypeDef pdef);

/* ��ָ�����ͱ�ʶ�Ķ����ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintVarTypeDefToFp(PUnionVarTypeDef pdef,FILE *fp);

/* ��ָ�����ͱ�ʶ�Ķ����Զ����ʽ��ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintVarTypeDefToFpInDefFormat(PUnionVarTypeDef pdef,FILE *fp);

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
	nameOfType	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintVarTypeDefInFileToFp(char *fileName,char *nameOfType,FILE *fp);
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
	nameOfType	��������
�������
	��
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputVarTypeDefInFile(char *nameOfType,char *fileName);
#endif

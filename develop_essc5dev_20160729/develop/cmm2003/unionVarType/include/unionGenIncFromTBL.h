//	Wolfgang Wang
//	2009/4/29

#ifndef _unionGenIncFromTBL_
#define _unionGenIncFromTBL_

#define conDataTBLListFileName		"unionDataTBLList"
#define conDataTBLListTBLName		"tableList"

// ����ļ��Ѵ��ڵĲ�������
#define conOverwriteModeWhenFileExistsOverwrite		1	//����
#define conOverwriteModeWhenFileExistsReturn		0	//��д������
#define conOverwriteModeWhenFileExistsUserConfirm	3	//�ȴ��û�ȷ��

#include "unionComplexDBCommon.h"

/*
����	
	���ݱ��壬����һ��ͷ�ļ�
�������
	tblName			����
	userSpecFileName	�û�ָ�����ļ���
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateVersionSpecConstDefAndStructDefFromTBLDefToSpecFile(char *tblName,char *userSpecFileName,int modeWhenFileExists);

/*
����	
	���ݱ�������ñ�ṹ����ͷ�ļ�·��
�������
	tblName		����
�������
	fullPath	ͷ�ļ�·��
����ֵ
	��
*/
void UnionFormFullPathOfTableDefStructHeadFileName(char *tblName,char *fullPath);

/*
����	
	���ݱ��������Ŀ���ļ���
�������
	tblName		����
�������
	fileName	�ļ���
����ֵ
	��
*/
void UnionFormObjFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName);

/*
����	
	���ݱ��������Ŀ���ļ���(��ȫ·��)
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormFullObjFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName);

/*
����	
	���ݱ��壬����һ��makefile�ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMakeFileFromTBLDefToFp(char *tblName,FILE *fp);

/*
����	
	���ݱ��������makefile�ļ���
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormFullMakeFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName);

/*
����	
	���ݱ��壬����һ��makefile�ļ�
�������
	tblName			����
	fileName		�ļ�����
	modeWhenFileExists	�ļ�����ʱ��д����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMakeFileFromTBLDefToSpecFile(char *tblName,char *fileName,int modeWhenFileExists);

/*
����	
	���ݱ��������makefile�ļ���
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormMakeFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName);

/*
����	
	���ݱ�������ñ������
�������
	tblName		����
�������
	pobjDef		�������
����ֵ
	��
*/
int UnionReadObjectDefFromDesignDoc(char *tblName,PUnionObject pobjDef);

/*
����	
	���ݱ�������ñ�ṹ����ͷ�ļ�ȫ������·��)
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormFullTableDefStructHeadFileName(char *tblName,char *incFileName);


/*
����	
	���ݱ�������ñ��ͷ�ļ���
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormTableDefStructHeadFileName(char *tblName,char *incFileName);

/*
����	
	���ݱ�������õ�ǰ�ṹ�汾��ʶ��
�������
	tblName		����
	version		�汾
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormStructTagNameOfCurrentVersionTBLDefFromTBLName(char *tblName,char *incFileName);

/*
����	
	���ݱ�������ýṹ�汾��ʶ��
�������
	tblName		����
	version		�汾
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormStructTagNameOfSpecVersionTBLDefFromTBLName(char *tblName,char *version,char *incFileName);

/*
����	
	���ݱ������ͺ궨������ƴװһ���궨����
�������
	tblName		����
	macroTag	���ʶ
�������
	macroName	������
����ֵ
	��
*/
void UnionFormMacroNameFromTBLName(char *tblName,char *macroTag,char *macroName);

/*
����	
	���ݱ�������ó�����
�������
	tblName		����
�������
	funName		������
����ֵ
	��
*/
void UnionFormTBLProgramNameFromTBLName(char *tblName,char *programName);

/*
����	
	���ݱ��������ͷ�ļ���
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormIncFileNameOfTBLProgramFromTBLName(char *tblName,char *incFileName);

/*
����	
	���ݱ��������ͷ�ļ���(����·��)
�������
	tblName		����
�������
	funName		ͷ�ļ���
����ֵ
	��
*/
void UnionFormFullIncFileNameOfTBLProgramFromTBLName(char *tblName,char *incFileName);


/*
����	
	���ݱ��������C�ļ���(����·���Ͱ汾)
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormFullCFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName);

/*
����	
	���ݱ�������ñ�����Ӧ�ĺ궨������
�������
	tblName		����
�������
	funName		�궨����
����ֵ
	��
*/
void UnionFormTBLNameConstNameFromTBLName(char *tblName,char *constName);

/*
����	
	���ݱ�������ñ���Դ��Ӧ�ĺ궨������
�������
	tblName		����
�������
	funName		�궨����
����ֵ
	��
*/
void UnionFormTBLResIDConstNameFromTBLName(char *tblName,char *constName);

/*
����	
	���ݱ�����ƴװһ������¼�ṹת��Ϊ��¼�ִ��ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ������¼�ṹ�еĹؼ���ת��Ϊ��¼�ִ��ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertPrimaryKeyInStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ������¼�ṹ�еķǹؼ���ת��Ϊ��¼�ִ��ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertNonPrimaryKeyInStructIntoRecStrFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ�������¼�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormInsertRecFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ����ȡ��¼�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormReadRecFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ���޸ļ�¼�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormUpdateRecFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ��ɾ����¼�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormDeleteRecFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ���޸ļ�¼ָ����ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormUpdateSpecFldRecFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ���޸ļ�¼ָ��������ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormUpdateIntTypeSpecFldRecFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ������¼�ִ�ת��Ϊ��¼�ṹ�ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertRecStrIntoStructFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ������¼�ִ�ת��Ϊ��¼�ṹ�Ĺؼ��ֵĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormConvertRecStrIntoStructPrimaryKeyFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ�����ȱʡ��ֵ��������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormStructDefaultValueSetFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ�����������ȡ���ݼ�¼��������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormBatchReadRecFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ����������ƺ궨����
�������
	tblName		����
	fldName		����
�������
	fldDefName	�����ƺ궨����
����ֵ
	��
*/
void UnionFormStructFldDefNameFromTBLName(char *tblName,char *fldName,char *fldDefName);

/*
����	
	���ݱ�����ƴװһ���ṹ����
�������
	tblName		����
�������
	structName	�ṹ����
����ֵ
	��
*/
void UnionFormStructNameFromTBLName(char *tblName,char *structName);

/*
����	
	���ݱ�����ƴװһ���ṹ����
�������
	tblName		����
	isPointer	�Ƿ���Ϊָ�룬1�ǣ�0��
�������
	structDefName	�ṹ����
����ֵ
	��
*/
void UnionFormStructDefNameFromTBLName(char *tblName,int isPointer,char *structDefName);

/*
����	
	���ݱ��壬����һ�����ݽṹ����
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateStructDefFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬�ж��Ƿ�ҪΪһ������궨��
�������
	tblName		����
�������
	��
����ֵ
	1	��Ҫ
	0	����Ҫ
	<0	������
*/
int UnionExistsMacroDefInTBL(char *tblName);

/*
����	
	���ݱ��еļ�¼������꣬һ����¼��Ӧһ���궨��
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateConstDefFromTBL(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ�����ݽṹȱʡ��ֵ����������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateStructDefaultValueSetFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����������ȡ��¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateBatchReadRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ�����ݽṹȱʡ��ֵ����
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateStructDefaultValueSetFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ��������ȡ��¼�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateBatchReadRecFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ������¼�ṹת��Ϊ��¼�ִ��ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ������¼�ṹ�йؼ���ת��Ϊ��¼�ִ��ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ������¼�ṹ�зǹؼ���ת��Ϊ��¼�ִ��ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ������¼�ṹת��Ϊ��¼�ִ��ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ������¼��ת��Ϊ��¼�ṹ�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertRecStrIntoStructFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ������¼��ת��Ϊ��¼�ṹ�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertRecStrIntoStructFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ��������м�¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateInsertRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ������¼������еĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateInsertRecFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ��ɾ�����м�¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateDeleteRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ������¼ɾ�����еĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateDeleteRecFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ���޸ı��м�¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateUpdateRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ������¼�޸ı��еĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateRecFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ����ȡ���м�¼�ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateSelectRecFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ����ȡ���м�¼�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSelectRecFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	�����Զ����ɵĴ����ʶ
�������
	��
�������
	��
����ֵ
	��
*/
void UnionGenerateAutoCodesTagToFp(FILE *fp);
	
/*
����	
	���ݱ��壬���������ͽṹ�����ͷ�ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/

int UnionGenerateVersionSpecConstDefAndStructDefFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ��ͷ�ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateIncFileFromTBLDefToFp(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ��ͷ�ļ�
�������
	tblName			����
	userSpecFileName	�û�ָ�����ļ���
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateIncFileFromTBLDef(char *tblName,char *userSpecFileName,int overwriteExists);

/*
����	
	���ݱ��壬����һ��c�ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateCFileFromTBLDefToFp(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ��C�ļ�
�������
	tblName			����
	userSpecFileName	�û�ָ�����ļ���
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateCFileFromTBLDef(char *tblName,char *userSpecFileName,int overwriteExists);

/*
����	
	���ݱ��壬����һ��C�ļ���ͷ�ļ�
�������
	tblName			����
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateIncFileAndCFileFromTBLDef(char *tblName,int overwriteExists);

/*
����	
	��ñ��嵥����ͷ�ļ�������
�������
	��
�������
	fileName	�ļ���
����ֵ
	>=0	�ɹ�
	<0	������
*/
void UnionGetIncFileNameOfTBLListDef(char *fileName);

/*
����	
	���ݱ��嵥���еļ�¼���������嵥����ͷ�ļ�
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateTBLListDefFromTableListTBLToFp(FILE *fp);

/*
����	
	���ݱ��嵥���еļ�¼���������嵥����ͷ�ļ�
�������
	��
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateTBLListDefFromTableListTBL();

/*
����	
	���ݱ��嵥���еļ�¼���������б�Ĳ����ļ�
�������
	modeWhenFileExists	����ļ��Ѵ��ڵĲ�������
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateSrcForAllTBLFromTableListTBL(int modeWhenFileExists);

/*
����	
	���ݱ��еļ�¼������enum��һ����¼��Ӧһ��enumֵ
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateEnumDefFromTBL(char *tblName,FILE *fp);

/*
����	
	���ݱ��еļ�¼������enum��һ����¼��Ӧһ��enumֵ
�������
	tblName		����
	fileName	�ļ���
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateEnumDefFromTBLToSpecFile(char *tblName,char *fileName);

/*
����	
	���ݱ��壬����һ���޸�ָ����ĺ���������
�������
	tblName		����
	isDeclaration	1��ʾ��������0��ʾ�Ǻ�����
	funName		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateSpecFldFunDeclarationFromTBLDef(char *tblName,int isDeclaration,char *funName,FILE *fp);

/*
����	
	���ݱ��壬����һ�����ݽṹȱʡ��ֵ����
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateSpecFldFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ���޸�ָ����ĺ���������
�������
	tblName		����
	isDeclaration	1��ʾ��������0��ʾ�Ǻ�����
	funName		��������
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateIntTypeSpecFldFunDeclarationFromTBLDef(char *tblName,int isDeclaration,char *funName,FILE *fp);

/*
����	
	���ݱ��壬����һ�����ݽṹȱʡ��ֵ����
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateUpdateIntTypeSpecFldFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ�����ƴװһ����ȡ��¼����ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormReadRecFldFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ�����ƴװһ����ȡ��¼����ĺ�������
�������
	tblName		����
�������
	funName		��������
����ֵ
	��
*/
void UnionFormReadRecIntTypeFldFunNameFromTBLName(char *tblName,char *funName);

/*
����	
	���ݱ��壬����һ����ȡ���м�¼����ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateSelectRecFldFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ����ȡ���м�¼�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSelectRecFldFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ����ȡ���м�¼����ĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateSelectRecIntTypeFldFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ����ȡ���м�¼�ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateSelectRecIntTypeFldFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݲ����嵥���еļ�¼��������������ͷ�ļ�
�������
	fp		�ļ����
�������
	��
����ֵ
	>=0	����Ŀ
	<0	������
*/
int UnionGenerateTBLCmdDefFromTBLDefToFp(FILE *fp);

/*
����	
	��ñ��������ͷ�ļ�������
�������
	��
�������
	fileName	�ļ���
����ֵ
	>=0	�ɹ�
	<0	������
*/
void UnionGetIncFileNameOfTBLCmdDef(char *fileName);

/*
����	
	���ݲ���������еļ�¼��������������ͷ�ļ�
�������
	��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateTBLCmdDefFromTBLDef();

/*
����	
	���ݱ��壬����һ������¼�ṹ�йؼ���ת��Ϊ��¼�ִ��ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertPrimaryKeyInStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ������¼�ṹ�еķǹؼ���ת��Ϊ��¼�ִ��ĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertNonPrimaryKeyInStructIntoRecStrFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��壬����һ������¼��ת��Ϊ��¼�ṹ�Ĺؼ��ֵĺ���������
�������
	tblName		����
	includeFunBody	�Ƿ���������壬1,������0������
	fp		�ļ����
�������
	funName		�����ĺ�������
����ֵ
	��
*/
void UnionGenerateConvertRecStrIntoStructPrimaryKeyFunDeclarationFromTBLDef(char *tblName,int includeFunBody,char *funName,FILE *outFp);

/*
����	
	���ݱ��壬����һ���Ӽ�¼���ж���¼�ؼ��ֵĺ���
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateConvertRecStrIntoStructPrimaryKeyFunFromTBLDef(char *tblName,FILE *fp);

/*
����	
	���ݱ��������makefile�ļ���ִ�нű��ļ�
�������
	tblName		����
	fp		�ļ����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMakeFileExcuteScriptFromTBLDefToFp(char *tblName,FILE *fp);

/*
����	
	���ݱ��������makefile�ļ���ִ�нű��ļ���
�������
	tblName		����
�������
	funName		c�ļ���
����ֵ
	��
*/
void UnionFormFullMakeFileExcuteScriptFileNameOfTBLProgramFromTBLName(char *tblName,char *cFileName);

/*
����	
	���ݱ��������makefile�ļ���ִ�нű��ļ���
�������
	tblName			����
	specFileName		�ļ�����
	modeWhenFileExists	�ļ�����ʱ��д����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionGenerateMakeFileExcuteScriptFileFromTBLDefToSpecFile(char *tblName,char *specFileName,int modeWhenFileExists);

/*
����	
	���ݱ��������makefile�ļ���ִ�нű��ļ���
�������
	tblName		����
�������
	fileName	�ļ���
����ֵ
	��
*/
void UnionFormMakeFileExcuteScriptFileNameOfTBLProgramFromTBLName(char *tblName,char *fileName);

/*
����	
	�Զ�������ݱ��Զ����ɵ�Դ����
�������
	tblName			����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionComplyAutoGeneratedCFileFromTBLDef(char *tblName);

#endif

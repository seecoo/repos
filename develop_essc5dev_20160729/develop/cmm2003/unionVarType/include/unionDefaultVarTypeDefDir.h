// wolfang wang
// 2008/10/3

#ifndef _unionDefaultVarTypeDefDir_
#define _unionDefaultVarTypeDefDir_

// �ؼ��ֵ�ȱʡĿ¼
#define conDefaultDesignDirOfBaseDef		"baseDef"
// ���������ȱʡĿ¼
#define conDefaultDesignDirOfDataStruct		"dataStruct"
// ���������ȱʡĿ¼
#define conDefaultDesignDirOfConst		"const"
// ���������ȱʡĿ¼
#define conDefaultDesignDirOfFun		"fun"
// ȫ�ֱ��������ȱʡĿ¼
#define conDefaultDesignDirOfVariable		"variable"
// ͷ�ļ������Ŀ¼
#define conDefaultDesignDirOfIncludeConf	"includeConf"

/*
����	
	���ָ��ģ��Ķ����ļ�
�������
	moduleName	ģ������
�������
	fileName	�ļ�����
����ֵ
	��
*/
void UnionGetDefaultFileNameOfModuleDef(char *moduleName,char *fileName);
/*
����	
	���ָ������Ķ����ļ�
�������
	moduleName	��������
�������
	fileName	�ļ�����
����ֵ
	��
*/
void UnionGetDefaultFileNameOfProgramDef(char *programName,char *fileName);

/*
����	
	��ȡ������ͷ�ļ������ļ�
�������
	oriFileName	ͷ�ļ������ļ�
�������
	fullFileName	������ͷ�ļ������ļ�
����ֵ
	ͷ�ļ�������
*/
void UnionGetFullNameOfIncludeFileConf(char *oriFileName,char *fullFileName);

/*
����	
	��ȡ�̶�����Ķ����ļ�
�������
	��
�������
	fileName	����̶�����Ķ����ļ�
����ֵ
	>=0		��ȷ
	<0		������
*/
int UnionFileNameOfFixedCodesConfFile(char *filename);

/*
����	
	���ָ��ȫ�ֱ��������ļ�
�������
	funName		��������
�������
	fileName	�ļ�����
����ֵ
	���������ļ���
*/
void UnionGetDefaultFileNameOfVariableDef(char *funName,char *fileName);

/*
����	
	���ñ������Ͷ���Ŀ¼
�������
	dir		Ŀ¼
�������
	��
����ֵ
	>=0		�ɹ�
	=		�������
*/
int UnionSetDefaultDesignDir(char *dir);

/*
����	
	��ñ������Ͷ���Ŀ¼
�������
	��
�������
	��
����ֵ
	��������Ŀ¼
*/
char *UnionGetDefaultDesignDir();

/*
����	
	���ָ���������ͱ�ʶ�����ļ�
�������
	varTypeName	������������
�������
	fileName	�ļ�����
����ֵ
	��������Ŀ¼
*/
void UnionGetDefaultFileNameOfVarTypeTagDef(char *varTypeName,char *fileName);

/*
����	
	���ָ���������Ͷ����ļ�
�������
	varTypeName	������������
�������
	fileName	�ļ�����
����ֵ
	��������Ŀ¼
*/
void UnionGetDefaultFileNameOfVarTypeDef(char *varTypeName,char *fileName);

/*
����	
	���ָ�����������ļ�
�������
	constName	��������
�������
	fileName	�ļ�����
����ֵ
	���������ļ���
*/
void UnionGetDefaultFileNameOfConstDef(char *constName,char *fileName);

#endif

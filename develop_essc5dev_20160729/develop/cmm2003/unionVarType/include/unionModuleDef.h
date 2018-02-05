// wolfang wang
// 2008/10/3

#ifndef _unionModuleDef_
#define _unionModuleDef_

#define conModuleDefTagNameOfModule	"nameOfModule"
#define conModuleDefTagDevDir		"devDir"
#define conModuleDefTagRemark		"remark"
#define conModuleDefTagIncFileConf	"incFileConf"

typedef struct
{
	char			nameOfModule[64+1];		// ģ������
	char			devDir[128+1];			// ����Ŀ¼
	char			incFileConf[64+1];		// ������ģ�������ͷ�ļ����ļ�
	char			remark[128+1];
} TUnionModuleDef;
typedef TUnionModuleDef	*PUnionModuleDef;

/*
����	
	���һ��ģ��Ŀ���Ŀ¼��ͷ�ļ�����
�������
	nameOfModulde	ģ������
�������
	devDir		����Ŀ¼
	incFileConf	ͷ�ļ�����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetDevDirAndIncFileConfOfModule(char *nameOfModule,char *devDir,char *incFileConf);

/*
����	
	��һ������ڴ��ж�ȡһ��ģ�鶨��
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadModuleDefFromStr(char *str,int lenOfStr,PUnionModuleDef pdef);

/* ��ָ�����ͱ�ʶ�Ķ����ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintModuleDefToFp(PUnionModuleDef pdef,FILE *fp);

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
nameOfModulee	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintModuleDefInFileToFp(char *fileName,char *nameOfModule,FILE *fp);
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
nameOfModulee	��������
�������
	��
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputModuleDefInFile(char *nameOfModule,char *fileName);
#endif

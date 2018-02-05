// wolfang wang
// 2008/10/3

#ifndef _unionProgramDef_
#define _unionProgramDef_

#define conProgramDefTagNameOfProgram	"nameOfProgram"
#define conProgramDefTagNameOfModule	"nameOfModule"
#define conProgramDefTagVersion		"version"
#define conProgramDefTagRemark		"remark"

typedef struct
{
	char			nameOfProgram[64+1];		// ��������
	char			nameOfModule[64+1];		// ģ������
	char			version[128+1];			// ����Ŀ¼
	char			remark[128+1];
} TUnionProgramDef;
typedef TUnionProgramDef	*PUnionProgramDef;

/*
����	
	���һ������������ģ������
�������
	nameOfProgram	��������
�������
	nameOfModule	ģ������
	version		�汾
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetModuleNameAndVersionOfProgram(char *nameOfProgram,char *nameOfModule,char *version);

/*
����	
	��һ������ڴ��ж�ȡһ��������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadProgramDefFromStr(char *str,int lenOfStr,PUnionProgramDef pdef);

/* ��ָ�����ͱ�ʶ�Ķ����ӡ���ļ���
�������
	pdef	���ͱ�ʶ����
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintProgramDefToFp(PUnionProgramDef pdef,FILE *fp);

/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ���ļ���
�������
	fileName	�ļ�����
nameOfPrograme	��������
�������
	fp		�ļ����
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionPrintProgramDefInFileToFp(char *fileName,char *nameOfProgram,FILE *fp);
	
/* ��ָ���ļ��ж�������ͱ�ʶ��ӡ����Ļ��
�������
	pdef	���ͱ�ʶ����
nameOfPrograme	��������
�������
	��
����ֵ��
	>=0 	��ӡ��������
	<0	�������	
	
*/
int UnionOutputProgramDefInFile(char *nameOfProgram,char *fileName);
#endif

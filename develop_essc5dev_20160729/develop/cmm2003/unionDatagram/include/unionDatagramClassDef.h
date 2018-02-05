// wolfang wang
// 2008/10/3

#ifndef _unionDatagrmClassDef_
#define _unionDatagrmClassDef_

#define conDatagramClassDefTagClassID		"classID"
#define conDatagramClassDefTagRemark		"remark"
#define conDatagramClassDefTagAppID		"idOfApp"
#define conDatagramClassDefTagNameOfModuleOfSvr	"nameOfModuleOfSvr"

// ���屨�ĵ����
#define conDatagramClassTagUnionCmmPack	"unionCmmPack"
#define conDatagramClassTagISO8583	"ISO-8583"

// һ����;�ı��Ĳ��õ�ʵ������
typedef struct
{
	char		appID[64+1];			// ���ĵ���;��ʶ
	char		classID[64+1];			// ���õı�������
	char		remark[128+1];			// ˵��
	char		nameOfModuleOfSvr[64+1];	// Svr��ģ��
} TUnionDatagramClassDef;
typedef TUnionDatagramClassDef		*PUnionDatagramClassDef;

/*
����	
	���Ӧ�ñ������Ͷ�����ļ���
�������
	��
�������
	fileName	��õ�Ӧ�ñ������Ͷ�����ļ�����
����ֵ
	��
*/
void UnionGetFileNameOfDatagramClassDef(char *fileName);

/*
����	
	���Ӧ�ñ������Ͷ�Ӧ�ķ����ģ���ʶ
�������
	appID			Ӧ�ñ�ʶ
�������
	nameOfModuleOfSvr	�����ģ���ʶ
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGetNameOfModuleSvrOfSpecDatagramClass(char *appID,char *nameOfModuleOfSvr);

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
int UnionReadDatagramClassDefFromStr(char *str,int lenOfStr,PUnionDatagramClassDef pdef);

/* ָ�����Ƶı������ͱ�ʶ�Ķ���
�������
	appID	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadDatagramClassDefFromFile(char *appID,PUnionDatagramClassDef pdef);


/*
����	
	��ӡһ�����ĵ����Ͷ���
�������
	pdef		Ҫ��ӡ�ı������Ͷ���
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintDatagramClassDefToFp(PUnionDatagramClassDef pdef,FILE *fp);
/*
����	
	��ӡһ���������͵Ķ���
�������
	appID		������;��ʶ
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecDatagramClassDefToFp(char *appID,FILE *fp);

/*
����	
	��ӡһ���������͵Ķ���
�������
	appID		������;��ʶ
	fileName	��������ļ�
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecDatagramClassDef(char *appID,char *fileName);

#endif

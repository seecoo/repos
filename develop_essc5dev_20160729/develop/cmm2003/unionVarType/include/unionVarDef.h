// wolfang wang
// 2008/10/3

#ifndef _unionVarDef_
#define _unionVarDef_

#define conVarDefTagTypePrefix		"typePrefix"
#define conVarDefTagVarName		"name"
#define conVarDefTagNameOfType		"nameOfType"
#define conVarDefTagIsPointer		"isPointer"
#define conVarDefTagDimisionNum		"dimisionNum"
#define conVarDefTagSizeOfDimision	"sizeOfDimision"
#define conVarDefTagRemark		"remark"

#define conMaxNumOfArrayDimision	4	// ��������ά��
// ��������
typedef struct
{
	char		typePrefix[32+1];	// ��������ǰ׺ union/struct��
	char		nameOfType[64+1];	// ��������
	int		isPointer;		// 1����ָ�룻0����ָ��
	char		name[64+1];		// ��������
	int		dimisionNum;		// �����ά�ȣ�0��ʾ��������
	char		sizeOfDimision[conMaxNumOfArrayDimision][64+1];	// ÿһά�ȵĴ�С
	char		remark[80+1];
} TUnionVarDef;
typedef TUnionVarDef	*PUnionVarDef;

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
int UnionReadVarDeclareTypeDefFromStr(char *str,int lenOfStr,PUnionVarDef pdef);


/*
����	
	��һ������ڴ��ж�ȡһ����������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	pdef		��������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadVarDefFromStr(char *str,int lenOfStr,PUnionVarDef pdef);

/*
����	
	��ӡһ�����������Ͷ���
�������
	pdef			Ҫ��ӡ�ı�������
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarTypeDeclareCProgramFormatToFp(PUnionVarDef pdef,FILE *fp);

/*
����	
	��ӡһ���������壬�����������Ĵ�Сδ���壬��ʹ��ȱʡ��С
�������
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefSetArraySizeToFp(PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
����	
	�������Ĳ������壬�Ա����������ʽ��ӡ���ļ���
�������
	prefixBlankNum			������ǰ׺�Ŀո���
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefToFpOfCProgramWithPrefixBlank(int prefixBlankNum,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
����	
	��ӡһ���������壬�ڱ�����ǰ������ǰ׺�������������Ĵ�Сδ���壬��ʹ��ȱʡ��С
�������
	prefix				����ǰ׺
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefWithPrefixSetArraySizeToFp(char *prefix,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
����	
	��ӡһ����������
�������
	pdef			Ҫ��ӡ�ı�������
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefToFp(PUnionVarDef pdef,int inCProgramFormat,FILE *fp);

/*
����	
	�������Ĳ������壬�Ա����������ʽ��ӡ���ļ���
�������
	prefix				����ǰ׺
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefToFpOfCProgram(PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
����	
	�������Ĳ������壬�Ա����������ʽ��ӡ���ļ��У��ڱ���������ǰ��ǰ׺
�������
	pdef				Ҫ��ӡ�ı�������
	defaultName			ȱʡ�����֣����δ�������֣�������������
	defaultArraySizeWhenSizeNotSet	ȱʡ�������С����������Сδ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefWithPrefixToFpOfCProgram(char *prefix,PUnionVarDef pdef,char *defaultName,int defaultArraySizeWhenSizeNotSet,FILE *fp);

/*
����	
	��ӡһ����������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
�������
	fp		������ļ�ָ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintVarDefInStrDefToFp(char *str,int lenOfStr,int inCProgramFormat,FILE *fp);

/*
����	
	��ӡһ����������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	inCProgramFormat	1��ʹ��c���Զ����ʽ�����0����ʹ��c���Զ������
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionOutputVarDefInStrDef(char *str,int lenOfStr,int inCProgramFormat);

#endif

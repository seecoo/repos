//	Wolfgang Wang
//	2008/10/08

#ifndef _unionPackAndFunRelation_
#define _unionPackAndFunRelation_

#define conPackFldAndFunVarRelationTagPackFldType	"fldType"
#define conPackFldAndFunVarRelationTagPackFldID		"packFldID"
#define conPackFldAndFunVarRelationTagVarValueTag	"varValueTag"
#define conPackFldAndFunVarRelationTagVarNameOfReaLen	"varNameOfRealLen"
#define conPackFldAndFunVarRelationTagRemark		"remark"
#define conPackFldAndFunVarRelationTagIsOptional	"isOptional"
#define conPackFldAndFunVarRelationTagIsPackToFun	"isPackToFun"
#define conPackAndFunRelatioTagFunAndPack		"funAndPack="	// �����뱨�Ķ�Ӧ��ϵ
#define conPackAndFunRelatioTagPackUsedByFun		"packUsedByFun="	// �����뱨�Ķ�Ӧ��ϵ
#define conPackAndFunRelatioTagServiceID		"serviceID"		

// ����һ���������������Ķ�Ӧ��ϵ
typedef struct
{
	char	packFldID[64+1];	// ������ı�ʶ
	char	varValueTag[128+1];	// ����ֵ��ʶ
	char	remark[128+1];		// ˵��
	int	isPackToFun;		// 1,�Ǳ����򵽺����������;0�Ǻ������뵽������
	char	varNameOfRealLen[64+1];	// ָʾʵ�ʳ��ȵı�����
	char	fldType[128+1];		// �������
	int	isOptional;		// �Ƿ��ǿ�ѡ��
} TUnionPackFldAndFunVarRelation;
typedef TUnionPackFldAndFunVarRelation	*PUnionPackFldAndFunVarRelation;

// ���屨�ĺͺ����Ķ�Ӧ��ϵ
#define conMaxNumOfPackFldAndFunVarRelation	64
typedef struct
{
	char				serviceID[3+1];				// �������
	char				relationNum;				// ת����ϵ������
	TUnionPackFldAndFunVarRelation	relationGrp[conMaxNumOfPackFldAndFunVarRelation];
} TUnionPackAndFunRelation;
typedef TUnionPackAndFunRelation	*PUnionPackAndFunRelation;

/*
����	
	���һ�����ĺͺ�����Ӧ��ϵ��Ӧ�ķ������
�������
	appID		���ĵ����
	datagramID	���ĵı�ʶ
�������
	fileName	��õı��Ķ�����ļ�����
����ֵ
	>= 0	�ɹ�
	<0	�������
*/
int UnionGetServiceIDFromPackAndFunRelationDef(char *funName,char *serviceID);

/*
����	
	���һ�����Ķ�����ļ���
�������
	appID		���ĵ����
	datagramID	���ĵı�ʶ
�������
	fileName	��õı��Ķ�����ļ�����
����ֵ
	��
*/
void UnionGetFileNameOfPackAndFunRelationDef(char *funName,char *fileName);

/*
����	
	��һ�����ݴ��ж�ȡһ���������뺯��������Ӧ��ϵ����
�������
	str		���崮
	lenOfStr	������
�������
	pdef		�����ı������뺯��������Ӧ��ϵ����
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadPackFldAndFunVarRelationFromStr(char *str,int lenOfStr,PUnionPackFldAndFunVarRelation pdef);
/*
����	
	�ӱ��ж�ȡһ�������뺯����ת����ϵ
�������
	funName		��������
	datagramID	���ĵı�ʶ
�������
	pdef		�����ı��Ķ���
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadSpecPackAndFunRelationFromFile(char *funName,PUnionPackAndFunRelation pdef);
/*
����	
	��ӡһ���������뺯������ת����ϵ
�������
	pdef		Ҫ��ӡ�ı������뺯��������Ӧ��ϵ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintPackFldAndFunVarRelationToFp(PUnionPackFldAndFunVarRelation pdef,FILE *fp);

/*
����	
	��ӡһ�������뺯����ת����ϵ
�������
	pdef		Ҫ��ӡ�ı��Ķ���
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintPackAndFunRelationToFp(PUnionPackAndFunRelation pdef,FILE *fp);

/*
����	
	��ӡһ�������뺯����ת����ϵ
�������
	funName		��������
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecPackAndFunRelationToFp(char *funName,FILE *fp);

/*
����	
	��ӡһ�������뺯����ת����ϵ��ָ���ļ�
�������
	funName		��������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecPackAndFunRelationToSpecFile(char *funName,char *fileName);

#endif


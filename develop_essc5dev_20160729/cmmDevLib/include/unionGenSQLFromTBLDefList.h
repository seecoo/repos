//	Wolfgang Wang
//	2009/4/29

#ifndef _unionGenSQLFromTBLDefList_
#define _unionGenSQLFromTBLDefList_

#define	conMaxNumOfChildrenTBL	32
#define	conMaxNumOfParentsTBL	32

typedef struct TUnionTBLDef
{
	int	created;	// 1,��ʾ�ѽ�����0��ʾδ����
	int	storeWay;	// �洢��ʽ
	char	name[40+1];	// ����
	int	childrenNum;	// �ӱ���Ŀ
	struct	TUnionTBLDef *children[conMaxNumOfChildrenTBL];	// �ӱ�
	int	parentNum;	// ������Ŀ
	struct	TUnionTBLDef *parents[conMaxNumOfParentsTBL];	// ����
} TUnionTBLDef;
typedef TUnionTBLDef	*PUnionTBLDef;


#define conMaxNumOfTBLDefPerGrp	1024
typedef struct TUnionTBLDefGrp
{
	int		num;
	int		relationsNum;
	int		dbTBLNum;
	PUnionTBLDef	ptblDefGrp[conMaxNumOfTBLDefPerGrp];
} TUnionTBLDefGrp;
typedef TUnionTBLDefGrp	*PUnionTBLDefGrp;

/*
����	
	��ʼ��������
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionResetTBLDefGrp(PUnionTBLDefGrp ptblDefGrp);

/*
����	
	�ͷű�������
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	��
*/
void UnionFreeTBLDefGrp(PUnionTBLDefGrp ptblDefGrp);

/*
����	
	��ʼ������
�������
	name	������
�������
	��
����ֵ
	�ɹ�	��ָ��
	NULL	��ָ��
*/
PUnionTBLDef UnionInitTBLDefFromDefFile(char *name);

/*
����	
	�ӱ����嵥��ʼ��������
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	�ɹ�	����ָ��
	ʧ��	NULL
*/
PUnionTBLDefGrp UnionInitTBLDefGrpFromTBLDefTBL();

/*
����	
	�ӱ����嵥��ʼ�����ϵ
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitAllTBLRelationsFromTBLDefList(PUnionTBLDefGrp ptblDefGrp);

/*
����	
	��һ��������������
�������
	name	����
	maxNum	�����������Ի��������������Ŀ
�������
	foreignTBLNameGrp	���������
����ֵ
	>=0	�ɹ��������Ŀ
	<0	������
*/
int UnionReadAllForeignTBLNames(char *name,char foreignTBLNameGrp[][40+1],int maxNum);

/*
����	
	��һ������Ǽǵ���������
�������
	ptblDef		����
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionRegisterSpecTBLToAllParents(PUnionTBLDef ptblDef,PUnionTBLDefGrp ptblDefGrp);

/*
����	
	��һ������Ǽǵ���������
�������
	ptblDefGrp	������ָ��
�������
	��
����ֵ
	�ɹ�	ָ��
	ʧ��	NULL
*/
PUnionTBLDef UnionFindSpecTBLDefInTBLDefGrp(PUnionTBLDefGrp ptblDefGrp,char *name);

/*
����	
	��ӡһ�����SQL���
�������
	ptblDef	������ָ��
	fp	�ļ�ָ��
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp,char *mainMenuName);

/*
����	
	��ӡ���б��SQL���
�������
	fp		�ļ�ָ��
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenu(FILE *fp,char *mainMenuName);

/*
����	
	��ӡ���б��SQL���
�������
	fileName	�ļ���
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDefInTableListOfSpecMainMenuToSpecFile(char *fileName,char *mainMenuName);

/*
����	
	�ж�һ����ĸ����Ƿ���ȫ������
�������
	ptblDef	������ָ��
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionIsAllParentsOfSpecTBLDefCreated(PUnionTBLDef ptblDef);

/*
����	
	�ж�һ������ӱ��Ƿ���ȫ������
�������
	ptblDef	������ָ��
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionIsAllChildrenOfSpecTBLDefCreated(PUnionTBLDef ptblDef);

/*
����	
	��ӡ���б��SQL���
�������
	ptblDefGrp	������ָ��
	fp		�ļ�ָ��
	mainMenuName	���˵�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,FILE *fp,char *mainMenuName);


/*
����	
	��ӡ���б��SQL���
�������
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDefInTableList(FILE *fp);

/*
����	
	��ӡ���б��SQL���
�������
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateSQLForAllTBLDefInTableListToSpecFile(char *fileName);


/*
����	
	��ӡһ��������
�������
	ptblDef	������ָ��
	prefix		������ǰ׺
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintTBLNameForSpecTBLDef(PUnionTBLDef ptblDef,char *prefix,FILE *fp);

/*
����	
	��ӡ���б�����
�������
	prefix		������ǰ׺
	ptblDefGrp	������ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintTBLNameForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,char *prefix,FILE *fp);

/*
����	
	��ӡ���б�����
�������
	prefix		������ǰ׺
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintTBLNameForAllTBLDefInTableList(char *prefix,FILE *fp);

/*
����	
	��ӡ���б�����
�������
	prefix		������ǰ׺
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintTBLNameForAllTBLDefInTableListToSpecFile(char *prefix,char *fileName);

/*
����	
	�������б������
�������
	dir		�������ļ�Ŀ¼
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionOutputAllRecForAllTBLDef(char *dir);

/*
����	
	��ʾ���б��ϵ
�������
	ptblDefGrp	������ָ��
	displayParents	1,��ʾ����0,��ʾ�ӱ�2��ʾ���й�ϵ
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,int displayParents,FILE *fp);

/*
����	
	��ʾ���б��ϵ
�������
	displayParents	1,��ʾ����0,��ʾ�ӱ�2��ʾ���й�ϵ
	fileName	�ļ�����
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsForAllTBLDefToSpecFile(int displayParents,char *fileName);

/*
����	
	��ʾ����ӱ�
�������
	ptblDef		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintChildrenOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp);

/*
����	
	��ʾ��ĸ���
�������
	ptblDef		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintParentsOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp);

/*
����	
	��ʾ��Ĺ�ϵ
�������
	ptblDef		��ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsOfSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp);

/*
����	
	��ʾָ��������й�ϵ
�������
	ptblDefGrp	������ָ��
	tableName	����
	displayParents	1,��ʾ����0,��ʾ�ӱ�2��ʾ���й�ϵ
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsForSpecTBLDef(PUnionTBLDefGrp ptblDefGrp,char *tableName,int displayParents,FILE *fp);

/*
����	
	��ʾָ��������й�ϵ
�������
	tableName	����
	displayParents	1,��ʾ����0,��ʾ�ӱ�2��ʾ���й�ϵ
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPrintRelationsForSpecTBLDefToSpecFile(char *tableName,int displayParents,char *fileName);

/*
����	
	��ӡһ�����ɾ�����SQL���
�������
	ptblDef	������ָ��
	fp	�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateDropTBLSQLForSpecTBLDef(PUnionTBLDef ptblDef,FILE *fp);

/*
����	
	��ӡ���б��ɾ�����SQL���
�������
	ptblDefGrp	������ָ��
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateDropTBLSQLForAllTBLDef(PUnionTBLDefGrp ptblDefGrp,FILE *fp);

/*
����	
	��ӡ���б��ɾ�����SQL���
�������
	fp		�ļ�ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateDropTBLSQLForAllTBLDefInTableList(FILE *fp);

/*
����	
	��ӡ���б��ɾ�����SQL���
�������
	fileName	�ļ���
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionCreateDropTBLSQLForAllTBLDefInTableListToSpecFile(char *fileName);

#endif

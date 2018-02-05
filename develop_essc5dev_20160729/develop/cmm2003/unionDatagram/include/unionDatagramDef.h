// wolfang wang
// 2008/10/3

#ifndef _unionDatagramDef_
#define _unionDatagramDef_

#define conDatagramFldDefTagFldID	"fldID"
#define conDatagramFldDefTagFldValueID	"valueID"
#define conDatagramFldDefTagAlais	"alais"
#define conDatagramFldDefTagLen		"len"
#define conDatagramFldDefTagRemark	"remark"
#define conDatagramFldDefTagIsRequest	"isRequest"
#define conDatagramFldDefTagOptional	"optional"
#define conDatagramTagAppID		"idOfApp"
#define conDatagramTagServiceID		"serviceID"
#define conDatagramTagHeader		"header="

/** �ⲿ���Ķ��� **/
// ��������
typedef struct
{
	int		isRequest;		// ����������Ӧ��ʶ
	char		fldID[64+1];		// �������ʶ
	char		alais[64+1];		// ����
	int		len;			// ��󳤶�
	char		valueID[64+1];		// ֵ�򣬶�����и�ֵ�����м�飬����ֵ����еı�ʶ
	char		remark[128+1];		// ˵��
	int		optional;		// �Ƿ��ǿ�ѡ��1��ʾ��ѡ��0��ʾ������
} TUnionDatagramFldDef;
typedef TUnionDatagramFldDef	*PUnionDatagramFldDef;

#define conMaxNumOfFldPerDatagram	64
// ����һ��˫���ĵĶ���
typedef struct
{
	char			appID[64+1];		// ���ĵ���;
	char			datagramID[64+1];	// ���ĵı�ʶ
	char			serviceID[3+1];		// �������
	int			fldNum;			// ����Ŀ
	TUnionDatagramFldDef	fldGrp[conMaxNumOfFldPerDatagram];	// ����������Ŀ
} TUnionDatagramDef;
typedef TUnionDatagramDef	*PUnionDatagramDef;

/*
����	
	��һ�����ݴ��ж�ȡһ�����ĵ�ͷ����
�������
	str		���崮
	lenOfStr	������
�������
	serviceID	����ͷ�еķ������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadDatagramHeaderFromStr(char *str,int lenOfStr,char *serviceID);

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
void UnionGetFileNameOfDatagramDef(char *appID,char *datagramID,char *fileName);

/*
����	
	��һ�����ݴ��ж�ȡһ����������
�������
	str		���崮
	lenOfStr	������
�������
	pdef		�����ı�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadDatagramFldDefFromStr(char *str,int lenOfStr,PUnionDatagramFldDef pdef);

/*
����	
	�ӱ��ж�ȡһ�����ĵĶ���
�������
	appID		���ĵ����
	datagramID	���ĵı�ʶ
�������
	pdef		�����ı��Ķ���
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadSpecDatagramDefFromFile(char *appID,char *datagramID,PUnionDatagramDef pdef);

/*
����	
	��ӡһ�����ĵ�����
�������
	pdef		Ҫ��ӡ�ı�������
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintDatagramFldDefToFp(PUnionDatagramFldDef pdef,FILE *fp);

/*
����	
	��ӡһ�����ĵĶ���
�������
	pdef		Ҫ��ӡ�ı��Ķ���
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintDatagramDefToFp(PUnionDatagramDef pdef,FILE *fp);

/*
����	
	��ӡһ�����ĵĶ���
�������
	appID		������;��ʶ
	datagramID	���ĵ����
	fp		������ļ�ָ��
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecDatagramDefToFp(char *appID,char *datagramID,FILE *fp);

/*
����	
	��ӡһ�����ĵĶ��嵽ָ���ļ�
�������
	appID		������;��ʶ
	datagramID	���ĵ����
	fileName	�ļ�����
�������
	��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintSpecDatagramDefToSpecFile(char *appID,char *datagramID,char *fileName);
#endif

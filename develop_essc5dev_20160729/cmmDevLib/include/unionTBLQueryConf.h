//---------------------------------------------------------------------------

#ifndef unionTBLQueryConfH
#define unionTBLQueryConfH
//---------------------------------------------------------------------------
#include "unionWorkingDir.h"
#include "unionFldGrp.h"

// ����һ����ͻ������ü�¼
typedef struct
{
	char			name[40+1];			// �����ƣ���������Ψһֵ��
	char			alais[40+1];			// ����������ֵ������ʾ�ڱ����ı�����
	int			id;				// ���ʶ���Ǳ��Ψһ��ʶ�����ֵ��ƽ̨����ı�ı�ʶ��Ӧ

	char			formIconFileName[40+1];		// ����ͼ���ļ�������
	char			formPopMenuDefFileName[40+1];	// �ڽ���հ״�����Ҽ�ʱ����ʾ���ĵ����˵������ļ�

	char			recIconFileName[40+1];		// ��¼ͼ���ļ�������
	char			recPopMenuDefFileName[40+1];	// �Ҽ������¼ʱ����ʾ���ĵ����˵������ļ�
	char			recDoubleClickMenuFileName[40+1];	// ˫����¼�˵�����
	int			maxNumOfRecPerPage;		// ÿҳ������ʾ�ļ�¼���������
	int			useSeparateTBL;			// �Ƿ��ҳ��ʾ
	TUnionQueryFldGrp	queryFldGrp;			// ��ѯ�ļ�¼��
	TUnionFldGrp		primaryKeyFldGrp;		// �ؼ�������
} TUnionTBLQueryConf;
typedef TUnionTBLQueryConf	*PUnionTBLQueryConf;

/*
����	��ñ����ü�¼
�������
	tblName		����
�������
	prec		�����ü�¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionReadTBLQueryConf(char *tblName,PUnionTBLQueryConf prec);

/*
����	�����������д�뵽�ļ���
�������
	prec		���ü�¼
	fp		�ļ����
�������
	
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionPrintTBLQueryConfToFp(PUnionTBLQueryConf prec,FILE *fp);

#endif
 


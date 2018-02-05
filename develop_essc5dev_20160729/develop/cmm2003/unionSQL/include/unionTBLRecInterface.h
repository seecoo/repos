//---------------------------------------------------------------------------

#ifndef unionTBLRecInterfaceH
#define unionTBLRecInterfaceH
//---------------------------------------------------------------------------
#include "unionWorkingDir.h"
#include "unionFldGrp.h"

#include "unionFldValueAssign.h"

// ����һ�����������¼
typedef struct
{
	char			name[40+1];			// �����ƣ���������Ψһֵ��
	char			alais[40+1];			// ����������ֵ������ʾ�ڱ����ı�����
	int			id;				// ���ʶ���Ǳ��Ψһ��ʶ�����ֵ��ƽ̨����ı�ı�ʶ��Ӧ

	char			recIconFileName[40+1];		// ��¼ͼ���ļ�������
	TUnionFldValueAssignMethodGrp	fldAssignGrp;			// ���ĸ�ֵ��������
	TUnionFldGrp		primaryKeyFldGrp;		// �ؼ�������
} TUnionTBLRecInterface;
typedef TUnionTBLRecInterface	*PUnionTBLRecInterface;

/*
����	�������������ļ�������
�������
	tblAlais	�����
�������
	fileName	�����ļ�����
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetTBLRecInterfaceFileName(char *tblAlais,char *fileName);
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
int UnionReadTBLRecInterface(char *tblName,PUnionTBLRecInterface prec);

/*
����	�����ı���
�������
	prec            ��¼��ֵ����
        fldName         ����
�������
	fldAlais        �����
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionReadFldAlaisFromTBLRecInterface(PUnionTBLRecInterface prec,char *fldName,char *fldAlais);

/*
����	�ж����Ƿ��ǹؼ�����
�������
	prec            ��¼��ֵ����
        fldName         ����
�������
	��
����ֵ
	1		�ǹؼ�����
	0		���ǹؼ�����
*/
int UnionFldOfTBLRecInterfaceIsPrimaryKeyFld(PUnionTBLRecInterface prec,char *fldName);

#endif


//---------------------------------------------------------------------------

#ifndef unionFldGrpH
#define unionFldGrpH
//---------------------------------------------------------------------------

#define 	conMaxNumOfQueryFld		64
#define 	conMaxNumOfPrimaryKeyFld	8
typedef char	TUnionFldName[40+1];

// ����һ����ѯ����
typedef struct
{
	int		fldNum;
	TUnionFldName	fldName[conMaxNumOfQueryFld];
	TUnionFldName	fldAlais[conMaxNumOfQueryFld];
} TUnionQueryFldGrp;
typedef TUnionQueryFldGrp *PUnionQueryFldGrp;

// ����һ������
typedef struct
{
	int		fldNum;
	TUnionFldName	fldName[conMaxNumOfQueryFld];
} TUnionFldGrp;
typedef TUnionFldGrp *PUnionFldGrp;

/*
����	���ļ��ж�ȡ��ѯ����
�������
	fileName	�ļ���
�������
	prec		��ѯ����
����ֵ
	>=0		��ѯ�����Ŀ
	<0		�������
*/
int UnionReadQueryFldGrpDef(char *fileName,PUnionQueryFldGrp prec);

/*
����	���ļ���д���ѯ����
�������
	prec		��ѯ����
	fp		�ļ����
�������
	prec		��ѯ����
����ֵ
	>=0		��ѯ�����Ŀ
	<0		�������
*/
int UnionPrintQueryFldGrpDefToFp(PUnionQueryFldGrp prec,FILE *fp);

/*
����	���ļ��ж�ȡ����
�������
	fileName	�ļ���
	fldFlag		���ʶ
�������
	prec		��ѯ����
����ֵ
	>=0		��ѯ�����Ŀ
	<0		�������
*/
int UnionReadFldGrpDef(char *fileName,char *fldFlag,PUnionFldGrp prec);

//---------------------------------------------------------------------------
/*
����	���ļ���д������
�������
	prec		��ѯ����
	fldFlag		���ʶ
	fp		�ļ����
�������
����ֵ
	>=0		��ѯ�����Ŀ
	<0		�������
*/
int UnionPrintFldGrpDefToFp(char *fldFlag,PUnionFldGrp prec,FILE *fp);

/*
����	���ļ��ж�ȡ�ؼ�������
�������
	fileName	�ļ���
�������
	prec		��ѯ����
����ֵ
	>=0		��ѯ�����Ŀ
	<0		�������
*/
int UnionReadPrimaryKeyFldGrpDef(char *fileName,PUnionFldGrp prec);

//---------------------------------------------------------------------------
/*
����	���ļ���д������
�������
	prec		��ѯ����
	fp		�ļ����
�������
����ֵ
	>=0		��ѯ�����Ŀ
	<0		�������
*/
int UnionPrintPrimaryKeyFldGrpDefToFp(PUnionFldGrp prec,FILE *fp);

#endif

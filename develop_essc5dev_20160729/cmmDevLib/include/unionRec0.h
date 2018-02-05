//---------------------------------------------------------------------------

#ifndef unionRecH
#define unionRecH
//---------------------------------------------------------------------------
// ����һ����¼
#define conMaxNumOfFldPerRec	64
typedef struct
{
	int		fldNum;
	char		fldName[conMaxNumOfFldPerRec][40+1];
	char		fldValue[conMaxNumOfFldPerRec][4096+1];
} TUnionRec;
typedef TUnionRec *PUnionRec;

/*
����	��һ���ַ�����ɼ�¼��,�����ظ�����һ����
�������
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	prec		�����ļ�¼
����ֵ
	>=0		��¼����������Ŀ
	<0		������
*/
int UnionReadRecFromRecStrRepeatPermitted(char *recStr,int lenOfRecStr,PUnionRec prec);

/*
����	��һ���ַ�����ɼ�¼��
�������
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	prec		�����ļ�¼
����ֵ
	>=0		��¼����������Ŀ
	<0		������
*/
int UnionReadRecFromRecStr(char *recStr,int lenOfRecStr,PUnionRec prec);

/*
����	��һ����¼��д�뵽��¼�ַ���
�������
	prec		��¼
	sizeOfRecStr	��¼������Ĵ�С
�������
	recStr		��ɵļ�¼��
����ֵ
	>=0		��¼���ĳ���
	<0		������
*/
int UnionPutRecIntoRecStr(PUnionRec prec,char *recStr,int sizeOfRecStr);

/*
����	�Ƚ�һ����¼���Ƿ���������
�������
	recStr		��¼��
	lenOfRecStr	��¼������
	conStr		������
	lenOfConStr	����������
�������
	��
����ֵ
	>0		����
	=0		������
	<0		��������
*/
int UnionIsRecStrFitSpecConditon(char *recStr,int lenOfRecStr,char *conStr,int lenOfConStr);

/*
����	�Ƚ�һ����¼���Ƿ���������������
�������
	pconRec		������
	index		�������ĸ���ʼ�Ƚ�
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	>0		����
	=0		������
	<0		��������
*/
int UnionIsRecStrFitAndCondition(PUnionRec pconRec,int index,char *recStr,int lenOfRecStr);

/*
����	�Ƚ�һ����¼���Ƿ��������л�����
�������
	pconRec		����
	index		�������ĸ���ʼ�Ƚ�
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	>0		����
	=0		������
	<0		��������
*/
int UnionIsRecStrFitOrCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr);

/*
����	�Ƚ�һ����¼���Ƿ��������з�����
�������
	pconRec		����
	index		�������ĸ���ʼ�Ƚ�
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	>0		����
	=0		������
	<0		��������
*/
int UnionIsRecStrFitNotCondition(PUnionRec pconRec,int startIndex,char *recStr,int lenOfRecStr);

#endif

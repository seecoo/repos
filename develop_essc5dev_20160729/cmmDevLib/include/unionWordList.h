// Wolfgang Wang
// 2008/7/24


#ifndef _unionWordList_
#define _unionWordList_

#define conFldQueryConditionLarger	1	// ���ڸ�ֵ
#define conFldQueryConditionEqual	2	// ���ڸ�ֵ
#define conFldQueryConditionSmaller	3	// С�ڸ�ֵ
#define conFldQueryConditionNotEqual	4	// �����ڸ�ֵ
#define conFldQueryConditionLike	5	// ������ֵ

typedef struct TWordList
{
	char			*word;
	struct	TWordList 	*next;
} TUnionWordList;
typedef TUnionWordList		*PUnionWordList;

/*
����	��ʼ��word��
�������
	word		ֵ
�������
	��
����ֵ
	�ַ���ָ��
*/
PUnionWordList UnionNewWordList(char *word);

/*
����	�ͷ�word��
�������
	plist		��
�������
	��
����ֵ
	��
*/
void UnionFreeWordList(PUnionWordList plist);


/*
����	��һ���ַ�������ִ�
�������
	recStr		��¼��
	lenOfRecStr	��¼������
�������
	��
����ֵ
	�ַ���ָ��
*/
PUnionWordList UnionConvertStrIntoWordList(char *recStr,int lenOfRecStr);

/*
����	��ӡһ����
�������
	plist		��
	fileName	�ļ�����
�������
	��
����ֵ
	>=0		������
	<0		�������
*/
int UnionPrintWordListToFile(PUnionWordList plist,char *fileName);

#endif

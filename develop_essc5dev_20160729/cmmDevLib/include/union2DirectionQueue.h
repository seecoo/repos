// Wolfgang Wang
// 2008/1/17

#ifndef _union2DirectionQueue_
#define _union2DirectionQueue_

typedef struct TUnion2DirectionQueueRec
{
	char				*key;		// ����
	char				lastUsedTime[14+1];	// ���һ��ʹ��ʱ��
	struct TUnion2DirectionQueueRec	*previous;	// ǰһ����¼
	struct TUnion2DirectionQueueRec	*next;		// ��һ����¼
	unsigned char			*reserved;	// ����ֵ
	int				lenOfData;	// �������ݵĳ���
} TUnion2DirectionQueueRec;
typedef TUnion2DirectionQueueRec	*PUnion2DirectionQueueRec;

// ��ʾ��������
/*
�������
	prec		������Ŀ
	resID		��Դ��
�������
	��
����ֵ
	>=0		����Ŀ
	<0		������
*/
int UnionSpierAllRecIn2DirectionQueue(PUnion2DirectionQueueRec prec,int resID);

// ��ʼ��һ���µļ�¼
/*
�������
	key		�ؼ���
	reserved	������
	sizeOfReserved	�����ֳ���
�������
	��
����ֵ
	�ɹ�		�¼�¼ָ��
	ʧ��		��ָ��
*/
PUnion2DirectionQueueRec UnionNew2DirectionQueueRec(char *key,unsigned char *reserved,int sizeOfReserved);

// �ͷ�һ����¼
/*
�������
	prec		��¼ָ��
�������
	��
����ֵ
	��
*/
void UnionFree2DirectionQueueRec(PUnion2DirectionQueueRec prec);

// ����һ��ֵ��Ӧ�ļ�¼
/*
�������
	prec		���ҵ���ʼλ��ָ��
	key		�ؼ���
�������
	��
����ֵ
	�ɹ�		��¼ָ��
	ʧ��		��ָ��
*/
PUnion2DirectionQueueRec UnionFind2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key);

// ��һ��ֵ�������
/*
�������
	prec		�������ʼλ�õ�ָ��
	key		�ؼ���
	reserved	������
	sizeOfReserved	�����ֳ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionInsert2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key,unsigned char *reserved,int sizeOfReserved);		

// ��һ��ֵ�ӱ���ɾ��
/*
�������
	prec		��¼ָ��
	key		�ؼ���
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionDelete2DirectionQueueRec(PUnion2DirectionQueueRec prec,char *key);

// ��ʾ���е����м�¼
/*
�������
	prec		��¼ָ��
	fp		�����м�¼��ʾ�����ļ�
�������
	��
����ֵ
	�ɹ�		>=0
	ʧ��		<0,������
*/
int UnionPrintAll2DirectionQueueRecToFile(PUnion2DirectionQueueRec prec,FILE *fp);

// ɾ�����е����м�¼
/*
�������
	prec		��¼ָ��
�������
	��
����ֵ
	�ɹ�		>=0,ɾ���ļ�¼��
	ʧ��		<0,������
*/
int UnionDeleteAll2DirectionQueueRec(PUnion2DirectionQueueRec prec);

#endif

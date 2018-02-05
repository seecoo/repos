// Wolfgang Wang,2009/8/1

#ifndef _unionMngSvrBrothers_
#define _unionMngSvrBrothers_

/*
���ܣ�
	���󱸷ݷ�����ͬ��һ�����붯��
�������:
	tableName	����
	record		���������Ĳ�����
	lenOfRecord	�������ĳ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBInsertToBrothers(char *tableName,char *record,int lenOfRecord);

/*
���ܣ�
	���󱸷ݷ�����ͬ��һ��ɾ������
�������:
	tableName	����
	primaryKey	���������Ĺؼ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBDeleteToBrothers(char *tableName,char *primaryKey);

/*
���ܣ�
	���󱸷ݷ�����ͬ��һ�����¶���
�������:
	tableName	����
	primaryKey	���������Ĺؼ���
	record		���������Ĳ�����
	lenOfRecord	�������ĳ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBUpdateToBrothers(char *tableName,char *primaryKey,char *record,int lenOfRecord);

/*
���ܣ�
	��ȡ��mngSvr�ı�ʶ
�������:
	��
�������:
	��
����ֵ
	�ɹ���	��mngSvr�ı�ʶ
	ʧ��	��ָ��
*/
char *UnionGetMyMngSvrID();

/*
���ܣ�
	��ȡ��mngSvr�������ֵ�
�������:
	maxNum		�ɶ�ȡ�������Ŀ
�������:
	brotherID	�ֵ�mngSvr�ı�ʶ
����ֵ
	>=0		��ȡ����Ŀ
	<0		������
*/
int UnionReadAllMngSvrBrothersOfMine(char brotherID[][128+1],int maxNum);

/*
���ܣ�
	����ָ�����ݷ�����ͬ��һ������
�������:
	mngSvrID	���ݷ������ı�ʶ
	tableName	����
	operation	����
	record		���������Ĳ�����
	lenOfRecord	�������ĳ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBOperationToSpecBrother(char *mngSvrID,char *tableName,int operation,char *record,int lenOfRecord);

/*
���ܣ�
	���󱸷ݷ�����ͬ��һ������
�������:
	tableName	����
	operation	����
	record		���������Ĳ�����
	lenOfRecord	�������ĳ���
�������:
	��
����ֵ
	>=0		�ɹ�
	<0		ʧ��
*/
int UnionSynchronizeComplexDBOperationToBrothers(char *tableName,int operation,char *record,int lenOfRecord);

#endif


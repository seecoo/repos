// Wolfgang Wang,2009/8/1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
int UnionSynchronizeComplexDBInsertToBrothers(char *tableName,char *record,int lenOfRecord)
{
	return 0;
}

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
int UnionSynchronizeComplexDBDeleteToBrothers(char *tableName,char *primaryKey)
{
	return 0;
}

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
int UnionSynchronizeComplexDBUpdateToBrothers(char *tableName,char *primaryKey,char *record,int lenOfRecord)
{
	return 0;
}

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
char *UnionGetMyMngSvrID()
{
	return(getenv("myMngSvrID"));
}

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
int UnionReadAllMngSvrBrothersOfMine(char brotherID[][128+1],int maxNum)
{
	return(0);
}


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
int UnionSynchronizeComplexDBOperationToSpecBrother(char *mngSvrID,char *tableName,int operation,char *record,int lenOfRecord)
{
	return 0;
}

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
int UnionSynchronizeComplexDBOperationToBrothers(char *tableName,int operation,char *record,int lenOfRecord)
{
	return(0);
}	

// �Ǽ�����ͬ����ˮ��
int UnionRegComplexDBOperationToDBDataSynJnl(char *mngSvrID, char *tableName, int operation, char *record, int lenOfRecord)
{
        return 0;
}

// �Ǽ��ļ�ͬ����ˮ��
int UnionRegFileOperationToFileDataSynJnl(char *tableName, char *dirName,char *fileName, int fileType)
{
        return 0;
}

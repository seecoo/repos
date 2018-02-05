//	Author: Wolfgang Wang
//	Date: 2008-12-3

#ifndef _unionDataManageByBranch_
#define _unionDataManageByBranch_

/*
���ܣ�
	��ȡ����Ա����Ȩ
�������:
	idOfOperator 	�û�����ģ�����Ψһʶ��һ����¼������
�������:
	branchID	����Ա��������
	dataReadAuth	����Ա����Ȩ��
����ֵ
	>=0		�ɹ�
	<0		ʧ��,������
*/
int UnionReadAuthDefOfOperator(char *idOfOperator,char *branchID,int *dataReadAuth);

/*
���ܣ�
	�ж��Ƿ�ֻ�����������
���������
	��
���������
	��
����ֵ��
	1��		��
	0��		��
*/
int UnionIsManageDataByBranch();

/*
���ܣ�
	�������������
���������
	idOfObject��	����ID
���������
	branchFldName	�����������
����ֵ��
	>=0��		�ɹ���������
	<0��		ʧ�ܣ�������
*/
int UnionReadBranchFldNameOfSpecTBL(char *tblName,char *branchFldName);
	
/*
���ܣ�
	�Զ����Ӳ�ѯ����
���������
	idOfObject������ID
	oriConLen:	Դ�����ĳ���
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	sizeOfBuf	����������Ĵ�С
���������
	condition	�������Զ���ѯ���������
����ֵ��
	>=0��		�ɹ���������
	<0��		ʧ�ܣ�������
*/
int UnionAutoAppendBranchIDToQueryCondition(char *tblName,int oriConLen,char *condition,int sizeOfBuf);

/*
���ܣ�
	�Զ��������ݴ�
���������
	idOfObject��	����ID
	record		��¼����"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
	lenOfRecord	��¼������
���������
	record		�������Զ���ļ�¼��
����ֵ��
	>=0��		�ɹ���������
	<0��		ʧ�ܣ�������
*/
int UnionAutoAppendFldOfSpecTBL(char *idOfObject,char *record,int lenOfRecord);

#endif

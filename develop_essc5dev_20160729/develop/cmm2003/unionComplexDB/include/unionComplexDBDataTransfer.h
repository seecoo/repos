//	Author: Wolfgang Wang
//	Date: 2008-10-23

#ifndef _unionComplexDBDataTransfer_
#define _unionComplexDBDataTransfer_

/*
���ܣ�
	�������е����з��������ļ�¼
���������
	resName		��������
	condition	����
	outputFileName	�ļ����ƣ���¼д�뵽���ļ���
���������
	��
����ֵ��
	0��		�����ļ�¼��
	<0��		ʧ�ܣ�������
*/
int UnionOutputAllRecFromSpecTBL(char *resName,char *condition,char *outputFileName);

/*
���ܣ�
	����¼�������
���������
	resName		��������
	inputFileName	�ļ����ƣ���¼�����ڸ��ļ���
���������
	��
����ֵ��
	0��		����ļ�¼��
	<0��		ʧ�ܣ�������
*/
int UnionInputAllRecIntoSpecTBL(char *resName,char *inputFileName);

#endif

//	Author: Wolfgang Wang
//	Date: 2008-10-23

#ifndef _unionComplexDBUnionQuery_
#define _unionComplexDBUnionQuery_

/*
���ܣ�
	ִ�����ϲ�ѯ����
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionSelectSpecRecInStr(char *triggerStr,int lenOfTriggerStr,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr);

/*
���ܣ�
	ִ��һ���ļ��е����ϲ�ѯ����
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionSelectSpecRecInFile(char *fileName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr);

/*
���ܣ�
	ִ��һ��������ϲ�ѯ����
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionSelectSpecRecOnObject(char *resName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr);

/*
���ܣ�������ѯһ�������ʵ��
���������
	idOfObject������ID
	condition����ѯ��������ʽΪ"��1=��ֵ|��2=��ֵ|��3=��ֵ|����N=��ֵ|"
���������
	fileName���洢�˲�ѯ������ʵ����ֵ���ļ���
����ֵ��
	>=0���ɹ������ز�ѯ���ļ�¼����Ŀ
	<0��ʧ�ܣ�������
*/
int UnionBatchSelectUnionObjectRecord(TUnionIDOfObject idOfObject,char *condition,char *fileName);

#endif


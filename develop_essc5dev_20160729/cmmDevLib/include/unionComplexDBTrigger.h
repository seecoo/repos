//	Author: Wolfgang Wang
//	Date: 2008-10-23

#ifndef _unionComplexDBTrigger_
#define _unionComplexDBTrigger_

/*
����	�򿪴����������Ķ���
�������
	��
�������
	��
����ֵ
	��
*/
void UnionOpenOperationOnTrigger();

/*
����	�رմ����������Ķ���
�������
	��
�������
	��
����ֵ
	��
*/
void UnionCloseOperationOnTrigger();

/*
����	�ж��Ƿ��Ǵ����������Ķ���
�������
	��
�������
	��
����ֵ
	1	��
	0	����
*/
int UnionIsOperationOnTrigger();

//---------------------------------------------------------------------------
/*
����	���ݶ����trigger�������Լ�����������ƴ��trigger��Ҫ�Ĳ�������
�������
	oriCondition		trigger����ĸ�ֵ����
	lenOfOriCondition	trigger����ĸ�ֵ��������
	recStr			����ʹ�õ�ʵ������
	lenOfRecStr		����ʹ�õ�ʵ�������ĳ���
	sizeOfDesCondition	Ŀ����������Ĵ�С
�������
	desCondition		Ŀ������
����ֵ
	>= 0			Ŀ�������ĳ���
	<0			�������
*/
int UnionFormConditionFromTriggerDefinedStr(char *oriCondition,int lenOfOriCondition,char *recStr,int lenOfRecStr,char *desCondition,int sizeOfDesCondition);

/*
���ܣ�
	ִ�д���������
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
	fileRecved	�Ƿ��յ����ļ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationInStr(char *triggerStr,int lenOfTriggerStr,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved);

/*
���ܣ�
	ִ��һ���ļ��еĴ���������
���������
	resName		��������
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
	fileRecved	�Ƿ��յ����ļ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationInFile(char *fileName,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved);

/*
���ܣ�
	ִ��һ����Ĵ���������
���������
	resName		��������
	operationTag	������ʶ
	beforeOrAfter	�ǲ���֮ǰִ�л���֮��ִ��
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
	sizeOfResStr	ִ�н������Ĵ�С
���������
	resStr		ִ�н��
	fileRecved	�Ƿ��յ����ļ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationOnObject(char *resName,char *operationTag,char *beforeOrAfter,char *recStr,int lenOfRecStr,char *resStr,int sizeOfResStr,int *fileRecved);

/*
���ܣ�
	ִ��һ����Ĵ���������
���������
	resName		��������
	operationID	������ʶ
	isBeforeOperation	�ǲ���֮ǰִ�л���֮��ִ�У�1 ��ʾ����֮ǰ
	recStr		����ʵ����ֵ����ʽΪ"��1=��ֵ|��2=��ֵ|��|��N=��ֵ"
	lenOfRecStr	record�ĳ���
���������
	��
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteTriggerOperationOnTable(char *resName,int operationID,int isBeforeOperation,char *recStr,int lenOfRecStr);

#endif
 


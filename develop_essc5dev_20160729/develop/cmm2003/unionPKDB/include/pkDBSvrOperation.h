#ifndef _PKDBSvrOperation_H_
#define _PKDBSvrOperation_H_

/*
���ܣ���Կ����������
���������
	serviceID�� �����ʶ
	reqStr����������
	lenOfReqStr���������ݵĳ���
	sizeOfResStr��resStr�Ļ�������С
���������
	resStr����Ӧ����
fileRecved��1����ʾ�������˷�����һ���ļ���0����ʾû���ļ�
����ֵ��
	0���ɹ�
	<0��ʧ�ܣ�������
*/
int UnionExcuteUnionPKDBSvrOperation(int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

#endif


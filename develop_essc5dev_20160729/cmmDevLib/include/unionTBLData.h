#ifndef _UNIONTBLDATA_H_
#define _UNIONTBLDATA_H_

/* 
���ܣ�
	��complexDB�е�������
����:
	[IN]:
		resName			: ������[����]
	[OUT]:
		NULL
����ֵ:
	[0, +d)				: �ɹ�, �������ݼ�¼������
	(-d, 0)				: ʧ��
*/
int UnionOutputTBLData(char *resName);

/* 
���ܣ�
	�� tblFileName �ļ���,�����ݵ��� resName ��
����:
	[IN]:
		tblFileName		: ��Ҫ���������ݵ��ļ���,Ҫ��Ϊȫ·���ļ���
		resName			: ������[����]
		isEnvData		: �Ƿ�Ϊ env ����
	[OUT]:
		NULL
����ֵ:
	[0, +d)				: �ɹ�, �������ݼ�¼������
	(-d, 0)				: ʧ��
*/
int UnionInputTBLData(char *tblFileName, char *resName, int isEnvData);

#endif


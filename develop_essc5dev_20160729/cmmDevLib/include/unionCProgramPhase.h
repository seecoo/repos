//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#ifndef _unionCProgramPhase_
#define _unionCProgramPhase_

/* ��ָ���ļ���һ��c���Ե���Ч������
�������
	fp		�ļ����
	maxNumOfLine	�ɶ������������
�������
	cprogramLine	������
	lineNum		���������ļ��ĵڼ���
����ֵ��
	>=0 	����������Ŀ��0��ʾ�ļ�������
	<0	�������	
	
*/
int UnionReadCProgramLineFromFile(FILE *fp,char cprogramLine[][1024+1],int maxNumOfLine,int *lineNum);

/* ȥ��һ�������е�ע����
�������
	oriFileName	Դ�ļ���
	desFileName	Ŀ���ļ���
�������
	��
����ֵ��
	>=0 	������������
	<0	�������	
	
*/
int UnionFilterRemarkFromCProgramFile(char *oriFileName,char *desFileName);

#endif

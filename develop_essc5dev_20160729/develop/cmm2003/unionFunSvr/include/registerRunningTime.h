#ifndef _RegisterRunningTime_
#define _RegisterRunningTime_

#define defStageOfTransBefore	0
#define defStageOfTransing	1
#define defStageOfTransAfter	2

// ���ý��״���׶�
void UnionSetStageOfTransProcessing(int stage);

// ��ʼ���Ǽǽڵ�ID
void UnionInitRegisterNodeID();

// �Ǽǽڵ�����ʱ��
void UnionRegisterRunningTime(char *nodeName);

// ���������ģ�����ʱ��
void UnionSetTimeByRequestPackage();

// ����ʱ�䵽������
void UnionSetTimeToRequestPackage();

#endif

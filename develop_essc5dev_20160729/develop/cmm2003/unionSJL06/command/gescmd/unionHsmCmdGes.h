/****************gesָ����װ********************

author:  lics
date:    2014-4-29

***********************************************/









/*
���ܣ��໥��֤

�������:
	zakByzmk: ��֤��Կ�����ڼ���mac
	lenOfrandData: ���������
	randData: ������� ���ڼ���mac
	mac: �û��˼������mac

���������
	lenOfrandData2: ���������
	randData2: �����
	mac2: ���ܻ�������mac

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(const char* zakByzmk,  const int* lenOfrandData, const char* randData, const char* mac,  int* lenOfrandData2, char* randData2, char* mac2);



/*
���ܣ��˳���֤

���������

���������

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM();



/*
���ܣ� д�����ܻ���

���������
	quotaByzmk����zmk���ܵ�������ģ�  ���ĸ�ʽΪ����Ʒ��,������,ʱ������,���ֵ
	mac���û��˼������mac

���������

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac);



/*
���ܣ��Ӽ��ܻ��ж�ȡָ�����

���������
	quotaByzmk����zmk���ܵ������Ϣ���ģ� ���ĸ�ʽΪ�� ��Ʒ��,������,ʱ������
	mac:  �û��˼������mac

���������
	quotaByzmk2: ��zmk���ܵ�������ģ�  ���ĸ�ʽΪ����Ʒ��,������,ʱ������,���ֵ

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac, char* quotaByzmk2);



/*
���ܣ�ɾ��ָ�����

���������
	quotaByzmk: ��zmk���ܵ������Ϣ���ģ� ���ĸ�ʽΪ�� ��Ʒ��,������,ʱ������
	mac: �û��˼������mac

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac);



/*
���ܣ�ɾ�����ܻ����������

���������
	lenOfrandData�����������
	randData�������
	mac���û��˼������mac

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(const int lenOfrandData, const char* randData, const char* mac);



/*
���ܣ���ȡȫ���������

���������
	batchNoBegin�� ��ʼ���κ�
	batchNoEnd�� �������κ�
	mac���û��˼������mac

���������
	numOfquota�� �������
	listOfquotaByzmk����ʽ�������������*���������  ����������ģ���Ʒ��,������,ʱ������,���ֵ

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(const int batchNoBegin, const int batchNoEnd, const char* mac, int* numOfquota, char* listOfquotaByzmk);



/*
���ܣ���ȡ����������

���������

���������
	numOfquota: ���������

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(int * numOfquota);




/****************gesָ����װ********************

author:  lics
date:    2014-4-29

***********************************************/
#include "unionHsmCmdGes.h"








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
int UnionHsmCmdMM(const char* zakByzmk,  const int* lenOfrandData, const char* randData, const char* mac,  int* lenOfrandData2, char* randData2, char* mac2)
{
	int ret;
	char hsmCmdBuf[1024+1];
	int hsmCmdLen;
	

	if ((zakByzmk == NULL) || (randData == NULL) || (mac == NULL))
	{
		UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
		return(errCodeParameter);
	}

	memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

	memcpy(hsmCmdBuf,"MM",2);
	hsmCmdLen = 2;

        if ((ret = UnionGenerateX917RacalKeyString(strlen(zakByzmk) ,zakByzmk ,hsmCmdBuf+hsmCmdLen)) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionGenerateX917RacalKeyString for [%s][%d]\n",zakByzmk, strlen(zakByzmk));
                return(ret);
        }
        hsmCmdLen += ret;
	
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", lenOfrandData);
	hsmCmdLen += 2;

	memcpy(hsmCmdBuf+hsmCmdLen, randData, lenOfrandData);
	hsmCmdLen += lenOfrandData;

	memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
	hsmCmdLen += 16;

	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
		return(ret);
	}
       
        hsmCmdBuf += 4;

	memcpy(lenOfrandData2, hsmCmdBuf, 2);
	hsmCmdBuf += 2;

	memcpy(randData2, hsmCmdBuf, lenOfrandData2);
	hsmCmdBuf += lenOfrandData2;

	memcpy(mac2, hsmCmdBuf, 16);

	return 0;
	
	
}



/*
���ܣ��˳���֤

���������

���������

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM()
{
	int ret;
	char hsmCmdBuf[32+1];

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }


	return 0;
}



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
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac)
{
        int ret;
        char hsmCmdBuf[128+1];
        int hsmCmdLen;


        if ((quotaByzmk == NULL) || (mac == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;

        memcpy(hsmCmdBuf+hsmCmdLen, quotaByzmk, 32);
        hsmCmdLen += 32;

        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

	return 0;

}



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
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac, char* quotaByzmk2)
{
        int ret;
        char hsmCmdBuf[128+1];
        int hsmCmdLen;


        if ((quotaByzmk == NULL) || (mac == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;

        memcpy(hsmCmdBuf+hsmCmdLen, quotaByzmk, 32);
        hsmCmdLen += 32;

        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf += 4;

        memcpy(quotaByzmk2, hsmCmdBuf, 32);

        return 0;
}



/*
���ܣ�ɾ��ָ�����

���������
	quotaByzmk: ��zmk���ܵ������Ϣ���ģ� ���ĸ�ʽΪ�� ��Ʒ��,������,ʱ������
	mac: �û��˼������mac

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(const char* quotaByzmk, const char* mac)
{
        int ret;
        char hsmCmdBuf[128+1];
        int hsmCmdLen;


        if ((quotaByzmk == NULL) || (mac == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;

        memcpy(hsmCmdBuf+hsmCmdLen, quotaByzmk, 32);
        hsmCmdLen += 32;

        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        return 0;
}



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
int UnionHsmCmdMM(const int lenOfrandData, const char* randData, const char* mac)
{
        int ret;
        char hsmCmdBuf[128+1];
        int hsmCmdLen;


        if ((randData == NULL) || (mac == NULL))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;
	
	sprintf(hsmCmdBuf+hsmCmdLen, "%02d", lenOfrandData);
        hsmCmdLen += 2;

        memcpy(hsmCmdBuf+hsmCmdLen, randData, lenOfrandData);
        hsmCmdLen += lenOfrandData;

        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        return 0;
}



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
int UnionHsmCmdMM(const int batchNoBegin, const int batchNoEnd, const char* mac, int* numOfquota, char* listOfquotaByzmk)
{
        int ret;
        char hsmCmdBuf[1024+1];
        int hsmCmdLen;


        if ((mac == NULL) || (batchNoBegin >= batchNoEnd))
        {
                UnionUserErrLog("in UnionHsmCmdMM:: wrong parameters!\n");
                return(errCodeParameter);
        }

        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);
        hsmCmdLen = 2;

        sprintf(hsmCmdBuf+hsmCmdLen, "%02d", batchNoBegin);
        hsmCmdLen += 2;


        sprintf(hsmCmdBuf+hsmCmdLen, "%02d", batchNoEnd);
        hsmCmdLen += 2;


        memcpy(hsmCmdBuf+hsmCmdLen, mac, 16);
        hsmCmdLen += 16;

        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf += 4;

        memcpy(numOfquota, 2, hsmCmdBuf);
        hsmCmdBuf += 2;

        memcpy(randData2, hsmCmdBuf, numOfquota);

        return 0;
}



/*
���ܣ���ȡ����������

���������

���������
	numOfquota: ���������

����ֵ
        >=0 �ɹ�
        <0 ʧ��
*/
int UnionHsmCmdMM(int * numOfquota)
{
	int ret;
        char hsmCmdBuf[32+1];


        memset(hsmCmdBuf,0,sizeof(hsmCmdBuf));

        memcpy(hsmCmdBuf,"MM",2);


        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
        {
                UnionUserErrLog("in UnionHsmCmdMM:: UnionDirectHsmCmd!\n");
                return(ret);
        }

        hsmCmdBuf += 4;

        memcpy(numOfquota, 2, hsmCmdBuf);
        
        return 0;
}




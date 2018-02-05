
#ifndef _UnionTASSCmd_
#define	_UnionTASSCmd_ 

//������ǩ��ָ��(����)
int UnionTASSHsmCmdK5(int vkIndex, char *hashID, int lenOfUsrID, char *usrID, int lengRCaPK,char *rCaPK,int lenCaCertDataSign,char *caCertDataSign,int lenHashVal,char *hashVal);

/*
   ��������:
   TJָ���Կ����
   �������:
encryMode: 1H �㷨��ʶ 1���������� 2���������� 3����֤ǩ�� 4��������Կ 5��������Կ
pkLength: ���빫Կ����
pk: nB/1A+3H DER����Ĺ�Կ��Կ��HSM��ȫ�洢���ڵ�λ��
signLength: ����֤ǩ��ֵ���ȣ�encryMode = 3ʱ�������
sign: nB ����֤ǩ��ֵ��encryMode = 3ʱ�������
keyLength: 1H ����/������Կ���ȣ�0 = ��������Կ��1 = ˫������Կ��encryMode = 4 �� encryMode = 5ʱ�������
inputDataLength: �ӽ������ݳ���
inputData: nB/1A+3H ���ӽ������ݻ���ӽ�����Կ��HSM��ȫ�洢���ڵ�����
�������:
outputData: nB �ӽ���/�ӽ�����Կ����
����ֵ:
>0      ouputData�ĳ���
<0      ʧ��
 */     
int UnionTASSHsmCmdTJ(char encryMode, int pkLength, char *pk, int signLength, char *sign, char keyLength, int inputDataLength, char *inputData, char *outputData);

#endif


#ifndef _UnionSJJ1127Cmd_
#define _UnionSJJ1127Cmd_

#include "unionDesKey.h"

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr);

// ת����Կ���ȣ�������Կ�ĳ���
int UnionTranslateSJJ1127HsmKeyLength(TUnionDesKeyLength length,char *lenStr);

/*      
�������ܣ�
        18ָ���ָ����Կ����ת����
���������      
        transformID��ת����ʽ
        oriPKIndex: Դ������Կ����
        lenOfOriPK: Դ������Կ����
        oriPKValue: Դ������Կ
        desPKIndex: Ŀ�ı�����Կ����
        lenOfDesPK: Ŀ�ı�����Կ����
        desPKValue: Ŀ�ı�����Կ
        lenOfOriKey����ת���ܵ���Կ����
        oriKeyValue����ת���ܵ���Կ����
        oriKeyCheckValue����ԿУ��ֵ
                
���������              
        desKeyValue��ת���ܺ�����
        desKeyCheckValue����ԿУ��ֵ
*/              
int UnionSJJ1127Cmd18(char *transformID,int oriPKIndex,TUnionDesKeyLength lenOfOriPK,char *oriPKValue,int desPKIndex,TUnionDesKeyLength lenOfDesPK,char *desPKValue,TUnionDesKeyLength lenOfOriKey,char *oriKeyValue,char *oriKeyCheckValue,char *desKeyValue,char *desKeyCheckValue);

/*
�������ܣ�
        50ָ���ָ����Կ��ָ�����ݽ��мӽ���
���������
        algorithmID���㷨��ʶ
                ֵ00- SM1
                ֵ 01- DES ECB
                ֵ 02- DES CBC
                ֵ03- DES CFB
                ֵ04- DES OFB
        encryptID����/���ܱ�־��1�����ܣ�0�����ܣ�
        indexOfKey: ��Կ������
        lenOfKey: ��Կ����
        keyValue: ��Կ����
        iv����ʼ����
        lenOfData: ���ݿ鳤��
        data�����ݿ�
���������
        lenOfResData: ���ݿ鳤��
        resData: ���ݿ�
*/
int UnionSJJ1127Cmd50(char *algorithmID,char *encryptID,int indexOfKey,TUnionDesKeyLength lenOfKey,char *keyValue,char *iv,int lenOfData,char *data,int *lenOfResData,char *resData);

/*
�������ܣ�
        60ָ����ɲ���ӡ��Կ
���������
        lenOfKey: ��Կ���ȱ�־
        numOfComponent: �ɷ���
        keyName����Կ����
���������
        keyValue: �����Կ
        keyCheckValue: ��Կ��У��ֵ
*/
int UnionSJJ1127Cmd60(TUnionDesKeyLength lenOfKey,int numOfComponent,char *keyName,char *keyValue,char *keyCheckValue);

/*
�������ܣ�
        62ָ�װ�ش�ӡ��ʽ
���������
        formatType: ��ʽ����
        format: ��ӡ��ʽ
���������
        ��
*/
int UnionSJJ1127Cmd62(char *formatType,char *format);

/*
�������ܣ�
        16ָ���ȡָ����Կ��������Կ
���������
        indexOfKey: ��Կ������
���������
        keyValue: �����Կ
        keyCheckValue: ��Կ��У��ֵ
����ֵ��
        <0������ִ��ʧ��
        =0���ɹ�
*/
int UnionSJJ1127Cmd16(int indexOfKey,char *keyValue,char *keyCheckValue);

/*
�������ܣ�
        14ָ�����Կ����д��ָ����Կ����
���������
        lenOfKey: ��Կ���ȱ�־
        keyValue: �����Կ
        keyCheckValue: ��Կ��У��ֵ
        indexOfKey: ��Կ������
���������
        ��
*/
int UnionSJJ1127Cmd14(TUnionDesKeyLength lenOfKey,char *keyValue,char *keyCheckValue,int indexOfKey);

/*
�������ܣ�
        90ָ������������Կ
���������
        lenOfKey: ��Կ���ȱ�־
        indexOfKey: ��Կ������
���������
        keyValue: �����Կ
        keyCheckValue: ��Կ��У��ֵ
*/
int UnionSJJ1127Cmd90(TUnionDesKeyLength lenOfKey,int indexOfKey,char *keyValue,char *keyCheckValue);

/*
�������ܣ�
        U2ָ�ʹ��ָ����Ӧ������Կ����2����ɢ�õ���ƬӦ������Կ��
                ʹ��ָ��������Կ����Կ���м��ܱ������������MAC���㡣
���������
        securityMech: ��ȫ����(S����DES���ܺ�MAC, T����DES���ܺ�MAC)
        mode: ģʽ��־, 0-������ 1-���ܲ�����MAC
        id: ����ID, 0=M/Chip4(CBCģʽ��ǿ�����X80) 1=VISA/PBOC(������ָ����ECB) 2=PBOC1.0ģʽ(ECBģʽ������������)

        mkIndex:        FFFF��ʹ��ָ���д���ĸ���Կ
                        0000-03E8����Կ����
                        �������Ƿ�
        lenOfMK:        ��������Կ����ΪFFFFʱ�д���
                        ֵ01- 64bits
                        ֵ02- 128bits
                        ֵ03- 192bits
        mk:             MK�����µĸ���Կ������
        mkDvsNum: ����Կ��ɢ����, 1-3��
        mkDvsData: ����Կ��ɢ����, n*16H, n������ɢ����

        pkIndex:        FFFF��ʹ��ָ���д���ı�����Կ
                        0000-03E8����Կ����
                        �������Ƿ�
        lenOfPK:        ����������Կ����ΪFFFFʱ�д���
                        ֵ01- 64bits
                        ֵ02- 128bits
                        ֵ03- 192bits
        pk:             MK�����µı�����Կ������
        pkDvsNum: ������Կ��ɢ����,������������Կ���͡�Ϊ1ʱ��,��MK-SMC��ɢ�õ�DK-SMC����ɢ����,��ΧΪ1-3
        pkDvsData: ������Կ��ɢ����,n*16H������������Կ���͡�Ϊ1ʱ��,������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������

        proKeyFlag:������Կ��ʶ��Y:���������Կ��N:�����������Կ����ѡ��:��û�и���ʱȱʡΪN
        proFactor: ��������(16H),��ѡ��:����������Կ��־ΪYʱ��

        ivCbc: IV-CBC,8H ����������ID��Ϊ0ʱ��

        encryptFillDataLen: ����������ݳ���(4H),����������ID��Ϊ2ʱ�У�������1024��
                ����Կ����һ����м��ܵ����ݳ���
        encryptFillData: ����������� nB ����������ID��Ϊ2ʱ��,����Կ����һ����м���
        encryptFillOffset: �����������ƫ���� 4H ����������ID��Ϊ2ʱ��
                ����Կ���Ĳ��뵽����������ݵ�λ��, ��ֵ������0������������ݳ���֮��

        ivMac: IV-MAC,16H ������ģʽ��־��Ϊ1ʱ��
        macDataLen: MAC������ݳ��� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC��������ݳ���
        macData: MAC������� nB ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC���������
        macOffset: ƫ���� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ���Ĳ��뵽MAC������ݵ�λ��
                ��ֵ������0��MAC������ݳ���֮��
���������
        mac: MACֵ 8B ������ģʽ��־��Ϊ1ʱ��
        criperDataLen: �������ݳ��� 4H �������ݳ���(������8�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
        criperData: nB �������������
����ֵ��
        <0������ִ��ʧ��
        =0���ɹ�
*/
int UnionSJJ1127CmdU2(char *securityMech, char *mode, char *id, int mkIndex,TUnionDesKeyLength lenOfMK,char *mk,int mkDvsNum, char *mkDvsData, int pkIndex, TUnionDesKeyLength lenOfPK, char *pk,int pkDvsNum, char *pkDvsData, char *proKeyFlag, char *proFactor, char *ivCbc,int encryptFillDataLen, char *encryptFillData, int encryptFillOffset,char *ivMac, int macDataLen, char *macData, int macOffset,char *mac, int *criperDataLen, char *criperData);

#endif


#ifndef UNION_GENKEY_BYPK
#define UNION_GENKEY_BYPK

//�������������des��Կ
char *UnionGenDESKey(char *outKeybuf);

//�������˫����des��Կ
char *UnionGen2DESKey(char *outKeybuf);

//�������3����des��Կ
char *UnionGen3DESKey(char *outKeybuf);

/*�������2����des,����ǿ��Ϊ1024��PK�������*/
/*
���룺1024bits��PK�㹫Կ,��չ�Ŀɼ��ַ���
�����DesKey, �漴������des��Կ����
���: DesKeyByPK,����PK���ܵ�des��Կ,��չ�Ŀɼ��ַ���
���أ�
    >= 0  �ɹ�
    <0   ʧ��
*/
int UnionGen2DesKeyBy1024PK(char *PK,char *DesKey,char *DesKeyByPK);

//��PK����DES��Կ
/*
���룺1024bits��PK�㹫Կ,��չ�Ŀɼ��ַ���
���룺DesKey, 2des��Կ����
���: DesKeyByPK,����PK���ܵ�des��Կ,��չ�Ŀɼ��ַ���
���أ�
    >= 0  �ɹ�
    <0   ʧ��
*/
int UnionEncDesKeyBy1024PK(char *PK,char *pDesKey,char *DesKeyByPK);

#endif


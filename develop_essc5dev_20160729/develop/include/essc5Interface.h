#ifndef _Essc5Interface_H_
#define _Essc5Interface_H_

#include "commWithHsmSvr.h"

// ���������״̬
int UnionDealServiceCodeE100(PUnionHsmGroupRec phsmGroupRec);
// ִ�������ָ��
int UnionDealServiceCodeE101(PUnionHsmGroupRec phsmGroupRec);
// ���ɶԳ���Կ
int UnionDealServiceCodeE110(PUnionHsmGroupRec phsmGroupRec);
// ���¶Գ���Կ
int UnionDealServiceCodeE111(PUnionHsmGroupRec phsmGroupRec);
// ����Գ���Կ
int UnionDealServiceCodeE112(PUnionHsmGroupRec phsmGroupRec);
// �����Գ���Կ
int UnionDealServiceCodeE113(PUnionHsmGroupRec phsmGroupRec);
// ����Գ���Կ
int UnionDealServiceCodeE114(PUnionHsmGroupRec phsmGroupRec);
// �ַ��Գ���Կ
int UnionDealServiceCodeE115(PUnionHsmGroupRec phsmGroupRec);
// ���ٶԳ���Կ
int UnionDealServiceCodeE116(PUnionHsmGroupRec phsmGroupRec);
// ��ȡ�Գ���Կ��Ϣ
int UnionDealServiceCodeE117(PUnionHsmGroupRec phsmGroupRec);
// ��ӡ��Կ
int UnionDealServiceCodeE118(PUnionHsmGroupRec phsmGroupRec);
// ����/����Գ���Կ
int UnionDealServiceCodeE119(PUnionHsmGroupRec phsmGroupRec);
// ���ɷǶԳ���Կ
int UnionDealServiceCodeE120(PUnionHsmGroupRec phsmGroupRec);
// ���·ǶԳ���Կ
int UnionDealServiceCodeE121(PUnionHsmGroupRec phsmGroupRec);
// ���빫Կ
int UnionDealServiceCodeE122(PUnionHsmGroupRec phsmGroupRec);
// ������Կ
int UnionDealServiceCodeE123(PUnionHsmGroupRec phsmGroupRec);
// ����ǶԳ���Կ
int UnionDealServiceCodeE124(PUnionHsmGroupRec phsmGroupRec);
// �ַ��ǶԳ���Կ
int UnionDealServiceCodeE125(PUnionHsmGroupRec phsmGroupRec);
// ���ٷǶԳ���Կ
int UnionDealServiceCodeE126(PUnionHsmGroupRec phsmGroupRec);
// ��ȡ�ǶԳ���Կ��Ϣ
int UnionDealServiceCodeE127(PUnionHsmGroupRec phsmGroupRec);
// ����PIN
int UnionDealServiceCodeE130(PUnionHsmGroupRec phsmGroupRec);
// ����PVV
int UnionDealServiceCodeE131(PUnionHsmGroupRec phsmGroupRec);
// ����PIN Offset
int UnionDealServiceCodeE132(PUnionHsmGroupRec phsmGroupRec);
// ����CVV
int UnionDealServiceCodeE133(PUnionHsmGroupRec phsmGroupRec);
// ����PIN
int UnionDealServiceCodeE140(PUnionHsmGroupRec phsmGroupRec);
// ����PIN
int UnionDealServiceCodeE141(PUnionHsmGroupRec phsmGroupRec);
// ת����PIN
int UnionDealServiceCodeE142(PUnionHsmGroupRec phsmGroupRec);
// ��֤PIN
int UnionDealServiceCodeE143(PUnionHsmGroupRec phsmGroupRec);
// ת����PIN Offset
int UnionDealServiceCodeE144(PUnionHsmGroupRec phsmGroupRec);
// ��֤CVV
int UnionDealServiceCodeE145(PUnionHsmGroupRec phsmGroupRec);
// ����MAC
int UnionDealServiceCodeE150(PUnionHsmGroupRec phsmGroupRec);
// ��֤MAC
int UnionDealServiceCodeE151(PUnionHsmGroupRec phsmGroupRec);
// ��֤������MAC
int UnionDealServiceCodeE152(PUnionHsmGroupRec phsmGroupRec);
// ��������
int UnionDealServiceCodeE160(PUnionHsmGroupRec phsmGroupRec);
// ��������
int UnionDealServiceCodeE161(PUnionHsmGroupRec phsmGroupRec);
// ˽Կǩ��
int UnionDealServiceCodeE170(PUnionHsmGroupRec phsmGroupRec);
// ��Կ��ǩ
int UnionDealServiceCodeE171(PUnionHsmGroupRec phsmGroupRec);
// ��Կ����
int UnionDealServiceCodeE172(PUnionHsmGroupRec phsmGroupRec);
// ˽Կ����
int UnionDealServiceCodeE173(PUnionHsmGroupRec phsmGroupRec);

/***************************************
�������:       E162
������:         RC4 ��������
��������:       RC4 ��������
add by zhangbs 20140627
***************************************/
int UnionDealServiceCodeE162(PUnionHsmGroupRec phsmGroupRec);

//���������״̬
int UnionDealServiceCodeE300(PUnionHsmGroupRec phsmGroupRec);
//У��ARQC
int UnionDealServiceCodeE301(PUnionHsmGroupRec phsmGroupRec);
//����ARPC
int UnionDealServiceCodeE302(PUnionHsmGroupRec phsmGroupRec);
//���ܽű�����
int UnionDealServiceCodeE303(PUnionHsmGroupRec phsmGroupRec);
//�ű����ݼ���mac
int UnionDealServiceCodeE304(PUnionHsmGroupRec phsmGroupRec);

//���������״̬
int UnionDealServiceCodeE200(PUnionHsmGroupRec phsmGroupRec);
//����PINת��(PK->ZPK)
int UnionDealServiceCodeE201(PUnionHsmGroupRec phsmGroupRec);
//�ַ�PINת��(PK->ZPK)
int UnionDealServiceCodeE202(PUnionHsmGroupRec phsmGroupRec);
//�ַ�PINת��(ZPK->ZPK)
int UnionDealServiceCodeE203(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeE204(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeE205(PUnionHsmGroupRec phsmGroupRec);

//PKCS7ǩ��
int UnionDealServiceCodeE501(PUnionHsmGroupRec phsmGroupRec);
//PKCS7��֤ǩ��
int UnionDealServiceCodeE502(PUnionHsmGroupRec phsmGroupRec);
//PKCS7����
int UnionDealServiceCodeE503(PUnionHsmGroupRec phsmGroupRec);
//PKCS7����
int UnionDealServiceCodeE504(PUnionHsmGroupRec phsmGroupRec);

// ���㰲ȫ����
int UnionDealServiceCodeE601(PUnionHsmGroupRec phsmGroupRec);
// ʹ����ɢ��Կ�ӽ������ݻ����MAC
int UnionDealServiceCodeE602(PUnionHsmGroupRec phsmGroupRec);
// ʹ����ɢ��Կת����PIN
int UnionDealServiceCodeE603(PUnionHsmGroupRec phsmGroupRec);


// ���ɱ��б���
int UnionDealServiceCodeE401(PUnionHsmGroupRec phsmGroupRec);
// �������а󶨱���
int UnionDealServiceCodeE402(PUnionHsmGroupRec phsmGroupRec);
// ����ǩ����ǩ
int UnionDealServiceCodeE403(PUnionHsmGroupRec phsmGroupRec);
// ����ǩ����ǩ
int UnionDealServiceCodeE404(PUnionHsmGroupRec phsmGroupRec);

// �����ǹ㷢����CGB�ͻ�������
// ���������״̬
int UnionDealServiceCodeEEA0(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEA1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEA2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEA3(PUnionHsmGroupRec phsmGroupRec);

// �����ǰ����Ʒ���ӿ�
int UnionDealServiceCodeEEB0(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB3(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB4(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB5(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB6(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB7(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB8(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEB9(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBA(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBB(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBC(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBD(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBE(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBF(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBG(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBH(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBI(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBJ(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBK(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEBL(PUnionHsmGroupRec phsmGroupRec);

// ���ɹ��籣����
int UnionDealServiceCodeEEC1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC3(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC4(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC5(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC6(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC7(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC8(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEC9(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEECA(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEECB(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEECC(PUnionHsmGroupRec phsmGroupRec);

//��ԭ���ж���
int UnionDealServiceCodeEED1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEED2(PUnionHsmGroupRec phsmGroupRec);

//����ũ�Ŷ���

int UnionDealServiceCodeEEF1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEF2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEF3(PUnionHsmGroupRec phsmGroupRec);

//���ս�ͨ������
int UnionDealServiceCodeJ001(PUnionHsmGroupRec phsmGroupRec);

// ��ͨ������
int UnionDealServiceCodeEEG1(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG2(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG3(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG4(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG5(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG6(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG7(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG8(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEG9(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGA(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGB(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGC(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGD(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGE(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGF(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGG(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGH(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGI(PUnionHsmGroupRec phsmGroupRec);
int UnionDealServiceCodeEEGJ(PUnionHsmGroupRec phsmGroupRec);

// ����ũ����
int UnionDealServiceCodeEEZ1(PUnionHsmGroupRec phsmGroupRec);

#endif //_Essc5Interface_H_

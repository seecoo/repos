#ifndef _UNION_JKS_FUN_
#define _UNION_JKS_FUN_

#define MAX_CERTCHAIN_NUM  5  //֤�������ļ���

/*
���ܣ���˽Կ���ܳ�JKS˽Կ����
����:
	VK: PKCS8��ʽ˽Կ
	passwd: ���ܿ���
	encryptVK: JKS˽Կ����
���أ�
	>=0 �ɹ�
	<0  ʧ��
*/
int UnionEncrytJKSPrivate(char *VKOfPKcs8,char *passwd,char *encryptVK);

/*
���ܣ���DER��ʽ��˽Կ�����PKCS8�ĸ�ʽ
���룺
	vkAschex: der��ʽ��˽Կ,��չ�Ŀɼ��ַ�
�����
	pkcs8VkAschex��	PKCS8�ĸ�ʽ��˽Կ,��չ�Ŀɼ��ַ�
���أ�
	>=0 �ɹ�
	<0  ʧ��
*/
int UnionPackPkcs8PrivateKeyInfo(char *vkAschex,char *pkcs8VkAschex);

/*
���ܣ���˽Կ��֤�飬����֤����JKS�ļ���ʽ����
���룺
	alias: �洢��JKS(keystore)�ı���
	passwd: ˽Կ��������
	certAscHex: ֤��,��չder��ʽ��X509֤��
	certChain: ֤��������, ÿ��֤��ĸ�ʽΪ��չder��ʽ��X509֤��
	certChainNum��֤����ʵ�ʰ�����֤����
	jksFileName: Ҫ���ɵ�JKS�ļ�������·��
	jskPasswd: jks������

���أ�
	>=0 �ɹ�
	<0  ʧ��
*/
int UnionJksStore(char *alias,char *vk,char *passwd, char *certAscHex, char certChain[MAX_CERTCHAIN_NUM][4096],int certChainNum,char *jksFileName,char *jskPasswd);

/*
���ܣ���pfx֤���ļ�ת��ΪJKS�ļ���ʽ
���룺
	pfxFileName: pfx֤���ļ�,��·��
	passwd: pfx֤�鱣������
	jksFileName: Ҫת�����ɵ�JKS�ļ�������·��
	jskPasswd: jks������
���أ�
	>=0 �ɹ�
	<0  ʧ��
*/
int UnionPfx2JKS(char *pfxFileName,char *passwd, char *jksFileName,char *jksPasswd);

/*
���ܣ���֤�������֤�鰴˳������
���룺
	certChain: ֤���ַ�������
	certChainNum: ֤�����
	descFlag: ������0�����ϵ���˳�򣨴Ӹ�CA����һ��CA��, 1: �����������򣨴����һ��CA����CA��
*/
int UnionSortCert(char certChain[MAX_CERTCHAIN_NUM][4096],int certChainNum,int descFlag,char orderCertChain[MAX_CERTCHAIN_NUM][4096]);

#endif


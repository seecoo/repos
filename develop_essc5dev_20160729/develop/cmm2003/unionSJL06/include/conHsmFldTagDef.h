// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2006/8/10
// Version:	1.0

#ifndef _esscFldTagDef_
#define _esscFldTagDef_


#define certInfoSubjectDNTag     0   //֤��Ψһ��ʾ��subject
#define certInfoIssuerDNTag      1   //�䷢��Ψһ��ʾ��issuer
#define certInfoSnTag            2   //֤�����к�SN(H���ַ���)
#define certInfoBeforeTimeTag    3   //��Чʱ��(YYYYMMDDHHMMSS)
#define certInfoAfterTimeTag     4   //����ʱ��(YYYYMMDDHHMMSS)
#define certInfoAlgIDTag         5   //֤�鹫Կ�㷨��ʶ(��RSA����SM2��)
#define certInfoPKTag            6   //֤�鹫Կ����(RSA��Կ��ʽΪDER Hex��ʽ,SM2��Կ Ϊ��04||xHex||yHex��)


#define conHsmFldDeviceHandle  100  //�豸���
#define conHsmFldSessionHandle 101  //�Ự���
#define conHsmFldDeviceInfo	   105  //�豸��Ϣ
#define conHsmFldRandom        106  //�����
#define conHsmFldRandomLength  107  //���������
#define conHsmFldKeyIndex      108  //�����豸�洢˽Կ������ֵ
#define conHsmFlducPassword    110  //ʹ��˽ԿȨ�޵ı�ʶ��
#define conHsmFldPwdLength     111  //˽Կ���ʿ����볤��

#define conHsmFlducPublicKey   115     //��Կ�ṹ
#define conHsmFlducTmpPublicKey   116  //��ʱ��Կ�ṹ
#define conHsmFlducPrivateKey  117     //RSA˽Կ�ṹ

#define  conHsmFlduiIPKIndex   118   //�����豸�ڲ��洢��Կ������ֵ
#define  conHsmFlduiISKIndex   119   //�����豸�ڲ��洢˽Կ������ֵ

#define  conHsmFlduiKeyBits        120   //�Ự��Կ����
#define  conHsmFldKeyValue         122   //��Կ����
#define  conHsmFldKeyValueLength   123   //��Կ����
#define  conHsmFldKeyHandle        124   //��Կ���



#define  conHsmFlducSponsorID      130  //��ԿЭ��ID
#define  conHsmFlduiSponsorIDLength      131  //��ԿЭ��ID����

#define  conHsmFldphAgreementHandle   133 //Э����Կ���

#define  conHsmFlducResponseID  	    140  //��Ӧ��IDֵ
#define  conHsmFlduiResponseIDLength   141 //��Ӧ��ID����

#define  conHsmFldAlgID   145 //��Կ���㷨��ʶ
#define  conHsmFldDataIV   146 //IV����

#define  conHsmFldDataInput 	 150 //������ָ�룬���ڴ�����������
#define  conHsmFldInputLength	 151//��������ݳ���
#define  conHsmFldDataOutput	 160//������ָ�룬���ڴ�����������
#define  conHsmFldOutputLength	 161//��������ݳ���

#define  conHsmFldSignature     170 //ǩ�����ݽṹ
	
#define  conHsmFlducData 	       31       //�������������
#define  conHsmFlduiDataLength    181	    //������������ĳ���	

#define  conHsmFlducMAC           183
#define  conHsmFlduiMACLength     184

#define  conHsmFlducID          186
#define  conHsmFlduiIDLength     187

#define  conHsmFlducHash         188
#define  conHsmFlduiHashLength   189

		
#define  conHsmFldFileName          190   //������ָ�룬���ڴ��������ļ���
#define  conHsmFldFileNameLen       191   //�ļ�������
#define  conHsmFldFileuiFileSize    196   // file size
#define  conHsmFldFileOffset        192   //ָ��д���ļ�ʱ��ƫ��ֵ
#define  conHsmFldFileLength        193   //ָ��д���ļ����ݵĳ���
#define  conHsmFldFileBuffer        194   //��������д�ļ�����	
	
#define conBankName  134	   //������
#define conCertData  136	   //֤������
#define conPageNo    137       //���ݼ�¼��ҳ��
#define conMaxNumPerPage  138  //ÿҳ������¼��
#define conTotalNum  139       //�ܵļ�¼��
#define conCurrentNum 140      //��ҳ��¼��
#define conCertDN     141      //֤��DN	
#define conCERTINFOissuer        142   //�䷢��DN
#define conCERTINFOserialNumber  143   //֤�����к�
#define conCERTINFOsubject       144   //֤������
#define conCERTINFOnotBefore     145   //֤����Ч�ڵ���ʼʱ��
#define conCERTINFOnotAfter      146   //֤����Ч�ڵ���ֹʱ��
#define conCERTINFOsignresult    147   //֤��ǩ�����
#define conCERTINFOcert          conCertData   //֤��Der����
#define conHsmFldAlgID2          155   //��Կ���㷨��ʶ
#define conFlag                  148    //��ʶ
#define conFlag1                 conFlag    //��ʶ1
#define conFlag2                 (conFlag+1)    //��ʶ2
#define conAPPID                 207     //Ӧ�ñ�ʶ
#define conBankCode              301	   //������
#define conVersion               410     //�汾��
#define conSignData              91     //ǩ��Ҫ��
#define conSignature             92     //����ǩ��
#define conOperatorID            214     //����������ʶ


#define conCountryName 331  //������
#define conStateOrProvinceName 332 //ʡ��
#define conLocalityName   333		//������
#define conOrganizationName  334  //��λ��
#define conOrganizationName2 335  //��λ��
#define conOrganizationalUnitName 336 //����
#define conOrganizationalUnitName2 337 //����
#define conCommonName  338		//�û���
#define conEmail   339			//EMail��ַ

//�������֧��
#define congkAlias             1     //����
#define congkInData           801   //��������
#define congkOutData          802   //�������
	
#define  conHsmECCPublicKey_bits     201   
#define  conHsmECCPublicKey_x		 202
#define  conHsmECCPublicKey_y        203

#define  conHsmECCPrivateKey_bits    206
#define  conHsmECCPrivateKey_D       207


#define  conHsmFldSignature_r       210
#define  conHsmFldSignature_s       211
	


#define  conHsmRSAPublicKey_BITS     	231   
#define  conHsmRSAPublicKey_M		232
#define  conHsmRSAPublicKey_E    		233


#define  conHsmRSAPrivateKey_BITS          241
#define  conHsmRSAPrivateKey_M             242
#define  conHsmRSAPrivateKey_E		 	   243
#define  conHsmRSAPrivateKey_D			   244
#define  conHsmRSAPrivateKey_PRIME         245
#define  conHsmRSAPrivateKey_PEXP         246
#define  conHsmRSAPrivateKey_COEF         247

#define  conHsmFldECCCipher_X			250
#define  conHsmFldECCCipher_Y			251
#define  conHsmFldECCCipher_C			252
#define  conHsmFldECCCipher_M			253
#define  conHsmFldECCCipher_L			254
	
#define  conHsmECCPublicKey_bits     201   
#define  conHsmECCPublicKey_x		 202   
#define  conHsmECCPublicKey_y        203    

#define  	conHsmECCPublicKeyTmp_bits    261
#define  	conHsmECCPublicKeyTmp_x	    262
#define  	conHsmECCPublicKeyTmp_y     263
		
#define  	conHsmFldContextHex			288
#define  	conHsmFldAgreementZA		289	
	
	
	
	
	

	
	
#define conEsscFldKeyName			1			// ��Կ������
#define conEsscFldFirstWKName			conEsscFldKeyName	// ��һ��������Կ������
#define conEsscFldSecondWKName			(conEsscFldKeyName+1)	// �ڶ���������Կ������
#define conEsscFldZMKName			11			// ZMK��Կ������
#define conEsscFldFirstZMKName			conEsscFldZMKName	// ��һ��ZMK��Կ������
#define conEsscFldSecondZMKName			(conEsscFldZMKName+1)	// �ڶ���ZMK��Կ������

#define conEsscFldMacData			21			// MAC����
#define conEsscFldMac				22			// MAC

#define conEsscFldPlainPin			31			// PIN����
#define conEsscFldPinOffset			32			// PINOffset
#define conEsscFldEncryptedPinByZPK		33			// ZPK���ܵ�PIN����
#define conEsscFldEncryptedPinByZPK1		conEsscFldEncryptedPinByZPK			// ZPK1���ܵ�PIN����
#define conEsscFldEncryptedPinByZPK2		(conEsscFldEncryptedPinByZPK+1)			// ZPK2���ܵ�PIN����
#define conEsscFldEncryptedPinByLMK0203		35			// LMK0203���ܵ�PIN����
#define conEsscFldVisaPVV			36			// Visa PVV
#define conEsscFldIBMPinOffset			37			// IBM PinOffset
#define conEsscFldPinByRsaPK			38			// RSAPK���ܵ�PIN

#define conEsscFldAccNo				41			// �˺�
#define conEsscFldAccNo1			conEsscFldAccNo			// �˺�1
#define conEsscFldAccNo2			(conEsscFldAccNo+1)		// �˺�2

#define conEsscFldKeyCheckValue			51				// ��ԿУ��ֵ
#define conEsscFldFirstKeyCheckValue		conEsscFldKeyCheckValue		// ��һ����ԿУ��ֵ
#define conEsscFldSecondKeyCheckValue		(conEsscFldKeyCheckValue+1)	// �ڶ�����ԿУ��ֵ

#define conEsscFldKeyValue			61			// ��Կֵ
#define conEsscFldFirstKeyValue			conEsscFldKeyValue	// ��һ����Կֵ
#define conEsscFldSecondKeyValue		(conEsscFldKeyValue+1)	// �ڶ�����Կֵ

#define conEsscFldVisaCVV			71			// VisaCVV
#define conEsscFldCardPeriod			72			// ����Ч��
#define conEsscFldServiceID			73			// �������

#define conEsscFldPlainData			81			// ��������
#define conEsscFldCiperData			82			// ��������

#define conEsscFldSignData			91			// ǩ������
#define conEsscFldSign				92			// ǩ��
#define conEsscFldSignDataPadFlag		93			// ǩ�����ݵ���䷽ʽ

#define conEsscFldDirectHsmCmdReq		100		// ֱ�ӵļ��ܻ�ָ��
#define conEsscFldDirectHsmCmdRes		101		// ֱ�ӵļ��ܻ�ָ����Ӧ

#define conEsscFldHsmGrpID			201		// �������
#define conEsscFldKeyContainer			202		// ��Կ����
#define conEsscFldKeyLenFlag			203		// ��Կ���ȱ�ʶ
#define conEsscFldForRemoteKMSvrFlag		204		// ΪԶ����Կ�������ı�־
#define conEsscFldForPinLength			205		// PIN�ĳ���
#define conEsscFldRefrenceNumber		206		// �ο�����
#define conEsscFldIDOfApp			207		// Ӧ�ñ��
#define conEsscFldLengthOfKey			208		// ��Կ����
#define conEsscFldSuccessHsmNum			209		// ִ��ָ��ɹ���HSM������
#define conEsscFldKeyIndex			210		// ��Կ����
#define conEsscFldHsmIPAddrList			211		// �����IP��ַ�б�
#define conEsscFldPKCodingMethod		212		// PK�ı��뷽ʽ
#define conEsscFldIV				213		// ��ʼ����
#define conEsscFldAlgorithmMode			214		// �㷨��־
#define conEsscFldKeyTypeFlag				221		// ��Կ���ͱ�ʾ
#define conEsscFldAlgorithm01Mode		conEsscFldAlgorithmMode		// ��һ���㷨��־
#define conEsscFldAlgorithm02Mode		(conEsscFldAlgorithmMode+1)	// �ڶ����㷨��־

#define conEsscFldAutoSignature			300		// �Զ���ֵ��ǩ��

// 2009/04/09����
#define conEsscFldRandNum			303				// �����
#define conEsscFldRandNum1			conEsscFldRandNum		// �����1
#define conEsscFldRandNum2			(conEsscFldRandNum+1)		// �����2
#define conEsscFldRandNum3			(conEsscFldRandNum+2)		// �����3
// 2009/04/09���ӽ���

// 2009/06/04����
#define conEsscFldARQC                          313                     // ARQC                 
#define conEsscFldARPC                          314                     // ARPC                 
#define conEsscFldARC                           315                     // ARC

#define conEsscFldAutoSign			400		// �Զ�ǩ��

// 2009/4/28,������,����
#define conEsscFldNodeID			401		// �ڵ��ʶ
#define conEsscFldFirstNodeID			conEsscFldNodeID		// ��һ���ڵ��ʶ
#define conEsscFldSecondNodeID			(conEsscFldNodeID+1)		// �ڶ����ڵ��ʶ
#define conEsscFldKeyVersion			410		// ��Կ�汾��
#define conEsscFldKeyActiveDate			411		// ��Կ��Ч����
#define conEsscFldKeyInactiveDate		412		// ��ԿʧЧ����
#define conEsscFldKeyReleaseDate		413		// ��Կ��������
#define conEsscFldKeyPlainValue			414		// ��Կ����ֵ
#define conEsscFldKeyModuleID			415		// ģ���ʶ
#define conEsscFldRecordNumber			416		// ��¼����
#define conEsscFldRecordList			417		// ������¼��
#define conEsscFldRecordStr			418		// Ψһ��¼��
#define conEsscFldKeyValueFormat		419		// ��Կֵ�ı�ʾ����
#define conEsscCertificateFormat		420		// ֤���ʽ
#define conEsscFirstCertificateFormat		conEsscCertificateFormat	// ��һ��֤���ʽ
#define conEsscSecondCertificateFormat		(conEsscCertificateFormat+1)	// �ڶ���֤���ʽ
#define conEsscDisperseTimes			425		// ��ɢ����
#define conEsscDisperseData			426		// ��ɢ����
#define conEsscARQC				427		// ARQC
#define conEsscARPC				428		// ARPC
#define conEsscARC				429		// ARC
// 2009/48,���ӽ���

#define conEsscFldPinCiperFormat		500					// PIN���ĵĸ�ʽ
#define conEsscFldFirstPinCiperFormat		conEsscFldPinCiperFormat		// ��һ��PIN���ĵĸ�ʽ
#define conEsscFldSecondPinCiperFormat		(conEsscFldPinCiperFormat+1)		// �ڶ���PIN���ĵĸ�ʽ

#define conEsscFldErrorCodeRemark		999		// ����������

// 2007/10/26����
#define conEsscFldRemark			998		// ��ע��Ϣ
#define conEsscFldResID				997		// ��Դ��ʶ
// 2007/10/26���ӽ���

#define conMngBankCode               110 //������
#define conMngBankName               111  //������
#define conMngCertData              112  //֤������
#define conMngIsAddtoCRL             113  //�Ƿ���������   1-����,0-������
#define conMngcertFlag                114  //֤���Ƿ����,1 -����   
#define conMngCertDN                   115     //֤��DN	
#define conMngCERTINFOissuer        116  //�䷢��DN
#define conMngCERTINFOserialNumber  117   //֤�����к�
#define conMngCERTINFOsubject       118  //֤������
#define conMngCERTINFOnotBefore     119   //֤����Ч�ڵ���ʼʱ��
#define conMngCERTINFOnotAfter      120   //֤����Ч�ڵ���ֹʱ��
#define conMngCERTINFOsignresult    121   //֤��ǩ�����
#define conMngCERTINFOcert          122   //֤��Der����
#define conMngTotalNum              123   //֤���ܵļ�¼��
#define conMngPageNo    124       //���ݼ�¼��ҳ��
#define conMngMaxNumPerPage  125  //ÿҳ������¼��
#define conMngCurrentNum 126      //��ҳ��¼��
#define conMngCertinfo   127     //֤����Ϣ
#define  conMngHsmFldFileName          128   //������ָ�룬���ڴ��������ļ���
#define  conMngCRLFlag     129  //CRL�Ƿ����,1 -����   
#define conMngKeyType  130  //��Կ���� 1--SM2,0--RSA
#define conMngFlducPassword    131  //ʹ��˽ԿȨ�޵ı�ʶ��

#define conFlag                  148    //��ʶ
#define conCountryName 331  //������
#define conStateOrProvinceName 332 //ʡ��
#define conLocalityName   333		//������
#define conOrganizationName  334  //��λ��
#define conOrganizationName2 335  //��λ��
#define conOrganizationalUnitName 336 //����
#define conOrganizationalUnitName2 337 //����
#define conCommonName  338		//�û���
#define conEmail   339			//EMail��ַ
#define conMngPKCS10Buf  340   //֤�������ļ�����
#define conMngCertSynDevinfo	136   //֤��ͬ�����豸��Ϣ
#define conMngCertSynTargetHsmIp	137 //֤��ͬ���豸IP
#define conMngCertSynTargetHsmport	138	//֤��ͬ���豸�˿�
#define conMngCertSynDevIsExist		139	//֤��ͬ���豸�Ƿ���� 0-������ 1-�Ѵ���

typedef struct
{
	int	tag;		// ���ʶ
	char	remark[40+1];	// ����˵��
} TUnionEsscFldTag;
typedef TUnionEsscFldTag	*PUnionEsscFldTag;

#define conMaxNumOfEsscFldTag	256
typedef struct
{
	int			realNum;
	TUnionEsscFldTag	tagList[conMaxNumOfEsscFldTag];
} TUnionEsscFldTagGroup;
typedef TUnionEsscFldTagGroup	*PUnionEsscFldTagGroup;
	
int UnionGetDefFileNameOfEsscFldTag(char *fileName);

// ���ַ����ж������ʶ����
int UnionReadEsscFldTagFromDefStr(char *str,PUnionEsscFldTag prec);

int UnionPrintEsscFldTagToFile(PUnionEsscFldTag prec,FILE *fp);

int UnionConnectEsscFldTagGroup();

int UnionDisconnectEsscFldTagGroup();

int UnionPrintEsscFldTagGroupToFile(FILE *fp);

int UnionPrintEsscFldTagGroupToSpecFile(char *fileName);

PUnionEsscFldTag UnionFindEsscFldTag(int tag);

char *UnionFindEsscFldTagRemark(int tag);

#endif


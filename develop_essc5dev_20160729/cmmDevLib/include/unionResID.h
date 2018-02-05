// 2008/9/27 wolfgang wang
#ifndef _unionResID_
#define _unionResID_

typedef enum
{
	conResTypeREC = 0,		// ���л���������Դ
	conResTransPack = 1,	// ���ױ�����Դ
	conResErrorInfo = 2,	// ������Ϣ��Դ
} TUnionResType;
	
// ���¶�����ԴID��
#define conResIDUsingTableName		970	//ʹ�ô��еı�����ѯ��ԴID
#define conResIDComplexField		73	//������ֵ������
#define conResIDSecondaryMenu		817	//�����˵�
#define conResIDViewList		823	//��ͼ
#define conResIDMainMenu		812	//���˵�
#define conResIDMenuDef			847	//�˵��鶨��
#define conResIDEnumValueDef		72	//ö��ֵ����
#define conResIDSecurityServiceDef	838	//��ȫ������

#define conResIDDynamicTBL		431	//��̬�����ö�̬����
#define conResIDMsgBuf			888	//��Ϣ������
#define conResIDHsmGrp			1	//�����������
#define conResIDTaskTBL			2	//����ע���
#define conResIDLogTBL			3	//��־ע���
#define conResIDREC			4	//�ͻ���������
#define conResIDKeyCache		5	//��Կ������
#define conResIDCommConf		6	//ͨѶ�ͻ��˱�
#define conResIDOperatorTBL		7	//����Ա��
//#define conResIDDesKeyDB		8	//DESKey��
#define conResIDDesKeyDB		222	//DESKey�� //hzh in 2011.8.9 �궨����8��Ϊ222������unionDataTBLList.h�ﶨ���һ��
#define conResIDPKDB			9	//PK��
#define conResIDRSADB			107	//RSA��
#define conResIDTransSpierBuf		10	//�����Ϣ������
#define conResIDLockTBL			11	// ����
#define conResIDHsmSckConnPool		12	// �����ͨѶ���ӳر�
#define conResIDClientSckConnPool	13	// �ͻ���ͨѶ���ӳر�
#define conResIDSecurityService		30	//��ȫ����
#define conResIDKeyMngService		31	//��Կ�������
#define conResIDHsmCmd			32	//�����ָ�����
#define conResIDMngService		33	//ϵͳ�������
#define conResIDCompareDesKey           34      //����ƽ̨֮�����Կ�ȶԽ��
#define conResIDRunningError		50	//ϵͳ���д���
#define conResIDSpierClientTBL		60	//��ؿͻ��˿��Ʊ�
#define conResIDTransSpierSvr		62	//ESSC���׼�ط���������һ��������Դ
#define conResIDResSpierSvr		63	//ESSC��Դ��ط���������һ��������Դ
#define conResIDTransSprerThread	64	//���׷����߳�	Add By Huangbaoxin,20081231
#define conResIDSessionContainer	65	//�Ự��	Add By Huangbaoxin,20081231
#define conResIDDataImageInMemory	66	//����ӳ��	Add By wolfgang wang 2009/1/8
#define conResIDPinPad			67	// �������	Add By xusj, 2009-2-5
#define conResIDKDBService		68	// ��Կ����
#define conResIDKDBSynchronizerService	69	// ��Կͬ������
#define conResIDMsgQueue		70	// ��Ϣ������ԴID
#define conResIDKDBBackuper		71	// ��Կ���ݷ���
#define conResIDKDBSvr			72	// ��Կӳ��

#define conResIDHsmError		100	//���������
#define conResIDError			101	//ESSC����
#define conResIDAppConf			102	//Ӧ������
#define conResIDLocalKMSvr		103	//������Կ�������������
#define conResIDRemoteKMSvr		104	//Զ����Կ�������������
#define conResIDKeyDBBackuper		105	//��Կ���ݷ�����
#define conResIDKDBSvrBuf		106	//��Կ�������Ļ���

#define conResIDOperationController	200	//��������
#define conResIDServiceDef		201	//��ȫ������
#define conResIDMngServiceDef		202	//���������
#define conResIDFldTagDef		203	//���ʶ����
#define conResIDSharedMemoryDef		204	//�����ڴ涨��
#define conResIDAutoRemoteKeyMngDef	205	//Զ����Կ�Զ�������
#define conResIDDefaultKeyPrintFormat	206	//ȱʡ��Կ��ӡ��ʽ�ļ�
#define conResIDDefaultHsmConf		207	//ȱʡ����������ļ�

#define conResIDKeyMng			300	// ��Կ����

// add in 20100825
#define conResIDDEAN			401

// add end

#define conResIDVarType			971	//���������嵥
#define conResIDTableField		972	//�����嵥
#define conResIDTableList		813	//���嵥
#define conResIDTableType		875	//�����

#define conResIDConfFileManager		999	//�����ļ�����Ա

// ���¶�����Դ��������
#define conResCmdInsert			1	// �����¼
#define conResCmdDelete			2	// ɾ����¼
#define conResCmdUpdate			3	// ����������¼
#define conResCmdLock			10	// ��ס��¼
#define conResCmdUnLock			11	// ������¼
#define conResCmdUnLockAllRec		12	// �������м�¼
#define conRecCmdUpdate			20	// ����������¼
#define conRecCmdUpdateAttrFld		21	// ����������
#define conRecCmdUpdateValueFld		22	// ����ֵ��
#define conResCmdRead			50	// ��ȡ��¼
#define conResCmdQuerySpecRec		conResCmdRead	// ��ȡ��¼
#define conResCmdQueryAllRec		51	// ��ѯ���м�¼
#define conResCmdQueryWithCondition	52	// ��ѯ�����������м�¼

#define conResCmdGenerateTBLDef		741	// ���ɱ���
#define conResCmdGenerateTBLCreateSQL	742	// ���ɽ����SQL���
#define conResCmdGenerateTBLQueryInterface	743	// ���ɱ���ͼ�Ľ���
#define conResCmdGenerateTBLIncFile	744	// ����ͷ�ļ�
#define conResCmdGenerateTBLCFile	745	// ����C����
#define conResCmdGenerateTBLIncAndCFile	746	// ����C�����ͷ�ļ�
#define conResCmdGenerateEnumDef	747	// ����ö��
#define conResCmdGenerateRecDoubleClickMenu	748	// ���ɼ�¼˫���˵�
#define conResCmdGenerateRecPopupMenu	749	// ���ɼ�¼�Ҽ��˵�
#define conResCmdGenerateFormPopupMenu	750	// ���ɽ����Ҽ��˵�
#define conResCmdGenerateTBLOperation	751	// ���ɱ�����嵥
#define conResCmdCreateTBLAnyway	752	// ���½���
#define conResCmdGenerateMngSvrTBLCreateFile	753	// ����mngSvr�����SQL���
#define conResCmdInsertInitRec		754	// �����ʼ��¼
#define conResCmdGenerateAllConfForTBL	755	// ���ɱ���Ҫ����������
#define conResCmdGenerateAllMenu	756	// �������в˵�
#define conResCmdGenerateMainMenu	757	// �������˵�
//#define conResCmdGenerateSecondaryMenuu	757	// ���ɶ����˵�
#define conResCmdGenerateSpec2LevelMenu	758	// ����ָ�������˵��˵�
#define conResCmdGenerateCreateAllTBLSQL	759	// �������еĽ���SQL���
#define conResCmdGenerateOperationInfo	780	// ���ɲ�����ʾ�ļ�
#define conResCmdGenerateTBLListIncFile	781	// ���ɱ��嵥�����ļ�
#define conResCmdGenerateOperationDefIncFile	782	// ���ɲ��������ļ�

#define conResCmdOutputTBLRec		783	// �ӱ��е�����¼
#define conResCmdInputTBLRec		784	// ����е����¼
#define conResCmdPrintRelationsOfSpecTBL	785	// ��ʾָ����Ĺ�ϵ
#define conResCmdPrintRelationsOfAllTBL	786	// ��ʾ���б�Ĺ�ϵ
#define conResCmdPrintAllTBLInOrder	787	// ������˳���ϵ��ʾ���б�
#define conResCmdCreateDropAllTBLFile	788	// ����ɾ�����б�����
#define conResCmdCreateDefaultViewOfTBL	789	// ������ȱʡ����ͼ
#define conResCmdDownloadEnumDef	780	// ��ȡö�ٶ����ļ�
#define conResCmdGetViewAuthorization	791	// ��ȡ��ͼ�Ĳ�����Ȩ
#define conResCmdCreateTableOnRealDB	642	// ���ݿ��ϴ������ش�����
#define conResCmdGenerateTBLStructDefIncFile	792	// ���ɱ�ṹ�����ͷ�ļ�
	
// ������ϵͳ�������
#define conResCmdApplicationStoped	999	// Ӧ�ó���ֹͣ����

// �����Ǽ������
#define conResCmdSendingSpiedRec	100	// ���ͼ�ؼ�¼
#define conResCmdStartSendingSpiedRec	98	// ��ʼ���ͼ�ؼ�¼
#define conResCmdFinishSendingSpiedRec	99	// �������ͼ�ؼ�¼

// �������ļ���������
#define conResCmdStartSendingData	200	// ��ʼ�����ļ�����
#define conResCmdSendingData		201	// �������ݼ�¼
#define conResCmdFinishSendingData	202	// ���������ļ�����
#define conResCmdDownloadFile		203	// ���������ļ�
#define conResCmdUploadFile		204	// ���������ļ�
#define conResCmdReloadResFile		205	// �ڷ����������¼�����Դ�ļ�
#define conResCmdQueryResStatus		206	// ��ѯ��Դ��״̬
#define conResCmdUnlockRes		207	// ������Դ
#define conResCmdUploadFileOnCondition	208	// �������������ļ�
#define conResCmdCreateFileDir		209	// �����ļ�Ŀ¼

// ������֪ͨ����

// ����ĳ�������������ɲ�ͬ����Դ���и������������塣
#define conResCmdSpec001		101	
#define conResCmdSpec002		102	
#define conResCmdSpec003		103	
#define conResCmdSpec004		104	
#define conResCmdSpec005		105	
#define conResCmdSpec006		106	
#define conResCmdSpec007		107
#define conResCmdSpec008		108
#define conResCmdSpec009		109
#define conResCmdSpec010		110
#define conResCmdSpec011		111

// ��ȫ�������
// ���ɰ�ȫ�������
#define conResSpecCmdOfSecuServiceDef_GenerateServiceDef	conResCmdSpec001

// ��Կ��������
// ��������һ����Կ�������ZMK���ܵ�����
#define conResSpecCmdOfKeyMng_GenerateKeyLocally		conResCmdSpec001
// ���ش洢һ��ZMK���ܵ���Կ
#define conResSpecCmdOfKeyMng_StoreKeyLocally		conResCmdSpec002
// ���ض�ȡһ��ZMK���ܵ���Կ
#define conResSpecCmdOfKeyMng_ReadKeyLocally		conResCmdSpec003
// �����������һ��ZMK���ܵ���Կ
#define conResSpecCmdOfKeyMng_GenerateRandomKeyLocally	conResCmdSpec004
// ��Զ���������ص�ǰ��Կ
#define conResSpecCmdOfKeyMng_ApplyCurrentKeyRemotely	conResCmdSpec005
// ��Զ��������������Կ
#define conResSpecCmdOfKeyMng_ApplyNewKeyRemotely		conResCmdSpec006
// ��Զ�̷ַ���ǰ��Կ
#define conResSpecCmdOfKeyMng_DeployCurrentKeyRemotely	conResCmdSpec007
// ��Զ�̷ַ�����Կ
#define conResSpecCmdOfKeyMng_DeployNewKeyRemotely		conResCmdSpec008
// ��ӡ��Կ�ŷ�
#define conResSpecCmdOfKeyMng_PrintKeyEnvolop		conResCmdSpec009
// ������Կ�ŷ��ʽ
#define conResSpecCmdOfKeyMng_SetKeyPrintFormat		conResCmdSpec010
// ��������Կ�ŷ��ʽ�ļ�����
#define conResSpecCmdOfKeyMng_DownloadAllKeyPrintFormatFile		conResCmdSpec011

// ��Կ�ⱸ�ݷ�������������
// ���ñ��ر��ݷ�����������
#define conResSpecCmdOfHsmGrp_SetMyself		conResCmdSpec001
// ����Զ�̱��ݷ�����������
#define conResSpecCmdOfHsmGrp_SetBrother		conResCmdSpec002
// ������Կ���ݷ�����
#define conResSpecCmdOfHsmGrp_Active		conResCmdSpec003
// �ر���Կ���ݷ�����
#define conResSpecCmdOfHsmGrp_Inactive		conResCmdSpec004

// ���ܻ�����������
// ��ѯ���м�¼�ļ�
#define conResSpecCmdOfHsmGrp_QueryAllRecFile	conResCmdSpec001
// ���������Ϊ�쳣״̬
#define conResSpecCmdOfHsmGrp_SetHsmAbnormal	conResCmdSpec002
// ���������Ϊ����״̬
#define conResSpecCmdOfHsmGrp_SetHsmNormal		conResCmdSpec003
// ���������Ϊ����״̬
#define conResSpecCmdOfHsmGrp_SetHsmBackup		conResCmdSpec004
// ����������Ĵ���ͳ��
#define conResSpecCmdOfHsmGrp_ResetHsmCounts	conResCmdSpec005
// ��������������Ĵ���ͳ��
#define conResSpecCmdOfHsmGrp_ResetAllHsmCounts	conResCmdSpec006
// ���������Ȩ
#define conResSpecCmdOfHsmGrp_AuthorizeHsm		conResCmdSpec007
// �������ȡ����Ȩ
#define conResSpecCmdOfHsmGrp_DisauthorizeHsm	conResCmdSpec008

// ��ͻ�����������
// ��ѯ����ָ��IP�Ŀͻ���
#define conResSpecCmdOfCommConf_QueryAllOfIPAddr	conResCmdSpec001
// ��ѯ����ָ��˵���Ŀͻ���
#define conResSpecCmdOfCommConf_QueryAllOfRemark	conResCmdSpec002
// ��ѯ����ָ���˿ڵĿͻ���
#define conResSpecCmdOfCommConf_QueryAllOfPort	conResCmdSpec003
// ɾ�����еĿͻ���
#define conResSpecCmdOfCommConf_DeleteIdleClient	conResCmdSpec004
// ����ָ���ͻ��˵�����
#define conResSpecCmdOfCommConf_ResetSpecRecAttr	conResCmdSpec005
//  �������пͻ��˵�����
#define conResSpecCmdOfCommConf_ResetAllRecAttr	conResCmdSpec006

// ��Ϣ��������������
// ������Ϣ�ȴ���������ʱ��
#define conResSpecCmdOfMsgBuf_ResetMaxStayTime	conResCmdSpec001
// ����������Ϣλ�ÿ���
#define conResSpecCmdOfMsgBuf_ResetAllPosAvailable	conResCmdSpec002
// ��ʾ���еȴ��������Ϣ
#define conResSpecCmdOfMsgBuf_QueryAllWaitingMsg	conResCmdSpec003

// ��־��������
// �����û���
#define conResSpecCmdOfLogTBL_ResetUsers		conResCmdSpec001

// ��ؿͻ��˿��Ʊ����������
// �򿪶���Դ�ļ��
#define conResSpecCmdOfSpierClientTBL_OpenRes	conResCmdSpec001
// �رն���Դ�ļ��
#define conResSpecCmdOfSpierClientTBL_CloseRes	conResCmdSpec002
// ����һ����Դ
#define conResSpecCmdOfSpierClientTBL_InsertRes	conResCmdSpec003
// ɾ��һ����Դ
#define conResSpecCmdOfSpierClientTBL_DeleteRes	conResCmdSpec004
// �޸�һ����Դ
#define conResSpecCmdOfSpierClientTBL_ModifyRes	conResCmdSpec005
// ����һ��ȱʡ�ͻ��˶���
#define conResSpecCmdOfSpierClientTBL_InsertDefaultClient	conResCmdSpec006
// ɾ��һ��ȱʡ�ͻ��˶���
#define conResSpecCmdOfSpierClientTBL_DeleteDefaultClient	conResCmdSpec007
// ��ѯ������Դ
#define conResSpecCmdOfSpierClientTBL_QueryAllRes	conResCmdSpec008
// ��ѯָ����Դ
#define conResSpecCmdOfSpierClientTBL_QuerySpecRes	conResCmdSpec009

// ������̹���
// �����������
#define conResSpecCmdOfPinPad_Add		conResCmdSpec001
#define conResSpecCmdOfPinPad_Update		conResCmdSpec002
#define conResSpecCmdOfPinPad_Delete		conResCmdSpec003
// ��ʼ���������
#define conResSpecCmdOfPinPad_Init		conResCmdSpec007
// �����������
#define conResSpecCmdOfPinPad_Active		conResCmdSpec008
// ͣ���������
#define conResSpecCmdOfPinPad_InActive		conResCmdSpec004
// ��ѯ�����������
#define conResSpecCmdOfPinPad_QueryAll		conResCmdSpec005
// ��ѯ�������
#define conResSpecCmdOfPinPad_QuerySpec 	conResCmdSpec006

// ��ؿͻ��˿��Ʊ����������
// ����Ա��¼
#define conResSpecCmdOfOperatorTBL_logon		conResCmdSpec001
// ����Ա�˳���¼
#define conResSpecCmdOfOperatorTBL_logoff		conResCmdSpec002
// ǿ�Ʋ���Ա�˳���¼
#define conResSpecCmdOfOperatorTBL_ForceLogoff	conResCmdSpec003

// ����������������
// ��ѯ���л����������
#define conResSpecCmdOfTaskTBL_QueryActiveClass	conResCmdSpec001
// ��ѯ���л������ʵ��
#define conResSpecCmdOfTaskTBL_QueryAllInstance	conResCmdSpec002
// ��ѯ�����쳣����������
#define conResSpecCmdOfTaskTBL_QueryAbnormalClass	conResCmdSpec003
// ��ѯ���б������ߵ���������
#define conResSpecCmdOfTaskTBL_QueryMustOnlineClass	conResCmdSpec004
//��ָֹ�����������ͳ���
#define conResSpecCmdOfTaskTBL_KillTaskOfSpecClass	conResCmdSpec005
// �ر�ָ�����������ͳ���
#define conResSpecCmdOfTaskTBL_CloseTaskOfSpecClass	conResCmdSpec006
// ���ݽ��̺���ָֹ��������
#define conResSpecCmdOfTaskTBL_KillSpecTaskInstance	conResCmdSpec007

//�������Ͷ�������
//���ݹ��������Զ������ĵ��滮���ĵ�����
#define conResSpecCmdOfWorkType_AutoAddDocRecByID	conResCmdSpec001

// ���ع����ڴ�ӳ��
#define conResCmdReloadSharedMemoryImage                                 881
// ��ʾ�����ڴ�ӳ��
#define conResCmdDisplaySharedMemoryImage                                882

// ��ȡ���˵��Ĳ˵���
#define conResCmdReadMainMenuItem	241
// ��ȡ�����˵��Ĳ˵���
#define conResCmdReadSecondaryMenuItem	242
// ��ȡ�����Ҽ��˵���
#define conResCmdReadInterfacePopupMenuItem	243
// ��ȡ��¼�Ҽ��˵���
#define conResCmdReadRecordPopupMenuItem	244
// ��ȡ��¼˫���˵���
#define conResCmdReadRecordDoubleClickMenuItem	245
// ��ȡ�������˵�
#define conResCmdReadAllMainMenuDef	246
// ��ȡ�˵����ִ������
#define conResCmdReadMenuItemCmd	251
// ��ȡ�Ҽ��˵����ִ������
#define conResCmdReadPopupMenuItem	252
// �޸�����
#define conResCmdUpdatePassword		231
// �Ӻ�̨��ȡһ�������
#define conResCmdGetOneRandomNumber	721

int UnionSetMyTransInfoResID(int resID);

int UnionGetMyTransInfoResID();

int UnionIsValidResType(TUnionResType type);

#endif

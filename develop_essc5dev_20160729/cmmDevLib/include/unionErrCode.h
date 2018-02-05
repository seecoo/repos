#ifndef _UnionErrCode_
#define _UnionErrCode_

#include <errno.h>
#include <stdlib.h>
#include <string.h>

// ������ƫ��������
#define errCodeOffsetOfDesKeyDBMDL		-11000	// DES��Կ��ģ��
#define errCodeOffsetOfPKDBMDL			-12000	// ������Կ��ģ��
#define errCodeOffsetOfTaskMDL			-13000	// ����ģ��
#define errCodeOffsetOfEnviMDL			-14000	// ��������ģ��
#define errCodeOffsetOfKeyDBBackuperMDL		-15000	// ��Կ�ⱸ��ģ��
#define errCodeOffsetOfLogMDL			-16000	// ��־ģ��
#define errCodeOffsetOfSharedMemoryMDL		-17000	// �����ڴ�ģ��
#define errCodeOffsetOfMsgBufMDL		-18000	// ��Ϣ������ģ��
#define errCodeOffsetOfOpertatorMDL		-19000	// ����Աģ��
#define errCodeOffsetOfRECMDL			-20000	// RECģ��
#define errCodeOffsetOfSckCommMDL		-21000	// ͨѶģ��
#define errCodeOffsetOfSocketMDL		-22000	// Socketģ��
#define errCodeOffsetOfSJL06MDL			-23000	// SJL06������ģ��
#define errCodeOffsetOfTransClassDefMDL		-24000	// ���׶���ģ��
#define errCodeOffsetOfPackageDefMDL		-25000	// ����ģ��
#define errCodeOffsetOfISO8583MDL		-26000	// 8583ģ��
#define errCodeOffsetOfEsscMDL			-29000	// �������ƽ̨ģ��
#define errCodeOffsetOfYLQZMDL			-30000	// ����ǰ�û�ģ��
#define errCodeOffsetOfKDBSvr			-31000	// ��Կ�������ģ��
#define errCodeOffsetOfDatabase			-32000	// ���ݿ�Ĵ�����
#define errCodeOffsetOfBankBasicPackageMDL	-33000	// һ�����б���ģ��
#define errCodeOffsetOfSJL08			-34000	// sjl08���Ĵ���
#define errCodeOffsetOfSQL			-35000	// SQLģ��Ĵ���	// 2008/7/24����
#define errCodeOffsetOfCmmPack			-36000	// ��������ģ��Ĵ���	// 2008/9/26����
#define errCodeOffsetOfCDP			-37000	// �������ƽ̨ģ��Ĵ���	// 2008/10/3����
#define errCodeOffsetOfFileSvr			-38000	// �ļ�������ģ��Ĵ���	// 2008/11/10����
#define errCodeOffsetOfObject			-39000	// ����Ĵ�����		// 2008/3/12����
#define errCodeOffsetOfLockMachanism		-40000	// ��������		// 2008/3/12����
#define errCodeOffsetOfHsm			-49000	// ��������صĴ�����	// 2008/11/19����
#define errCodeOffsetOfSimuUnixSharedMemory	-50000	// ���湲���ڴ�	// 2008/11/21����
#define errCodeOffsetOfPBOC			-52000  // PBOCģ��
#define errCodeOffsetOfOTPS			-51000	// ��̬����ϵͳ
#define errCodeOffsetOfDPS                      -53000  // ����׼��ϵͳ 
#define errCodeOffsetOfPKCS11			-59000	// pkcs11ģ��		// 2008/3/12����
#define errCodeOffsetOfDataImageInMemory	-60000	// ����ӳ��		// 2009/1/8����
#define errCodeOffsetOfBinaryFileDB		-61000	// �������ļ�ģ��	// 2009/3/23����
#define errCodeOffsetOfOnlineKeyMng		-62000	// ������Կ����ģ��	// 2009/8/23����
#define errCodeOffsetOfFileTransSvrMDL		-63000	// �ļ�����ģ���ƫ��
#define errCodeOffsetOfCertificate		-64000	// ֤��ģ��		// 2012/3/30����
#define errCodeOffsetOfMemcached		-65000	// memcachedģ��
#define errCodeOffsetOfCertMDL			-69000	// ֤��ģ��
#define errCodeOffsetOfSJL05MDL			-70000	// SJL05������ģ��
#define errCodeOffsetOfHsmCmdMDL		-9000	// �����ָ��ģ��
#define errCodeOffsetOfCmbcEuspPackageMDL	-8000	// �������б���ģ��
#define errCodeOffsetOfYLCommConfMDL		-7000	// ����ͨѶģ��
#define errCodeOffsetOfKeyCacheMDL		-6000	// ��Կ����ģ��
#define errCodeOffsetOfAPI			-5000	// APIģ��
#define errCodeOffsetOfTransSpierBufMDL		-4000	// ���׼��ģ��
#define errCodeOffset2OfEsscMDL			-3000	// �������ƽ̨ģ��2
#define errCodeOffsetOfErrCodeMDL		-2000	// �������ģ��
#define errCodeOffsetOfHsmReturnCodeMDL		-1000	// �����������
#define errCodeOffsetOfOracle			-900000	// ORACLE��Ӧ��ƫ��
#define errCodeOffsetOfDB2			-800000	// DB2��Ӧ��ƫ��
#define errCodeOffsetOfInformix			-700000	// INFORMIX��Ӧ��ƫ��
#define errCodeOffsetOfMysql			-600000	// Mysql��Ӧ��ƫ��
#define errCodeOffsetOfODBC			-500000	// ODBC��Ӧ��ƫ��
#define errCodeOffsetOfSqlite			-400000	// Sqlite��Ӧ��ƫ��
#define errCodeOffsetOfSRJ1401			-100000	// SRJ1401��Ӧ��ƫ��

// �����ǹ��ô�����
#define errCodeUseOSErrCode			(0-abs(errno))	// ʹ�ò���ϵͳ�Ĵ�����
#define errCodeParameter			-10001	// ��������
#define errCodeSharedMemoryModule		-10002	// �����ڴ��
#define errCodeCreateTaskInstance		-10003	// �����������
#define errCodeSmallBuffer			-10004	// ̫С�Ļ�����
#define errCodeInvalidIPAddr			-10005	// �Ƿ���IP��ַ
#define errCodeCallThirdPartyFunction		-10006	// ���õ�������������
#define errCodeTooShortLength			-10007	// ����̫С
#define errCodeUserSelectExit			-10008	// �û�ѡ�����˳�
#define errCodeDefaultErrCode			-10009	// ȱʡ������
#define errCodeIsRemarkLine			-10010	// ��ע����
#define errCodeNoneOperationDefined		-10011	// û�����������
#define errCodeNoneVarDefined			-10012	// û�������
#define errCodeTimeout				-10013	// ��ʱ
#define errCodeOutRange				-10014	// ����
#define errCodeUserForgetSetErrCode		-10015	// ����Ա�������ô�����
#define errCodeUserRequestHelpInfo		-10016	// �û�Ҫ�������Ϣ 2007/11/30����
#define errCodeTimerNotStart			-10017	// ��ʱû�п�ʼ 2008/6/20����
#define errCodeLiscenceCodeInvalid		-10018	// ��Ȩ���2008/7/16����
#define errCodeTcpipSvrNotDefined		-10019	// tcpipSvrû�ж���
#define errCodeFileEnd				-10020	// �ļ�������
#define errCodeRequestAndResponseNotSame	-10021	// ��������Ӧ��ƥ��
#define errCodeKeyWordAlreadyExists		-10022	// �ؼ����Ѵ���
#define errCodeKeyWordNotExists			-10023	// �ؼ��ֲ�����
#define errCodeKeyWordIsMyself			-10024	// �ؼ����Ǳ���
#define errCodeLockKeyWord			-10025	// ���ؼ��ֳ���
#define errCodeVarNameNotCorrect		-10026	// �������ƴ�
#define errCodeTestFileContentError		-10027	// ���������ļ������д�
#define errCodeSckConnNoData			-10028	// socket������û�д�������	Mary add, 20081225
#define errCodeMarcoDefNameNotDefined		-10029	// �궨������û����
#define errCodeMarcoDefValueNotDefined		-10030	// �궨��ֵû����
#define errCodeDefaultValueTooLong		-10031	// ȱʡֵ̫��
#define errCodeFileAlreadyExists		-10032	// �ļ��Ѵ���
#define errCodeNullPointer			-10033	// ��ָ��
#define errCodeNullRecPointer			-10034	// �ռ�¼ָ��
#define errCodeNullRecStrPointer		-10035	// �ռ�¼��ָ��
#define errCodeTableNameReserved		-10036	// �����Ǳ�������
#define errCodeTooManyLoopTimes			-10037	// ѭ��̫�����
#define errCodeNodeIDNotFoundInNetDef		-10038	// �ڵ㶨��û���ҵ�
#define errCodeTooManyParentsNode		-10039	// ���ڵ�̫��
#define errCodeTheTwoNodeIsTheSame		-10040	// ͬһ�ڵ�
#define errCodeNodeNotParentOfTheSpecNode	-10041	// ���Ǹýڵ�ĸ��ڵ�
#define errCodeWrongNetLayer			-10042	// �ڵ�����
#define errCodeTooManyNetNodes			-10043	// �ڵ�̫��
#define errCodeInvalidKeyDBType			-10044	// �Ƿ�����Կ������
#define errCodeErrCounterGrpIsFull		-10045	// �����������������
#define errCodeErrCounterGrpNotConnected	-10046	// �������������δ����
#define errCodeNameIsUsed			-10047	// �����ѱ�ʹ��
#define errCodeTooManyRetryTimes		-10048	// ̫�����Դ���
#define errCodeUserInputWrongParametersNum	-10049	// �û������˴���Ĳ�������
#define errCodeHelpToUser			-10050	// ���û��ṩ��������
#define errCodePeerCloseSckConn			-10051	// �Զ˹ر�������
#define errCodeAlgorithmUsingNonKey		-10052	// �㷨����Ҫ�õ���Կ
#define errCodeMngSvrClientParameter		-10053	// �ͻ��˲�������
#define errCodeMngSvrHsmIPAddrNotSet		-10054	// û��Ϊ���÷�����ָ�����ܻ�IP��ַ
#define errCodeMngSvrFileNotExists		-10055	// ���������ļ�������
#define errCodeFunctionNotSupported		-10056	// ��֧�ִ˺���
#define errCodeInvalidDataInFile		-10057	// �ļ����зǷ�����
#define errCodeInvalidReportMethod		-10058	// �Ƿ��ı�����
#define errCodeNoDataFoundInQueue		-10059	// ��������������
#define errCodeReportMethodNotDefined		-10060	// �������ɷ���û�ж���
#define errCodeTestingResultNotSameAsExpected	-10061	// ���Խ����Ԥ�ڲ���
#define errCodeInvalidFileName			-10062	// �Ƿ��ļ���
#define errCodeInvalidService			-10063	// �Ƿ�����
#define errCodeHashValueErr			-10064	// ��ϣֵ����
#define errCodeRecordNotExists                  -10065  //��¼������ 
#define errCodeDPFileCheck			-10066  // ���ʧ��

#define errCodeAPIParameter						(errCodeOffsetOfAPI-1)		// API��������
#define errCodeAPIReqStrTooLong						(errCodeOffsetOfAPI-2)		// ����̫��
#define errCodeAPIReqStrTooShort					(errCodeOffsetOfAPI-3)		// ����̫��
#define errCodeAPIWrongPackRecveived					(errCodeOffsetOfAPI-4)		// �յ�����İ�
#define errCodeAPICommWithEssc						(errCodeOffsetOfAPI-5)		// ��ESSCͨѶ����
#define errCodeAPIBufferSmallForRecvData				(errCodeOffsetOfAPI-6)		// ���ݻ��岻������յ�������
#define errCodeAPIRecvDataLenNotEqualDefinedLen				(errCodeOffsetOfAPI-7)		// �յ������ݳ����붨��ĳ��Ȳ���
#define errCodeAPISvrReturnZeroLenAnswer				(errCodeOffsetOfAPI-8)		// �����������˳���Ϊ0����Ӧ
#define errCodeAPISvrReturnTooShortAnswer				(errCodeOffsetOfAPI-9)		// �����������˳���̫�̵���Ӧ
#define errCodeAPISvrReturnWrongKeyLen					(errCodeOffsetOfAPI-10)		// ���������ص���Կ���ȴ�
#define errCodeAPIShouldReturnMinusButRetIsNotMinus			(errCodeOffsetOfAPI-11)		// Ӧ�÷���һ�����󣬵������еķ���ֵ���Ǹ���
#define errCodeAPIEsscSvrIPAddrNotDefined				(errCodeOffsetOfAPI-12)		// ESSCIPAddrû�ж���
#define errCodeAPIEsscSvrPortNotDefined					(errCodeOffsetOfAPI-13)		// ESSC�˿�û�ж���
#define errCodeAPIEsscSvrPortInvalid					(errCodeOffsetOfAPI-14)		// ESSC�˿ڴ���
#define errCodeAPIEsscSvrTimeoutInvalid					(errCodeOffsetOfAPI-15)		// ESSC�ĳ�ʱֵ�Ƿ�
#define errCodeAPIEsscSvrIDOfAppApiInvalid				(errCodeOffsetOfAPI-16)		// ESSC�Ŀͻ���Ӧ��ID
#define errCodeAPIClientReqLen						(errCodeOffsetOfAPI-17)		// �ͻ������󳤶ȴ�
#define errCodeAPIRecvClientReqTimeout					(errCodeOffsetOfAPI-18)		// ���տͻ�������ʱ
#define errCodeAPIErrCodeNotSet						(errCodeOffsetOfAPI-19)		// δ���ó�����
#define errCodeAPIPackageTooShort					(errCodeOffsetOfAPI-20)		// ������̫��
#define errCodeAPIPackageNotRequest					(errCodeOffsetOfAPI-21)		// ���������
#define errCodeAPIPackageNotResponse					(errCodeOffsetOfAPI-22)		// ������Ӧ��
#define errCodeAPIInvalidService					(errCodeOffsetOfAPI-23)		// �Ƿ�����

// �ļ�����ģ��Ĵ�����
#define errCodeFileTransSvrNoFileActionStrategyPrimaryKeyDefinedInReqStr		(errCodeOffsetOfFileTransSvrMDL-1)	// ������δ�����ļ����䶯���Ĺؼ���
#define errCodeFileTransSvrReadFileActionStrategy			(errCodeOffsetOfFileTransSvrMDL-2)	// ��ȡ�ļ�������Գ���
#define errCodeFileTransSvrThisLevelNotAuthorized			(errCodeOffsetOfFileTransSvrMDL-3)	// �ü���δ����Ȩ
#define errCodeFileTransSvrThisPersonNotAuthorized			(errCodeOffsetOfFileTransSvrMDL-4)	// ����δ����Ȩ
#define errCodeFileTransSvrInvalidClient				(errCodeOffsetOfFileTransSvrMDL-5)	// �Ƿ��ķ���ڵ�
#define errCodeFileTransSvrNoFileName					(errCodeOffsetOfFileTransSvrMDL-6)	// δָ���ļ���
#define errCodeFileTransSvrDataStrategyDefinedError			(errCodeOffsetOfFileTransSvrMDL-7)	// �ļ����ܶ����д�
#define errCodeFileTransSvrMacStrategyDefinedError			(errCodeOffsetOfFileTransSvrMDL-8)	// �ļ�У�鶨���д�
#define errCodeFileTransSvrSignStrategyDefinedError			(errCodeOffsetOfFileTransSvrMDL-9)	// �ļ���֤�����д�
#define errCodeFileTransSvrResendForbidden				(errCodeOffsetOfFileTransSvrMDL-10)	// ���������·���
#define errCodeFileTransSvrTransferFinished				(errCodeOffsetOfFileTransSvrMDL-11)	// �ѷ������
#define errCodeFileTransSvrTransferStillActive				(errCodeOffsetOfFileTransSvrMDL-12)	// �������ڽ�����
#define errCodeFileTransSvrPackAuthResStr				(errCodeOffsetOfFileTransSvrMDL-13)	// ƴװ��Ȩ��Ӧ������
#define errCodeFileTransSvrInsertOnlineTransLog				(errCodeOffsetOfFileTransSvrMDL-14)	// ���봫����ˮ����
#define errCodeFileTransSvrInvalidOriNode				(errCodeOffsetOfFileTransSvrMDL-15)	// �Ƿ���Դ�ڵ�
#define errCodeFileTransSvrInvalidDesNode				(errCodeOffsetOfFileTransSvrMDL-16)	// �Ƿ���Ŀ��ڵ�
#define errCodeFileTransSvrFileSizeNotDefined				(errCodeOffsetOfFileTransSvrMDL-17)	// �ļ���Сδ����
#define errCodeFileTransSvrFileSizeSmallThanOffset			(errCodeOffsetOfFileTransSvrMDL-18)	// �ļ���СС���ļ�ƫ��
#define errCodeFileTransSvrSenderMustApplyAuthorizationFirst		(errCodeOffsetOfFileTransSvrMDL-19)	// ���ͷ���δ������Ȩ
#define errCodeFileTransSvrInvalidServiceCode				(errCodeOffsetOfFileTransSvrMDL-20)	// �Ƿ��������
#define errCodeFileTransSvrInvalidTransDirection			(errCodeOffsetOfFileTransSvrMDL-21)	// �Ƿ��Ĵ��䷽��
#define errCodeFileTransSvrReadCurrentLogRec				(errCodeOffsetOfFileTransSvrMDL-22)	// ��ȡ��ǰ��ˮ��¼
#define errCodeFileTransSvrAuthorizationForbiddednOnCurrentStatus	(errCodeOffsetOfFileTransSvrMDL-23)	// ������Ե�ǰ״̬����ˮ������Ȩ
#define errCodeFileTransSvrInvalidTransStatusChange			(errCodeOffsetOfFileTransSvrMDL-24)	// �Ƿ��Ĵ���״̬�仯
#define errCodeFileTransSvrInvalidTransStatus				(errCodeOffsetOfFileTransSvrMDL-25)	// �Ƿ��Ĵ���״̬
#define errCodeFileTransSvrInvalidFileOffset				(errCodeOffsetOfFileTransSvrMDL-26)	// �Ƿ���ƫ��λ��
#define errCodeFileTransSvrInvalidDataLen				(errCodeOffsetOfFileTransSvrMDL-27)	// �Ƿ��Ĵ��䳤��
#define errCodeFileTransSvrInvalidFileSize				(errCodeOffsetOfFileTransSvrMDL-28)	// �ļ���С����
#define errCodeFileTransSvrInvalidFileControlType			(errCodeOffsetOfFileTransSvrMDL-29)	// �Ƿ����ļ���Ȩ����
#define errCodeFileTransSvrOneSideNotFinishTransmitting			(errCodeOffsetOfFileTransSvrMDL-30)	// ��һ����û����ɴ���
#define errCodeFileTransSvrCancelByCancelService			(errCodeOffsetOfFileTransSvrMDL-31)	// ������ֹ������ֹ�˴���
#define errCodeFileTransSvrCancelArtificially				(errCodeOffsetOfFileTransSvrMDL-32)	// �˹���ֹ�˴���
#define errCodeFileTransSvrInvalidEngineInstance			(errCodeOffsetOfFileTransSvrMDL-33)	// ��ͬ�Ĵ���ʵ��
#define errCodeFileTransSvrOriNodeDirNotSame				(errCodeOffsetOfFileTransSvrMDL-34)	// �������Ķ����Դ�ڵ�Ŀ¼�뱾�صĲ�һ��
#define errCodeFileTransSvrInvalidPackNo				(errCodeOffsetOfFileTransSvrMDL-35)	// �Ƿ��İ�����
#define errCodeFileTransSvrWithoutFinishedPack				(errCodeOffsetOfFileTransSvrMDL-36)	// ���ǽ�����
#define errCodeFileTransSvrSenderNotCloseConn				(errCodeOffsetOfFileTransSvrMDL-37)	// ���ͷ�δ�ر�����
#define errCodeFileTransSvrReceiverWriteFile				(errCodeOffsetOfFileTransSvrMDL-38)	// ���շ�д�ļ�����
#define errCodeFileTransSvrSendDataAndRecvDataLenNotSame		(errCodeOffsetOfFileTransSvrMDL-39)	// ���շ��ͷ��ͷ����Ȳ�һ��
#define errCodeFileTransSvrPlainDataLenErrorAfterDecrypt		(errCodeOffsetOfFileTransSvrMDL-40)	// ���ܺ�����ݳ��ȴ�
#define errCodeFileTransSvrFunTypeNotDefined				(errCodeOffsetOfFileTransSvrMDL-41)	// �Ƿ��Ĺ��ܱ�ʶ
#define errCodeFileTransSvrSecurityProtocolNotDefined			(errCodeOffsetOfFileTransSvrMDL-42)	// �Ƿ��İ�ȫЭ��
#define errCodeFileTransSvrMaxClientNo					(errCodeOffsetOfFileTransSvrMDL-43)	// �������Ŀͻ�������
#define errCodeFileTransSvrCreateFileReceiverHandleFail			(errCodeOffsetOfFileTransSvrMDL-44)	//�ļ�����������ʧ��
#define errCodeFileTransSvrReciveFileFail				(errCodeOffsetOfFileTransSvrMDL-45)	//�����ļ�ʧ��
#define errCodeFileTransSvrMkdirFail					(errCodeOffsetOfFileTransSvrMDL-45)	//����Ŀ¼ʧ��
// ����ӳ��	// 2009/1/8����
#define errCodeDataImageInMemoryNotExists				(errCodeOffsetOfDataImageInMemory-1)	// ����ӳ�񲻴���
#define errCodeDataImageInMemoryInsertError				(errCodeOffsetOfDataImageInMemory-2)	// ����ӳ������ʧ��

// // �ļ�������ģ��Ĵ���	// 2008/11/10����
#define errCodeFileSvrMDL_InvalidCmd					(errCodeOffsetOfFileSvr-1)		// �Ƿ����ļ�������ָ��

// ������
#define errCodeLockMachanism_WritingLockedAlready			(errCodeOffsetOfLockMachanism-1)		// �Ѿ���д��
#define errCodeLockMachanism_InvalidLockRecPos				(errCodeOffsetOfLockMachanism-2)		// �Ƿ�������¼λ��
#define errCodeLockMachanism_ResNotLocked				(errCodeOffsetOfLockMachanism-3)		// ��Դû�б���ס
#define errCodeLockMachanism_ResWritingLockedNotExists			(errCodeOffsetOfLockMachanism-4)		// ��Դû�б�д��ס
#define errCodeLockMachanism_ResReadingLockedNotExists			(errCodeOffsetOfLockMachanism-5)		// ��Դû�б�����ס
#define errCodeLockMachanism_ResWritingLockedNotPermittedWhenRecWritingLocked	(errCodeOffsetOfLockMachanism-6)		// ��¼��д��ʱ��������д����Դ
#define errCodeLockMachanism_RecWritingLockedAlready			(errCodeOffsetOfLockMachanism-7)		// �Ѿ���д��
#define errCodeLockMachanism_RecNotLocked				(errCodeOffsetOfLockMachanism-8)		// ��¼û�б���ס
#define errCodeLockMachanism_RecWritingLockedNotExists			(errCodeOffsetOfLockMachanism-9)		// ��¼û�б�д��ס
#define errCodeLockMachanism_RecReadingLockedNotExists			(errCodeOffsetOfLockMachanism-10)		// ��¼û�б�����ס
#define errCodeLockMachanism_RecockedNotPermittedWhenResWritingLocked	(errCodeOffsetOfLockMachanism-11)		// ��Դ��д��ʱ������������¼
#define errCodeLockMachanism_WritingLockedNotPermittedWhenReadingLocked	(errCodeOffsetOfLockMachanism-12)		// ��Դ������ʱ��������д��


// ���湲���ڴ�
#define errCodeSimuUnixSHM_SHMNotInitialized				(errCodeOffsetOfSimuUnixSharedMemory-1)		// �����ڴ�û�ж���
#define errCodeSimuUnixSHM_SizeLargerThanExisted			(errCodeOffsetOfSimuUnixSharedMemory-2)		// ����Ĺ����ڴ�ߴ��Ԥ�ڵĴ�
#define errCodeSimuUnixSHM_SHMBufferIsFull				(errCodeOffsetOfSimuUnixSharedMemory-3)		// �����ڴ������
#define errCodeSimuUnixSHM_NotExists					(errCodeOffsetOfSimuUnixSharedMemory-4)		// �����ڴ治����

// �������ƽ̨ģ��Ĵ���	2008/10/3����
#define errCodeCDPMDL_TooManyRequestDatagramFld				(errCodeOffsetOfCDP-1)		// ������̫�౨��������
#define errCodeCDPMDL_TooManyResponseDatagramFld			(errCodeOffsetOfCDP-2)		// ������̫�౨����Ӧ��
#define errCodeCDPMDL_TooManyFunVarDefined				(errCodeOffsetOfCDP-3)		// ������̫�ຯ������
#define errCodeCDPMDL_VarTypeNotDefined					(errCodeOffsetOfCDP-4)		// û�ж����������
#define errCodeCDPMDL_VarNameNotDefined					(errCodeOffsetOfCDP-5)		// û�ж����������
#define errCodeCDPMDL_ArrayDimisionNotDefined				(errCodeOffsetOfCDP-6)		// �����ά��û�ж���
#define errCodeCDPMDL_ArrayDimisionSizeNotDefined			(errCodeOffsetOfCDP-7)		// ����Ĵ�Сû�ж���
#define errCodeCDPMDL_ReadVarFailure					(errCodeOffsetOfCDP-8)		// ������ʧ��
#define errCodeCDPMDL_InvalidVarType					(errCodeOffsetOfCDP-9)		// �Ƿ���������
#define errCodeCDPMDL_NotBaseVarType					(errCodeOffsetOfCDP-10)		// ���ǻ�����������
#define errCodeCDPMDL_ConstNameNotDefined				(errCodeOffsetOfCDP-11)		// ��������û�ж���
#define errCodeCDPMDL_ConstNotDefined					(errCodeOffsetOfCDP-12)		// ����û�ж���
#define errCodeCDPMDL_PointerNotDefined					(errCodeOffsetOfCDP-13)		// ָ������û�ж���
#define errCodeCDPMDL_ArrayNotDefined					(errCodeOffsetOfCDP-14)		// ��������û�ж���
#define errCodeCDPMDL_SimpleTypeNotDefined				(errCodeOffsetOfCDP-15)		// ������û�ж���
#define errCodeCDPMDL_NotIntConst					(errCodeOffsetOfCDP-16)		// �������ͳ���
#define errCodeCDPMDL_ArraySizeNotSet					(errCodeOffsetOfCDP-17)		// û�����������С
#define errCodeCDPMDL_VarValueTagNotDefined				(errCodeOffsetOfCDP-18)		// û�ж������ȡֵ�ı�־
#define errCodeCDPMDL_VarBaseTypeTagNotDefined				(errCodeOffsetOfCDP-19)		// û�ж������ȡֵ�����ͱ�־
#define errCodeCDPMDL_NoValueForSpecVarValueTagDefined			(errCodeOffsetOfCDP-20)		// û��Ϊָ���ı���ȡֵ��ʶ����ֵ
#define errCodeCDPMDL_NoValidConstType					(errCodeOffsetOfCDP-21)		// �Ƿ��ĳ�������
#define errCodeCDPMDL_InvalidKeyWord					(errCodeOffsetOfCDP-22)		// �Ƿ��ؼ���
#define errCodeCDPMDL_GlobalVariableNotDefined				(errCodeOffsetOfCDP-23)		// ȫ�ֱ���û����
#define errCodeCDPMDL_DatagramFldTagNotDefined				(errCodeOffsetOfCDP-24)		// �������ʶû�ж���
#define errCodeCDPMDL_DatagramHeaderDefinedError			(errCodeOffsetOfCDP-25)		// ����ͷ�����
#define errCodeCDPMDL_DatagramClassNotDefined				(errCodeOffsetOfCDP-26)		// ��������û�ж���
#define errCodeCDPMDL_ModuleNotDefined					(errCodeOffsetOfCDP-27)		// ģ��û�ж���
#define errCodeCDPMDL_ModuleDevDirNotDefined				(errCodeOffsetOfCDP-28)		// ģ�鿪��Ŀ¼û�ж���
#define errCodeCDPMDL_ProgramNotDefined					(errCodeOffsetOfCDP-29)		// ����û�ж���
#define errCodeCDPMDL_DatagramFldAlaisNotDefined			(errCodeOffsetOfCDP-30)		// ���������û�ж���
#define errCodeCDPMDL_InvalidCalcuOperator				(errCodeOffsetOfCDP-31)		// �Ƿ������
#define errCodeCDPMDL_InvalidRelationCalcuOperator			(errCodeOffsetOfCDP-32)		// �Ƿ���ϵ�����

// ��������ģ��Ĵ���	// 2008/9/26����
#define errCodeCmmPackMDL_DatatFldLength				(errCodeOffsetOfCmmPack-1)		// �����򳤶ȴ�
#define errCodeCmmPackMDL_DatatLengthToShort				(errCodeOffsetOfCmmPack-2)		// ������̫��
#define errCodeCmmPackMDL_FldNotDefined					(errCodeOffsetOfCmmPack-3)		// ��û�ж���
#define errCodeCmmPackMDL_PackIDNotIdentified				(errCodeOffsetOfCmmPack-4)		// ����ƥ���

// SQLģ��Ĵ���	2008/7/24 ����
#define errCodeSQLInvalidFldCompareCondition				(errCodeOffsetOfSQL-1)		// �Ƿ��ıȽ�����
#define errCodeSQLInvalidSQLStr						(errCodeOffsetOfSQL-2)		// �Ƿ���SQL������
#define errCodeSQLRecFldNameTooLong					(errCodeOffsetOfSQL-3)		// ����̫��
#define errCodeSQLInvalidFldName					(errCodeOffsetOfSQL-4)		// �����Ƿ�
#define errCodeSQLNullFldName						(errCodeOffsetOfSQL-5)		// ����Ϊ��
#define errCodeSQLFldAssignMethodNotSet					(errCodeOffsetOfSQL-6)		// δ������ֵ����
#define errCodeSQLFldAssignMethodNotSupport				(errCodeOffsetOfSQL-7)		// ��֧�ֵ���ֵ����
#define errCodeSQLPrimaryKeyIsNull					(errCodeOffsetOfSQL-8)		// �ؼ����ǿմ�
#define errCodeSQLPrimaryKeyUpdateNotPermitted				(errCodeOffsetOfSQL-9)		// �ؼ��ֲ������޸�
#define errCodeSQLFldAssignMethodInvalid				(errCodeOffsetOfSQL-10)		// ��ֵ��������

// ��������룬2008/3/12 ����
#define errCodeObjectMDL_ReadObjectDefinition				(errCodeOffsetOfObject-1)	// ��������
#define errCodeObjectMDL_ObjectDefinitionAlreadyExists			(errCodeOffsetOfObject-2)	// �������Ѵ���
#define errCodeObjectMDL_ObjectDefinitionFileLen			(errCodeOffsetOfObject-3)	// �������ļ����ȴ�
// Mary add begin, 2008-8-18
#define errCodeObjectMDL_ObjectDefinitionNotExists			(errCodeOffsetOfObject-4)	// �����岻����
#define errCodeObjectMDL_ObjectNameError				(errCodeOffsetOfObject-5)	// �������ƴ���
#define errCodeObjectMDL_ObjectRecordNotExist				(errCodeOffsetOfObject-6)	// ����ʵ��δ�ҵ�
#define errCodeObjectMDL_ForeignObjectNotExist				(errCodeOffsetOfObject-7)	// �ⲿ���󲻴���
#define errCodeObjectMDL_ForeignRecordNotExist				(errCodeOffsetOfObject-8)	// �ⲿ����ʵ��������
#define errCodeObjectMDL_ObjectIsInvalid				(errCodeOffsetOfObject-9)	// �����岻�Ϸ�
#define errCodeObjectMDL_RecordExistAlready				(errCodeOffsetOfObject-10)	// �����ʵ����Ȼ����
#define errCodeObjectMDL_ObjectIsReferenced				(errCodeOffsetOfObject-11)	// ������Ա�������������
#define errCodeObjectMDL_FieldNumberError				(errCodeOffsetOfObject-12)	// ����Ŀ����ȷ
#define errCodeObjectMDL_FieldValueIsInvalid				(errCodeOffsetOfObject-13)	// ��ֵ���岻�Ϸ�
#define errCodeObjectMDL_FieldNotExist					(errCodeOffsetOfObject-14)	// �򲻴���
#define errCodeObjectMDL_PrimaryKeyIsRepeat				(errCodeOffsetOfObject-15)	// ��ֵ�ظ�
#define errCodeObjectMDL_UniqueKeyIsRepeat				(errCodeOffsetOfObject-16)	// Ψһֵ�ظ�
#define errCodeObjectMDL_RecordIsReferenced				(errCodeOffsetOfObject-17)	// ʵ����Ȼ����������ʵ������
#define errCodeObjectMDL_ObjectHasNoRecord				(errCodeOffsetOfObject-18)	// �ö���û��ʵ��
#define errCodeObjectMDL_FieldCanNotUpdate				(errCodeOffsetOfObject-19)	// �������޸�
#define errCodeObjectMDL_FieldNumberTooMuch				(errCodeOffsetOfObject-20)	// ����Ŀ̫��
#define errCodeObjectMDL_FieldTypeInvalid				(errCodeOffsetOfObject-21)	// �����Ͳ���
#define errCodeObjectMDL_FieldValueTooLong				(errCodeOffsetOfObject-22)	// ��ֵ̫��
#define errCodeObjectMDL_FieldListDefError				(errCodeOffsetOfObject-23)	// ���嵥�����
#define errCodeObjectMDL_RecordNumverTooMuch				(errCodeOffsetOfObject-24)	// ��¼����̫��
// Mary add end, 2008-8-18

// pkcs11ģ��
#define errCodeOffsetOfPkcs11						errCodeOffsetOfPKCS11

// ��Կ��ģ��
#define errCodeKDBBufMDL_ConnectIndexTBL				(errCodeOffsetOfKDBSvr-1)	// ����ģ��
#define errCodeKDBBufMDL_InitDef					(errCodeOffsetOfKDBSvr-2)	// ��ʼ��ģ��

// �������ģ��
#define errCodeOffsetOfErrCodeMDL_CliErrCodeMustSpecified		(errCodeOffsetOfErrCodeMDL-1)	// �ͻ��˴���������ָ��
#define errCodeOffsetOfErrCodeMDL_CodeNotDefined			(errCodeOffsetOfErrCodeMDL-2)	// ������û����
#define errCodeOffsetOfErrCodeMDL_CodeAlreadyDefined			(errCodeOffsetOfErrCodeMDL-3)	// �������Ѿ�����

// ����ͨѶģ��
// offset = -7000
#define errCodeYLCommConfMDL_CityIDNotDefined				(errCodeOffsetOfYLCommConfMDL-1)	// ������δ����
#define errCodeYLCommConfMDL_CommConfNotDefined				(errCodeOffsetOfYLCommConfMDL-2)	// ͨѶ����δ����
#define errCodeYLCommConfMDL_AddCommConf				(errCodeOffsetOfYLCommConfMDL-3)	// ����ָ����ͨѶ����

// offset = -8000
// ����������������Cmbc����
#define errCodeCmbcEuspPackageMDL_InvalidDDU				(errCodeOffsetOfCmbcEuspPackageMDL-1)	// �Ƿ���DDU
#define errCodeCmbcEuspPackageMDL_LenOfValueLargerThanMaxLenDefined	(errCodeOffsetOfCmbcEuspPackageMDL-2)	// ֵ�ĳ��ȴ��ڶ����ֵ
#define errCodeCmbcEuspPackageMDL_DDULength				(errCodeOffsetOfCmbcEuspPackageMDL-3)	// DDU�ĳ��ȴ�
#define errCodeCmbcEuspPackageMDL_DDUValueIsNull			(errCodeOffsetOfCmbcEuspPackageMDL-4)	// DDU��ֵ�ǿ�
#define errCodeCmbcEuspPackageMDL_LenOfValueLargerThanStr		(errCodeOffsetOfCmbcEuspPackageMDL-5)	// ֵ�ĳ��ȱȴ��ĳ��ȴ�
#define errCodeCmbcEuspPackageMDL_FldOfNameNotDefinedInSpecForm		(errCodeOffsetOfCmbcEuspPackageMDL-6)	// �ڱ����δ�������
#define errCodeCmbcEuspPackageMDL_InvalidFormPointer			(errCodeOffsetOfCmbcEuspPackageMDL-7)	// �Ƿ��ı��ָ��
#define errCodeCmbcEuspPackageMDL_FormIsFull				(errCodeOffsetOfCmbcEuspPackageMDL-8)	// ���Ϊ��
#define errCodeCmbcEuspPackageMDL_FormInvalid				(errCodeOffsetOfCmbcEuspPackageMDL-9)	// ���Ƿ�
#define errCodeCmbcEuspPackageMDL_CreateFormRec				(errCodeOffsetOfCmbcEuspPackageMDL-10)	// ��������¼����
#define errCodeCmbcEuspPackageMDL_FormCurrentRecNotInitialized		(errCodeOffsetOfCmbcEuspPackageMDL-11)	// ���ĵ�ǰ��¼δ��ʼ��
#define errCodeCmbcEuspPackageMDL_LocalAndRemoteFormNameNotSame		(errCodeOffsetOfCmbcEuspPackageMDL-12)	// ���غ�Զ�˵ı��������ͬ
#define errCodeCmbcEuspPackageMDL_LocalAndRemoteFldNameNotSame		(errCodeOffsetOfCmbcEuspPackageMDL-13)	// ���غ�Զ�˵���������ͬ
#define errCodeCmbcEuspPackageMDL_DDUOfNameNotFoundInTransData		(errCodeOffsetOfCmbcEuspPackageMDL-14)	// ����������û�и����Ƶ�DDU
#define errCodeCmbcEuspPackageMDL_FormOfNameNotFoundInTransData		(errCodeOffsetOfCmbcEuspPackageMDL-15)	// ����������û�и����Ƶı��
#define errCodeCmbcEuspPackageMDL_NotSupportedUnit			(errCodeOffsetOfCmbcEuspPackageMDL-16)	// ��֧�ֵĵ�Ԫ
#define errCodeCmbcEuspPackageMDL_ConnectCmbcEuspPackageMDL		(errCodeOffsetOfCmbcEuspPackageMDL-17)	// ����CmbcEusp����ģ�����
#define errCodeCmbcEuspPackageMDL_CmbcEuspPackageNotConnected		(errCodeOffsetOfCmbcEuspPackageMDL-18)	// CmbcEusp����ģ��δ����

// offset = -9000
// �����ָ�����
#define errCodeHsmCmdMDL_ReturnLen			(errCodeOffsetOfHsmCmdMDL-1)	// ���س��ȴ�	
#define errCodeHsmCmdMDL_NoMK				(errCodeOffsetOfHsmCmdMDL-2)	// û������Կ
#define errCodeHsmCmdMDL_WrongTerminalKey		(errCodeOffsetOfHsmCmdMDL-3)	// ������ն���Կ
#define errCodeHsmCmdMDL_WKParity			(errCodeOffsetOfHsmCmdMDL-4)	// ��Կ����żУ���
#define errCodeHsmCmdMDL_InvalidTerminalKeyIndex	(errCodeOffsetOfHsmCmdMDL-5)	// �Ƿ����ն���Կ����
#define errCodeHsmCmdMDL_InvalidBmkIndex		(errCodeOffsetOfHsmCmdMDL-6)	// �Ƿ���BMK����
#define errCodeHsmCmdMDL_MacOrPinCheckFailure		(errCodeOffsetOfHsmCmdMDL-7)	// MAC������ԿУ���
#define errCodeHsmCmdMDL_FirstPikParity			(errCodeOffsetOfHsmCmdMDL-8)	// ��һ��PIK��żУ���
#define errCodeHsmCmdMDL_SecondPikParity		(errCodeOffsetOfHsmCmdMDL-9)	// �ڶ���PIK��żУ���
#define errCodeHsmCmdMDL_InvalidPinType			(errCodeOffsetOfHsmCmdMDL-10)	// �Ƿ���PIN����
#define errCodeHsmCmdMDL_MacDataLength			(errCodeOffsetOfHsmCmdMDL-11)	// MAC���ݵĳ��ȴ�
#define errCodeHsmCmdMDL_Mak1Parity			(errCodeOffsetOfHsmCmdMDL-13)	// ��һ��ZAK��żУ���
#define errCodeHsmCmdMDL_Mak2Parity			(errCodeOffsetOfHsmCmdMDL-14)	// �ڶ���ZAK��żУ���
#define errCodeHsmCmdMDL_PinType			(errCodeOffsetOfHsmCmdMDL-15)	// PIN�����ʹ�
#define errCodeHsmCmdMDL_InvalidCmd			(errCodeOffsetOfHsmCmdMDL-16)	// �Ƿ���ָ��
#define errCodeHsmCmdMDL_CmdTooShort			(errCodeOffsetOfHsmCmdMDL-17)	// ָ��̫��
#define errCodeHsmCmdMDL_CmdTooLong			(errCodeOffsetOfHsmCmdMDL-18)	// ����̫��
#define errCodeHsmCmdMDL_CommError			(errCodeOffsetOfHsmCmdMDL-19)	// ͨѶ����
#define errCodeHsmCmdMDL_InvalidChar			(errCodeOffsetOfHsmCmdMDL-20)	// �Ƿ��ַ�
#define errCodeHsmCmdMDL_Timeout			(errCodeOffsetOfHsmCmdMDL-21)	// ��ʱ
#define errCodeHsmCmdMDL_NoBmkOrBmkParity		(errCodeOffsetOfHsmCmdMDL-22)	// û��BMK��BMK��żУ���
#define errCodeHsmCmdMDL_Unknown			(errCodeOffsetOfHsmCmdMDL-23)	// δ֪����
#define errCodeHsmCmdMDL_ErrCodeNotSuccess		(errCodeOffsetOfHsmCmdMDL-24)	// ���صĴ����벻��00
#define errCodeHsmCmdMDL_VerifyFailure			(errCodeOffsetOfHsmCmdMDL-25)	// ��֤ʧ��
#define errCodeHsmCmdMDL_KeyLen				(errCodeOffsetOfHsmCmdMDL-26)	// ��Կ����
#define errCodeHsmCmdMDL_InvalidKeyType			(errCodeOffsetOfHsmCmdMDL-27)	// ��Կ����
#define errCodeHsmCmdMDL_KeyLenFlag			(errCodeOffsetOfHsmCmdMDL-28)	// ��Կ���ȱ�ʶ
#define errCodeHsmCmdMDL_HsmNotAvailable		(errCodeOffsetOfHsmCmdMDL-29)	// �����������
#define errCodeHsmCmdMDL_PinByLmk0203			(errCodeOffsetOfHsmCmdMDL-30)	// LMK02-03���ܵ�PIN��
#define errCodeHsmCmdMDL_InvalidPinInputData		(errCodeOffsetOfHsmCmdMDL-31)	// PIN��������
#define errCodeHsmCmdMDL_PrinterNotReady		(errCodeOffsetOfHsmCmdMDL-32)	// ��ӡ��û׼����
#define errCodeHsmCmdMDL_HSMNotAuthorized		(errCodeOffsetOfHsmCmdMDL-33)	// ���ܻ�û����Ȩ
#define errCodeHsmCmdMDL_FormatNotLoaded		(errCodeOffsetOfHsmCmdMDL-34)	// ��ʽδ����
#define errCodeHsmCmdMDL_DieboldTableInvalid		(errCodeOffsetOfHsmCmdMDL-35)	// DieboldTable
#define errCodeHsmCmdMDL_PinBlock			(errCodeOffsetOfHsmCmdMDL-36)	// PinBlock
#define errCodeHsmCmdMDL_InvalidIndex			(errCodeOffsetOfHsmCmdMDL-37)	// �Ƿ�����
#define errCodeHsmCmdMDL_InvalidAcc			(errCodeOffsetOfHsmCmdMDL-38)	// �Ƿ��˺�
#define errCodeHsmCmdMDL_PinBlockFormat			(errCodeOffsetOfHsmCmdMDL-39)	// PinBlock��ʽ
#define errCodeHsmCmdMDL_PinData			(errCodeOffsetOfHsmCmdMDL-40)	// Pin����
#define errCodeHsmCmdMDL_DecimalizationTable		(errCodeOffsetOfHsmCmdMDL-41)	// DecimalizationTable
#define errCodeHsmCmdMDL_KeyScheme			(errCodeOffsetOfHsmCmdMDL-42)	// ��Կ����
#define errCodeHsmCmdMDL_IncompitableKeyLen		(errCodeOffsetOfHsmCmdMDL-43)	// ��������Կ����
#define errCodeHsmCmdMDL_KeyType			(errCodeOffsetOfHsmCmdMDL-44)	// ��Կ����
#define errCodeHsmCmdMDL_KeyNotPermitted		(errCodeOffsetOfHsmCmdMDL-45)	// ������ʹ����Կ
#define errCodeHsmCmdMDL_ReferenceNumber		(errCodeOffsetOfHsmCmdMDL-46)	// ReferenceNumber
#define errCodeHsmCmdMDL_InsufficientSolicitation	(errCodeOffsetOfHsmCmdMDL-47)	// �����Solicitation
#define errCodeHsmCmdMDL_LMKKeyChangeCorrupted		(errCodeOffsetOfHsmCmdMDL-48)	// 
#define errCodeHsmCmdMDL_DesFailure			(errCodeOffsetOfHsmCmdMDL-49)	// DES����
#define errCodeHsmCmdMDL_DataLength			(errCodeOffsetOfHsmCmdMDL-50)	// ���ݳ���
#define errCodeHsmCmdMDL_LRCError			(errCodeOffsetOfHsmCmdMDL-51)	// LRC
#define errCodeHsmCmdMDL_InternalCommand		(errCodeOffsetOfHsmCmdMDL-52)	// InternalCommand
#define errCodeHsmCmdMDL_CmdResTooShort			(errCodeOffsetOfHsmCmdMDL-53)	// ָ����Ӧ̫��
#define errCodeHsmCmdMDL_InvalidStorageIndex		(errCodeOffsetOfHsmReturnCodeMDL-21)	// �Ƿ��洢����	// 21
#define errCodeHsmCmdMDL_InvalidInputData		(errCodeOffsetOfHsmReturnCodeMDL-15)	// �������ݴ�	//15
#define errCodeHsmCmdMDL_LmkError			(errCodeOffsetOfHsmReturnCodeMDL-13)	// LMK����	// 13
#define errCodeHsmCmdMDL_NoKeyLoadsInStorage		(errCodeOffsetOfHsmReturnCodeMDL-12)	// û����Կ���ص��û��洢��	//12
#define errCodeHsmCmdMDL_ZmkParity			(errCodeOffsetOfHsmReturnCodeMDL-10)	// ZMK��żУ���	//10

// offset = -30000
// ����ǰ�û�����
#define errCodeYLQZMDL_InvalidMDLID					(errCodeOffsetOfYLQZMDL-1)	// �Ƿ���ģ���ʶ
#define errCodeYLQZMDL_ProcNotExistsAnymore				(errCodeOffsetOfYLQZMDL-2)	// �����Ѳ�����
#define errCodeYLQZMDL_WaitResponse					(errCodeOffsetOfYLQZMDL-3)	// �ȴ���Ӧ��ʱ
#define errCodeYLQZMDL_MsgForThisModuleTooShort				(errCodeOffsetOfYLQZMDL-4)	// ����ģ�����Ϣ̫��
#define errCodeYLQZMDL_ConnectYLPackageMDL				(errCodeOffsetOfYLQZMDL-5)	// ������������ģ�����
#define errCodeYLQZMDL_NoAtmpErrCodeDefinedForYLErrCode			(errCodeOffsetOfYLQZMDL-6)	// δ�����������Ӧ���Ӧ��ATMP��Ӧ��
#define errCodeYLQZMDL_NoYLErrCodeDefinedForHostErrCode			(errCodeOffsetOfYLQZMDL-7)	// δ�����������Ӧ���Ӧ��������Ӧ��
#define errCodeYLQZMDL_HostCommConfNotDefinedInCommConfMDL		(errCodeOffsetOfYLQZMDL-8)	// ͨѶģ����δ��������ͨѶ����
#define errCodeYLQZMDL_AtmpTHKTransDefTBLConnected			(errCodeOffsetOfYLQZMDL-9)	// ���п����׶���ģ��������
#define errCodeYLQZMDL_THKTransNotDefined				(errCodeOffsetOfYLQZMDL-10)	// ���п�����δ����
#define errCodeYLQZMDL_GetAtmpTransMapFile				(errCodeOffsetOfYLQZMDL-11)	// ��ȡATMP���׵�λͼ�ļ���
#define errCodeYLQZMDL_AtmpPackageFldPoolNotInitialized			(errCodeOffsetOfYLQZMDL-12)	// ATMP�������δ��ʼ��
#define errCodeYLQZMDL_MacFldTBLDefOfSpecifiedIDDefined			(errCodeOffsetOfYLQZMDL-13)	// δ����MAC�������
#define errCodeYLQZMDL_InvalidIDOfMacFldTBL				(errCodeOffsetOfYLQZMDL-14)	// �Ƿ���MAC��������ʶ
#define errCodeYLQZMDL_InvalidMacFldTBLIndex				(errCodeOffsetOfYLQZMDL-15)	// �Ƿ���MAC�����������
#define errCodeYLQZMDL_MacFldTBLOfSameID				(errCodeOffsetOfYLQZMDL-16)	// MAC�������ı�ʶ�ظ�
#define errCodeYLQZMDL_InvalidMaxFldNum					(errCodeOffsetOfYLQZMDL-17)	// �Ƿ����������Ŀ
#define errCodeYLQZMDL_IDOfMacFldTBLNotDefined				(errCodeOffsetOfYLQZMDL-18)	// MAC��������IDδ����
#define errCodeYLQZMDL_MacFldTBLDefNotDefined				(errCodeOffsetOfYLQZMDL-19)	// MAC���������δ����
#define errCodeYLQZMDL_InvalidMacFldIndex				(errCodeOffsetOfYLQZMDL-20)	// �Ƿ���MAC��������
#define errCodeYLQZMDL_YLPackageMacNotSameAsThatLocalGenerated		(errCodeOffsetOfYLQZMDL-21)	// �������ɵ�MAC�����������еĲ�ͬ
#define errCodeYLQZMDL_GetYLPackage					(errCodeOffsetOfYLQZMDL-22)	// ��ȡ�������ĳ���
#define errCodeYLQZMDL_YLQZKeyTypeInvalid				(errCodeOffsetOfYLQZMDL-23)	// �Ƿ���ǰ�û���Կ����
#define errCodeYLQZMDL_YLQZKeyLengthInvalid				(errCodeOffsetOfYLQZMDL-24)	// ǰ�û���Կ�ĳ��Ȳ���
#define errCodeYLQZMDL_YLQZZmkNotPermittedUpdated			(errCodeOffsetOfYLQZMDL-25)	// ǰ�û���ZMK���������
#define errCodeYLQZMDL_SSNConfFileNotExists				(errCodeOffsetOfYLQZMDL-26)	// SSN�����ļ�������
#define errCodeYLQZMDL_SSNConfFileUpdate				(errCodeOffsetOfYLQZMDL-27)	// ����SSN�����ļ�����
#define errCodeYLQZMDL_ApplyHostKey					(errCodeOffsetOfYLQZMDL-28)	// ����������Կ����
#define errCodeYLQZMDL_YLResponseCodeNotSuccess				(errCodeOffsetOfYLQZMDL-29)	// ������Ӧ�벻��00
#define errCodeYLQZMDL_LengthOf8583PackIsZero				(errCodeOffsetOfYLQZMDL-30)	// 8583���ĳ�����0
#define errCodeYLQZMDL_InvalidPinLength					(errCodeOffsetOfYLQZMDL-31)	// PIN���ȷǷ�
#define errCodeYLQZMDL_InvalidAccNoLength				(errCodeOffsetOfYLQZMDL-32)	// �Ƿ����˺ų���
#define errCodeYLQZMDL_NoYLErrCodeDefinedForYLQZErrCode			(errCodeOffsetOfYLQZMDL-33)	// δ�����ǰ�û��������Ӧ��������Ӧ��
#define errCodeYLQZMDL_NoAtmpErrCodeDefinedForYLQZErrCode		(errCodeOffsetOfYLQZMDL-34)	// δ�����ǰ�û��������Ӧ��ATMP��Ӧ��
#define errCodeYLQZMDL_SetYLQZSSNToHost					(errCodeOffsetOfYLQZMDL-35)	// ����ǰ�û���ˮ�ų���
#define errCodeYLQZMDL_YLRequestNotSameWithHostResponse			(errCodeOffsetOfYLQZMDL-37)	// �����������ĺ�������Ӧ���Ĳ�ƥ��
#define errCodeYLQZMDL_GetDefaultPreAuthEffectiveDate			(errCodeOffsetOfYLQZMDL-38)	// ��ȡȱʡ��Ԥ��Ȩ��Ч�ڳ���
#define errCodeYLQZMDL_ThisTransIsNotPermitted				(errCodeOffsetOfYLQZMDL-39)	// �ý��ײ�����
#define errCodeYLQZMDL_SetHostFlds					(errCodeOffsetOfYLQZMDL-40)	// �������������
#define errCodeYLQZMDL_TransNotPermitted				(errCodeOffsetOfYLQZMDL-41)	// ���ײ�����
#define errCodeYLQZMDL_FindAtmpErrCodeForYLQZErrCode			(errCodeOffsetOfYLQZMDL-42)	// ��������ǰ�ô������Ӧ��ATMP��Ӧ�����
#define errCodeYLQZMDL_SetYLFlds					(errCodeOffsetOfYLQZMDL-43)	// �������������
#define errCodeYLQZMDL_FindAtmpErrCodeForYLErrCode			(errCodeOffsetOfYLQZMDL-44)	// ����������Ӧ���Ӧ��ATMP��Ӧ�����
#define errCodeYLQZMDL_YLReqAreNotSameWithYLRes				(errCodeOffsetOfYLQZMDL-45)	// �����������Ӧ��ƥ��
#define errCodeYLQZMDL_OriginTransNotSuccess				(errCodeOffsetOfYLQZMDL-46)	// ԭʼ���ײ��ɹ�
#define errCodeYLQZMDL_TransNotSurpported				(errCodeOffsetOfYLQZMDL-47)	// ���ײ�֧��
#define errCodeYLQZMDL_Not8583RequestMTI				(errCodeOffsetOfYLQZMDL-48)	// ����8583��������MTI
#define errCodeYLQZMDL_YLNotSupportedReversalForThisTrans		(errCodeOffsetOfYLQZMDL-49)	// ������֧�ָý��׵ĳ�������
#define errCodeYLQZMDL_TransConfFileNotSet				(errCodeOffsetOfYLQZMDL-50)	// ���������ļ�δ����
#define errCodeYLQZMDL_HostPackageNotConnected				(errCodeOffsetOfYLQZMDL-51)	// ��������ģ��δ����
#define errCodeYLQZMDL_NoSetValueMethodForHostFld			(errCodeOffsetOfYLQZMDL-52)	// δΪ�������帳ֵ������
#define errCodeYLQZMDL_AtmpPackageNotConnected				(errCodeOffsetOfYLQZMDL-53)	// ATMP����ģ��δ����
#define errCodeYLQZMDL_NoSetValueMethodForAtmpFld			(errCodeOffsetOfYLQZMDL-54)	// δΪATMP���帳ֵ����
#define errCodeYLQZMDL_AtmpTransDefNotDefined				(errCodeOffsetOfYLQZMDL-55)	// ATMP���׶���δ����
#define errCodeYLQZMDL_YLTransDataDefNotConnected			(errCodeOffsetOfYLQZMDL-56)	// �����������ݶ���δ����
#define errCodeYLQZMDL_YL8583PackageFldDefIsNull			(errCodeOffsetOfYLQZMDL-57)	// �������������ǿ�
#define errCodeYLQZMDL_WaitHostResponse					(errCodeOffsetOfYLQZMDL-58)	// �ȴ�������Ӧ����
#define errCodeYLQZMDL_FixedMDLIDHasNoPID				(errCodeOffsetOfYLQZMDL-59)	// �̶�ģ��ı�ʶ�������̺�
#define errCodeYLQZMDL_IDOfYLNotDefined					(errCodeOffsetOfYLQZMDL-60)	// δ����������ʶ
#define errCodeYLQZMDL_NoBankErrCodeDefinedForYLErrCode			(errCodeOffsetOfYLQZMDL-61)	// δ����������Ӧ���Ӧ��������Ӧ��
#define errCodeYLQZMDL_NoYLErrCodeDefinedForBankErrCode			(errCodeOffsetOfYLQZMDL-62)	// δ����������Ӧ���Ӧ��������Ӧ��
#define errCodeYLQZMDL_NoBankErrCodeDefinedForYLQZErrCode		(errCodeOffsetOfYLQZMDL-63)	// δ����ǰ�û��������Ӧ��������Ӧ��
#define errCodeYLQZMDL_KeyGroupNotDefined				(errCodeOffsetOfYLQZMDL-64)	// ��Կ��δ����
#define errCodeYLQZMDL_KeyGroupAlreadyExists				(errCodeOffsetOfYLQZMDL-65)	// ��Կ���Ѵ���
#define errCodeYLQZMDL_KeyGroupTBLIsFull				(errCodeOffsetOfYLQZMDL-66)	// ��Կ���ǿ�
#define errCodeYLQZMDL_ConnectHsmSvrCmdPackageMDL			(errCodeOffsetOfYLQZMDL-67)	// ��������������ĳ���
#define errCodeYLQZMDL_InvalidHsmSvrCmd					(errCodeOffsetOfYLQZMDL-68)	// �Ƿ����������ָ��
#define errCodeYLQZMDL_ConnectToHsm					(errCodeOffsetOfYLQZMDL-69)	// �������������
#define errCodeYLQZMDL_ResetKeyFld48					(errCodeOffsetOfYLQZMDL-70)	// ��ֵ�û����ĵ�48���
#define errCodeYLQZMDL_ConnectHostPackageMDL				(errCodeOffsetOfYLQZMDL-71)	// �����������ĳ���
#define errCodeYLQZMDL_ConnectAtmpPackageMDL				(errCodeOffsetOfYLQZMDL-72)	// ����ATMP���ĳ���
#define errCodeYLQZMDL_NoStrategyDefinedWhenHostTimeout			(errCodeOffsetOfYLQZMDL-73)	// δ����������ʱ�Ĵ������
#define errCodeYLQZMDL_HostResponseCodeNotSuccess			(errCodeOffsetOfYLQZMDL-74)	// ������Ӧ�벻�ǳɹ�
#define errCodeYLQZMDL_HostNotSupportedThisKindOfTrans			(errCodeOffsetOfYLQZMDL-75)	// ������֧�����ֽ���
#define errCodeYLQZMDL_NoAtmTeller					(errCodeOffsetOfYLQZMDL-76)	// ����ATM��Ա
#define errCodeYLQZMDL_NoPosTeller					(errCodeOffsetOfYLQZMDL-77)	// ����POS��Ա
#define errCodeYLQZMDL_CommWithDBSvr					(errCodeOffsetOfYLQZMDL-78)	// ��DBSvrͨѶ��
#define errCodeYLQZMDL_PinNotExists					(errCodeOffsetOfYLQZMDL-79)	// PIN������
#define errCodeYLQZMDL_AmountExceedPerTransLimit			(errCodeOffsetOfYLQZMDL-80)	// ���׶��ÿ�ʵ����ֵ
#define errCodeYLQZMDL_AmountExceedPerDayLimit				(errCodeOffsetOfYLQZMDL-81)	// ���ս��׶���ۼ�
#define errCodeYLQZMDL_IsBitMapFld					(errCodeOffsetOfYLQZMDL-82)	// ��λͼ��
#define errCodeYLQZMDL_ThisKindCardNotPermitYLTrans			(errCodeOffsetOfYLQZMDL-83)	// ���ֿ�����������������
#define errCodeYLQZMDL_CardHasNoCVV					(errCodeOffsetOfYLQZMDL-84)	// ������CVV
#define errCodeYLQZMDL_YLQZAuthorizedTrans				(errCodeOffsetOfYLQZMDL-85)	// ǰ�û���Ȩ����
#define errCodeYLQZMDL_ForbideTransFromThisOccurer			(errCodeOffsetOfYLQZMDL-86)	// ��ֹ���״���������з���
#define errCodeYLQZMDL_NoCreditTeller					(errCodeOffsetOfYLQZMDL-87)     // ���Ǵ��ǿ���Ա
#define errCodeYLQZMDL_NoDebitTeller					(errCodeOffsetOfYLQZMDL-88)     // ���ǽ������Ա
#define errCodeYLQZMDL_InvalidIDCard					(errCodeOffsetOfYLQZMDL-89)     // �Ƿ������֤��
#define errCodeYLQZMDL_InvalidCustName					(errCodeOffsetOfYLQZMDL-90)     // ������һ��
#define errCodeYLQZMDL_OriginTransSuccess				(errCodeOffsetOfYLQZMDL-91)	// ԭʼ���׳ɹ�
#define errCodeYLQZMDL_SytInvalidAcctNo					(errCodeOffsetOfYLQZMDL-92) // ����ͨת�뿨�����ڣ��ڸ���δ�Ǽǣ�
#define errCodeYLQZMDL_SytAmountExceedPerTransLimit			(errCodeOffsetOfYLQZMDL-93) // ����ͨ���ʳ����������
#define errCodeYLQZMDL_SytAmountExceedPerDayLimit			(errCodeOffsetOfYLQZMDL-94) // ����ͨ���պϼƳ����������

// offset = -26000
// 8583 ģ�����
#define errCodeISO8583MDL_MTINotDefined				(errCodeOffsetOfISO8583MDL-1)	// MTIδ����
#define errCodeISO8583MDL_8583PackageClassTBLConnected		(errCodeOffsetOfISO8583MDL-2)	// ����8583�����ඨ�����
#define errCodeISO8583MDL_8583BHKTransDefTBLConnected		(errCodeOffsetOfISO8583MDL-3)	// ����8583���п����׶������
#define errCodeISO8583MDL_BHKTransNotDefined			(errCodeOffsetOfISO8583MDL-4)	// ���п�����δ����
#define errCodeISO8583MDL_8583YLMngTransDefTBLConnected		(errCodeOffsetOfISO8583MDL-5)	// �������������ཻ�ױ��
#define errCodeISO8583MDL_YLMngTransNotDefined			(errCodeOffsetOfISO8583MDL-6)	// ����������δ����
#define errCodeISO8583MDL_8583Fld90Length			(errCodeOffsetOfISO8583MDL-7)	// 90�򳤶ȴ���
#define errCodeISO8583MDL_FldNoValue				(errCodeOffsetOfISO8583MDL-8)	// ����ֵ

// offset = -25000
// ����ģ�����
#define errCodePackageDefMDL_MaxNumOfPackageFldsNotDefined	(errCodeOffsetOfPackageDefMDL-1)	// �������������δ����
#define errCodePackageDefMDL_PackageDefGroupIsFull		(errCodeOffsetOfPackageDefMDL-2)	// ���Ķ���������
#define errCodePackageDefMDL_PackageDefNotDefined		(errCodeOffsetOfPackageDefMDL-3)	// ���Ķ���δ����
#define errCodePackageDefMDL_IDOfPackageNotDefined		(errCodeOffsetOfPackageDefMDL-4)	// ���ĵı�ʶδ����
#define errCodePackageDefMDL_InvalidIDOfPackage			(errCodeOffsetOfPackageDefMDL-5)	// �Ƿ��ı��ı�ʶ
#define errCodePackageDefMDL_PackageOfSameID			(errCodeOffsetOfPackageDefMDL-6)	// ���ı�ʶ�ظ�
#define errCodePackageDefMDL_InvalidMaxFldNum			(errCodeOffsetOfPackageDefMDL-7)	// �Ƿ����������Ŀ
#define errCodePackageDefMDL_PackageTypeNotDefined		(errCodeOffsetOfPackageDefMDL-8)	// ��������δ����
#define errCodePackageDefMDL_InvalidPackageType			(errCodeOffsetOfPackageDefMDL-9)	// �Ƿ��ı�������
#define errCodePackageDefMDL_InvalidPrimaryKey			(errCodeOffsetOfPackageDefMDL-10)	// �Ƿ�������
#define errCodePackageDefMDL_FldNotDefined			(errCodeOffsetOfPackageDefMDL-11)	// ��δ����
#define errCodePackageDefMDL_PackageDefOfSpecifiedIDDefined	(errCodeOffsetOfPackageDefMDL-12)	// ָ����ʶ�ı��Ķ����
#define errCodePackageDefMDL_PackFldIsNull			(errCodeOffsetOfPackageDefMDL-13)	// ����������
#define errCodePackageDefMDL_PackFldLength			(errCodeOffsetOfPackageDefMDL-14)	// �����򳤶ȴ�
#define errCodePackageDefMDL_FldNotDefinedInBitMap		(errCodeOffsetOfPackageDefMDL-15)	// λͼ��δ������
#define errCodePackageDefMDL_BitMapHaveValueForNullValueFld	(errCodeOffsetOfPackageDefMDL-16)	// λͼ�ж�����ֵΪ�յ���
#define errCodePackageDefMDL_PackageNotConnected		(errCodeOffsetOfPackageDefMDL-17)	// ����δ����
#define errCodePackageDefMDL_InvalidFldIndex			(errCodeOffsetOfPackageDefMDL-18)	// �Ƿ���������
#define errCodePackageDefMDL_BitMapFldCannotBeSet		(errCodeOffsetOfPackageDefMDL-19)	// λͼ��������
#define errCodePackageDefMDL_InvalidPackageIndex		(errCodeOffsetOfPackageDefMDL-20)	// �Ƿ��ı�������
#define errCodePackageDefMDL_TwoPackageOfSamePackageIndex	(errCodeOffsetOfPackageDefMDL-21)	// �������ľ�����ͬ�ı�������
#define errCodePackageDefMDL_ConnectPackage			(errCodeOffsetOfPackageDefMDL-22)	// ���ӱ��ĳ���
#define errCodePackageDefMDL_PackExchDefTBLOfSpecifiedIDDefined	(errCodeOffsetOfPackageDefMDL-23)	// ָ����ʶ�ı��Ľ�����������
#define errCodePackageDefMDL_IDOfPackExchNotDefined		(errCodeOffsetOfPackageDefMDL-24)	// ָ���ı��Ľ�������
#define errCodePackageDefMDL_InvalidIDOfPackExch		(errCodeOffsetOfPackageDefMDL-25)	// �Ƿ��ı��Ľ��������ʶ
#define errCodePackageDefMDL_PackExchDefTBLNotDefined		(errCodeOffsetOfPackageDefMDL-26)	// ���Ľ��������δ����
#define errCodePackageDefMDL_PackExchDefOfSameID		(errCodeOffsetOfPackageDefMDL-27)	// ���Ľ�����������ͬ�ı�ʶ
#define errCodePackageDefMDL_NotBitMapFld			(errCodeOffsetOfPackageDefMDL-28)	// ����λͼ��
#define errCodePackageDefMDL_TwoMuchBitMapLevel			(errCodeOffsetOfPackageDefMDL-29)	// ��

// �������ʹ���
// offset = -24000
#define errCodeTransClassDefMDL_MaxNumOfTransClassDefNotDefined		(errCodeOffsetOfTransClassDefMDL-1)	// ���Ľ���������δ����
#define errCodeTransClassDefMDL_SpecifiedTransClassDefNotDefined	(errCodeOffsetOfTransClassDefMDL-2)	// ָ���Ľ�������δ����
#define errCodeTransClassDefMDL_TransDefNotDefined			(errCodeOffsetOfTransClassDefMDL-3)	// ����δ����

// SJL06ģ�����
// offset = -23000
#define errCodeSJL06MDL_ServiceFailure		(errCodeOffsetOfSJL06MDL-1)	// ����ʧ��
#define errCodeSJL06MDL_NoWorkingSJL06		(errCodeOffsetOfSJL06MDL-2)	// û�й����������
#define errCodeSJL06MDL_SJL06Def		(errCodeOffsetOfSJL06MDL-3)	// SJL06�����
#define errCodeSJL06MDL_InvalidSJL06Type	(errCodeOffsetOfSJL06MDL-4)	// �Ƿ���SJL06����
#define errCodeSJL06MDL_NotConnected		(errCodeOffsetOfSJL06MDL-5)	// SJL06ģ��δ����
#define errCodeSJL06MDL_WorkingTableFull	(errCodeOffsetOfSJL06MDL-6)	// �����������
#define errCodeSJL06MDL_SJL06NotExists		(errCodeOffsetOfSJL06MDL-7)	// ָ����SJL06������
#define errCodeSJL06MDL_SJL06StillOnline	(errCodeOffsetOfSJL06MDL-8)	// ��SJL06���ڹ���
#define errCodeSJL06MDL_GenerayKeyByStatus      (errCodeOffsetOfSJL06MDL-9)     // ����״̬������ɲ���ӡ��Կ
#define errCodeSJL06MDL_ModifyKeyPosStatus	(errCodeOffsetOfSJL06MDL-10)	// �޸���Կλ��״̬����
#define errCodeSJL06MDL_InvalidLMKPaire		(errCodeOffsetOfSJL06MDL-11)	// �Ƿ���LMK��
#define errCodeSJL06MDL_SharedKeySpaceConfFile	(errCodeOffsetOfSJL06MDL-12)	// ������Կ�ռ������ļ���
#define errCodeSJL06MDL_SharedKeySpaceNotConnected	(errCodeOffsetOfSJL06MDL-13)	// ������Կ�ռ����ӳ���
#define errCodeSJL06MDL_SharedKeySpaceFull	(errCodeOffsetOfSJL06MDL-14)	// ������Կ�ռ�����
#define errCodeSJL06MDL_NoAvaliableSharedKeySpacePos	(errCodeOffsetOfSJL06MDL-15)	// �޿��õĹ�����Կλ��
#define errCodeSJL06MDL_NotSharedKeySpacePos	(errCodeOffsetOfSJL06MDL-16)	// ���ǹ�����Կλ��
#define errCodeSJL06MDL_SharedKeySpacePosNum	(errCodeOffsetOfSJL06MDL-17)	// ������Կ�ռ�λ������
#define errCodeSJL06MDL_SJL06StillNotOnline	(errCodeOffsetOfSJL06MDL-18)	// �����״̬������
#define errCodeSJL06MDL_InvalidHsmResponseCode	(errCodeOffsetOfSJL06MDL-19)	// �Ƿ����������Ӧ����
#define errCodeSJL06MDL_InvalidKeyExchange	(errCodeOffsetOfSJL06MDL-20)	// �Ƿ�����Կ����
#define errCodeSJL06MDL_InvalidHsmRetLen	(errCodeOffsetOfSJL06MDL-21)	// �Ƿ�����������س���
#define errCodeSJL06MDL_PanCannotBeNull		(errCodeOffsetOfSJL06MDL-22)	// PAN����Ϊ��
#define errCodeSJL06MDL_ConnectSJL06		(errCodeOffsetOfSJL06MDL-23)	// ���������
#define errCodeSJL06MDL_DefaultSJL06Conf	(errCodeOffsetOfSJL06MDL-24)	// SJL06ȱʡ���ô���
#define errCodeSJL06MDL_SJL06Abnormal		(errCodeOffsetOfSJL06MDL-25)	// ������쳣
#define errCodeSJL06MDL_ConnectHsmGrp		(errCodeOffsetOfSJL06MDL-26)	// �����������
#define errCodeSJL06MDL_SJL06AlreadyExists	(errCodeOffsetOfSJL06MDL-27)	// SJL06�Ѵ���
#define errCodeSJL06MDL_MustSpecifyIPAddrForSJL06	(errCodeOffsetOfSJL06MDL-28)	// SJL06��IP��ַ����ָ��
#define errCodeSJL06MDL_SJL06AlreadyInGrpFile	(errCodeOffsetOfSJL06MDL-29)	// SJL06�Ѿ�����������ļ�����
#define errCodeSJL06MDL_SJL06NotExistsInGrpFile	(errCodeOffsetOfSJL06MDL-30)	// SJL06������������ļ�����
#define errCodeSJL06MDL_SJL06AlreadyAbnormal	(errCodeOffsetOfSJL06MDL-31)	// ����������쳣״̬
#define errCodeSJL06MDL_SJL06AlreadyBackup	(errCodeOffsetOfSJL06MDL-32)	// ��������Ǳ���״̬
#define errCodeSJL06MDL_SJL06AlreadyOnline	(errCodeOffsetOfSJL06MDL-33)	// �������������״̬
//add by linxj 20130502
#define errCodeSJL06MDL_HsmGrpAlreadyBackup     (errCodeOffsetOfSJL06MDL-34)    // ���������Ϊ����״̬
#define errCodeSJL06MDL_HsmIPNotExists		(errCodeOffsetOfSJL06MDL-35)	// ָ���������IP������


// SJL05ģ�����
// offset = -70000
#define errCodeSJL05MDL_HsmCmdFormatError	(errCodeOffsetOfSJL05MDL-1)	// ָ���ʽ����

// ����ģ�����
// offset = -13000
#define errCodeTaskMDL_CreateTaskInstance	(errCodeOffsetOfTaskMDL-1)	// ��������ʵ����
#define errCodeTaskMDL_ConfFile			(errCodeOffsetOfTaskMDL-2)	// ���������ļ���
#define errCodeTaskMDL_NotConnected		(errCodeOffsetOfTaskMDL-3)	// ����ģ��δ����
#define errCodeTaskMDL_TaskNotExists		(errCodeOffsetOfTaskMDL-4)	// ���񲻴���
#define errCodeTaskMDL_TaskClassAlreadyExists	(errCodeOffsetOfTaskMDL-5)	// ���������Ѵ���
#define errCodeTaskMDL_TaskClassNameTooLong	(errCodeOffsetOfTaskMDL-6)	// ������������̫��
#define errCodeTaskMDL_TaskClassStartCmdTooLong	(errCodeOffsetOfTaskMDL-7)	// ������������̫��
#define errCodeTaskMDL_TaskClassLogFileNameIsNull	(errCodeOffsetOfTaskMDL-8)	// ������־����Ϊ��
#define errCodeTaskMDL_TaskClassNameIsNull	(errCodeOffsetOfTaskMDL-9)	// ������������Ϊ��
#define errCodeTaskMDL_TaskClassLogFileNameTooLong	(errCodeOffsetOfTaskMDL-10)	// ������־����̫��
#define errCodeTaskMDL_TaskClassTableIsFull	(errCodeOffsetOfTaskMDL-11)	// �������ͱ�����
#define errCodeTaskMDL_Connect			(errCodeOffsetOfTaskMDL-12)	// ������������
#define errCodeTaskMDL_TaskClassIsUsed		(errCodeOffsetOfTaskMDL-13)	// �����������ڱ�ʹ��
#define errCodeTaskMDL_TaskClassNameNotDefined	(errCodeOffsetOfTaskMDL-14)	// ������������δ����
#define errCodeTaskMDL_TaskClassNotExists	(errCodeOffsetOfTaskMDL-15)	// �������Ͳ�����
#define errCodeTaskMDL_TaskClassDefLineError	(errCodeOffsetOfTaskMDL-16)	// �������Ͷ������д�
#define errCodeTaskMDL_AnotherTaskOfNameExists	(errCodeOffsetOfTaskMDL-17)	// ͬ���������Ѿ����� 2007/11/30������������

// Socketģ�����
// offset = -22000
#define errCodeSocketMDL_Timeout		(errCodeOffsetOfSocketMDL-1)	// ��ʱ
#define errCodeSocketMDL_ReceiveLen		(errCodeOffsetOfSocketMDL-2)	// ���ճ���
#define errCodeSocketMDL_Error			(errCodeOffsetOfSocketMDL-3)	// ͨѶʧ��
#define errCodeSocketMDL_DataLen		(errCodeOffsetOfSocketMDL-4)	// ���ݳ��ȴ�
#define errCodeSocketMDL_RecvDataTimeout	(errCodeOffsetOfSocketMDL-5)	// �������ݳ�ʱ
#define errCodeSocketMDL_ErrCodeNotSet		(errCodeOffsetOfSocketMDL-6)	// δ���ó�����

// ͨѶģ�����
// offset = -21000
#define errCodeSckCommMDL_ConfFile		(errCodeOffsetOfSckCommMDL-1)	// �����ļ�
#define errCodeSckCommMDL_InvalidWorkingMode	(errCodeOffsetOfSckCommMDL-2)	// �Ƿ�����״̬
#define errCodeSckCommMDL_ServerNotDefined	(errCodeOffsetOfSckCommMDL-3)	// ������δ����
#define errCodeSckCommMDL_ServiceFailure	(errCodeOffsetOfSckCommMDL-4)	// ����ʧ��
#define errCodeSckCommMDL_NoWorkingServer	(errCodeOffsetOfSckCommMDL-5)	// ���ǹ����ŵķ�����
#define errCodeSckCommMDL_InternalError		(errCodeOffsetOfSckCommMDL-6)	// �ڲ�����
#define errCodeSckCommMDL_Connect		(errCodeOffsetOfSckCommMDL-7)	// ����ͨѶģ��


// RECģ�����
// offset = -20000
#define errCodeRECMDL_ConfFile			(errCodeOffsetOfRECMDL-1)	// �����ļ�����
#define errCodeRECMDL_VarNotExists		(errCodeOffsetOfRECMDL-2)	// ����������
#define errCodeRECMDL_VarType			(errCodeOffsetOfRECMDL-3)	// �������ʹ�
#define errCodeRECMDL_VarValue			(errCodeOffsetOfRECMDL-4)	// ����ֵ������
#define errCodeRECMDL_Connect			(errCodeOffsetOfRECMDL-5)	// ���ӱ�������ģ��
#define errCodeRECMDL_VarAlreadyExists		(errCodeOffsetOfRECMDL-6)	// �����Ѵ���
#define errCodeRECMDL_VarTBLIsFull		(errCodeOffsetOfRECMDL-7)	// ����������
#define errCodeRECMDL_VarNameLength		(errCodeOffsetOfRECMDL-8)	// �������Ƴ��ȴ�
#define errCodeRECMDL_VarValueTooLong		(errCodeOffsetOfRECMDL-9)	// ֵ̫����
#define errCodeRECMDL_RealNumError		(errCodeOffsetOfRECMDL-10)	// ��������Ŀ��

// ��Կģ�����
// offset = -12000
#define errCodePKDBMDL_KeyNonExists		(errCodeOffsetOfPKDBMDL-1)	// ��Կ������
#define errCodePKDBMDL_KeyNum			(errCodeOffsetOfPKDBMDL-2)	// ��Կ������
#define errCodePKDBMDL_KeyDBFull		(errCodeOffsetOfPKDBMDL-3)	// ��Կ����
#define errCodePKDBMDL_KeyDBNonConnected	(errCodeOffsetOfPKDBMDL-4)	// ��Կ��δ����
#define errCodePKDBMDL_InvalidKeyLength		(errCodeOffsetOfPKDBMDL-5)	// �Ƿ�����Կ����
#define errCodePKDBMDL_KeyAlreadyExists		(errCodeOffsetOfPKDBMDL-6)	// ��Կ�Ѿ�����
#define errCodePKDBMDL_KeyOutdate		(errCodeOffsetOfPKDBMDL-7)	// ��Կ������Ч��
#define errCodePKDBMDL_KeyValueWithoutEnd	(errCodeOffsetOfPKDBMDL-8)	// ��Կֵ�޽�����
#define errCodePKDBMDL_KeyIndexFileAlreadyExists	(errCodeOffsetOfPKDBMDL-9)	// ��Կ�ļ��Ѵ���
#define errCodePKDBMDL_KeyIndexFileNotExists	(errCodeOffsetOfPKDBMDL-10)	// ��Կ�ļ�������
#define errCodePKDBMDL_TBLLocked		(errCodeOffsetOfPKDBMDL-11)	// ��Կ�ļ��ѱ���ס
#define errCodePKDBMDL_SomeRecIsWritingLocked	(errCodeOffsetOfPKDBMDL-12)	// �м�¼��д��ס��
#define errCodePKDBMDL_KeyNotEffective		(errCodeOffsetOfPKDBMDL-13)	// ��Կ��Ч
#define errCodePKDBMDL_Connect			(errCodeOffsetOfPKDBMDL-14)	// ���ӿ�
#define errCodePKDBMDL_KeyNotLocked		(errCodeOffsetOfPKDBMDL-15)	// ��Կδ����ס
#define errCodePKDBMDL_KeyAlreadyLocked		(errCodeOffsetOfPKDBMDL-16)	// ��Կ�ѱ���ס

// ����Աģ�����
// offset = -19000
#define errCodeOperatorMDL_OperatorNum		(errCodeOffsetOfOpertatorMDL-1)	// ����Ա��Ŀ��
#define errCodeOperatorMDL_OperatorNotExists	(errCodeOffsetOfOpertatorMDL-2)	// ����Ա������
#define errCodeOperatorMDL_NotConnected		(errCodeOffsetOfOpertatorMDL-3)	// ����Աģ��δ����
#define errCodeOperatorMDL_TableFull		(errCodeOffsetOfOpertatorMDL-4)	// ����Ա����
#define errCodeOperatorMDL_ReadOperator		(errCodeOffsetOfOpertatorMDL-5)	// ������Ա
#define errCodeOperatorMDL_WrongPassword	(errCodeOffsetOfOpertatorMDL-6)	// ����Ա�����
#define errCodeOperatorMDL_PasswordLocked	(errCodeOffsetOfOpertatorMDL-7)	// ����Ա���뱻��ס��
#define errCodeOperatorMDL_AlreadyLogon		(errCodeOffsetOfOpertatorMDL-8)	// ����Ա�Ѿ���¼��
#define errCodeOperatorMDL_NotLogon		(errCodeOffsetOfOpertatorMDL-9)	// ����Ա��û�е�¼
#define errCodeOperatorMDL_TooLongTimeWithoutOperation		(errCodeOffsetOfOpertatorMDL-10)	// ����Ա̫��ʱ��û�в���
#define errCodeOperatorMDL_StillLogon		(errCodeOffsetOfOpertatorMDL-11)	// ����Ա�ڵ�¼״̬
#define errCodeOperatorMDL_CannotDeleteSelf	(errCodeOffsetOfOpertatorMDL-12)	// ����ɾ���Լ�
#define errCodeOperatorMDL_OperatorAlreadyExists	(errCodeOffsetOfOpertatorMDL-13)	// ����Ա�Ѵ���
#define errCodeOperatorMDL_OperatorPwdOverTime	(errCodeOffsetOfOpertatorMDL-14)	// ����Ա�����ѹ��� 
#define errCodeOperatorMDL_OperatorOverMaxFreeTime	(errCodeOffsetOfOpertatorMDL-15)	// ����Ա����������ʱ��
#define errCodeOperatorMDL_NotLegitimateClient	(errCodeOffsetOfOpertatorMDL-16)	// ����Ա�����ںϷ��ͻ��˵�½
#define errCodeOperatorMDL_NoPermissions	(errCodeOffsetOfOpertatorMDL-17)	// ����Աû��Ȩ��ִ�иò���
#define errCodeOperatorMDL_OperatorIsLocked	(errCodeOffsetOfOpertatorMDL-18)	// ����Ա����ס��
#define errCodeOperatorMDL_AuthOperatorIsNotValid   (errCodeOffsetOfOpertatorMDL-19)    // ��Ȩ����Ա��֤��ͨ��
#define errCodeOperatorMDL_PasswordIsSame	(errCodeOffsetOfOpertatorMDL-20) // �¾�������ͬ
#define errCodeOperatorMDL_IdentifyCodeNotExists	(errCodeOffsetOfOpertatorMDL-21)	// ��֤�벻����
#define errCodeOperatorMDL_InvalidLogon		(errCodeOffsetOfOpertatorMDL-22)	// �Ƿ���¼
#define errCodeOperatorMDL_OperatorPwdOldEncType	(errCodeOffsetOfOpertatorMDL-23)	//����Ϊ�ɸ�ʽ 


// ��Ϣ����������
// offset = -18000
#define errCodeMsgBufMDL_NotConnected		(errCodeOffsetOfMsgBufMDL-1)	// ��Ϣ������δ����
#define errCodeMsgBufMDL_OutofRange		(errCodeOffsetOfMsgBufMDL-2)	// ����
#define errCodeMsgBufMDL_InitDef		(errCodeOffsetOfMsgBufMDL-3)	// ��ʼ�����
#define errCodeMsgBufMDL_ConnectIndexTBL	(errCodeOffsetOfMsgBufMDL-4)	// �����������
#define errCodeMsgBufMDL_ProviderExit		(errCodeOffsetOfMsgBufMDL-5)	// �ṩ�����˳�
#define errCodeMsgBufMDL_NotOriginMsg		(errCodeOffsetOfMsgBufMDL-6)	// ����ԭʼ��Ϣ
#define errCodeMsgBufMDL_InvalidIndex		(errCodeOffsetOfMsgBufMDL-7)	// �Ƿ�������
#define errCodeMsgBufMDL_ReadIndex		(errCodeOffsetOfMsgBufMDL-8)	// ��ȡ����
#define errCodeMsgBufMDL_MsgLen			(errCodeOffsetOfMsgBufMDL-9)	// ��Ϣ���ȴ�
#define errCodeMsgBufMDL_MsgOfTypeAndOfIndexNotExists	(errCodeOffsetOfMsgBufMDL-10)	// ����ָ�����ͺ���������Ϣ������
#define errCodeMsgBufMDL_MsgIndexAlreadyLosted	(errCodeOffsetOfMsgBufMDL-11)	// ��Ϣ�����Ѿ���ʧ
#define errCodeMsgBufMDL_PointerAbnormal	(errCodeOffsetOfMsgBufMDL-12)	// ��Ϣ������ָ���쳣

// ���׼����Ϣ����������
// offset = -4000
#define errCodeTransSpierBufMDL_NotConnected		(errCodeOffsetOfTransSpierBufMDL-1)	// ��Ϣ������δ����
#define errCodeTransSpierBufMDL_OutofRange		(errCodeOffsetOfTransSpierBufMDL-2)	// ����
#define errCodeTransSpierBufMDL_InitDef			(errCodeOffsetOfTransSpierBufMDL-3)	// ��ʼ�����
#define errCodeTransSpierBufMDL_ConnectIndexTBL		(errCodeOffsetOfTransSpierBufMDL-4)	// �����������
#define errCodeTransSpierBufMDL_ProviderExit		(errCodeOffsetOfTransSpierBufMDL-5)	// �ṩ�����˳�
#define errCodeTransSpierBufMDL_NotOriginMsg		(errCodeOffsetOfTransSpierBufMDL-6)	// ����ԭʼ��Ϣ
#define errCodeTransSpierBufMDL_InvalidIndex		(errCodeOffsetOfTransSpierBufMDL-7)	// �Ƿ�������
#define errCodeTransSpierBufMDL_ReadIndex		(errCodeOffsetOfTransSpierBufMDL-8)	// ��ȡ����
#define errCodeTransSpierBufMDL_MsgLen			(errCodeOffsetOfTransSpierBufMDL-9)	// ��Ϣ���ȴ�
#define errCodeTransSpierBufMDL_MsgOfTypeAndOfIndexNotExists	(errCodeOffsetOfTransSpierBufMDL-10)	// ����ָ�����ͺ���������Ϣ������

// �����ڴ�ģ�����
// offset = -17000
#define errCodeSharedMemoryMDL_MDLNotDefined	(errCodeOffsetOfSharedMemoryMDL-1)	// �����ڴ�ģ��δ����
#define errCodeSharedMemoryMDL_LockMDL		(errCodeOffsetOfSharedMemoryMDL-2)	// �������ڴ��
#define errCodeSharedMemoryMDL_Connect		(errCodeOffsetOfSharedMemoryMDL-3)	// ���ӹ����ڴ�ģ��

// ��־ģ�����
// offset = -16000
#define errCodeLogMDL_ConfFile			(errCodeOffsetOfLogMDL-1)	// �����ļ���
#define errCodeLogMDL_LogFileAlreadyExists	(errCodeOffsetOfLogMDL-2)	// ��־�Ѵ���
#define errCodeLogMDL_LogFileTblFull		(errCodeOffsetOfLogMDL-3)	// ��־������
#define errCodeLogMDL_LogFileUsedByUsers	(errCodeOffsetOfLogMDL-4)	// ��־����ʹ��
#define errCodeLogMDL_LogFileNotExists		(errCodeOffsetOfLogMDL-5)	// ��־������
#define errCodeLogMDL_NotConnected		(errCodeOffsetOfLogMDL-6)	// ��־ģ��δ����
#define errCodeLogMDL_Connect			(errCodeOffsetOfLogMDL-7)	// ������־ģ��
#define errCodeLogMDL_LogFileNameMustSpecified	(errCodeOffsetOfLogMDL-8)	// ����ָ����־����

// ��������ģ�����
// offset = -14000
#define errCodeEnviMDL_VarNotExists		(errCodeOffsetOfEnviMDL-1)	// ����������
#define errCodeEnviMDL_NullLine			(errCodeOffsetOfEnviMDL-2)	// ����
#define errCodeEnviMDL_OccupiedByOther		(errCodeOffsetOfEnviMDL-3)	// ��������ģ�鱻����ռ����
#define errCodeEnviMDL_NoValueDefinedForVar	(errCodeOffsetOfEnviMDL-4)	// ����ֵû����
#define errCodeEnviMDL_VarAlreadyExists		(errCodeOffsetOfEnviMDL-5)	// �����Ѵ���
#define errCodeEnviMDL_NotRecFormatDefStr	(errCodeOffsetOfEnviMDL-6)	// ���Ǽ�¼��ʽ�����ַ���
#define errCodeEnviMDL_NotRecStrTooLong		(errCodeOffsetOfEnviMDL-7)	// ��¼�ַ���̫��
#define errCodeEnviMDL_NotRecDefStr		(errCodeOffsetOfEnviMDL-8)	// ���Ǽ�¼���崮

// ���ݿⱸ��ģ�����
// offset = -15000
#define errCodeKeyDBBackuperMDL_ConfError	(errCodeOffsetOfKeyDBBackuperMDL-1)	// ���ó���
#define errCodeKeyDBBackuperMDL_IPAddrMustInput	(errCodeOffsetOfKeyDBBackuperMDL-2)	// IP��ַ��������
#define errCodeKeyDBBackuperMDL_PortMustInput	(errCodeOffsetOfKeyDBBackuperMDL-3)	// PORT��������
#define errCodeKeyDBBackuperMDL_BrotherNotDefined	(errCodeOffsetOfKeyDBBackuperMDL-4)	// δ����ı��ݷ�����
#define errCodeKeyDBBackuperMDL_BrotherReturnFailure	(errCodeOffsetOfKeyDBBackuperMDL-5)	// ����������ʧ��	// 2007/11/29����
#define errCodeKeyDBBackuperMDL_BrotherNotActive	(errCodeOffsetOfKeyDBBackuperMDL-6)	// ���ݷ������ǻ״̬	// 2007/12/25����

// DES��Կ��ģ�����
// offset = -11000
#define errCodeDesKeyDBMDL_KeyNonExists		(errCodeOffsetOfDesKeyDBMDL-1)	// ��Կ������
#define errCodeDesKeyDBMDL_KeyNum		(errCodeOffsetOfDesKeyDBMDL-2)	// ��Կ������
#define errCodeDesKeyDBMDL_KeyDBFull		(errCodeOffsetOfDesKeyDBMDL-3)	// ����
#define errCodeDesKeyDBMDL_KeyDBNonConnected	(errCodeOffsetOfDesKeyDBMDL-4)	// ��δ����
#define errCodeDesKeyDBMDL_InvalidKeyLength	(errCodeOffsetOfDesKeyDBMDL-5)	// �Ƿ���Կ����
#define errCodeDesKeyDBMDL_KeyAlreadyExists	(errCodeOffsetOfDesKeyDBMDL-6)	// ��Կ�Ѵ���
#define errCodeDesKeyDBMDL_KeyOutdate		(errCodeOffsetOfDesKeyDBMDL-7)	// ��Կ����
#define errCodeDesKeyDBMDL_KeyValueWithoutEnd	(errCodeOffsetOfDesKeyDBMDL-8)	// ��Կֵ�޽�����
#define errCodeDesKeyDBMDL_KeyIndexFileAlreadyExists	(errCodeOffsetOfDesKeyDBMDL-9)	// ��Կ�ļ��Ѵ���
#define errCodeDesKeyDBMDL_KeyIndexFileNotExists	(errCodeOffsetOfDesKeyDBMDL-10)	// ��Կ�ļ�������
#define errCodeDesKeyDBMDL_TBLLocked		(errCodeOffsetOfDesKeyDBMDL-11)	// ��Կ�ļ��ѱ���ס
#define errCodeDesKeyDBMDL_SomeRecIsWritingLocked	(errCodeOffsetOfDesKeyDBMDL-12)	// �м�¼��д��ס��
#define errCodeDesKeyDBMDL_KeyNotEffective	(errCodeOffsetOfDesKeyDBMDL-13)	// ��Կ��Ч
#define errCodeDesKeyDBMDL_Connect		(errCodeOffsetOfDesKeyDBMDL-14)	// ���ӿ�
#define errCodeDesKeyDBMDL_KeyNotLocked		(errCodeOffsetOfDesKeyDBMDL-15)	// ��Կδ����ס
#define errCodeDesKeyDBMDL_InvalidKeyRec	(errCodeOffsetOfDesKeyDBMDL-16)	// �Ƿ�����Կ��¼
#define errCodeDesKeyDBMDL_KeyValueNotSameAsExpected	(errCodeOffsetOfDesKeyDBMDL-17)	// �����ڴ�����Կֵ��2007/11/28����
#define errCodeDesKeyDBMDL_KeyAlreadyLocked	(errCodeOffsetOfDesKeyDBMDL-18)	// ��Կ�ѱ���ס
#define errCodeDesKeyDBMDL_KeyStrategyInactive	(errCodeOffsetOfDesKeyDBMDL-19)	// ��Կ����δ����

// �������ƽ̨����
// offset = -29000
#define errCodeEssc_InvalidUseKey		(errCodeOffsetOfEsscMDL-1)	// �Ƿ�ʹ����Կ
#define errCodeEssc_CheckValue			(errCodeOffsetOfEsscMDL-2)	// У��ֵ��
#define errCodeEssc_KeyLength			(errCodeOffsetOfEsscMDL-3)	// ��Կ���ȴ�
#define errCodeEssc_AppNotDefined		(errCodeOffsetOfEsscMDL-4)	// Ӧ��δ����
#define errCodeEssc_LmkNotDefined		(errCodeOffsetOfEsscMDL-5)	// LMKδ����
#define errCodeEsscMDLHsmKeyPosOccupied		(errCodeOffsetOfEsscMDL-6)	// �������Կλ���ѱ�ռ
#define errCodeEsscMDLNoAvaliableHsmKeyPos	(errCodeOffsetOfEsscMDL-7)	// �����õ��������Կλ��
#define errCodeEsscMDLNoEnoughHsmKeyPos		(errCodeOffsetOfEsscMDL-8)	// �������Կλ�ò���
#define errCodeEsscMDLInitHsmKeyPosForSK	(errCodeOffsetOfEsscMDL-9)	// ΪSK��ʼ���������Կλ��
#define errCodeEsscMDLWrongMaxAgentNum		(errCodeOffsetOfEsscMDL-10)	// ����������������
#define errCodeEsscMDLWrongAgentNo		(errCodeOffsetOfEsscMDL-11)	// ����������
#define errCodeEsscMDLAgentNoAlreadExists	(errCodeOffsetOfEsscMDL-12)	// �����Ѵ���
#define errCodeEsscMDLAgentTBLIsFull		(errCodeOffsetOfEsscMDL-13)	// ���������
#define errCodeEsscMDLWrongAgentType		(errCodeOffsetOfEsscMDL-14)	// �������������
#define errCodeEsscMDLInvalidAgentType		(errCodeOffsetOfEsscMDL-15)	// �Ƿ�����������
#define errCodeEsscMDLWrongMaxAtmNum		(errCodeOffsetOfEsscMDL-16)	// ��������ATM����
#define errCodeEsscMDLWrongAtmNo		(errCodeOffsetOfEsscMDL-17)	// �����ATM���
#define errCodeEsscMDLAtmNoAlreadExists		(errCodeOffsetOfEsscMDL-18)	// ATM�Ѵ���
#define errCodeEsscMDLAtmTBLIsFull		(errCodeOffsetOfEsscMDL-19)	// ATM������
#define errCodeEsscMDLAtmNotExists		(errCodeOffsetOfEsscMDL-20)	// ATM������
#define errCodeEsscMDLWrongBranchNo		(errCodeOffsetOfEsscMDL-21)	// ����ķ��к�
#define errCodeEsscMerelyGeneralBranchAction	(errCodeOffsetOfEsscMDL-22)	// ֻ�����вſ�ִ�б�����
#define errCodeEsscNotGeneralBranchAction	(errCodeOffsetOfEsscMDL-23)	// ���в���ִ�иò���
#define errCodeEsscKeyCannotGeneratedByGeneralBranch	(errCodeOffsetOfEsscMDL-24)	// ���в������ɸ���Կ
#define errCodeEsscConnectAgentTBL		(errCodeOffsetOfEsscMDL-25)	// ������������
#define errCodeEsscConnectAtmKeyTBL		(errCodeOffsetOfEsscMDL-26)	// ����ATM�����
#define errCodeEsscAtmUseBranchTMK		(errCodeOffsetOfEsscMDL-27)	// ATMʹ�÷��е�TMK
#define errCodeEsscMDLReqStr			(errCodeOffsetOfEsscMDL-28)	// ���󴮳���
#define errCodeEsscMDLNotKMSvrServer		(errCodeOffsetOfEsscMDL-29)	// ������Կ���������
#define errCodeEsscMDLNotKMSvrClient		(errCodeOffsetOfEsscMDL-30)	// ������Կ����ͻ���
#define errCodeEsscMDLKeyOfSKAgentCannotBeStored	(errCodeOffsetOfEsscMDL-31)	// SK�������Կ���ܴ洢
#define errCodeEsscMDLKeyOperationNotPermitted	(errCodeOffsetOfEsscMDL-32)	// ��Կ����������
#define errCodeEsscMDLInvalidKeyOperation	(errCodeOffsetOfEsscMDL-33)	// �Ƿ�����Կ����
#define errCodeEsscMDLInvalidKeyTable		(errCodeOffsetOfEsscMDL-34)	// �Ƿ�����Կ��
#define errCodeEsscMDLConnectKMSvrCommConf	(errCodeOffsetOfEsscMDL-35)	// ������Կ���������ͨѶ����
#define errCodeEsscMDL_InvalidMDLID		(errCodeOffsetOfEsscMDL-36)	// �Ƿ���ģ���
#define errCodeEsscMDL_ProcNotExistsAnymore	(errCodeOffsetOfEsscMDL-37)	// �����Ѳ�����
#define errCodeEsscMDL_MsgForThisModuleTooShort	(errCodeOffsetOfEsscMDL-38)	// �ṩ����ģ�����Ϣ̫����
#define errCodeEsscMDL_WaitResponse		(errCodeOffsetOfEsscMDL-39)	// �ȴ���Ӧ����
#define errCodeEsscMDL_PeerDataError		(errCodeOffsetOfEsscMDL-40)	// �Է�����������
#define errCodeEsscMDL_TimeoutForReceivingData	(errCodeOffsetOfEsscMDL-41)	// �������ݳ�ʱ
#define errCodeEsscMDL_InvalidService		(errCodeOffsetOfEsscMDL-42)	// �Ƿ��ķ���
#define errCodeEsscMDL_InvalidUseOfKey		(errCodeOffsetOfEsscMDL-43)	// �Ƿ�ʹ����Կ
#define errCodeEsscMDL_CVVVerifyFailure		(errCodeOffsetOfEsscMDL-44)	// CVV��֤ʧ��
#define errCodeEsscMDL_TwoMacNotSame		(errCodeOffsetOfEsscMDL-45)	// ����MAC��ͬ
#define errCodeEsscMDL_AnotherTaskOfSameFunExists	(errCodeOffsetOfEsscMDL-46)	// ��һ��ͬ���ܵ������Ѵ���
#define errCodeEsscMDL_NotValidKeyDownloadRequest	(errCodeOffsetOfEsscMDL-47)	// �Ƿ�����Կ��������
#define errCodeEsscMDL_NotValidKeyFinishRequest	(errCodeOffsetOfEsscMDL-48)	// �Ƿ�����Կ�������
#define errCodeEsscMDL_HsmStoreKeysCmdNotFound	(errCodeOffsetOfEsscMDL-49)	// ������洢��Կָ��δ����
#define errCodeEsscMDL_NoAvailableTmpKeyPos	(errCodeOffsetOfEsscMDL-50)	// �����õ���Կ��ʱλ��
#define errCodeEsscMDL_TmpKeyPosNotExists	(errCodeOffsetOfEsscMDL-51)	// ��ʱ��Կλ�ò�����
#define errCodeEsscMDL_ShortRetLenOfTmpKeyPosSvr	(errCodeOffsetOfEsscMDL-52)	// ��ʱ��Կλ�÷������ķ��س���̫��
#define errCodeEsscMDL_HsmTaskAbnormal		(errCodeOffsetOfEsscMDL-53)	// ����������쳣
#define errCodeEsscMDL_BranchAlreadyExists	(errCodeOffsetOfEsscMDL-54)	// �����Ѿ�����
#define errCodeEsscMDL_ClientReqPack		(errCodeOffsetOfEsscMDL-55)	// �ͻ��������Ĵ�
#define errCodeEsscMDL_DecryptPin		(errCodeOffsetOfEsscMDL-57)	// PIN����ʧ��
#define errCodeEsscMDL_TranslatePin		(errCodeOffsetOfEsscMDL-58)	// PINת��ʧ��
#define errCodeEsscMDL_TranslateKey		(errCodeOffsetOfEsscMDL-59)	// ��Կת��ʧ��
#define errCodeEsscMDL_InvalidEsscIPAddr	(errCodeOffsetOfEsscMDL-60)	// ESSCIP�Ƿ�
#define errCodeEsscMDL_NotDefineEsscIPAddr	(errCodeOffsetOfEsscMDL-61)	// ESSCIPδ����
#define errCodeEsscMDL_TCPIPTaskTooIdle		(errCodeOffsetOfEsscMDL-62)	// TCPIPSvr������ʵ��̫������
#define errCodeEsscMDL_EsscSpierClientAlreadyExists		(errCodeOffsetOfEsscMDL-63)	// ��ؿͻ����Ѿ�����
#define errCodeEsscMDL_EsscSpierClientTBLFull	(errCodeOffsetOfEsscMDL-64)	// ��ؿͻ����б��Ѿ�����
#define errCodeEsscMDL_EsscSpierClientNotExists	(errCodeOffsetOfEsscMDL-65)	// ��ؿͻ��˲�����
#define errCodeEsscMDL_EsscDefaultSpierClientCannotBeDeleted	(errCodeOffsetOfEsscMDL-66)	// ȱʡ��ؿͻ��˲��ܱ�ɾ��
#define errCodeEsscMDL_FindEsscRes		(errCodeOffsetOfEsscMDL-67)	// �Ҳ�����Դ
#define errCodeEsscMDL_ClientDataLenIsZero	(errCodeOffsetOfEsscMDL-68)	// �ͻ������ݳ�����0
#define errCodeEsscMDL_ClientDataLenTooLong	(errCodeOffsetOfEsscMDL-69)	// �ͻ������ݳ���̫��
#define errCodeEsscMDL_ClientDataLenTooShort	(errCodeOffsetOfEsscMDL-70)	// �ͻ������ݳ���̫��
#define errCodeEsscMDL_InvalidResID		(errCodeOffsetOfEsscMDL-71)	// �Ƿ�����ԴID
#define errCodeEsscMDL_WrongResInterprotorCalled	(errCodeOffsetOfEsscMDL-72)	// �����˴������Դ�ܵó���
#define errCodeEsscMDL_WrongResMngSvrCmd	(errCodeOffsetOfEsscMDL-73)	// �Ƿ�����Դ��������
#define errCodeEsscMDL_WrongPasswordLength	(errCodeOffsetOfEsscMDL-74)	// ���볤�ȴ�
#define errCodeEsscMDL_ConnectEsscSpierClientTBL	(errCodeOffsetOfEsscMDL-75)	// ���Ӽ�ؿͻ��˿��Ʊ�
#define errCodeEsscMDL_ResCannotBeSpied		(errCodeOffsetOfEsscMDL-76)	// ����Դ�����Ա����
#define errCodeEsscMDL_CommErrForSendingResSpierInfo	(errCodeOffsetOfEsscMDL-77)	// ��ͨѶԭ������Դ�����Ϣʧ��
#define errCodeEsscMDL_TransSpierCommManagePoolIsFull	(errCodeOffsetOfEsscMDL-78)	// ���׼��ͨѶ���������
#define errCodeEsscMDL_NoMaxHsmGrpNumDefinedInLocalKMSvrDef	(errCodeOffsetOfEsscMDL-79)	// �ڱ�����Կ���������ļ���δ����������������Ŀ
#define errCodeEsscMDL_ReqAndResNotIsIndentified	(errCodeOffsetOfEsscMDL-80)	// ��������Ӧ��ƥ��
#define errCodeEsscMDL_NoLocalKMSvrDefinedForHsmGrp	(errCodeOffsetOfEsscMDL-81)	// û��Ϊָ����������鶨�屾����Կ���������
#define errCodeEsscMDL_NoRemoteKMSvrDefinedForContainer	(errCodeOffsetOfEsscMDL-82)	// û��Ϊָ������Կ��������Զ����Կ���������
#define errCodeEsscMDL_TooMuchEsscPackageFld	(errCodeOffsetOfEsscMDL-83)	// ̫���ESSC������
#define errCodeEsscMDL_EsscPackageFldNotFound	(errCodeOffsetOfEsscMDL-84)	// ESSC������δ�ڸñ����ж���
#define errCodeEsscMDL_EsscPackageFldLength	(errCodeOffsetOfEsscMDL-85)	// ESSC�����򳤶ȴ�
#define errCodeEsscMDL_EsscPackageLength	(errCodeOffsetOfEsscMDL-86)	// ESSC���ĳ��ȴ�
#define errCodeEsscMDL_NotEsscRequestPackage	(errCodeOffsetOfEsscMDL-87)	// ����ESSC������
#define errCodeEsscMDL_NotEsscResponsePackage	(errCodeOffsetOfEsscMDL-88)	// ����ESSC��Ӧ����
#define errCodeEsscMDL_RemoteKeyMngNotPermitted	(errCodeOffsetOfEsscMDL-89)	// ������Զ�̹������Կ
#define errCodeEsscMDL_NotPermitUseKey		(errCodeOffsetOfEsscMDL-90)	// ������ʹ����Կ
#define errCodeEsscMDL_ZmkAndWKNotOwnedBySameOne	(errCodeOffsetOfEsscMDL-91)	// ZMK�͹�����Կ������ͬһ����
#define errCodeEsscMDL_WrongUsageOfKey		(errCodeOffsetOfEsscMDL-92)	// �Ƿ�ʹ����Կ
#define errCodeEsscMDL_NotKMSvr			(errCodeOffsetOfEsscMDL-93)	// ����KMSvr
#define errCodeEsscMDL_NoContainerSetForKey	(errCodeOffsetOfEsscMDL-94)	// û��Ϊ��Կ����container
#define errCodeEsscMDL_InvalidOperation		(errCodeOffsetOfEsscMDL-95)	// �Ƿ�����
#define errCodeEsscMDL_NoIPAddrDefinedInMngSvrClientDefFile	(errCodeOffsetOfEsscMDL-96)	// ��mngSvr�Ŀͻ��˶����ļ���û�ж���ESSC��IP��ַ
#define errCodeEsscMDL_NoPortDefinedInMngSvrClientDefFile	(errCodeOffsetOfEsscMDL-97)	// ��mngSvr�Ŀͻ��˶����ļ���û�ж���ESSC�Ķ˿�
#define errCodeEsscMDL_DataLenFromMngSvr	(errCodeOffsetOfEsscMDL-98)	// ��mngSvr���������ݳ��ȴ�
#define errCodeEsscMDL_NoResponsePackFromMngSvr	(errCodeOffsetOfEsscMDL-99)	// mngSvrδ������Ӧ��
#define errCodeEsscMDL_AnotherStartingSendDataCmd	(errCodeOffset2OfEsscMDL-1)	// ��һ��mngSvr���ݿ�ʼ���Ͱ�
#define errCodeEsscMDL_NotFinishSendingData	(errCodeOffset2OfEsscMDL-2)	// mngSvr���ݴ���δ����
#define errCodeEsscMDL_FailToGetTempFile	(errCodeOffset2OfEsscMDL-3)	// ���ܻ�ȡ��ʱ�ļ�
#define errCodeEsscMDL_NoStartingSendDataCmdBefore	(errCodeOffset2OfEsscMDL-4)	// û���յ������ݿ�ʼ�����
#define errCodeEsscMDL_FldNotExists		(errCodeOffset2OfEsscMDL-5)	// �򲻴���
#define errCodeEsscMDL_PVVVerifyFailure		(errCodeOffset2OfEsscMDL-6)	// PVV��֤ʧ��
#define errCodeEsscMDL_NoOperationAuthority	(errCodeOffset2OfEsscMDL-7)	// �޲���Ȩ��
#define errCodeEsscMDL_NotOperationDefLine	(errCodeOffset2OfEsscMDL-8)	// ���ǲ���������
#define errCodeEsscMDL_NoSuchOperationDefined	(errCodeOffset2OfEsscMDL-9)	// û�ж������
#define errCodeEsscMDL_FldIndexOutOfPackageFldNum	(errCodeOffset2OfEsscMDL-10)	// �������ų���������Ŀ
#define errCodeEsscMDL_PackageFldValueLen	(errCodeOffset2OfEsscMDL-11)	// ��ֵ���ȴ�
#define errCodeEsscMDL_ResSpiedAlreadyOpened	(errCodeOffset2OfEsscMDL-12)	// �Ը���Դ����ѱ���
#define errCodeEsscMDL_ResSpiedNotOpened	(errCodeOffset2OfEsscMDL-13)	// �Ը���Դ���δ����
#define errCodeEsscMDL_EsscResAlreadyExists	(errCodeOffset2OfEsscMDL-14)	// ��Դ�Ѿ�����
#define errCodeEsscMDL_EsscResNotExists		(errCodeOffset2OfEsscMDL-15)	// ��Դ���岻����
#define errCodeEsscMDL_EsscAppAlreadyExists	(errCodeOffset2OfEsscMDL-16)	// Ӧ���Ѿ�����
#define errCodeEsscMDL_EsscAppIDMustInput	(errCodeOffset2OfEsscMDL-17)	// ��������Ӧ��ID��
#define errCodeEsscMDL_LocalKMSvrDefAlreadyExists	(errCodeOffset2OfEsscMDL-18)	// ������Կ��������������Ѵ���
#define errCodeEsscMDL_EsscHsmGrpIDMustInput	(errCodeOffset2OfEsscMDL-19)	// ��������������
#define errCodeEsscMDL_EsscHsmGrpIDNotDefined	(errCodeOffset2OfEsscMDL-20)	// �������û�ж���
#define errCodeEsscMDL_RemoteKMSvrDefAlreadyExists	(errCodeOffset2OfEsscMDL-21)	// Զ����Կ��������������Ѵ���
#define errCodeEsscMDL_NotEsscFldTagDefLine	(errCodeOffset2OfEsscMDL-22)	// ����EsscFldTag������
#define errCodeEsscMDL_ThisOperationForbidden	(errCodeOffset2OfEsscMDL-23)	// ��������ֹ
#define errCodeEsscMDL_SendRequestTimeout	(errCodeOffset2OfEsscMDL-24)	// ��������ʱ
#define errCodeEsscMDL_SendResponseTimeout	(errCodeOffset2OfEsscMDL-25)	// ������Ӧ��ʱ
#define errCodeEsscMDL_ResponseCodeNotSuccess	(errCodeOffset2OfEsscMDL-26)	// ��Ӧ�벻�ǳɹ�
#define errCodeEsscMDL_ResponseToShort		(errCodeOffset2OfEsscMDL-27)	// ��Ӧ����̫��
#define errCodeEsscMDL_ZmkIndexNotDefined	(errCodeOffset2OfEsscMDL-28)	// ZMK����û�ж���
#define errCodeEsscMDL_VerifyAutSign		(errCodeOffset2OfEsscMDL-29)	// ��֤�Զ����ǩ������
#define errCodeEsscMDL_IDOfAppNotDefined	(errCodeOffset2OfEsscMDL-30)	// δ����Ӧ��
#define errCodeEsscMDL_RsaPairNotDefinedForApp	(errCodeOffset2OfEsscMDL-31)	// û��ΪӦ�ö���˽Կ��
#define errCodeEsscMDL_NotMngSvrDefDefLine	(errCodeOffset2OfEsscMDL-32)	// ����mngSvr�Ķ����С� 2007/12/22
// Mary add begin, 2008-9-9
#define errCodeEsscMDL_BufferIsFull		(errCodeOffset2OfEsscMDL-33)	// ���������
#define errCodeEsscMDL_ReachPreDefinedTime	(errCodeOffset2OfEsscMDL-34)	// ����Ԥ����ʱ��
// Mary add end, 2008-9-9
#define errCodeEsscMDL_MngSvrNotDefined		(errCodeOffset2OfEsscMDL-35)	// δ����mngSvr
#define errCodeEsscMDL_SecuControlInactive	(errCodeOffset2OfEsscMDL-36)	// �ؼ�δ����
#define errCodeEsscMDL_SecuControlOutdate	(errCodeOffset2OfEsscMDL-37)	// �ؼ�ʧЧ
#define errCodeEsscMDL_SecuControlExceedMaxDownloadTimes	(errCodeOffset2OfEsscMDL-38)	// �ؼ����ش��������涨����
#define errCodeEsscMDL_DBSvrNotDefined		(errCodeOffset2OfEsscMDL-39)	// DBServerû����
#define errCodeEsscMDL_KeyObjectTypeNotSupported	(errCodeOffset2OfEsscMDL-40)	// ��Կ�������Ͳ�֧��
#define errCodeEsscMDL_VKPosNotOccupiedByThisApp	(errCodeOffset2OfEsscMDL-41)	// ˽Կλ�ò��Ǳ����Ӧ��ռ��
#define errCodeEsscMDL_HsmVKSpaceSizeTooSmall	(errCodeOffset2OfEsscMDL-42)	// ˽Կ�ռ�̫С
#define errCodeEsscMDL_TBLDefGrpInitFailure	(errCodeOffset2OfEsscMDL-43)	// �������ʼ��ʧ��
#define errCodeEsscMDL_TooManyParentTBLs	(errCodeOffset2OfEsscMDL-44)	// ̫�ุ����
#define errCodeEsscMDL_TooManyChildTBLs		(errCodeOffset2OfEsscMDL-45)	// ̫���ӱ���
#define errCodeEsscMDL_HsmDesKeySpaceSizeTooSmall	(errCodeOffset2OfEsscMDL-46)	// DES��Կ�洢�ռ�̫С
#define errCodeEsscMDL_InvalidAlgorithmID	(errCodeOffset2OfEsscMDL-47)	// �㷨��ʶ�Ƿ�
#define errCodeEsscMDL_KeyStatusDisabled	(errCodeOffset2OfEsscMDL-48)	// ��Կδ����
#define errCodeEsscMDL_KeyOutputNotPermitted	(errCodeOffset2OfEsscMDL-49)	// ��Կ��������
#define errCodeEsscMDL_KeyStatusNotActive	(errCodeOffset2OfEsscMDL-50)	// ��Կδ��Ч
#define errCodeEsscMDL_KeyInputNotPermitted	(errCodeOffset2OfEsscMDL-51)	// ��Կ��������
//add begin by lusj 20151116
#define errCodeEsscMDL_maxConnIsFull    (errCodeOffset2OfEsscMDL-52)    // sysID���������������
//add end by lusj 20151116



// ��ԿCACHEģ�����
// offset = -11000
#define errCodeKeyCacheMDL_KeyNonExists		(errCodeOffsetOfKeyCacheMDL-1)	// ��Կ������
#define errCodeKeyCacheMDL_KeyNum		(errCodeOffsetOfKeyCacheMDL-2)	// ��Կ������
#define errCodeKeyCacheMDL_KeyDBFull		(errCodeOffsetOfKeyCacheMDL-3)	// ����
#define errCodeKeyCacheMDL_KeyDBNonConnected	(errCodeOffsetOfKeyCacheMDL-4)	// ��δ����
#define errCodeKeyCacheMDL_InvalidKeyLength	(errCodeOffsetOfKeyCacheMDL-5)	// �Ƿ���Կ����
#define errCodeKeyCacheMDL_KeyAlreadyExists	(errCodeOffsetOfKeyCacheMDL-6)	// ��Կ�Ѵ���
#define errCodeKeyCacheMDL_KeyOutdate		(errCodeOffsetOfKeyCacheMDL-7)	// ��Կ����
#define errCodeKeyCacheMDL_KeyValueWithoutEnd	(errCodeOffsetOfKeyCacheMDL-8)	// ��Կֵ�޽�����
#define errCodeKeyCacheMDL_WrongKeyName		(errCodeOffsetOfKeyCacheMDL-9)	// ��Կ���Ʋ���

// offset = -33000
// һ�������������Ĵ���
#define errCodeBankBasicPackageMDL_InvalidDDU				(errCodeOffsetOfBankBasicPackageMDL-1)	// �Ƿ���DDU
#define errCodeBankBasicPackageMDL_LenOfValueLargerThanMaxLenDefined	(errCodeOffsetOfBankBasicPackageMDL-2)	// ֵ�ĳ��ȴ��ڶ����ֵ
#define errCodeBankBasicPackageMDL_DDULength				(errCodeOffsetOfBankBasicPackageMDL-3)	// DDU�ĳ��ȴ�
#define errCodeBankBasicPackageMDL_DDUValueIsNull			(errCodeOffsetOfBankBasicPackageMDL-4)	// DDU��ֵ�ǿ�
#define errCodeBankBasicPackageMDL_LenOfValueLargerThanStr		(errCodeOffsetOfBankBasicPackageMDL-5)	// ֵ�ĳ��ȱȴ��ĳ��ȴ�
#define errCodeBankBasicPackageMDL_FldOfNameNotDefinedInSpecForm	(errCodeOffsetOfBankBasicPackageMDL-6)	// �ڱ����δ�������
#define errCodeBankBasicPackageMDL_InvalidFormPointer			(errCodeOffsetOfBankBasicPackageMDL-7)	// �Ƿ��ı��ָ��
#define errCodeBankBasicPackageMDL_FormIsFull				(errCodeOffsetOfBankBasicPackageMDL-8)	// ���Ϊ��
#define errCodeBankBasicPackageMDL_FormInvalid				(errCodeOffsetOfBankBasicPackageMDL-9)	// ���Ƿ�
#define errCodeBankBasicPackageMDL_CreateFormRec			(errCodeOffsetOfBankBasicPackageMDL-10)	// ��������¼����
#define errCodeBankBasicPackageMDL_FormCurrentRecNotInitialized		(errCodeOffsetOfBankBasicPackageMDL-11)	// ���ĵ�ǰ��¼δ��ʼ��
#define errCodeBankBasicPackageMDL_LocalAndRemoteFormNameNotSame	(errCodeOffsetOfBankBasicPackageMDL-12)	// ���غ�Զ�˵ı��������ͬ
#define errCodeBankBasicPackageMDL_LocalAndRemoteFldNameNotSame		(errCodeOffsetOfBankBasicPackageMDL-13)	// ���غ�Զ�˵���������ͬ
#define errCodeBankBasicPackageMDL_DDUOfNameNotFoundInTransData		(errCodeOffsetOfBankBasicPackageMDL-14)	// ����������û�и����Ƶ�DDU
#define errCodeBankBasicPackageMDL_FormOfNameNotFoundInTransData	(errCodeOffsetOfBankBasicPackageMDL-15)	// ����������û�и����Ƶı��
#define errCodeBankBasicPackageMDL_NotSupportedUnit			(errCodeOffsetOfBankBasicPackageMDL-16)	// ��֧�ֵĵ�Ԫ
#define errCodeBankBasicPackageMDL_ConnectBankBasicPackageMDL		(errCodeOffsetOfBankBasicPackageMDL-17)	// ����BankBasic����ģ�����
#define errCodeBankBasicPackageMDL_BankBasicPackageNotConnected		(errCodeOffsetOfBankBasicPackageMDL-18)	// BankBasic����ģ��δ����

// Mary add begin, 20080717
// offset = -32000
// ���ݿ�Ĵ�����
#define errCodeDatabaseMDL_DatabaseAlreadyExist		(errCodeOffsetOfDatabase-1)	// ���ݿ��Ѿ�����
#define errCodeDatabaseMDL_DatabaseNotFound		(errCodeOffsetOfDatabase-2)	// ���ݿ�û���ҵ�
#define errCodeDatabaseMDL_TableAlreadyExist		(errCodeOffsetOfDatabase-3)	// ���Ѿ�����
#define errCodeDatabaseMDL_TableNotFound		(errCodeOffsetOfDatabase-4)	// ��û���ҵ�
#define errCodeDatabaseMDL_RecordNotFound		(errCodeOffsetOfDatabase-5)	// ��¼û���ҵ�
#define errCodeDatabaseMDL_RecordAlreadyExist		(errCodeOffsetOfDatabase-6)	// ��¼�Ѿ�����
#define errCodeDatabaseMDL_NoData			(errCodeOffsetOfDatabase-7)	// û������
// Mary add end, 20080717
#define errCodeDatabaseMDL_MoreRecordFound		(errCodeOffsetOfDatabase-8)     // �ҵ�̫������		20090609���¼�÷�ӣ�������Ҫ��
#define errCodeDatabaseMDL_RecordUpdateFailure		(errCodeOffsetOfDatabase-9)	// ��������ʧ��
#define errCodeDatabaseMDL_MoreTable			(errCodeOffsetOfDatabase-10)	// ��̫��
#define errCodeDatabaseMDL_ReconnectDataBase		(errCodeOffsetOfDatabase-11)	// �������ݿ�

// xusj add begin, 20090323
// offset = -61000
// �������ļ��Ĵ�����
#define errCodeBinaryFileMDL_ConditionErr		(errCodeOffsetOfBinaryFileDB-4)	// ��ѯ������ʽ����
#define errCodeBinaryFileMDL_RecordNotFound		(errCodeOffsetOfBinaryFileDB-5)	// ��¼û���ҵ�
#define errCodeBinaryFileMDL_RecordAlreadyExist		(errCodeOffsetOfBinaryFileDB-6)	// ��¼�Ѿ�����
#define errCodeBinaryFileMDL_TableDefErr		(errCodeOffsetOfBinaryFileDB-7)	// ��ṹ�������
// xusj add end, 20090323

// xusj add begin, 20090823
// offset = -62000
// ������Կ����Ĵ�����
#define errCodeOnlineKeyMngMDL_authCodeErr		(errCodeOffsetOfOnlineKeyMng-1)	// ��Ȩ�����
#define errCodeOnlineKeyMngMDL_authCodeOverTime		(errCodeOffsetOfOnlineKeyMng-2)	// ��Ȩ���ѹ���
#define errCodeOnlineKeyMngMDL_authCodeOverUseTimes	(errCodeOffsetOfOnlineKeyMng-3)	// ��Ȩ�볬��ʹ�ô���
#define errCodeOnlineKeyMngMDL_secuControlInactive	(errCodeOffsetOfOnlineKeyMng-4)	// ��ȫ�ؼ�δ����
#define errCodeOnlineKeyMngMDL_secuControlOverTime	(errCodeOffsetOfOnlineKeyMng-5)	// ��ȫ�ؼ��ѹ���
#define errCodeOnlineKeyMngMDL_secuControlOverUseTimes	(errCodeOffsetOfOnlineKeyMng-6)	// ��ȫ�ؼ�����ʹ�ô���
#define errCodeOnlineKeyMngMDL_generateKey		(errCodeOffsetOfOnlineKeyMng-7)	// ������Կʧ��
#define errCodeOnlineKeyMngMDL_storeKey			(errCodeOffsetOfOnlineKeyMng-8)	// ������Կ����Կ��ʧ��
#define errCodeOnlineKeyMngMDL_overMaxNumOfKey		(errCodeOffsetOfOnlineKeyMng-9)	// ������Կ�������ֵ
#define errCodeOnlineKeyMngMDL_GetKeyModel		(errCodeOffsetOfOnlineKeyMng-10)	// ��ȡ�ն�����Կ����Կģ��ʧ��
#define errCodeOnlineKeyMngMDL_GetMyKmcID		(errCodeOffsetOfOnlineKeyMng-11)	// ��ȡ��KMC��IDʧ��
#define errCodeOnlineKeyMngMDL_CallSecutrlOfNode	(errCodeOffsetOfOnlineKeyMng-12)	// ��ڵ��ϵİ�ȫ�ؼ�ͨѶʧ��
#define errCodeOnlineKeyMngMDL_NotFoundPKFile           (errCodeOffsetOfOnlineKeyMng-13)        // �ؼ�û���ҵ�KMC��PK�ļ�
#define errCodeOnlineKeyMngMDL_veryfySignFail           (errCodeOffsetOfOnlineKeyMng-14)        // ��֤ǩ��ʧ��
#define errCodeOnlineKeyMngMDL_GetSignFail              (errCodeOffsetOfOnlineKeyMng-15)        // ��ȡǩ��ʧ��
#define errCodeOnlineKeyMngMDL_keyInOthSysIsUsing       (errCodeOffsetOfOnlineKeyMng-16)        // ��Կ��������ʹ��
#define errCodeOnlineKeyMngMDL_hsmKeyPosIsOccupied         (errCodeOffsetOfOnlineKeyMng-17)        // �������Կ�����ѱ�ռ��
// xusj add end, 20090823

// ֤��ģ�������
#define errCodeOffsetOfCertificate_InvalidDate		(errCodeOffsetOfCertificate-1)	// �Ƿ����ڸ�ʽ
#define errCodeOffsetOfCertificate_DateTooBig		(errCodeOffsetOfCertificate-2)	// ����̫��
#define errCodeOffsetOfCertificate_InvalidRID		(errCodeOffsetOfCertificate-3)	// �Ƿ���RID
#define errCodeOffsetOfCertificate_InvalidServiceID	(errCodeOffsetOfCertificate-4)	// �Ƿ��ķ����ʶ
#define errCodeOffsetOfCertificate_InvalidRecordHead	(errCodeOffsetOfCertificate-5)	// �Ƿ��ļ�¼ͷ
#define errCodeOffsetOfCertificate_HashValueErr		(errCodeOffsetOfCertificate-6)	// ��ϣֵ����
#define errCodeOffsetOfCertificate_InvalidRecordTail	(errCodeOffsetOfCertificate-7)	// �Ƿ��ļ�¼β

#define errCodeOffsetOfCertMDL_FileNotExist		(errCodeOffsetOfCertMDL-1)	//�ļ�������
#define errCodeOffsetOfCertMDL_ReadFile			(errCodeOffsetOfCertMDL-2)	//���ļ�ʧ��
#define errCodeOffsetOfCertMDL_WriteFile		(errCodeOffsetOfCertMDL-3)	//д�ļ�ʧ��
#define errCodeOffsetOfCertMDL_FileFormat		(errCodeOffsetOfCertMDL-4)	//�ļ���ʽ��ƥ��
#define errCodeOffsetOfCertMDL_VerifyCertReq		(errCodeOffsetOfCertMDL-5)	//��֤֤������ʧ��
#define errCodeOffsetOfCertMDL_VerifyCert		(errCodeOffsetOfCertMDL-6)	//��֤֤��ʧ��
#define errCodeOffsetOfCertMDL_Passwd			(errCodeOffsetOfCertMDL-7)	//���벻��ȷ
#define errCodeOffsetOfCertMDL_Arithmetic		(errCodeOffsetOfCertMDL-8)	//�������ʧ��
#define errCodeOffsetOfCertMDL_SmallBuffer		(errCodeOffsetOfCertMDL-9)	//̫С�Ļ�����
#define errCodeOffsetOfCertMDL_CodeParameter		(errCodeOffsetOfCertMDL-10)	//������
#define errCodeOffsetOfCertMDL_Sign			(errCodeOffsetOfCertMDL-11)	//ǩ��ʧ��
#define errCodeOffsetOfCertMDL_VerifySign		(errCodeOffsetOfCertMDL-12)	//��֤ǩ��ʧ��
#define errCodeOffsetOfCertMDL_OpenSSl			(errCodeOffsetOfCertMDL-13)	//openssl���ó���
#define errCodeOffsetOfCertMDL_PKNotMatchVK		(errCodeOffsetOfCertMDL-14)	//rsa��˽Կ��ƥ��
#define errCodeOffsetOfCertMDL_AlgorithmFlag		(errCodeOffsetOfCertMDL-15)	//�㷨��ʶ������
#define errCodeOffsetOfCertMDL_CaNotExist		(errCodeOffsetOfCertMDL-16)	//��֤���¼������
#define errCodeOffsetOfCertMDL_RootCertExpire		(errCodeOffsetOfCertMDL-17)     //��֤���ѹ���Ч��
#define errCodeOffsetOfCertMDL_OutOfDate		(errCodeOffsetOfCertMDL-18)     //֤���ѹ���Ч��


// ��̬����ϵͳ������ errCodeOffsetOfOTPS
#define errCodeOffsetOfOTPS_SNIsGEN			(errCodeOffsetOfOTPS-1)   //���������Ѿ�����
#define errCodeOffsetOfOTPS_SNLenERR			(errCodeOffsetOfOTPS-2)
#define errCodeOffsetOfOTPS_exportFileOpenErr		(errCodeOffsetOfOTPS-3)
#define errCodeOffsetOfOTPS_qcodeLenIsTooLong		(errCodeOffsetOfOTPS-4)//��սֵ̫��
#define errCodeOffsetOfOTPS_tokenStatusErr		(errCodeOffsetOfOTPS-5) //����״̬����
#define errCodeOffsetOfOTPS_keyCheckValueErr		(errCodeOffsetOfOTPS-6)//��ԿУ��ֵ����
#define errCodeOffsetOfOTPS_keyBackupFileNotExist	(errCodeOffsetOfOTPS-7)//��Կ�����ļ�������
#define errCodeOffsetOfOTPS_snIsNotRegister		(errCodeOffsetOfOTPS-8)//�Ŷ�����δע��
#define errCodeOffsetOfOTPS_tokenTemplateIsNoExist	(errCodeOffsetOfOTPS-9)//�����಻����
#define errCodeOffsetOfOTPS_seedExportFileIsNoExist	(errCodeOffsetOfOTPS-10) //���ӵ����ļ�������
#define errCodeOffsetOfOTPS_tokenSeqHasNotGen		(errCodeOffsetOfOTPS-11)//������Ż�Ϊ����
#define errCodeOffsetOfOTPS_tokenSeedHasGen		(errCodeOffsetOfOTPS-12) //�����Ѿ�����
#define errCodeOffsetOfOTPS_otpErr			(errCodeOffsetOfOTPS-13)//��̬�������
#define errCodeOffsetOfOTPS_tokenERR			(errCodeOffsetOfOTPS-14) //���ƴ���
#define errCodeOffsetOfOTPS_manuCodeIsNotExist		(errCodeOffsetOfOTPS-15) //û�г��̺�
#define errCodeOffsetOfOTPS_tokenIDnotExist		(errCodeOffsetOfOTPS-16)//û���������
#define errCodeOffsetOfOTPS_tokenRecordError		(errCodeOffsetOfOTPS-17)//���Ƽ�¼����
#define errCodeOffsetOfOTPS_tokenIsLock			(errCodeOffsetOfOTPS-18)//��������
#define errCodeOffsetOfOTPS_tokenIsHangUp		(errCodeOffsetOfOTPS-19)//���ƹ���
#define errCodeOffsetOfOTPS_tokenHasNoActive		(errCodeOffsetOfOTPS-20) //����δ����
#define errCodeOffsetOfOTPS_tokenIsCancel		(errCodeOffsetOfOTPS-21) //�����ѷ�ֹ
#define errCodeOffsetOfOTPS_tokenIsOverDue		(errCodeOffsetOfOTPS-22) //�����ѹ���
#define errCodeOffsetOfOTPS_tokenKeyDeriveType		(errCodeOffsetOfOTPS-23) //���Ʊ����㷨δ����
#define errCodeOffsetOfOTPS_windowsTypeErr		(errCodeOffsetOfOTPS-24) //��������ֵ��
#define errCodeOffsetOfOTPS_seedFileNameFormatErr	(errCodeOffsetOfOTPS-25) //�����ļ�����ʽ�� otp_001001_20140122_2.data
#define errCodeOffsetOfOTPS_importSeedHaveErr		(errCodeOffsetOfOTPS-26) //��������ʱ�д�
#define errCodeOffsetOfOTPS_otpUsed			(errCodeOffsetOfOTPS-27) //��������ʱ�д�
#define errCodeOffsetOfOTPS_OTPIsOverDue              	(errCodeOffsetOfOTPS-28) //�����ѹ���
#define errCodeOffsetOfOTPS_OTPIsOverMaxErrTimes        (errCodeOffsetOfOTPS-29) //�����ѳ������������


//DPS ����׼��ϵͳ
#define errCodeOffsetOfDPS_repeat                       (errCodeOffsetOfDPS-1)   //�ƿ������ļ���¼���ظ�
#define errCodeOffsetOfDPS_notStandardized              (errCodeOffsetOfDPS-2)   //�ƿ������ļ���¼���淶
#define errCodeOffsetOfDPS_connectionException          (errCodeOffsetOfDPS-3)	//DPC��DPS�����쳣

//pboc

#define errCodeOffsetOfPBOC_iccTypeERR (errCodeOffsetOfPBOC-1) //iccType����
#define errCodeOffsetOfPBOC_encryptModeErr (errCodeOffsetOfPBOC-2) //����ģʽ�� 
#define errCodeOffsetOfPBOC_ivFormatErr (errCodeOffsetOfPBOC-3)//iv��ʽ��
#define errCodeOffsetOfPBOC_arcFormatErr (errCodeOffsetOfPBOC-4)//arc��ʽ��

#endif

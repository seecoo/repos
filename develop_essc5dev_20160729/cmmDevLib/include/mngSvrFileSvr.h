//	Wolfgang Wang, 2008/11/10

#ifndef _mngSvrFileSvr_
#define _mngSvrFileSvr_

/* ����
	�������󴮣���ȡ�ļ����õ���Ϣ
�������
	reqStr		����
	lenOfReqStr	���󴮳���
	sizeOfBuf	�ؼ��ֻ���Ĵ�С
�������
	dirName		�ļ��洢Ŀ¼,���ָ�벻Ϊ��
	tableName	����,���ָ�벻Ϊ��
	fileNameFldName	�ļ����ֶ���,���ָ�벻Ϊ��
	primaryKey	�ؼ��ִ�,���ָ�벻Ϊ��
����ֵ
	>=0	�ɹ������عؼ��ִ��ĳ���
	<0	ʧ�ܣ�������
*/
int UnionReadFileConfFromReqStr(char *reqStr,int lenOfReqStr,char *dirName,char *tableName,char *fileNameFldName,char *primaryKey,char *primaryKeyValueStr,int *fileType,int sizeOfBuf);

/* ����
	���տͻ����ϴ���һ���ļ�
�������
	handle		socket���
	reqStr		����
	lenOfReqStr	���󴮳���
	sizeOfResStr	��Ӧ������Ĵ�С
�������
	resStr		��Ӧ��
	fileRecved	�Ƿ����ļ����ؿͻ���
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionRecvFileFromClient(int handle,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

/* ����
	�ļ�������������
�������
	handle		socket���
	resID		��Դ��ʶ
	serviceID	������
	reqStr	����
	lenOfReqStr	���󴮳���
	sizeOfResStr	��Ӧ������Ĵ�С
�������
	resStr		��Ӧ��
	fileRecved	�Ƿ����ļ����յ�
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionExcuteMngSvrFileSvrService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved);

#endif

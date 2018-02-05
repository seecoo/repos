// 2008/7/21
// Wolfgang Wang

#ifndef unionWorkingDirH
#define unionWorkingDirH

// ���һ����ʱ�ļ���
void UnionGetTempFileNameForThisProc(char *fileName);

/*
����	���������Ŀ¼
	����Unix����Ŀ¼Ϊ$UNIONMAINDIR
	����Win����Ŀ¼Ϊ".\\"
�������
	��
�������
	workingDir	��ȡ��������Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetMainWorkingDir(char *workingDir);

/*
����	��ñ��¼�ӿڶ���Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/tblQuery"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetTBLRecInterfaceDir(char *workingDir);

/*
����	���tcpipSvr�����ļ�Ŀ¼
	Ŀ¼Ϊ"mainWorkingDir/server"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetTcpipSvrConfDir(char *workingDir);

/*
����	�����ʱ����Ŀ¼
	Ŀ¼Ϊ"mainWorkingDir/tmp"
�������
	��
�������
	workingDir	��ȡ����ʱ����Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetTempWorkingDir(char *workingDir);

/*
����	��÷���������ݹ���Ŀ¼
	Ŀ¼Ϊ"mainWorkingDir/simuData"
�������
	��
�������
	workingDir	��ȡ�Ĺ���Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetSimuDataWorkingDir(char *workingDir);

/*
����	��ÿ��ƶ���Ŀ¼
	Ŀ¼Ϊ"mainWorkingDir/control"
�������
	��
�������
	workingDir	��ȡ�Ŀ��ƶ���Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetControlDefWorkingDir(char *workingDir);

/*
����	�����ʱ�ļ�����
	���ļ�Ϊ"mainWorkingDir/tmp/x.tmp"
�������
        index           ��ʱ�ļ����
�������
	UnionGetIconDir	��ȡ���ļ�����
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetTmpFileName(unsigned int index,char *fileName);

/*
����	��ÿͻ��˱���Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/tblConf"
�������
	��
�������
	workingDir	��ȡ�Ŀͻ��˱��幤��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetTBLQueryConfDir(char *workingDir);

/*
����	��ò˵�����Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/menu"
�������
	��
�������
	workingDir	��ȡ�Ĳ˵�����Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetMenuDefDir(char *workingDir);

/*
����	���ͼ��Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/icon"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetIconDir(char *workingDir);

/*
����	���ͼ���ļ�����
	��Ŀ¼Ϊ"mainWorkingDir/icon"
�������
	��
�������
	UnionGetIconDir	��ȡ���ļ�����
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetIconFileName(char *iconName,char *fileName);

// ����������ʱ�ļ���
void UnionSetMngSvrTempFileName(char *fileName);

/*
����	�����ֵ����Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/enumValueDef"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetEnumValueDefDir(char *workingDir);

/*
����	�����ֵ����Ŀ¼
	��Ŀ¼Ϊ"mainWorkingDir/enumValueDef"
�������
	��
�������
	workingDir	��ȡ��Ŀ¼
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionGetEnumValueDefFileName(char *enumType,char *fileName);

#endif

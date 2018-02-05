//---------------------------------------------------------------------------

#ifndef unionRecFileH
#define unionRecFileH
//---------------------------------------------------------------------------

#include <stdio.h>

typedef FILE	*TUnionRecFileHDL;

/*
����	��һ����д�뵽�ļ�
�������
	fileName	��¼�ļ���
	fmt		��
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionWriteToFile(char *fileName,char *fmt,...);

//----------------------------------------------------------------------------
/*
����	������Ŀ¼���ϲ���һ��Ŀ¼��
�������
	firstDir	��һ��Ŀ¼
	secondDir	�ڶ���Ŀ¼
�������
	newDirName  	��Ŀ¼��
����ֵ
	>=0		Ŀ¼����
	<0		�������
*/
int UnionFormDirName(char *firstDir,char *secondDir,char *newDirName);

//----------------------------------------------------------------------------
/*
����	����һ��Ŀ¼׷���ӵ��ڶ���Ŀ¼��
�������
	firstDir	��һ��Ŀ¼
	secondDir	�ڶ���Ŀ¼
�������
	secondDir  	�ڶ���Ŀ¼
����ֵ
	>=0		Ŀ¼����
	<0		�������
*/
int UnionAppendFirstDirToSecondDir(char *firstDir,char *secondDir);

//---------------------------------------------------------------------------
/*
����	����һ���ļ�׷�ӵ��ڶ����ļ���
�������
	oriFileName	Դ�ļ�����
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionAppend2ndFileTo1stFile(char *oriFileName,char *desFileName);

/*
����	��һ����׷�ӵ��ļ�
�������
	fileName	��¼�ļ���
	fmt		��
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionAppendToFile(char *fileName,char *fmt,...);

//---------------------------------------------------------------------------
// 2010-12-13������������
/*
����	�Ե�һ���ļ�Ϊ��׼���Ƚ������ļ����������һ�µ���
�������
	firstFileDir	��һ���ļ���Ŀ¼
	firstFileName	��һ���ļ�����
	secondFileName	�ڶ����ļ�����
	unsameFileFp	��һ�µ�����
�������
	��
����ֵ
	>=0		��һ�µ�����
	<0		�������
*/
int UnionCompareFile_1stFileWithDir(char *firstFileDir,char *firstFileName,char *secondFileName,FILE *unsameFileFp);

//---------------------------------------------------------------------------
// 2010-12-13������������
/*
����	�Ե�һ���ļ�Ϊ��׼���Ƚ������ļ����������һ�µ���
�������
	firstFileName	��һ���ļ�����
	secondFileDir	�ڶ����ļ���Ŀ¼
	secondFileName	�ڶ����ļ�����
	unsameFileFp	��һ�µ�����
�������
	��
����ֵ
	>=0		��һ�µ�����
	<0		�������
*/
int UnionCompareFile_2ndFileWithDir(char *firstFileName,char *secondFileDir,char *secondFileName,FILE *unsameFileFp);

//---------------------------------------------------------------------------
// 2010-12-13������������
/*
����	�Ե�һ���ļ�Ϊ��׼���Ƚ������ļ����������һ�µ���
�������
	firstFileDir	��һ���ļ���Ŀ¼
	firstFileName	��һ���ļ�����
	secondFileDir	�ڶ����ļ���Ŀ¼
	secondFileName	�ڶ����ļ�����
	unsameFileFp	��һ�µ�����
�������
	��
����ֵ
	>=0		��һ�µ�����
	<0		�������
*/
int UnionCompareFile_BothFileWithDir(char *firstFileDir,char *firstFileName,char *secondFileDir,char *secondFileName,FILE *unsameFileFp);

//---------------------------------------------------------------------------
// 2010-12-13������������
/*
����	�Ե�һ���ļ�Ϊ��׼���Ƚ������ļ����������һ�µ���
�������
	firstFileName	��һ���ļ�����
	secondFileName	�ڶ����ļ�����
	unsameFileFp	��һ�µ�����
�������
	��
����ֵ
	>=0		��һ�µ�����
	<0		�������
*/
int UnionCompareFile(char *firstFileName,char *secondFileName,FILE *unsameFileFp);

//---------------------------------------------------------------------------
/*
����	����һ���ļ�
�������
	oriFileName	�ļ�����
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionCreateFile(char *oriFileName);

//---------------------------------------------------------------------------
/*
����	����һ���ļ�
�������
	oriFileName	Դ�ļ�����
	desDir		Ŀ��Ŀ¼
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionCopyFileToSpecDir(char *oriFileName,char *desDir,char *desFileName);

/*
����	����Ŀ¼
�������
	fullFileName	�ļ�ȫ����������·��
�������
	��
����ֵ
	>=0		������Ŀ¼��
	<0		�������
*/
int UnionCreateDirFromFullFileName(char *fullFileName);

/*
����	����һ���ļ�
�������
	oriFileName	Դ�ļ�����
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionBackupFileAutomatically(char *oriFileName);

//---------------------------------------------------------------------------
/* 
����	�ر���¼�ļ����
�������
	hdl	�ļ����
�������
	��
����ֵ
	��
*/
void UnionCloseRecFileHDLOfFileName(TUnionRecFileHDL hdl,char *fileName);
//---------------------------------------------------------------------------

/* 
����	�ر���¼�ļ����
�������
	hdl	�ļ����
�������
	��
����ֵ
	��
*/
void UnionCloseRecFileHDLOfFileNameWithoutLock(TUnionRecFileHDL hdl,char *fileName);

/*
���ܣ�  ɾ��Ŀ¼�е��ļ�
������
        dir[IN]         ��Ҫ��ɾ����Ŀ¼��
return:
        -1              ɾ��ʧ��
        0               ɾ���ɹ�
*/
int UnionDelDirFiles(char *dir);
//---------------------------------------------------------------------------
/*
����	����һ���ļ�����ʱĿ¼
�������
	oriFileName	Դ�ļ�����
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionBackupFileToTmpDir(char *oriFileName);

/*
����	����ʱĿ¼�ָ��ļ�
�������
	oriFileName	Դ�ļ�����
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionRestoreFileFromTmpDir(char *oriFileName);

/*
����	����һ���ļ�
�������
	oriFileName	Դ�ļ�����
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionCopyFile(char *oriFileName,char *desFileName);

/*
����	������һ���ļ�
�������
	oriFileName	Դ�ļ�����
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		�ļ��Ĵ�С
	<0		�������
*/
int UnionRenameFile(char *oriFileName,char *desFileName);

//---------------------------------------------------------------------------
/*
����	�Ƚ�������¼�����Ƿ�һ��
�������
	recStr1		��¼��1
	lenOfRecStr1	��¼���ĳ���
	recStr2		��¼��2
	lenOfRecStr2	��¼���ĳ���
	fldName		Ҫ�Ƚϵ�����
�������
	��
����ֵ
	=0		��ͬ
	-1		��һ�����ȵڶ�����С
	=1		��һ�����ȵ���������
	<0		������루��-1)
*/
int UnionCompare2RecStr(char *recStr1,int lenOfRecStr1,char *recStr2,int lenOfRecStr2,char *fldName);

//---------------------------------------------------------------------------
/*
����	��һ����¼�ļ���ɾ��һ����¼��
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
	fldName		Ҫɾ������
�������
	��
����ֵ
	>=0		ɾ���ļ�¼��
	<0		�������
*/
int UnionDeleteRecStrFromFileByFld(char *fileName,char *recStr,int lenOfRecStr,char *fldName);

//---------------------------------------------------------------------------
/*
����	������һ����¼�ļ�
�������
	oriFileName	Դ�ļ�����
	desFileName	Ŀ���ļ���
�������
	��
����ֵ
	>=0		��¼��
	<0		�������
*/
int UnionRenameRecFile(char *oriFileName,char *desFileName);

//---------------------------------------------------------------------------
/*
����	��������ķ�ʽ��һ����¼�ļ��в���һ����¼��
����    ��¼�ļ��������һ����������ʽ���е��ļ�
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
	fldName		���ݸ���������
	isAscOrder	����ʽ�������򣬣�����
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionInsertRecStrToFileInOrderByFld(char *fileName,char *recStr,int lenOfRecStr,char *fldName,int isAscOrder);

/*
����	��һ����¼�ļ��ж�ȡһ����¼��
�������
	fileName	��¼�ļ���
	sizeOfRecStr	���ռ�¼���Ļ���Ĵ�С
�������
	recStr		��ȡ�ļ�¼��
����ֵ
	>=0		��¼���ĳ���
	<0		�������
*/
int UnionReadRecStrFromFile(char *fileName,char *recStr,int sizeOfRecStr);

/*
����	����ļ������м�¼��ָ������
�������
	fileName	�ļ�����
	fldName		����
	maxNum		�������������
�������
	��
����ֵ
	>=0		������������
	<0		�������
*/
int UnionReadAllRecFldInFile(char *fileName,char *fldName,char fldValue[][128+1],int maxNum);

/*
����	��һ����¼��׷�ӵ���¼�ļ�
�������
	fileName	��¼�ļ���
	recStr		��¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
#ifndef _WIN32_
int UnionAppendRecStrToFile(char *fileName,char *recStr,int lenOfRecStr);
#else
int UnionAppendRecStrToFile(char *fileName,char *recStr, int lenOfRecStr = 0);
#endif

/*
����	��һ�������Ƽ�¼��׷�ӵ���¼�ļ�
�������
	fileName	��¼�ļ���
	recStr		��¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionAppendBinaryRecStrToFile(char *fileName,char *recStr,int lenOfRecStr);

/*
����	��һ����¼��д�뵽��¼�ļ�
�������
	fileName	��¼�ļ���
	recStr		��¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionWriteRecStrIntoFile(char *fileName,char *recStr,int lenOfRecStr);

// Mary add begin, 2008-10-27
/*
����	��һ����¼��׷�ӵ���¼�ļ���ʹ���ļ�ָ��
�������
	hdl		�ļ�ָ��
	recStr		��¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		��ȷ
	<0		�������
*/
int UnionAppendRecStrToFileUseFileHDL(TUnionRecFileHDL hdl,char *recStr,int lenOfRecStr);

/* 
����	Ϊд�ļ��򿪼�¼�ļ����
�������
	fileName	�ļ�����
�������
	��
����ֵ
	�ļ����
	NULL		����
*/
TUnionRecFileHDL UnionOpenRecFileHDLForWrite(char *fileName);
// Mary add end, 2008-10-27

/* 
����	Ϊд�ļ��򿪼�¼�ļ����
�������
	fileName	�ļ�����
�������
	��
����ֵ
	�ļ����
	NULL		����
*/
TUnionRecFileHDL UnionOpenRecFileHDLWithoutLock(char *fileName);

/* 
����	�򿪼�¼�ļ����
�������
	fileName	�ļ�����
�������
	��
����ֵ
	�ļ����
	NULL		����
*/
TUnionRecFileHDL UnionOpenRecFileHDL(char *fileName);

/* 
����	�ر���¼�ļ����
�������
	hdl	�ļ����
�������
	��
����ֵ
	��
*/
void UnionCloseRecFileHDL(TUnionRecFileHDL hdl);

/* 
����	����ļ��еļ�¼������
�������
	fileName	�ļ�����
�������
	��
����ֵ
	>=0		�ļ��еļ�¼������
	<0		�������
*/
int UnionGetTotalRecNumInFile(char *fileName);

/* 
����	���ļ��ж�ȡ��һ����¼
�������
	hdl		�ļ�ָ��
	sizeOfRecStr	���ռ�¼�Ļ���Ĵ�С
�������
	recStr		��ȡ�ļ�¼
����ֵ
	>=0		��ȡ�ļ�¼��С,=0,��ʾ�ļ������޺�����¼
	<0		�������
*/
int UnionReadNextRecFromFile(TUnionRecFileHDL hdl,char *recStr,int sizeOfRecStr);

/* 
����	���ļ��ж�ȡָ�������ļ�¼
�������
	hdl		�ļ�ָ��
	index		��¼������
	sizeOfRecStr	���ռ�¼�Ļ���Ĵ�С
�������
	recStr		��ȡ�ļ�¼
����ֵ
	>=0		��ȡ�ļ�¼��С,=0,��ʾ�ļ������޺�����¼
	<0		�������
*/
int UnionReadRecOfIndexFromFile(TUnionRecFileHDL hdl,unsigned int index,char *recStr,int sizeOfRecStr);

//---------------------------------------------------------------------------
/* 
����	���ļ��ж�ȡָ�������ļ�¼
�������
	fileName	�ļ�����
	index		��¼������
	sizeOfRecStr	���ռ�¼�Ļ���Ĵ�С
�������
	recStr		��ȡ�ļ�¼
����ֵ
	>=0		��ȡ�ļ�¼��С,=0,��ʾ�ļ������޺�����¼
	<0		�������
*/
int UnionReadRecOfIndexDirectlyFromFile(char *fileName,unsigned int index,char *recStr,int sizeOfRecStr);

//---------------------------------------------------------------------------
/*
����	��һ����¼�ļ���ɾ��һ����¼��
�������
	fileName	��¼�ļ���
	recStr		��ȡ�ļ�¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		ɾ���ļ�¼��
	<0		�������
*/
int UnionDeleteRecStrFromFile(char *fileName,char *recStr,int lenOfRecStr);

// �ж��Ƿ���ڼ�¼�ļ�
int UnionExistsFile(char *fileName);

// ɾ���ļ�
int UnionDeleteRecFile(char *fileName);
// ɾ���ļ�
int UnionDeleteFile(char *fileName);

// ����һ��Ŀ¼
int UnionCreateDir(char *dir);

// ɾ��һ��Ŀ¼
int UnionRemoveDir(char *dir);

// ���һ��Ŀ¼�Ƿ����
// 1���ڣ�0������
int UnionExistsDir(char *dir);
//---------------------------------------------------------------------------
// add by chenliang, 2008-10-10
// ��ȡһ���ļ��Ĵ�С
long UnionFileSize(char *fileName);

/*
  ����: �ж�һ���ļ��Ƿ���ڣ�������������
  ����: fileName - �ļ���; dirName - �ļ�·��
  ���: ��
  ����: =0 - �ɹ�
        <0 - ʧ��
*/
int UnionExistAndCreateFile(char *fileName, char *dirName);

/*
  ����: �ж�һ��Ŀ¼�Ƿ���ڣ�������������,
        ���·���е�ĳ��Ŀ¼������һ����
  ����: dir - Ŀ¼·��
  ���: ��
  ����: =0 - �ɹ�
        <0 - ʧ��
*/
int UnionExistAndCreateDir(char *dir);

/*
  ����: �ж�һ��Ŀ¼�Ƿ���ڣ�������������,
        ���·���е�ĳ��Ŀ¼������һ����
  ����: fullFileName - �ļ�ȫ��(��·��)
  ���: ��
  ����: =0 - �ɹ�
        <0 - ʧ��
*/
int UnionExistAndCreateDirFromFullFileName(char *fullFileName);


/*
����	���ս���ķ�ʽ��һ����¼�ļ��в���һ����¼��
����    ��¼�ļ��������һ�����ս���ʽ���е��ļ�
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionInsertRecStrToFileInDesOrder(char *fileName,char *recStr,int lenOfRecStr);

/*
����	��������ķ�ʽ��һ����¼�ļ��в���һ����¼��
����    ��¼�ļ��������һ����������ʽ���е��ļ�
�������
	fileName	��¼�ļ���
	recStr		Ҫɾ���ļ�¼��
	lenOfRecStr	��¼���ĳ���
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionInsertRecStrToFileInAscOrder(char *fileName,char *recStr,int lenOfRecStr);

/*
����    ���������һ���ļ������ݽ�������
�������
	fileName	��¼�ļ���
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionSortFileContentInAscOrder(char *fileName);

/*
����    ���ս����һ���ļ������ݽ�������
�������
	fileName	��¼�ļ���
�������
	��
����ֵ
	>=0		����ɹ�
	<0		�������
*/
int UnionSortFileContentInDesOrder(char *fileName);

int UnionFormFullFileName(char *dirName,char *fileName,char *fullFileName);

// add end.
//---------------------------------------------------------------------------
#endif

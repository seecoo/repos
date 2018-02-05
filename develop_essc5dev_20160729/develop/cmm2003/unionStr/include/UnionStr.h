// Wolfgang Wang
// 2002/08/26

#ifndef _UnionString_
#define _UnionString_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define conWordAsStr		1
#define conWordAsSeperator	2
#define conWordAsName		3
#define conWordAsCalculateChar	4

#define conStrValueFormatBCD			128	// ʮ�����Ʊ�ʾ���ַ���
#define	conIntValueFormatShiJinZhi		1	// ʮ����
#define	conIntValueFormatXiaoXieShiLiuZhi	2	// Сдʮ������
#define	conIntValueFormatDaXieShiLiuZhi		3	// ��дʮ������

#define conStrCellAlignLeft			0	// �����
#define	conStrCellAlignMiddle			1	// λ���м�
#define	conStrCellAlignRight			2	// �Ҷ���

#define conTestCaseTypeNormal			1	// �������԰���
#define conTestCaseTypeAbnormal			0	// ������԰���

// 2011-1-25,����������
/* ��Ŀ¼�ж�ȡһ���ļ������汾����׺
�������
	dir		�ļ�ȫ��
	lenOfDir	�ļ�ȫ���ĳ���
�������
	nullFileName	�������ļ���
	version		�汾
	suffix		��׺
����ֵ��
	>=0 	�ɹ�
	<0	�������

*/
int UnionReadSrcFileInfoFromFullDir(char *dir,int lenOfDir,char *nullFileName,char *version,char *suffix);

// 2011/1/1,����������
/* ��һ���ļ�ȫ�����۷ֳ�Ŀ¼���ļ���
�������
	dir		�ļ�ȫ��
	lenOfDir	�ļ�ȫ���ĳ���
�������
	onlyDir		��ֳ���Ŀ¼
	fileName	�������ļ���
����ֵ��
	>=0 	�ɹ�
	<0	�������

*/
int UnionSeperateDir(char *dir,int lenOfDir,char *onlyDir,char *fileName);

// 2010/12/10,������
/*
����	�ж��Ƿ�ͬһ�ļ�
�������
	firstFileName	��һ���ļ�����
	secondFileName	�ڶ����ļ�����
�������
        ��
����ֵ
	1		ͬһ�ļ�
	0		��ͬ�ļ�
*/
int UnionIsSameFileName(char *firstFileName,char *secondFileName);

// 2010/12/10,������
/*
����	ִ��һ���ļ���������������ָ���ļ���
�������
	dir		�ļ�Ŀ¼
	excuteFileName	��ִ���ļ�����
	outputFileName	����ļ�����
	overwrite	�������ļ��Ѵ��ڣ��Ƿ񸲸�����1�����ǣ�0��������
�������
        ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionExecuteSpecFile(char *dir,char *excuteFileName,char *outputFileName,int overwrite);

// 2010/12/10,������
/*
����	дһ��ִ���ļ��Ľű��ļ�
�������
	dir		�ļ�Ŀ¼
	excuteFileName	��ִ���ļ�����
	scriptFileName	�ű��ļ�����
�������
        ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionGenerateScriptFileForExecuteSpecFile(char *dir,char *excuteFileName,char *scriptFileName);

// 2010/12/10,������
/*
����	��һ��unix�ļ������Ը�Ϊ��ִ��
�������
	dir		�ļ�Ŀ¼
	fileName	�ļ�����
�������
        ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionChangeUnixFileToExecutable(char *dir,char *fileName);

// 2010/12/9,������
/*
����	��һ���ַ���д���ļ���
�������
	str		�ַ���
	lenOfStr	�ַ�������
	seperator	�ָ���
	fileName	�ļ���
�������
        ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionOutputStrToSpecFile(char *str,int lenOfStr,char seperator,char *fileName);

// 2010/11/1,������
/*
����	��ӡָ���������ַ����ļ���
�������
	num		�ո�����
	specChar	ָ�����ַ�
	fp		�ļ�ָ��
�������
        ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintNumOfSpecCharToFp(int num,char specChar,FILE *fp);

/*
����	��ָ���ĸ�ʽ��ӡ�ַ������ļ���
�������
	str		�ַ���
	lenOfStr	�ַ�������
	fp		�ļ�ָ��
	formatLen	�ַ������ļ�����ռ�ĳ��ȣ����㲹�ո�
	DJFS		�ַ������뷽ʽ
			0	�����
			1	�м����
			2	�Ҷ���
�������
        ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintStrToFpInFormat(char *str,int lenOfStr,FILE *fp,int formatLen,int DJFS);

// 2010/10/30,����������
/*
����
	��һ���ļ�
�������
	fileName	�ļ�����
�������
	��
����ֵ
	�ɹ�		�Ϸ����ļ����
	ʧ��		��ָ��
*/
FILE *UnionOpenFile(char *fileName);

// 2010/10/30,����������
/*
����
	��һ���ļ�
�������
	fileName	�ļ�����
�������
	��
����ֵ
	�ɹ�		�Ϸ����ļ����
	ʧ��		��ָ��
*/
FILE *UnionOpenFileForAppend(char *fileName);

// 2010/10/30,����������
/*
����
	����һ���Ϸ����ļ���������fp��Ϊ�գ��򷵻�fp�����򷵻�stdout
�������
	fp		�ļ����
�������
	��
����ֵ
	�ɹ�		�Ϸ����ļ����
	ʧ��		��ָ��
*/
FILE *UnionGetValidFp(FILE *fp);

// 2010/10/30,����������
/*
����
	�ر�һ���ļ����
�������
	fp		�ļ����
�������
	��
����ֵ
	��
*/
void UnionCloseFp(FILE *fp);

// 2010/10/30,������
/*
����	ƴ��һ���ո�
�������
	num		�ո�����
	sizeOfStr	����С
�������
        str		��ָ��
����ֵ
	>=0		�ɹ���������
	<0		�������
*/
int UnionGenerateStrOfBlankOfNum(int num,char *str,int sizeOfStr);

// 2010/8/19,����������
/*
����
	���һ������
	�����ʽΪ
	fldName=fldOffset[,fldLen]
�������
	fldDefStr		���崮
	sizeOfFldName		���������С
	sizeOfFldAlais		����������С
�������
	fldName			����
	fldOffset		��ƫ��
	fldLen			�򳤶�
	fldAlais		�����
����ֵ
	>=0			�ɹ�
	<0			�������
*/
int UnionSeperateFldAssignmentDef(char *fldDefStr,char *fldName,int sizeOfFldName,int *fldOffset,int *fldLen,char *fldAlais,int sizeOfFldAlais);

// 2010/8/19,����������
/*
����
	����һ����ѡ���壬����Ӧ����д�뵽ָ������
�������
	fldDefStr		���崮
	lenOfFldDefStr		���崮����
	oriDataStr		ԭʼ���ݴ�
	lenOfOriDataStr		ԭʼ���ݴ�����
	sizeOfDesDataStr	Ŀ�����ݴ���С
�������
	desDataStr		Ŀ�����ݴ�
����ֵ
	>=0			Ŀ�����ݴ��ĳ���
	<0			�������
*/
int UnionFilterSpecRecFldUnderSpecDef(char *fldDefStr,int lenOfFldDefStr,char *oriDataStr,int lenOfOriDataStr,char *desDataStr,int sizeOfDesDataStr);

/*
����
	��ȡһ���·ݵ�����
�������
	month			�·�
	year			��
�������
	��
����ֵ
	>=0			ָ���·ݵ�����
	<0			ʧ��
*/
int UnionGetDaysOfSpecMonth(int month,int year);

/*
����
	����һ������
�������
	firstYear		�������
	daysSinceFirstYear	����ʼʱ��֮���ѹ�ȥ����
�������
	year			��
	month			��
	day			��
����ֵ
	>=0			�ɹ�
	<0			ʧ��
*/
int UnionAnalysisAfterDate(int firstYear,long daysSinceFirstYear,long *year,long *month,long *day);

/*
����
	�����ض�ʱ��
�������
	totalSeconds		����ʼ������ʱ�䣬�����
	firstYear		��ʼ��
	timeZoneOffset		ʱ��ƫ�ƣ��Ը�������ʱ��Ϊ׼
�������
	timeStr			����ʱ�䴮����ʽ"YYYY-MM-DD HH:MM:SS"
����ֵ
	>=0			�ɹ�
	<0			ʧ��
*/
int UnionAnalysisSpecDateTime(long totalSeconds,int firstYear,int timeZoneOffset,char *timeStr);

/*
����
	��ȡ��ǰ����ʱ��
�������
	��
�������
	timeStr			����ʱ�䴮����ʽ"YYYY-MM-DD HH:MM:SS MMM:MMM"
����ֵ
	>=0			�ɹ�
	<0			ʧ��
*/
void UnionGetCurrentDateTimeInMacroFormat(char *timeStr);

// ��ȡ��ǰ����ʱ��
void UnionGetUserSetTime(struct timeval *userTime);

// ���õ�ǰʱ��
void UnionSetUserSetTime(struct timeval *userTime);

// ���õ�ǰʱ��
void UnionReInitUserSetTime();

// ��ȡ��ǰ����ʱ��,��΢���ʾ
long UnionGetRunningTimeInMicroSeconds();

// ��ȡ����ʱ��,�Ժ����ʾ
long UnionGetRunningTimeInMilliSeconds();

// ��ȡ����ʱ��,�����ʾ
long UnionGetRunningTimeInSeconds();

// ��ȡ����ʱ��,�����ʾ
long UnionGetRunningTime();

// ��ȡ������ʱ��,��΢���ʾ
long UnionGetTotalRunningTimeInMacroSeconds();

// ��ȡ����ʱ��ƫ����,��΢���ʾ
long UnionGetRunningTimeOffsetInMacroSeconds();

// ��ȡ������ʱ��,�Ժ����ʾ
long UnionGetTotalRunningTimeInMilliSeconds();

// ��ȡ������ʱ��,�����ʾ
long UnionGetTotalRunningTimeInSeconds();

// ��ȡ������ʱ��,�����ʾ
long UnionGetTotalRunningTime();

// ��ȡ�����ʾ�ĵ�ǰʱ��
// ����ֵ>0,��ǰʱ��.<0,������
long UnionGetCurrentTimeInMilliSeconds();

// ��ȡ΢���ĵ�ǰʱ��
// ����ֵ>0,��ǰʱ��.<0,������
long UnionGetCurrentTimeInMicroSeconds();

// 2010/7/6,����������
/*
����
	��һ������ֵд�뵽�ַ�����
���������
	intValue	����ֵ
	format		д���ʽ
�������
	value		�ַ���
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPutIntTypeValueIntoStr(int intValue,int format,char *value,int sizeOfBuf);

// 2010/5/12,����������
/*
����
	����һ���������Դ�����븴�Ƶĳ��ȶ̣�����λ
���������
	oriStr		Դ��
	expectedFldLen	�ڴ����Ƶĳ���,�����ֵ<=0����ȫ������
	patchChar	��λ�ַ�
	sizeOfBuf	Ŀ�괮����Ĵ�С
�������
	desStr		Ŀ�괮
����ֵ
	>=0	�ɹ�,�������Ŀ
	<0	������
*/
int UnionCopyStrWithLeftPatch(char *oriStr,int expectedFldLen,char patchChar,char *desStr,int sizeOfBuf);

// 2010/5/12,����������
/*
����
	����һ���������Դ�����븴�Ƶĳ��ȶ̣����Ҳ�λ
���������
	oriStr		Դ��
	expectedFldLen	�ڴ����Ƶĳ���,�����ֵ<=0����ȫ������
	patchChar	��λ�ַ�
	sizeOfBuf	Ŀ�괮����Ĵ�С
�������
	desStr		Ŀ�괮
����ֵ
	>=0	�ɹ�,�������Ŀ
	<0	������
*/
int UnionCopyStrWithRightPatch(char *oriStr,int expectedFldLen,char patchChar,char *desStr,int sizeOfBuf);

// 2010/3/6
/* �����ļ����ĺ�׺
�������
	fileName	�ļ�ȫ��
	lenOfFileName	�ļ�ȫ���ĳ���
	withDot		�Ƿ������
			1���
			0�����
�������
	suffix		�����ĺ�׺
����ֵ��
	>=0 		��׺�ĳ���
	<0		�������

*/
int UnionReadSuffixOfFileName(char *fileName,int lenOfFileName,int withDot,char *suffix);

/*
��һ��SQL��where����ֳ�һ��һ������
*/
int UnionReadSQLWhereWordFromStr(char *str,int lenOfStr,char wordGrp[][128],int maxNum);

// 2008/10/13
/* ��Ŀ¼�ж�ȡһ���ļ���
�������
	dir		�ļ�ȫ��
	lenOfDir	�ļ�ȫ���ĳ���
�������
	fileName	�������ļ���
����ֵ��
	>=0 	�ļ����ĳ���
	<0	�������

*/
int UnionReadFullFileNameFromFullDir(char *dir,int lenOfDir,char *fileName);

// �ж�һ��Ŀ¼�Ƿ�����һ��Ŀ¼����Ŀ¼
/*
�������
	childDir		��Ŀ¼
	fullDir			��Ŀ¼
�������
	��
����ֵ
	1			��
	0			����
	<0			������
*/
int UnionThisDirIsFirstChildDirOfSpecDir(char *childDir,char *fullDir);

// ��һ���ļ�ȫ�����۷ֳ�Ŀ¼���ļ���
/*
�������
	fullFileName		������·�����ļ�ȫ��
	lenOfFullFileName	�ļ�ȫ���ĳ���
�������
	dir			�ļ�Ŀ¼��ȥ��������/
	fileName		�ļ���
����ֵ
	>=0			�ɹ�
	<0			ʧ��
*/
int UnionGetFileDirAndFileNameFromFullFileName(char *fullFileName,int lenOfFullFileName,char *dir,char *fileName);

// 2009/6/23������������
/*
����
	��һ������ת���ɴ�һ����ǰ׺��������
�������
	fldName		ת��ǰ����
�������
	prefixFldName	ת��������ǰ׺������
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionChargeFieldNameToDBFieldName(char *fldName,char *prefixFldName);

// 2009/5/23,������
/*
����	�ָ�һ�����е�ָ���ַ�
�������
	specChar	ָ���ַ�
	oriStr         	Դ��
        lenOfOriStr  	Դ������
        sizeOfDesStr	Ŀ�괮�����С
�������
        desStr		Ŀ�괮
����ֵ
	>=0		ƴ�õı������ĳ���
	<0		�������
*/
int UnionRestoreSpecCharInStr(char specChar,char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 2009/5/23,������
/*
����	��һ�����е�ָ���ַ�����Ϊ�����ַ�����󣬲���һ���´�
�������
	specChar	ָ���ַ�
	oriStr         	Դ��
        lenOfOriStr  	Դ������
        sizeOfDesStr	Ŀ�괮�����С
�������
        desStr		Ŀ�괮
����ֵ
	>=0		ƴ�õı������ĳ���
	<0		�������
*/
int UnionTranslateSpecCharInStr(char specChar,char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 2009/5/3������������
/*
����
	�ж�һ�������ڲ����ִ���
�������
	str		��
	lenOfStr	������
	speratorTag	��ָ���
	specFld		Ҫ��Ĵ�
�������
	��
����ֵ
	>0	�ڴ���
	==0	���ڴ���
	<0	������
*/
int UnionIsFldStrInUnionFldListStr(char *str,int lenOfStr,char speratorTag,char *specFld);

// 2009/4/30,������
/*
����	����һ��ȱʡֵ���壬��ȡȱʡֵ
�������
	��
�������
        ��
����ֵ
	1	��
	0	��
*/
int UnionConvertDefaultValue(char *defaultValueDef,char *defaultValue);

// 2009/4/29,���������ӣ��ж�һ���ַ����Ƿ�����ѧ��
int UnionIsDecimalStr(char *str);

// 2008/12/17,������
/*
����	�ж������Ƿ���ô�д
�������
	��
�������
        ��
����ֵ
	1	��
	0	��
*/
int UnionIsFldNameUseUpperCase();

// 2008/12/17,������
/*
����	����һ���������ô�д��ĸ
�������
	��
�������
        ��
����ֵ
	��
*/
void UnionSetFldNameUseUpperCase();

// 2008/12/17,������
/*
����	����һ�����������ô�д��ĸ
�������
	��
�������
        ��
����ֵ
	��
*/
void UnionSetFldNameUseNonUpperCase();

// 2008/11/24,������
/*
����	��ӡָ�������Ŀո�
�������
	num		�ո�����
	fp		�ļ�ָ��
�������
        ��
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionPrintBlankOfNumToFp(int num,FILE *fp);

// 2008/11/11,������
/*
����	��һ����ƴװ�ɿ��ѵļ�¼��
	�Ὣ�ַ����е�:
		^^ת����.
		^*ת����*
�������
	oriStr         	Դ��
        lenOfOriStr  	Դ������
        sizeOfDesStr	Ŀ�괮�����С
�������
        desStr		Ŀ�괮
����ֵ
	>=0		ƴ�õı������ĳ���
	<0		�������
*/
int UnionTranslateRecFldStrIntoStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 2008/11/11,������
/*
����	��һ����ƴװ�ɿ��ѵļ�¼��
	�Ὣ�ַ����е�:
		.ת����^^
		^ת����^*

�������
	oriStr         	Դ��
        lenOfOriStr  	Դ������
        sizeOfDesStr	Ŀ�괮�����С
�������
        desStr		Ŀ�괮
����ֵ
	>=0		ƴ�õı������ĳ���
	<0		�������
*/
int UnionTranslateStrIntoRecFldStr(char *oriStr,int lenOfOriStr,char *desStr,int sizeOfDesStr);

// 2008/11/11,������
/*
����	ƴװһ��c���Եı�������Ҫ��Դ�������е�*�Ƶ�������ǰ��
�������
	prefix         	��������ǰ׺
        oriVarName  	Դ������
�������
        desVarName	ƴ�õı�����
����ֵ
	>=0		ƴ�õı������ĳ���
	<0		�������
*/
int UnionFormCSentenceVarName(char *prefix,char *oriVarName,char *desVarName);

// 2008/10/23,������
/*
���ܣ�
	ƴװһ����ֵ��
���������
	fldGrp		Ҫѡ�������嵥��ÿ����֮����,�ָ���
	lenOfFldGrp	���嵥�ĳ���
	sizeOfRecStr	�򴮻���Ĵ�С
���������
	recStr		��
����ֵ��
	>=0���ɹ��������򴮵ĳ���
	<0��ʧ�ܣ�������
*/
int UnionGenerateNullValueRecStr(char *fldGrp,int lenOfFldGrp,char *recStr,int sizeOfRecStr);

// 2008/10/23,������
/*
���ܣ�
	�Ӽ�¼���ж���ָ����
���������
	recStr		��¼��
	lenOfRecStr	��¼������
	fldGrp		Ҫѡ�������嵥��ÿ����֮����,�ָ���
	lenOfFldGrp	���嵥�ĳ���
	sizeOfFldStr	�򴮻���Ĵ�С
���������
	fldStr		��
����ֵ��
	>=0���ɹ��������򴮵ĳ���
	<0��ʧ�ܣ�������
*/
int UnionReadFldGrpFromRecStr(char *recStr,int lenOfRecStr,char *fldGrp,int lenOfFldGrp,char *fldStr,int sizeOfFldStr);

// 2008/10/22
/* ���˵�ָ�������Ŀ¼
�������
	dir		�ļ�ȫ��
	lenOfDir	�ļ�ȫ���ĳ���
	dirNum		���˵���Ŀ¼����
�������
	desDir		Ŀ��Ŀ¼
����ֵ��
	>=0 	�ļ����ĳ���
	<0	�������

*/
int UnionFilterDirFromDirStr(char *dir,int lenOfDir,int dirNum,char *desDir);

// 2008/10/20,������
/*
�������ܣ����ָ�����
���������
	varStr��	�ַ���
	lenOfVarStr	�ַ�������
	oper		�ָ���
	maxNumOfVar	�۷ֵ��ַ������е������Ŀ
�������
	varGrp		�۷ֵ��ַ�������
���أ�
	>=0		�𴮺����
	<0		�������
*/
int UnionSeprateVarStrIntoVarGrp(char *varStr, int lenOfVarStr,char oper,char varGrp[][128],int maxNumOfVar);

// 2015/1/21,������
/*
�������ܣ����ָ����𴮣��������UnionSeprateVarStrIntoVarGrp
���������
	varStr��	�ַ���
	lenOfVarStr	�ַ�������
	oper		�ָ���
	numOfArray	�����һά
	sizeOfArray	����Ķ�ά
�������
	arrayPtr	������׵�ַ
���أ�
	>=0		�𴮺����
	<0		�������
*/
int UnionSeprateVarStrIntoArray(char *varStr, int lenOfVarStr,char oper,char *arrayPtr,int numOfArray,int sizeOfArray);

// 2015/1/21,������
/*
�������ܣ����ָ����𴮣�Դ��varStr��ı䣬
���������
	varStr��	�ַ���
	lenOfVarStr	�ַ�������
	oper		�ָ���
	maxNumOfVar	�۷ֵ��ַ������е������Ŀ
�������
	varPtr		�۷ֵ��ַ���ָ���ָ��
���أ�
	>=0		�𴮺����
	<0		�������
*/
int UnionSeprateVarStrIntoPtr(char *varStr, int lenOfVarStr,char oper,char **varPtr,int maxNumOfVar);

// 2008/10/13
/* ��Ŀ¼�ж�ȡһ���ļ���
�������
	dir		�ļ�ȫ��
	lenOfDir	�ļ�ȫ���ĳ���
�������
	fileName	�������ļ���
����ֵ��
	>=0 	�ļ����ĳ���
	<0	�������

*/
int UnionReadFileNameFromFullDir(char *dir,int lenOfDir,char *fileName);

// 2008/10/13
// �ж�һ���ַ��Ƿ�������
int UnionIsDigitChar(char ch);

// �ж�һ���ַ����Ƿ�ȫ������
int UnionIsDigitString(char *str);

// 2008/10/13
// �ж�һ���ַ��Ƿ�����ĸ
int UnionIsAlphaChar(char ch);

// 2008/10/13
/* ��һ���ַ����ж�ȡһ��C���Եı�������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
�������
	varName		����������
����ֵ��
	>=0 	�����������ַ�����ռ�ĳ���
	<0	�������

*/
int UnionReadCProgramVarNameFromStr(char *str,int lenOfStr,char *varName);

// 2008/10/13
/* ��һ���ַ����ж�ȡ����C���Եı�������
�������
	str		���崮
	lenOfStr	���崮�ĳ���
	maxNumOfVar	���ı�������
�������
	varName		����������
����ֵ��
	>=0 		�����ı������ֵ�����
	<0		�������

*/
int UnionReadAllCProgramVarNameFromStr(char *str,int lenOfStr,char varName[][128],int maxNumOfVar);

// 2008/10/8,����
// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UnionPutCharTypeRecFldIntoRecStr(char *fldName,char value,char *recStr,int sizeOfRecStr);

// 2008/10/8,����
// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UnionPutDoubleTypeRecFldIntoRecStr(char *fldName,double value,char *recStr,int sizeOfRecStr);

// 2008/10/8,����
// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UnionPutLongTypeRecFldIntoRecStr(char *fldName,long value,char *recStr,int sizeOfRecStr);

int UnionPutLongLongTypeRecFldIntoRecStr(char *fldName,long long value,char *recStr,int sizeOfRecStr);

// 2008/7/18,����
// ��һ�����������͵ļ�¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UnionPutBitsTypeRecFldIntoRecStr(char *fldName,unsigned char *value,int lenOfValue,char *recStr,int sizeOfRecStr);

// 2008/10/8,����
// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UnionPutIntTypeRecFldIntoRecStr(char *fldName,int value,char *recStr,int sizeOfRecStr);

// 2008/10/8,����
// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UnionPutStringTypeRecFldIntoRecStr(char *fldName,char *value,char *recStr,int sizeOfRecStr);

// add by chenliang, 2009-02-09
/*
����:
	��unionRecStr������ΪfldName��ֵ��ΪfldValue
����:
	������� [in]
		sizeOfRecStr	:��Ҫ�޸ĵ� unionRec ������С
		fldName		:��Ҫ�޸ĵ�����
		fldValue	:�޸ĺ����ֵ
	������� [out]
		unionRecStr	:��Ҫ�޸ĵ� unionRec
����ֵ:
	>=0	�޸ĺ� unionRecStr �ĳ���
	< 0	�������
*/
int UnionUpdateSpecFldValueOnUnionRec(char *unionRecStr, int sizeOfRecStr, char *fldName, char *fldValue);
// add end, 2009-02-09



// UnionDeleteSpecFldOnUnionRec; add by LiangJie, 2009-05-13
/*
����:
	ɾ����¼����ָ������
����:
	������� [in]
		unionRecStr	:�޸�ǰ�ļ�¼��
		lenOfRecStr	:��¼���ĳ���
		fldName		:��Ҫɾ��������
	������� [out]
		unionRecStr	:ɾ��ָ�����ļ�¼��
����ֵ:
	>=0	�޸ĺ� unionRecStr �ĳ���
	< 0	�������
*/
int UnionDeleteSpecFldOnUnionRec(char *unionRecStr, int lenOfRecStr, char *fldName);


// 2008/10/5������
/*
����	���ļ��ж�ȡһ�������У����˵�ע����
�������
	fp		�ļ�����
	sizeOfBuf	��������С
�������
	buf		����������
����ֵ
	>= 0		���������ݵĳ���
	<0		�������
*/


int UnionReadOneDataLineFromTxtFile(FILE *fp,char *buf,int sizeOfBuf);

// 2008/10/5������
/*
����	�ж�һ�������Ƿ��ǺϷ���c���Ա�������
�������
	name		����
	lenOfName	���ֳ���
�������
	��
����ֵ
	1		��
	0		����
*/
int UnionIsValidCProgramName(char *name,int lenOfName);

// 2008/10/3,����
// ���ı��ļ���ȡһ��
int UnionReadOneLineFromTxtFile(FILE *fp,char *buf,int sizeOfBuf);

// 2008/7/29������
/*
����	ʹ��ͨ�õ���ָ����滻ָ���ķָ���
�������
	oriStr		��
	lenOfOriStr	������
	oriFldSeperator	Դ�ָ���
	desFldSeperator	Ŀ��ָ���
	sizeOfDesStr	Ŀ�괮����Ĵ�С
�������
	desStr		Ŀ�괮
����ֵ
	>= 0		Ŀ�괮�ĳ���
	<0		�������
*/
int UnionConvertOneFldSeperatorInRecStrIntoAnother(char *oriStr,int lenOfOriStr,char oriFldSeperator,char desFldSeperator,char *desStr,int sizeOfDesStr);

// 2008/7/24������
/*
����	ʹ��ͨ�õ���ָ����滻ָ���ķָ���
�������
	oriStr		��
	lenOfOriStr	������
	fldSeperator	�ָ���
	sizeOfDesStr	Ŀ�괮����Ĵ�С
�������
	desStr		Ŀ�괮
����ֵ
	>= 0		Ŀ�괮�ĳ���
	<0		�������
*/
int UnionConvertSpecFldSeperatorInRecStrIntoCommonSeperator(char *oriStr,int lenOfOriStr,char fldSeperator,char *desStr,int sizeOfDesStr);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionReadIntTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int *value);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionReadLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,long *value);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionReadDoubleTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,double *value);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionReadCharTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value);

// 2008/7/18,����
// ��һ����¼�������ַ�����
// ����ֵ�Ǽ�¼���ڴ��еĳ���
int UnionPutRecFldIntoRecStr(char *fldName,char *value,int lenOfValue,char *recStr,int sizeOfRecStr);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionReadRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionReadBitsTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,unsigned char *value,int sizeOfBuf);

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionReadRecFldFromRecStrForCommon(char *recStr,int lenOfRecStr,char *fldName,char *value,int sizeOfBuf,int isBitsData);

// 2008/5/22������
/* ����ָ����IP��ַ����
�������
	ipAddr	ԭʼip��ַ
	level	Ҫ������ip��ַ�ļ���
		4������4����
		3������ǰ3����
		2������ǰ2����
		1������ǰ1����
		0��ȫ��������
�������
	outIPAddr	�ӹ����ip��ַ
			�������Ĳ�����XXX���
����ֵ
	0	�ɹ�
	��ֵ	�������
*/
int UnionDealWithIPAddr(char *ipAddr,int level,char *outIPAddr);

// 2008/5/15����
// ���һ���ַ��Ƿ������У��
int UnionIsOddParityChar(char chCheckChar);

// 2008/5/15����
// ��һ���ַ���������У��
int UnionMakeStrIntoOddParityStr(char *pData,int nLenOfData);

// 2008/5/15����
// ���һ���ַ����Ƿ������У��
// ������У�鷵��ֵΪ1�����򷵻�ֵΪ0
int UnionCheckStrIsOddStr(char *pData,int nLenOfData);

/* 2008/3/31����
���ܣ���һ���ַ����õ�ָ���ڼ������ֵ
�������:
	buf		�����ַ���
	spaceMark	�ָ���
	index		�ڼ���ֵ
���������
	fld		��index��ֵ

����ֵ��
	����ֵ�ĳ���
*/
int UnionReadFldFromBuf(char *buf,char spaceMark,int index,char *fld);

// ȥ���ַ��������пո�2008-3-19
int UnionFilterAllBlankChars(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

// 2008/3/17����
// ȥ���ַ����еĶ���ո��tab����������ո�ѹ��Ϊһ���ո�ȥ��ͷ��β�Ŀո��tab
int UnionFilterRubbisBlankAndTab(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

// �ж��Ƿ�Ϸ��ı�����
int UnionIsValidAnsiCVarName(char *varName);

// ���ı��ļ���ȡһ��
int UnionReadOneLineFromTxtStr(FILE *fp,char *buf,int sizeOfBuf);

// ��һ���ַ����ж�ȡ����
/* ���������
	str	�ַ���
   �������
   	par	�Ŷ����Ĳ���
   	maxNum	Ҫ��������������Ŀ
   ����ֵ
   	���ض�ȡ�Ĳ�����Ŀ
*/
int UnionReadParFromStr(char *str,char par[][128],int maxNum);

// �������Сд���ת��Ϊ��д���
int UnionSmallAmtToBigAmt(char *smallAmt, char *bigAmt);

// �Ӳ������ж�ȡ����
// ��������
/* ���������
	parGrp,������
	parNum��������Ŀ
	parID,��������
	sizeOfParValue,��������Ĵ�С
   �������
   	parValue,���ܲ����Ļ���
   ����ֵ��
   	<0,������
   	>=0,��������
*/
int UnionReadParFromParGrp(char *parGrp[],int parNum,char *parID,char *parValue,int sizeOfParValue);

// ������ж�ȡ����
// ����Ķ����ʽΪ:
//	-parID parValue
/* ���������
	cmdStr,���
	parID,��������
	sizeOfParValue,��������Ĵ�С
   �������
   	parValue,���ܲ����Ļ���
   ����ֵ��
   	<0,������
   	>=0,��������
*/
int UnionReadParFromCmdStr(char *cmdStr,char *parID,char *parValue,int sizeOfParValue);

// �Ӽ�¼���崮�ж�ȡ��ֵ
// ��¼���Ķ����ʽΪ:
//	fldName=fldValue;fldName=fldValue;...fldName=fldValue;
/* ���������
	recStr,��¼��
	fldName,����
	fldTypeName,������
		�Ϸ�ֵ	char/short/int/long/double/string
	sizeOfFldValue,��ֵ����Ĵ�С
   �������
   	fldValue,������ֵ�Ļ���
   ����ֵ��
   	<0,������
   	>=0,��ֵ����
*/
int UnionReadFldFromRecStr(char *recStr,char *fldName,char *fldTypeName,unsigned char *fldValue,int sizeOfFldValue);

// ����PK���ĳ���
int UnionGetPKOutOfRacalHsmCmdReturnStr(unsigned char *racalPKStr,int lenOfRacalPKStr,char *pk,int *lenOfPK,int sizeOfBuf);

int UnionFormFullDateFromMMDDDate(char *mmDDDate,char *fullDate);

// �жϴ����Ƿ���.
int UnionIsStringHasDotChar(char *str);

// ȥ���ַ��������пո�2008-3-19
int UnionFilterAllSpecChars(char *sourStr,int lenOfSourStr,char specChar,char *destStr,int sizeOfBuf);

// ȥ���ַ����еĶ���ո񣬼�������ո�ѹ��Ϊһ���ո�ȥ��ͷ��β�Ŀո�
int UnionFilterRubbisBlank(char *sourStr,int lenOfSourStr,char *destStr,int sizeOfBuf);

int UnionConvertLongStrIntoDoubleOf2TailStr(char *str);
int UnionConvertDoubleOf2TailStrIntoLongStr(char *str);

int UnionIsBCDStr(char *str1);	// Added by Wolfgang Wang, 2003/09/09
int UnionIsDigitStr(char *str);

int UnionConvertIntIntoStr(int data,int len,char *str);
int UnionConvertLongIntoStr(long data,int len,char *str);

int UnionConvertIntStringToInt(char *str,int lenOfStr);
long UnionConvertLongStringToLong(char *str,int lenOfStr);

int UnionConvertIntoLen(char *str,int intLen);

int UnionToUpper(char *lowerstr,char *upperstr);
int UnionUpperMySelf(char *str,int lenOfStr);
int UnionToUpperCase(char *str);
int UnionToLowerCase(char *str);
int UnionFilterRightBlank(char *Str);
int UnionFilterLeftBlank(char *Str);
int UnionVerifyDateStr(char *Str);
int UnionIsValidIPAddrStr(char *ipAddr);
int UnionCopyFilterHeadAndTailBlank(char *str1,char *str2);
int UnionFilterHeadAndTailBlank(char *str);
int UnionReadDirFromStr(char *str,int dirLevel,char *dir);

char hextoasc(int xxc);
char hexlowtoasc(int xxc);
char hexhightoasc(int xxc);
char asctohex(char ch1,char ch2);
int aschex_to_bcdhex(char aschex[],int len,char bcdhex[]);
int bcdhex_to_aschex(char bcdhex[],int len,char aschex[]);
int byte2int_to_bcd(int aa,char xx[]);
int IntToBcd(int aa,unsigned char xx[]);
int BcdToInt(char xx[]);

int UnionFormANSIX80LenString(int len,char *str,int sizeOfStr);
int UnionFormANSIDERRSAPK(char *pk,int lenOfPK,char *derPK,int sizeOfDERPK);
int UnionGetPKOutOfANSIDERRSAPK(char *derPK,int lenOfDERPK,char *pk,int sizeOfPK);
int UnionFilterRSASignNullChar(char *str,int len);

int UnionIsUnixShellRemarkLine(char *line);
int UnionConvertUnixShellStr(char *oldStr,int lenOfOldStr,char *newStr,int sizeOfNewStr);
int UnionReadUnixEnvVarFromStr(char *envVarStr,int lenOfEnvVarStr,char *envVar,int *envVarNameLen);
int UnionDeleteSuffixVersionFromStr(char *oldStr,char *newStr);
int UnionStrContainCharStar(char *str);

int UnionIsValidFullDateStr(char *date);

int UnionIsStringContainingOnlyZero(char *str,int lenOfStr);

// Mary add begin, 20080925
// ����3�������ɲ������д���ԣ��¼�÷������������������������
int	unionIsDigit(char c);
int	unionIsHChar(char c);
int	unionStrToInt(char	*str,int Len);
// Mary add end, 20080925

// Mary add begin, 20081007
// ����2�������ɲ������д���ԣ��¼�÷������������������������
/*
2008/10/06������������
���ܣ������Էָ����ָ���Ӵ�
�������
	srcStr����
	Seperator��Դ�ָ���
�������
	desStr���ָ��ַ���
	CntOfDesStr���ָ���Ӵ�����
����ֵ
	>=0����λ�� offset
	<0���������
*/
int UnionStrChr(char *srcStr,char Seperator,char desStr[][1024],int *CntOfDesStr);

/*
2008/10/06������������
����
	�����Էָ����ָ���Ӵ�
�������
	srcStr����
	Seperator���ָ���
�������
	desStr���ָ��ַ���
����ֵ
	>=0���ָ���Ӵ�����
	<0���������
*/
int UnionStrToK(char *srcStr,char *Seperator,char desStr[][1024]);
// Mary add end, 20081007

// 2008/7/18,����
// ���ַ����ж�ȡ��¼��
// ������ֵ�ĳ���
int UnionExistsRecFldInRecStr(char *recStr,int lenOfRecStr,char *fldName);

// Mary add begin, 20081124
/*
���ܣ�����ת����ʮ�������ַ���ת��Ϊʮ�����ַ���
���������
	hexStr��ʮ�������ַ���
	hexStrLen��ʮ�������ַ����ĳ���
	transTable��ʮ�������ַ���Ӧ�����ֱ����磺"0123456789012345"����ʾ'A-F'ת��Ϊ'0-5'
	digitStrLen����Ҫ���ص����ִ��ĳ���
���������
	digitStr��ʮ�����ַ���
	sizeOfDigitStr��digitStr�Ĵ洢�ռ��С
����ֵ��
	>=0���ɹ�������digitStr�ĳ���
	<0��ʧ��
�㷨���̣�
	1��ʮ�������ַ����д�����ȡ'0'��'9'֮������֣�˳�����һ������
	2��ʮ�������ַ����д�����ȡ'A'��'F'֮�����ĸ������ʮ��������ĸת����
	   ����ĸת�������֣���˳�����һ������
	3������1���͵�2�����������������õ�һ��ȫ���ִ������ճ���ȡֵ����
*/
int UnionTranslateHEXStringToDigitString(char *hexStr,int hexStrLen,char *transTable,int digitStrLen,char *digitStr,int sizeOfDigitStr);
// Mary add end, 20081124

// �¼�÷����������д�ĺ���, 20081124
// ���ַ������͵ĵ�ʱ��ֵת��Ϊtime_t�ͣ�datetime����Ϊ"yyyymmddhhmmss"
time_t UnionTranslateStringTimeToTime(char *datetime);

//������baseΪ���׵�n�η�
int UnionPower(int base, int n);

//ʮ�������ַ���ת��Ϊʮ��������
int UnionOxToD(char *hexStr);

// Mary add begin, 20090327, ��������������д
/*
���ܣ�		�����ֽ�����������

���������	dNumber	���������
		digit����С��λ��λ��

����ֵ��	��������������
*/

double UnionChangeDigitOfNumber(double dNumber,int digit);
// Mary add end, 20090327

// ȥ���ַ������ҿո�
int UnionRTrim(char *str);

// ��һ��mngSvr��ʽ��������ɾ��ĳ��
int UnionDeleteRecFldFromRecStr(char *str, char *fldName);

// ȡ���ݿ���ֶ�ǰ׺
char *UnionGetPrefixOfDBField();

/* 2010-3-23 wangk
����	�����ݿ���ַ����еĵ������滻Ϊ2�������ţ�����͸���ʱ�ַ������е����ŵ�ʱ������
�������
	oriStr         	Դ��
	lenOfOriStr  	Դ������
	sizeOfDesStr	Ŀ�괮�����С
�������
	desStr		Ŀ�괮
����ֵ
	>=0		ƴ�õı������ĳ���
	<0		�������
*/
int UnionTranslateSpecCharInDBStr(char *oriStr, int lenOfOriStr, char *desStr, int sizeOfDesStr);

// ��������ǲ���MMYY�ĸ�ʽ
int UnionIsValidMMYYDateStr(char *mmyyDate);

// ��λȡ��
int UnionCPL(unsigned char *pData1, int nLenOfData, unsigned char *pData2);

/*
����	дini�ļ��е�һ����
�������
	appName		�ֶ���
	keyName		����
	keyValue	��ֵ
	fileName	ini�ļ���
�������
	��
����ֵ
	>=0	��ȷ
	<0	����
*/
int UnionINIWriteString(char *appName,char *keyName,char *keyValue,char *fileName);

/*
����	��ini�ļ��е�һ���ַ�����
�������
	appName		�ֶ���
	keyName		����
	defaultValue	Ĭ��ֵ�����û�ҵ����������ֵ
	sizeOfBuf	��ֵ�������Ĵ�С
	fileName	ini�ļ���
�������
	keyValue	��ֵ	
����ֵ
	>0	��ֵ�ĳ���
	=0	δ��
	<0	����
*/
int UnionINIReadString(char *appName,char *keyName,char *defaultValue,char *keyValue,int sizeOfBuf,char *fileName);

/*
����	��ini�ļ��е�һ��������
�������
	appName		�ֶ���
	keyName		����
	defaultValue	Ĭ��ֵ�����û�ҵ����������ֵ
	fileName	ini�ļ���
�������
����ֵ
	��ֵ

*/
int UnionINIReadInt(char *appName,char *keyName,int defaultValue,char *fileName);

// ���ָ������֮ǰ��һ��ʱ��ֵagoTime����ʽΪ"yyyymmddHHMMSS"
int UnionGetFullDateTimeSecondsBeforeNow(int secondsBefore, char *agoTime);

// ��������ֵ:1~7
int UnionGetDays();

// added 2012-09-12
// �Ƿ�ʹ�ö�̬RSA���κ� 
// ���κ�:1NNNNYYDDD, NNNNΪ���κ�,YYΪ���,DDDΪһ���еĵڼ���
int UnionIsUseDynamicRsaBatchNo();

int UnionFormRsaBatchNoCondition(int inBatchNo, char *condition);


/*
 * UnionTransRsaBatchNo ����ת��������κ�
 * ʹ��ʱ��Ϊǰ׺�����κţ��Է�ֹ���̺��ظ�ʹ���Ե��¼�¼�ظ�
 * ת��������κŸ�ʽΪ:1NNNNYYDDD,
 *   ����YYΪ��ݣ�DDDΪһ���е�������NNNNΪ�����������κ�
 */
int UnionTransRsaBatchNo(int inBatchNo);

int UnionTime(int *ctime);

//add by hzh in 2012.11.8���õ�ǰrsaָ��Ϊ3, ��65537
void UnionSetCurrentRsaExponent(int exponent);
int UnionGetCurrentRsaExponent();
//add end 

char *UnionStrCaseStr(const char *haystack, const char *needle);

char *UnionStrRCaseStr(const char *haystack, const char *needle);

int UnionGetTableNameAndFieldNameFromSQL(char *sql,char *tableName,char *fieldName);

int UnionFormRecStrUnderSpecDef(char *fldDefStr,int lenOfFldDefStr,char *oriDataStr,int lenOfOriDataStr,char *desDataStr,int sizeOfDesDataStr);

int UnionReadLongLongTypeRecFldFromRecStr(char *recStr,int lenOfRecStr,char *fldName,int64_t  *value);

// ��str2����str1��ָ��posλ��,pos���±�Ϊ1
int UnionInsertString(char *str1,char *str2,int pos);

// ��ȡ�ͻ�����ʶ
char *UnionGetIDOfCustomization();

// �ж��Ƿ��ǽ������
int UnionIsUITrans(char *sysID);

// 2014-6-10 ����������
// �ж������Ƿ����
// ���룺passwdUpdateTime - ��������޸�����
// ���أ�>=0 - ��ʣ�µ�������<0 - �ѹ���
int UnionIsOverduePasswd(char *passwdUpdateTime,char *remark);

// ���µĺ�������һ������Ϊ0x500����10��������1280����cryptTable[0x500]  
void UnionHashTablePrepareCryptTable();

// ���º�������hashData �ַ�����hashֵ������dwHashType Ϊhash�����ͣ�  
// �����ȡ��ֵΪ0��1��2���ú���  
// ����hashData �ַ�����hashֵ��  
unsigned long UnionHashTableHashString( char *hashData, unsigned long dwHashType);

// ��鱨�ĸ�ʽ�������ر��ĳ���
int UnionIsCheckKMSPackage(char flag,char *package,int lenOfPackage,char *operator,char *appID,char *resID,char *serviceCode);

int UnionGetCurrentSecondTime();

// �Ƿ���ʾ������Ϣ
int UnionIsDisplaySensitiveInfo();

/*
���ܣ�	
	�Ƚ��ַ�������ͨ���
���룺	
	s,������ͨ���
	t������ͨ���
����:
	0��ʾ��ƥ�䣬
	1��ʾƥ��
*/
int UnionStrComp(char *s, char *t);

#endif

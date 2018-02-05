//	HuangBaoxin
//	2009/07/22

#ifndef _UnionDesKey_1x_3x_
#define _UnionDesKey_1x_3x_

#ifndef _UnionDesKeyDB_2_x_
#define _UnionDesKeyDB_2_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif
#ifndef _UnionDesKey_4_x_
#define _UnionDesKey_4_x_
#endif


#include "unionDesKey.h"
#include "unionDesKeyDB.h"

typedef char		TUnionDate[8+1];	// YYYYMMDD

// ������Կ����
typedef struct
{
	char				fullName[40+1];		// ��Կȫ��������owner.keyApp.keyName
	char				value[48+1];		// ��Կ����
	char				checkValue[16+1];	// ��ԿУ��ֵ
	TUnionDesKeyLength		length;			// ��Կǿ��
	//TUnionDesKeyProtectMethod	protectMethod;		// ��Կ��������
	TUnionDesKeyType		type;			// ��Կ����
	TUnionDate			activeDate;		// ��Կ��Ч����
	TUnionDate			passiveDate;		// ��ԿʧЧ����
} TUnionDesKey_1x;
typedef TUnionDesKey_1x			*PUnionDesKey_1x;

// 1.0 ��Կ�ṹ��ת��Ϊ 3.0
int UnionDesKey1xToDesKey3x(PUnionDesKey_1x pdesKey1x, PUnionDesKey_1x pdesKey1xOld, PUnionDesKey pdesKey3x);

// 3.0 ��Կ�ṹ��ת��Ϊ1.0
int UnionDesKey3xToDesKey1x(PUnionDesKey pdesKey3x, PUnionDesKey_1x pdesKey1x, PUnionDesKey_1x pdesKey1xOld);

// ����Կ�ļ���ȡһ����Կ����
int UnionReadDesKey1x(char *keyFileName, PUnionDesKey_1x pDesKey1x);

// ����1.0��Կ����Կ�ļ�
int UnionSaveDesKey1x(PUnionDesKey_1x pDesKey1x);

// ɾ��1.0��Կ����Կ�ļ�
int UnionDeleteDesKey1x(PUnionDesKey_1x pDesKey1x);

// ����3.x��Կ����Կ�ļ�
int UnionSaveDesKey3x(PUnionDesKey pdesKey3x);

// ����3.x��Կ����Կ�ļ�
// ѡ�񱣴���Կ����old��Կ
int UnionSaveDesKey3xByValueType(PUnionDesKey pdesKey3x, int valueType);

// ɾ��3.x��Կ����Կ�ļ�
int UnionDeleteDesKey3x(PUnionDesKey pdesKey3x);

/*
	���������� 1.0 ����Կ�������Կ�������ڴ�
	������OLD��Կ
*/
int UnionLoadDesKey1xIntoMemory(PUnionDesKeyDB pdesKeyDB);

/*
	���������� 1.0 ����Կ��� OLD��Կ ������Կ�������ڴ�
*/
int UnionLoadOldDesKey1xIntoMemory(PUnionDesKeyDB pdesKeyDB);

/*
	������Կ���ڵ�λ��
*/
int UnionFindDesKeyPosInKeyDB2(char *fullName, PUnionDesKeyDB pdesKeyDB);


/* ������tt������ss��������  ��ʽΪYYYYMMDD */
int GetDaysBetween2Date(char *ss, char *tt);

/* ��ĳ������ss����i��������, i�Ǹ����Ǳ�ʾss֮ǰ������  ��ʽΪYYYYMMDD */
int AddDateByDatesYMD(int i, char *ss, char *tt);


// ��ʼ����Կ�⹲���ڴ�
int UnionInitDesKeyDB1x();

#endif

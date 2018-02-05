//	zhang yong ding
//	2014/07/06
//	Version 1.0

#ifndef _UnionTableData_
#define _UnionTableData_

#define		maxNumOfTableData	1024

#if ( defined _AIX )
#define		USE_HASH_NUM		50
#else
#define		USE_HASH_NUM		10
#endif

typedef struct
{
	unsigned long		id;
	char			key[128];
	int			len;
	char			value[8192];
} TUnionTableData;
typedef TUnionTableData		*PUnionTableData;

typedef struct
{
	int			num;
	int			hashflag;
	TUnionTableData		tableData[maxNumOfTableData];
} TUnionTableDataTBL;
typedef TUnionTableDataTBL	*PUnionTableDataTBL;

int UnionIsTableDataTBLConnected(char *tableName);

int UnionGetMDLNameOfTableDataTBL(char *tableName,char *mdlName);

int UnionConnectTableDataTBL(char *tableName);

int UnionDisconnectTableDataTBL(char *tableName);

int UnionRemoveTableDataTBL(char *tableName);

int UnionReloadTableDataTBL(char *tableName);

int UnionPrintTableDataTBLToFile(char *tableName,FILE *fp);

//int UnionPrintTableDataToFile(PUnionTableDataTBL ptableDataTBL,FILE *fp);

//PUnionTableDataTBL UnionGetTableDataTBL(char *tableName);

//int UnionPrintRecOfTableDataToFile(char *tableName,FILE *fp);

char * UnionFindTableValue(char *tableName, char *primaryKeyData);

int UnionIsExistTableValue(char *tableName);

#endif

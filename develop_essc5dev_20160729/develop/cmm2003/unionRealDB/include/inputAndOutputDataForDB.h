#ifndef _InputAndOutputDataForDB_H_
#define _InputAndOutputDataForDB_H_

int UnionInputOneTableDataToDB(char *tableName,int *successNum, int *failNum);
int UnionInputOneTableDataToDB_oldVersion(char *tableName,char *prefix,int *successNum, int *failNum);

int UnionOutputOneTableDataFromDB(char *tableName);

#endif

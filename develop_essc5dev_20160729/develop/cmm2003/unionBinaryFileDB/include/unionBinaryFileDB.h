#ifndef  _BINARYFILEDB_
#define  _BINARYFILEDB_

typedef struct
{
	char	fldName[20+1];
	char	fldType[20+1];
	char	fldLen[4+1];
}TUnionTableDef;
typedef TUnionTableDef *PUnionTableDef; 

/******************************
 * 插入一记录
 * input:  tbl - 表名
 *         primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         data - 要插入的结构体指针
 *         sizeOfData - 结构体大小
 * output:
 * return: =0;成功
 *         <0;失败
******************************/
int UnionInsertIntoDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData);


/******************************
 * 删除一条记录
 * input:  tbl - 表名
 *         primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         sizeOfData - 结构体大小
 * output:
 * return: =0;成功
 *         <0;失败
******************************/
int UnionDeleteByPrimaryKey(char *tbl, char *primaryKey, size_t sizeOfData);


/******************************
 * 修改一条记录
 * input:  tbl - 表名
 *         primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         data - 结构体指针
 *         sizeOfData - 结构体大小
 * output:
 * return: =0;成功
 *         <0;失败
******************************/
int UnionUpdateIntoDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData);


/******************************
 * 查询一条记录
 * input:  tbl - 表名
 *         primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         sizeOfData - 结构体大小
 * output: data - 结构体指针
 * return: =0;成功
 *         <0;失败
******************************/
int UnionSelectSpecFromDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData);


/******************************
 * 批量查询记录
 * input:  tbl - 表名
 *         condition - 条件，格式："字段1=值|...|字段n=值|"
 *         sizeOfData - 结构体大小
 *         outFile - 结果文件名
 * output:
 * return: >=0;成功，代表总查询的数量
 *         <0;失败
******************************/
int UnionSelectBatchFromDB(char *tbl, char *condition, size_t sizeOfData, char *outFile);

/******************************
 * 将一个结构体转换为一个字符串
 * input:  tbl - 表名
 *         data - 结构体
 * output: resStr - 字符串，格式为: "字段1=值|...|字段n=值|"
 * return: >=0;成功，代表resStr的长度
 *         <0;失败
******************************/
int UnionTransStructToString(char *tbl, unsigned char *data, char *resStr);


#endif


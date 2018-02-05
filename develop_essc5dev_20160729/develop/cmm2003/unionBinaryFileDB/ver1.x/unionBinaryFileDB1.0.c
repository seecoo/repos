#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "unionBinaryFileDB.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"

int UnionReadFldNameValue(char *condition, char fldName[][20], char fldValue[][256])
{
	int	i;
	char	*p,*q;
	
	p=q=condition;
	i = 0;
	while( (p=strchr(q, '=')) != NULL )
	{
		memcpy(fldName[i], q, (p-q));
		p++;
		q=p;
		q=strchr(p,'|');
		if (q==NULL)
		{
			UnionUserErrLog("UnionReadFldNameValue err!\n condition=[%s]\n",condition);
			return (errCodeBinaryFileMDL_ConditionErr);
		}
		memcpy(fldValue[i], p, q-p);
		q++;
		p=q;
		i++;
	}
	return i;
}

int UnionCompareTheCondition(char *tbl, char *condition, unsigned char *data, size_t sizeOfData)
{
	int	i;
	int	fldLen;
	int	fldNum;
	int	offset;
	int	intValue;
	char	fldName[10][20];
	char	fldValue[10][256];
	unsigned char *nullData=NULL;
	TUnionTableDef def;

	memset(fldName, 0, sizeof fldName);
	memset(fldValue, 0, sizeof fldValue);
	memset(&def, 0, sizeof def);

	nullData = (unsigned char *)malloc(sizeOfData);
	if (nullData == NULL)
	{
		UnionSystemErrLog("in UnionCompareTheCondition:: malloc err!\n");
		return errCodeUseOSErrCode;
	}
	memset(nullData, 0, sizeOfData);
	if (memcmp(data, nullData, sizeOfData) == 0)
	{
		free(nullData);
		return 0;
	}

	fldNum = UnionReadFldNameValue(condition, fldName, fldValue);
	if (fldNum < 0)
	{
		UnionUserErrLog("in UnionCompareTheCondition:: UnionReadFldNameValue err!condition=[%s]\n",condition);
		free(nullData);
		return (fldNum);
	}

	for(i=0;i<fldNum;i++)
	{
		offset = UnionReadTableFldAttr(tbl, fldName[i], &def);
		if (offset < 0)
		{
			UnionUserErrLog("in UnionCompareTheCondition:: UnionReadTableFldAttr err! fldName=[%s]\n", fldName[i]);
			free(nullData);
			return (errCodeBinaryFileMDL_TableDefErr);
		}
		fldLen = atoi(def.fldLen);
		if (strcmp(def.fldType,"int") == 0)
		{
			intValue = atoi(fldValue[i]);
			if (memcmp(data+offset, &intValue, sizeof(int)) != 0)
			{
				free(nullData);
				return 0;
			}
		}
		else
		{
			if (memcmp(data+offset, fldValue[i], fldLen) != 0)
			{
				free(nullData);
				return 0;
			}
		}
	}

	free(nullData);
	return 1;
}

/******************************
 * 取一条记录在文件中的位置(内部接口)
 * input:  tbl - 表名
 *	   primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         sizeOfData - 结构体大小
 * output: 
 * return: >=0;成功，代表索引号
 *	   <0;失败
******************************/
int UnionGetIndexByPrimaryKey(char *tbl, char *primaryKey, size_t sizeOfData)
{
	FILE	*fp;
	int	i;
	unsigned char *data;
	char	fileName[80];

	memset(fileName, 0, sizeof fileName);

	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".DB");

	fp=fopen(fileName,"rb");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionGetIndexByPrimaryKey:: fopen err! fileName=[%s]\n",fileName);
		return errCodeUseOSErrCode;
	}

	data = (unsigned char *)malloc(sizeOfData);
	if (data==NULL)
	{
		UnionSystemErrLog("in UnionGetIndexByPrimaryKey:: malloc [%d] err!\n", sizeOfData);
		fclose(fp);
		return errCodeUseOSErrCode;
	}

	i = 0;
	while(fread(data, sizeOfData, 1, fp) == 1)
	{
		if (UnionCompareTheCondition(tbl, primaryKey, data, sizeOfData) == 1)
		{
			fclose(fp);
			free(data);
			return i;
		}
		i++;
	}

	fclose(fp);
	free(data);
	return -1;
}

/******************************
 * 读表的字段信息(内部接口)
 * input:  tbl - 表名
 *         fldName - 字段名
 * output: def - 字段定义结构
 * return: >=0;成功，代表fldName在结构体里的偏移
 *	   <0;失败
******************************/
int UnionReadTableFldAttr(char *tbl, char *fldName, PUnionTableDef def)
{
	char	buf[100];
	FILE	*fp;
	size_t	offset=0;
	char	fileName[80];

	memset(fileName, 0, sizeof fileName);
	memset(buf, 0, sizeof buf);
	
	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".def");

	fp=fopen(fileName,"r");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionReadTableFldAttr:: fopen err! fileName=[%s]\n",fileName);
		return errCodeUseOSErrCode;
	}

	while(fgets(buf, sizeof buf, fp)!=NULL)
	{
		sscanf(buf,"%s%s%s",def->fldName,def->fldType,def->fldLen);
		if (strcmp(fldName, def->fldName)==0)
		{
			if (strcmp(def->fldType, "int") == 0)
			{
				if (offset%sizeof(int) != 0)
					offset += (sizeof(int) - offset%sizeof(int));
			}
			else if (strcmp(def->fldType, "short") == 0)
			{
				if (offset%sizeof(short) != 0)
					offset += (sizeof(short) - offset%sizeof(short));
			}
			else if (strcmp(def->fldType, "long") == 0)
			{
				if (offset%sizeof(long) != 0)
					offset += (sizeof(long) - offset%sizeof(long));
			}
			fclose(fp);
			return offset;
		}
		if (strcmp(def->fldType, "int") == 0)
		{
			if (offset%sizeof(int) != 0)
				offset += (sizeof(int) - offset%sizeof(int));
			offset += sizeof(int);
		}
		else if (strcmp(def->fldType, "short") == 0)
		{
			if (offset%sizeof(short) != 0)
				offset += (sizeof(short) - offset%sizeof(short));
		}
		else if (strcmp(def->fldType, "long") == 0)
		{
			if (offset%sizeof(long) != 0)
				offset += (sizeof(long) - offset%sizeof(long));
		}
		else
			offset += atoi(def->fldLen);
	}

	fclose(fp);
	return -1;
}

/******************************
 * 找到文件里的第一个空位置(内部接口)
 * input:  tbl - 表名
 *         sizeOfData - 结构大小
 * output: 
 * return: >=0;成功，代表空位置索引
 *	   <0;失败
******************************/
int UnionFindNullPos(char *tbl, size_t sizeOfData)
{
	int	index;
	FILE	*fp;
	unsigned char *data;
	unsigned char *nullData;
	char	fileName[80];

	memset(fileName, 0, sizeof fileName);
	
	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".DB");

	fp=fopen(fileName,"r");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionFindNullPos:: fopen err! fileName=[%s]\n",fileName);
		return errCodeUseOSErrCode;
	}

	data = (unsigned char *)malloc(sizeOfData);
	if (data == NULL)
	{
		UnionSystemErrLog("in UnionFindNullPos:: malloc err!\n");
		fclose(fp);
		return errCodeUseOSErrCode;
	}
	nullData = (unsigned char *)malloc(sizeOfData);
	if (nullData == NULL)
	{
		UnionSystemErrLog("in UnionFindNullPos:: malloc err!\n");
		fclose(fp);
		free(data);
		return errCodeUseOSErrCode;
	}
	memset(nullData, 0, sizeOfData);
	
	index = 0;
	while(fread(data, sizeOfData, 1, fp) == 1)
	{
		if (memcmp(data, nullData, sizeOfData) == 0)
		{
			fclose(fp);
			free(data);
			free(nullData);
			return index;
		}
		index++;
	}

	fclose(fp);
	free(data);
	free(nullData);
	return -1;
}

/******************************
 * 插入一记录(外部接口)
 * input:  tbl - 表名
 *	   primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         data - 要插入的结构体指针
 *         sizeOfData - 结构体大小
 * output: 
 * return: =0;成功
 *	   <0;失败
******************************/
int UnionInsertIntoDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData)
{
	int	index;
	int	fd;
	FILE	*fp;
	char	fileName[80];
	char	cmd[100];

	memset(fileName, 0, sizeof fileName);
	memset(cmd, 0, sizeof cmd);
	
	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".DB");

	if (access(fileName, F_OK) !=0 )
	{
		sprintf(cmd,"%s%s",">",fileName);
		system(cmd);
	}

	if (UnionGetIndexByPrimaryKey(tbl, primaryKey, sizeOfData) >= 0)
	{
		UnionUserErrLog("duplicate record!\n");
		return (errCodeBinaryFileMDL_RecordAlreadyExist);
	}

	/* 该步与写文件应该作为一个原子操作 */
	index = UnionFindNullPos(tbl, sizeOfData);
	if (index < 0)
	{
		fp=fopen(fileName,"ab");
		if (fp == NULL)
		{
			UnionSystemErrLog("in UnionInsertIntoDB:: fopen err! fileName=[%s]\n",fileName);
			return errCodeUseOSErrCode;
		}
	} 
	else
	{
		fp=fopen(fileName,"r+b");
		if (fp == NULL)
		{
			UnionSystemErrLog("in UnionInsertIntoDB:: fopen err! fileName=[%s]\n",fileName);
			return errCodeUseOSErrCode;
		}
		fseek(fp, index*sizeOfData, SEEK_SET);
	}

	fd = fileno(fp);
	lockf(fd, F_LOCK, 0l);
	if (fwrite(data, sizeOfData, 1, fp) != 1)
	{
		UnionSystemErrLog("in UnionInsertIntoDB:: fwrite err! fileName=[%s]\n",fileName);
		lockf(fd, F_ULOCK, 0l);
		fclose(fp);
		return errCodeUseOSErrCode;
	}

	lockf(fd, F_ULOCK, 0l);
	fclose(fp);
	return 0;
}

/******************************
 * 删除一条记录(外部接口)
 * input:  tbl - 表名
 *	   primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         sizeOfData - 结构体大小
 * output: 
 * return: =0;成功
 *	   <0;失败
******************************/
int UnionDeleteByPrimaryKey(char *tbl, char *primaryKey, size_t sizeOfData)
{
	FILE	*fp;
	int	index;
	int	fd;
	char	fileName[80];
	unsigned char *data;

	memset(fileName, 0, sizeof fileName);
	
	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".DB");

	fp=fopen(fileName,"r+b");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionDeleteByPrimaryKey:: fopen err! fileName=[%s]\n",fileName);
		return errCodeUseOSErrCode;
	}

	data = (unsigned char *)malloc(sizeOfData);
	if (data == NULL)
	{
		UnionSystemErrLog("in UnionDeleteByPrimaryKey:: malloc err!\n");
		fclose(fp);
		return errCodeUseOSErrCode;
	}

	index = UnionGetIndexByPrimaryKey(tbl, primaryKey, sizeOfData);
        if (index<0)
        {
                UnionUserErrLog("in UnionDeleteByPrimaryKey:: record is not found!key=[%s]\n",primaryKey);
                fclose(fp);
		free(data);
                return (errCodeBinaryFileMDL_RecordNotFound);
        }

	fseek(fp, index*sizeOfData, SEEK_SET);
	memset(data, 0, sizeOfData);
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0l);
	if (fwrite(data, sizeOfData, 1, fp) != 1)
	{
		UnionSystemErrLog("in UnionInsertIntoDB:: fwrite err! fileName=[%s]\n",fileName);
		lockf(fd, F_ULOCK, 0l);
		fclose(fp);
		free(data);
		return errCodeUseOSErrCode;
	}

	lockf(fd, F_ULOCK, 0l);
	fclose(fp);
	free(data);
	return 0;
}

/******************************
 * 修改一条记录(外部接口)
 * input:  tbl - 表名
 *	   primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         data - 结构体指针
 *         sizeOfData - 结构体大小
 * output: 
 * return: =0;成功
 *	   <0;失败
******************************/
int UnionUpdateIntoDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData)
{
	FILE	*fp;
	int	fd;
	int	index;
	char	fileName[80];

	memset(fileName, 0, sizeof fileName);
	
	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".DB");
	
	fp=fopen(fileName,"r+b");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionUpdateIntoDB:: fopen err! fileName=[%s]\n",fileName);
		return errCodeUseOSErrCode;
	}

	index = UnionGetIndexByPrimaryKey(tbl, primaryKey, sizeOfData);
	if (index<0)
	{
                UnionUserErrLog("in UnionUpdateIntoDB:: record is not found!key=[%s]\n",primaryKey);
                fclose(fp);
                return (errCodeBinaryFileMDL_RecordNotFound);
	}

	fseek(fp, index*sizeOfData, SEEK_SET);
	fd = fileno(fp);
	lockf(fd, F_LOCK, 0l);
	if (fwrite(data, sizeOfData, 1, fp) != 1)
	{
		UnionSystemErrLog("in UnionUpdateIntoDB:: fwrite err! fileName=[%s]\n",fileName);
		lockf(fd, F_ULOCK, 0l);
		fclose(fp);
		return errCodeUseOSErrCode;
	}

	lockf(fd, F_ULOCK, 0l);
	fclose(fp);
	return 0;
}

/******************************
 * 查询一条记录(外部接口)
 * input:  tbl - 表名
 *	   primaryKey - 唯一索引，格式："关键字1=值|...|关键字n=值|"
 *         sizeOfData - 结构体大小
 * output: data - 结构体指针
 * return: =0;成功
 *	   <0;失败
******************************/
int UnionSelectSpecFromDB(char *tbl, char *primaryKey, unsigned char *data, size_t sizeOfData)
{
	FILE	*fp;
	int	index;
	char	fileName[80];

	memset(fileName, 0, sizeof fileName);
	
	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".DB");

	fp=fopen(fileName,"rb");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionSelectSpecFromDB:: fopen err! fileName=[%s]\n",fileName);
		return errCodeUseOSErrCode;
	}

	index = UnionGetIndexByPrimaryKey(tbl, primaryKey, sizeOfData);
	if (index<0)
	{
                UnionUserErrLog("in UnionSelectSpecFromDB:: record is not found!key=[%s]\n",primaryKey);
                fclose(fp);
                return (errCodeBinaryFileMDL_RecordNotFound);
	}

	fseek(fp, index*sizeOfData, SEEK_SET);
	if (fread(data, sizeOfData, 1, fp) != 1)
	{
		UnionSystemErrLog("in UnionSelectSpecFromDB:: fread err! fileName=[%s]\n",fileName);
		fclose(fp);
		return errCodeUseOSErrCode;
	}

	fclose(fp);
	return 0;
}

/******************************
 * 批量查询记录(外部接口)
 * input:  tbl - 表名
 *	   condition - 条件，格式："字段1=值|...|字段n=值|"
 *         sizeOfData - 结构体大小
 *         outFile - 结果文件名
 * output: 
 * return: >=0;成功，代表总查询的数量
 *	   <0;失败
******************************/
int UnionSelectBatchFromDB(char *tbl, char *condition, size_t sizeOfData, char *outFile)
{
	int	num;
	FILE	*fp;
	FILE	*fp1;
	unsigned char *data;
	char	fileName[80];
	char	record[1024];

	memset(fileName, 0, sizeof fileName);
	memset(record, 0, sizeof record);
	
	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".DB");

	fp=fopen(fileName,"rb");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionSelectBatchFromDB:: fopen err! fileName=[%s]\n",fileName);
		return errCodeUseOSErrCode;
	}

	fp1=fopen(outFile,"wb");
	if (fp1 == NULL)
	{
		UnionSystemErrLog("in UnionSelectBatchFromDB:: fopen err! fileName=[%s]\n",fileName);
		return errCodeUseOSErrCode;
	}

	data = (unsigned char *)malloc(sizeOfData);
	if (data==NULL)
	{
		UnionSystemErrLog("in UnionSelectBatchFromDB:: malloc err!\n");
		fclose(fp);
		fclose(fp1);
		return errCodeUseOSErrCode;
	}

	memset(data, 0, sizeOfData);
	while (fread(data, sizeOfData, 1, fp) == 1)
	{
		if (UnionCompareTheCondition(tbl, condition, data, sizeOfData) == 1)
		{
			/* 转化格式 */
			if ( UnionTransStructToString(tbl, data, record) < 0 )
			{
				UnionUserErrLog("in UnionSelectBatchFromDB:: UnionTransStructToString err!\n");
				continue;
			}
				
			fprintf(fp1,"%s\n", record);
			num++;
			memset(record, 0, sizeof record);
		}
		memset(data, 0, sizeOfData);
	}

	free(data);
	fclose(fp);
	fclose(fp1);
	return num;
}


/******************************
 * 将一个结构体转换为一个字符串
 * input:  tbl - 表名
 *         data - 结构体
 * output: resStr - 字符串，格式为: "字段1=值|...|字段n=值|"
 * return: >=0;成功，代表resStr的长度
 *	   <0;失败
******************************/
int UnionTransStructToString(char *tbl, unsigned char *data, char *resStr)
{
	TUnionTableDef def;
	int	offset;
	FILE	*fp;
	char	fileName[80];
	char	buf[100];
	char	fldName[20+1];
	char	fldValue[256];
	int	fldValue_i;
	long	fldValue_l;
	short	fldValue_s;

	memset(&def, 0, sizeof def);
	memset(fileName, 0, sizeof fileName);
	memset(buf, 0, sizeof buf);
	memset(fldName, 0, sizeof fldName);
	memset(fldValue, 0, sizeof fldValue);

	strcpy(fileName, getenv("UNIONETC"));
	strcat(fileName, "/");
	strcat(fileName, tbl);
	strcat(fileName, ".def");

	fp=fopen(fileName,"r");
	if (fp == NULL)
	{
		UnionSystemErrLog("in UnionTransStructToString:: fopen err! fileName=[%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}

	while(fgets(buf, sizeof buf, fp)!=NULL)
	{
		if (strlen(buf)==0)
			continue;
		sscanf(buf,"%s%s%s",fldName,def.fldType,def.fldLen);
		offset = UnionReadTableFldAttr(tbl, fldName, &def);
		if (offset < 0)
		{
			UnionUserErrLog("in UnionTransStructToString:: UnionReadTableFldAttr err! fldName=[%s]\n", def.fldName);
			fclose(fp);
			return (offset);
		}
		if (strcmp(def.fldType, "int") == 0)
		{
			memcpy(&fldValue_i, data+offset, sizeof(int));
			sprintf(resStr, "%s%s%c%d%c", resStr, def.fldName, '=', fldValue_i, '|');
		}
		else if (strcmp(def.fldType, "short") == 0)
		{
			memcpy(&fldValue_s, data+offset, sizeof(short));
			sprintf(resStr, "%s%s%c%d%c", resStr, def.fldName, '=', fldValue_s, '|');
		}
		else if (strcmp(def.fldType, "long") == 0)
		{
			memcpy(&fldValue_l, data+offset, sizeof(long));
			sprintf(resStr, "%s%s%c%ld%c", resStr, def.fldName, '=', fldValue_l, '|');
		}
		else if (strcmp(def.fldType, "char") == 0)
		{
			memcpy(fldValue, data+offset, atoi(def.fldLen));
			sprintf(resStr, "%s%s%c%s%c", resStr, def.fldName, '=', fldValue, '|');
		}
		memset(fldValue, 0, sizeof fldValue);
		memset(buf, 0, sizeof buf);
		memset(&def, 0, sizeof def);
	}

	fclose(fp);
	return(strlen(resStr));
}

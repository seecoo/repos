#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionREC.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionCommBetweenMDL.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "UnionStr.h"

#define		defTablePinPadName	"pinPad"
#define		defTableBranchName	"pinPadBranch"
#define		defTableProducterName	"pinPadProducter"
#define		defTableSysUserName	"sysUser"

int UnionTaskActionBeforeExit()
{
        exit(0);
}

typedef struct
{
	char    pinPadID[20+1];
	char    regTime[14+1];
	char    lastTime[14+1];
	char    status[2+1];
	char    branch[12+1];
	char    teller[12+1];
	char    remark[40+1];
	char    producter[40+1];
	char    producterID[40+1];
	char    zmkLength[2+1];
	char    zpkLength[2+1];
	char    zakLength[2+1];
	char    appNo[10+1];
} TUnionPinPad;
typedef TUnionPinPad   *PUnionPinPad;


int UnionHelp()
{
	printf("Usage::	%s  mdoe  fileName\n",UnionGetApplicationName());
	printf("Usage::	mode = 1, fileName must be bankTBL.DB\n");
	printf("Usage::	mode = 2, fileName must be producter.DB\n");
	printf("Usage::	mode = 3, fileName must be pinPad.DB\n");
	printf("Usage::	mode = 4, fileName must be operator.DB\n");
	return 0;
}

// 读取'[][][]'格式的文件
int UnionReadFieldValueToFieldGrp(char *src,char fieldValueGrp[][256],int fieldNum);
// 读取'[][]'格式的文件数据到数据库
int UnionPutOldTableDataFileToDB(char *fileName,char *tableName,char *fieldNameList,int fieldNum,int *successNum,int *failNum);
// 读取旧密码键盘文件数据到数据库
int UnionPutOldPinPadToDB(char *fileName,int *successNum,int *failNum);

int main(int argc,char *argv[])
{
	int	ret = 0;
	int	mode = 0;
	char	fileName[128];
	int	successNum = 0;
	int	failNum = 0;
	char	fieldNameList[256];
	int	fieldNum = 0;

	UnionSetApplicationName(argv[0]);
	if (argc < 3)
		return(UnionHelp());
	
	mode = atoi(argv[1]);
	snprintf(fileName,sizeof(fileName),"%s",argv[2]);

	switch(mode)
	{
		case	1:	// 迁移旧的分行机构号
			snprintf(fieldNameList,sizeof(fieldNameList),"branchNo,branchName,contact,remark");
			fieldNum = 4;
			if ((ret = UnionPutOldTableDataFileToDB(fileName,defTableBranchName,fieldNameList,fieldNum,&successNum,&failNum)) < 0)
			{
				UnionUserErrLog("in %s:: UnionPutOldBranchFileToDB tableName[%s]!\n",UnionGetApplicationName(),defTableBranchName);		
				return(ret);
			}
			printf("导入表名:%s	总记录数:[%d]	成功数:[%d]	失败数:[%d]!\n",defTableBranchName,ret,successNum,failNum);
			break;
		case	2:	// 迁移旧的密码键盘厂商数据
			snprintf(fieldNameList,sizeof(fieldNameList),"producter,dllName,remark");
			fieldNum = 3;
			if ((ret = UnionPutOldTableDataFileToDB(fileName,defTableProducterName,fieldNameList,fieldNum,&successNum,&failNum)) < 0)
			{
				UnionUserErrLog("in %s:: UnionPutOldTableDataFileToDB tableName[%s]!\n",UnionGetApplicationName(),defTableProducterName);
				return(ret);
			}
			printf("导入表名:%s	总记录数:[%d]	成功数:[%d]	失败数:[%d]!\n",defTableProducterName,ret,successNum,failNum);
			break;
		case	3:	// 迁移旧的密码键盘数据
			if ((ret = UnionPutOldPinPadToDB(fileName,&successNum,&failNum)) < 0)
			{
				UnionUserErrLog("in %s:: UnionPutOldPinPadToDB!\n",UnionGetApplicationName());		
				return(ret);
			}
			printf("导入表名:%s	总记录数:[%d]	成功数:[%d]	失败数:[%d]!\n",defTablePinPadName,ret,successNum,failNum);
			break;
		case	4:
			snprintf(fieldNameList,sizeof(fieldNameList),"userID,userName,userPassword,userRoleList,loginFlag,userPK,loginSysID,remark");
			fieldNum = 5;
			if ((ret = UnionPutOldTableDataFileToDB(fileName,defTableSysUserName,fieldNameList,fieldNum,&successNum,&failNum)) < 0)
			{
				UnionUserErrLog("in %s:: UnionPutOldTableDataFileToDB tableName[%s]!\n",UnionGetApplicationName(),defTableSysUserName);
				return(ret);
			}
			printf("导入表名:%s	总记录数:[%d]	成功数:[%d]	失败数:[%d]!\n",defTableSysUserName,ret,successNum,failNum);
			break;
		default:
			printf("in %s:: mode[%d] is invalid!\n",UnionGetApplicationName(),mode);
			return(0);
	}
	return 0;
}

// 读取'[][][]'格式的文件
int UnionReadFieldValueToFieldGrp(char *src,char fieldValueGrp[][256],int fieldNum)
{
	int	realNum = 0;
	int	offset = 0;
	char	*head = NULL;
	char	*tail = NULL;
	char	*ptr = NULL;

	ptr = src;

	while((ptr != NULL) && (ptr != '\0'))
	{
		if ((head = strchr(ptr,'[')) == NULL)
			break;
		
		if ((tail = strchr(head,']')) == NULL)
			break;

		offset = tail - head;
		memcpy(fieldValueGrp[realNum],ptr+1,offset-1);
		fieldValueGrp[realNum][offset] = 0;

		realNum ++;
		if (realNum == fieldNum)
			break;

		ptr += offset + 1;
	}
	if (realNum != fieldNum)
	{
		UnionUserErrLog("in UnionReadFieldValueToFieldGrp:: '[]' fieldNum[%d] != currFieldNum[%d] src[%s]!\n",fieldNum,realNum, src);
		return (errCodeParameter);
	}

	return realNum;
}

// 读取'[][]'格式的文件数据到数据库
int UnionPutOldTableDataFileToDB(char *fileName,char *tableName,char *fieldNameList,int fieldNum,int *successNum,int *failNum)
{
	int		ret = 0;
	int		lenOfBuf = 0;
	char		buf[512];
	FILE		*fp = NULL;
	char		sql[1024];
	char		fieldValueGrp[12][256];
	int		i = 0;
	int		len = 0;
	char		createTime[16];

	memset(createTime,0,sizeof(createTime));
        UnionGetFullSystemDateTime(createTime);

	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionUserErrLog("in UnionPutOldTableDataFileToDB:: fopen[%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}

	while(!feof(fp))
	{
		if ((lenOfBuf = UnionReadOneLineFromTxtStr(fp,buf,sizeof(buf))) < 0)		
		{
			if (lenOfBuf == errCodeEnviMDL_NullLine)
				continue;
			UnionUserErrLog("in UnionPutOldTableDataFileToDB:: UnionReadOneLineFromTxtStr!\n");
			break;
		}
		memset(fieldValueGrp,0,sizeof(fieldValueGrp));	
		if ((ret = UnionReadFieldValueToFieldGrp(buf,fieldValueGrp,fieldNum)) < 0)
		{
			UnionUserErrLog("in UnionPutOldTableDataFileToDB:: UnionReadFieldValueToFieldGrp buf[%s]!\n",buf);
			(*failNum) ++;
			continue;
			//return (ret);
		}
		
		// 拼装插入sql语句
		if (strcmp(tableName,defTableSysUserName) == 0)
		{
			if (strlen(fieldValueGrp[1]) == 0)
				snprintf(fieldValueGrp[1],sizeof(fieldValueGrp[1]),"%s",fieldValueGrp[0]);

			snprintf(sql,sizeof(sql),"insert into sysUser(userID,userName,userPassword,userRoleList,organization,loginFlag,loginTimes,wrongPasswordTimes,userStatus,createTime) values ("
			"'%s','%s','%s','%s','密码键盘',0,0,0,1,'%s')",
			fieldValueGrp[0],fieldValueGrp[1],fieldValueGrp[2],fieldValueGrp[3],createTime);	
		}
		else
		{
			for (i = 0; i < fieldNum; i++)
			{
				if (i == 0)	// 第一行
					len = snprintf(sql,sizeof(sql),"insert into %s (%s) values ('%s'", tableName, fieldNameList, fieldValueGrp[i]);
				else if (i < (fieldNum - 1))	// 中间值
					len += snprintf(sql+len,sizeof(sql)-len,",'%s'", fieldValueGrp[i]);
				else		// 最后一个值
					len += snprintf(sql+len,sizeof(sql)-len,",'%s')", fieldValueGrp[i]);
			}
		}

		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionPutOldTableDataFileToDB:: UnionExecRealDBSql ret[%d] sql[%s]!\n",ret,sql);
			(*failNum) ++;
			continue;
		}
		(*successNum) ++;
		if ((*failNum + *successNum) % 1000 == 0)
			printf("导入表:%s		当前导入成功数:[%d]		当前导入失败数:[%d]\n",tableName,*successNum,*failNum);
	}
	printf("导入表:%s		当前导入成功数:[%d]		当前导入失败数:[%d]\n",tableName,*successNum,*failNum);
	if (fp != NULL)
		fclose(fp);
	return(*failNum + *successNum);
}

// 读取旧密码键盘文件数据到数据库
int UnionPutOldPinPadToDB(char *fileName,int *successNum,int *failNum)
{
	int		ret = 0;
	char		sql[5120];
	FILE		*fp = NULL;
	char		buf[512];
	TUnionPinPad	tpinPadDB;
	int		len = 0;
	int		totalNum = 0;
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionUserErrLog("in UnionPutOldPinPadToDB:: fopen[%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}

	memset(buf,0,sizeof(buf));
	while (fread(buf, sizeof(TUnionPinPad), 1, fp) == 1)
	{
		memset(&tpinPadDB,0,sizeof(TUnionPinPad));

		memcpy(&tpinPadDB,buf,sizeof(TUnionPinPad));

		len += snprintf(sql + len,sizeof(sql) -len ,"insert into %s (pinPadID,regTime,lastTime,status,branch,teller,producter,producterID,zmkLength,zpkLength,zakLength,appNo,remark) values ('%s','%s','%s','%s','%s','%s','%s','%s',%s,%s,%s,'%s','%s');",
					defTablePinPadName,
					tpinPadDB.pinPadID,
					tpinPadDB.regTime,
					tpinPadDB.lastTime,
					tpinPadDB.status,
					tpinPadDB.branch,
					tpinPadDB.teller,
					tpinPadDB.producter,
					tpinPadDB.producterID,
					tpinPadDB.zmkLength,
					tpinPadDB.zpkLength,
					tpinPadDB.zakLength,
					tpinPadDB.appNo,
					tpinPadDB.remark);
		totalNum ++;
		if (totalNum % 10 == 0)
		{
			len = 0;
			if ((ret = UnionExecRealDBSql(sql)) < 0)
			{
				UnionUserErrLog("in UnionPutOldPinPadToDB:: UnionExecRealDBSql ret[%d] sql[%s]!\n",ret,sql);
				(*failNum) += 10;
				continue;
			}

			(*successNum) += 10;

			if ((*failNum + *successNum) % 1000 == 0)
				printf("导入表:%s		当前导入成功数:[%d]		当前导入失败数:[%d]\n",defTablePinPadName,*successNum,*failNum);
		}
		memset(buf,0,sizeof(buf));
	}

	if (len > 0)
	{
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionPutOldPinPadToDB:: UnionExecRealDBSql ret[%d] sql[%s]!\n",ret,sql);
			(*failNum) += 1;
		}
		else
			(*successNum) += 1;
	}
	printf("导入表:%s		当前导入成功数:[%d]		当前导入失败数:[%d]\n",defTablePinPadName,*successNum,*failNum);

	if (fp != NULL)
		fclose(fp);
	return (totalNum);
}

//	Author: zhangyongding
//	Date: 2008-12-3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionRec0.h"
#include "unionREC.h"

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionCalcuOperator.h"
#include "unionRealDBTransformStr.h"
#include "UnionStr.h"
#include "unionTableDef.h"

char szKeyWord[32][32]={"SELECT", "UPDATE", "DELETE", "AS", "SUM", "COUNT", "DISTINCT", "FROM", "WHERE", 
	"ORDER", "GROUP", "BY", "HAVING", "AND", "OR","IN", "select", "update", "delete", "as", "sum", "count", 
	"distinct", "from", "where", "order", "group", "by", "having", "and", "or", "in"};

/*
//add function by hzh in 2011.9.21
功能：
	将界面输入的模糊查询串自动添加'%'
*/
int UnionAutoAddPercentsign(char *likeStrFld)
{
	int len = 0;
	char buf[1024];
	
	len = strlen(likeStrFld);
	if (likeStrFld == NULL || len < 2)
		return 0;
	
	if(likeStrFld[0] != '\'' || likeStrFld[len-1] != '\'')
		return 0;
		
	memset(buf,0,sizeof(buf));	
	strcpy(buf,likeStrFld);
	if(likeStrFld[1] != '%')
	{
		memset(buf,0,sizeof(buf));
		buf[0] = '\'';
		buf[1] = '%';
		strcpy(buf+2,likeStrFld+1);
	}
	len = strlen(buf);
	if(buf[len-2] != '%')
	{
		buf[len-1] = '%';
		buf[len] = '\'';
	}
	strcpy(likeStrFld,buf);
	len = strlen(buf);
	return len;
}

/*
功能：
	将一个条件串中的域名替换为库中的域名
输入参数：
	resName		表名
	oriSQLStr	原串
输出参数：
	desSQLStr	目标串
返回值：
	>=0：		目标串的长度
	<0：		失败，错误码
*/
int UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject(char *resName,char *oriSQLStr,char *desSQLStr)
{
	char	oriFldGrp[128][128];
	int	index;
	int	num;
	int	len;
	int	ret;
	
	// 2009/12/15，xusj add
	UnionSetTBLFldPrefixAttr(resName);
	// 2009/12/15,xusj add ended.

	if ((num = UnionReadSQLWhereWordFromStr(oriSQLStr,strlen(oriSQLStr),oriFldGrp,128)) < 0)
	{
		UnionUserErrLog("in UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject:: UnionReadSQLWhereWordFromStr!\n");
		return(num);
	}
	//UnionLog("in UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject:: UnionReadSQLWhereWordFromStr OK! num = [%d]\n",num);
	if ((ret = UnionConvertObjectFldNameOfSpecObjectInNameGrp2(resName,oriFldGrp,num,oriFldGrp)) < 0)
	{
		UnionUserErrLog("in UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject:: UnionConvertObjectFldNameOfSpecObjectInNameGrp [%s]!\n",resName);
		return(ret);
	}
	len = 0;
	//UnionLog("in UnionConvertFldDefNameInSQLStrIntoDBFldNameOfSpecObject:: UnionConvertObjectFldNameOfSpecObjectInNameGrp OK! num = [%d]\n",num);
	for (index = 0; index < num; index++)
	{
		//add by hzh in 2011.9.22， 对模糊查询，如界面未对查询字段输入'%',则自动添加
		if(strcmp(oriFldGrp[index],"like") == 0)  
		{
			if(index+2 < num)
			{
				UnionAutoAddPercentsign(oriFldGrp[index+2]);
			}
		}
		//add end 	
		sprintf(desSQLStr+len,"%s",oriFldGrp[index]);
		len = strlen(desSQLStr);
	}
	//UnionProgramerLog("================desSQLStr[%s]======================!\n",desSQLStr);
	return(strlen(desSQLStr));
}
	
/*
功能：判断一个字符串是不是SQL语句
输入参数：
	recStr：	请求字符串
	lenOfRecStr：	recStr的长度
	sizeOfBuf：	sqlStr的存储大小
输出参数：
	sqlStr：	如果是SQL语句，过滤后的sql字符串，否则返回空值
返回值：
	>0：		返回过滤后SQL语句的长度
	0：		非SQL语句
	<0：		失败，错误码
*/
int UnionIsSQLString(char *recStr,int lenOfRecStr,char *sqlStr,int sizeOfBuf)
{
	char	*ptr;
	int	len;
	
	if ((recStr == NULL) || (sqlStr == NULL))
	{
		UnionUserErrLog("in UnionIsSQLString:: recStr or sqlStr is NULL!\n");
		return(errCodeParameter);
	}

	if (memcmp(recStr,"SQL::",5) == 0)
	{
		memcpy(sqlStr,recStr+5,lenOfRecStr-5);
		return(lenOfRecStr-5);
	}
	if ((ptr = strstr(recStr,"|SQL::")) == NULL)
		return(0);
	*ptr = 0;
	strcpy(sqlStr,recStr);
	len = strlen(sqlStr);
	*ptr = '|';
	sqlStr[len] = *ptr;
	len++;
	if (lenOfRecStr-len-6 < 0)
	{
		UnionUserErrLog("in UnionIsSQLString:: lenOfRecStr = [%d] error!\n",lenOfRecStr);
		return(errCodeParameter);
	}
	memcpy(sqlStr+len,ptr+6,lenOfRecStr-len-5);
	//UnionLog("sqlStr = [%s]\n",sqlStr);
	return(lenOfRecStr-5);
}

/*
功能：将一个记录串转换为SQL的字符串
输入参数：
	recStr：	请求字符串
	lenOfRecStr：	recStr的长度
	type：		SQL的类型
	sizeOfBuf：	sqlStr的存储大小
输出参数：
	sqlStr：	插入SQL的值串
返回值：
	>=0：		成功
	<0：		失败，错误码
*/
int UnionTransformRecStrToSQLStr(char *tblName,char *recStr,int lenOfRecStr,TUnionTransformType type,char *sqlStr,int sizeOfBuf)
{
	int		i;
	int		realFldNum = 0;
	int		ret;
	char		tmpBuf1[8192*2+1];
	char		tmpBuf2[8192*4+1];
	char		tmpBuf3[8192*2+1];
	TUnionRec	rec;
	PUnionTableDef	pobjDef = NULL;
	char		realFldName[128+1];
	int		len;		// 2009/8/22,Wolfgang Wang added
	int		compType = conCalcuOperatorAnd;	// 2009/8/22,Wolfgang Wang added
	int		newCompType;
	int		currentPage = -1,numOfPerPage = -1;	// 2009/11/11,Wolfgang Wang added;
	
	// 2009/10/10，wangchunjun add
	UnionSetTBLFldPrefixAttr(tblName);
	// 2009/10/10,wangchunjun add ended.

	if ((pobjDef = UnionFindTableDef(tblName)) == NULL)
	{
		ret = UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists);
		UnionUserErrLog("in UnionTransformRecStrToSQLStr:: UnionFindTableDef [%s]!\n", tblName);
		return(ret);
	}
	
	if ((recStr == NULL) || (sqlStr == NULL))
	{
		UnionUserErrLog("in UnionTransformRecStrToSQLStr:: recStr or sqlStr is NULL!\n");
		return(errCodeParameter);
	}

	memset(&rec,0,sizeof(rec));
	// 2009/8/22, Wolfgang Wang modified end
	switch (type)
	{
		case conTransformTypeCondition:
			if ((ret = UnionReadRecFromRecStrRepeatPermitted(recStr,lenOfRecStr,&rec)) < 0)
			{
				UnionUserErrLog("in UnionTransformRecStrToSQLStr:: UnionReadRecFromRecStrRepeatPermitted!\n");
				return(ret);
			}
			break;
		default:
			if ((ret = UnionReadRecFromRecStr(recStr,lenOfRecStr,&rec)) < 0)
			{
				UnionUserErrLog("in UnionTransformRecStrToSQLStr:: UnionReadRecFromRecStr!\n");
				return(ret);
			}
			break;
	}
	// 2009/8/22, Wolfgang Wang modified end
	if (type == conTransformTypeInsert)		// 插入值串
	{
		memset(tmpBuf1,0,sizeof(tmpBuf1));
		memset(tmpBuf2,0,sizeof(tmpBuf2));
		for(i = 0; i < rec.fldNum; i++)
		{
			memset(tmpBuf3,0,sizeof(tmpBuf3));
			if ((ret = UnionGetFormatFldValueOfSpecFldFromTableDef(pobjDef,rec.fldName[i],rec.fldValue[i],tmpBuf3)) < 0)
			{
				if (ret == errCodeObjectMDL_FieldNotExist)
					continue;
				UnionUserErrLog("in UnionTransformRecStrToSQLStr:: UnionGetFormatFldValueOfSpecFldFromTableDef ret = [%d]\n",ret);
				return(ret);
			}
			if (strlen(tmpBuf3) == 0)
				continue;
			realFldNum++;
			if (realFldNum > 1)
			{
				strcat(tmpBuf1,",");
				strcat(tmpBuf2,",");
			}
			UnionChargeFieldNameToDBFieldName(rec.fldName[i],realFldName);
			strcat(tmpBuf1,realFldName);
			strcat(tmpBuf2,tmpBuf3);
		}
		if (sizeOfBuf <= strlen(tmpBuf1) + strlen(tmpBuf2) + strlen("values") + 6)
		{
			UnionUserErrLog("in UnionTransformRecStrToSQLStr:: sizeOfBuf = [%d] too small for [%s] [%s]\n",sizeOfBuf,tmpBuf1,tmpBuf2);
			return(errCodeSmallBuffer);
		}
		sprintf(sqlStr,"(%s) values (%s)",tmpBuf1,tmpBuf2);
	}
	else if (type == conTransformTypeUpdate)	// 更新值串
	{
		memset(tmpBuf1,0,sizeof(tmpBuf1));
		for(i = 0; i < rec.fldNum; i++)
		{
			memset(tmpBuf3,0,sizeof(tmpBuf3));
			if ((ret = UnionGetFormatFldValueOfSpecFldFromTableDef(pobjDef,rec.fldName[i],rec.fldValue[i],tmpBuf3)) < 0)
			{
				if (ret == errCodeObjectMDL_FieldNotExist)
					continue;
				UnionUserErrLog("in UnionTransformRecStrToSQLStr:: UnionGetFormatFldValueOfSpecFldFromTableDef ret = [%d]\n",ret);
				return(ret);
			}
			if (strlen(tmpBuf3) == 0)
			{
				switch (ret)
				{
					case	conObjectFldType_Int:
					case	conObjectFldType_Bool:
					case	conObjectFldType_Double:
						strcpy(tmpBuf3,"0");
						break;
				}
			}
			realFldNum++;
			if (realFldNum > 1)
			{
				strcat(tmpBuf1,",");
				strcat(tmpBuf2,",");
			}
			UnionChargeFieldNameToDBFieldName(rec.fldName[i],realFldName);
			strcat(tmpBuf1,realFldName);
			strcat(tmpBuf1,"=");
			strcat(tmpBuf1,tmpBuf3);
		}
		sprintf(sqlStr,"%s",tmpBuf1);
	}
	else if (type == conTransformTypeCondition)	// 条件串
	{
		memset(tmpBuf1,0,sizeof(tmpBuf1));
		for(i = 0; i < rec.fldNum; i++)
		{
			// added by Wolfgang Wang, 2009/8/22
			/* // 2009/11/16删除，王纯军
			if ((i == 0) && (strcmp(rec.fldName[i],"operator") == 0))	
			{
				if ((compType = UnionConvertCalcuOperatorStrTagIntoIntTag(rec.fldValue[0])) < 0)
					compType = conCalcuOperatorAnd;
				else
					continue;
			}
			*/ // 2009/11/16删除结束，王纯军
			if (strcmp(rec.fldName[i],"operator") == 0)	// 2009/11/16增加，王纯军
			{
				if ((newCompType = UnionConvertCalcuOperatorStrTagIntoIntTag(rec.fldValue[i])) >= 0)
				{
					compType = newCompType;
					continue;
				}
			}	// 2009/11/16增加，王纯军结束	
			// end addition by wolfgang wang,2009/8/22
			// 2009/11/11，王纯军重写了以下这段
			if (strcmp(rec.fldName[i],"currentPage") == 0)
			{
				currentPage = atoi(rec.fldValue[i]);
				continue;
			}
			else if (strcmp(rec.fldName[i],"numOfPerPage") == 0)
			{
				numOfPerPage = atoi(rec.fldValue[i]);
				continue;
			}

			/** del by xusj 20100607 begin***
			UnionChargeFieldNameToDBFieldName(rec.fldName[i],realFldName);
			strcat(tmpBuf1,realFldName);
			//	strcat(tmpBuf1,"="); deleted by Wolfgang Wang, 2009/8/22
			// added by Wolfgang Wang, 2009/8/22
			len = strlen(tmpBuf1);
			UnionConvertSpecRelationCompTypeIntoDBValueAssignTag(compType,tmpBuf1+len);
			// end addition by wolfgang wang,2009/8/22
			** del by xusj 20100607 end***/

			memset(tmpBuf3,0,sizeof(tmpBuf3));
			if ((ret = UnionGetFormatFldValueOfSpecFldFromTableDef(pobjDef,rec.fldName[i],rec.fldValue[i],tmpBuf3)) < 0)
			{
				if (ret == errCodeObjectMDL_FieldNotExist)
					continue;
				UnionUserErrLog("in UnionTransformRecStrToSQLStr:: UnionGetFormatFldValueOfSpecFldFromTableDef ret = [%d]\n",ret);
				return(ret);
			}

			/*** add by xusj 20100607 begin ***/
			if ( (i != 0) && (strlen(tmpBuf1) != 0) )
			{
				len = strlen(tmpBuf1);
				UnionConvertSpecRelationCompTypeIntoDBRelationCompTag(compType,tmpBuf1+len);
			}
			UnionChargeFieldNameToDBFieldName(rec.fldName[i],realFldName);
			strcat(tmpBuf1,realFldName);
			len = strlen(tmpBuf1);
			UnionConvertSpecRelationCompTypeIntoDBValueAssignTag(compType,tmpBuf1+len);
			/*** add by xusj 20100607 end ***/

			strcat(tmpBuf1,tmpBuf3);
			/** del by xusj 20100607 begin***
			if (i != rec.fldNum - 1)
			{
				// strcat(tmpBuf1," and ");	//  deleted by Wolfgang Wang, 2009/8/22
				// added by Wolfgang Wang, 2009/8/22
				len = strlen(tmpBuf1);
				UnionConvertSpecRelationCompTypeIntoDBRelationCompTag(compType,tmpBuf1+len);
				// end addition by wolfgang wang,2009/8/22
			}
			** del by xusj 20100607 end***/
			// 2009/11/11，王纯军重写结束
		}
		// 2009/11/11，王纯军增加以下这段
		len = 0;
		if (currentPage >= 0)
		{
			sprintf(sqlStr,"currentPage=%d|",currentPage);
			len = strlen(sqlStr);
		}
		if (numOfPerPage > 0)
		{
			sprintf(sqlStr+len,"numOfPerPage=%d|",numOfPerPage);
			len = strlen(sqlStr);
		}
		// 2009/11/11，王纯军增加结束
		if (strlen(tmpBuf1) != 0)	// 2009/11/17，王纯军增加此句
			sprintf(sqlStr+len,"( %s )",tmpBuf1);
		//UnionLog("sqlStr = [%s]\n",sqlStr);
	}
	return(0);
}

/*
功能：从一个记录串获取操作符
输入参数：
	recStr：	请求字符串
	lenOfRecStr：	recStr的长度
输出参数：
	oper：		操作符
返回值：
	>=0：		成功
	<0：		失败，错误码
*/
int UnionGetOperatorFromRecStr(char *recStr,int lenOfRecStr,char *oper)
{
	int		ret;
	TUnionRec	conRec;
	int		compType = conCalcuOperatorAnd;

	// 读取记录条件串
	memset(&conRec,0,sizeof(conRec));
	if ((ret = UnionReadRecFromRecStr(recStr,lenOfRecStr,&conRec)) < 0)
	{
		UnionUserErrLog("in UnionGetOperatorFromRecStr:: UnionReadRecFromRecStr [%d] [%s]\n",lenOfRecStr,recStr);
		return(ret);
	}
	
	// 读取条件符
	if (strcmp(conRec.fldName[0],"operator") == 0)
	{
		if ((compType = UnionConvertCalcuOperatorStrTagIntoIntTag(conRec.fldValue[0])) < 0)
		{
			strcpy(oper,"and");
			return(0);
		}
	}
	
	switch (compType)
	{
		case	conCalcuOperatorAnd:
			strcpy(oper,"and");
			return(0);
		case	conCalcuOperatorOr:
			strcpy(oper,"or");
			return(0);
		case	conCalcuOperatorNot:
			strcpy(oper,"not");
			return(0);
		default:
			UnionUserErrLog("in UnionGetOperatorFromRecStr:: [%s] not valid relation operator!\n",conRec.fldValue[0]);
			strcpy(oper,"and");
			return(0);
	}		
}

/*
功能：
格式化域值
输入参数：
pobject：	指向对象定义的指针
fldName		域名称
输出参数：
formatFldVal	格式化后的域值
返回值：
>=0：	类型
<0：	失败，返回错误码
*/
int UnionGetFormatFldValueOfSpecFldFromTableDef(PUnionTableDef pobject,char *fldName,char *fldValue,char *formatFldVal)
{
	int	fldNum;

	if ((pobject == NULL) || (fldName == NULL) || (fldValue == NULL))
	{
		UnionUserErrLog("in UnionGetFormatFldValueOfSpecFldFromTableDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}

	for (fldNum = 0; fldNum < pobject->fieldNum; fldNum++)
	{
		if (strcmp(fldName, pobject->fieldDef[fldNum].fieldName) == 0)
		{
			switch (pobject->fieldDef[fldNum].fieldType)
			{
			case	1:
				sprintf(formatFldVal,"'%s'",fldValue);
				break;
			case	2:
				sprintf(formatFldVal,"%s",fldValue);
				break;
			default:
				UnionUserErrLog("in UnionGetFormatFldValueOfSpecFldFromTableDef:: invalid type [%d] of fld [%s] of object [%s]\n", pobject->fieldDef[fldNum].fieldType, fldName, pobject->tableName);
				return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldTypeInvalid));
			}
			return(pobject->fieldDef[fldNum].fieldType);
		}
	}
	UnionAuditLog("in UnionGetFormatFldValueOfSpecFldFromTableDef:: fldName [%s] not valid fld of object [%s]\n",fldName,pobject->tableName);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNotExist));
}

/*
功能：
对SQL语句做词法分析
输入参数：
strSQL：	SQL语句
iMaxSQLAnalyseResNum：	词法分析出的单词最大个数
输出参数：
tSQLAnalyseRes：	词法分析结果
返回值：
>=0：	成功，词法分析出的单词个数
<0：	失败，返回错误码
*/
int UnionAnalyzeSQL(char *strSQL, TUnionSQLAnalyseRes tSQLAnalyseRes[], int iMaxSQLAnalyseResNum)
{
	char *pTmpSql = strSQL;
	int i = 0;
	int j = 0;
	int k = 0;

	if( strSQL == NULL )
	{
		UnionUserErrLog("in UnionAnalyzeSQL:: NULL pointer!\n");
		return (errCodeParameter);
	}

	i = 0;
	while( *pTmpSql != '\0' && i < iMaxSQLAnalyseResNum )
	{
		// 判断标识符的情况
	    if( isalpha(*pTmpSql) )
	    {
	    	j = 0;
	    	while( isalpha(*pTmpSql) || isdigit(*pTmpSql) || *pTmpSql == '_' )
	    	{
		    	tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
		    }
		    tSQLAnalyseRes[i].str[j] = '\0';
		    // 判断是否为关键字
		    for( k = 0; k < 32; k++ )
		    {
		    	if( strcmp(szKeyWord[k], tSQLAnalyseRes[i].str) == 0 )
		    	{
		    		break;
		    	}
		    }
		    if( k >= 32 )
		    {
		    	tSQLAnalyseRes[i].iType = 2;
		    }
		    else
		    {
		    	tSQLAnalyseRes[i].iType = 1;
		    }
		    i++;
	    }
	    else if( isdigit(*pTmpSql) ) // 数值，类型3
	    {
	    	j = 0;
			while( isdigit(*pTmpSql) )
			{
				tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			}
			tSQLAnalyseRes[i].str[j] = '\0';
			tSQLAnalyseRes[i].iType = 3;
		    i++;
	    }
	    else if( *pTmpSql == '\'' ) // 字符串
	    {
	    	j = 0;
	    	tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			while( *pTmpSql != '\'' && j < 1024-2)
			{
				tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			}
			if( *pTmpSql == '\'' )
			{
				tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
				tSQLAnalyseRes[i].str[j] = '\0';
				tSQLAnalyseRes[i].iType = 4;
			    i++;
			}
			else
			{
				UnionUserErrLog("in UnionAnalyzeSQL:: string is too long [more   than   1024   bytes]!\n");
				return (errCodeParameter);
			}
	    }
	    else if( *pTmpSql == '\"' ) // 字符串
	    {
	    	j = 0;
	    	tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			while( *pTmpSql != '\"' && j < 1024-2)
			{
				tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			}
			if( *pTmpSql == '\"' )
			{
				tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
				tSQLAnalyseRes[i].str[j] = '\0';
				tSQLAnalyseRes[i].iType = 4;
			    i++;
			}
			else
			{
				UnionUserErrLog("in UnionAnalyzeSQL:: string is too long [more   than   1024   bytes]!\n");
				return (errCodeParameter);
			}
	    }
	    else if( *pTmpSql ==',' || *pTmpSql == '(' || *pTmpSql == ')' || *pTmpSql == '.' ) // 特殊分割符
		{
			j = 0;
			tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			tSQLAnalyseRes[i].str[j] = '\0';
			tSQLAnalyseRes[i].iType = 5;
		    i++;
		}
		else if( *pTmpSql == '=' || *pTmpSql == '+' || *pTmpSql == '-' || *pTmpSql == '*' || *pTmpSql == '/' ) // 运算符，单目
		{
			j = 0;
			tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			tSQLAnalyseRes[i].str[j] = '\0';
			tSQLAnalyseRes[i].iType = 6;
		    i++;
		}
		else if( *pTmpSql == '<' ) // 运算符，双目
		{
			j = 0;
			tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			if( *pTmpSql == '=' || *pTmpSql == '>' )
			{
				tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			}
			tSQLAnalyseRes[i].str[j] = '\0';
			tSQLAnalyseRes[i].iType = 6;
		    i++;
		}
		else if( *pTmpSql == '>' ) // 运算符，双目
		{
			j = 0;
			tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			if( *pTmpSql == '=' )
			{
				tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			}
			tSQLAnalyseRes[i].str[j] = '\0';
			tSQLAnalyseRes[i].iType = 6;
		    i++;
		}
		else if( *pTmpSql == '!' ) // 运算符，双目
		{
			j = 0;
			tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
			if( *pTmpSql == '=' )
			{
				tSQLAnalyseRes[i].str[j++] = *pTmpSql++;
				tSQLAnalyseRes[i].str[j] = '\0';
				tSQLAnalyseRes[i].iType = 6;
			    i++;
			}
			else
			{
				UnionUserErrLog("in UnionAnalyzeSQL:: syntax error [!]!\n");
				return (errCodeParameter);
			}
		}
		else
		{
			pTmpSql++;
		}
	}

	if( *pTmpSql != '\0' )
	{
		UnionUserErrLog("in UnionAnalyzeSQL:: iMaxSQLAnalyseResNum=[%d] is too small!\n", iMaxSQLAnalyseResNum);
		return (errCodeParameter);
	}

	return (i);
}

/*
功能：
将前台传入的SQL语句的域名转化为真正数据库域名
输入参数：
strSQL：		SQL语句
strTableName：	表名
iSizeOfDestSQL：存放转化后的SQL语句缓冲区的大小
输出参数：
strDestSQL：	转化后的SQL语句
返回值：
>=0：	成功，转换后SQL语句的长度
<0：	失败，返回错误码
*/
int UnionTransSQL(char *strSQL, char *strTableName, char *strDestSQL, int iSizeOfDestSQL)
{
	int ret = 0;
	int i = 0;
	int iCnt = 0;
	int iOffset = 0;
	TUnionSQLAnalyseRes tSQLRes[1000];
	PUnionTableDef		pobjDef = NULL;
	char szDefFldNameGrp[10][128];
	char szRealFldNameGrpInDB[10][128];

	if( strSQL == NULL || strTableName == NULL || strDestSQL == NULL )
	{
		UnionUserErrLog("in UnionTransSQL:: NULL pointer!\n");
		return (errCodeParameter);
	}

	memset(tSQLRes, 0, 1000 * sizeof(TUnionSQLAnalyseRes));
	if( (iCnt = UnionAnalyzeSQL(strSQL, tSQLRes, 1000)) < 0 )
	{
		UnionUserErrLog("in UnionTransSQL:: UnionAnalyzeSQL [%s] error!\n", strSQL);
		return (iCnt);
	}
	UnionAuditLog("in UnionTransSQL:: strSQL=[%s],iCnt=[%d]\n",strSQL,iCnt);
	
	if ((pobjDef = UnionFindTableDef(strTableName)) == NULL)
	{
		ret = UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists);
		UnionUserErrLog("in UnionTransSQL:: UnionFindTableDef [%s]!\n", strTableName);
		return(ret);
	}

	memset(szDefFldNameGrp, 0, sizeof(szDefFldNameGrp));
	for( i = 0; i < iCnt; i++ )
	{
		// 从标识符中找出域名
		if( tSQLRes[i].iType == 2 )
		{
			// 前面不能有'AS''as'
			if( i > 1 )
			{
				if( strcmp(tSQLRes[i-1].str, "AS") == 0 || strcmp(tSQLRes[i-1].str, "as") == 0 )
				{
					continue;
				}
				if( strcmp(tSQLRes[i-1].str, "FROM") == 0 || strcmp(tSQLRes[i-1].str, "from") == 0 )
				{
					continue;
				}
			}
			// 后面不能有'.'
			if( i < iCnt - 1 )
			{
				if( strcmp(tSQLRes[i+1].str, ".") == 0 )
				{
					continue;
				}
			}

			// 替换域名为数据库真实域名
			strcpy(szDefFldNameGrp[0], tSQLRes[i].str);
			memset(szRealFldNameGrpInDB, 0, sizeof(szRealFldNameGrpInDB));
			if( (ret = UnionConvertObjectFldNameInNameGrp(pobjDef, szDefFldNameGrp, 1, szRealFldNameGrpInDB)) < 0 )
			{
				UnionUserErrLog("in UnionTransSQL:: UnionConvertObjectFldNameInNameGrp!\n");
				return (ret);
			}
			strcpy(tSQLRes[i].str, szRealFldNameGrpInDB[0]);
		}
	}

	// 拼装SQL语句
	sprintf(strDestSQL, "%s ", tSQLRes[0].str);
	iOffset = strlen(tSQLRes[0].str) + 1;
	for( i = 1; i < iCnt - 1; i++ )
	{
		if( tSQLRes[i].iType == 5 )
		{
			if( strcmp(tSQLRes[i].str, "(") == 0 )
			{
				sprintf(strDestSQL+iOffset, "%s", tSQLRes[i].str);
				iOffset += strlen(tSQLRes[i].str);
			}
			else 
			{
				if( strcmp(tSQLRes[i].str, ".") == 0 )
				{
					iOffset--;
					sprintf(strDestSQL+iOffset, "%s", tSQLRes[i].str);
					iOffset += strlen(tSQLRes[i].str);
				}
				else
				{
					iOffset--;
					sprintf(strDestSQL+iOffset, "%s ", tSQLRes[i].str);
					iOffset += (strlen(tSQLRes[i].str) + 1);
				}
			}
		}
		else
		{
			sprintf(strDestSQL+iOffset, "%s ", tSQLRes[i].str);
			iOffset += (strlen(tSQLRes[i].str) + 1);
		}
	}
	// 最后一个单词
	if( tSQLRes[i].iType == 5 )
	{
		if( *(strDestSQL+iOffset-1) == ' ' )
		{
			iOffset--;
		}
	}
	sprintf(strDestSQL+iOffset, "%s", tSQLRes[i].str);
	iOffset += strlen(tSQLRes[i].str);


	if( iOffset >= iSizeOfDestSQL )
	{
		UnionUserErrLog("in UnionTransSQL:: Small buffer [%d]!\n", iSizeOfDestSQL);
		return (errCodeParameter);
	}

	return (iOffset);
}

/*
  功能：
  	转换用分隔符分开的多个域值成SQL的域值，如aaa,bbb,ccc,ddd转为'aaa','bbb','ccc'
  输入参数：
  	tblName：表名称
  	fldName：域名
  	valueList：域值清单
  	speratorTag：分隔符
  	sizeOfDesValueList：缓冲区大小
*/
int UnionTransValueListForFormatStr(char *tblName, char *fldName, char *valueList, char speratorTag, char *desValueList, int sizeOfDesValueList)
{
	int		ret;
	char		valueGrp[100][128];
	int		valueNum = 0;
	PUnionTableDef	pobjDef = NULL;
	char		formatFldVal[128+1];
	int		i;
	int		lenStr;
	char		tmpBuf[4096+1];
	
	if ((tblName == NULL) || (fldName == NULL) || (valueList == NULL) || (desValueList == NULL))
	{
		UnionUserErrLog("in UnionTransValueListForFormatStr:: NULL pointer!\n");
		return (errCodeParameter);
	}
	if (strlen(valueList) == 0)
		return 0;
	
	if ((pobjDef = UnionFindTableDef(tblName)) == NULL)
	{
		ret = UnionSetUserDefinedErrorCode(errCodeObjectMDL_ObjectDefinitionNotExists);
		UnionUserErrLog("in UnionTransValueListForFormatStr:: UnionFindTableDef [%s]!\n", tblName);
		return(ret);
	}

	// 拆分域值清单
	memset(valueGrp, 0, sizeof(valueGrp));
	if ((valueNum = UnionSeprateVarStrIntoVarGrp(valueList,strlen(valueList),speratorTag,valueGrp,100)) < 0)
	{
		UnionUserErrLog("in UnionTransValueListForFormatStr UnionSeprateVarStrIntoVarGrp [%s]\n",valueList);
		return(valueNum);
	}
	// 格式化域值
	memset(tmpBuf,0,sizeof(tmpBuf));
	lenStr = 0;
	for(i = 0; i < valueNum; i++)
	{ 
		memset(formatFldVal,0,sizeof(formatFldVal));
		if ((ret = UnionGetFormatFldValueOfSpecFldFromTableDef(pobjDef,fldName,valueGrp[i],formatFldVal)) < 0)
		{
			UnionUserErrLog("in UnionTransValueListForFormatStr UnionGetFormatFldValueOfSpecFldFromTableDef [%s]!ret=[%d]\n",valueGrp[i],ret);
			return(ret);
		}
		sprintf(tmpBuf+lenStr, "%s%c",formatFldVal,speratorTag);
		lenStr += strlen(formatFldVal) + 1;
	}
	if (lenStr-1 > sizeOfDesValueList)
	{
		UnionUserErrLog("in UnionTransValueListForFormatStr [%d] > sizeOfDesValueList[%d]!\n",lenStr-1,sizeOfDesValueList);
		return(errCodeSmallBuffer);
	}
	memcpy(desValueList, tmpBuf, lenStr-1);
	
	return valueNum;
}


/////////copy from unionComplexDBObjectDef.20090421.c/////////////
/*
功能：检查一个对象的定义是否存在
输入参数：
	idOfObject：对象ID
输出参数：
	无
返回值：
	1：存在
	0：不存在
	<0：失败，错误码
*/
int UnionExistsObjectDef(TUnionIDOfObject idOfObject)
{
	PUnionTableDef                  pgunionTableDef = NULL;

	if (idOfObject == NULL)
	{
		UnionUserErrLog("in UnionExistsObjectDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	if((pgunionTableDef = UnionFindTableDef(idOfObject)) == NULL)
	{
		UnionAuditLog("in UnionExistsObjectDef:: tableName = [%s] not found in image!\n", idOfObject);
                return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}	
	
	return(1);
}

/*
function:
        判断是否是对象中的字段
param:
        [IN]:
        idOfObject:     对象ID
        fldName:        字段名
        [OUT]:
        NULL
return:
        1               是关键字段
        0               不是关键字段
*/
int UnionIsFldOfObject(PUnionTableDef pobject, char *fldName)
{
        int             i;
	
        if (NULL == fldName || pobject == 0)
	{
                return 0;
	}

        for(i = 0; i < pobject->fieldNum; i++)
	{
                if(strcmp(pobject->fieldDef[i].fieldName, fldName) == 0)
		{
                        return 1;
		}
	}

        return 0;
}

/*
功能：
	将对象定义的域名称转换为实际数据库中的名称
输入参数：
	pobject：		指向对象定义的指针
	defFldNameGrp		域定义时采用的名称
输出参数：
	realFldNameGrpInDB	实际数据库中的名称	
返回值：
	>=0：	返回转换的域名称数量
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldNameInNameGrp(PUnionTableDef pobject,char defFldNameGrp[][128],int numOfFld,char realFldNameGrpInDB[][128])
{
	int	index;
	int	isTableName=0;
	int	num = 0;
	char	tmpBuf[128+1];
	
	for (index = 0; index < numOfFld; index++)
	{
		strcpy(tmpBuf,defFldNameGrp[index]);
		if (!UnionIsFldOfObject(pobject,tmpBuf) || isTableName == 2)
		{
                        if (isTableName == 1)
                                isTableName=2;
                        else
                                isTableName=0;
			strcpy(realFldNameGrpInDB[index],tmpBuf);
			if (strcmp(tmpBuf,"from") == 0)
				isTableName = 1;
		}
		else
		{
			++num;
			UnionChargeFieldNameToDBFieldName(tmpBuf,realFldNameGrpInDB[index]);
		}
	}
	return(num);
}

/*
功能：
	将对象定义的域名称转换为实际数据库中的名称
输入参数：
	idOfObject		对象名称
	defFldNameGrp		域定义时采用的名称
输出参数：
	realFldNameGrpInDB	实际数据库中的名称	
返回值：
	>=0：	返回转换的域名称数量
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldNameOfSpecObjectInNameGrp2(TUnionIDOfObject idOfObject,char defFldNameGrp[][128],int numOfFld,char realFldNameGrpInDB[][128])
{
	PUnionTableDef	pobjDef = NULL;
	
	if ((pobjDef = UnionFindTableDef(idOfObject)) == NULL)
	{
		UnionUserErrLog("in UnionConvertObjectFldNameOfSpecObjectInNameGrp:: UnionReadObjectDef [%s]!\n",idOfObject);
		return(errCodeParameter);
	}
	return(UnionConvertObjectFldNameInNameGrp(pobjDef,defFldNameGrp,numOfFld,realFldNameGrpInDB));
}

/*
功能	
	获得数据库域的最大长度
输入参数
	fldSize		域长度
输入出数
	无
返回值
	域的合法长度
*/
int UnionGetValidFldSizeOfDatabase(int fldSize)
{
	int	maxFldSize;
	
	if ((maxFldSize = UnionReadIntTypeRECVar("maxFldSizeOfDatabase")) <= 0)
		return(fldSize);
	if (fldSize >= maxFldSize)
		return(maxFldSize);
	else
		return(fldSize);
}


/*
功能：
	转换一个对象域的名称
输入参数：
	objectName	对象名称
	oriFldName	域名称
输出参数：
	defName		域定义时采用的名称
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldNameOfSpecObject(char *objectName,char *oriFldName,char *defName)
{
	PUnionTableDef	object = NULL;
	
	if ((object = UnionFindTableDef(objectName)) == NULL)
	{
		UnionUserErrLog("in UnionConvertObjectFldNameOfSpecObject:: UnionReadObjectDef [%s]!\n",objectName);
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	return(UnionConvertObjectFldName(&object,oriFldName,defName));
}

/*
功能：
	转换一个对象域的名称
输入参数：
	pobject：	指向对象定义的指针
	oriFldName	域名称
输出参数：
	defName		域定义时采用的名称
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
int UnionConvertObjectFldName(PUnionTableDef pobject,char *oriFldName,char *defName)
{
	int	fldNum;
	char	tmpBuf3[100];
	char	tmpBuf1[100];
	char	tmpBuf2[100];		
	char	*ptr;
	int	offset;
	char	prefix[100];
	
	if ((pobject == NULL) || (oriFldName == NULL) || (defName == NULL))
	{
		UnionUserErrLog("in UnionConvertObjectFldName:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	strcpy(tmpBuf3,oriFldName);
	UnionToUpperCase(tmpBuf3);
	if ((ptr = UnionGetPrefixOfDBField()) == NULL)
		strcpy(tmpBuf1,tmpBuf3);
	else
	{
		offset = strlen(ptr);
		strcpy(prefix,ptr);
		UnionToUpperCase(prefix);
		if (strncmp(tmpBuf3,prefix,offset) == 0)
			strcpy(tmpBuf1,tmpBuf3+offset);
		else
			strcpy(tmpBuf1,tmpBuf3);
	}
	for (fldNum = 0; fldNum < pobject->fieldNum; fldNum++)
	{
		strcpy(tmpBuf2,pobject->fieldDef[fldNum].fieldName);
		UnionToUpperCase(tmpBuf2);
		if (strcmp(tmpBuf1,tmpBuf2) == 0)
		{
			strcpy(defName,pobject->fieldDef[fldNum].fieldName);
			return(strlen(defName));
		}
	}
	UnionAuditLog("in UnionConvertObjectFldName:: fldName [%s] not valid fld of object [%s]\n",oriFldName,pobject->tableName);
	//strcpy(defName,oriFldName);
	//return(strlen(defName));
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNotExist));
}


/*
功能：
	获得一个对象的域定义的类型
输入参数：
	pobject：	指向对象定义的指针
	fldName		域名称
输出参数：
	fldTypeName	域的c语言类型
返回值：
	>=0：	类型
	<0：	失败，返回错误码
*/
int UnionGetTypeOfSpecFldFromObjectDef(PUnionTableDef pobject,char *fldName,char *fldTypeName)
{
	int	fldNum;
		
	if ((pobject == NULL) || (fldName == NULL))
	{
		UnionUserErrLog("in UnionGetTypeOfSpecFldFromObjectDef:: null pointer!\n");
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	}
	
	for (fldNum = 0; fldNum < pobject->fieldNum; fldNum++)
	{
		if (strcmp(fldName,pobject->fieldDef[fldNum].fieldName) == 0)
		{
			if (fldTypeName == NULL)
				return(pobject->fieldDef[fldNum].fieldType);
			switch (pobject->fieldDef[fldNum].fieldType)
			{
				case	conObjectFldType_String:
					if (pobject->fieldDef[fldNum].fieldSize > 1)
						strcpy(fldTypeName,"char *");
					else
						strcpy(fldTypeName,"char");
					break;
				case	conObjectFldType_Int:
					strcpy(fldTypeName,"int");
					break;
				case	conObjectFldType_Double:
					strcpy(fldTypeName,"double");
					break;
				case	conObjectFldType_Bit:
					strcpy(fldTypeName,"unsigned char *");
					break;
				case	conObjectFldType_Bool:
					strcpy(fldTypeName,"int");
					break;
				case	conObjectFldType_Long:
					strcpy(fldTypeName,"long");
					break;
				default:
					UnionUserErrLog("in UnionGetTypeOfSpecFldFromObjectDef:: invalid type [%d] of fld [%s] of object [%s]\n",
						pobject->fieldDef[fldNum].fieldType,fldName,pobject->tableName);
					return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldTypeInvalid));
			}
			return(pobject->fieldDef[fldNum].fieldType);
		}
	}
	UnionUserErrLog("in UnionGetTypeOfSpecFldFromObjectDef:: fldName [%s] not valid fld of object [%s]\n",fldName,pobject->tableName);
	return(UnionSetUserDefinedErrorCode(errCodeObjectMDL_FieldNotExist));
}

/*
功能	
	根据表定义，将关键字写成函数参数
输入参数
	pobjectDef	表定义
	fp		文件句柄
输入出数
	无
返回值
	>=0	成功
	<0	错误码
*/
int UnionGenerateRecPrimaryKeyAsVarFunToFp(PUnionTableDef pobjectDef,FILE *fp)
{
	FILE	*outFp = stdout;
	int	type;
	char	fldTypeName[256+1];
	int	fldNum;
	int	index;
	int	primaryKeyNum;
	char	primaryKeyGrp[6][40+1];
	
	if (pobjectDef == NULL)
		return(UnionSetUserDefinedErrorCode(errCodeParameter));
	if (fp != NULL)
		outFp = fp;
	
	primaryKeyNum = 0;
	for (index = 0; index < pobjectDef->fieldNum; index++)
	{       
		if((pobjectDef->fieldDef[index]).isPrimaryKey == 0)
			continue;
		strcpy(primaryKeyGrp[primaryKeyNum], (pobjectDef->fieldDef[index]).fieldName);
		primaryKeyNum++;
	}
	
	for (fldNum = 0; fldNum < primaryKeyNum; fldNum++)
	{
		if ((type = UnionGetTypeOfSpecFldFromObjectDef(pobjectDef,primaryKeyGrp[fldNum],fldTypeName)) < 0)
		{
			UnionUserErrLog("in UnionGenerateRecPrimaryKeyAsVarFunToFp:: UnionGetTypeOfSpecFldFromObjectDef!\n");
			return(type);
		}
		if (fldNum >= 1)
			fprintf(outFp,",");
		if (fldTypeName[strlen(fldTypeName)-1] == '*')
			fprintf(outFp,"%s%s",fldTypeName,primaryKeyGrp[fldNum]);
		else
			fprintf(outFp,"%s %s",fldTypeName,primaryKeyGrp[fldNum]);
	}
	return(0);
}


int UnionConvertTBLFldTypeIntoName(int type,char *typeName)
{
	switch (type)
	{
		case	conObjectFldType_String:
			strcpy(typeName,"char");
			break;
		case	conObjectFldType_Int:
			strcpy(typeName,"int");
			break;
		case	conObjectFldType_Double:
			strcpy(typeName,"double");
			break;
		case	conObjectFldType_Bit:
			strcpy(typeName,"unsigned char");
			break;
		case	conObjectFldType_Bool:
			strcpy(typeName,"int");
			break;
		case	conObjectFldType_Long:
			strcpy(typeName,"long");
			break;
		default:
			strcpy(typeName,"unknown");
			break;
	}
	return(strlen(typeName));
}


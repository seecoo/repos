//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionVarTypeDef.h"
#include "unionEnumDefFile.h"
#include "UnionStr.h"

/* 从指定文件读指定名称的枚举的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的枚举定义
返回值：
	>=0 	读出的枚举的大小
	<0	出错代码	
	
*/
int UnionReadEnumDefFromSpecFile(char *fileName,PUnionEnumDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadEnumDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadEnumDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// 空行
				continue;
			UnionSystemErrLog("in UnionReadEnumDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if (strncmp(lineStr,conEnumDeclareLineTag,strlen(conEnumDeclareLineTag)) == 0)
		{
			if ((ret = UnionReadEnumDeclareDefFromStr(lineStr,lineLen,&(pdef->declareDef))) < 0)
			{
				UnionUserErrLog("in UnionReadEnumDefFromSpecFile:: declare error in [%s]\n",fileName);
				fclose(fp);
				return(errCodeParameter);
			}
			continue;
		}
		if (pdef->constNum >= conMaxNumOfVarPerEnum)	// 域定义太多
		{
			UnionUserErrLog("in UnionReadEnumDefFromSpecFile:: too many constNum [%d] defined in [%s]\n",pdef->constNum,fileName);
			fclose(fp);
			return(errCodeParameter);
		}
		if ((ret = UnionReadConstDefFromStr(lineStr,lineLen,&(pdef->constGrp[pdef->constNum]))) < 0)
		{
			UnionAuditLog("in UnionReadEnumDefFromSpecFile:: UnionReadConstDefFromStr from [%s]\n",lineStr);
			//fclose(fp);
			//return(ret);
			continue;
		}
		pdef->constGrp[pdef->constNum].type = conVarTypeTagEnum;
		pdef->constNum += 1;
	}
	fclose(fp);
	return(0);
}

/* 从缺省定义文件读指定名称的枚举的定义
输入参数
	nameOfType	枚举类型
输出参数
	pdef	读出的枚举定义
返回值：
	>=0 	读出的枚举的大小
	<0	出错代码	
	
*/
int UnionReadEnumDefFromDefaultDefFile(char *nameOfType,PUnionEnumDef pdef)
{
	char       record[2560+1];
        char       varDefList[2048+1];
        char       condition[1024+1];
	char       varGrp[conMaxNumOfVarPerEnum][128+1];
        int        ret;
        int        i;

        memset(record, 0, sizeof record);
        memset(varDefList, 0, sizeof varDefList);
        memset(condition, 0, sizeof condition);
        memset(varGrp, 0, sizeof varGrp);

        strcpy(condition, "typeDefName=");
        strcat(condition, nameOfType);
        strcat(condition, "|");
        /* 从表里取出一条record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("complexTypeDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadEnumDefFromDefaultDefFile:: UnionReadEnumDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* 给枚举名称赋值 */
        strcpy(pdef->declareDef.name, nameOfType);

        /* 给枚举说明赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->declareDef.remark, sizeof (pdef->declareDef.remark));

        /* 取枚举域的数量 */
        UnionReadRecFldFromRecStr(record, strlen(record), "varDefList", varDefList, sizeof (varDefList));
        ret = UnionSeprateVarStrIntoVarGrp(varDefList, strlen(varDefList), ',', varGrp, conMaxNumOfVarPerEnum);
        if (ret < 0)
        {
                UnionUserErrLog("in UnionReadEnumDefFromDefaultDefFile:: UnionReadEnumDefFromDefaultDefFile!\n");
                return(ret);
        }
	pdef->constNum = ret;

        /* 取枚举的域 */
	for (i=0; i<pdef->constNum; i++)
	{
		memset(record, 0 ,sizeof record);
		memset(condition, 0 ,sizeof condition);

		strcpy(condition, "name=");
		strcat(condition, varGrp[i]);
		strcat(condition, "|");
		/* 从表里取出一条record */
		if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("constDef", condition, record, sizeof(record)))<0)

		{
			UnionUserErrLog("in UnionReadEnumDefFromDefaultDefFile:: UnionReadEnumDefFromDefaultDefFile!\n");
			return(ret);
		}
		strcpy(pdef->constGrp[i].name, varGrp[i]);

		UnionReadRecFldFromRecStr(record, strlen(record), "value", pdef->constGrp[i].value, sizeof (pdef->constGrp[i].value));

		UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "typeTag", &(pdef->constGrp[i].type) );

		UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->constGrp[i].remark, sizeof (pdef->constGrp[i].remark));
	}

        return(0);
}

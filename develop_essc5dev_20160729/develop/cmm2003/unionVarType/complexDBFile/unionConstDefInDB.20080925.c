// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionConstDefFile.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/*
功能	
	从缺省定义文件读取常量的值
输入参数
	constName	常量名称
	previousName	constName对应的值
输出参数
	constValue	常量值
返回值
	>=0		成功
	<0		出错代码
*/
int UnionReadConstValueFromDefaultDefFile(char *constName,char *previousName,char *constValue)
{
        char       record[1024+1];
        char       condition[1024+1];
        int        ret;
        memset(record, 0, sizeof record);
        memset(condition, 0, sizeof condition);

        strcpy(condition, "name=");
        strcat(condition, constName);
        strcat(condition, "|");
        /* 从表里取出一条record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("constDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadConstValueFromDefaultDefFile:: UnionReadConstValueFromDefaultDefFile!\n");
                return(ret);
        }

        return(UnionReadRecFldFromRecStr(record, strlen(record), "value", constValue, sizeof (constValue)));

}

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadConstDefFromSpecFile(char *fileName,PUnionConstDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadConstDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionAuditLog("in UnionReadConstDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeCDPMDL_ConstNotDefined);
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
			UnionSystemErrLog("in UnionReadConstDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadConstDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_ConstNotDefined);
}


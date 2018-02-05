// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVariableDefFile.h"
#include "unionVarTypeDef.h"
#include "UnionLog.h"

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadVariableDefFromSpecFile(char *fileName,PUnionVariableDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadVariableDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadVariableDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadVariableDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadVariableDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_GlobalVariableNotDefined);
}

/* 从缺省定义文件读指定名称的全局变量的定义
输入参数
	nameOfVariable	全局变量类型
输出参数
	pdef	读出的全局变量定义
返回值：
	>=0 	读出的全局变量的大小
	<0	出错代码	
	
*/
int UnionReadVariableDefFromDefaultDefFile(char *nameOfVariable,PUnionVariableDef pdef)
{
	char        condition[1024+1];
	char        record[2560+1];
        char        sizeList[1024+1];
        char        fldGrp[conMaxNumOfArrayDimision][128+1];
        int         ret;
        int         i;

        memset(record, 0, sizeof record);
        memset(condition, 0, sizeof condition);
        memset(sizeList, 0, sizeof sizeList);
        memset(fldGrp, 0, sizeof fldGrp);

        strcpy(condition, "name=");
        strcat(condition, nameOfVariable);
        strcat(condition, "|");
        /* 从表里取出一条record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("varDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadVariableDefFromDefaultDefFile:: UnionReadVariableDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* 给变量类型赋值 */
	UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->varDef.nameOfType, sizeof (pdef->varDef.nameOfType));

        /* 给域的属性(是否是指针)赋值 */
	UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->varDef.isPointer) );

        /* 给变量名称赋值 */
	strcpy(pdef->varDef.name, fldGrp[i]);

        /* 判断变量是否为数组 */
	UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
	ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', fldGrp, conMaxNumOfArrayDimision);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadVariableDefFromDefaultDefFile::UnionReadVariableDefFromDefaultDefFile!\n");
		return(ret);
	}
	if (ret == 0)      /* 不是数组 */
	{
		pdef->varDef.dimisionNum = 0;
	}
	else if (ret > 0)  /* 是数组 */
        {
		pdef->varDef.dimisionNum = ret;
		/* 给每一维大小赋值 */
		for(i=0; i<pdef->varDef.dimisionNum; i++)
		{
			strncpy(pdef->varDef.sizeOfDimision[i] , fldGrp[i], 64);
		}
	}

	/* 给变量缺省值赋值 */
	UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->defaultValue, sizeof (pdef->defaultValue));
        
        return(0);
}

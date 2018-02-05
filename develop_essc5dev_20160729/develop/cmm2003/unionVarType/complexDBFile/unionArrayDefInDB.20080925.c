//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionArrayDefFile.h"
#include "UnionStr.h"

/* 从缺省文件读指定名称的数组的类型定义
输入参数
	nameOfType	数组类型的名称
输出参数
	typeNameOfArray	数组类型的类型
返回值：
	>=0 	成功
	<0	出错代码	
	
*/
int UnionReadTypeOfArrayDefFromDefaultDefFile(char *nameOfType,char *typeNameOfArray)
{
        TUnionArrayDef  def;
        int                     ret;

        if ((ret = UnionReadArrayDefFromDefaultDefFile(nameOfType,&def)) < 0)
        {
                UnionUserErrLog("in UnionReadTypeOfArrayDefFromDefaultDefFile::UnionReadArrayDefFromDefaultDefFile!\n");
                return(ret);
        }
        strcpy(typeNameOfArray,def.nameOfType);
        return(0);
}

/* 从缺省定义文件读指定名称的数组的定义
输入参数
	nameOfType	变量类型
输出参数
	pdef	读出的数组定义
返回值：
	>=0 	读出的数组的大小
	<0	出错代码	
	
*/
int UnionReadArrayDefFromDefaultDefFile(char *nameOfType,PUnionArrayDef pdef)
{
	char 	   record[1024+1];
	char       condition[1024+1];
	char       sizeList[256+1];
	char       constValue[129+1];
	char       varGrp[conMaxNumOfArrayDimision][128+1];
	int	   ret;
	int	   i;

	memset(record, 0, sizeof record);
	memset(condition, 0, sizeof condition);
	memset(sizeList, 0, sizeof sizeList);
	memset(constValue, 0, sizeof constValue);
	memset(varGrp, 0, sizeof varGrp);
	
	strcpy(condition, "name=");
	strcat(condition, nameOfType);
	strcat(condition, "|");
	/* 从表里取出一条record */
	if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("arrayDef", condition, record, sizeof(record)))<0)

	{
		UnionUserErrLog("in UnionReadTypeOfArrayDefFromDefaultDefFile:: UnionReadArrayDefFromDefaultDefFile!\n");
		return(ret);
	}

        /* 给数组名赋值 */
        strcpy(pdef->name, nameOfType);

        /* 给数组类型赋值 */
	UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->nameOfType, sizeof (pdef->nameOfType));

        /* 给数组说明赋值 */
	UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->remark, sizeof (pdef->remark));

        /* 根据维数清单取出数组维数和每一维大小 */
	UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
        ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', varGrp, conMaxNumOfArrayDimision);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadTypeOfArrayDefFromDefaultDefFile:: UnionReadArrayDefFromDefaultDefFile!\n");
		return(ret);
	}

        /* 给数组维数赋值 */
	pdef->dimisionNum = ret;

        /* 给每一维大小赋值 */
	for(i=0;i<pdef->dimisionNum;i++)
	{
		memset(constValue, 0, sizeof constValue);
		
                ret = UnionReadConstValueFromDefaultDefFile(varGrp[i], "", constValue);
                if (ret < 0)
		{
			UnionUserErrLog("in UnionReadTypeOfArrayDefFromDefaultDefFile:: UnionReadConstValueFromDefaultDefFile!\n");
			return(ret);
		}
		strncpy(pdef->sizeOfDimision[i], constValue, 64);
	}

	return(0);
}

/* 从指定文件读指定名称的数组的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的数组定义
返回值：
	>=0 	读出的数组的大小
	<0	出错代码	
	
*/
int UnionReadArrayDefFromSpecFile(char *fileName,PUnionArrayDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadArrayDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadArrayDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadArrayDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if ((ret = UnionReadArrayDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		fclose(fp);
		return(0);
	}
	fclose(fp);
	return(errCodeCDPMDL_ArrayNotDefined);
}


//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionUnionDefFile.h"
#include "UnionStr.h"

/* 从缺省定义文件读指定名称的联合的定义
输入参数
	nameOfType	联合类型
输出参数
	pdef	读出的联合定义
返回值：
	>=0 	读出的联合的大小
	<0	出错代码	
	
*/
int UnionReadUnionDefFromDefaultDefFile(char *nameOfType,PUnionUnionDef pdef)
{
	char       record[2560+1];
        char       varDefList[2048+1];
        char       condition[1024+1];
        char       sizeList[1024+1];
	char       fldGrp[conMaxNumOfVarPerUnion][128+1];
	char       fldGrp1[conMaxNumOfArrayDimision][128+1];
        int        ret;
        int        i,j;

        memset(record, 0, sizeof record);
        memset(varDefList, 0, sizeof varDefList);
        memset(condition, 0, sizeof condition);
        memset(sizeList, 0, sizeof sizeList);
        memset(fldGrp, 0, sizeof fldGrp);
        memset(fldGrp1, 0, sizeof fldGrp1);

        strcpy(condition, "typeDefName=");
        strcat(condition, nameOfType);
        strcat(condition, "|");
        /* 从表里取出一条record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("complexTypeDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile:: UnionReadUnionDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* 给联合名称(TypeDef)赋值 */
        strncpy(pdef->declareDef.typeDefName, nameOfType, 48);

        /* 给联合名称(union后跟的)赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "privateName", pdef->declareDef.unionName, sizeof (pdef->declareDef.unionName));

        /* 给联合说明赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->declareDef.remark, sizeof (pdef->declareDef.remark));

        /* 取联合域的数量 */
        UnionReadRecFldFromRecStr(record, strlen(record), "varDefList", varDefList, sizeof (varDefList));
        ret = UnionSeprateVarStrIntoVarGrp(varDefList, strlen(varDefList), ',', fldGrp, conMaxNumOfVarPerUnion);
        if (ret < 0)
        {
                UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile:: UnionReadUnionDefFromDefaultDefFile!\n");
                return(ret);
        }
	pdef->fldNum = ret;

        /* 取联合的域 */
	for (i=0; i<pdef->fldNum; i++)
	{
		memset(record, 0 ,sizeof record);
		memset(condition, 0 ,sizeof condition);

		strcpy(condition, "typeDefName=");
		strcat(condition, nameOfType);
		strcat(condition, "|");
		strcat(condition, "name=");
		strcat(condition, fldGrp[i]);
		strcat(condition, "|");
		/* 从表里取出一条record */
		if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("typeVarDef", condition, record, sizeof(record)))<0)

		{
			UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
			return(ret);
		}

                /* 给域类型赋值 */
		UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->fldGrp[i].nameOfType, sizeof (pdef->fldGrp[i].nameOfType));
        
                /* 给域的属性(是否是指针)赋值 */
                UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->fldGrp[i].isPointer) );

                /* 给域的名称赋值 */
		strcpy(pdef->fldGrp[i].name, fldGrp[i]);

                /* 判断参数是否为数组 */
                UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
                ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', fldGrp1, conMaxNumOfArrayDimision);
                if (ret < 0)
                {
                        UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile::UnionSeprateVarStrIntoVarGrp!\n");
                        return(ret);
                }
                if (ret == 0)      /* 不是数组 */
                {
                        pdef->fldGrp[i].dimisionNum = 0;
                }
                else if (ret > 0)  /* 是数组 */
                {
                        pdef->fldGrp[i].dimisionNum = ret;
                        /* 给每一维大小赋值 */
                        for(j=0; j<pdef->fldGrp[i].dimisionNum; j++)
                        {
                                strncpy(pdef->fldGrp[i].sizeOfDimision[j] , fldGrp1[j], 64);
                        }
                }

                /* 给参数说明赋值 */
                UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->fldGrp[i].remark, sizeof (pdef->fldGrp[i].remark));
	}

        return(0);
}

/* 从指定文件读指定名称的联合的定义
输入参数
	fileName	文件名称
输出参数
	pdef	读出的联合定义
返回值：
	>=0 	读出的联合的大小
	<0	出错代码	
	
*/
int UnionReadUnionDefFromSpecFile(char *fileName,PUnionUnionDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	char	*ptr;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadUnionDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadUnionDefFromSpecFile:: fopen [%s] error!\n",fileName);
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
			UnionSystemErrLog("in UnionReadUnionDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))
			continue;
		if (strncmp(lineStr,conUnionDeclareLineTag,strlen(conUnionDeclareLineTag)) == 0)
		{
			if ((ret = UnionReadUnionDeclareDefFromStr(lineStr,lineLen,&(pdef->declareDef))) < 0)
			{
				UnionUserErrLog("in UnionReadUnionDefFromSpecFile:: declare error in [%s]\n",fileName);
				fclose(fp);
				return(errCodeParameter);
			}
			continue;
		}
		if (pdef->fldNum >= conMaxNumOfVarPerUnion)	// 域定义太多
		{
			UnionUserErrLog("in UnionReadUnionDefFromSpecFile:: too many fldNum [%d] defined in [%s]\n",pdef->fldNum,fileName);
			fclose(fp);
			return(errCodeParameter);
		}
		if ((ret = UnionReadVarDefFromStr(lineStr,lineLen,&(pdef->fldGrp[pdef->fldNum]))) < 0)
		{
			UnionAuditLog("in UnionReadUnionDefFromSpecFile:: UnionReadVarDefFromStr from [%s]\n",lineStr);
			//fclose(fp);
			//return(ret);
			continue;
		}
		pdef->fldNum += 1;
	}
	fclose(fp);
	return(0);
}


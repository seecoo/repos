//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionUnionDefFile.h"
#include "UnionStr.h"

/* ��ȱʡ�����ļ���ָ�����Ƶ����ϵĶ���
�������
	nameOfType	��������
�������
	pdef	���������϶���
����ֵ��
	>=0 	���������ϵĴ�С
	<0	�������	
	
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
        /* �ӱ���ȡ��һ��record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("complexTypeDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile:: UnionReadUnionDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* ����������(TypeDef)��ֵ */
        strncpy(pdef->declareDef.typeDefName, nameOfType, 48);

        /* ����������(union�����)��ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "privateName", pdef->declareDef.unionName, sizeof (pdef->declareDef.unionName));

        /* ������˵����ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->declareDef.remark, sizeof (pdef->declareDef.remark));

        /* ȡ����������� */
        UnionReadRecFldFromRecStr(record, strlen(record), "varDefList", varDefList, sizeof (varDefList));
        ret = UnionSeprateVarStrIntoVarGrp(varDefList, strlen(varDefList), ',', fldGrp, conMaxNumOfVarPerUnion);
        if (ret < 0)
        {
                UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile:: UnionReadUnionDefFromDefaultDefFile!\n");
                return(ret);
        }
	pdef->fldNum = ret;

        /* ȡ���ϵ��� */
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
		/* �ӱ���ȡ��һ��record */
		if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("typeVarDef", condition, record, sizeof(record)))<0)

		{
			UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile:: UnionSelectUniqueObjectRecordByPrimaryKey!\n");
			return(ret);
		}

                /* �������͸�ֵ */
		UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->fldGrp[i].nameOfType, sizeof (pdef->fldGrp[i].nameOfType));
        
                /* ���������(�Ƿ���ָ��)��ֵ */
                UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->fldGrp[i].isPointer) );

                /* ��������Ƹ�ֵ */
		strcpy(pdef->fldGrp[i].name, fldGrp[i]);

                /* �жϲ����Ƿ�Ϊ���� */
                UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
                ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', fldGrp1, conMaxNumOfArrayDimision);
                if (ret < 0)
                {
                        UnionUserErrLog("in UnionReadUnionDefFromDefaultDefFile::UnionSeprateVarStrIntoVarGrp!\n");
                        return(ret);
                }
                if (ret == 0)      /* �������� */
                {
                        pdef->fldGrp[i].dimisionNum = 0;
                }
                else if (ret > 0)  /* ������ */
                {
                        pdef->fldGrp[i].dimisionNum = ret;
                        /* ��ÿһά��С��ֵ */
                        for(j=0; j<pdef->fldGrp[i].dimisionNum; j++)
                        {
                                strncpy(pdef->fldGrp[i].sizeOfDimision[j] , fldGrp1[j], 64);
                        }
                }

                /* ������˵����ֵ */
                UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->fldGrp[i].remark, sizeof (pdef->fldGrp[i].remark));
	}

        return(0);
}

/* ��ָ���ļ���ָ�����Ƶ����ϵĶ���
�������
	fileName	�ļ�����
�������
	pdef	���������϶���
����ֵ��
	>=0 	���������ϵĴ�С
	<0	�������	
	
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
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// ����
				continue;
			UnionSystemErrLog("in UnionReadUnionDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
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
		if (pdef->fldNum >= conMaxNumOfVarPerUnion)	// ����̫��
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


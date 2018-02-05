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

/* ��ָ���ļ���ָ�����Ƶ�ö�ٵĶ���
�������
	fileName	�ļ�����
�������
	pdef	������ö�ٶ���
����ֵ��
	>=0 	������ö�ٵĴ�С
	<0	�������	
	
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
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// ����
				continue;
			UnionSystemErrLog("in UnionReadEnumDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
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
		if (pdef->constNum >= conMaxNumOfVarPerEnum)	// ����̫��
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

/* ��ȱʡ�����ļ���ָ�����Ƶ�ö�ٵĶ���
�������
	nameOfType	ö������
�������
	pdef	������ö�ٶ���
����ֵ��
	>=0 	������ö�ٵĴ�С
	<0	�������	
	
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
        /* �ӱ���ȡ��һ��record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("complexTypeDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadEnumDefFromDefaultDefFile:: UnionReadEnumDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* ��ö�����Ƹ�ֵ */
        strcpy(pdef->declareDef.name, nameOfType);

        /* ��ö��˵����ֵ */
        UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->declareDef.remark, sizeof (pdef->declareDef.remark));

        /* ȡö��������� */
        UnionReadRecFldFromRecStr(record, strlen(record), "varDefList", varDefList, sizeof (varDefList));
        ret = UnionSeprateVarStrIntoVarGrp(varDefList, strlen(varDefList), ',', varGrp, conMaxNumOfVarPerEnum);
        if (ret < 0)
        {
                UnionUserErrLog("in UnionReadEnumDefFromDefaultDefFile:: UnionReadEnumDefFromDefaultDefFile!\n");
                return(ret);
        }
	pdef->constNum = ret;

        /* ȡö�ٵ��� */
	for (i=0; i<pdef->constNum; i++)
	{
		memset(record, 0 ,sizeof record);
		memset(condition, 0 ,sizeof condition);

		strcpy(condition, "name=");
		strcat(condition, varGrp[i]);
		strcat(condition, "|");
		/* �ӱ���ȡ��һ��record */
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

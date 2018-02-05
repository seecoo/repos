//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionArrayDefFile.h"
#include "UnionStr.h"

/* ��ȱʡ�ļ���ָ�����Ƶ���������Ͷ���
�������
	nameOfType	�������͵�����
�������
	typeNameOfArray	�������͵�����
����ֵ��
	>=0 	�ɹ�
	<0	�������	
	
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

/* ��ȱʡ�����ļ���ָ�����Ƶ�����Ķ���
�������
	nameOfType	��������
�������
	pdef	���������鶨��
����ֵ��
	>=0 	����������Ĵ�С
	<0	�������	
	
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
	/* �ӱ���ȡ��һ��record */
	if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("arrayDef", condition, record, sizeof(record)))<0)

	{
		UnionUserErrLog("in UnionReadTypeOfArrayDefFromDefaultDefFile:: UnionReadArrayDefFromDefaultDefFile!\n");
		return(ret);
	}

        /* ����������ֵ */
        strcpy(pdef->name, nameOfType);

        /* ���������͸�ֵ */
	UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->nameOfType, sizeof (pdef->nameOfType));

        /* ������˵����ֵ */
	UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->remark, sizeof (pdef->remark));

        /* ����ά���嵥ȡ������ά����ÿһά��С */
	UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
        ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', varGrp, conMaxNumOfArrayDimision);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadTypeOfArrayDefFromDefaultDefFile:: UnionReadArrayDefFromDefaultDefFile!\n");
		return(ret);
	}

        /* ������ά����ֵ */
	pdef->dimisionNum = ret;

        /* ��ÿһά��С��ֵ */
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

/* ��ָ���ļ���ָ�����Ƶ�����Ķ���
�������
	fileName	�ļ�����
�������
	pdef	���������鶨��
����ֵ��
	>=0 	����������Ĵ�С
	<0	�������	
	
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
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// ����
				continue;
			UnionSystemErrLog("in UnionReadArrayDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
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


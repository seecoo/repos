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

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
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
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// ����
				continue;
			UnionSystemErrLog("in UnionReadVariableDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
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

/* ��ȱʡ�����ļ���ָ�����Ƶ�ȫ�ֱ����Ķ���
�������
	nameOfVariable	ȫ�ֱ�������
�������
	pdef	������ȫ�ֱ�������
����ֵ��
	>=0 	������ȫ�ֱ����Ĵ�С
	<0	�������	
	
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
        /* �ӱ���ȡ��һ��record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("varDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadVariableDefFromDefaultDefFile:: UnionReadVariableDefFromDefaultDefFile!\n");
                return(ret);
        }
        
        /* ���������͸�ֵ */
	UnionReadRecFldFromRecStr(record, strlen(record), "nameOfType", pdef->varDef.nameOfType, sizeof (pdef->varDef.nameOfType));

        /* ���������(�Ƿ���ָ��)��ֵ */
	UnionReadIntTypeRecFldFromRecStr( record, strlen(record), "isPointer", &(pdef->varDef.isPointer) );

        /* ���������Ƹ�ֵ */
	strcpy(pdef->varDef.name, fldGrp[i]);

        /* �жϱ����Ƿ�Ϊ���� */
	UnionReadRecFldFromRecStr(record, strlen(record), "sizeList", sizeList, sizeof (sizeList));
	ret = UnionSeprateVarStrIntoVarGrp(sizeList, strlen(sizeList), ',', fldGrp, conMaxNumOfArrayDimision);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionReadVariableDefFromDefaultDefFile::UnionReadVariableDefFromDefaultDefFile!\n");
		return(ret);
	}
	if (ret == 0)      /* �������� */
	{
		pdef->varDef.dimisionNum = 0;
	}
	else if (ret > 0)  /* ������ */
        {
		pdef->varDef.dimisionNum = ret;
		/* ��ÿһά��С��ֵ */
		for(i=0; i<pdef->varDef.dimisionNum; i++)
		{
			strncpy(pdef->varDef.sizeOfDimision[i] , fldGrp[i], 64);
		}
	}

	/* ������ȱʡֵ��ֵ */
	UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->defaultValue, sizeof (pdef->defaultValue));
        
        return(0);
}

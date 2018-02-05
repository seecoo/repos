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
����	
	��ȱʡ�����ļ���ȡ������ֵ
�������
	constName	��������
	previousName	constName��Ӧ��ֵ
�������
	constValue	����ֵ
����ֵ
	>=0		�ɹ�
	<0		�������
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
        /* �ӱ���ȡ��һ��record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("constDef", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadConstValueFromDefaultDefFile:: UnionReadConstValueFromDefaultDefFile!\n");
                return(ret);
        }

        return(UnionReadRecFldFromRecStr(record, strlen(record), "value", constValue, sizeof (constValue)));

}

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
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
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// ����
				continue;
			UnionSystemErrLog("in UnionReadConstDefFromSpecFile:: UnionReadOneLineFromTxtStr error in [%s]\n",fileName);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
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


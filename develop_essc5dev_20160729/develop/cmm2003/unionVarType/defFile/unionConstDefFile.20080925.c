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
	char		fileName[256+1];
	int		ret;
	TUnionConstDef	def;
	
	if ((constName == NULL) || (constValue == NULL))
		return(errCodeParameter);
	
	if (!UnionIsValidCProgramName(constName,strlen(constName)))
	{
		ret = errCodeCDPMDL_ConstNotDefined;
		goto notConstDefName;	
	}
	// ��ȱʡ�ļ��ж�ȡ��������
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfConstDef(constName,fileName);
	memset(&def,0,sizeof(def));
	if (((ret = UnionReadConstDefFromSpecFile(fileName,&def)) < 0) && (ret != errCodeCDPMDL_ConstNotDefined))
	{
		UnionUserErrLog("in UnionReadConstValueFromDefaultDefFile:: UnionReadConstDefFromSpecFile!\n");
		return(ret);
	}
notConstDefName:
	// û�ж���
	if (ret == errCodeCDPMDL_ConstNotDefined)
	{
		if ((previousName != NULL) && (strlen(previousName) != 0))	// ȡ֮ǰ������ֵ
		{
			strcpy(constValue,previousName);
			return(0);
		}
		else
			return(ret);
	}
	return(UnionReadConstValueFromDefaultDefFile(def.value,def.value,constValue));
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


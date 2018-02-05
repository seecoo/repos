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
	TUnionArrayDef	def;
	int			ret;
	
	if ((ret = UnionReadArrayDefFromDefaultDefFile(nameOfType,&def)) < 0)
	{
		UnionUserErrLog("in UnionReadTypeOfArrayDefFromDefaultDefFile:: UnionReadArrayDefFromDefaultDefFile!\n");
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
	char			fileName[256+1];

	UnionGetDefaultFileNameOfVarTypeDef(nameOfType,fileName);
	return(UnionReadArrayDefFromSpecFile(fileName,pdef));
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


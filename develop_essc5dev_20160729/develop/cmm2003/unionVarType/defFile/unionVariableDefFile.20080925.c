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
	char	fileName[512+1];
	int	ret;
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfVariableDef(nameOfVariable,fileName);
	if ((ret = UnionReadVariableDefFromSpecFile(fileName,pdef)) < 0)
	{
		UnionUserErrLog("in UnionReadVariableDefFromDefaultDefFile:: UnionReadVariableDefFromSpecFile [%s] from [%s]\n",nameOfVariable,fileName);
		return(ret);
	}
	return(ret);
}


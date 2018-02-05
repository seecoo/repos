// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionModuleDefFile.h"
#include "UnionLog.h"

/* ��ָ���ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	fileName	�ļ�����
	nameOfModule	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadModuleDefFromSpecFile(char *fileName,char *nameOfModule,PUnionModuleDef pdef)
{
	char	lineStr[1024+1];
	int	lineLen;
	int	ret;
	FILE	*fp;
	
	if ((fileName == NULL) || (pdef == NULL))
	{
		UnionUserErrLog("in UnionReadModuleDefFromSpecFile:: parameter!\n");
		return(errCodeParameter);
	}
	if ((fileName == NULL) || (strlen(fileName) == 0) || (strcmp(fileName,"null") == 0))
		return(UnionReadModuleDefFromDefaultFile(nameOfModule,pdef));
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadModuleDefFromSpecFile:: fopen [%s] error!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// ����
				continue;
			UnionSystemErrLog("in UnionReadModuleDefFromSpecFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// ����
			continue;
		if ((ret = UnionReadModuleDefFromStr(lineStr,lineLen,pdef)) < 0)
			continue;
		if (strcmp(nameOfModule,pdef->nameOfModule) == 0)
		{
			fclose(fp);
			return(0);
		}
	}
	fclose(fp);
	UnionUserErrLog("in UnionReadModuleDefFromSpecFile:: [%s] not defined in [%s]\n",nameOfModule,fileName);
	return(errCodeCDPMDL_ModuleNotDefined);
}

/* ��ȱʡ�ļ���ָ�����Ƶ����ͱ�ʶ�Ķ���
�������
	nameOfModule	ָ��������
�������
	pdef	���������ͱ�ʶ����
����ֵ��
	>=0 	���������ͱ�ʶ�Ĵ�С
	<0	�������	
	
*/
int UnionReadModuleDefFromDefaultFile(char *nameOfModule,PUnionModuleDef pdef)
{
	char	fileName[256];
	
	UnionGetDefaultFileNameOfModuleDef(nameOfModule,fileName);
	return(UnionReadModuleDefFromSpecFile(fileName,nameOfModule,pdef));
}


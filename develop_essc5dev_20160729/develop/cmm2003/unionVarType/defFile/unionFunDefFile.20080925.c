// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionVarDef.h"
#include "unionFunDefFile.h"
#include "UnionLog.h"

/*
����	
	��ȱʡ�����ļ���ȡһ����������
�������
	funName		��������
�������
	pdef		�����ĺ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadFunDefFromDefaultDefFile(char *funName,PUnionFunDef pdef)
{
	char	fileName[512+1];
	
	memset(fileName,0,sizeof(fileName));
	UnionGetDefaultFileNameOfFunDef(funName,fileName);
	return(UnionReadFunDefFromSpecFile(fileName,pdef));
}

/*
����	
	���ļ���ȡһ����������
�������
	fileName	�ļ�����
�������
	pdef		�����ĺ�������
����ֵ
	>=0		�ɹ�
	<0		�������
*/
int UnionReadFunDefFromSpecFile(char *fileName,PUnionFunDef pdef)
{
	FILE			*fp;
	int			lineLen;
	char			lineStr[1024+1];
	int			lineNum = 0;
	int			ret;
	
	if ((pdef == NULL) || (fileName == NULL))
	{
		UnionUserErrLog("in UnionReadFunDefFromSpecFile:: null parameter!\n");
		return(errCodeParameter);
	}
	
	// �򿪶����ļ�
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionReadFunDefFromSpecFile:: fopen [%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// ���ļ��ж�ȡ����
	memset(pdef,0,sizeof(*pdef));
	while (!feof(fp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeEnviMDL_NullLine))
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadOneLineFromTxtFile [%s] !lineNum = [%04d] ret = [%d]\n",fileName,lineNum,lineLen);
			ret = lineLen;
			goto errExit;
		}
		lineNum++;
		if (lineLen == 0)	// ����
			continue;
		if (UnionIsUnixShellRemarkLine(lineStr))	// ע����
			continue;
		// ��������
		if (strncmp(lineStr,conFunDefTagFunName,strlen(conFunDefTagFunName)) == 0)
		{
			if ((ret = UnionReadRecFldFromRecStr(lineStr,lineLen,conFunDefTagFunName,pdef->funName,sizeof(pdef->funName))) < 0)
			{
				UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadIntTypeRecFldFromRecStr [%s]! lineNum = [%04d]\n",conFunDefTagFunName,lineNum);
				goto errExit;
			}
			UnionReadRecFldFromRecStr(lineStr,lineLen,conFunDefTagFunRemark,pdef->remark,sizeof(pdef->remark));
			continue;
		}
		// ����ֵ����
		if (strncmp(lineStr,conFunDefTagReturnType,strlen(conFunDefTagReturnType)) == 0)
		{
			if ((ret = UnionReadVarDeclareTypeDefFromStr(lineStr+strlen(conFunDefTagReturnType),lineLen-strlen(conFunDefTagReturnType),&(pdef->returnType))) < 0)
			{
				UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadVarDeclareTypeDefFromStr [%s]! lineNum = [%04d]\n",conFunDefTagReturnType,lineNum);
				goto errExit;
			}
			continue;
		}
		// ��������
		if (strncmp(lineStr,conFunDefTagVarDef,strlen(conFunDefTagVarDef)) != 0)
			continue;
		if (pdef->varNum >= conMaxNumOfVarPerFun)
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: too many var [%d] defined for fun [%s]! lineNum = [%04d]\n",pdef->varNum,pdef->funName,lineNum);
			ret = errCodeCDPMDL_TooManyFunVarDefined;
			goto errExit;
		}
		// ��������
		if ((ret = UnionReadVarDefFromStr(lineStr+strlen(conFunDefTagVarDef),lineLen-strlen(conFunDefTagVarDef),&(pdef->varGrp[pdef->varNum].varDef))) < 0)
		{
			UnionUserErrLog("in UnionReadFunDefFromSpecFile:: UnionReadVarDefFromStr [%s]! lineNum = [%04d]\n",conFunDefTagReturnType,lineNum);
			goto errExit;
		}
		// �������
		if ((ret = UnionReadIntTypeRecFldFromRecStr(lineStr+strlen(conFunDefTagVarDef),lineLen-strlen(conFunDefTagVarDef),conFunDefTagIsOutputVar,&(pdef->varGrp[pdef->varNum].isOutput))) < 0)
			pdef->varGrp[pdef->varNum].isOutput = 0;
		pdef->varNum += 1;
	}
	ret = 0;
errExit:
	fclose(fp);
	return(ret);
}


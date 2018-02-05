//	Wolfgang Wang
//	2008/9/25
//	Version 1.0

#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionLog.h"
#include "unionStructDef.h"
#include "UnionStr.h"

/* ��ָ���ļ���һ��c���Ե���Ч������
�������
	fp		�ļ����
	maxNumOfLine	�ɶ������������
�������
	cprogramLine	������
	lineNum		���������ļ��ĵڼ���
����ֵ��
	>=0 	����������Ŀ��0��ʾ�ļ�������
	<0	�������	
	
*/
int UnionReadCProgramLineFromFile(FILE *fp,char cprogramLine[][1024+1],int maxNumOfLine,int *lineNum)
{
	char	lineStr[1024+1];
	char	tmpStr[1024+1];
	int	ret;
	int	lineLen;
	int	isRemarkLines = 0;
	char	*ptr;
	char	*cStr;
	int	lenOfCStr;
	int	cStrNum = 0;
	
	while (!feof(fp))
	{
		// ���ļ��ж�ȡһ��
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneLineFromTxtStr(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if (lineLen == errCodeEnviMDL_NullLine)	// ����
				continue;
			UnionSystemErrLog("in UnionReadCProgramLineFromFile:: UnionReadOneLineFromTxtStr error! lineIndex = [%d]\n",*lineNum);
			return(lineLen);
		}
		*lineNum += 1;
		if (lineLen == 0)	// ����
			continue;
		cStr = lineStr;
		lenOfCStr = lineLen;
loop:
		// ȥ������ո��tab
		if ((lenOfCStr = UnionFilterRubbisBlankAndTab(cStr,lenOfCStr,tmpStr,sizeof(tmpStr))) <= 0)	// ����
			continue;
		memcpy(cStr,tmpStr,lenOfCStr);
		cStr[lenOfCStr] = 0;
		if (strncmp(cStr,"//",2) == 0) // ע����
			continue;
		if (isRemarkLines)	// ������ע�ͱ�ʶ���ڵ���
		{
			if ((ptr = strstr(cStr,"*/")) == NULL)	// δ����ע�ͽ�����ʶ������ע������
				continue;
			// ���а�����ע�ͽ�����ʶ
			isRemarkLines = 0;
			cStr = ptr + 2;
			lenOfCStr -= 2;
			goto loop;	// �ж�ע�ͽ�����־���Ƿ��г�����
		}
		// �����ǳ�����
		if ((ptr = strstr(cStr,"/*")) != NULL)	// ���а�����ע�Ϳ�ʼ��־
		{
			isRemarkLines = 1;
			*ptr = 0;
			if ((lenOfCStr = strlen(cStr)) <= 0) // ע�Ϳ�ʼ��־���п�ʼλ��
				continue;
		}
		if ((ptr = strstr(cStr,"//")) != NULL)	// ���а�������ע��
		{
			*ptr = 0;
			if ((lenOfCStr = strlen(cStr)) <= 0) // ����Ϊ��
				continue;
		}		
		// �ǳ�����
		if (cStrNum >= maxNumOfLine)	// �Ѷ������㹻�������
		{
			UnionUserErrLog("in UnionReadCProgramLineFromFile:: too many cPhase defined in line [%d]\n",*lineNum); 
			return(errCodeSmallBuffer);
		}
		strcpy(cprogramLine[cStrNum],cStr);
		cStrNum++;
		// �ó������в�����ע����Ϣ
		if (!isRemarkLines)
			return(cStrNum);
		// �ó������а���ע����Ϣ
		cStr = cStr + lenOfCStr + 2;
		lenOfCStr = strlen(cStr);
		goto loop;
	}
	return(0);
}

/* ȥ��һ�������е�ע����
�������
	oriFileName	Դ�ļ���
	desFileName	Ŀ���ļ���
�������
	��
����ֵ��
	>=0 	������������
	<0	�������	
	
*/
int UnionFilterRemarkFromCProgramFile(char *oriFileName,char *desFileName)
{
	FILE	*inFp;
	int	oriLineNum = 0,desLineNum = 0;
	char	tmpBuf[10][1024+1];
	int	ret;
	int	i;
	FILE	*outFp = stdout;
	
	if ((desFileName != NULL) && (strlen(desFileName) != 0) && (strcmp(desFileName,"null") != 0))
	{
		if ((outFp = fopen(desFileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionFilterRemarkFromCProgramFile:: fopen [%s]\n",desFileName);
			return(errCodeUseOSErrCode);
		}
	}
		
	if ((inFp = fopen(oriFileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionFilterRemarkFromCProgramFile:: fopen [%s]!\n",oriFileName);
		return(errCodeUseOSErrCode);
	}
	while (!feof(inFp))
	{
		if ((ret = UnionReadCProgramLineFromFile(inFp,tmpBuf,10,&oriLineNum)) <= 0)
			break;
		desLineNum += ret;
		for (i = 0; i < ret; i++)
		{
			fprintf(outFp,"%s\n",tmpBuf[i]);
		}
	}
	fclose(inFp);
	fclose(outFp);
	return(ret);
}

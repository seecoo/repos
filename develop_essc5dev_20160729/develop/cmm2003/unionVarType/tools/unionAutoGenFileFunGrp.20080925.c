//	Wolfgang Wang
//	2008/2/25

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "unionAutoGenFileFunGrp.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionDefaultVarTypeDefDir.h"

/*
����	
	��ͷ�ļ�д�뵽���Գ�����
�������
	incConfFile	������Ҫ������ͷ�ļ����ļ�
	fp		���Գ�����
�������
	��
����ֵ
	ͷ�ļ�������
*/
int UnionAddIncludeFileToFile(char *incConfFile,FILE *fp)
{
	FILE		*includeFp;
	char		fileName[256+1];
	int		includeNum = 0;
	char		lineStr[1024+1];
	int		lineLen;
	int		lineNum = 0;
	
	fprintf(fp,"#include <stdio.h>\n");
	fprintf(fp,"#include <string.h>\n");
	fprintf(fp,"#include <stdlib.h>\n");
	fprintf(fp,"\n");
	includeNum = 3;
	
	if ((incConfFile == NULL) || (strlen(incConfFile) == 0) || (strcmp(incConfFile,"null") == 0))
		return(includeNum);
	
	UnionGetFullNameOfIncludeFileConf(incConfFile,fileName);	
	if ((includeFp = fopen(fileName,"r")) == NULL)
	{
		UnionLog("in UnionAddIncludeFileToFile:: no specical include files defined in [%s]!\n",fileName);
		return(includeNum);
	}
	while (!feof(includeFp))
	{
		memset(lineStr,0,sizeof(lineStr));
		if (((lineLen = UnionReadOneDataLineFromTxtFile(includeFp,lineStr,sizeof(lineStr))) < 0) && (lineLen != errCodeFileEnd))
		{
			UnionUserErrLog("in UnionAddIncludeFileToFile:: UnionReadOneDataLineFromTxtFile! lineNum = [%04d]\n",lineNum);
			continue;
		}
		if (lineLen <= 0)
			continue;
		fprintf(fp,"#include \"%s\"\n",lineStr);
		includeNum++;
	}
	fclose(includeFp);
	return(includeNum);
}



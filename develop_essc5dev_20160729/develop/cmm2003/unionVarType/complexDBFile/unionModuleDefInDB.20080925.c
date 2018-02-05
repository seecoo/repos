// wolfang wang
// 2008/10/3

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionModuleDefFile.h"
#include "UnionLog.h"

/* 从指定文件读指定名称的类型标识的定义
输入参数
	fileName	文件名称
	nameOfModule	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
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
		// 从文件中读取一行
		memset(lineStr,0,sizeof(lineStr));
		if ((lineLen = UnionReadOneDataLineFromTxtFile(fp,lineStr,sizeof(lineStr))) < 0)
		{
			if ((lineLen == errCodeEnviMDL_NullLine) || (lineLen == errCodeFileEnd))	// 空行
				continue;
			UnionSystemErrLog("in UnionReadModuleDefFromSpecFile:: UnionReadOneDataLineFromTxtFile error in [%s]\n",fileName);
			fclose(fp);
			return(lineLen);
		}
		if (lineLen == 0)	// 空行
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

/* 从缺省文件读指定名称的类型标识的定义
输入参数
	nameOfModule	指定的类型
输出参数
	pdef	读出的类型标识定义
返回值：
	>=0 	读出的类型标识的大小
	<0	出错代码	
	
*/
int UnionReadModuleDefFromDefaultFile(char *nameOfModule,PUnionModuleDef pdef)
{
	char       record[1024+1];
        char       condition[1024+1];
        int        ret;

        memset(record, 0, sizeof record);
        memset(condition, 0, sizeof condition);

        strcpy(condition, "nameOfModule=");
        strcat(condition, nameOfModule);
        strcat(condition, "|");
        /* 从表里取出一条record */
        if ((ret = UnionSelectUniqueObjectRecordByPrimaryKey("module", condition, record, sizeof(record)))<0)

        {
                UnionUserErrLog("in UnionReadModuleDefFromDefaultFile:: UnionReadModuleDefFromDefaultFile!\n");
                return(ret);
        }
        
        /* 给模块名称赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "nameOfModule", pdef->nameOfModule, sizeof (pdef->nameOfModule));

        /* 给模块开发目录赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "devDir", pdef->devDir, sizeof (pdef->devDir));

        /* 给模块头文件赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "incFileConf", pdef->incFileConf, sizeof (pdef->incFileConf));

        /* 给模块说明赋值 */
        UnionReadRecFldFromRecStr(record, strlen(record), "remark", pdef->remark, sizeof (pdef->remark));
        
        return(0);

}

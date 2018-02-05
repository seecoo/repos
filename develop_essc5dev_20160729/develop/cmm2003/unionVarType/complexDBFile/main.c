#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "testincl.h"

int
main()
{
    int   ret,i;
    char  constValue[129];
    TUnionVariableDef tdef;
    
    memset(&tdef,0,sizeof tdef);
    ret = UnionReadVariableDefFromDefaultDefFile("Var4",&tdef);
    if (ret < 0)
    {
	    printf("UnionReadModuleDefFromDefaultFile err!\n");
	    return -1;
    }
    /***
    printf("nameOfProgram=[%s]\n", tdef.nameOfProgram);
    printf("nameOfModule=[%s]\n", tdef.nameOfModule);
    printf("version=[%s]\n", tdef.version);
    printf("remark=[%s]\n", tdef.remark);
    ***/
    return 0;
}

#include <stdio.h>
#include <string.h>

int main()
{
        int ret;
        char pk[1024+1];
	char vk[1024+1];
        memset(pk,0,sizeof( pk));
	memset(vk,0,sizeof (vk));
	ret = UnionGenRSAPair(pk,vk);
	printf("ret=%d,pk=%s,vk=%s",ret,pk,vk);	 

}

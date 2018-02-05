int  UnionMD5(unsigned char *pData, unsigned long cbData, unsigned char *pDigest);
int UnionGenRSAPair(char *pk,char *vk);
int UnionEncByVK(char *input,char *output,char *vk);
int UnionDecByPK(char *input,char *output,char *pk);

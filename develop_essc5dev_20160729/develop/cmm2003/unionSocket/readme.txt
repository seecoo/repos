// 与1.2版相比，将函数：
/*			int UnionNewTCPIPConnection(int scksvr)
			int UnionInitializeTCPIPServer(int port)
   移到UnionSocket1.3.c中。
*/

// 与1.3版相比，在UnionTCPIPServer中，创建了一个子进程 */

/* 本版本在1.4版本基础上改写 */
// 2002/2/18, 将UnionTCPIPServer升级了。2.x以前版本，带两个参数，2.x版本带三个参数，第三个参数为一个函数指针。


// 2003/02/26，王纯军，侦听进程的注册名有问题，对之进行了改进。
/*
	socket通讯中，若客户端和服务器建立长链接，服务器程序关闭链接，终止进程后，
	若客户端没有关闭该链接，则不能重新启动服务器程序，bind函数调用失败。
	在服务器程序的accept函数调用之后增加语句，即使客户端没有关闭链接，
	服务器程序也可以主动关闭socket链接，重新启动。
*/


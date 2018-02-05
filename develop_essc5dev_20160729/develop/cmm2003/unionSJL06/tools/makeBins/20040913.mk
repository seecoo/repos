HOMEDIR = $(HOME)/unionSJL06/
LIBDIR = $(HOMEDIR)lib/
BINDIR = $(HOMEDIR)bin/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

TESTBINDIR = $(HOME)/bin/

all:	genProductTime			\
	checkVK.20040913		\
	backupVK.20040913		\
	restoreVK.20040913		\
	finalProducts			\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

commLibs =	$(CMM2003LIBDIR)sjl062.0.a				\
		$(CMM2003LIBDIR)sjl06CmdForJK.IC.RSA.2.0.a		\
		$(CMM2003LIBDIR)sjl06Protocol2.0.a			\
		$(CMM2003LIBDIR)UnionSocket3.0.a			\
		$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a

# Bins
libs0110 = 	$(CMM2003LIBDIR)checkVK.20040913.a			\
		$(commLibs)
checkVK.20040913:	$(null)
		cc -o checkVK.20040913 $(libs0110) -lsocket

libs0210 = 	$(CMM2003LIBDIR)backupVK.20040913.a			\
		$(commLibs)
backupVK.20040913:	$(null)
		cc -o backupVK.20040913 $(libs0210) -lsocket

libs0310 = 	$(CMM2003LIBDIR)restoreVK.20040913.a			\
		$(commLibs)
restoreVK.20040913:	$(null)
		cc -o restoreVK.20040913 $(libs0310) -lsocket

finalProducts:	$(null)
	#bins
		cp checkVK.20040913		$(BINLIBS)
		mv checkVK.20040913		$(TESTBINDIR)checkVK
		cp backupVK.20040913		$(BINLIBS)
		mv backupVK.20040913		$(TESTBINDIR)backupVK
		cp restoreVK.20040913		$(BINLIBS)
		mv restoreVK.20040913		$(TESTBINDIR)restoreVK

rmTmpFiles:
		rm *.a

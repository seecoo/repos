
BINLIBS = $(UNIONLIBDIR)/binLibs

HOMEDIR = $(HOME)/unionFileTransfer/
TESTBINDIR=$(HOME)/bin/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX

all:	genProductTime				\
	unionFileSender.20040723		\
	unionFileReceiver.20040723		\
	finalProducts				\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

commLibs = 	$(CMM2003LIBDIR)unionFileTransfer.20040723.a	\
		$(CMM2003LIBDIR)unionTmpFileDir.20040723.a		\
		$(CMM2003LIBDIR)UnionSocket3.0.a			\
		$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003LIBDIR)unionMonitor1.0.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a			\
		-lsocket
# bins
libs0132 =	$(CMM2003LIBDIR)unionFileSender.20040723.a		\
		$(commLibs)
unionFileSender.20040723:	$(null)
		cc -o unionFileSender.20040723 $(libs0132)

# 02 taskMon
libs0232 =	$(CMM2003LIBDIR)unionFileReceiver.20040723.a	\
		$(commLibs)
unionFileReceiver.20040723:	$(null)
		cc -o unionFileReceiver.20040723 $(libs0232) -lcurses

finalProducts:	$(null)
		cp	unionFileSender.20040723		$(TESTBINDIR)fileSender
		cp	unionFileReceiver.20040723		$(TESTBINDIR)fileReceiver
		mv	unionFileSender.20040723		$(BINLIBS)
		mv	unionFileReceiver.20040723		$(BINLIBS)

rmTmpFiles:	$(null)
		rm	unionProductGenerateTime.a

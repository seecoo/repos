HOMEDIR = $(HOME)/unionSJL06/
LIBDIR = $(HOMEDIR)lib/
BINDIR = $(HOMEDIR)bin/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	genProductTime			\
	genKeyComponent2.1		\
	binProducts			\
	finalProducts			\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

# Bins
libs0221 = 	$(CMM2003LIBDIR)genKeyComponent2.1.a		\
		$(CMM2003LIBDIR)sjl061.0.a				\
		$(CMM2003LIBDIR)sjl06CmdForNewRacal.1.0.a		\
		$(CMM2003LIBDIR)sjl06Protocol1.0.a			\
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
		$(CMM2003LIBDIR)unionDesKey2.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion2.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
genKeyComponent2.1:	$(null)
		cc -o genKeyComponent2.1 $(libs0221) -lsocket

binProducts:	$(null)
		cp genKeyComponent2.1			$(BINLIBS)
		mv genKeyComponent2.1 			$(BINDIR)

finalProducts:	$(null)
	#bins
		cp $(BINDIR)genKeyComponent2.1		$(CMM2003BINDIR)genKeyComponent

rmTmpFiles:
		rm *.a


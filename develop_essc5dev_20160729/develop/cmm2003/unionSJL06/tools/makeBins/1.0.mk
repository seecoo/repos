HOMEDIR = $(HOME)/unionSJL06/
LIBDIR = $(HOMEDIR)lib/
BINDIR = $(HOMEDIR)bin/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	genProductTime			\
	authRacal1.0			\
	readBMK1.0			\
	genKeyComponent1.0		\
	readTransKey1.0			\
	binProducts			\
	finalProducts			\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

# Bins
libs0110 = 	$(CMM2003LIBDIR)authRacal1.0.a			\
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
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
authRacal1.0:	$(null)
		cc -o authRacal1.0 $(libs0110) -lsocket

libs0210 = 	$(CMM2003LIBDIR)genKeyComponent1.0.a		\
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
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
genKeyComponent1.0:	$(null)
		cc -o genKeyComponent1.0 $(libs0210) -lsocket

libs0310 = 	$(CMM2003LIBDIR)readBMK1.0.a			\
		$(CMM2003LIBDIR)sjl061.0.a				\
		$(CMM2003LIBDIR)sjl06CmdForJK.IC.RSA.1.0.a		\
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
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
readBMK1.0:	$(null)
		cc -o readBMK1.0 $(libs0310) -lsocket

libs0410 = 	$(CMM2003LIBDIR)readTransKey1.0.a			\
		$(CMM2003LIBDIR)sjl061.0.a				\
		$(CMM2003LIBDIR)sjl06CmdForJK.IC.RSA.1.0.a		\
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
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
readTransKey1.0:	$(null)
		cc -o readTransKey1.0 $(libs0410) -lsocket

binProducts:	$(null)
		cp authRacal1.0				$(BINLIBS)
		mv authRacal1.0 			$(BINDIR)
		cp genKeyComponent1.0			$(BINLIBS)
		mv genKeyComponent1.0 			$(BINDIR)
		cp readBMK1.0				$(BINLIBS)
		mv readBMK1.0 				$(BINDIR)
		cp readTransKey1.0			$(BINLIBS)
		mv readTransKey1.0 			$(BINDIR)

finalProducts:	$(null)
	#bins
		cp $(BINDIR)authRacal1.0		$(CMM2003BINDIR)authRacal
		cp $(BINDIR)genKeyComponent1.0		$(CMM2003BINDIR)genKeyComponent
		cp $(BINDIR)readBMK1.0			$(CMM2003BINDIR)readBMK
		cp $(BINDIR)readTransKey1.0		$(CMM2003BINDIR)readTransKey

rmTmpFiles:
		rm *.a

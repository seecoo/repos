OSName := $(shell uname)

include dirAndLib.def

CMMBINDIR=$(subst $(UNIONREC),$(UNIONLIBDIR)/bin/,$(TESTBINDIR))

# Ҫ���ɵĳ����б�
all:	allBins

allBins:
ifeq ($(OSName),AIX)
	cp $(CMMBINDIR)* $(TESTBINDIR)
else
	cp -uv $(CMMBINDIR)* $(TESTBINDIR)
endif

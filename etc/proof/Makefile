.PHONY: all install

all: KaliVeda.par install

KaliVeda.par: KaliVeda/PROOF-INF/BUILD.sh KaliVeda/PROOF-INF/SETUP.C
	@tar zcf $@ KaliVeda

install:
	@-cp KaliVeda.par $(KVINSTALLDIR)/KVFiles/

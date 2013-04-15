# Makefile for all LyX-generated HTML documentation
# To add a new module "TOTO":
#  - make a subdirectory in this directory called "TOTO"
#  - put in it the '.lyx' file and any image files
#  - copy a Makefile from any existing subdirectory into TOTO
#  - add "TOTO" to the MODULES list in this Makefile
# After installation your doc will be available in
#   $KVROOT/KaliVedaDoc/TOTODoc/TOTO.html
#   $KVROOT/KaliVedaDoc/(VERSION)/TOTODoc/TOTO.html
# where (VERSION) is the current version of KaliVeda

.PHONY:all clean install

MODULES = KVedaLoss KVSimDirGUI KVTreeAnalyzer

all:
	@for module in $(MODULES) ; do \
		cd $(PWD)/$$module && $(MAKE) ; \
	done

clean:
	@for module in $(MODULES) ; do \
		cd $(PWD)/$$module && $(MAKE) clean ; \
	done

install:
	@for module in $(MODULES) ; do \
		cd $(PWD)/$$module && $(MAKE) install ; \
	done

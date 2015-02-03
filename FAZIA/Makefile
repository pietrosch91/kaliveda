PROJECT = FAZIA
MODULES = analysis geometry identification db

.PHONY: all install clean $(MODULES)

all: $(MODULES)

install:
	@for module in $(MODULES); do \
		makefile=Makefile.$$module; \
		[ ! -f $$makefile ] && makefile=Makefile.$(PROJECT); \
		make -f $$makefile MODULE=$$module install ; if [ "$$?" -ne 0 ] ; then break ; fi ; \
	done

clean:
	@for module in $(MODULES); do \
		makefile=Makefile.$$module; \
		[ ! -f $$makefile ] && makefile=Makefile.$(PROJECT); \
		make -f $$makefile MODULE=$$module clean ; if [ "$$?" -ne 0 ] ; then break ; fi ; \
	done

analysis: db geometry
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
geometry:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
identification:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
db:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"

MODULE=
PROJECT=
MOD_DEPS=

SOURCES=$(wildcard $(MODULE)/*.cpp)
HEADERS=$(wildcard $(MODULE)/*.h) $(MODULE)/$(PROJECT)$(MODULE)LinkDef.h
	
all:
	@module="$(MODULE)"; \
	echo `tput bold``tput setaf 4`"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% BUILDING MODULE $(PROJECT)/$$module..."`tput sgr0`; \
	include_paths="$(patsubst %,-I%,$(MOD_DEPS))"; \
	link_libraries="$(patsubst %,lib$(PROJECT)%.so,$(MOD_DEPS))"; \
	makefile=Makefile.$$module; \
	[ ! -f $$makefile ] && makefile=Makefile.$(PROJECT); \
	make -f $$makefile MODULE=$$module INCLUDE_PATHS="$$include_paths" LINK_LIBRARIES="$$link_libraries"; if [ "$$?" -ne 0 ] ; then break ; fi ; \
	echo `tput bold``tput setaf 4`"++++++++++++++++++++++++++++++++++++++++ FINISHED MODULE $(PROJECT)/$$module"`tput sgr0`; \

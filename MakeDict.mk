# generate CINT dictionary in subdir MODULE
# invoke using: 
#       make -f MakeDict.mk MODULE=base LIBNAME=KVMultiDet INCLUDE="[include paths]" EXCLUDE="[*.h to exclude]" DICT_COMPILE_FLAGS="[flags for compiling dictionary]"
# in order to generate the dictionary KVMultiDetbaseDict.cxx/h
# for subdirectory base/ using base/KVMultiDetbaseLinkDef.h
MODULE = 
LIBNAME = 
EXCLUDE =
INCLUDE =
RLIBMAPDEPS =
DICT_COMPILE_FLAGS =
LINKDEF = $(MODULE)/$(LIBNAME)$(MODULE)LinkDef.h
DICT_CXX = $(LIBNAME)$(MODULE)Dict.cpp
DICT_OBJ = $(patsubst %.cpp,%.o,$(DICT_CXX))
TMPHEADERS := $(wildcard $(MODULE)/*.h)
TMPHEADERS2 := $(filter-out $(EXCLUDE),$(TMPHEADERS))
HEADERS := $(filter-out $(LINKDEF),$(TMPHEADERS2))
LIB_SO = lib$(LIBNAME)$(MODULE).so
MOD_RMAP = lib$(LIBNAME)$(MODULE).rootmap

all: $(DICT_OBJ) rootlibmap

$(DICT_OBJ): $(DICT_CXX)
	@echo `tput setaf 6`"Compiling dictionary $@..."`tput sgr0`
	@$(CXX) $(DICT_COMPILE_FLAGS) -c -o $@ $^

$(DICT_CXX): $(HEADERS) $(LINKDEF)
	@echo `tput setaf 6`"Generating dictionary $@ using $(LINKDEF)..."`tput sgr0`
ifeq ($(shell expr $(ROOT_VERSION_CODE) \< $(ROOT_v5_99_00)),1)
	@rootcint -v -f $@ -c -p $(INCLUDE) $^
else
	@rootcling -v -f $@ -rml $(LIB_SO) -rmf $(MOD_RMAP) -c -p $(INCLUDE) $^
endif

rootlibmap :
ifeq ($(shell expr $(ROOT_VERSION_CODE) \< $(ROOT_v5_99_00)),1)
	rlibmap -f -r $(MOD_RMAP) -l $(LIB_SO) -d $(RLIBMAPDEPS) -c $(LINKDEF)
	@-cp $(MOD_RMAP) $(KVINSTALLDIR)/lib/
else
# for ROOT v6 we have to copy all the *.pcm files into $KVROOT/lib
# in addition, rootlibmap files are generated at the same time as dictionaries
# and only need to be copied at this point
	@-cp *.pcm $(KVINSTALLDIR)/lib/
	@-cp *.rootmap $(KVINSTALLDIR)/lib/
endif


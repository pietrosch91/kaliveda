# generate CINT dictionary in subdir MODULE
# invoke using: 
#       make -f MakeDict.mk MODULE=base LIBNAME=KVMultiDet INCLUDE="[include paths]" EXCLUDE="[*.h to exclude]" DICT_COMPILE_FLAGS="[flags for compiling dictionary]"
# in order to generate the dictionary KVMultiDetbaseDict.cxx/h
# for subdirectory base/ using base/KVMultiDetbaseLinkDef.h
MODULE = 
LIBNAME = 
EXCLUDE =
INCLUDE =
DICT_COMPILE_FLAGS =
LINKDEF = $(MODULE)/$(LIBNAME)$(MODULE)LinkDef.h
DICT_CXX = $(LIBNAME)$(MODULE)Dict.cpp
DICT_OBJ = $(patsubst %.cpp,%.o,$(DICT_CXX))
TMPHEADERS := $(wildcard $(MODULE)/*.h)
TMPHEADERS2 := $(filter-out $(EXCLUDE),$(TMPHEADERS))
HEADERS := $(filter-out $(LINKDEF),$(TMPHEADERS2))

$(DICT_OBJ): $(DICT_CXX)
	@echo "Compiling dictionary $@..."
	@$(CXX) $(DICT_COMPILE_FLAGS) -c -o $@ $^

$(DICT_CXX): $(HEADERS) $(LINKDEF)
	@echo "Generating dictionary $@ using $(LINKDEF)..."
	@rootcint -v -f $@ -c -p $(INCLUDE) $^


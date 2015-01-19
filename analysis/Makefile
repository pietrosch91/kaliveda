#$Id: Makefile,v 1.113 2009/03/03 13:36:00 franklan Exp $
#$Revision: 1.113 $
#$Date: 2009/03/03 13:36:00 $
#$Author: franklan $
#
#Makefile for KaliVeda analysis tools

###########################################
#                                                                        #
#   DO NOT CHANGE ANY OF THE FOLLOWING   #
#                                                                        #
###########################################

ifeq (,$(wildcard $(ROOT_MAKEFILE_PATH)/Makefile.arch))
SrcSuf        = cxx
ObjSuf        = o
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o # keep whitespace after "-o"
RC           := root-config
ROOTCFLAGS   := $(shell $(RC) --cflags)
ROOTLDFLAGS  := $(shell $(RC) --ldflags)
ROOTLIBS     := $(shell $(RC) --libs)
ROOTGLIBS    := $(shell $(RC) --glibs)
OPT2          = -O2
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
SOFLAGS       = -shared
CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)
else
include $(ROOT_MAKEFILE_PATH)/Makefile.arch
endif

#set the option of the cp command
ifeq ($(PLATFORM),linux)
CPYOPT = "--update"
endif

KV_INC = 

CXXFLAGS += -I$(KVINSTALLDIR)/include $(KV_INC)
ifeq ($(WITH_GRU_LIB),yes)
CXXFLAGS += -DNET_LIB
endif

MDETLIBS := $(wildcard $(KVROOT)/lib/libKVMultiDet*.so) 
MDETLIBS := $(notdir $(MDETLIBS))
MDETLIBS := $(patsubst lib%.so,-l%,$(MDETLIBS))
LIBKVMULTIDET := -L$(KVROOT)/lib $(MDETLIBS)

INDRALIBS := $(wildcard $(KVROOT)/lib/libKVIndra*.so) 
INDRALIBS := $(notdir $(INDRALIBS))
INDRALIBS := $(patsubst lib%.so,-l%,$(INDRALIBS))
LIBKVINDRA := -L$(KVROOT)/lib $(INDRALIBS)

LINK_LIBS := -L$(KVROOT)/lib $(INDRALIBS) $(MDETLIBS)

OPT = -O
OPT2 = -O2 
OPT3 = -O3 
TMPFLAGS := $(filter-out $(OPT2),$(CXXFLAGS))
CXXFLAGS_NOOPT = $(filter-out $(OPT),$(TMPFLAGS))

#debugging/profiling/memchecking on linux machine
ifeq ($(PLATFORM),linux)
ifeq ($(debug),yes)
CXXFLAGS_NOOPT += -g
CXXFLAGS := $(CXXFLAGS_NOOPT) $(OPT)
LDFLAGS := $(ROOTLDFLAGS) -g $(OPT)
else
CXXFLAGS := $(CXXFLAGS_NOOPT) $(OPT3)
LDFLAGS := $(ROOTLDFLAGS) $(OPT3)
endif
ifeq ($(memcheck),yes)
CXXFLAGS += -g -fno-inline
LDFLAGS += -g -fno-inline
TMPFLAGS := $(filter-out -O2,$(CXXFLAGS))
CXXFLAGS = $(TMPFLAGS)
endif
ifeq ($(profile),yes)
CXXFLAGS += -g -pg
LDFLAGS += -g -pg
endif
endif

ifeq ($(shell expr $(ROOT_VERSION_CODE) \< $(ROOT_v5_32_00)),1)
# before ROOT 5.32 xrootd libraries were in main ROOT installation
XRDPOSIX = -L$(ROOTSYS)/lib -lXrdPosix
else
# since 5.32 xrootd is installed separately in $XRDSYS directory
XRDPOSIX = -L$(XRDSYS)/lib64 -lXrdPosixPreload
endif

ANALOGS = analyse_logs.cxx
ANALOGO = analyse_logs.$(ObjSuf)
ANALOG  = analyse_logs$(ExeSuf)
ANALOG_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
ANALOG_LIBS += $(XRDPOSIX)
endif

DELOGS = delete_logs.cxx
DELOGO = delete_logs.$(ObjSuf)
DELOG  = delete_logs$(ExeSuf)
DELOG_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
DELOG_LIBS += $(XRDPOSIX)
endif

KVDTANS = kvdatanalyser.cxx
KVDTANO = kvdatanalyser.$(ObjSuf)
KVDTAN  = kvdatanalyser$(ExeSuf)
KVDTAN_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVDTAN_LIBS += $(XRDPOSIX)
endif

KVTRANS = kvtreeanalyzer.cxx
KVTRANO = kvtreeanalyzer.$(ObjSuf)
KVTRAN  = kvtreeanalyzer$(ExeSuf)
KVTRAN_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVTRAN_LIBS += $(XRDPOSIX)
endif

KVRSRDS = runsheetreader.cpp
KVRSRDO = runsheetreader.$(ObjSuf)
KVRSRD  = KVDataBaseGUI$(ExeSuf)
KVRSRD_LIBS = $(LINK_LIBS)
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVRSRD_LIBS += $(XRDPOSIX)
endif

KVANALS = KaliVedaAnalysis.cpp
KVANALO = KaliVedaAnalysis.$(ObjSuf)
KVANAL  = KaliVedaAnalysis$(ExeSuf)
KVANAL_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVANAL_LIBS += $(XRDPOSIX)
endif

KVGUIS = KaliVedaGUI.cpp
KVGUIO = KaliVedaGUI.$(ObjSuf)
KVGUI  = KaliVedaGUI$(ExeSuf)
KVGUI_LIBS = $(LIBKVINDRA) $(LIBKVMULTIDET) -L$(ROOTSYS)/lib -lGui
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVGUI_LIBS += $(XRDPOSIX)
endif

UPDRLTS = update_runlist.cxx
UPDRLTO = update_runlist.$(ObjSuf)
UPDRLT  = update_runlist$(ExeSuf)
UPDRLT_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
UPDRLT_LIBS += $(XRDPOSIX)
endif

SCRIPTS = $(UPDRLT) $(KVDTAN) $(KVTRAN) $(ANALOG) $(DELOG) $(KVRSRD) $(KVANAL) $(KVGUI) KVA KaliVeda

SOURCES = $(ANALOGS) $(DELOGS) $(KVDTANS) $(KVTRANS) $(KVRSRDS) $(KVANALS) $(KVGUIS) $(UPDRLTS)
PREQ = $(SOURCES:%.cxx=%.d)
PREREQ = $(PREQ:%.cpp=%.d)

.PHONY: all install clean

all: $(ANALOG) $(DELOG) $(KVDTAN) $(KVTRAN) $(KVRSRD) $(UPDRLT) $(KVANAL) $(KVGUI) install
	
$(ANALOG): $(ANALOGO)
	@echo `tput setaf 5`"Building executable $@"`tput sgr0`; \
	$(LD) $(LDFLAGS) $^ $(ANALOG_LIBS) $(LIBS) $(OutPutOpt)$@

$(ANALOGO): $(ANALOGS)
	@echo `tput setaf 1`"   CXX $<..."`tput sgr0`
	@$(CXX) $(CXXFLAGS) -DGRIDENGINE_LOG_READER -c $<

$(DELOG): $(DELOGO)
	@echo `tput setaf 5`"Building executable $@"`tput sgr0`; \
	$(LD) $(LDFLAGS) $^  $(DELOG_LIBS) $(LIBS) $(OutPutOpt)$@

$(KVDTAN): $(KVDTANO)
	@echo `tput setaf 5`"Building executable $@"`tput sgr0`; \
	$(LD) $(LDFLAGS) $^ $(KVDTAN_LIBS) $(LIBS) $(OutPutOpt)$@

$(KVTRAN): $(KVTRANO)
	@echo `tput setaf 5`"Building executable $@"`tput sgr0`; \
	$(LD) $(LDFLAGS) $^ $(KVTRAN_LIBS) $(LIBS) $(OutPutOpt)$@

#$(KVDTANO): $(KVDTANS)
#	$(CXX) $(CXXFLAGS) -c $<
	
$(KVRSRD): $(KVRSRDO)
ifeq ($(ARCH),win32)
	$(LD) $(LDFLAGS) $^ $(LIBS) '$(ROOTSYS)\lib\libTreePlayer.lib' '$(ROOTSYS)\lib\libTreeViewer.lib' '$(ROOTSYS)\lib\libGed.lib' $(LINK_LIBS) $(OutPutOpt)$@
else
	@echo `tput setaf 5`"Building executable $@"`tput sgr0`; \
	$(LD) $(LDFLAGS) $^ $(KVRSRD_LIBS) $(LIBS) -lTreePlayer -lTreeViewer -lGed $(OutPutOpt)$@
endif

#$(KVRSRDO): $(KVRSRDS)
#	$(CXX) $(CXXFLAGS) -c $<

$(UPDRLT): $(UPDRLTO)
	@echo `tput setaf 5`"Building executable $@"`tput sgr0`; \
	$(LD) $(LDFLAGS) $^ $(UPDRLT_LIBS) $(LIBS) $(OutPutOpt)$@

#$(UPDRLTO): $(UPDRLTS)
#	$(CXX) $(CXXFLAGS) -c $<

$(KVANAL): $(KVANALO)
	@echo `tput setaf 5`"Building executable $@"`tput sgr0`; \
	$(LD) $(LDFLAGS) $^ $(KVANAL_LIBS) $(LIBS) $(OutPutOpt)$@

#$(KVANALO): $(KVANALS)
#	$(CXX) $(CXXFLAGS) -c $<
	
$(KVGUI): $(KVGUIO)
	@echo `tput setaf 5`"Building executable $@"`tput sgr0`; \
	$(LD) $(LDFLAGS) $^ $(KVGUI_LIBS) $(LIBS) $(OutPutOpt)$@

#$(KVGUIO): $(KVGUIS)
#	$(CXX) $(CXXFLAGS) -c $<
	
install:
	-chmod +x $(SCRIPTS)
	-cp $(CPYOPT) $(SCRIPTS) $(KVINSTALLDIR)/bin/

clean:
	-rm -f *.$(ObjSuf)
	-rm -f *Dict*
	-rm -f *.$(DllSuf) *.d .depend
	-rm -f $(ANALOG)
	-rm -f $(DELOG)
	-rm -f $(KVDTAN)
	-rm -f $(KVTRAN)
	-rm -f $(KVRSRD)
	-rm -f $(KVANAL)
	-rm -f $(KVGUI)
	-rm -f $(UPDRLT)
ifeq ($(ARCH),macosx)
	-rm -f *.dylib
endif

uninstall:
	$(foreach scrip,$(SCRIPTS),$(shell rm -f $(KVINSTALLDIR)/bin/$(scrip)))

config :
	@echo "--------------------------------------------------------------------------------"
	@echo "CONFIGURATION OF MAKEFILE FOR analysis tools"
	@echo "--------------------------------------------------------------------------------"
	@echo "SCRIPTS = "$(SCRIPTS)
	@echo "--------------------------------------------------------------------------------"
	@echo "SOURCES = "$(SOURCES)
	@echo "--------------------------------------------------------------------------------"
	@echo "PREREQ = "$(PREREQ)
	@echo "--------------------------------------------------------------------------------"
	@echo "SrcSuf = "$(SrcSuf)
	@echo "--------------------------------------------------------------------------------"

ifneq ($(ARCH),win32)
%.d : %.cxx
	@echo `tput setaf 3`"Analysing dependencies of $^"`tput sgr0`; \
	set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@
%.d : %.cpp
	@echo `tput setaf 3`"Analysing dependencies of $^"`tput sgr0`; \
	set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@
	
##### Include all the *.o target rules generated by gcc -MM #####
##### However, if we are cleaning or checking config or making #####
##### logs, we don't include this file since it may screw up things #####
ifeq ($(findstring $(MAKECMDGOALS), clean config),)
-include $(PREREQ)
endif
endif

%.$(ObjSuf) : %.cxx
	@echo `tput setaf 1`"   CXX $<..."`tput sgr0`
	@$(CXX) $(CXXFLAGS) -c $<
%.$(ObjSuf) : %.cpp
	@echo `tput setaf 1`"   CXX $<..."`tput sgr0`
	@$(CXX) $(CXXFLAGS) -c $<

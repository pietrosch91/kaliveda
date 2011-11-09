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

include $(ROOT_MAKEFILE_PATH)/Makefile.arch

#set the option of the cp command
ifeq ($(PLATFORM),linux)
CPYOPT = "--update"
endif

KV_INC = 

CXXFLAGS += -I$(KVINSTALLDIR)/include $(KV_INC)
ifeq ($(WITH_GRU_LIB),yes)
CXXFLAGS += -DNET_LIB
endif

LIBKVINDRA = -L$(KVROOT)/lib -lKVIndra
LIBKVMULTIDET = -L$(KVROOT)/lib -lKVMultiDet

ifeq ($(ARCH),win32)
LINK_LIBS = '$(KVROOT)\lib\libKVIndra.lib' '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIBS = -L$(KVROOT)/lib -lKVIndra -lKVMultiDet
endif

ifeq ($(ARCH),macosx)
LIBS += -L$(ROOTSYS)/lib -lGUI
LIBKVMULTIDET += -L$(ROOTSYS)/lib -lGeom
LIBKVINDRA += $(LIBKVMULTIDET)
endif

ifeq ($(ARCH),win32)
LIBS += '$(ROOTSYS)\lib\libGUI.lib'
endif

ifeq ($(ARCH),win32)
#Windows/VC++
#CXXFLAGS without "-O2" optimisation flag
#to avoid warnings (optimisation not available in standard edition compiler)
TMPFLAGS := $(filter-out -O2,$(CXXFLAGS))
CXXFLAGS = $(TMPFLAGS)
endif

#debugging/profiling/memchecking on linux machine
ifeq ($(PLATFORM),linux)
ifeq ($(debug),yes)
CXXFLAGS += -g
LDFLAGS += -g
TMPFLAGS := $(filter-out -O2,$(CXXFLAGS))
CXXFLAGS = $(TMPFLAGS)
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

ANALOGS = analyse_logs.cxx
ANALOGO = analyse_logs.$(ObjSuf)
ANALOG  = analyse_logs$(ExeSuf)
ANALOG_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
ANALOG_LIBS += -L$(ROOTSYS)/lib -lXrdPosix
endif

KVDTANS = kvdatanalyser.cxx
KVDTANO = kvdatanalyser.$(ObjSuf)
KVDTAN  = kvdatanalyser$(ExeSuf)
KVDTAN_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVDTAN_LIBS += -L$(ROOTSYS)/lib -lXrdPosix
endif

KVRSRDS = runsheetreader.cpp
KVRSRDO = runsheetreader.$(ObjSuf)
KVRSRD  = KVDataBaseGUI$(ExeSuf)
KVRSRD_LIBS = $(LINK_LIBS)
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVRSRD_LIBS += -L$(ROOTSYS)/lib -lXrdPosix
endif

KVANALS = KaliVedaAnalysis.cpp
KVANALO = KaliVedaAnalysis.$(ObjSuf)
KVANAL  = KaliVedaAnalysis$(ExeSuf)
KVANAL_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVANAL_LIBS += -L$(ROOTSYS)/lib -lXrdPosix
endif

KVGUIS = KaliVedaGUI.cpp
KVGUIO = KaliVedaGUI.$(ObjSuf)
KVGUI  = KaliVedaGUI$(ExeSuf)
KVGUI_LIBS = $(LIBKVINDRA) -L$(ROOTSYS)/lib -lGui
ifeq ($(MAKE_GANTAPE_XRD),yes)
KVGUI_LIBS += -L$(ROOTSYS)/lib -lXrdPosix
endif

UPDRLTS = update_runlist.cxx
UPDRLTO = update_runlist.$(ObjSuf)
UPDRLT  = update_runlist$(ExeSuf)
UPDRLT_LIBS = $(LIBKVMULTIDET)
ifeq ($(MAKE_GANTAPE_XRD),yes)
UPDRLT_LIBS += -L$(ROOTSYS)/lib -lXrdPosix
endif

SCRIPTS = $(UPDRLT) $(KVDTAN) $(ANALOG) $(KVRSRD) $(KVANAL) $(KVGUI) KVA KaliVeda

SOURCES = $(ANALOGS) $(KVDTANS) $(KVRSRDS) $(KVANALS) $(KVGUIS) $(UPDRLTS)
PREQ = $(SOURCES:%.cxx=%.d)
PREREQ = $(PREQ:%.cpp=%.d)

.PHONY: all install clean

all: config $(ANALOG) $(KVDTAN) $(KVRSRD) $(UPDRLT) $(KVANAL) $(KVGUI) install
	
$(ANALOG): $(ANALOGO)
	$(LD) $(LDFLAGS) $^  $(LIBS) $(ANALOG_LIBS) $(OutPutOpt)$@
	@echo "$@ done"

#$(ANALOGO): $(ANALOGS)
#	$(CXX) $(CXXFLAGS) -c $<

$(KVDTAN): $(KVDTANO)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(KVDTAN_LIBS) $(OutPutOpt)$@
	@echo "$@ done"

#$(KVDTANO): $(KVDTANS)
#	$(CXX) $(CXXFLAGS) -c $<
	
$(KVRSRD): $(KVRSRDO)
ifeq ($(ARCH),win32)
	$(LD) $(LDFLAGS) $^ $(LIBS) '$(ROOTSYS)\lib\libTreePlayer.lib' '$(ROOTSYS)\lib\libTreeViewer.lib' '$(ROOTSYS)\lib\libGed.lib' $(LINK_LIBS) $(OutPutOpt)$@
else
	$(LD) $(LDFLAGS) $^ $(LIBS) -lTreePlayer -lTreeViewer -lGed $(KVRSRD_LIBS) $(OutPutOpt)$@
endif
	@echo "$@ done"

#$(KVRSRDO): $(KVRSRDS)
#	$(CXX) $(CXXFLAGS) -c $<

$(UPDRLT): $(UPDRLTO)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(UPDRLT_LIBS) $(OutPutOpt)$@
	@echo "$@ done"

#$(UPDRLTO): $(UPDRLTS)
#	$(CXX) $(CXXFLAGS) -c $<

$(KVANAL): $(KVANALO)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(KVANAL_LIBS) $(OutPutOpt)$@
	@echo "$@ done"

#$(KVANALO): $(KVANALS)
#	$(CXX) $(CXXFLAGS) -c $<
	
$(KVGUI): $(KVGUIO)
	$(LD) $(LDFLAGS) $^ $(LIBS) $(KVGUI_LIBS) $(OutPutOpt)$@
	@echo "$@ done"

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
	-rm -f $(KVDTAN)
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
	@echo Making prerequisite $@; \
	set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@
%.d : %.cpp
	@echo Making prerequisite $@; \
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
	$(CXX) $(CXXFLAGS) -c $<
%.$(ObjSuf) : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

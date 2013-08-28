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
KVGUI_LIBS = $(LIBKVINDRA) -L$(ROOTSYS)/lib -lGui
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

SCRIPTS = $(UPDRLT) $(KVDTAN) $(ANALOG) $(DELOG) $(KVRSRD) $(KVANAL) $(KVGUI) KVA KaliVeda

SOURCES = $(ANALOGS) $(DELOGS) $(KVDTANS) $(KVRSRDS) $(KVANALS) $(KVGUIS) $(UPDRLTS)
PREQ = $(SOURCES:%.cxx=%.d)
PREREQ = $(PREQ:%.cpp=%.d)

.PHONY: all install clean

all: config $(ANALOG) $(DELOG) $(KVDTAN) $(KVRSRD) $(UPDRLT) $(KVANAL) $(KVGUI) install
	
$(ANALOG): $(ANALOGO)
	$(LD) $(LDFLAGS) $^ $(ANALOG_LIBS) $(LIBS) $(OutPutOpt)$@
	@echo "$@ done"

$(ANALOGO): $(ANALOGS)
	$(CXX) $(CXXFLAGS) -DGRIDENGINE_LOG_READER -c $<

$(DELOG): $(DELOGO)
	$(LD) $(LDFLAGS) $^  $(DELOG_LIBS) $(LIBS) $(OutPutOpt)$@
	@echo "$@ done"

$(KVDTAN): $(KVDTANO)
	$(LD) $(LDFLAGS) $^ $(KVDTAN_LIBS) $(LIBS) $(OutPutOpt)$@
	@echo "$@ done"

#$(KVDTANO): $(KVDTANS)
#	$(CXX) $(CXXFLAGS) -c $<
	
$(KVRSRD): $(KVRSRDO)
ifeq ($(ARCH),win32)
	$(LD) $(LDFLAGS) $^ $(LIBS) '$(ROOTSYS)\lib\libTreePlayer.lib' '$(ROOTSYS)\lib\libTreeViewer.lib' '$(ROOTSYS)\lib\libGed.lib' $(LINK_LIBS) $(OutPutOpt)$@
else
	$(LD) $(LDFLAGS) $^ $(KVRSRD_LIBS) $(LIBS) -lTreePlayer -lTreeViewer -lGed $(OutPutOpt)$@
endif
	@echo "$@ done"

#$(KVRSRDO): $(KVRSRDS)
#	$(CXX) $(CXXFLAGS) -c $<

$(UPDRLT): $(UPDRLTO)
	$(LD) $(LDFLAGS) $^ $(UPDRLT_LIBS) $(LIBS) $(OutPutOpt)$@
	@echo "$@ done"

#$(UPDRLTO): $(UPDRLTS)
#	$(CXX) $(CXXFLAGS) -c $<

$(KVANAL): $(KVANALO)
	$(LD) $(LDFLAGS) $^ $(KVANAL_LIBS) $(LIBS) $(OutPutOpt)$@
	@echo "$@ done"

#$(KVANALO): $(KVANALS)
#	$(CXX) $(CXXFLAGS) -c $<
	
$(KVGUI): $(KVGUIO)
	$(LD) $(LDFLAGS) $^ $(KVGUI_LIBS) $(LIBS) $(OutPutOpt)$@
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
	-rm -f $(DELOG)
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

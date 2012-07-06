#$Id: Makefile,v 1.25 2009/03/03 14:27:15 franklan Exp $
#$Revision: 1.25 $
#$Date: 2009/03/03 14:27:15 $
#$Author: franklan $
#
#Makefile for KaliVeda web site/documentation

###########################################
#                                                                        #
#   DO NOT CHANGE ANY OF THE FOLLOWING   #
#                                                                        #
###########################################

include $(ROOT_MAKEFILE_PATH)/Makefile.arch

CXXFLAGS += -I$(KVINSTALLDIR)/include 

ifeq ($(debug),yes)
CXXFLAGS += -g
endif

KV_INC =
#-----------Handle backwards compatibility issues depending on ROOT version
include $(KVPROJ_ROOT_ABS)/Makefile.compat
#-------------------------------------------------------------------------------------------
CXXFLAGS += $(KV_INC)

ifeq ($(ARCH),win32)
LINK_LIBS = '$(KVROOT)\lib\libKVIndra.lib' '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIBS = -L$(KVROOT)/lib -lKVIndra -lKVMultiDet
endif

#ganil libraries for reading raw data only build on linux systems
ifeq ($(PLATFORM),linux)
export ROOTGANILTAPE = yes
ifneq ($(WITH_VAMOS),no)
LINK_LIBS +=  -lVAMOS
endif
else
export ROOTGANILTAPE = no
endif

ifeq ($(shell expr $(ROOT_VERSION_CODE) \< $(ROOT_v5_32_00)),1)
XRDPOSIX = -L$(ROOTSYS)/lib -lXrdPosix
else
XRDPOSIX = -L$(XRDSYS)/lib -lXrdPosix
endif

ifeq ($(MAKE_GANTAPE_XRD),yes)
LINK_LIBS += $(XRDPOSIX)
endif

ifeq ($(ARCH),macosx)
LIBS += -L$(ROOTSYS)/lib -lPhysics -lGUI
endif

ifeq ($(ARCH),win32)
LIBS += '$(ROOTSYS)lib\libHtml.lib' '$(ROOTSYS)lib\libThread.lib'
else
LIBS += -lHtml -lThread
endif

ifeq ($(ARCH),win32)
MAKEHTMLS = make_html_doc.cxx
$(MAKEHTMLS): make_html_doc.C
	-cp make_html_doc.C $(MAKEHTMLS)
else
MAKEHTMLS = make_html_doc.C
endif
MAKEHTMLO = make_html_doc.$(ObjSuf)
MAKEHTML  = make_html_doc$(ExeSuf)
SCANCLASSESS = ScanClasses.cpp
SCANCLASSESO = ScanClasses.$(ObjSuf)

.PHONY : clean

clean:
	-rm -f *.d $(MAKEHTMLO) $(SCANCLASSESO)  $(MAKEHTML) MyPathDefinition_cxx.so examples/*.so examples/*.d
        
$(MAKEHTML): $(SCANCLASSESO) $(MAKEHTMLO) MyPathDefinition_cxx.so 
	$(LD) $(LDFLAGS) $^ $(LIBS) $(LINK_LIBS) $(OutPutOpt)$@
	@echo "$@ done"

MyPathDefinition_cxx.so: MyPathDefinition.h MyPathDefinition.cxx
	@root -b -n -q -l loadMyPathDefinition.C
	-cp MyPathDefinition_cxx.so $(KVROOT)/lib/

$(MAKEHTMLO): $(MAKEHTMLS)
	$(CXX) $(CXXFLAGS) -c $<

$(SCANCLASSESO): $(SCANCLASSESS)
	$(CXX) $(CXXFLAGS) -c $<

install_html: $(MAKEHTML)
	-mkdir -p $(KVROOT)/KaliVedaDoc
	-mkdir -p $(KVROOT)/KaliVedaDoc/$(VERSION_NUMBER)
	-mkdir -p $(KVROOT)/KaliVedaDoc/images
	-mkdir -p $(KVROOT)/KaliVedaDoc/$(VERSION_NUMBER)/images
	-mkdir -p $(KVROOT)/KaliVedaDoc/css
	-mkdir -p $(KVROOT)/KaliVedaDoc/js
	-mkdir -p $(KVROOT)/KaliVedaDoc/KaliVedaGUI_fichiers
	./$(MAKEHTML)
#	-cp $(KVROOT)/KaliVedaDoc/$(VERSION_NUMBER)/*.html $(KVROOT)/KaliVedaDoc/
	-cp css/ROOT.css $(KVROOT)/KaliVedaDoc/
	-ln -s -t$(KVROOT)/KaliVedaDoc $(VERSION_NUMBER)/incl $(VERSION_NUMBER)/inh $(VERSION_NUMBER)/inhmem $(VERSION_NUMBER)/src $(VERSION_NUMBER)/examples
	-cp *.html $(KVROOT)/KaliVedaDoc/
	-cp css/* $(KVROOT)/KaliVedaDoc/css/
	-cp css/ROOT.css $(KVROOT)/KaliVedaDoc/$(VERSION_NUMBER)/
	-cp js/* $(KVROOT)/KaliVedaDoc/js/
	-cp images/* $(KVROOT)/KaliVedaDoc/$(VERSION_NUMBER)/images/
	-cp images/* $(KVROOT)/KaliVedaDoc/images/
	-cp KaliVedaGUI_fichiers/* $(KVROOT)/KaliVedaDoc/KaliVedaGUI_fichiers/

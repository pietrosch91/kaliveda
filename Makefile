#$Id: Makefile,v 1.105 2008/03/25 09:18:49 franklan Exp $
#Makefile for libKVIndra.so

PROJ_NAME = KVIndra

MODULES = analysis base calibration daq_cec db detectors events gui identification indra particles

MANIP_DIRS = INDRA_camp1 INDRA_camp2 INDRA_camp4 INDRA_e475s INDRA_e503 INDRA_e494s
MANIP_DIRS += $(shell ls factory/*Template.[c,h]*)

#list of sources that are not yet ready to be compiled
NOT_READY := daq_cec/KVS*.cpp
ifeq ($(INDRADLT),no)
NOT_READY += analysis/KVINDRARawDataReader.cpp analysis/KVINDRARawDataAnalyser.cpp analysis/KVINDRARawDataReconstructor.cpp analysis/KVINDRARawIdent.cpp 
endif

ifeq ($(INDRADLT),yes)
KV_INC_EXTRAS := -I$(GANTAPE_INC) -I$(GANILTAPE_INC) -DINDRADLT
endif

include ../Makefile.general 

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVMultiDet
endif

ifeq ($(INDRADLT),yes)
LINK_LIB += -L$(KVROOT)/lib -lROOTGanilTape
ifeq ($(MAKE_GANTAPE_RFIO),yes)
LINK_LIB += -L/usr/local/lib -lshift
endif
#ifeq ($(MAKE_GANTAPE_XRD),yes)
#LINK_LIB += -L$(ROOTSYS)/lib -lXrdPosix
# -lXrdSec
#endif
endif

indra2root : 
	-cp INDRA2ROOT/indra2root.C $(INSTALL)/src/
	-cp INDRA2ROOT/veda2root $(INSTALL)/bin/
	-cp INDRA2ROOT/veda2root_kali $(INSTALL)/bin/
	-cp INDRA2ROOT/veda2root_kali96 $(INSTALL)/bin/
	-cp INDRA2ROOT/veda2root_lanceur $(INSTALL)/bin/
	-cp INDRA2ROOT/fin_faire_arbre.exe $(INSTALL)/bin/
	-cp INDRA2ROOT/fin_faire_arbre.exe $(INSTALL)/bin/fin_faire_arbre_c2.exe
	-cp INDRA2ROOT/fin_faire_arbre.exe $(INSTALL)/bin/fin_faire_arbre_c4.exe
	-cp INDRA2ROOT/faire_arbre.f $(INSTALL)/src/
	-cp INDRA2ROOT/faire_arbre_c2.f $(INSTALL)/src/

uninstall-indra2root : 
	-rm -f $(INSTALL)/src/indra2root.C
	-rm -f $(INSTALL)/bin/veda2root
	-rm -f $(INSTALL)/bin/veda2root_kali 
	-rm -f $(INSTALL)/bin/veda2root_kali96
	-rm -f $(INSTALL)/bin/veda2root_lanceur 
	-rm -f $(INSTALL)/bin/fin_faire_arbre.exe 
	-rm -f $(INSTALL)/bin/fin_faire_arbre_c2.exe
	-rm -f $(INSTALL)/bin/fin_faire_arbre_c4.exe
	-rm -f $(INSTALL)/src/faire_arbre.f 
	-rm -f $(INSTALL)/src/faire_arbre_c2.f 

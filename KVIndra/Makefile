#$Id: Makefile,v 1.108 2009/03/03 13:36:00 franklan Exp $
#Makefile for libKVIndra.so

PROJ_NAME = KVIndra

MODULES = analysis base calibration daq_cec db detectors events gui identification idtelescopes idmaps indra particles

ETC_DATA = $(shell ls factory/*Template.[c,h]*)
MANIP_DIRS = INDRA_camp1 INDRA_camp2 INDRA_camp4 INDRA_camp5 INDRA_e475s INDRA_e503 INDRA_e494s t10_02 e613

#list of sources that are not yet ready to be compiled
NOT_READY := daq_cec/KVS*.cpp
ifeq ($(ROOTGANILTAPE),no)
NOT_READY += analysis/KVINDRARawDataAnalyser.cpp analysis/KVINDRARawDataReconstructor.cpp analysis/KVINDRARawIdent.cpp 
endif

ifeq ($(ROOTGANILTAPE),yes)
KV_INC_EXTRAS := -I$(GANTAPE_INC) -DROOTGANILTAPE
endif
ifeq ($(WITH_GRU_LIB),yes)
KV_INC_EXTRAS += -I$(GRU_DIR)/include
endif

include ../Makefile.general 

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVMultiDet
endif
RLIBMAPDEPS += libKVMultiDet.so

ifeq ($(ROOTGANILTAPE),yes)
ifeq ($(MAKE_GANTAPE_RFIO),yes)
LINK_LIB += -L/usr/local/lib -lshift
endif
endif
ifeq ($(WITH_GRU_LIB),yes)
LINK_LIB += -L$(GRU_DIR)/linux -lGRU -L$(ROOTSYS)/lib -lXMLIO -lSpectrum
endif

indra2root : 
	cd INDRA2ROOT && $(MAKE) -f GetDefaultF77.mk
	INDRA2ROOT/fortran_lecture_dst/compil_kali	
#	-cp INDRA2ROOT/indra2root.C $(KVINSTALLDIR)/src/
#	-cp INDRA2ROOT/veda2root $(KVINSTALLDIR)/bin/
	-cp INDRA2ROOT/veda2root_kali $(KVINSTALLDIR)/bin/
	-cp INDRA2ROOT/GetDefaultF77.mk $(KVINSTALLDIR)/bin/
#	-cp INDRA2ROOT/veda2root_kali96 $(KVINSTALLDIR)/bin/
#	-cp INDRA2ROOT/veda2root_lanceur $(KVINSTALLDIR)/bin/
#	-cp INDRA2ROOT/SetBatchRessources $(KVINSTALLDIR)/bin/
#	-cp INDRA2ROOT/fin_faire_arbre.exe $(KVINSTALLDIR)/bin/fin_faire_arbre_c1.exe
#	-cp INDRA2ROOT/fin_faire_arbre.exe $(KVINSTALLDIR)/bin/fin_faire_arbre_c2.exe
#	-cp INDRA2ROOT/fin_faire_arbre.exe $(KVINSTALLDIR)/bin/fin_faire_arbre_c4.exe
	-cp INDRA2ROOT/faire_arbre_c1.f $(KVINSTALLDIR)/src/
	-cp INDRA2ROOT/faire_arbre_c2.f $(KVINSTALLDIR)/src/
	-cp INDRA2ROOT/faire_arbre_c4.f $(KVINSTALLDIR)/src/

uninstall-indra2root : 
#	-rm -f $(KVINSTALLDIR)/src/indra2root.C
#	-rm -f $(KVINSTALLDIR)/bin/veda2root
	-rm -f $(KVINSTALLDIR)/bin/veda2root_kali 
#	-rm -f $(KVINSTALLDIR)/bin/veda2root_kali96
#	-rm -f $(KVINSTALLDIR)/bin/veda2root_lanceur 
#	-rm -f $(KVINSTALLDIR)/bin/SetBatchRessources
#	-rm -f $(KVINSTALLDIR)/bin/fin_faire_arbre_c1.exe 
#	-rm -f $(KVINSTALLDIR)/bin/fin_faire_arbre_c2.exe
#	-rm -f $(KVINSTALLDIR)/bin/fin_faire_arbre_c4.exe
	-rm -f $(KVINSTALLDIR)/src/faire_arbre_c1.f 
	-rm -f $(KVINSTALLDIR)/src/faire_arbre_c2.f 
	-rm -f $(KVINSTALLDIR)/src/faire_arbre_c4.f 

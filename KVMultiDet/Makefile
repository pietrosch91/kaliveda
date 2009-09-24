#$Id: Makefile,v 1.99 2009/03/06 08:59:18 franklan Exp $
#Makefile for libKVMultiDet.so

PROJ_NAME = KVMultiDet

MODULES = analysis base calibration daq_cec db detectors events geometry gui identification particles \
	vg_base vg_charge vg_energy vg_multiplicity vg_shape vg_momentum trieur

EXTRA_HEADERS = base/KVError.h base/KVParameter.h

ETC_DATA = data
ETC_DATA += $(shell ls factory/*Template.[c,h]*)

#list of sources that are not yet ready to be compiled
NOT_READY := daq_cec/KVReg*.cpp
ifeq ($(ROOTGANILTAPE),no)
NOT_READY += analysis/KVGANILDataReader.cpp analysis/KVRawDataAnalyser.cpp
endif

NOT_ROOTCINT := base/KVError.h

ifeq ($(ROOTGANILTAPE),yes)
KV_INC_EXTRAS := -I$(GANTAPE_INC) -I$(GANILTAPE_INC) -DROOTGANILTAPE
endif

include ../Makefile.general
RLIBMAPDEPS +=libCore.so libCint.so libRIO.so libNet.so libHist.so libGraf.so \
libGraf3d.so libGpad.so libTree.so libRint.so libPostscript.so libMatrix.so \
libPhysics.so libMathCore.so libThread.so libGui.so

#required for use of TGeo* classes in KVMaterial, KVDetector, etc.
LINK_LIB += -L$(ROOTSYS)/lib -lGeom
RLIBMAPDEPS += libGeom.so

ifneq ($(ARCH),win32)
LINK_LIB += -L$(KVROOT)/lib -lfitltg
endif

ifeq ($(SITE),CCIN2P3)
KV_INC += -D__CCIN2P3_SRB
endif

ifeq ($(ROOTGANILTAPE),yes)
LINK_LIB += -L$(KVROOT)/lib -lROOTGanilTape
RLIBMAPDEPS += libROOTGanilTape.so
endif

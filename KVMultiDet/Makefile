#$Id: Makefile,v 1.99 2009/03/06 08:59:18 franklan Exp $
#Makefile for libKVMultiDet.so

PROJ_NAME = KVMultiDet

MODULES = analysis base stopping calibration daq_cec data_management db detectors events geometry gui identification idmaps idtelescopes particles \
	vg_base vg_charge vg_energy vg_multiplicity vg_shape vg_momentum trieur

EXTRA_HEADERS = base/KVError.h base/KVParameter.h

ETC_DATA = data
ETC_DATA += $(shell ls factory/*Template.[c,h]*)

#list of sources that are not yet ready to be compiled
NOT_READY := daq_cec/KVReg*.cpp
ifeq ($(ROOTGANILTAPE),no)
NOT_READY += daq_cec/GT*.cpp analysis/KVGANILDataReader.cpp analysis/KVRawDataAnalyser.cpp
endif

NOT_ROOTCINT := base/KVError.h

ifeq ($(ROOTGANILTAPE),yes)
KV_INC_EXTRAS := -I$(GANTAPE_INC) -DROOTGANILTAPE
endif
ifeq ($(WITH_GRU_LIB),yes)
KV_INC_EXTRAS += -I$(GRU_DIR)/include
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
LINK_LIB += -L$(KVROOT)/lib -lgan_tape
RLIBMAPDEPS += libgan_tape.so
ifeq ($(MAKE_GANTAPE_XRD),yes)
KV_INC += -DCCIN2P3_XRD
RLIBMAPDEPS += libXrdPosix.so
endif
endif
ifeq ($(WITH_GRU_LIB),yes)
LINK_LIB += -L$(GRU_DIR)/linux -lGRU -L$(ROOTSYS)/lib -lXMLIO -lSpectrum
endif

#$Id: Makefile,v 1.96 2009/01/27 08:06:56 franklan Exp $
#Makefile for libKVMultiDet.so

PROJ_NAME = KVMultiDet

MODULES = analysis base calibration daq_cec db detectors events geometry gui identification particles \
	vg_base vg_charge vg_energy vg_multiplicity vg_shape vg_momentum trieur

EXTRA_HEADERS = base/KVError.h base/KVParameter.h

ETC_DATA = data
ETC_DATA += $(shell ls factory/*Template.[c,h]*)

#list of sources that are not yet ready to be compiled
NOT_READY := daq_cec/KVReg*.cpp

NOT_ROOTCINT := base/KVError.h

include ../Makefile.general

#required for use of TGeo* classes in KVMaterial, KVDetector, etc.
LINK_LIB += -L$(ROOTSYS)/lib -lGeom

ifeq ($(SITE),CCIN2P3)
LINK_LIB += -L/usr/local/lib -lshift -L$(ROOTSYS)/lib -lRFIO
KV_INC += -D__CCIN2P3_RFIO
endif

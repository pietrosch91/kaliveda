#$Id: Makefile,v 1.13 2009/02/06 11:33:20 franklan Exp $
#Makefile for libVAMOS.so

PROJ_NAME = VAMOS

MODULES = daq_cec analysis base reconstruction identification detectors geometry db events particles calibration

EXTRA_HEADERS = analysis/Defines.h

MANIP_DIRS = VAMOS_e503
ETC_DATA = $(shell ls factory/*Template.[c,h]*)

NOT_ROOTCINT := analysis/Defines.h

ifeq ($(ROOTGANILTAPE),yes)
KV_INC_EXTRAS := -DROOTGANILTAPE -I$(GANTAPE_INC) -D__unix -D__linux -fbounds-check
endif

NOT_READY :=

ifeq ($(ROOTGANILTAPE),no)
NOT_READY += daq_cec/*.cpp analysis/KVIVRawDataReconstructor.cpp analysis/KVIVRawDataAnalyser.cpp
endif

include ../Makefile.general


#required for use of TGeo* classes in KVMaterial, KVDetector, etc.
LINK_LIB += -L$(ROOTSYS)/lib -lGeom
RLIBMAPDEPS += libGeom.so

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVIndra.lib' '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVIndra -lKVMultiDet
endif
RLIBMAPDEPS += libKVIndra.so libKVMultiDet.so
ifeq ($(ROOTGANILTAPE),yes)
ifeq ($(MAKE_GANTAPE_RFIO),yes)
LINK_LIB += -L/usr/local/lib -lshift
endif
endif


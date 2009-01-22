#$Id: Makefile,v 1.11 2009/01/19 13:39:45 franklan Exp $
#Makefile for libVAMOS.so

PROJ_NAME = VAMOS

MODULES = daq_cec analysis base identification detectors

EXTRA_HEADERS = analysis/Defines.h

NOT_ROOTCINT := analysis/Defines.h

ifeq ($(INDRADLT),yes)
KV_INC_EXTRAS := -DINDRADLT -I$(GANTAPE_INC) -D__unix -D__linux -fbounds-check
endif

ifeq ($(INDRADLT),no)
NOT_READY := daq_cec/*.cpp analysis/KVIVRawDataReader.cpp analysis/KVIVRawDataReconstructor.cpp analysis/KVIVRawDataAnalyser.cpp
endif

include ../Makefile.general

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVIndra.lib' '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVIndra -lKVMultiDet
endif
ifeq ($(INDRADLT),yes)
LINK_LIB += -L$(GANILTAPE_LIB) -lROOTGanilTape -lgan_tape
ifeq ($(MAKE_GANTAPE_RFIO),yes)
LINK_LIB += -L/usr/local/lib -lshift
endif
#ifeq ($(MAKE_GANTAPE_XRD),yes)
#LINK_LIB += -L$(ROOTSYS)/lib -lXrdPosix -lXrdSec
#endif
endif

#$Id: Makefile,v 1.13 2009/02/06 11:33:20 franklan Exp $
#Makefile for libVAMOS.so

PROJ_NAME = VAMOS

MODULES = daq_cec analysis base identification detectors geometry

EXTRA_HEADERS = analysis/Defines.h

MANIP_DIRS = VAMOS_e503

NOT_ROOTCINT := analysis/Defines.h

ifeq ($(ROOTGANILTAPE),yes)
KV_INC_EXTRAS := -DROOTGANILTAPE -I$(GANTAPE_INC) -D__unix -D__linux -fbounds-check
endif

ifeq ($(ROOTGANILTAPE),no)
NOT_READY := daq_cec/*.cpp analysis/KVIVRawDataReader.cpp analysis/KVIVRawDataReconstructor.cpp analysis/KVIVRawDataAnalyser.cpp
endif

include ../Makefile.general

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVIndra.lib' '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVIndra -lKVMultiDet
endif
RLIBMAPDEPS += libKVIndra.so libKVMultiDet.so
ifeq ($(ROOTGANILTAPE),yes)
LINK_LIB += -L$(GANILTAPE_LIB) -lROOTGanilTape -lgan_tape
RLIBMAPDEPS += libROOTGanilTape.so
ifeq ($(MAKE_GANTAPE_RFIO),yes)
LINK_LIB += -L/usr/local/lib -lshift
endif
endif

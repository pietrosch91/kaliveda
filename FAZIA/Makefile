#$Id: Makefile,v 1.13 2009/02/06 11:33:20 franklan Exp $
#Makefile for libFAZIA.so

PROJ_NAME = FAZIA

MODULES = analysis fazia idtelescopes

MANIP_DIRS = LNS_2011 INDRAFAZIA

ETC_DATA = data

ifeq ($(ROOTGANILTAPE),yes)
KV_INC_EXTRAS := -DROOTGANILTAPE -I$(GANTAPE_INC) -D__unix -D__linux -fbounds-check
endif

ifeq ($(ROOTGANILTAPE),no)
NOT_READY += daq_cec/*.cpp analysis/KVIVRawDataReconstructor.cpp analysis/KVIVRawDataAnalyser.cpp
endif

include ../Makefile.general

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

#required for use of TGeo* classes in FAZIABlock
LINK_LIB += -L$(ROOTSYS)/lib -lGeom
RLIBMAPDEPS += libGeom.so


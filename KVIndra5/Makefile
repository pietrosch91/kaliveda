#$Id: Makefile,v 1.60 2009/02/06 11:33:20 franklan Exp $
#Makefile for libKVIndra5.so

PROJ_NAME = KVIndra5

MODULES = calibration db identification indra

MANIP_DIRS = INDRA_camp5

#list of sources that are not yet ready to be compiled
NOT_READY := calibration/KVCalibrationMaker5.cpp

include ../Makefile.general

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVIndra.lib' '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVIndra -lKVMultiDet
endif
RLIBMAPDEPS += libKVIndra.so libKVMultiDet.so

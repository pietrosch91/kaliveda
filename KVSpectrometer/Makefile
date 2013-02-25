#$Id: Makefile,v 1.1 2012/07/19 08:59:18 Guilain Exp $
#Makefile for libKVSpectrometer.so

PROJ_NAME = KVSpectrometer

MODULES = detectors geometry db events particles calibration analysis

#ETC_DATA = data
#ETC_DATA += $(shell ls factory/*Template.[c,h]*)

include ../Makefile.general

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVMultiDet
endif
RLIBMAPDEPS += libKVMultiDet.so

#required for use of TGeo* classes in KVMaterial, KVDetector, etc.
LINK_LIB += -L$(ROOTSYS)/lib -lGeom
RLIBMAPDEPS += libGeom.so

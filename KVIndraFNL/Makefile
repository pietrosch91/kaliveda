#$Id: Makefile,v 1.12 2009/02/06 11:33:20 franklan Exp $
#Makefile for libKVIndraFNL.so

PROJ_NAME = KVIndraFNL

MODULES = detectors indra idtelescopes calibration db

MANIP_DIRS = INDRA_e416a

include ../Makefile.general 

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVIndra.lib' '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVIndra -lKVMultiDet
endif
RLIBMAPDEPS += libKVIndra.so libKVMultiDet.so

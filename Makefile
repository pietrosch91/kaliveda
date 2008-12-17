#$Id: Makefile,v 1.11 2007/06/22 09:57:24 franklan Exp $
#Makefile for libKVIndraFNL.so

PROJ_NAME = KVIndraFNL

MODULES = detectors indra identification calibration db

MANIP_DIRS = INDRA_e416a

include ../Makefile.general 

ifeq ($(ARCH),win32)
LINK_LIB = '$(KVROOT)\lib\libKVIndra.lib' '$(KVROOT)\lib\libKVMultiDet.lib'
else
LINK_LIB = -L$(KVROOT)/lib -lKVIndra -lKVMultiDet
endif

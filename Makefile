# $Id: Makefile,v 1.22 2009/03/03 13:36:00 franklan Exp $
# Author: $Author: franklan $
#***************************************************************************
#                        Makefile
#                             -------------------
#    begin                : Thu Jun 14 2001
#    copyright            : (C) 2001 by Garp
#    email                : patois@ganil.fr
#####################################
#
# Makefile for ROOTGanilTape
# 
# WARNING: You must use a version of make which is compatible with GNUmake !
# 'gmake' will of course do.
#
#####################################
#
# -------------------------------------------------------------------------
#**************************************************************************
#                                                                        #
#   This program is free software; you can redistribute it and/or modify #
#   it under the terms of the GNU General Public License as published by #
#   the Free Software Foundation; either version 2 of the License, or    #
#   (at your option) any later version.                                  #
#                                                                        #
#**************************************************************************/

# Architecture dependant config
# This has to be same as argument of "VERSION = xxx" in Gan_Tape make_VERSION
HOSTTYPE = i386-linux
GANILTAPE = ../GanTape
include config/Makefile.$(HOSTTYPE)

KVINSTALLDIR  =      $(KVROOT)

ABSPATH  =      $(shell pwd)
SRC      =      $(ABSPATH)/src
LIB      =      $(ABSPATH)/lib
BIN      =      $(ABSPATH)/bin
INC      =      $(ABSPATH)/include
LIBFIX   =      $(HOSTTYPE)_lib

# ganil_tape lib
# GANILTAPE env variable must be set and point to the right directory
GTINCLUDE     = -I$(GANILTAPE)/include
GTLIBS        = -L$(KVINSTALLDIR)/lib -lgan_tape

# ROOT
ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)


#Optimisation
OPT           = -g

# Compiler:
ifeq ($(MAKE_GANTAPE_XRD),yes)
EXTRA_CXXFLAGS += -DCCIN2P3_XRD
endif

CXXFLAGS      = -fPIC $(OPT) $(ROOTCFLAGS) $(EXTRA_CXXFLAGS) $(GTINCLUDE) -Iinclude -I$(KVINSTALLDIR)/include

DPDFLAGS      = -Iinclude $(ROOTCFLAGS) -I$(GANILTAPE)/include  \
		-I/usr/include/g++ $(DPDFLAGS_ARCH)

# Linker:
LDFLAGS       = $(OPT) $(ROOTLIBS) $(EXTRA_LDFLAGS) $(GTLIBS)

OBJECTS       = $(SRC)/GTDataParameters.o \
		$(SRC)/GTGanilData.o \
		$(SRC)/GTOneScaler.o \
		$(SRC)/GTScalers.o


OBJECTSDICT   = $(SRC)/GTDataParameters.dict_o \
		$(SRC)/GTGanilData.dict_o \
		$(SRC)/GTOneScaler.dict_o \
		$(SRC)/GTScalers.dict_o


OBJEXAMPLE    = $(SRC)/Example.o
OBJGANIL2ROOT = $(SRC)/ganil2root.o

EXAMPLE       = $(BIN)/Example.exe
GANIL2ROOT    = $(BIN)/ganil2root

LIBROOTGANIL  = $(LIB)/libROOTGanilTape.so

all: $(LIBROOTGANIL) 

$(EXAMPLE): $(OBJEXAMPLE) $(LIBROOTGANIL)
	$(LD) -L$(LIB) -lROOTGanilTape $(LDFLAGS) $(OBJEXAMPLE) -o $(EXAMPLE) 

$(GANIL2ROOT): $(OBJGANIL2ROOT) $(LIBROOTGANIL)
	$(LD) -L$(LIB) -lROOTGanilTape $(LDFLAGS) $(OBJGANIL2ROOT) -o $(GANIL2ROOT)


$(LIBROOTGANIL): $(OBJECTS) $(OBJECTSDICT) $(GANILTAPE)/$(LIBFIX)/libgan_tape.so
	-mkdir -p $(LIB)
	-cp $(GANILTAPE)/$(LIBFIX)/libgan_tape.so $(KVINSTALLDIR)/lib/
	$(LD) $(SOFLAGS) $(OBJECTS) $(OBJECTSDICT) $(LDFLAGS) -o $(LIBROOTGANIL)
	-cp $(INC)/GT*.H $(KVINSTALLDIR)/include/
	-cp $(LIBROOTGANIL) $(KVINSTALLDIR)/lib/

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $< -c -o $@


%.dict_o : %.C
	$(CXX) $(CXXFLAGS) $< -c -o $@


$(SRC)/%.C : $(INC)/%.H $(INC)/%_LinkDef.h
	rootcint -f $@ -c $^

$(SRC)/%.C : $(INC)/%.H
	rootcint -f $@ -c $(GTINCLUDE) -I$(KVINSTALLDIR)/include $<


install: 
  ifeq ( $(KVINSTALLDIR),)
	@echo "KVROOT environment variable doesn't seem to be set."
	@echo "You must it in order to install KaliVeda project files"
	@echo "use 'setenv KVROOT install_path' (shell)"
	@echo "or  'export KVROOT=install_path' (bash)"
	@echo "use 'make install' to perform the installation"
  else
	-mkdir -p $(KVINSTALLDIR)/include
	-mkdir -p $(KVINSTALLDIR)/src
	-mkdir -p $(KVINSTALLDIR)/lib
	-cp $(INC)/GT*.H $(KVINSTALLDIR)/include/
	-cp $(SRC)/GT*.cpp $(KVINSTALLDIR)/src/
	-cp $(LIB)/lib* $(KVINSTALLDIR)/lib/
  endif

html:  $(EXAMPLE) $(LIBROOTGANIL)
	$(EXAMPLE) HTML

clean:
	-rm -f $(SRC)/*.o $(SRC)/*.dict_o $(SRC)/*.C $(SRC)/*.h

# Clean to perform to create a release version
distclean:
	rm -r $(SRC)/*.o $(SRC)/*.dict_o $(SRC)/*~ $(INC)/*~ $(BIN)/* $(LIB)/* CVS */CVS \
	dependancies.* html htmldoc

dependancies.make:
	touch  dependancies.make
	makedepend  -fdependancies.make $(DPDFLAGS) $(SRC)/*.cpp

depend:
	makedepend  -fdependancies.make $(DPDFLAGS) $(SRC)/*.cpp

include dependancies.make


#$Id: Makefile,v 1.81 2009/01/19 12:04:40 franklan Exp $
#$Revision: 1.81 $
#$Date: 2009/01/19 12:04:40 $
#$Author: franklan $
#
#General Makefile for the entire KaliVeda project

###########################################
#                                                                        #
#   DO NOT CHANGE ANY OF THE FOLLOWING   #
#                                                                        #
###########################################

ARCH         := $(shell root-config --arch)
PLATFORM     := $(shell root-config --platform)

#By default, we use the system-dependent definitions contained in the ROOT
#makefile found in $ROOTSYS/test/Makefile.arch, for standard installations.
#If ROOT is not installed in a single directory (ROOTSYS) you must
#give the path to the Makefile by invoking make with the argument:
#
# $>  make ROOT_MAKEFILE_PATH=/usr/local/share/root/test
#
#which will override the following definition
export ROOT_MAKEFILE_PATH = $(ROOTSYS)/test

# GanTape library with RFIO or XROOTD
# To compile the Ganil acquisition library using RFIO or XROOTD to
# open and read the files. Uncomment one if required.
# export MAKE_GANTAPE_XRD = yes
# export MAKE_GANTAPE_RFIO = no
# By default on CCIN2P3 machines, we use XROOTD
ifeq ($(SITE),CCIN2P3)
export MAKE_GANTAPE_XRD = yes
endif

# Compile for debugging with gdb:
#    make debug=yes
#
# Compile for memory leak check with valgrind/memcheck:
#    make memcheck=yes
#
# Compile for profiling with gprof:
#   make profile=yes

#Location of KaliVeda project sources root directory - absolute path
export KVPROJ_ROOT_ABS = $(shell pwd)

ifeq ($(ARCH),win32)
#on windows with cygwin and VC++, KVROOT is in DOS format!!
export INSTALL = $(shell cygpath -u '$(KVROOT)')
else
export INSTALL = $(KVROOT)
endif

#split ROOT version into major, minor and release version numbers
rootvers:= $(shell root-config --version)
empty:=
space:= $(empty) $(empty)
rootvers1:=$(subst .,$(space),$(rootvers))
rootvers2:=$(subst /,$(space),$(rootvers1))
root_maj := $(word 1,$(rootvers2))
root_min := $(word 2,$(rootvers2))
root_rel := $(word 3,$(rootvers2))
#define macro for calculating ROOT version code from maj, min and release
get_root_version = $(shell expr $(1) \* 10000 \+ $(2) \* 100 \+ $(3))
export ROOT_VERSION_CODE = $(call get_root_version,$(root_maj),$(root_min),$(root_rel))
#define constants for different versions tested in submakefiles
export ROOT_v4_00_06 = $(call get_root_version,4,0,6)
export ROOT_v4_00_08 = $(call get_root_version,4,0,8)
export ROOT_v4_01_04 = $(call get_root_version,4,1,4)
export ROOT_v4_01_01 = $(call get_root_version,4,1,1)
export ROOT_v4_01_02 = $(call get_root_version,4,1,2)
export ROOT_v4_03_04 = $(call get_root_version,4,3,4)
export ROOT_v5_02_00 = $(call get_root_version,5,2,0)
export ROOT_v5_04_00 = $(call get_root_version,5,4,0)
export ROOT_v5_08_00 = $(call get_root_version,5,8,0)
export ROOT_v5_10_00 = $(call get_root_version,5,10,0)
export ROOT_v5_11_02 = $(call get_root_version,5,11,2)
export ROOT_v5_12_00 = $(call get_root_version,5,12,0)
export ROOT_v5_13_06 = $(call get_root_version,5,13,6)
export ROOT_v5_17_00 = $(call get_root_version,5,17,0)
export ROOT_v5_20_00 = $(call get_root_version,5,20,0)

#ganil libraries for reading raw data only build on linux systems
#+ extensions for VAMOS data
ifeq ($(PLATFORM),linux)
export INDRADLT = yes
RGTAPE = gan_tape ROOTGT
INDRAVAMOS = VAMOS
else
export INDRADLT = no
RGTAPE =
INDRAVAMOS = VAMOS
endif

#need to make tarball & install on website, if at CC-IN2P3
ifeq ($(SITE),CCIN2P3)
CCALI = dist html_ccali
else
CCALI = html
endif

#for logs
OLD_TAG = init
NEW_TAG = HEAD

export KV_BUILD_DATE = $(shell date +%F)
DATE_RECORD_FILE = $(KV_BUILD_DATE).date
ROOT_VERSION_TAG = .root_v$(ROOT_VERSION_CODE)
export KV_CONFIG__H = KVConfig.h

.PHONY : MultiDet Indra gan_tape ROOTGT VAMOS Indra5 clean cleangantape logs unpack install init analysis FNL html html_ccali byebye distclean

all : init $(KV_CONFIG__H) KVVersion.h MultiDet $(RGTAPE) Indra $(INDRAVAMOS) Indra5 FNL install analysis byebye

doc : $(CCALI) byebye

export GANILTAPE_INC = $(KVPROJ_ROOT_ABS)/ROOTGanilTape/include
export GANTAPE_INC = $(KVPROJ_ROOT_ABS)/GanTape/include
export GANILTAPE_LIB = $(KVPROJ_ROOT_ABS)/ROOTGanilTape/lib

export VERSION_NUMBER = $(shell cat VERSION)
KV_DIST = KaliVeda-$(VERSION_NUMBER)-$(KV_BUILD_DATE)
export CVS2CL = ../cvs2cl

init :
	-mkdir -p $(INSTALL)/lib
	-mkdir -p $(INSTALL)/bin
	-mkdir -p $(INSTALL)/include

KVVersion.h : VERSION $(DATE_RECORD_FILE)
	@echo '#define KV_VERSION "$(VERSION_NUMBER)-$(KV_BUILD_DATE)"' > KVVersion.h;\
	echo '#define KV_BUILD_DATE "$(KV_BUILD_DATE)"' >> KVVersion.h;\
	echo '#define KV_BUILD_USER "$(USER)"' >> KVVersion.h;\
	echo '#define KV_SOURCE_DIR "$(KVPROJ_ROOT_ABS)"' >> KVVersion.h

$(DATE_RECORD_FILE) :
	@if test ! -f $@; then \
	  rm -f *.date; \
	  touch $@; \
	else :; fi

$(KV_CONFIG__H) : $(ROOT_VERSION_TAG)
	$(MAKE) -f Makefile.compat
	-cp $@ $(INSTALL)/include/
		
$(ROOT_VERSION_TAG) :
	@if test ! -f $@; then \
	  rm -f .root_v*; \
	  touch $@; \
	else :; fi

gan_tape : init
	cd GanTape && ./make_linux_i386

ROOTGT : init
	cd ROOTGanilTape && $(MAKE)
		
MultiDet : init
	cd KVMultiDet && $(MAKE)

Indra : init
	cd KVIndra && $(MAKE)

Indra5 : init
	cd KVIndra5 && $(MAKE)

FNL : init
	cd KVIndraFNL && $(MAKE)

analysis : init
	cd analysis && $(MAKE)
	
VAMOS : init
	cd VAMOS && $(MAKE)

html :
	cd html && $(MAKE) install_html debug=$(debug)
		
html_ccali :
	cd html && $(MAKE) install_html
	-rm -f $(KVROOT)/KaliVedaDoc/*.tgz
	-cp $(KV_DIST).tgz $(KVROOT)/KaliVedaDoc/$(KV_DIST).tgz

cleangantape :
	cd ROOTGanilTape && $(MAKE) clean
	cd GanTape && rm -rf i386-linux_*
	
clean :
	-rm -f $(KVPROJ_ROOT_ABS)/KVVersion.h
	-rm -f $(KVPROJ_ROOT_ABS)/KVConfig.h
	cd KVMultiDet && $(MAKE) clean
	cd KVIndra && $(MAKE) clean
ifeq ($(INDRADLT),yes)
	cd ROOTGanilTape && $(MAKE) clean
	cd GanTape && rm -rf i386-linux_*
endif
	cd KVIndra5 && $(MAKE) clean
	cd KVIndraFNL && $(MAKE) clean
	cd VAMOS && $(MAKE) clean
	cd analysis && $(MAKE) clean
	cd html && $(MAKE) clean

distclean : clean
	-rm -f $(INSTALL)/KVFiles/*/DataBase.root
	-rm -f $(HOME)/.KVDataAnalysisGUIrc*
		
install :
	-mkdir -p $(INSTALL)/src
	-mkdir -p $(INSTALL)/KVFiles
	-mkdir -p $(INSTALL)/db
	-mkdir -p $(INSTALL)/examples
	-mkdir -p $(INSTALL)/tools
	cd KVMultiDet && $(MAKE) install
ifeq ($(INDRADLT),yes)
	cd ROOTGanilTape && $(MAKE) install
endif
	cd KVIndra && $(MAKE) install
	cd KVIndra5 && $(MAKE) install
	cd KVIndraFNL && $(MAKE) install
	cd VAMOS && $(MAKE) install
	-cp html/tools/.nedit html/tools/SetUpKaliVeda.csh html/tools/SetUpKaliVedaDirectories.sh html/tools/SetUpROOT.csh html/tools/SetUpROOTDirectories.sh html/tools/WhichKaliVeda html/tools/WhichROOT $(INSTALL)/tools/
	-cp html/examples/*.C html/examples/*.cpp html/examples/*.h $(INSTALL)/examples/
	-cp etc/KaliVeda.rootrc $(INSTALL)/KVFiles/.kvrootrc
ifeq ($(SITE),CCIN2P3)
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/ccali.available.datasets $(INSTALL)/KVFiles/ccali.available.datasets
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp1/available_runs.campagne1.raw $(INSTALL)/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp1/available_runs.campagne1.root $(INSTALL)/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp2/available_runs.campagne2.raw $(INSTALL)/KVFiles/INDRA_camp2/ccali.available_runs.campagne2.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp2/available_runs.campagne2.root $(INSTALL)/KVFiles/INDRA_camp2/ccali.available_runs.campagne2.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp4/available_runs.campagne4.raw $(INSTALL)/KVFiles/INDRA_camp4/ccali.available_runs.campagne4.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp4/available_runs.campagne4.root $(INSTALL)/KVFiles/INDRA_camp4/ccali.available_runs.campagne4.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.raw $(INSTALL)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.recon $(INSTALL)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.ident $(INSTALL)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.root $(INSTALL)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.raw $(INSTALL)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.recon $(INSTALL)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.ident $(INSTALL)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.root $(INSTALL)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.raw $(INSTALL)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.recon $(INSTALL)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.ident $(INSTALL)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.root $(INSTALL)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.raw $(INSTALL)/KVFiles/INDRA_e503/ccali.available_runs.e503.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.recon $(INSTALL)/KVFiles/INDRA_e503/ccali.available_runs.e503.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.ident $(INSTALL)/KVFiles/INDRA_e503/ccali.available_runs.e503.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.root $(INSTALL)/KVFiles/INDRA_e503/ccali.available_runs.e503.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.raw $(INSTALL)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.recon $(INSTALL)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.ident $(INSTALL)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.root $(INSTALL)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.root
endif
	
uninstall :
	-rm -rf $(INSTALL)/tools
	-rm -rf $(INSTALL)/examples
	-rm -rf $(INSTALL)/KaliVedaDoc
	-rm -rf $(INSTALL)/db
ifeq ($(INDRADLT),yes)
	-rm -f $(INSTALL)/include/GT*.H
	-rm -f $(INSTALL)/src/GT*.cpp
	-rm -f $(INSTALL)/lib/libROOTGanilTape.so
	-rm -f $(INSTALL)/lib/libgan_tape.a
endif
	cd KVMultiDet && $(MAKE) uninstall
	cd KVIndra && $(MAKE) uninstall
	cd KVIndra5 && $(MAKE) uninstall
	cd KVIndraFNL && $(MAKE) uninstall
	cd KVIndra && $(MAKE) uninstall-indra2root
	cd VAMOS && $(MAKE) uninstall
	cd analysis && $(MAKE) uninstall
	cd KVMultiDet && $(MAKE) removemoduledirs
	cd KVIndra && $(MAKE) removemoduledirs
	cd KVIndra5 && $(MAKE) removemoduledirs
	cd KVIndraFNL && $(MAKE) removemoduledirs
	cd VAMOS && $(MAKE) removemoduledirs
	-rm -rf $(INSTALL)/KVFiles
		
dist : clean
	tar -czf libKVMultiDet-$(VERSION_NUMBER).tgz KVMultiDet
	tar -czf libKVIndra-$(VERSION_NUMBER).tgz KVIndra
	tar -czf libKVIndra5-$(VERSION_NUMBER).tgz KVIndra5
	tar -czf libKVIndraFNL-$(VERSION_NUMBER).tgz KVIndraFNL
	tar -czf libVAMOS-$(VERSION_NUMBER).tgz VAMOS
	tar -czf analysis-$(VERSION_NUMBER).tgz analysis
	tar -czf html-$(VERSION_NUMBER).tgz html
	-mkdir $(KV_DIST)
	-cp libKV*.tgz $(KV_DIST)/
	-cp libVAMOS-$(VERSION_NUMBER).tgz $(KV_DIST)/
	-cp analysis*.tgz $(KV_DIST)/
	-cp html*.tgz $(KV_DIST)/
	-cp -r CVS $(KV_DIST)/
	-cp -r etc $(KV_DIST)/
	-cp -r cvs2cl $(KV_DIST)/
	-cp -r ROOTGanilTape $(KV_DIST)/
	-cp -r GanTape $(KV_DIST)/
	-cp Makefile* $(KV_DIST)/
	-cp VERSION $(KV_DIST)/
	-cp INSTALL $(KV_DIST)/
	-tar czf $(KV_DIST).tgz $(KV_DIST)
	-rm -Rf $(KV_DIST)
	-rm -Rf lib*.tgz
	-rm -f analysis-$(VERSION_NUMBER).tgz
	-rm -f html-$(VERSION_NUMBER).tgz
	
unpack :
	@echo Unpacking compressed archives
	-tar zxf libKVMultiDet-$(VERSION_NUMBER).tgz 
	-tar zxf libKVIndra-$(VERSION_NUMBER).tgz 
	-tar zxf libKVIndra5-$(VERSION_NUMBER).tgz 
	-tar zxf libKVIndraFNL-$(VERSION_NUMBER).tgz 
	-tar zxf libVAMOS-$(VERSION_NUMBER).tgz
	-tar zxf analysis-$(VERSION_NUMBER).tgz
	-tar zxf html-$(VERSION_NUMBER).tgz
	-rm libKVMultiDet-$(VERSION_NUMBER).tgz 
	-rm libKVIndra-$(VERSION_NUMBER).tgz 
	-rm libKVIndraFNL-$(VERSION_NUMBER).tgz 
	-rm libKVIndra5-$(VERSION_NUMBER).tgz 
	-rm libVAMOS-$(VERSION_NUMBER).tgz
	-rm analysis-$(VERSION_NUMBER).tgz
	-rm html-$(VERSION_NUMBER).tgz
	
config :
	@echo "--------------------------------------------------------------------------------"
	@echo "CONFIGURATION OF MAKEFILE FOR KALIVEDA"
	@echo "--------------------------------------------------------------------------------"
	@echo "KVPROJ_ROOT_ABS = "$(KVPROJ_ROOT_ABS)
	@echo "--------------------------------------------------------------------------------"
	@echo "INSTALL = "$(INSTALL)
	@echo "--------------------------------------------------------------------------------"
	@echo "ROOT_VERSION_CODE = "$(ROOT_VERSION_CODE)
	@echo "--------------------------------------------------------------------------------"

indent :
	cd KVMultiDet && $(MAKE) indent
	cd KVIndra && $(MAKE) indent
	cd KVIndra5 && $(MAKE) indent
	cd KVIndraFNL && $(MAKE) indent
	
byebye :
	@echo ''
	@echo 'NORMAL SUCCESSFUL COMPLETION OF MAKEFILE'
	@echo ''	


logs :
	cvs2cl/cvs2cl.pl --delta $(OLD_TAG):$(NEW_TAG) --xml --xml-encoding iso-8859-15 --noxmlns -f ChangeLog.xml
	xsltproc -o ChangeLog.html cvs2cl/cl2html-ciaglia.xslt ChangeLog.xml
	-cp ChangeLog.html $(KVPROJ_ROOT_ABS)/html/
	cd html && ./stripchangelog ChangeLog.html $(NEW_TAG) KaliVedaChangeLog_$(OLD_TAG)_$(NEW_TAG).html
	-cp $(KVPROJ_ROOT_ABS)/html/KaliVedaChangeLog_$(OLD_TAG)_$(NEW_TAG).html $(INSTALL)/KaliVedaDoc/KaliVedaChangeLog_$(OLD_TAG)_$(NEW_TAG).html
	-rm -f $(KVPROJ_ROOT_ABS)/html/ChangeLog.html
	-rm -f $(KVPROJ_ROOT_ABS)/html/KaliVedaChangeLog_$(OLD_TAG)_$(NEW_TAG).html

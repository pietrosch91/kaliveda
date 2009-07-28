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
export KVINSTALLDIR = $(shell cygpath -u '$(KVROOT)')
else
export KVINSTALLDIR = $(KVROOT)
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

ifeq ($(WITH_VAMOS),no)
INDRAVAMOS = 
endif

export KV_BUILD_DATE = $(shell date +%F)
DATE_RECORD_FILE = $(KV_BUILD_DATE).date
ROOT_VERSION_TAG = .root_v$(ROOT_VERSION_CODE)
export KV_CONFIG__H = KVConfig.h

.PHONY : MultiDet Indra gan_tape ROOTGT VAMOS Indra5 clean cleangantape unpack install analysis FNL html html_ccali byebye distclean

all : fitltg-0.1/configure .init $(KV_CONFIG__H) KVVersion.h ltgfit MultiDet $(RGTAPE) Indra $(INDRAVAMOS) Indra5 FNL install analysis byebye

doc : html byebye

export GANILTAPE_INC = $(KVPROJ_ROOT_ABS)/ROOTGanilTape/include
export GANTAPE_INC = $(KVPROJ_ROOT_ABS)/GanTape/include
export GANILTAPE_LIB = $(KVPROJ_ROOT_ABS)/ROOTGanilTape/lib

export VERSION_NUMBER = $(shell cat VERSION)
KV_DIST = KaliVeda-$(VERSION_NUMBER)-$(KV_BUILD_DATE)

fitltg-0.1/configure: fitltg-0.1/configure.ac 
	cd fitltg-0.1 && autoreconf -ivf

.init :
	-mkdir -p $(KVINSTALLDIR)/lib
	-mkdir -p $(KVINSTALLDIR)/bin
	-mkdir -p $(KVINSTALLDIR)/include
	cd fitltg-0.1 && ./configure --prefix=$(KVINSTALLDIR)
	touch .init

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
	@echo 'Updating KVConfig.h'
	$(MAKE) -f Makefile.compat
		
$(ROOT_VERSION_TAG) :
	@if test ! -f $@; then \
	  echo 'Updating ROOT_VERSION_TAG'; \
	  rm -f .root_v*; \
	  touch $@; \
	else :; fi

gan_tape : .init
	cd GanTape && ./make_linux_i386

ROOTGT : .init
	cd ROOTGanilTape && $(MAKE)

ltgfit : .init
	cd fitltg-0.1 && make && make install
		
MultiDet : .init
	cd KVMultiDet && $(MAKE)

Indra : .init
	cd KVIndra && $(MAKE)

Indra5 : .init
	cd KVIndra5 && $(MAKE)

FNL : .init
	cd KVIndraFNL && $(MAKE)

analysis : .init
	cd analysis && $(MAKE)
	
VAMOS : .init
	cd VAMOS && $(MAKE)

html :
	cd html && $(MAKE) install_html debug=$(debug)
		
cleangantape :
	cd ROOTGanilTape && $(MAKE) clean
	cd GanTape && rm -rf i386-linux_*
	
clean :
	-rm -f $(KVPROJ_ROOT_ABS)/KVVersion.h
	-rm -f $(KVPROJ_ROOT_ABS)/KVConfig.h
	-rm -f .init
	cd KVMultiDet && $(MAKE) clean
	cd fitltg-0.1 && make clean
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
	-rm -f $(KVINSTALLDIR)/KVFiles/*/DataBase.root
	-rm -f $(HOME)/.KVDataAnalysisGUIrc*
		
install :
	-mkdir -p $(KVINSTALLDIR)/src
	-mkdir -p $(KVINSTALLDIR)/KVFiles
	-mkdir -p $(KVINSTALLDIR)/db
	-mkdir -p $(KVINSTALLDIR)/examples
	-mkdir -p $(KVINSTALLDIR)/tools
	cd KVMultiDet && $(MAKE) install
ifeq ($(INDRADLT),yes)
	cd ROOTGanilTape && $(MAKE) install
endif
	cd KVIndra && $(MAKE) install
	cd KVIndra5 && $(MAKE) install
	cd KVIndraFNL && $(MAKE) install
	cd VAMOS && $(MAKE) install
	-cp html/tools/.nedit html/tools/SetUpKaliVeda.csh html/tools/SetUpKaliVedaDirectories.sh html/tools/SetUpROOT.csh html/tools/SetUpROOTDirectories.sh html/tools/WhichKaliVeda html/tools/WhichROOT $(KVINSTALLDIR)/tools/
	-cp html/examples/*.C html/examples/*.cpp html/examples/*.h $(KVINSTALLDIR)/examples/
	-cp etc/KaliVeda.rootrc $(KVINSTALLDIR)/KVFiles/.kvrootrc
ifeq ($(SITE),CCIN2P3)
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/ccali.available.datasets $(KVINSTALLDIR)/KVFiles/ccali.available.datasets
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp1/available_runs.campagne1.raw $(KVINSTALLDIR)/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp1/available_runs.campagne1.root $(KVINSTALLDIR)/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp2/available_runs.campagne2.raw $(KVINSTALLDIR)/KVFiles/INDRA_camp2/ccali.available_runs.campagne2.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp2/available_runs.campagne2.root $(KVINSTALLDIR)/KVFiles/INDRA_camp2/ccali.available_runs.campagne2.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp4/available_runs.campagne4.raw $(KVINSTALLDIR)/KVFiles/INDRA_camp4/ccali.available_runs.campagne4.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp4/available_runs.campagne4.root $(KVINSTALLDIR)/KVFiles/INDRA_camp4/ccali.available_runs.campagne4.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.raw $(KVINSTALLDIR)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.recon $(KVINSTALLDIR)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.ident $(KVINSTALLDIR)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.root $(KVINSTALLDIR)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.raw $(KVINSTALLDIR)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.recon $(KVINSTALLDIR)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.ident $(KVINSTALLDIR)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.root $(KVINSTALLDIR)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.raw $(KVINSTALLDIR)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.recon $(KVINSTALLDIR)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.ident $(KVINSTALLDIR)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.root $(KVINSTALLDIR)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.raw $(KVINSTALLDIR)/KVFiles/INDRA_e503/ccali.available_runs.e503.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.recon $(KVINSTALLDIR)/KVFiles/INDRA_e503/ccali.available_runs.e503.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.ident $(KVINSTALLDIR)/KVFiles/INDRA_e503/ccali.available_runs.e503.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.root $(KVINSTALLDIR)/KVFiles/INDRA_e503/ccali.available_runs.e503.root
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.raw $(KVINSTALLDIR)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.recon $(KVINSTALLDIR)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.recon
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.ident $(KVINSTALLDIR)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.ident
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.root $(KVINSTALLDIR)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.root
endif
	
uninstall :
	-rm -rf $(KVINSTALLDIR)/tools
	-rm -rf $(KVINSTALLDIR)/examples
	-rm -rf $(KVINSTALLDIR)/KaliVedaDoc
	-rm -rf $(KVINSTALLDIR)/db
ifeq ($(INDRADLT),yes)
	-rm -f $(KVINSTALLDIR)/include/GT*.H
	-rm -f $(KVINSTALLDIR)/src/GT*.cpp
	-rm -f $(KVINSTALLDIR)/lib/libROOTGanilTape.so
	-rm -f $(KVINSTALLDIR)/lib/libgan_tape.a
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
	-rm -rf $(KVINSTALLDIR)/KVFiles
		
dist : clean
	cd fitltg-0.1 && make dist
	tar -czf libKVMultiDet-$(VERSION_NUMBER).tgz KVMultiDet
	tar -czf libKVIndra-$(VERSION_NUMBER).tgz KVIndra
	tar -czf libKVIndra5-$(VERSION_NUMBER).tgz KVIndra5
	tar -czf libKVIndraFNL-$(VERSION_NUMBER).tgz KVIndraFNL
	tar -czf libVAMOS-$(VERSION_NUMBER).tgz VAMOS
	tar -czf analysis-$(VERSION_NUMBER).tgz analysis
	tar -czf html-$(VERSION_NUMBER).tgz html
	-mkdir $(KV_DIST)
	-cp libKV*.tgz $(KV_DIST)/
	-cp fitltg-0.1/fitltg-0.1.tar.gz $(KV_DIST)/
	-cp libVAMOS-$(VERSION_NUMBER).tgz $(KV_DIST)/
	-cp analysis*.tgz $(KV_DIST)/
	-cp html*.tgz $(KV_DIST)/
	-cp -r etc $(KV_DIST)/
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
	-tar zxf fitltg-0.1.tar.gz 
	-tar zxf libKVMultiDet-$(VERSION_NUMBER).tgz 
	-tar zxf libKVIndra-$(VERSION_NUMBER).tgz 
	-tar zxf libKVIndra5-$(VERSION_NUMBER).tgz 
	-tar zxf libKVIndraFNL-$(VERSION_NUMBER).tgz 
	-tar zxf libVAMOS-$(VERSION_NUMBER).tgz
	-tar zxf analysis-$(VERSION_NUMBER).tgz
	-tar zxf html-$(VERSION_NUMBER).tgz
	-rm fitltg-0.1.tar.gz
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
	@echo "KVINSTALLDIR = "$(KVINSTALLDIR)
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

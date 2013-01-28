#General Makefile for the entire KaliVeda project

###########################################
#                                                                        #
#   DO NOT CHANGE ANY OF THE FOLLOWING   #
#                                                                        #
###########################################

ARCH         := $(shell root-config --arch)
PLATFORM     := $(shell root-config --platform)

#split ROOT version into major, minor and release version numbers
rootvers:= $(shell root-config --version)
empty:=
space:= $(empty) $(empty)
rootvers1:=$(subst .,$(space),$(rootvers))
rootvers2:=$(subst /,$(space),$(rootvers1))
rootvers3:=$(subst -,$(space),$(rootvers2))
root_maj := $(word 1,$(rootvers3))
root_min := $(word 2,$(rootvers3))
root_rel := $(word 3,$(rootvers3))
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
export ROOT_v5_29_01 = $(call get_root_version,5,29,1)
export ROOT_v5_32_00 = $(call get_root_version,5,32,0)

#By default, we use the system-dependent definitions contained in the ROOT
#makefile found (for standard installations) in
#        $ROOTSYS/test/Makefile.arch     (versions up to 5.30)
#        $ROOTSYS/etc/Makefile.arch      (versions from 5.32)
ifeq ($(shell expr $(ROOT_VERSION_CODE) \< $(ROOT_v5_32_00)),1)
export ROOT_MAKEFILE_PATH = $(ROOTSYS)/test
else
export ROOT_MAKEFILE_PATH = $(shell root-config --etcdir)
endif
#If ROOT is not installed in a single directory (ROOTSYS) you must
#give the path to the Makefile by invoking make with the ROOT_MAKEFILE_PATH argument:
#
#        $>  make ROOT_MAKEFILE_PATH=/usr/local/share/root/test     (versions up to 5.30)
# OR $>  make ROOT_MAKEFILE_PATH=/usr/local/share/root/etc      (versions from 5.32)
#
#which will override the previous definition

# for compilation with gru lib
export WITH_GRU_LIB = no
export GRU_DIR = /home/acqexp/GRU/GRUcurrent

# for compilation with Ricardo Yanez's 'range' dE/dX and range library
export WITH_RANGE_YANEZ = no
export RANGE_YANEZ_DIR = /usr/local
# set this to 'yes' when using specially modified version of range library
# with special treatment of gaseous materials and icor=2 interpolation
# between NCS and HBG tables
export WITH_MODIFIED_RANGE_YANEZ = no

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

#ganil libraries for reading raw data only build on linux systems
#+ extensions for VAMOS data
ifeq ($(PLATFORM),linux)
export ROOTGANILTAPE = yes
RGTAPE = gan_tape
INDRAVAMOS = VAMOS
else
export ROOTGANILTAPE = no
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

BZR = $(shell which bzr)
ifneq ($(BZR),)
BZR_INFOS = KVBzrInfo.h
BZR_LAST_REVISION = $(shell if [ -f .bzr/branch/last-revision ]; then echo '.bzr/branch/last-revision'; fi)
BZR_BRANCH_NICK = $(shell bzr info | grep 'checkout of branch' | awk '{ print $$4 }')
ifeq ($(BZR_BRANCH_NICK),)
BZR_BRANCH_NICK = $(shell bzr info | grep 'parent branch' | awk '{ print $$3 }')
endif
else
BZR_INFOS =
BZR_LAST_REVISION =
endif

.PHONY : changelog MultiDet Indra Spectrometer gan_tape VAMOS clean cleangantape unpack install analysis html html_ccali byebye distclean

all : fitltg-0.1/configure .init $(KV_CONFIG__H) KVVersion.h $(BZR_INFOS) ltgfit $(RGTAPE) MultiDet Indra Spectrometer $(INDRAVAMOS) install analysis byebye

doc : html byebye

export GANTAPE_INC = $(KVPROJ_ROOT_ABS)/GanTape/include

export VERSION_NUMBER = $(shell cat VERSION)
KV_DIST = KaliVeda-$(VERSION_NUMBER)-$(KV_BUILD_DATE)

changelog :
	@bzr log --forward --short -v -n0 -r$(oldrev).. > changelog_$(VERSION_NUMBER).txt

fitltg-0.1/configure: fitltg-0.1/configure.ac 
	cd fitltg-0.1 && autoreconf -ivf

.init :
	-mkdir -p $(KVINSTALLDIR)/lib
	-mkdir -p $(KVINSTALLDIR)/bin
	-mkdir -p $(KVINSTALLDIR)/include
	cd fitltg-0.1 && ./configure --prefix=$(KVINSTALLDIR)
	touch .init

KVVersion.h : VERSION $(DATE_RECORD_FILE)
	@echo '#define KV_VERSION "$(VERSION_NUMBER)"' > $@;\
	echo '#define KV_BUILD_DATE "$(KV_BUILD_DATE)"' >> $@;\
	echo '#define KV_BUILD_USER "$(USER)"' >> $@;\
	echo '#define KV_SOURCE_DIR "$(KVPROJ_ROOT_ABS)"' >> $@

$(DATE_RECORD_FILE) :
	@if test ! -f $@; then \
	  rm -f *.date; \
	  touch $@; \
	else :; fi

$(KV_CONFIG__H) : $(ROOT_VERSION_TAG)
	@echo 'Updating KVConfig.h'
	$(MAKE) -f Makefile.compat debug=$(debug)
		
$(ROOT_VERSION_TAG) :
	@if test ! -f $@; then \
	  echo 'Updating ROOT_VERSION_TAG'; \
	  rm -f .root_v*; \
	  touch $@; \
	else :; fi
	
$(BZR_INFOS) : $(BZR_LAST_REVISION)
	@bzr version-info --custom --template="#define BZR_REVISION_ID "\""{revision_id}"\"" \n#define BZR_REVISION_DATE "\""{date}"\"" \n#define BZR_REVISION_NUMBER {revno}\n#define BZR_BRANCH_IS_CLEAN {clean}\n" \
		> $@
	@echo '#define BZR_BRANCH_NICK "$(BZR_BRANCH_NICK)"' >> $@

gan_tape : .init
	cd GanTape && ./make_linux_i386
	cp GanTape/i386-linux_lib/libgan_tape.so $(KVINSTALLDIR)/lib/

ltgfit : .init
	cd fitltg-0.1 && make && make install
		
MultiDet : .init
	cd KVMultiDet && $(MAKE)

Indra : .init
	cd KVIndra && $(MAKE)

Spectrometer : .init
	cd KVSpectrometer && $(MAKE)

analysis : .init
	cd analysis && $(MAKE)
	
VAMOS : .init
	cd VAMOS && $(MAKE)

html :
	cd html && $(MAKE) install_html debug=$(debug)
		
cleangantape :
	cd GanTape && rm -rf i386-linux_*
	
clean :
	-rm -f $(KVPROJ_ROOT_ABS)/KVVersion.h
	-rm -f $(KVPROJ_ROOT_ABS)/KVConfig.h
	-rm -f .init
	cd KVMultiDet && $(MAKE) clean
	cd fitltg-0.1 && make clean
	cd KVIndra && $(MAKE) clean
	cd KVSpectrometer && $(MAKE) clean
ifeq ($(ROOTGANILTAPE),yes)
	cd GanTape && rm -rf i386-linux_*
endif
	cd VAMOS && $(MAKE) clean
	cd analysis && $(MAKE) clean
	cd html && $(MAKE) clean

distclean : clean
	-rm -f $(KVINSTALLDIR)/KVFiles/*/DataBase.root
	-rm -f $(HOME)/.KVDataAnalysisGUIrc*
		
install :
	-chmod 0775 $(KVINSTALLDIR)
	-mkdir -p $(KVINSTALLDIR)/src
	-mkdir -p $(KVINSTALLDIR)/KVFiles
	-mkdir -p $(KVINSTALLDIR)/db
	-mkdir -p $(KVINSTALLDIR)/examples
	-mkdir -p $(KVINSTALLDIR)/tools
	cd KVMultiDet && $(MAKE) install
	cd KVIndra && $(MAKE) install
	cd KVSpectrometer && $(MAKE) install
	cd VAMOS && $(MAKE) install
	-cp tools/.nedit $(KVINSTALLDIR)/tools/
	-cp tools/* $(KVINSTALLDIR)/tools/
	-cp html/examples/*.C html/examples/*.cpp html/examples/*.h $(KVINSTALLDIR)/examples/
ifeq ($(SITE),CCIN2P3)
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp1/available_runs.campagne1.raw $(KVINSTALLDIR)/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.recon2 $(KVINSTALLDIR)/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.recon2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.root2 $(KVINSTALLDIR)/KVFiles/INDRA_camp1/ccali.available_runs.campagne1.root2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp2/available_runs.campagne2.raw $(KVINSTALLDIR)/KVFiles/INDRA_camp2/ccali.available_runs.campagne2.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp2/ccali.available_runs.campagne2.root2 $(KVINSTALLDIR)/KVFiles/INDRA_camp2/ccali.available_runs.campagne2.root2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp4/available_runs.campagne4.raw $(KVINSTALLDIR)/KVFiles/INDRA_camp4/ccali.available_runs.campagne4.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp4/ccali.available_runs.campagne4.root2 $(KVINSTALLDIR)/KVFiles/INDRA_camp4/ccali.available_runs.campagne4.root2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/available_runs.campagne5.raw $(KVINSTALLDIR)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.recon2 $(KVINSTALLDIR)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.recon2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.ident2 $(KVINSTALLDIR)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.ident2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.root2 $(KVINSTALLDIR)/KVFiles/INDRA_camp5/ccali.available_runs.campagne5.root2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/available_runs.e416a.raw $(KVINSTALLDIR)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/ccali.available_runs.e416a.recon2 $(KVINSTALLDIR)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.recon2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/ccali.available_runs.e416a.ident2 $(KVINSTALLDIR)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.ident2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e416a/ccali.available_runs.e416a.root2 $(KVINSTALLDIR)/KVFiles/INDRA_e416a/ccali.available_runs.e416a.root2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/available_runs.e475s.raw $(KVINSTALLDIR)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/ccali.available_runs.e475s.recon2 $(KVINSTALLDIR)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.recon2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/ccali.available_runs.e475s.ident2 $(KVINSTALLDIR)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.ident2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e475s/ccali.available_runs.e475s.root2 $(KVINSTALLDIR)/KVFiles/INDRA_e475s/ccali.available_runs.e475s.root2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/available_runs.e503.raw $(KVINSTALLDIR)/KVFiles/INDRA_e503/ccali.available_runs.e503.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/ccali.available_runs.e503.recon2 $(KVINSTALLDIR)/KVFiles/INDRA_e503/ccali.available_runs.e503.recon2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/ccali.available_runs.e503.ident2 $(KVINSTALLDIR)/KVFiles/INDRA_e503/ccali.available_runs.e503.ident2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e503/ccali.available_runs.e503.root2 $(KVINSTALLDIR)/KVFiles/INDRA_e503/ccali.available_runs.e503.root2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/available_runs.e494s.raw $(KVINSTALLDIR)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/ccali.available_runs.e494s.recon2 $(KVINSTALLDIR)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.recon2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/ccali.available_runs.e494s.ident2 $(KVINSTALLDIR)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.ident2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e494s/ccali.available_runs.e494s.root2 $(KVINSTALLDIR)/KVFiles/INDRA_e494s/ccali.available_runs.e494s.root2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e613/ccali.available_runs.e613.raw $(KVINSTALLDIR)/KVFiles/INDRA_e613/ccali.available_runs.e613.raw
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e613/ccali.available_runs.e613.recon2 $(KVINSTALLDIR)/KVFiles/INDRA_e613/ccali.available_runs.e613.recon2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e613/ccali.available_runs.e613.ident2 $(KVINSTALLDIR)/KVFiles/INDRA_e613/ccali.available_runs.e613.ident2
	-ln -s $(THRONG_DIR)/KaliVeda/KVFiles/INDRA_e613/ccali.available_runs.e613.root2 $(KVINSTALLDIR)/KVFiles/INDRA_e613/ccali.available_runs.e613.root2
	-cat etc/KaliVeda.rootrc etc/ccali.rootrc > $(KVINSTALLDIR)/KVFiles/.kvrootrc
else
	-cat etc/KaliVeda.rootrc etc/standard.rootrc > $(KVINSTALLDIR)/KVFiles/.kvrootrc
endif
	
uninstall :
	-rm -rf $(KVINSTALLDIR)/tools
	-rm -rf $(KVINSTALLDIR)/examples
	-rm -rf $(KVINSTALLDIR)/KaliVedaDoc
	-rm -rf $(KVINSTALLDIR)/db
ifeq ($(ROOTGANILTAPE),yes)
	-rm -f $(KVINSTALLDIR)/lib/libgan_tape.a
endif
	cd KVMultiDet && $(MAKE) uninstall
	cd KVIndra && $(MAKE) uninstall
	cd KVIndra && $(MAKE) uninstall-indra2root
	cd KVSpectrometer && $(MAKE) uninstall
	cd VAMOS && $(MAKE) uninstall
	cd analysis && $(MAKE) uninstall
	cd KVMultiDet && $(MAKE) removemoduledirs
	cd KVIndra && $(MAKE) removemoduledirs
	cd KVSpectrometer && $(MAKE) removemoduledirs
	cd VAMOS && $(MAKE) removemoduledirs
	-rm -rf $(KVINSTALLDIR)/KVFiles
		
dist : fitltg-0.1/configure .init clean $(BZR_INFOS)
	cd fitltg-0.1 && make dist
	tar -czf libKVMultiDet-$(VERSION_NUMBER).tgz KVMultiDet
	tar -czf libKVIndra-$(VERSION_NUMBER).tgz KVIndra
	tar -czf libKVSpectrometer-$(VERSION_NUMBER).tgz KVSpectrometer
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
	-cp -r tools $(KV_DIST)/
	-cp -r GanTape $(KV_DIST)/
	-cp Make* $(KV_DIST)/
	-cp VERSION $(KV_DIST)/
	-cp KVBzrInfo.h $(KV_DIST)/
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
	-tar zxf libKVSpectrometer-$(VERSION_NUMBER).tgz 
	-tar zxf libVAMOS-$(VERSION_NUMBER).tgz
	-tar zxf analysis-$(VERSION_NUMBER).tgz
	-tar zxf html-$(VERSION_NUMBER).tgz
	-rm fitltg-0.1.tar.gz
	-rm libKVMultiDet-$(VERSION_NUMBER).tgz 
	-rm libKVIndra-$(VERSION_NUMBER).tgz 
	-rm libKVSpectrometer-$(VERSION_NUMBER).tgz 
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
	
byebye :
	@echo ''
	@echo 'NORMAL SUCCESSFUL COMPLETION OF MAKEFILE'
	@echo ''	

# the following copied from GSL build
# neat way to set up symbolic links from current build directory
# to all the header files in other subdirectories of main project
#header_links :
#	HEADERLIST="../gsl*.h ../*/gsl*.h"; \
#	for h in $HEADERLIST; do \
#	  BASENAME=`basename $h`; \
#	  test -r $BASENAME || ln -s $h $BASENAME; \
#	done

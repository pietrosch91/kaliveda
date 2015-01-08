PROJECT = KVIndra
#MODULES = analysis calibration daq_cec db detectors events geometry gui identification \
#	idmaps idtelescopes indra particles
MODULES = daq_cec identification

.PHONY: all install clean $(MODULES)

all: $(MODULES)

install:
	@for module in $(MODULES); do \
		makefile=Makefile.$$module; \
		[ ! -f $$makefile ] && makefile=Makefile.$(PROJECT); \
		make -f $$makefile MODULE=$$module install ; if [ "$$?" -ne 0 ] ; then break ; fi ; \
	done

clean:
	@for module in $(MODULES); do \
		makefile=Makefile.$$module; \
		[ ! -f $$makefile ] && makefile=Makefile.$(PROJECT); \
		make -f $$makefile MODULE=$$module clean ; if [ "$$?" -ne 0 ] ; then break ; fi ; \
	done

analysis: indra db daq_cec detectors calibration idmaps particles identification events idtelescopes gui geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
calibration: detectors indra daq_cec idmaps particles identification events idtelescopes gui geometry db 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
daq_cec: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
db: indra daq_cec detectors calibration idmaps particles identification events idtelescopes gui geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
detectors: calibration indra db daq_cec idmaps particles identification events idtelescopes gui geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
events: identification detectors indra db daq_cec calibration idtelescopes idmaps particles gui geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
geometry: indra db daq_cec detectors calibration idmaps particles identification events idtelescopes gui 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
gui: analysis db daq_cec detectors idtelescopes identification idmaps particles events calibration geometry indra 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
identification: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
idmaps: particles identification events detectors indra db daq_cec calibration idtelescopes geometry gui 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
idtelescopes: detectors calibration indra db daq_cec idmaps particles identification events gui geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
indra: db daq_cec detectors calibration idmaps particles identification events idtelescopes gui geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
particles: identification events detectors indra db daq_cec calibration idtelescopes idmaps gui geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"

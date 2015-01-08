PROJECT = KVIndra
MODULES = analysis calibration daq_cec db events geometry gui identification idmaps idtelescopes indra particles

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

analysis:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="calibration daq_cec db events geometry gui identification idmaps idtelescopes indra particles"
calibration:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis daq_cec db events geometry gui identification idmaps idtelescopes indra particles"
daq_cec: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration db events geometry gui identification idmaps idtelescopes indra particles"
db:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec events geometry gui identification idmaps idtelescopes indra particles"
events:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db geometry gui identification idmaps idtelescopes indra particles"
geometry:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db events gui identification idmaps idtelescopes indra particles"
gui:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db events geometry identification idmaps idtelescopes indra particles"
identification: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db events geometry gui idmaps idtelescopes indra particles"
idmaps:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db events geometry gui identification idtelescopes indra particles"
idtelescopes:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db events geometry gui identification idmaps indra particles"
indra:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db events geometry gui identification idmaps idtelescopes particles"
particles:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db events geometry gui identification idmaps idtelescopes indra"

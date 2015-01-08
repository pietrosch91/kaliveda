PROJECT = KVIndra
MODULES = analysis calibration daq_cec db exp_events geometry gui identification indra

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
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="calibration daq_cec db exp_events geometry gui identification indra"
calibration:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis daq_cec db exp_events geometry gui identification indra"
daq_cec: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration db exp_events geometry gui identification indra"
db:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec exp_events geometry gui identification indra"
exp_events:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db geometry gui identification indra"
geometry:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db exp_events gui identification indra"
gui:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db exp_events geometry identification indra"
identification: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db exp_events geometry gui indra"
indra:
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="analysis calibration daq_cec db exp_events geometry gui identification"

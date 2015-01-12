PROJECT = KVIndra
MODULES = analysis calibration daq_cec db exp_events geometry gui identification

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

analysis: exp_events daq_cec geometry identification calibration db 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
calibration: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
daq_cec: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
db: exp_events daq_cec geometry identification calibration 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
exp_events: identification daq_cec geometry calibration 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
geometry: calibration 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
gui: analysis db exp_events daq_cec geometry calibration identification 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
identification: calibration geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"

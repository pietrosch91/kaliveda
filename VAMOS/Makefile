PROJECT = VAMOS
MODULES = exp_events daq_cec analysis base reconstruction identification geometry db calibration

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

geometry: base 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
calibration: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
reconstruction: base geometry 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
daq_cec: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
exp_events: calibration reconstruction geometry identification 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
analysis: calibration base exp_events geometry reconstruction identification 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
base: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
db: 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
identification: base reconstruction geometry calibration 
	@$(MAKE) -f ../BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"

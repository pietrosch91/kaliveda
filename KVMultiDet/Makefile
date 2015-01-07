PROJECT = KVMultiDet
MODULES = analysis calibration data_management events geometry identification particles \
	base daq_cec db exp_events globvars stopping gui trieur

.PHONY: all install clean $(MODULES)

all: $(MODULES)

install:
	@for module in $(MODULES); do \
		makefile=Makefile.$$module; \
		[ ! -f $$makefile ] && makefile=Makefile.KVMultiDet; \
		make -f $$makefile MODULE=$$module install ; if [ "$$?" -ne 0 ] ; then break ; fi ; \
	done

clean:
	@for module in $(MODULES); do \
		makefile=Makefile.$$module; \
		[ ! -f $$makefile ] && makefile=Makefile.KVMultiDet; \
		make -f $$makefile MODULE=$$module clean ; if [ "$$?" -ne 0 ] ; then break ; fi ; \
	done

base:
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
daq_cec: base 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
particles: base 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
identification: particles base 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
events: particles base 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
stopping: events base particles
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
geometry: events base stopping particles 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
calibration: geometry base stopping particles 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
db:stopping particles events base 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
data_management: base db stopping particles events 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
globvars: events base particles 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
trieur: base 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
analysis: data_management db stopping globvars base events particles 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
exp_events: analysis globvars daq_cec identification base particles geometry stopping events data_management db 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"
gui: analysis exp_events geometry data_management db stopping events base identification particles 
	@$(MAKE) -f BuildModule.mk PROJECT=$(PROJECT) MODULE=$@ MOD_DEPS="$^"


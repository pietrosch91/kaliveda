
############################################################################
# Compilation in C                                                         *
#                                                                          *
############################################################################

$(LIBDIR)mon_traitement.o : $(SRCDIR)mon_traitement.c \
			$(INCDIR)GEN_TYPE.H \
			$(INCDIR)STR_EVT.H \
			$(INCDIR)gan_acq_buf.h\
			$(INCDIR)gan_tape_erreur.h
	$(CC) $(DEBUG) $(SRCDIR)mon_traitement.c

#$(LIBDIR)mon_traitement.o : $(SRCDIR)mon_traitement.for
#	$(FOR) $(DEBUG) $(SRCDIR)mon_traitement.for

lib_LIBRARIES = libfitltg.a
libfitltg_a_SOURCES = fit_ede.c fit_ede_f.f get_index_var.c fit_ede.h
include_HEADERS = fit_ede.h
libfitltg_a_LIBADD = 

check_PROGRAMS = testfitltg
testfitltg_SOURCES = pgm.c
testfitltg_LDADD = libfitltg.a $(FLIBS)

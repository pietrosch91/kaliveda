lib_LTLIBRARIES = libfitltg.la
libfitltg_la_SOURCES = fede.c fit_ede.c fit_ede_f.f
include_HEADERS = fit_ede.h
libfitltg_la_LIBADD = $(FLIBS)

check_PROGRAMS = testfitltg
testfitltg_SOURCES = pgm.c
testfitltg_LDADD = libfitltg.la

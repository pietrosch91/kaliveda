#---CheckFitLTG module
#
#---compilation requires Fortran compiler & C-Fortran interface capabilities

##---make sure a Fortran compiler is available
include(CheckLanguage)

check_language(Fortran)

if(CMAKE_Fortran_COMPILER)

	enable_language(Fortran)
	#---check C-Fortran interface capabilities
	include(FortranCInterface)
	FortranCInterface_VERIFY(QUIET)
	
	if(NOT FortranCInterface_VERIFIED_C)
		message(STATUS "Problem with C/Fortran interfacing. Disabling fitltg option.")
		set(USE_FITLTG OFF CACHE BOOL "" FORCE)
	endif()
	
else(CMAKE_Fortran_COMPILER)

	message(STATUS "No Fortran compiler available. Disabling fitltg option.")
	set(USE_FITLTG OFF CACHE BOOL "" FORCE)
	
endif(CMAKE_Fortran_COMPILER)

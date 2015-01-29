cmake_minimum_required (VERSION 2.6)

#---make sure a Fortran compiler is available
include(CheckLanguage)
check_language(Fortran)
if(CMAKE_Fortran_COMPILER)
	enable_language(Fortran)
else()
	message(FATAL_ERROR "No Fortran support")
endif()

project(FitLTG C Fortran)

#---make sure that Fortran/C interface is available
include(FortranCInterface)
FortranCInterface_VERIFY()
FortranCInterface_HEADER(FC.h MACRO_NAMESPACE "FC_")

add_subdirectory(src)

enable_testing()
add_test(NAME TestFit
	COMMAND testfitltg ${PROJECT_SOURCE_DIR}/testdata/ChioSi_new.dat)

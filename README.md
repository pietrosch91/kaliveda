# KaliVeda Data Analysis Toolkit

KaliVeda is an object-oriented toolkit based on ROOT for the analysis of heavy-ion collisions in the Fermi energy domain.

## Build & Install

See INSTALL file for details. ROOT v5.34 or v6.10 are recommended. CMake (v2.8.11 or greater) is required.

## Use in ROOT interactive session

The 'kaliveda' command launches a ROOT session with dynamic shared library paths set up so that all classes will be loaded as & when needed by the ROOT interpreter (either Cint or Cling). Example of use:

    $ kaliveda
    ***********************************************************
    *                    HI COQUINE !!!                       *
    *                                                         *
    *         W E L C O M E     to     K A L I V E D A        *
    *                                                         *
    * Version: 1.10/15                      Built: 2018-03-27 *
    * git: heads/1.10@release-1.10.15-173-gb31f90df           *
    *                                                         *
    *               For help, see the website :               *
    *             http://indra.in2p3.fr/kaliveda              *
    *                                                         *
    *                          ENJOY !!!                      *
    ***********************************************************
    kaliveda [0] 

## Compiling & linking with KaliVeda & ROOT libraries

    $ g++ `root-config --cflags` -I`kaliveda-config --incdir` -c MyCode.cxx
    $ g++ `root-config --ldflags` MyCode.o -L`kaliveda-config --libdir` `kaliveda-config --libs` `root-config --glibs`

## Use in CMake-based project

Given a C++ file using KaliVeda classes such as toto.cpp:

    #include "KVBase.h"
    int main()
    {
       KVBase::InitEnvironment();
       return 0;
    }

You can compile and link this executable with the following CMakeLists.txt file:

    cmake_minimum_required(VERSION 2.8.11)
    project(toto)
    find_package(KaliVeda REQUIRED)
    include(${KALIVEDA_USE_FILE})
    find_package(ROOT REQUIRED)
    include(${ROOT_USE_FILE})
    add_executable(toto toto.cpp)
    target_link_libraries(toto ${KALIVEDA_LIBRARIES})

Build the executable 'toto' by doing:

    $ mkdir build && cd build
    $ cmake ..
    $ make

See the wiki page https://github.com/kaliveda-dev/kaliveda/wiki/Using-KaliVeda-in-a-CMake-based-project for more detailed information.

## More information

See the website http://indra.in2p3.fr/kaliveda



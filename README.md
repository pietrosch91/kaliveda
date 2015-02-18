# KaliVeda Data Analysis Toolkit

KaliVeda is an object-oriented toolkit based on ROOT for the analysis of heavy-ion collisions in the Fermi energy domain.

## Build & Install

See INSTALL file for details. CMake (v2.8 or greater) is required.

## Use in ROOT interactive session

Make sure path to installed libraries is in LD_LIBRARY_PATH, then all classes will be loaded as & when needed by the ROOT interpreter (either Cint or Cling). For example:

    root[0] KVBase::InitEnvironment()
    Info in <KVBase::InitEnvironment>: Initialising KaliVeda environment...
    Info in <KVBase::InitEnvironment>: Using KaliVeda version 1.10.00 built on 2015-02-10
    Info in <KVBase::InitEnvironment>: (git : heads/1.10@079a949)

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

    cmake_minimum_required(VERSION 2.8)
    project(toto)
    find_package(ROOT REQUIRED)
    include(${ROOT_USE_FILE})
    find_package(KaliVeda REQUIRED)
    include(${KALIVEDA_USE_FILE})
    add_executable(toto toto.cpp)
    target_link_libraries(toto KVMultiDetbase)

Build the executable 'toto' by doing:

    $ mkdir build && cd build
    $ cmake ..
    $ make

## More information

See the website http://indra.in2p3.fr/KaliVedaDoc

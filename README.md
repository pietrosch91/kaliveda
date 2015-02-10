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

## More information

See the website http://indra.in2p3.fr/KaliVedaDoc

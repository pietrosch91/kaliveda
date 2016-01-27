// Author: Peter C. Wigg
// Created Wed Jun 11 15:09:35 CEST 2014

////////////////////////////////////////////////////////////////////////////////
//
// KVIVUpDater_e503.h
//
// Description
//
// KVIVUpDater class for the E503 experiment. This class inherits from that of
// our sister experiment (E494s) and adds the functionality required to
// implement the VAMOS identification corrections.
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Wed Jun 11 15:09:35 CEST 2014
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __KVIVUPDATER_E503_H__
#define __KVIVUPDATER_E503_H__

#include <cassert>
#include <cstdlib>
#include <stdexcept>

#include "KVDBParameterSet.h"
#include "KVDBRecord.h"
#include "KVDBRun.h"
#include "KVIDHarpeeICSi_e503.h"
#include "KVIDHarpeeSiCsI_e503.h"
#include "KVINDRADB.h"
#include "KVIVUpDater.h"
#include "KVVAMOS.h"

class KVIVUpDater_e503 : public KVIVUpDater {

   // Copy constructor (Private - forbid copy construction)
   KVIVUpDater_e503(const KVIVUpDater_e503&);

   // Assignment operator (Private - forbid assignment)
   KVIVUpDater_e503& operator=(const KVIVUpDater_e503&);

public:

   // Default Constructor
   KVIVUpDater_e503();

   // Destructor
   virtual ~KVIVUpDater_e503();

   // Set parameters for a particular run
   // Parameter: run - The run number
   virtual void SetParameters(UInt_t run);

   ClassDef(KVIVUpDater_e503, 1)
};

#endif


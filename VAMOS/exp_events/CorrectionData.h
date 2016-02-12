// Author: Peter C. Wigg <peter.wigg.314159@gmail.com>
// Created Wed 20 Jan 14:03:01  2016

////////////////////////////////////////////////////////////////////////////////
//
// CorrectionData.h
//
// Description
//
// Basic class acting as a container for any identification date which need to
// be passed into the idc::ApplyCorrections function. We prefer to pass in this
// data manually as the relevant getters in KVVAMOSReconNuc are liable to
// recalculate values we don't want it to!
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Wed 20 Jan 14:03:01  2016
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CORRECTION_DATA_H__
#define __CORRECTION_DATA_H__

#include "KVIDTelescope.h"
#include "Rtypes.h"

// Namespace for the identification correction code, to prevent polluting the
// global namespace.
namespace idc {

   class CorrectionData {

      // Copy function
      // Parameter: rhs - Target to copy
      void Copy(const CorrectionData& rhs);

   public:

      // Default constructor
      CorrectionData();
      // Destructor
      virtual ~CorrectionData();

      // Copy constructor
      // Parameter: rhs - Target to copy
      CorrectionData(const CorrectionData& rhs);

      // Assignment operator
      // Parameter: rhs - Target to copy
      CorrectionData& operator=(const CorrectionData& rhs);

      // Identifying telescope
      KVIDTelescope* idt;
      // Z value returned from the standard ID routine
      Int_t z_value;
      // A value returned from the standard ID routine
      Int_t a_value;
      // Real Z returned from the standard ID routine
      Double_t z_real;
      // Real A returned from the standard ID routine
      Double_t a_real;
      // Particle ID returned from the standard ID routine
      Double_t pid;
      // Total energy of the identified particle
      Double_t energy;

      ClassDef(CorrectionData, 1);
   };

} // namespace idc

#endif

// Author: Peter C. Wigg
// Created Thu  4 Feb 15:43:34  2016

////////////////////////////////////////////////////////////////////////////////
//
// MinimiserData.h
//
// Description
//
// Base class for handling data produced by the SiliconEnergyMinimiser. The
// minimiser function usually just returns the A value it has determined to be
// the most likely for a given Z (i.e. It returns an integer). Therefore to
// obtain more interesting information about how well the algorithm is
// performing, we need to pass in an (optional) pointer to a data class such as
// this one for the minimiser implementation to populate.
//
// This class handles only the essential data produced by any hypothetical
// minimiser implementation. For data specific to the implementation you must
// create an inheriting class (See MonoMinimiserData and ThreadedMinimiserData).
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Thu  4 Feb 15:43:34  2016
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MINIMISER_DATA_H__
#define __MINIMISER_DATA_H__

#include "Rtypes.h"

class MinimiserData {

public:

   // Default constructor
   MinimiserData();
   // Destructor
   virtual ~MinimiserData();

   // Accessors

   Int_t GetZ() const;
   Int_t GetA() const;
   Double_t GetDelta() const;
   Int_t GetStatusCode() const;

   // Mutators

   void SetZ(Int_t value);
   void SetA(Int_t value);
   void SetDelta(Double_t value);
   void SetStatusCode(Int_t value);

private:

   // Copy constructor (Declared private to prevent copying as it is not worth
   // implementing yet).
   MinimiserData(const MinimiserData& rhs);
   // Assignment operator (Declared private to prevent copying as it is not
   // worth implementing yet).
   MinimiserData& operator=(const MinimiserData& rhs);

   // All minimisers must provide at least the following values:

   Int_t z_value_;
   Int_t a_value_;
   // Delta: Difference between measured and simulated silicon energies
   Double_t delta_;
   Int_t status_code_;

   ClassDef(MinimiserData, 1)

};

#endif

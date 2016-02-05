// Author: Peter C. Wigg
// Created Thu  4 Feb 16:01:37  2016

////////////////////////////////////////////////////////////////////////////////
//
// MonoMinimiserData.h
//
// Description
//
// Class for handling data produced by the SiliconEnergyMinimiser. The minimiser
// function usually just returns the A value it has determined to be the most
// likely for a given Z (i.e. It returns an integer). Therefore to obtain more
// interesting information about how well the algorithm is performing, we need
// to pass in an (optional) pointer to a data class such as this one for the
// minimiser implementation to populate.
//
// This class handles the extra data produced by the MonoMinimiserImpl
// implementation, in addition to the basic minimiser data (See MinimiserData).
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Thu  4 Feb 16:01:37  2016
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __MONO_MINIMISER_DATA_H__
#define __MONO_MINIMISER_DATA_H__

#include "MinimiserData.h"

class MonoMinimiserData : public MinimiserData {

public:

   // Default constructor
   MonoMinimiserData();
   // Destructor
   virtual ~MonoMinimiserData();

   // Accessors

   Int_t GetForwardCounter() const;
   Int_t GetBackwardCounter() const;

   // Mutators

   void SetForwardCounter(Int_t value);
   void SetBackwardCounter(Int_t value);

private:

   // Copy constructor (Declared private to prevent copying as it is not worth
   // implementing yet).
   MonoMinimiserData(const MonoMinimiserData& rhs);
   // Assignment operator (Declared private to prevent copying as it is not
   // worth implementing yet).
   MonoMinimiserData& operator=(const MonoMinimiserData& rhs);

   // Additional data provided by the MonoMinimiserImpl

   Int_t forward_counter_;
   Int_t backward_counter_;

   ClassDef(MonoMinimiserData, 1)

};

#endif

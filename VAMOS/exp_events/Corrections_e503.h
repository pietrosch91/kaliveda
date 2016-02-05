// Author: Peter C. Wigg <peter.wigg.314159@gmail.com>
// Created Wed 20 Jan 13:58:10  2016

///////////////////////////////////////////////////////////////////////////////
//
// Corrections_e503.h
//
// Description
//
// A set of non-member, non-friend functions which together act as a wrapper
// for the standard KaliVeda identification functions and enable the
// identified nucleus to be modified in accordance with the implemented
// identification corrections.
//
// Peter C. Wigg <peter.wigg.314159@gmail.com>
// Wed 20 Jan 13:58:10  2016
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CORRECTIONS_E503_H__
#define __CORRECTIONS_E503_H__

#include "AbsorberEnergies.h"
#include "CorrectionData.h"
#include "KVIDHarpeeICSi_e503.h"
#include "KVIDHarpeeSiCsI_e503.h"
#include "KVVAMOSReconNuc.h"

// Namespace for the identification correction code, to prevent polluting the
// global namespace.
namespace idc {

// Return codes for the idc::Identify function
// See idc::Identify()
   enum IDReturnCodes {
      kAllOK = 0,
      kNucleusNotDetected,   //  1
      kNullNucleusSupplied,  //  2
      kBadTimeCal,           //  3
      kBasicIdNotOk,         //  4
      kBothInherited,        //  5
      kCorrectionsFailed,    //  6
      kICNotInherited,       //  7
      kICSiNullParlist,      //  8
      kInvalidIdCode,        //  9
      kNoIdResult,           //  10
      kNoIdTelescope,        //  11
      kNoTarget,             //  12
      kNotCalibrated,        //  13
      kNotIdentified,        //  14
      kNotInherited,         //  15
      kNotStoppingDetector,  //  16
      kNullParset,           //  17
      kResidualEnergyFail,   //  18
      kSegmentZero,          //  19
      kSiCsINullParlist,     //  20
      kUnCharged,            //  21
      kUnChargedResult,      //  22
      kZIdNotOk              //  23
   };

// 'Hack' identification function for E503.
//
// This function re-implements the old KVVAMOSReconNuc::Identify() function
// which has been removed in the mainline. Here we set the identification result
// and the identifying telescope for the nucleus. The algorithm has been
// implemented exactly as it used to be in the old 1.9 source tree but with a
// couple of improvements to prevent infinite loops. You should really have no
// reason to call this yourself, it is called within idc::Identify().
//
// WARNING: This function is intended to be a temporary 'hack' and not a long
// term solution. It is a stop-gap measure!
//
// Parameter: n - The nucleus to be identified.
   void VAMOSIdentifyHack(KVVAMOSReconNuc* const n);

// Main identification function for E503.
//
// This function acts as a wrapper to the standard identification routines and
// MODIFIES THE IDENTIFIED NUCLEUS by implementing the identification
// correction functions via idc::ApplyCorrections.
//
// Parameter: n - The nucleus to be identified.
//
// Parameter: energy_data - An optional pointer to an idc::AbsorberEnergies object,
// this object is then used to store the energy the nucleus deposits in each of
// the absorbers of the VAMOS setup.
//
// Return: Return code from the enum idc::IDReturnCodes.
   UChar_t Identify(
      KVVAMOSReconNuc* const n,
#if __cplusplus < 201103L
      AbsorberEnergies* const energy_data = NULL
#else
      AbsorberEnergies* const energy_data = nullptr
#endif
   );

// Applies the identification corrections to the nucleus
//
// Once the nucleus has been identified using the standard KaliVeda routines
// one can supply the nucleus to this function and it will apply all of the
// implemented identification corrections. This function MODIFIES THE SUPPLIED
// NUCLEUS!
//
// Parameter: n - The nucleus to be corrected.
// Parameter: data - Pointer to an idc::CorrectionData object containing all of
// the necessary values for the correction procedure (We don't trust routines of
// the type n->GetSomeValue() which can rely on mass values etc. we have not set
// yet).
//
// Return: Boolean indicating success or failure in applying the corrections.
   Bool_t ApplyCorrections(
      KVVAMOSReconNuc* const n,
      const CorrectionData* const data
   );

// Applies the ID corrections for the ionisation chamber -> silicon telescope.
//
// Generally called from idc::ApplyCorrections, here we apply any corrections
// which are required in the ionisation chamber -> silicon identification
// telescope the particle passed through.
//
// Parameter: n - The nucleus to be corrected.
// Parameter: idt - The identification telescope.
// Parameter: data - Pointer to an idc::CorrectionData object containing all of
// the necessary values for the correction procedure (We don't trust routines of
// the type n->GetSomeValue() which can rely on mass values etc. we have not set
// yet).
//
// Return: Boolean indicating success or failure in applying the corrections.
   Bool_t ApplyIcSiCorrections(
      KVVAMOSReconNuc* const n,
      const KVIDHarpeeICSi_e503* const idt,
      const CorrectionData* const data
   );

// Applies the ID corrections for the silicon -> caesium iodide telescope.
//
// Generally called from idc::ApplyCorrections, here we apply any corrections
// which are required in the silicon -> caesium iodide identification
// telescope the particle passed through.
//
// Parameter: n - The nucleus to be corrected.
// Parameter: idt - The identification telescope.
// Parameter: data - Pointer to an idc::CorrectionData object containing all of
// the necessary values for the correction procedure (We don't trust routines of
// the type n->GetSomeValue() which can rely on mass values etc. we have not set
// yet).
//
// Return: Boolean indicating success or failure in applying the corrections.
   Bool_t ApplySiCsiCorrections(
      KVVAMOSReconNuc* const n,
      const KVIDHarpeeSiCsI_e503* const idt,
      const CorrectionData* const data
   );

// Apply M.Boisjoli's correction to the A/Q value.
//
// This algorithm has been taken directly from the old code
// (VAMOS/deprecated/Identificationv).
//
// Parameter: parset - Parameter set associated with this correction.
// Parameter: uncorrected - Uncorrected A/Q value.
// Parameter: pid - The particle identification code.
//
// Return: Corrected A/Q value
   Double_t CorrectAoverQ(
      KVDBParameterSet* const parset,
      Double_t uncorrected,
      Double_t pid
   );

} // namespace idc


#endif

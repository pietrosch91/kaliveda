/***************************************************************************
$Id: VAMOSLinkDef.h,v 1.4 2009/01/14 15:59:49 franklan Exp $
 ***************************************************************************/

#ifdef __CINT__
#include "RVersion.h"
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#ifdef ROOTGANILTAPE
#pragma link C++ class ParameterName+;
#pragma link C++ class Parameters+;
#pragma link C++ class GTGanilDataVAMOS+;
#pragma link C++ class KVIVRawDataAnalyser+;
#pragma link C++ class KVIVRawDataReconstructor+;
#endif
#pragma link C++ class KVIVReconIdent+;
#pragma link C++ class Analysisv+;
#pragma link C++ class Analysisv_e494s+;
#pragma link C++ class Analysisv_e503+;
#pragma link C++ class Reconstructionv+;
#pragma link C++ class ReconstructionSeDv+;
#pragma link C++ class LogFile+;
#pragma link C++ class Random+;
#pragma link C++ class DriftChamber+;
#pragma link C++ class DriftChamberv+;
#pragma link C++ class IonisationChamber+;
#pragma link C++ class IonisationChamberv+;
#pragma link C++ class SeDv+;
#pragma link C++ class SeD12v+;
#pragma link C++ class Siv+;
#pragma link C++ class Identificationv+;
#pragma link C++ class KVCsIVamos+;
#pragma link C++ class KVSiliconVamos+;
#pragma link C++ class KVIDSiCsIVamos+;
#pragma link C++ class KVFocalPlanVamos+;
#pragma link C++ class KVIVSelector+;
#pragma link C++ class EnergyTree+;
#pragma link C++ class CsIv+;
#pragma link C++ class PlaneAbsorber+;

// Experiment with KVVAMOS.h requires these
// ** Will not compile yet **

//#pragma link C++ class KVVAMOSCodeMask+;
//#pragma link C++ class KVVAMOSCodes+;
//#pragma link C++ class KVVAMOSReconEvent+;
//#pragma link C++ class KVVAMOSReconNuc+;
//#pragma link C++ class KVIDChIoSiVamos+;

#endif

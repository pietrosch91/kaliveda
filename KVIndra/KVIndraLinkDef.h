/***************************************************************************
$Id: KVIndraLinkDef.h,v 1.95 2009/04/15 09:46:27 ebonnet Exp $
                              kaliveda5LinkDef.h
                               description
                             -------------------
                       Auteur   :  Alexis Mignon + J.D. Frankland
                       email    :  mignon@ganil.fr
                       Cree le  :  21/05/2002
 ***************************************************************************/

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ global gIndra;
#pragma link C++ global gIndraDB;
#pragma link C++ enum EVedaCodes;
#pragma link C++ enum EIDCodes;
#pragma link C++ enum EBaseIndra_type;
#pragma link C++ enum EBaseIndra_typePhos;
#pragma link C++ class KVDP2toCsIGridConvertor+;
#pragma link C++ class KVINDRA+;
#pragma link C++ class KVINDRARRMValidator+;
#pragma link C++ class KVINDRADB;
#pragma link C++ class KVINDRA2+;
#pragma link C++ class KVINDRA4+;
#pragma link C++ class KVINDRAe475s+;
#pragma link C++ class KVINDRAe503+;
#pragma link C++ class KVINDRACodes+;
#pragma link C++ class KVINDRACodeMask+;
#ifdef ROOTGANILTAPE
#pragma link C++ class KVINDRARawDataReader;
#pragma link C++ class KVINDRARawDataAnalyser;
#pragma link C++ class KVINDRARawDataReconstructor;
#pragma link C++ class KVINDRARawIdent;
#endif
#pragma link C++ class KVRunListLine;
#pragma link C++ class KVINDRARunListReader;
#pragma link C++ class KVINDRARunSheetReader;
#pragma link C++ class KVINDRARunSheetGUI;
#pragma link C++ class KVIRSGChooseDataSetDialog;
#pragma link C++ class KVDataAnalysisLauncher;
#pragma link C++ class KVGFileList;
#pragma link C++ class KVGDirectoryList;
#pragma link C++ class KVTimeEntry;
#pragma link C++ class KVTimeEntryLayout;
#pragma link C++ class KVSilicon-;
#pragma link C++ class KVSi75+;
#pragma link C++ class KVSiLi+;
#pragma link C++ class KVSilicon_e475s-;
#pragma link C++ class KVChIo-;
#pragma link C++ class KVChIo_e475s-;
#pragma link C++ class KVCsI-;
#pragma link C++ class KVCsI_e475s-;
#pragma link C++ class KVPhoswich+;
#pragma link C++ class KVChannelVolt+;
#pragma link C++ class KVChannelEnergyBrho+;
#pragma link C++ class KVChannelEnergyAlpha+;
#pragma link C++ class KVChannelEnergyChIo+;
#pragma link C++ class KVLightEnergyCsI+;
#pragma link C++ class KVFunctionCal+;
#pragma link C++ class KVINDRAReconNuc-;//custardpie speaker
#pragma link C++ class KVINDRAReconEvent-;//customized streamer
#pragma link C++ class KVVoltEnergy+;
#pragma link C++ class KVDBParameterSet-;//customized streamer
#pragma link C++ class KVINDRADBRun;
#pragma link C++ class KVINDRADataAnalyser;
#pragma link C++ class KVINDRAReconDataAnalyser;
#pragma link C++ class KVINDRAGeneDataSelector;
#pragma link C++ class KVINDRAGeneDataAnalyser;
#pragma link C++ class KVDBChIoPressures;
#pragma link C++ class KVDBTape;
#pragma link C++ class KVDBPeak;
#pragma link C++ class KVDBAlphaPeak;
#pragma link C++ class KVDBElasticPeak;
#pragma link C++ class KVElasticScatterEvent_e475s+;
#pragma link C++ class KVINDRADB1;
#pragma link C++ class KVINDRADB2;
#pragma link C++ class KVINDRADB4;
#pragma link C++ class KVINDRADB_e475s;
#pragma link C++ class KVINDRADB_e503;
#pragma link C++ class KVINDRAPulserDataTree+;
#pragma link C++ class KVINDRATriggerInfo+;
#pragma link C++ class KVINDRAUpDater;
#pragma link C++ class KVINDRAUpDater_e475s+;
#pragma link C++ class KVIDSiCsI+;
#pragma link C++ class KVIDSiCsI_e475s+;
#pragma link C++ class KVIDSiLiCsI+;
#pragma link C++ class KVIDSi75SiLi+;
#pragma link C++ class KVIDChIoSi+;
#pragma link C++ class KVIDChIoSi75+;
#pragma link C++ class KVIDChIoSi_e475s+;
#pragma link C++ class KVIDChIoCsI_e475s+;
#pragma link C++ class KVIDCsI_e475s+;
#pragma link C++ class KVIDCsI+;

#pragma link C++ class KVIDPhoswich+;
#pragma link C++ class KVIDChIoCsI+;
#pragma link C++ class KVIDCsIRLLine+;
#pragma link C++ class KVIDGCsI+;
#pragma link C++ class KVIDGChIoSi-;
#pragma link C++ class KVNewGridRRMDialog;
#pragma link C++ class KVSelector+;
#pragma link C++ class KVFortranAnalysis+;
#pragma link C++ class KVDataSelector;
#pragma link C++ class KVReconIdent+;
#pragma link C++ class KVReconIdent_e475s+;
#pragma link C++ class KVIdentRoot+;
#endif

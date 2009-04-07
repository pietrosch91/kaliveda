/***************************************************************************
$Id: KVMultiDetLinkDef.h,v 1.135 2009/04/07 12:04:11 franklan Exp $
                              KVMultiDetLinkDef.h
                               description
                             -------------------
                       Auteur   :  Alexis Mignon + J.D. Frankland
                       email    :  mignon@ganil.fr
 ***************************************************************************/

#ifdef __CINT__
#include "RVersion.h"
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ global gDataBase;
#pragma link C++ global gMultiDetArray;
#pragma link C++ global gBatchSystem;
#pragma link C++ global gBatchSystemManager;
#pragma link C++ global gIDGridManager;
#pragma link C++ global gDataSetManager;
#pragma link C++ global gDataRepository;
#pragma link C++ global gDataRepositoryManager;
#pragma link C++ global gDataSet;
#pragma link C++ function SearchFile(const Char_t*, TString&, int);
#pragma link C++ namespace KVTGIDFunctions;
#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;
#pragma extra_include "Rtypes.h";
#pragma link C++ class Binary_t<UChar_t>+;
#pragma link C++ class Binary_t<UShort_t>+;
#pragma link C++ class Binary_t<UInt_t>+;
#pragma link C++ class Binary_t<Long64_t>+;
#ifdef __WITHOUT_TPARAMETER
#pragma link C++ class KVParameter<Int_t>+;
#pragma link C++ class KVParameter<Double_t>+;
#elif ! defined ( __WITH_TPARAMETER_INT )
#pragma link C++ class KVParameter<Int_t>+;
#endif
#ifdef __WITHOUT_TPARAMETER
#pragma link C++ class KVParameter<KVString>+;
#pragma link C++ class KVParameter<TString>+;
#else
#pragma link C++ class TParameter<KVString>+;
#pragma link C++ class TParameter<TString>+;
#endif
#pragma link C++ class KVParameterList<Int_t>+;
#pragma link C++ class KVParameterList<Double_t>+;
#pragma link C++ class KVParameterList<TString>+;
#pragma link C++ class KVParameterList<KVString>+;
#pragma link C++ class Hexa_t+;
#pragma link C++ class KVACQParam+;
#pragma link C++ class KVAvailableRunsFile+;
#pragma link C++ class KVRemoteAvailableRunsFile+;
#pragma link C++ class KVBase-;//customised streamer
#pragma link C++ class KVBatchSystem;
#pragma link C++ class KVBatchSystemManager;
#pragma link C++ class KV_CCIN2P3_BQS;
#pragma link C++ class KVRootBatch;
#pragma link C++ class KVLinuxBatch;
#pragma link C++ class KVBrowser;
#pragma link C++ class KVBrowserWidget;
#pragma link C++ class KVBrowserNameDialog;
#pragma link C++ class KVCalibrator-;//customised streamer
#pragma link C++ class KVClassFactory+;
#pragma link C++ class KVClassMethod+;
#pragma link C++ class KVDetector-;
#pragma link C++ class KVDetectorBrowser;
#pragma link C++ class KVDataBase;
#pragma link C++ class KVDataBaseBrowser;
#pragma link C++ class KVDataSet+;
#pragma link C++ class KVDataAnalysisTask+;
#pragma link C++ class KVDataAnalyser+;
#pragma link C++ class KVDataRepository+;
#pragma link C++ class KVDataTransfer+;
#pragma link C++ class KVDataTransferSFTP+;
#pragma link C++ class KVDataTransferBBFTP+;
#pragma link C++ class KVDP2toIDGridConvertor+;
#pragma link C++ class KVRemoteDataRepository+;
#pragma link C++ class KVRemoteDataSet+;
#pragma link C++ class KVRemoteDataSetManager+;
#pragma link C++ class KVDataRepositoryManager+;
#pragma link C++ class KVDataSetManager+;
#pragma link C++ class KVDatime+;
#pragma link C++ class KVDBKey;
#pragma link C++ class KVDBRecord;
#pragma link C++ class KVDBRun;
#pragma link C++ class KVDBSystem;
#pragma link C++ class KVDBSystemDialog+;
#pragma link C++ class KVDBTable;
#pragma link C++ class KVDatedFileManager+;
#pragma link C++ class KVDetectorEvent+;
#pragma link C++ class KVElasticScatter+;
#pragma link C++ class KVElasticScatterEvent+;
#pragma link C++ class KVEvent+;
#pragma link C++ class KVGroup+;
#pragma link C++ class KVGenParList+;
#pragma link C++ class KVIDentifier+;
#pragma link C++ class KVIDGraph+;
#pragma link C++ class KVIDLine-;
#pragma link C++ class KVIDContour+;
#pragma link C++ class KVIDCutContour+;
#pragma link C++ class KVIDCutLine+;
#pragma link C++ class KVIDZAContour+;
#pragma link C++ class KVIDZALine-;
#pragma link C++ class KVIDZLine-;
#pragma link C++ class KVIDMap+;
#pragma link C++ class KVIDGrid-;
#pragma link C++ class KVIDGridManager;
#pragma link C++ class KVIDZAGrid-;
#pragma link C++ class KVIDZGrid-;
#pragma link C++ class KVIDTelescope-;
#pragma link C++ class KVIDSubCodeManager+;
#pragma link C++ class KVVirtualIDFitter+;
#pragma link C++ class KVLayer-;
#pragma link C++ class KVLayerBrowser;
#pragma link C++ class KVLinCal+;
#pragma link C++ class KVList+;
#pragma link C++ class KVListView;
#pragma link C++ class KVLockfile+;
#pragma link C++ class KVLogReader+;
#pragma link C++ class KVLVContainer;
#pragma link C++ class KVLVEntry;
#pragma link C++ class KVLVColumnData;
#pragma link C++ class KVMaterial+;
#pragma link C++ class KVModule+;
#pragma link C++ class KVMultiDetArray-;
#pragma link C++ class KVMultiDetBrowser;
#pragma link C++ class KVNucleus+;
#pragma link C++ class KVNumberList+;
#pragma link C++ class KVParticle+;
#pragma link C++ class KVParticleCondition+;
#pragma link C++ class KVPosition+;
#pragma link C++ class KVPulseHeightDefect+;
#pragma link C++ class KVRawDataReader+;
#pragma link C++ class KVReconstructedNucleus-;//customised streamer
#pragma link C++ class KVReconstructedEvent-;//customised streamer
//#pragma link C++ class KVRegister+;
//#pragma link C++ class KVRegAnalogue+;
//#pragma link C++ class KVRegArray+;
//#pragma link C++ class KVRegBinary+;
//#pragma link C++ class KVRegHexa+;
//#pragma link C++ class KVRegShared+;
//#pragma link C++ class KVRBValAttribut+;
#pragma link C++ class KVRing-;
#pragma link C++ class KVRingBrowser;
#pragma link C++ class KVRList+;
#pragma link C++ class KVSortableDatedFile+;
#pragma link C++ class KVTGID-;
#pragma link C++ class KVTGIDZ+;
#pragma link C++ class KVTGIDZA+;
#pragma link C++ class KVTGIDManager+;
#pragma link C++ class KVTGIDFitter+;
#pragma link C++ class KVTGIDGrid+;
#pragma link C++ class KVTarget+;
#pragma link C++ class KVTelescope-;
#pragma link C++ class KVTestEvent+;
#pragma link C++ class KVWidget;
#pragma link C++ class KVWidgetList;
#pragma link C++ class KV2Body+;
#pragma link C++ class KVString+;
#pragma link C++ class KVMergeGridsDialog+;
#pragma link C++ class KVNewGridDialog+;
#pragma link C++ class KVTestIDGridDialog+;
#pragma link C++ class KVCalculateChIoSiGridDialog+;
#pragma link C++ class KVIDGUITelescopeChooserDialog+;
#pragma link C++ class KVIDGridManagerGUI+;
#pragma link C++ class KVDropDownDialog+;
#pragma link C++ class KVInputDialog+;
#pragma link C++ class KVTextEntry+;
#pragma link C++ class KVUpDater;
#pragma link C++ class KVHistoManipulator;
#pragma link C++ class KVEventListMaker;
#ifdef __CCIN2P3_RFIO
#pragma link C++ class KVRFIOSystem+;
#endif
#pragma link C++ class KVVarGlob;
#pragma link C++ class KVVarGlob1;
#pragma link C++ class KVVarGlobMean;
#pragma link C++ class KVVGSum;
#pragma link C++ class KVZtot;
#pragma link C++ class KVEtrans;
#pragma link C++ class KVEtransLCP;
#pragma link C++ class KVEkin;
#pragma link C++ class KVCalorimetry;
#pragma link C++ class KVZmean;
#pragma link C++ class KVTenseur3;
#pragma link C++ class KVTensP;
#pragma link C++ class KVTensE;
#pragma link C++ class KVTensPCM;
#pragma link C++ class KVRiso;
#pragma link C++ class KVFoxH2;
#pragma link C++ class KVZbound;
#pragma link C++ class KVZBoundMean;
#pragma link C++ class KVMultIMF;
#pragma link C++ class KVMultLeg;
#pragma link C++ class KVZtotAv;
#pragma link C++ class KVMultAv;
#pragma link C++ class KVZboundAv;
#pragma link C++ class KVMultIMFAv;
#pragma link C++ class KVMultLegAv;
#pragma link C++ class KVRisoAv;
#pragma link C++ class KVZmax;
#pragma link C++ class KVPtot;
#pragma link C++ class KVZVtot;
#pragma link C++ class KVGVList;
#pragma link C++ class KVTrieur;
#pragma link C++ class KVTrieurLin;
#pragma link C++ class KVTrieurTranche;
#pragma link C++ class KVTrieurBloc;
#pragma link C++ class KVRelativeVelocity;
#pragma link C++ class KVUpdateChecker;
#pragma link C++ class KVCVSUpdateChecker;
#endif

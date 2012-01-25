/*
$Id: ShapeVariables.cpp,v 1.1 2007/11/30 13:45:14 franklan Exp $
$Revision: 1.1 $
$Date: 2007/11/30 13:45:14 $
*/

//Created by KVClassFactory on Wed Nov 28 13:22:45 2007
//Author: franklan

#include "ShapeVariables.h"
#include "KVINDRAReconNuc.h"
#include "KVBatchSystem.h"
#include "KVINDRA.h"

ClassImp(ShapeVariables)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>ShapeVariables</h2>
<h4>Automatic generation of global variable branches</h4>
<p>This analysis class is an example of using
<a href="KVGVList.html#KVGVList:MakeBranches">KVGVList::MakeBranches</a>
and <a href="KVGVList.html#KVGVList:FillBranches">KVGVList::FillBranches</a>
in order to automatically generate and fill
the necessary branches in a TTree to store event-by-event the values
of all the global variables calculated in the analysis.
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void ShapeVariables::InitAnalysis(void)
 {
//   if( gBatchSystem )  // running in batch
//      file = new TFile( Form("%s.root", gBatchSystem->GetJobName()), "recreate");
//   else  // interactive
//      file = new TFile( "ShapeVariables.root", "recreate");
//  
//    AddGV("KVZtot", "ztot");
//    AddGV("KVZVtot", "zvtot");
//    AddGV("KVTensPCM", "CMtensor")->SetParameter("Zmin", 5);
//    AddGV("KVFoxH2", "fox2")->SetParameter("Zmin", 5);
//       
//    tree = new TTree("EventShape","Event shape global variables");
//    GetGVList()->MakeBranches( tree );
  if( gBatchSystem )  // running in batch
     file = new TFile( Form("%s.root", gBatchSystem->GetJobName()), "recreate");
  else  // interactive
     file = new TFile( "ShapeVariables.root", "recreate");
 
   AddGV("KVZtot", "ztot");
   AddGV("KVZVtot", "zvtot");
   AddGV("KVTensPCM", "CMtensor")->SetParameter("Zmin", 5);
   AddGV("KVFoxH2", "fox2")->SetParameter("Zmin", 5);
      
   tree = new TTree("EventShape","Event shape global variables");
   GetGVList()->MakeBranches( tree );
 }

//_____________________________________
void ShapeVariables::InitRun(void)
 {
//       GetEvent()->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);//VEDA id-codes code(i)=2,3,4 and 6 accepted
//       GetEvent()->AcceptECodes( kECode1|kECode2 );//VEDA ecode(i)=1&2 accepted
      GetEvent()->AcceptIDCodes(kIDCode2 | kIDCode3 | kIDCode4 | kIDCode6);//VEDA id-codes code(i)=2,3,4 and 6 accepted
      GetEvent()->AcceptECodes( kECode1|kECode2 );//VEDA ecode(i)=1&2 accepted
 }

//_____________________________________
Bool_t ShapeVariables::Analysis(void)
 {
//    if( GetEvent()->IsOK() ){
//       GetGVList()->FillBranches();
//       tree->Fill();
//    }
//    
//    return kTRUE;
   if( GetEvent()->IsOK() ){
      GetGVList()->FillBranches();
      tree->Fill();
   }
   
   return kTRUE;
 }

//_____________________________________
void ShapeVariables::EndRun(void)
 {
 }

//_____________________________________
void ShapeVariables::EndAnalysis(void)
 {
//     tree->Write();
//     delete file;
    tree->Write();
    delete file;
 }

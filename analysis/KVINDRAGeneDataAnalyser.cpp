/*
$Id: KVINDRAGeneDataAnalyser.cpp,v 1.4 2007/11/15 14:59:45 franklan Exp $
$Revision: 1.4 $
$Date: 2007/11/15 14:59:45 $
*/

//Created by KVClassFactory on Mon Jun  4 19:24:11 2007
//Author: e_ivamos

#include "KVINDRAGeneDataAnalyser.h"
#include "TChain.h"
#include "KVDataSet.h"
#include "KVINDRAGeneDataSelector.h"

ClassImp(KVINDRAGeneDataAnalyser)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVINDRAGeneDataAnalyser</h2>
<h4>For analysing INDRA gene data</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVINDRAGeneDataAnalyser::KVINDRAGeneDataAnalyser()
{
   //Default constructor
}

KVINDRAGeneDataAnalyser::~KVINDRAGeneDataAnalyser()
{
   //Destructor
}

//_________________________________________________________________

void KVINDRAGeneDataAnalyser::SubmitTask()
{
   //Run the interactive analysis
   
   //make the chosen dataset the active dataset ( = gDataSet; note this also opens database
   //and positions gDataBase & gIndraDB).
   fDataSet->cd();

   TChain *t = new TChain("GeneData");
   t->SetDirectory(0);//we handle delete
   
   fRunList.Begin(); Int_t run;
   while( !fRunList.End() ){
      
      run = fRunList.Next();
      cout << "Adding file " << gDataSet->GetFullPathToRunfile(fDataType.
                                                               Data(),
                                                               run);
      cout << " to the TChain." << endl;
      t->Add(gDataSet->GetFullPathToRunfile(fDataType.Data(),run), -1);
   }
      
   KVINDRAGeneDataSelector *selector = (KVINDRAGeneDataSelector*)GetInstanceOfUserClass();
   
   if(!selector || !selector->InheritsFrom("KVINDRAGeneDataSelector"))
    {
    cout << "The selector \"" << GetUserClass() << "\" is not valid." << endl;
    cout << "Process aborted." << endl;
    }
   else
    {
    if (nbEventToRead) {
     t->Process(GetUserClass(), "",nbEventToRead);
     } else {
     t->Process(GetUserClass());
     }
    }
   if(selector) delete selector;
   delete t;
}


//______________________________________________________________________________
void ReadScalers(void)
{
  // Read Scalers, fill histograms with counts and display the result.
  // Very very basic script.

  GTScalers      *scalers=new GTScalers;

  // Open file, get the Scaler Tree, and connecting scalers 
  // variable to the data in file
  TFile *ScalerFile = new TFile("scaler.root","read");
  TTree *ScalerTree = (TTree*)gDirectory->Get("AutoScalers");
  ScalerTree->SetBranchAddress("scalers",&scalers);
  
  Int_t nentries = Int_t(ScalerTree->GetEntries()); // Number of entry in Tree

  TH1 **histoScaler; // Scaler histogram array
  int nbChannel;     // Number of scaler channels: assumed to be constant once
                     // initialised (current limitation).

  for (Int_t jentry=0; jentry<nentries;jentry++) {
    ScalerTree->GetEntry(jentry);

    if (jentry==0) {
      nbChannel=fScalers->GetNbChannel(); // Set number of channels
      // Create needed number of histograms (one per channel).
      histoScaler = new TH1*[nbChannel];
      for (int i=0;i<nbChannel;i++) {
	TString name ="H";
	name+=i;
	TString title="Count, Scaler ";
	title+=i+1;
	histoScaler[i]=new TH1D(name,title,nentries+1,-.5,nentries+.5);
      }
    }    
    
    for (int i=0;i<nbChannel;i++) {
      // Fill histo with data
      const GTOneScaler *oneScaler=fScalers->GetScalerPtr(i);
       histoScaler[i]->Fill(jentry,oneScaler->GetCount());
    }
  }

  // Display the result by group of 5 channels per Canvas

  TCanvas *cnv;
  for (int i=0;i<nbChannel;i++) {
    if (!(i%5)) { // A new canva each 5 histo
      char name1 [100];
      char title1[100];
      sprintf(name1 ,"Scalers%d",i);
      sprintf(title1,"Scalers %d to %d (counts)",i+1,i+6);
      cnv=new TCanvas(name1,title1,1);
      cnv->Divide(1,5);
    }
    // Draw histos
    int k=(i%5)+1;
    cnv->cd(k);
    histoScaler[i]->Draw();
  }
}


//Created by KVClassFactory on Thu Mar 25 11:25:27 2010
//Author: bonnet

#include "KVBreakUp.h"
#include "TMath.h"
#include "Riostream.h"
#include "TMethodCall.h"
#include "TRandom3.h"

ClassImp(KVBreakUp)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVBreakUp</h2>
<h4>Partitioning of an integer in a given number of interger
under different conditions and/or wau of break up
</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////


void KVBreakUp::init(void) {
	
	size_max=500;
	//Condition initiale de la cassure Ztot,Mtot,Zlim
	Ztotal=0;
	bound = 0;
	SetConditions(0,0,0);
	
	//generateurs de nombre aleatoire
	alea = 0;
	RedefineTRandom("TRandom3");
	
	BreakUpMethod="BreakUsingChain";
	SetBreakUpMethod(BreakUpMethod);
	
	nraffine=0;
	
	lobjects=0;	
	
	lhisto=0;	
	DefineHistos();
	
	parman=0;
	tt=0;
	iterations_total = 0;
	tstart = tstop = tellapsed = 0;
}

void KVBreakUp::DefineHistos(){
	
	lhisto = new KVList();
	//histogramme de control
	hzz = new TH1F("KVBreakUp_hzz","distri z",200,-0.5,199.5);   				lhisto->Add(hzz);
	hzt = new TH1F("KVBreakUp_hzt","h ztotal",200,-0.5,199.5);   				lhisto->Add(hzt);
	hmt = new TH1F("KVBreakUp_hmt","h mtotal",200,-0.5,199.5);   				lhisto->Add(hmt);
}
	
void KVBreakUp::StoreEntriesInTree(){
	
	if (!tt){
		tt = new TTree("KVBreakUp_tree","tree");
		if (!lobjects) lobjects = new KVList(); 
		lobjects->Add(tt);
	
		tt->Branch("zt",&Ztotal,"zt/I");
		tt->Branch("mt",&Mtotal,"mt/I");
	
		tt->Branch("tabz",size,"tabz[mt]/I");
		SetBit(kFillTree,kTRUE);
	}

}

void KVBreakUp::StorePartition(Bool_t choix){
	SetBit(kStorePartitions,choix);
	if ( !GetTree() ) StoreEntriesInTree();
	if ( !GetManager() ){
		parman = new KVPartitionManager();
		if (!lobjects) lobjects = new KVList(); 
		lobjects->Add(parman);
	}
}


Int_t KVBreakUp::BreakUsingChain(void){

	//Conditions de depart
	//Mtotal clusters de taille minimale Zmin
	for (Int_t mm=0;mm<Mtotal;mm+=1) { size[mm]=Zmin-1; }
	//On initilise la taille a Zmin-1
	//Le tirage aleatoire se fait 
	//sur la taille restante Ztotal_corr
	Int_t Ztotal_corr = Ztotal-(Zmin-1)*Mtotal;
	
	nl="";
	Int_t bb=0;
	Int_t mtire=0;
	while (mtire<Mtotal){
		//Tirage uniform d'entier entre 0 et Ztotal_corr - 1 
		bb = TMath::Nint(alea->Uniform(0,(Ztotal_corr)-0.5));
		//test si ce lien a deja ete casse
		if (bound[bb]==1){
			nl.Add(bb);
			mtire+=1;
			bound[bb]=0;
		}
	}
	
	//La cassure a reussie
	//on recupere les tailles additionelles
	Int_t mtot_corr=0;
	Int_t* val = nl.GetArray(mtot_corr);
	
	Int_t zc=0;
	//boucle sur les liens casses
	Int_t taille=0;
	for (Int_t ii=1;ii<mtot_corr;ii+=1){
		taille = val[ii]-val[ii-1];
		size[ii] += taille;	//mise a jour des tailles des clusters
		hzz->Fill(size[ii]);
		bound[val[ii]]=1;	//Reset du lien pour le prochain tirage
		zc += size[ii];	//incrementation de la taille totale de controle
	}
	taille = Ztotal_corr-val[mtot_corr-1]+val[0]; 
	
	size[0] += taille;
	hzz->Fill(size[0]);
	bound[val[0]]=1;
	zc += size[0];
	
	delete [] val;
	//cout << zc << " " << Ztotal << endl;
	if (zc==Ztotal && mtot_corr==Mtotal) return 1;
	else return 0;
	
}

Int_t KVBreakUp::BreakUsingLine(void){

	//Conditions de depart
	//Mtotal clusters de taille minimale Zmin
	for (Int_t mm=0;mm<Mtotal;mm+=1) { size[mm]=Zmin-1; }
	//On initilise la taille a Zmin-1
	//Le tirage aleatoire se fait 
	//sur la taille restante Ztotal_corr
	Int_t Ztotal_corr = Ztotal-(Zmin-1)*Mtotal;
	
	nl="";
	Int_t bb=0;
	Int_t mtire=0;
	while (mtire<Mtotal-1){
		//Tirage uniform d'entier entre 0 et Ztotal_corr - 1 
		bb = TMath::Nint(alea->Uniform(1,(Ztotal_corr-1)-0.5));
		//test si ce lien a deja ete casse
		if (bound[bb]==1){
			//cout << "bb="<<bb << endl;
			nl.Add(bb);
			mtire+=1;
			bound[bb]=0;
		}
	}

	//cout << "mtire="<<mtire << endl;
	//
	nl.Add(0);
	nl.Add(Ztotal_corr);
	//cout << "nl="<<nl.AsString() << endl;;
	//La cassure a reussie
	//on recupere les tailles additionelles
	Int_t mtot_corr=0;
	Int_t* val = nl.GetArray(mtot_corr);
	//cout << "mtot_corr="<<mtot_corr << endl;
	Int_t zc=0;
	//boucle sur les liens casses
	Int_t taille=0;
	for (Int_t ii=1;ii<mtot_corr;ii+=1){
		taille = val[ii]-val[ii-1];
		//cout << "ii="<< ii <<" taille="<<taille<<endl;
		size[ii-1] += taille;	//mise a jour des tailles des clusters
		hzz->Fill(size[ii-1]);
		bound[val[ii]]=1;	//Reset du lien pour le prochain tirage
		zc += size[ii-1];	//incrementation de la taille totale de controle
	}
	
	delete [] val;
	//cout << "zc="<<zc << " " << Ztotal << endl;
	if (zc==Ztotal && mtot_corr-1==Mtotal) return 1;
	else return 0;
	
}

Int_t KVBreakUp::BreakUsingIndividual(void){

	//Conditions de depart
	// Mtotal clusters de taille minimale Zmin
	for (Int_t mm=0;mm<Mtotal;mm+=1) { size[mm]=Zmin; }
	//On initilise la taille a Zmin
	//Le tirage aleatoire se fait 
	//sur la taille restante Ztotal_corr
	Int_t Ztotal_corr = Ztotal-(Zmin)*Mtotal;
	
	Double_t val[Mtotal],sum_val=0;
	for (Int_t mm=0;mm<Mtotal;mm+=1){
		//Tirage des Mtotal valeurs entre [0; 1[
		val[mm] = alea->Uniform(0,1);
		sum_val+=val[mm];
	}
	Int_t zc=0;
	Int_t surplus[Mtotal];
	//normalisation a une charge totale de Ztotal_corr
	for (Int_t mm=0;mm<Mtotal;mm+=1){
		surplus[mm] = TMath::Nint(val[mm]*Ztotal_corr/sum_val);
		zc += surplus[mm];
	}	
	//Test effets spurieux pasage reel -> entier
	//Redistribution eventuelle de charge manquante ou en exces
	if (zc!=Ztotal_corr){
		Int_t diff = Ztotal_corr-zc;
		nraffine+=1;
		Int_t bb=0;
		if (diff>0){
			for (Int_t mm=0;mm<TMath::Abs(diff);mm+=1){
				bb = TMath::Nint(alea->Uniform(0,(Mtotal)-0.5));
				surplus[bb] += 1;
			}
		}
		else {
			for (Int_t mm=0;mm<TMath::Abs(diff);mm+=1){
				bb = TMath::Nint(alea->Uniform(0,(Mtotal)-0.5));
				if (surplus[bb]>0) 
					surplus[bb] -= 1;
				else 
					mm-=1;
			}
		}
	}
	zc=0;
	for (Int_t mm=0;mm<Mtotal;mm+=1){
		size[mm]+=surplus[mm];
		zc += size[mm];
		hzz->Fill(size[mm]);
	}

	if (zc == Ztotal) return 1;
	else return 0;
	
}

void KVBreakUp::Reset_ForNextTime(){
	//routine ne servant pas actuellement
	nl.Begin();
	while (!nl.End()){
		Int_t tag = nl.Next();
		for (Int_t mm=tag;mm<tag+Zmin;mm+=1){
		Int_t pos = mm%(Ztotal);
			bound[pos]=1;
		}
	}
	nl="";
}

Int_t KVBreakUp::BreakUsingPile(void){

	
	Int_t bb=0;
	
	for (Int_t mm=0;mm<Mtotal;mm+=1) { size[mm]=Zmin; }
	//on distribue aleatoirement les charges
	//disponibles une par une sur les differents fragments 
	for (Int_t mm=0;mm<nbre_nuc;mm+=1){
		bb = TMath::Nint(alea->Uniform(0,(Mtotal)-0.5));
		size[bb]+=1;
	}
		
	Int_t mc=0,zc=0;
	for (Int_t mm=0;mm<Mtotal;mm+=1){
		Int_t taille = size[mm];
		hzz->Fill(taille);
		mc+=1;
		zc += taille;
	}	
	if (mc==Mtotal && zc==Ztotal) return 1;
	else return 0;
	

}

void KVBreakUp::TreatePartition(){

	//OrdonneCharge();
	if (TestBit(kFillTree)) tt->Fill();
	hmt->Fill(Mtotal);
	hzt->Fill(Ztotal);

}
	

/*
void KVBreakUp::OrdonneCharge(){

	//On ordonne le tableau de charge par ordre decroissant
	Int_t index[Mtotal],tampon[Mtotal];
	TMath::Sort(Mtotal,size,index);
	for (Int_t nn=0;nn<Mtotal;nn+=1) tampon[nn] = size[index[nn]];
	for (Int_t nn=0;nn<Mtotal;nn+=1) size[nn] = tampon[nn];

}	
*/	
	
void KVBreakUp::BreakNtimes(Int_t times){

	
	Start();
	
	TMethodCall meth;
   meth.InitWithPrototype(this->IsA(), BreakUpMethod.Data(),"");
	Long_t ret;
	if (meth.IsValid() && meth.ReturnType()==TMethodCall::kLong) {
		for (Int_t nn=0;nn<times;nn+=1){
			if ( nn%10000 == 0 ) Info("BreakNtimes","%d partitions generees sur %d",nn,times);
			meth.Execute(this,"",ret);
			if (ret==1)
				TreatePartition();
			else {
				Info("BreakNtimes","%s retourne %d",BreakUpMethod.Data(),ret);
				nn-=1;
			}
		}
	}
	Info("BreakNtimes","Tirage termine");
	iterations_total+=times;	
	
	Stop();
	Info("BreakNtimes","Temps ecoule en secondes : %d",GetDeltaTime());

	if (TestBit(kStorePartitions)){
		TransfertFromTree();
	}

}	
	
void KVBreakUp::TransfertFromTree(void){	
	
	Start();
	
	Info("TransfertFromTree","Transfert des partitions");
	KVString snom;
	if (TestBit(kStorePartitions)){
		for (Int_t nn=0;nn<tt->GetEntries();nn+=1){
			//if ( nn%10000 == 0 ) Info("TransfertFromTree","%d entrees traites",nn);
			tt->GetEntry(nn);
			
			partition = new KVPartition(GetZtot());
			partition->Fill(size,Mtotal);
			
			if ( !(parman->TestPartition(partition)) )
				delete partition;
			
		}
		
		parman->ReduceSubLists();
		
		ResetTree();
	}
	
	Stop();
	Info("TransfertFromTree","Temps ecoule en secondes : %d",GetDeltaTime());

}	


void KVBreakUp::BreakNtimesOnGaussian(Int_t times,Double_t Ztot_moy,Double_t Ztot_rms,Double_t Mtot_moy,Double_t Mtot_rms,Int_t zmin){

	Start();
	
	TMethodCall meth;
   meth.InitWithPrototype(this->IsA(), BreakUpMethod.Data(),"");
	Long_t ret;
	TRandom3* gaus = new TRandom3();
	if (meth.IsValid() && meth.ReturnType()==TMethodCall::kLong) {
		for (Int_t nn=0;nn<times;nn+=1){
			if ( nn%1000 == 0 ) printf("%d partitions generees sur %d\n",nn,times);
		
			Int_t zt = TMath::Nint(gaus->Gaus(Ztot_moy,Ztot_rms));
			Int_t mt = TMath::Nint(gaus->Gaus(Mtot_moy,Mtot_rms));
			
			if (mt>0 && zt>0 && zt>=mt*zmin){
				SetConditions(zt,mt,zmin);
				meth.Execute(this,"",ret);
				if (ret==1)
					TreatePartition();
				else {
					//cout << BreakUpMethod << " retourne " << ret << endl; 
				}	
			}
			else {
				nn-=1;
			}
		}
	}
	delete gaus;
	iterations_total+=times;
	
	Stop();
	Info("BreakNtimesOnGaussian","Temps ecoule en secondes : %d",GetDeltaTime());

	if (TestBit(kStorePartitions)){
		TransfertFromTree();
	}
	
}		

void KVBreakUp::BreakFromHisto(TH2F* hh_zt_VS_mt,Int_t zmin){

	TH2F* h2 = hh_zt_VS_mt;
	if (!h2) return;
	
	Start();
	
	TMethodCall meth;
   meth.InitWithPrototype(this->IsA(), BreakUpMethod.Data(),"");
	Long_t ret;
	
	Int_t zt,mt;
	Int_t stat_tot=Int_t(h2->Integral());
	Int_t stat_par=0;
	for (Int_t nx=1; nx<=h2->GetNbinsX(); nx+=1)
		for (Int_t ny=1; ny<=h2->GetNbinsY(); ny+=1){
			Int_t stat = TMath::Nint(h2->GetBinContent(nx,ny));
			if (stat>0){
				mt = TMath::Nint(h2->GetXaxis()->GetBinCenter(nx));
				zt = TMath::Nint(h2->GetYaxis()->GetBinCenter(ny));
				if (mt>0 && zt>0 && zt>=mt*zmin){
					
					SetConditions(zt,mt,zmin);
					for (Int_t nn=0;nn<stat;nn+=1){ 
						meth.Execute(this,"",ret);
						if (ret==1)
							TreatePartition();
						stat_par+=1;
						if ( stat_par%1000 == 0 ) printf("%d partitions generees sur %d\n",stat_par,stat_tot);
					}
				}
				else {
					cout << zt << " " << mt << endl;
				}
			}
		}
	
	iterations_total+=stat_par;

	Stop();
	Info("BreakNtimesFromHisto","Temps ecoule en secondes : %d",GetDeltaTime());

	if (TestBit(kStorePartitions)){
		TransfertFromTree();
	}

}		

void KVBreakUp::DrawPanel(){
		
	TCanvas* c1 = 0;
	if ( !(c1 = (TCanvas* )gROOT->GetListOfCanvases()->FindObject("BreakUp_Control")) ){
		
		c1 = new TCanvas("BreakUp_Control","Control",0,0,900,900);
		c1->Divide(2,2,0.001,0.001,10);
		c1->cd(1);	if (hzz->GetEntries()>0)	gPad->SetLogy(1); hzz->Draw();
		c1->cd(2);	if (hzt->GetEntries()>0)	gPad->SetLogy(1); hzt->Draw();
		c1->cd(3);	if (hmt->GetEntries()>0)	gPad->SetLogy(1); hmt->Draw();
		
	}
}

void KVBreakUp::ResetHistos(){ lhisto->Execute("Reset",""); }

void KVBreakUp::ResetTree(){ GetTree()->Reset();}

void KVBreakUp::ResetManager(){ GetManager()->Reset(); }


void KVBreakUp::SaveHistos(KVString filename,KVString suff,Bool_t update){

	if (filename=="") filename="KVBreakUp_Ouput.root";
	if (suff=="")	
		suff.Form("Zt%d_Mt%d_Zm%d_%s",GetZtot(),GetMtot(),GetZmin(),GetBreakUpMethod().Data());
	TFile* file = 0;
	if (update) file = new TFile(filename.Data(),"update");
	else 			file = new TFile(filename.Data(),"recreate");
	
	KVString snom;
	for (Int_t nn=0;nn<lhisto->GetEntries();nn+=1){
		snom.Form("%s_%s",lhisto->At(nn)->GetName(),suff.Data());
		lhisto->At(nn)->Write(snom.Data());
	}
	file->Close();

}

void KVBreakUp::SaveTree(KVString filename,KVString suff,Bool_t update){
	
	if (!TestBit(kFillTree)) return;
	
	TFile* file = 0;
	if (update) file = new TFile(filename.Data(),"update");
	else 			file = new TFile(filename.Data(),"recreate");
	
	KVString snom;
	snom.Form("%s_%s",tt->GetName(),suff.Data());
	tt->Write(snom.Data());
	
	file->Close();

}

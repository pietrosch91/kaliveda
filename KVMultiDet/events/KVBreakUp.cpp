//Created by KVClassFactory on Thu Mar 25 11:25:27 2010
//Author: bonnet

#include "KVBreakUp.h"
#include "TMath.h"
#include "TMethodCall.h"
#include "TRandom3.h"

ClassImp(KVBreakUp)

////////////////////////////////////////////////////////////////////////////////
/* 
BEGIN_HTML
<h2>KVBreakUp</h2>
<h4>Partitioning (KVPartition) of an integer in a given number of intenger
under different conditions and/or way of break up
</h4>
END_HTML
Initialisation :
- Méthode SetConditions(Int_t zt,Int_t mt,Int_t zmin=1), définie la taille totale de la partition,
le nombre de fragments et la taille minimale de la partition
- Méthode DefineBreakUpMethod(KVString bup_method=""), permet de definir la facon de casser la taille initiale
Sont implémentées : 
		BreakUsingChain (defaut) -> On casse aléatoirement les liens entre charge
		BreakUsingLine				-> On casse aléatoirement les liens entre charge + "effets de bords"
		BreakUsingIndividual		-> On tire aleatoirement une taille puis les autres tailles seront tirées a partir de la charge restante
		BreakUsingPile				-> On distribue 1 par un 1 les charges entre les fragments
		etc...
Normalement toutes ces methodes garantissent à chaque tirage, les conditions imposées par SetConditions
- Méthode RedefineTRandom(KVString TRandom_Method), permet de redefinir la classe de tirage aleatoire voir TRandom et classes filles, la classe
par default est TRandom3
Exemple d'utilisation : 
void test{

KVBreakUp* bu = new KVBreakUp();
bu->SetConditions(80,6,5);
bu->StorePartitions();
bu->BreakNtimes(10000);
bu->GetManager()->TransfertToMainList();

bu->BreakNtimesOnGaussian(10000,80, 5, 6, 1, 5);
bu->GetManager()->TransfertToMainList();

bu->GetTree()->GetListOfBranches()->ls();
bu->GetHistos()->ls();

bu->DrawPanel();

}

*/
////////////////////////////////////////////////////////////////////////////////


void KVBreakUp::init(void)
{
	
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
	niter_tot = 0;
	tstart = tstop = tellapsed = 0;
	
	StorePartitions();
}

//_______________________________________________________
KVBreakUp::KVBreakUp()
{ 

	init();

}
	
//_______________________________________________________
KVBreakUp::~KVBreakUp(){
	
	delete alea;
	delete lhisto;
	TCanvas* c1 = 0;
	if ( (c1 = (TCanvas* )gROOT->GetListOfCanvases()->FindObject("BreakUp_Control")) ) delete c1;
	if (bound) delete [] bound;
		
	lobjects->Clear();
	delete lobjects;
			
}
//_______________________________________________________
void KVBreakUp::Clear(Option_t* opt){

	GetManager()->Clear(opt);
	ResetHistos();
	niter_tot = 0;

}

//_______________________________________________________
void KVBreakUp::DefineHistos(){
	
	lhisto = new KVList();
	//histogramme de control
	hzz = new TH1F("KVBreakUp_hzz","distri z",200,-0.5,199.5);	lhisto->Add(hzz);
	hzt = new TH1F("KVBreakUp_hzt","h ztotal",200,-0.5,199.5);	lhisto->Add(hzt);
	hmt = new TH1F("KVBreakUp_hmt","h mtotal",200,-0.5,199.5);	lhisto->Add(hmt);
}
	
//_______________________________________________________
void KVBreakUp::SetZtot(Int_t zt) 
{
	//Protected method	
	if (zt!=Ztotal){
		Ztotal=zt; 
		if (bound) delete bound;	bound = new Int_t[Ztotal];
		for (Int_t nn=0;nn<Ztotal;nn+=1) bound[nn]=1;
		nbre_nuc = Ztotal-Mtotal*Zmin;
	}
}
	
//_______________________________________________________
void KVBreakUp::SetMtot(Int_t mt)
{ 
	//Protected method	
	if (mt>size_max) {
		Warning("SetMtot","%d -> La multiplicite max (%d) est depassee",mt,size_max);
		exit(EXIT_FAILURE);
	}
	Mtotal=mt; 
	nbre_nuc = Ztotal-Mtotal*Zmin;
}
	
//_______________________________________________________
void KVBreakUp::SetZmin(Int_t zlim)
{

	//Protected method	
	Zmin=zlim; 
	nbre_nuc = Ztotal-Mtotal*Zmin;

}

//_______________________________________________________
void KVBreakUp::SetBreakUpMethod(KVString bup_method)
{
	
	//Protected method	
	BreakUpMethod=bup_method;

}

//_______________________________________________________
void KVBreakUp::RedefineTRandom(KVString TRandom_Method)
{
	if (alea) delete alea;
	TClass *cl = new TClass(TRandom_Method.Data());
	alea = (TRandom* )cl->New();
	delete cl;
}
	
//_______________________________________________________
void KVBreakUp::SetConditions(Int_t zt,Int_t mt,Int_t zmin)
{
	SetZtot(zt);
	SetMtot(mt);
	SetZmin(zmin);
}

//_______________________________________________________
void KVBreakUp::DefineBreakUpMethod(KVString bup_method)
{
	
	if (bup_method==""){
		Info("DefineBreakUpMethod", "Available methods are");
		cout << "BreakUsingChain" << endl;
		cout << "BreakUsingPile" << endl;
		cout << "BreakUsingIndividual" << endl;
		cout << "BreakUsingLine" << endl;
		cout << "Make your choice" << endl;
	}
	else {
		SetBreakUpMethod(bup_method);
	}

}

//_______________________________________________________
Int_t KVBreakUp::GetZtot(void) const
{

	return Ztotal;

}

//_______________________________________________________
Int_t KVBreakUp::GetMtot(void) const
{

	return Mtotal;

}

//_______________________________________________________
Int_t KVBreakUp::GetZmin(void) const
{

	return Zmin;
	
}

//_______________________________________________________
KVString KVBreakUp::GetBreakUpMethod(void) const
{

	return BreakUpMethod;

}

//_______________________________________________________
void KVBreakUp::StorePartitions(Bool_t choix){
	
	SetBit(kStorePartitions,choix);
	
	if ( !GetManager() ){
		parman = new KVPartitionList();
		if (!lobjects) lobjects = new KVList(); 
		lobjects->Add(parman);
	}
}

//_______________________________________________________
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

//_______________________________________________________
Int_t KVBreakUp::BreakUsingLine(void)
{

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

//_______________________________________________________
Int_t KVBreakUp::BreakUsingIndividual(void)
{

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

//_______________________________________________________
Int_t KVBreakUp::BreakUsingPile(void)
{

	
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

//_______________________________________________________
void KVBreakUp::TreatePartition()
{

	hmt->Fill(Mtotal);
	hzt->Fill(Ztotal);
	
	partition = new KVIntegerList();
	partition->Fill(size,Mtotal);
			
	if (parman->Fill(partition)) delete partition;

}
	
//_______________________________________________________
void KVBreakUp::BreakNtimes(Int_t times)
{

	
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
	niter_tot+=times;	
	
	Stop();
	Info("BreakNtimes","Temps ecoule en secondes : %d",GetDeltaTime());

}	

//_______________________________________________________
void KVBreakUp::BreakNtimesOnGaussian(Int_t times,Double_t Ztot_moy,Double_t Ztot_rms,Double_t Mtot_moy,Double_t Mtot_rms,Int_t zmin)
{

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
	niter_tot+=times;
	
	Stop();
	Info("BreakNtimesOnGaussian","Temps ecoule en secondes : %d",GetDeltaTime());

}

//_______________________________________________________
void KVBreakUp::BreakFromHisto(TH2F* hh_zt_VS_mt,Int_t zmin)
{

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
	
	niter_tot+=stat_par;

	Stop();
	Info("BreakNtimesFromHisto","Temps ecoule en secondes : %d",GetDeltaTime());

}		

//_______________________________________________________
Int_t KVBreakUp::TotalIterations(void)
{

	return niter_tot;

}

//_______________________________________________________
KVList* KVBreakUp::GetHistos()
{

	return lhisto;
	
}

//_______________________________________________________
KVList* KVBreakUp::GetObjects()
{

	return lobjects;
	
}
	
//_______________________________________________________
KVPartitionList* KVBreakUp::GetManager()
{
	
	return parman;

}

//_______________________________________________________
void KVBreakUp::ResetTotalIterations()
{

	niter_tot=0;
	
}

//_______________________________________________________
void KVBreakUp::ResetHistos(){

	lhisto->Execute("Reset","");
	
}

//_______________________________________________________
void KVBreakUp::DrawPanel()
{
		
	TCanvas* c1 = 0;
	if ( !(c1 = (TCanvas* )gROOT->GetListOfCanvases()->FindObject("BreakUp_Control")) ){
		
		c1 = new TCanvas("BreakUp_Control","Control",0,0,900,900);
		c1->Divide(2,2,0.001,0.001,10);
		c1->cd(1);	if (hzz->GetEntries()>0)	gPad->SetLogy(1); hzz->Draw();
		c1->cd(2);	if (hzt->GetEntries()>0)	gPad->SetLogy(1); hzt->Draw();
		c1->cd(3);	if (hmt->GetEntries()>0)	gPad->SetLogy(1); hmt->Draw();
		
	}
}

//_______________________________________________________
void KVBreakUp::SaveHistos(KVString filename,KVString suff,Option_t* option){

	if (filename=="") filename="KVBreakUp_Ouput.root";
	if (suff=="")	
		suff.Form("Zt%d_Mt%d_Zm%d_%s",GetZtot(),GetMtot(),GetZmin(),GetBreakUpMethod().Data());
	TFile* file = new TFile(filename.Data(),option);

	KVString snom;
	for (Int_t nn=0;nn<lhisto->GetEntries();nn+=1){
		snom.Form("%s_%s",lhisto->At(nn)->GetName(),suff.Data());
		lhisto->At(nn)->Write(snom.Data());
	}
	file->Close();

}

//_______________________________________________________
void KVBreakUp::Print(Option_t* option) const
{
	Info("Print","Configuration for the break up");
	printf(" Ztot=%d - Mtot=%d - Zmin=%d\n",GetZtot(),GetMtot(),GetZmin());
	printf(" Charge to be distributed %d - Biggest possible charge %d\n",nbre_nuc,Zmin+nbre_nuc);
	printf(" Methode de cassage aleatoire %s\n",GetBreakUpMethod().Data());
	alea->Print();
	printf(" Partition are stored via KVPartitionList : %d\n",Int_t(TestBit(kStorePartitions)));
	printf("------------------------------------------------------");
}
	
//_______________________________________________________
void KVBreakUp::Start()
{

	TDatime time;
	tstart = time.GetHour()*3600+time.GetMinute()*60+time.GetSecond();

}

//_______________________________________________________
void KVBreakUp::Stop()
{
	TDatime time;
	tstop = time.GetHour()*3600+time.GetMinute()*60+time.GetSecond();
	tellapsed = tstop-tstart;

}

//_______________________________________________________
Int_t KVBreakUp::GetDeltaTime()
{

	return tellapsed;

}

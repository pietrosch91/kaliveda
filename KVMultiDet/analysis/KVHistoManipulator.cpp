/*
$Id: KVHistoManipulator.cpp,v 1.9 2009/04/07 14:54:15 ebonnet Exp $
$Revision: 1.9 $
$Date: 2009/04/07 14:54:15 $
*/

//Created by KVClassFactory on Thu Oct 18 11:48:18 2007
//Author: Eric Bonnet

#include "KVHistoManipulator.h"
#include "Riostream.h"

#include "TProfile.h"
#include "TH1.h"
#include "TH2.h"

#include "TF1.h"
#include "TCutG.h"
#include "TList.h"
#include "TString.h"
#include "TClass.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "TROOT.h"
#include "TMethodCall.h"
#include "TMath.h"

#include "KVNumberList.h"
#include "KVList.h"

#include "TSpline.h"


ClassImp(KVHistoManipulator)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVHistoManipulator</h2>
<h4>Propose differentes operations sur les histo</h4>
<h5>En evolution merci de me tenir informer d'eventuels bugs bonnet@ganil.fr</h5>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVHistoManipulator *gHistoManipulator;

KVHistoManipulator::KVHistoManipulator(void){ 
	//Default constructor
	init();
	gHistoManipulator = this;
}


KVHistoManipulator::~KVHistoManipulator()
{
   if (gHistoManipulator == this)
      gHistoManipulator = 0;

}

//###############################################################################################################"
//-------------------------------------------------
Int_t KVHistoManipulator::CutStatBin(TH1* hh,Int_t stat_min,Int_t stat_max){
//-------------------------------------------------
	// IMPORTANT l'histo est modifie 
	//
	// Efface les bins dont la statistique est hors de l 'intervalle ]stat_min,stat_max[
	// l'erreur et le contenu sont mis a zero
	// le nombre d entree (GetEntries) reste inchange 
	// pour les TH1 cela correspond a GetBinContent(xx)
	// pour les TH2 cela correspond a GetBinContent(xx,yy) --> cellules
	// pour les TProfile cela correspond a GetBinEntries(xx) 
	// la fonction renvoie le nbre de bins ou cellules mis a zero
	// si stat_min ou stat_max sont egales à -1 la borne associée n'est pas prise en compte
	if (!hh) { cout << "pointeur histogramme nul" << endl; return -1; }
	Int_t nb_raz=0;
	Bool_t raz=kFALSE;
	if (hh->InheritsFrom("TH2")){
		cout << "TH2"<< endl;
		for (Int_t nx=1;nx<=hh->GetNbinsX();nx+=1){
			for (Int_t ny=1;ny<=hh->GetNbinsY();ny+=1) {
				raz=kFALSE;
				if (stat_min!=-1 && hh->GetBinContent(nx,ny)<stat_min) raz=kTRUE;
				if (stat_max!=-1 && hh->GetBinContent(nx,ny)>stat_max) raz=kTRUE;
				if (raz) { hh->SetBinContent(nx,ny,0); hh->SetBinError(nx,ny,0); nb_raz+=1; }
			}
		}	 
	}
	else if (hh->InheritsFrom("TProfile")){
		TProfile *prof = (TProfile *)hh;
		cout << "TProfile"<< endl;
		for (Int_t nx=1;nx<=prof->GetNbinsX();nx+=1){
			raz=kFALSE;
			if (stat_min!=-1 && prof->GetBinEntries(nx)<stat_min) raz=kTRUE;
			if (stat_max!=-1 && prof->GetBinEntries(nx)>stat_max) raz=kTRUE;
			if (raz) { prof->SetBinContent(nx,0); prof->SetBinEntries(nx,0); prof->SetBinError(nx,0); nb_raz+=1; }
		}	 
	}
	else if (hh->InheritsFrom("TH1")){
		cout << "TH1"<< endl;
		for (Int_t nx=1;nx<=hh->GetNbinsX();nx+=1){
			raz=kFALSE;
			if (stat_min!=-1 && hh->GetBinContent(nx)<stat_min) raz=kTRUE;
			if (stat_max!=-1 && hh->GetBinContent(nx)>stat_max) raz=kTRUE;
			if (raz) { hh->SetBinContent(nx,0); hh->SetBinError(nx,0);nb_raz+=1; }
		} 
	}
	
	return nb_raz;
}

//###############################################################################################################"
//-------------------------------------------------
Int_t KVHistoManipulator::Apply_TCutG(TH2* hh,TCutG *cut,TString mode){
//-------------------------------------------------
	// IMPORTANT l'histo est modifie 
	//
	// Applique une selection sur un histo 2D a partir d'un TCutG;
	// Si mode=In seules les cellules comprises dans le TCutG sont gardes
	// mode=Out --> Inverse
	// la fonction renvoie le nbre de cellules mis a zero
	// Attention le test ne se fait que sur les valeurs centrales des cellules (GetBinCenter())
	
	if (!hh) { cout << "pointeur histogramme nul" << endl; return -1; }
	Int_t nb_raz=0;
	Bool_t raz=kFALSE;
		
	for (Int_t nx=1;nx<=hh->GetNbinsX();nx+=1){
		for (Int_t ny=1;ny<=hh->GetNbinsY();ny+=1) {
			raz=kFALSE;
			Double_t valx = hh->GetXaxis()->GetBinCenter(nx);
			Double_t valy = hh->GetYaxis()->GetBinCenter(ny);
			if (mode=="in" && !cut->IsInside(valx,valy)) raz=kTRUE;
			if (mode=="out" && cut->IsInside(valx,valy)) raz=kTRUE;
			if (raz) { hh->SetBinContent(nx,ny,0); hh->SetBinError(nx,ny,0); nb_raz+=1; }
		}
	}	 
	
	return nb_raz;
}

//###############################################################################################################
//-------------------------------------------------
TH1* KVHistoManipulator::ScaleHisto(TH1 *hh,TF1*fx,TF1*fy,Int_t nx,Int_t ny,Double_t xmin,Double_t xmax,Double_t ymin,Double_t ymax,Option_t* norm){
//-------------------------------------------------
	// Applique les transformations correspondantes aux fonctions (TF1 *) donnees en argument
	// et donne en sortie l histogramme transforme celui ci a pour nom "nom_de_histo_input"_scaled
	//
	// IMPORTANT l'histo d'entree n'est pas modifie 
	//
	// pour TH1 et TProfile n'est pris en compte que TF1 *fx;
	// les parametres de la fonction doivent etre initialises avant.
	// Si la fonction est un pointeur NULL, aucune transformation n est appliquee et l axe reste tel quel.
	// L'intervalle de validite des fonctions TF1::SetRange est determine a partir des limites de l histo apres transformations
	// nx xmin et xmax sont les parametres de binnage des histo
	// si nx=-1, ceux ci sont recalcules automatiquement nx = nx_initial, xmin = fx(xmin), xmax = fx(xmax)
	// si nx!=-1, xmin et xmax doivent etre aussi specifies
	// il en est de meme pour l'axe y
	//
	// OPTION: norm
	//  norm = "" (default) : no adjustment is made for the change in bin width due to the transformation
	//  norm = "width" : bin contents are adjusted for width change, so that the integral of the histogram
	//                   contents taking into account the bin width (i.e. TH1::Integral("width")) is the same.
	//
	// NOTE for 1D histos:
	// If the binning of the scaled histogram is imposed by the user (nx!=-1), then in order
	// to achieve a continuous scaled distribution we have to randomize X within each bin.
	// This will only work if the bin contents of 'hh' are integer numbers, i.e. unnormalised raw data histogram.

	TRandom3 *alea = new TRandom3();	//generateur de nbre aleatoire
	Bool_t width = !strcmp(norm,"width");
	TH1 *gg = NULL;
	Double_t abs;
	Bool_t fixed_bins = (nx!=-1);
	if (fx){
		if (nx==-1){
			nx = hh->GetNbinsX();
			abs = hh->GetXaxis()->GetBinLowEdge(1); 					xmin = fx->Eval(abs);
			abs = hh->GetXaxis()->GetBinUpEdge(hh->GetNbinsX()); 	xmax = fx->Eval(abs);
		}
		fx->SetRange(hh->GetXaxis()->GetBinLowEdge(1),hh->GetXaxis()->GetBinUpEdge(nx)); fx->SetNpx(nx+1);
	}
	else {
		nx = hh->GetNbinsX();
		xmin = hh->GetXaxis()->GetBinLowEdge(1); 
		xmax = hh->GetXaxis()->GetBinUpEdge(hh->GetNbinsX());
	}
	
	if (hh->InheritsFrom("TH2")){
		if (fy){
			if (ny==-1){
				ny = hh->GetNbinsY();
				abs = hh->GetYaxis()->GetBinLowEdge(1); 					ymin = fy->Eval(abs);
				abs = hh->GetYaxis()->GetBinUpEdge(hh->GetNbinsY()); 	ymax = fy->Eval(abs);
			}
			fy->SetRange(hh->GetYaxis()->GetBinLowEdge(1),hh->GetYaxis()->GetBinUpEdge(ny)); fy->SetNpx(ny+1);
		}
		else {
			ny = hh->GetNbinsY();
			ymin = hh->GetYaxis()->GetBinLowEdge(1); 
			ymax = hh->GetYaxis()->GetBinUpEdge(hh->GetNbinsY());
		}
	}	

	TClass *clas = gROOT->GetClass(hh->ClassName());
	gg = (TH1 *) clas->New();
	TString hname; hname.Form("%s_scaled",hh->GetName());
	gg->SetNameTitle(hname.Data(),hh->GetTitle());
	if (!gg) return NULL;

	if (hh->InheritsFrom("TH2")) 	gg->SetBins(nx,xmin,xmax,ny,ymin,ymax);
	else 									gg->SetBins(nx,xmin,xmax);

	// if option norm="width', take account of change of X bin width between original and scaled histograms
	Double_t Xbin_width_corr = 1.0, Ybin_width_corr = 1.0;
	if(width){
		Double_t orig_Xbin_width = (hh->GetXaxis()->GetXmax()-hh->GetXaxis()->GetXmin())/hh->GetNbinsX();
		Double_t new_Xbin_width = (gg->GetXaxis()->GetXmax()-gg->GetXaxis()->GetXmin())/gg->GetNbinsX();
		Xbin_width_corr = orig_Xbin_width/new_Xbin_width;
		if (hh->InheritsFrom("TH2")){
			// Take account of change of X bin width between original and scaled histograms
			Double_t orig_Ybin_width = (hh->GetYaxis()->GetXmax()-hh->GetYaxis()->GetXmin())/hh->GetNbinsY();
			Double_t new_Ybin_width = (gg->GetYaxis()->GetXmax()-gg->GetYaxis()->GetXmin())/gg->GetNbinsY();
			Ybin_width_corr = orig_Ybin_width/new_Ybin_width;
		}
	}
	
	for (Int_t xx=1;xx<=hh->GetNbinsX();xx+=1){
		Double_t bmin = hh->GetXaxis()->GetBinLowEdge(xx);
		Double_t bmax = hh->GetXaxis()->GetBinUpEdge(xx);
		abs  = alea->Uniform(bmin,bmax); if (abs==bmax) abs=bmin; 
		Double_t resx = abs;
		if (fx) resx = fx->Eval(abs);
		if (hh->InheritsFrom("TH2")){
			for (Int_t yy=1;yy<=hh->GetNbinsY();yy+=1){
				if (hh->GetBinContent(xx,yy)>0){
					bmin = hh->GetYaxis()->GetBinLowEdge(yy);
					bmax = hh->GetYaxis()->GetBinUpEdge(yy);
					abs  = alea->Uniform(bmin,bmax); if (abs==bmax) abs=bmin; 
					Double_t resy = abs;
					if (fy) resy = fy->Eval(abs);
					gg->SetBinContent(gg->GetXaxis()->FindBin(resx),
							gg->GetYaxis()->FindBin(resy),
							hh->GetBinContent(xx,yy)*Xbin_width_corr*Ybin_width_corr);
					//gg->SetBinError(gg->GetXaxis()->FindBin(resx),gg->GetYaxis()->FindBin(resy),hh->GetBinError(xx,yy));
				}
			}
		}
		else {
			// 1-D histogram
			if(fixed_bins){
				// histogram binning imposed by user. we need to randomise inside bins of original histogram
				// otherwise scaled histogram will be discontinuously filled.
				Int_t nmax = (Int_t)hh->GetBinContent(xx);
				for(register int i=0;i<nmax;i++){
					abs  = alea->Uniform(bmin,bmax);
					Double_t resx = fx->Eval(abs);
					gg->Fill(resx,Xbin_width_corr);
				}
			}
			else
			{
				// range and binning calculated from function.
				Double_t resy = hh->GetBinContent(xx);
				if (fy) resy = fy->Eval(resy);
				gg->SetBinContent(gg->GetXaxis()->FindBin(resx),resy*Xbin_width_corr); 
			}
		}
	}

	delete alea;
	return gg;

}

//###############################################################################################################
//-------------------------------------------------
TGraph* KVHistoManipulator::ScaleGraph(TGraph *hh,TF1*fx,TF1*fy){
//-------------------------------------------------
	// Applique les transformations correspondantes aux fonctions (TF1 *) donnees en argument
	// et donne en sortie le graph transforme celui ci a pour nom "nom_de_histo_input"_scaled
	//
	// IMPORTANT le graph d'entree n'est pas modifie 
	// 
	// les parametres de la fonction doivent etre initialises avant.
	// Si la fonction est un pointeur NULL, aucune transformation n est appliquee et l axe reste tel quel.
	
	TGraph* gg = NULL;
	TClass *clas = gROOT->GetClass(hh->ClassName());
	gg = (TGraph *) clas->New();
	TString hname; hname.Form("%s_scaled",hh->GetName());
	gg->SetNameTitle(hname.Data(),hh->GetTitle());
	if (!gg) return NULL;

	Int_t np = hh->GetN();
	for (Int_t nn=0;nn<np;nn+=1){
		Double_t xx1,yy1;
		hh->GetPoint(nn,xx1,yy1);
		Double_t xx2 = xx1; if (fx) xx2 = fx->Eval(xx1);
		Double_t yy2 = yy1; if (fy) yy2 = fy->Eval(yy1);
		gg->SetPoint(nn,xx2,yy2);
		if (gg->InheritsFrom("TGraphErrors")) {
			((TGraphErrors* )gg)->SetPointError(nn,((TGraphErrors* )hh)->GetErrorX(nn),((TGraphErrors* )hh)->GetErrorY(nn));
		}
	}	

	return gg;

}
//###############################################################################################################
//-------------------------------------------------
TH1* KVHistoManipulator::CentreeReduite(TH1 *hh,Int_t nx,Int_t ny,Double_t xmin,Double_t xmax,Double_t ymin,Double_t ymax){
//-------------------------------------------------
	//Exemple d utilisation de la methode KVHistoManipulator::ScaleHisto avec ici 
	//L'obtention des distributions centrees reduites
	
	if (!hh) { cout << "pointeur histogramme nul" << endl; return NULL; }
	
	TString expression;
	expression.Form("(x-%lf)/%lf",hh->GetMean(1),hh->GetRMS(1));
	TF1 *fx = new TF1("fx",expression.Data());
	TF1 *fy = NULL;
	if (hh->InheritsFrom("TH2")){
		expression.Form("(x-%lf)/%lf",hh->GetMean(2),hh->GetRMS(2));
		fy = new TF1("fy",expression.Data());
	}	
	
	TH1* gg = ScaleHisto(hh,fx,fy,nx,ny,xmin,xmax,ymin,ymax);
	TString hname; hname.Form("%s_centred",hh->GetName()); gg->SetName(hname.Data());
	if (fx) delete fx;
	if (fy) delete fy;
	
	return gg;

}
//###############################################################################################################
//-------------------------------------------------
TH2* KVHistoManipulator::CentreeReduiteX(TH2 *hh,Int_t nx,Double_t xmin,Double_t xmax){
//-------------------------------------------------
	//Exemple d utilisation de la methode KVHistoManipulator::ScaleHisto avec ici 
	//L'obtention des distributions centrees reduites
	
	if (!hh) { cout << "pointeur histogramme nul" << endl; return NULL; }
	
	TString expression;
	expression.Form("(x-%lf)/%lf",hh->GetMean(1),hh->GetRMS(1));
	TF1 *fx = new TF1("fx",expression.Data());
	TH2* gg = (TH2* )ScaleHisto(hh,fx,NULL,nx,-1,xmin,xmax,-1.,-1.);
	TString hname; hname.Form("%s_centred_X",hh->GetName()); gg->SetName(hname.Data());
	if (fx) delete fx;
	
	return gg;

}
//###############################################################################################################
//-------------------------------------------------
TH2* KVHistoManipulator::CentreeReduiteY(TH2 *hh,Int_t ny,Double_t ymin,Double_t ymax){
//-------------------------------------------------
	//Exemple d utilisation de la methode KVHistoManipulator::ScaleHisto avec ici 
	//L'obtention des distributions centrees reduites
	
	if (!hh) { cout << "pointeur histogramme nul" << endl; return NULL; }
	
	TString expression;
	expression.Form("(x-%lf)/%lf",hh->GetMean(1),hh->GetRMS(1));
	TF1 *fy = new TF1("fy",expression.Data());
	TH2* gg = (TH2* )ScaleHisto(hh,NULL,fy,-1,ny,-1.,-1.,ymin,ymax);
	TString hname; hname.Form("%s_centred_Y",hh->GetName()); gg->SetName(hname.Data());
	if (fy) delete fy;
	
	return gg;

}

//###############################################################################################################
//-------------------------------------------------
TH2* KVHistoManipulator::RenormaliseHisto(TH2 *hh,Int_t bmin,Int_t bmax,TString axis,Double_t valref){
//-------------------------------------------------
	// Renormalisation de l histogramme 2D sous la contrainte d'une ditribution plate suivant X ou Y
	// (signal de bimodalite par exemple)
	// et donne en sortie l histogramme transforme celui ci a pour nom "nom_de_histo_input"_normalised
	//
	// IMPORTANT l'histo d'entree n'est pas modifie 
	//
	// bmin, bmax : intervalle de bins ou l'on effectue la renormalisation (par defaut -1,-1 --> toute la plage)
	// les contenus hors de l intervalle [bmin,bmax] sont remis a zero
	// Si on veut une distribution plate en X ils indiquent l'intervalle sur cet axe x
	// valref : valeur en stat de la distribution plate (par defaut 1)
	// 
	// ATTENTION la propagation des erreurs n est pas (pour l instant) implementee
	
	if (!hh) { cout << "pointeur histogramme nul" << endl; return NULL; }

	if (bmin==-1) bmin=1;
	TString hname; hname.Form("%s_normalised",hh->GetName());
	TH2* clone=0;
	if ( (clone = (TH2* )gDirectory->Get(hname.Data())) ) delete clone;
	clone = (TH2 *)hh->Clone(hname.Data()); clone->Reset();

	if (axis=="X"){
		if (bmax==-1) { bmax=hh->GetNbinsX(); }
		for (Int_t nx=bmin;nx<=bmax;nx+=1){
			Double_t integ=0; for (Int_t ny=1;ny<=hh->GetNbinsY();ny+=1) integ+=hh->GetBinContent(nx,ny);
			if (integ>0){ 
				for (Int_t ny=1;ny<=hh->GetNbinsY();ny+=1){
					clone->SetBinContent(nx,ny,hh->GetBinContent(nx,ny)*valref/integ); 
					if (hh->GetBinContent(nx,ny)>0){
						Double_t erreur = clone->GetBinContent(nx,ny)*TMath::Sqrt(1./hh->GetBinContent(nx,ny)+1./integ);
						clone->SetBinError(nx,ny,erreur);
					}
				}
			}
		}	
	}		
	else if (axis=="Y"){
		if (bmax==-1) { bmax=hh->GetNbinsY(); }
		for (Int_t ny=bmin;ny<=bmax;ny+=1){
			Double_t integ=0; for (Int_t nx=1;nx<=hh->GetNbinsX();nx+=1) integ+=hh->GetBinContent(nx,ny);
			if (integ>0){ 
				for (Int_t nx=1;nx<=hh->GetNbinsX();nx+=1){
					clone->SetBinContent(nx,ny,hh->GetBinContent(nx,ny)*valref/integ);
					Double_t erreur = clone->GetBinContent(nx,ny)*TMath::Sqrt(1./hh->GetBinContent(nx,ny)+1./integ);
					clone->SetBinError(nx,ny,erreur);
				}
			}
		}
	}
	else { cout << "l option TString axis doit etre X ou Y" << endl; }	 
	return clone;
}
//###############################################################################################################
//-------------------------------------------------
TH2* KVHistoManipulator::RenormaliseHisto(TH2 *hh,Double_t valmin,Double_t valmax,TString axis,Double_t valref){
//-------------------------------------------------
	// Les bornes valmin, valmax definissent l'intervalle ou l on effectue la renormalisation
	// On en derive les valeurs de bins
	// pour la methode KVHistoManipulator::RenormaliseHisto(TH1 *hh,TString axis,Double_t valref,Int_t bmin,Int_t bmax)
	
	if (!hh) { cout << "pointeur histogramme nul" << endl; return NULL; }
	if (axis=="X") 		return RenormaliseHisto(hh,hh->GetXaxis()->FindBin(valmin),hh->GetXaxis()->FindBin(valmax),axis,valref);
	else if (axis=="Y") 	return RenormaliseHisto(hh,hh->GetYaxis()->FindBin(valmin),hh->GetYaxis()->FindBin(valmax),axis,valref);
	else { cout << "l option TString axis doit etre X ou Y" << endl; return NULL; }					

}

//###############################################################################################################
//-------------------------------------------------
TH1* 	KVHistoManipulator::CumulatedHisto(TH1* hh,TString direction,Int_t bmin,Int_t bmax, Option_t* norm){
//-------------------------------------------------
	// Cumule le contenu de l histo hh entre bin bmin et bmax et retourne l histo correspondant
	// Si direction="C" (default):
	// SetBinContent(n) = GetBinContent(bmin)+GetBinContent(bmin+1)+ ... + GetBinContent(n)
	// Si direction="D":
	// SetBinContent(n) = GetBinContent(bmax)+GetBinContent(bmax-1)+ ... + GetBinContent(n)
	// 
	// Donne en sortie l histogramme transforme celui ci a pour nom "nom_de_histo_input"_cumulated
	//
	// IMPORTANT l'histo d'entree n'est pas modifie 
	//
	// si bmin=-1, bmin=1
	// si bmax=-1, bmax=GetNbinsX()
	// les contenus hors de l intervalle [bmin,bmax] sont remis a zero
	//
	// Avec norm = "surf" (default) l'integral de l histo cumule est egale a 1
	// Avec norm = "max" le contenu de l'histogram est renormalise de facon a ce que le maximum soit 1
	
	if (!hh) { cout << "pointeur histogramme nul" << endl; return NULL; }
	if(direction!="C"&&direction!="D"){
		cout << "l option TString direction doit etre C ou D" << endl; return NULL;
	}
	if (TString(hh->ClassName()).Contains("TH1")){
		if (bmin==-1) bmin=1;
		if (bmax==-1) bmax=hh->GetNbinsX();
		TString hname; hname.Form("%s_cumulated",hh->GetName());
		TH1* clone = (TH1 *)hh->Clone(hname.Data()); clone->Reset();
		// get array of cumulated bins
		Double_t* integral = hh->GetIntegral();
		for (Int_t nx=bmin;nx<=bmax;nx+=1){	
			if (direction=="C") clone->SetBinContent(nx, integral[nx]);
			else if(direction=="D") clone->SetBinContent(nx, 1.0-integral[nx]);
		}
		// normalisation
		if(!strcmp(norm,"surf")){
			Double_t sw = clone->GetSumOfWeights();
			if(sw>0) clone->Scale(1./sw);
		}
		else if(!strcmp(norm,"max")){
			Double_t max = clone->GetMaximum();
			if(max>0) clone->Scale(1./max);
		}
		return clone;
	}
	else {
		cout << "cette methode n est pas prevue pour les TH2, TH3 ou TProfile" << endl;
		return NULL; 
	}
	
	}

//###############################################################################################################
//-------------------------------------------------
TH1* 	KVHistoManipulator::GetDerivative(TH1* hh,Int_t order){
//-------------------------------------------------
	// retourne la derivee d ordre 0, 1 ou 2 d'un histogramme
	// 0 -> correspond a un lissage (smooth) de la distribution
	// 1 et 2 correspondent aux derivees premieres et deuxiemes
	//
	//	0 -> derivee zero Yi = 1/35*(-3yi-2 + 12yi-1 +17yi +12yi1 -3yi2)
	//	1 -> derivee premiere Y'i = 1/12h*(yi-2 - 8yi-1 +8yi1 -1yi2)
	// 2 -> derivee seconde Y''i = 1/7h/h*(2yi-2 -1 yi-1 -2yi -1yi1 +2yi2)
	// les derivees pour les bins 1,2 et n-1,n ne sont pas calculees
	//
	// IMPORTANT l'histo d'entree n'est pas modifie 
	
	if (!hh) { cout << "pointeur histogramme nul" << endl; return NULL; }
	if ( !(0<=order && order<=2) ) { cout << "ordre "<< order << "n est pas implemente" << endl; return NULL; }
	if ( hh->GetDimension()==1 && !hh->InheritsFrom("TProfile")){
		
		TString hname; hname.Form("%s_derivated_%d",hh->GetName(),order);
		TH1* clone = 0; 
		/*
		if (hh->InheritsFrom("TProfile"))	clone = (TProfile *)hh->Clone(hname.Data()); 
		else 											clone = (TH1 *)hh->Clone(hname.Data()); 
		*/
		clone = (TH1 *)hh->Clone(hname.Data()); 
		clone->Reset();
		
		for (Int_t nx=3;nx<=hh->GetNbinsX()-2;nx+=1){
			Double_t dev = 0;
			if (order==0){
				dev = 1./35.*(
				-3*hh->GetBinContent(nx-2)
				+12*hh->GetBinContent(nx-1)
				+17*hh->GetBinContent(nx)
				+12*hh->GetBinContent(nx+1)
				-3*hh->GetBinContent(nx+2)
				);
			}
			else if (order==1){
				Double_t h = hh->GetBinWidth(1);
				dev = 1/12./h*(
					1*hh->GetBinContent(nx-2)
					-8*hh->GetBinContent(nx-1)
					+0*hh->GetBinContent(nx)
					+8*hh->GetBinContent(nx+1)
					-1*hh->GetBinContent(nx+2)
				);
			}
			else {
				Double_t h2 = pow(hh->GetBinWidth(1),2.);
				dev = 1/7./h2*(
				2*hh->GetBinContent(nx-2)
				-1*hh->GetBinContent(nx-1)
				-2*hh->GetBinContent(nx)
				-1*hh->GetBinContent(nx+1)
				+2*hh->GetBinContent(nx+2)
				);
			}
			clone->SetBinContent(nx,dev);
		}
		return clone;
	}
	else {
		cout << "cette methode n est pas prevue pour les TH2, TH3" << endl;
		return NULL; 
	}
	
	}

//###############################################################################################################
//-------------------------------------------------
TGraphErrors*  KVHistoManipulator::GetMomentEvolution(TH2 *hh,TString momentx,TString momenty,TString axis){
//-------------------------------------------------
	// Renvoie un TGraph
	// A partir d'un 2D histo, permet de tracer l'evolution d'un moment en fonction d'un autre moment
	// ou d'un moment d'un axe en fonction de l'aure axe.
	// Les TString momentx et momenty doivent etre des "Get like" methodes connues de TH1
	// comme GetMean, GetRMS ou GetKurtosis :
	// - Si momenty="" -> on obtient l'evolution du moment momentx en fonction de 
	// la variable associée a l'autre axe.
	// - Si momenty!="" -> on obtient l'evolution du moment momenty en fonction du momentx
	// 
	// Si TString axis = X la variable en X est le parametre d echantillonage et vice-versa
	// 
	// Exemple :
	// GetMomentEvolution(histo,"GetMean","GetSkewness","X") -> Evolution de la skewness en fonction de la moyenne de l'observable
	// en Y par tranche de l'observable X
	
	if (axis!="X" && axis!="Y") {
		cout << "GetMomentEvolution(TH2*,TString ,TString ,TString) Mauvaise syntaxe pour TString axis (X ou Y) " << endl;
		return NULL;
	}
	TMethodCall *cmx = new TMethodCall();  cmx->InitWithPrototype(gROOT->GetClass("TH1D"),Form("%s",momentx.Data()),"int");
	TMethodCall *Ecmx = new TMethodCall(); Ecmx->InitWithPrototype(gROOT->GetClass("TH1D"),Form("%sError",momentx.Data()),"int");
	if (!cmx->IsValid()) { 
		cout << "GetMomentEvolution(TH2*,TString ,TString ,TString) TString momentx n'est pas une methode valide " << momentx.Data() << endl;
		delete cmx; cmx=0; return NULL; 
	}
	else if (!Ecmx->IsValid()) {delete Ecmx; Ecmx=0; }
	
	TMethodCall *cmy = NULL,*Ecmy = NULL;
	if (momenty!="") 	{ 
		cmy = new TMethodCall(); 	cmy->InitWithPrototype(gROOT->GetClass("TH1D"),Form("%s",momenty.Data()),"int"); 
		Ecmy = new TMethodCall(); 	Ecmy->InitWithPrototype(gROOT->GetClass("TH1D"),Form("%sError",momenty.Data()),"int"); 
		if (!cmy->IsValid()) { 
			cout << "GetMomentEvolution(TH2*,TString ,TString ,TString) TString momenty n'est pas une methode valide " << momenty.Data() << endl;
			delete cmy; return NULL; 
		}
		else if (!Ecmy->IsValid()) {delete Ecmy; Ecmy=0; }
	}
	
	TString fmt_histo; fmt_histo.Form("GetMomentEvolution_%s",hh->GetName());
	KVNumberList lbins="";
	Int_t nmax=0;
	if (axis=="Y") nmax=hh->GetNbinsX(); else nmax=hh->GetNbinsY();
	
	Int_t npts=0; 
	for (Int_t nn=1;nn<=nmax;nn+=1){ 
		Double_t stat = 0;
		if (axis=="Y") stat=((TH1D* )hh->ProjectionY(fmt_histo.Data(),nn,nn))->Integral();  
		else 				stat=((TH1D* )hh->ProjectionX(fmt_histo.Data(),nn,nn))->Integral(); 
		if (stat>0) { npts+=1; lbins.Add(nn); } gDirectory->Delete(fmt_histo.Data());
	}	
	if (npts>0){
		TGraphErrors *gr = new TGraphErrors(npts);
		TH1D *hp;
		npts=0;
		Double_t valx,valy,Evaly=0,Evalx=0;
		lbins.Begin(); while(!lbins.End()) { 
			Int_t bin=lbins.Next(); 
			if (axis=="Y") hp = (TH1D* )hh->ProjectionY(fmt_histo.Data(),bin,bin); 
			else 				hp = (TH1D* )hh->ProjectionX(fmt_histo.Data(),bin,bin); 
			cmx->Execute(hp,"1",valx);
			if (Ecmx) Ecmx->Execute(hp,"1",Evalx);
			if (cmy) {	
				cmy->Execute(hp,"1",valy);
				if (Ecmy) Ecmy->Execute(hp,"1",Evaly);
				gr->SetPoint(npts,valx,valy);
				if (Evalx!=0 && Evaly!=0) gr->SetPointError(npts,Evalx,Evaly);
				npts+=1;
			}
			else {
				if (axis=="Y") { valy = hh->GetXaxis()->GetBinCenter(bin); Evaly = hh->GetXaxis()->GetBinWidth(bin)/2.; }
				else 				{ valy = hh->GetYaxis()->GetBinCenter(bin); Evaly = hh->GetYaxis()->GetBinWidth(bin)/2.; }
				gr->SetPoint(npts,valy,valx);
				if (Evalx!=0) gr->SetPointError(npts,Evaly,Evalx);
				npts+=1;
			}
			gDirectory->Delete(fmt_histo.Data());
		}
		if (cmx) delete cmx; 	if (cmy) delete cmy;
		if (Ecmx) delete Ecmx; 	if (Ecmy) delete Ecmy;
		return gr;
	}
	else { 
		cout << "GetMomentEvolution(TH2*,TString ,TString ,TString) Aucun point dans le TGraph" << endl;
		if (cmx) delete cmx; 	if (cmy) delete cmy; return NULL; 
		if (Ecmx) delete Ecmx; 	if (Ecmy) delete Ecmy; return NULL; 
	}
	
}


//-------------------------------------------------
TGraph* KVHistoManipulator::LinkGraphs(TGraph* grx,TGraph* gry){
//-------------------------------------------------
	// A partir de deux graphs ayant le meme nombre de points et le meme axe X,
	// cette methode produit un graph correspondant a la correlation entre les deux axes Y
	// Les inputs peuvent etre aussi bien des TGraph que des TGraphErrors dans ce dernier cas
	// les barres d erreurs sont prises en compte
	
	Double_t* yy = gry->GetY();
	Double_t* xx = grx->GetY();
	Double_t* ey = 0;
	Double_t* ex = 0;
	
	if (grx->GetN()!=gry->GetN()) {
		printf("ERREUR : KVHistoManipulator::LinkGraphs : les deux graphs n ont pas le meme nbre de points\n");
		return 0;
	}
	Int_t npoints = grx->GetN();
	
	TGraph* corre = 0;
	if (grx->InheritsFrom("TGraphErrors") || gry->InheritsFrom("TGraphErrors")){
		if (grx->InheritsFrom("TGraphErrors")) ex = grx->GetEX();
		if (gry->InheritsFrom("TGraphErrors")) ey = gry->GetEY();
		corre = new TGraphErrors(npoints,xx,yy,ex,ey);
	}
	else corre = new TGraph(npoints,xx,yy);
	
	TString name; name.Form("corre_%s_VS_%s",gry->GetName(),grx->GetName()); 
	corre->SetNameTitle(name.Data(),grx->GetTitle());
	
	return corre;

}


//###############################################################################################################
//-------------------------------------------------
Double_t KVHistoManipulator::GetCorrelationFactor(TH2* hh){
//-------------------------------------------------
	// Calcul le coefficient de correlation entre les variables X et Y 
	// d'un bidim... Equivalent a TH2F::GetCorrelationFactor() de ROOT 
Double_t sumxy=0;
Double_t sumx=0,sumx2=0;
Double_t sumy=0,sumy2=0;

Double_t compt=0;
for (Int_t nx=1;nx<=hh->GetNbinsX();nx+=1){
	for (Int_t ny=1;ny<=hh->GetNbinsY();ny+=1){
		compt += hh->GetBinContent(nx,ny);
		sumxy += hh->GetBinContent(nx,ny)*hh->GetXaxis()->GetBinCenter(nx)*hh->GetYaxis()->GetBinCenter(ny);
		sumx  += hh->GetBinContent(nx,ny)*hh->GetXaxis()->GetBinCenter(nx);
		sumy  += hh->GetBinContent(nx,ny)*hh->GetYaxis()->GetBinCenter(ny);
		sumx2 += hh->GetBinContent(nx,ny)*pow(hh->GetXaxis()->GetBinCenter(nx),2.);
		sumy2 += hh->GetBinContent(nx,ny)*pow(hh->GetYaxis()->GetBinCenter(ny),2.);
	}
}

Double_t meanxy = sumxy/compt;
Double_t meanx = sumx/compt;
Double_t meany = sumy/compt;
Double_t meanx2 = sumx2/compt; Double_t sigmax = sqrt(meanx2 - pow(meanx,2.));
Double_t meany2 = sumy2/compt; Double_t sigmay = sqrt(meany2 - pow(meany,2.));

Double_t rho = (meanxy-meanx*meany)/(sigmax*sigmay);
return rho;

}
//###############################################################################################################"
//-------------------------------------------------
KVList* KVHistoManipulator::Give_ProjectionList(TH2* hh,TString axis){
//-------------------------------------------------
	// Retourne une liste contenant toutes les projections par tranche de l'axe (TString axis)
	// si axis="X", les projections sur l'axe Y de l'histogramme est fait pour chaque bin de l'axe X

	if (!hh) { cout << "pointeur histogramme nul" << endl; return NULL; }
	TString proj_name;
	if (hh->InheritsFrom("TH2")){
		KVList *list = new KVList();
		cout << "TH2"<< endl;
		if (axis=="X"){
			for (Int_t nx=1;nx<=hh->GetNbinsX();nx+=1){
				Double_t integ=0;
				for (Int_t ny=1;ny<=hh->GetNbinsY();ny+=1) integ+=hh->GetBinContent(nx,ny);
				proj_name.Form("%s_bX_%d",hh->GetName(),nx);
				list->Add(hh->ProjectionY(proj_name.Data(),nx,nx));	
			}
		}
		else if (axis=="Y"){
			for (Int_t ny=1;ny<=hh->GetNbinsY();ny+=1){
				Double_t integ=0;
				for (Int_t nx=1;nx<=hh->GetNbinsX();nx+=1) integ+=hh->GetBinContent(nx,ny);
				proj_name.Form("%s_bY_%d",hh->GetName(),ny);
				list->Add(hh->ProjectionX(proj_name.Data(),ny,ny));
			}
		}
		else {cout << "l option TString axis doit etre X ou Y" << endl; }	 
		
		return list;
	}
	else {
		cout << "cette methode n est prevue que pour les TH2 and sons" << endl;
		return NULL; 
	}
	
}

//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefinePattern(TH1* ob,TString titleX,TString titleY,TString labelX,TString labelY){
//-------------------------------------------------
	DefinePattern(ob->GetXaxis(),titleX,labelX);
	DefinePattern(ob->GetYaxis(),titleY,labelY);

}
//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefinePattern(TGraph* ob,TString titleX,TString titleY,TString labelX,TString labelY){
//-------------------------------------------------
	DefinePattern(ob->GetXaxis(),titleX,labelX);
	DefinePattern(ob->GetYaxis(),titleY,labelY);

}
//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefinePattern(TF1* ob,TString titleX,TString titleY,TString labelX,TString labelY){
//-------------------------------------------------
	DefinePattern(ob->GetXaxis(),titleX,labelX);
	DefinePattern(ob->GetYaxis(),titleY,labelY);

}
//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefinePattern(TAxis* ax,TString title,TString label){
//-------------------------------------------------
	
	TObjArray *tok = NULL;
	
	if (!title.IsNull()){
		tok = title.Tokenize(" ");
		if (tok->GetEntries()==3){
			ax->SetTitleFont( ((TObjString* )tok->At(0))->GetString().Atoi() );
			ax->SetTitleSize( ((TObjString* )tok->At(1))->GetString().Atof() );
			ax->SetTitleOffset( ((TObjString* )tok->At(2))->GetString().Atof() );
		}
	}

	if (!label.IsNull()){
		tok = label.Tokenize(" ");
		if (tok->GetEntries()==3){
			ax->SetLabelFont( ((TObjString* )tok->At(0))->GetString().Atoi() );
			ax->SetLabelSize( ((TObjString* )tok->At(1))->GetString().Atof() );
			ax->SetLabelOffset( ((TObjString* )tok->At(2))->GetString().Atof() );
		}
	}
	
	if (tok) delete tok;
	
}

//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefineLineStyle(TAttLine* ob,TString line){
//-------------------------------------------------

	TObjArray *tok = NULL;
	if (ob->IsA()->InheritsFrom("TAttLine")){
		if (!line.IsNull()){
			tok = line.Tokenize(" ");
			if (tok->GetEntries()==3){
				ob->SetLineColor( ((TObjString* )tok->At(0))->GetString().Atoi() );
				ob->SetLineStyle( ((TObjString* )tok->At(1))->GetString().Atoi() );
				ob->SetLineWidth( ((TObjString* )tok->At(2))->GetString().Atoi() );
			}	
		}
	}
	if (tok) delete tok;
	
}

//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefineMarkerStyle(TAttMarker* ob,TString marker){
//-------------------------------------------------

	TObjArray *tok = NULL;
	if (ob->IsA()->InheritsFrom("TAttMarker")){
		if (!marker.IsNull()){
			tok = marker.Tokenize(" ");
			if (tok->GetEntries()==3){
				ob->SetMarkerColor( ((TObjString* )tok->At(0))->GetString().Atoi() );
				ob->SetMarkerStyle( ((TObjString* )tok->At(1))->GetString().Atoi() );
				ob->SetMarkerSize( ((TObjString* )tok->At(2))->GetString().Atof() );
			}
		}
	}
	if (tok) delete tok;
	
}

//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefineStyle(TObject* ob,TString line,TString marker){
//-------------------------------------------------

	DefineLineStyle( (TAttLine* )ob,line );
	DefineMarkerStyle( (TAttMarker* )ob,marker);
	
}

//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefineTitle(TH1* ob,TString xtit,TString ytit){
//-------------------------------------------------

	ob->GetXaxis()->SetTitle(xtit);
	ob->GetYaxis()->SetTitle(ytit);
	
}
//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefineTitle(TGraph* ob,TString xtit,TString ytit){
//-------------------------------------------------

	ob->GetXaxis()->SetTitle(xtit);
	ob->GetYaxis()->SetTitle(ytit);
	
}
//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::DefineTitle(TF1* ob,TString xtit,TString ytit){
//-------------------------------------------------

	ob->GetXaxis()->SetTitle(xtit);
	ob->GetYaxis()->SetTitle(ytit);
	
}
//###############################################################################################################"
//-------------------------------------------------
Double_t KVHistoManipulator::GetX(TH1* ob, Double_t val, Double_t eps, Int_t nmax)
{
	// Return value of abscissa X for which the interpolated value
	// of the histogram contents is equal to the given value, val.
	// We use the false position method (which should always converge...)
	// eps is required precision, i.e. convergence condition is that no further change
	// in result greater than eps is found.
	// nmax = maximum number of iterations
	// A solution is searched for X between the limits Xmin and Xmax of the X axis of the histo.
	
	TSpline5* spline = new TSpline5(ob);
	Double_t Xmax = ob->GetXaxis()->GetXmax();
	Double_t Xmin = ob->GetXaxis()->GetXmin();

	 Int_t n,side=0;
    Double_t r,fr,fs,s,ft,t;
	 s = Xmin; t = Xmax;
	 fs = spline->Eval(s) - val;
	 ft = spline->Eval(t) - val;
 
    for (n = 1; n <= nmax; n++)
    {
        r = (fs*t - ft*s) / (fs - ft);
        if (fabs(t-s) < eps*fabs(t+s)) break;
        fr = spline->Eval(r) - val;
 
        if (fr * ft > 0)
        {
          t = r; ft = fr;
          if (side==-1) fs /= 2;
          side = -1;
        }
        else if (fs * fr > 0)
        {
          s = r;  fs = fr;
          if (side==+1) ft /= 2;
          side = +1;
        }
        else break;
    }
	 delete spline;
    return r;
}
//###############################################################################################################"
//-------------------------------------------------
TF1* KVHistoManipulator::RescaleX(TH1* hist1, TH1* hist2, Int_t degree, Double_t* params, Double_t eps)
{
	// Find the polynomial transformation of the X-axis of 1-D histogram hist1
	// so that the distribution ressembles that of histogram hist2.
	// Returns a TF1 which can be used to rescale hist1 (hist1 and hist2 are not modified).
	// User's responsibility to delete the TF1.
	// After fitting, the array params is filled with:
	// params[0] = a0
	// params[1] = a1
	// ...
	// params[degree] = an
	// params[degree+1] = Chisquare/NDF = Chisquare (NDF is always equal to 1)
	// Therefore params MUST BE at least of dimension (degree+2)
	//
	// METHOD
	// ======
	// 'hist1' contains the distribution P1 of variable X1
	// 'hist2' contains the distribution P2 of variable X2
	// Supposing that we can write X2=f_n(X1), with f_n(x)=a0 + a1*x + a2*x^2 + ... + an*x^n,
	// what are the parameters of the polynomial for which P1(f_n(X1))=P2(X2) ?
	//
	// Consider the cumulative distributions, C1(X1) and C2(X2).
	// For degree=1 we compare the 3 X1 & X2 values for which C1=C2=0.1, 0.5, 0.9
	// For degree=2 we compare the 4 X1 & X2 values for which C1=C2=0.1, 0.366..., 0.633..., 0.9
	// etc. etc.
	// In each case we fit the (degree+2) couples (X1,X2) with f_n
	
	register int i;
	// calculate comparison points
	Int_t npoints = degree+2;
	TString func_name = Form("pol%d",degree);
	TF1* fonc = new TF1("f", func_name.Data());
	fonc->SetName(Form("RescaleX-%s",func_name.Data()));
	RescaleX(hist1, hist2, fonc, npoints, eps);
	for(i=0;i<degree+1;i++){
		params[i] = fonc->GetParameter(i);
	}
	Double_t chisquare = fonc->GetChisquare();
	if(fonc->GetNDF()>0.0) chisquare/=fonc->GetNDF();
	params[degree+1] = chisquare;
		
	return fonc;
}
//###############################################################################################################"
//-------------------------------------------------
TH1* KVHistoManipulator::MakeHistoRescaleX(TH1* hist1, TH1* hist2, Int_t degree, Double_t* params,
		Option_t* opt, Double_t eps)
{
	// Uses RescaleX(TH1* hist1, TH1* hist2, Int_t degree, Double_t* params, Double_t eps)
	// to find a polynomial transformation of 'hist1' abscissa so that its
	// distribution resembles that of 'hist2', then generates a new rescaled version of 'hist1'.
	//
	// degree = degree of polynomial to use
	// params = array of dimension [degree+2], after rescaling it will contain
	//          the values of fitted parameters of polynomial, plus the Chi2/NDF of the fit:
	// 	params[0] = a0
	// 	params[1] = a1
	// 	...
	// 	params[degree] = an
	// 	params[degree+1] = Chisquare/NDF = Chisquare (NDF is always equal to 1)
	//
	// eps = relative precision used to find comparison points of histos (default = 1.e-07)
	//
	// OPTIONS
	// =======
	//  opt = "norm" : rescaled histogram normalised to have same integral as hist2
	//  opt = "bins" : rescaled histogram will have same number of bins & same limits as hist2
	//  opt = "normbins" |
	//  opt = "binsnorm" |--> rescaled histogram can be superposed and added to hist2
	//
	// EXAMPLE OF USE
	// ==============
	// In the following example, we fill two histograms with different numbers of random
	// values drawn from two Gaussian distributions with different centroids and widths.
	// We also add to each histogram a 'pedestal' peak which is unrelated to the 'physical'
	// distributions (note that this does not affect the rescaling unless the number of counts
	// in the 'pedestal' is significant compared to the counts in the 'physical' part).
	// Then we generate a rescaled version of the second histogram and superimpose it on
	// the plot with the two initial distributions.
	//
	// void example()
	// {
	// 	TH1F* h1 = new TH1F("h1","gaussian",4096,-.5,4095.5);
	// 	TF1 g1("g1","gaus(0)",0,4100);
	// 	g1.SetParameters(1,1095,233);
	// 	h1->FillRandom("g1",56130);
	// 	h1->SetBinContent(85,179);
	// 	TH1F* h2 = new TH1F("h2","gaussian",4096,-.5,4095.5);
	// 	g1.SetParameters(1,1673,487);
	// 	h2->FillRandom("g1",21370);
	// 	h2->SetBinContent(78,179);
	// 	KVHistoManipulator HM;
	// 	Double_t params[3];
	// 	h1->Draw();
	// 	h2->Draw("same");
	// 	TH1* sc3 = HM.MakeHistoRescaleX(h1,h2,1,params,"binsnorm");
	// 	sc3->SetLineColor(kGreen);
	// 	sc3->Draw("same");
	// }	
	
	TF1* scalefunc = RescaleX(hist1,hist2,degree,params,eps);
	TString options(opt);
	options.ToUpper();
	Bool_t norm = options.Contains("NORM");
	Bool_t bins = options.Contains("BINS");
	Int_t nx = (bins ? hist2->GetNbinsX():-1);
	Double_t xmin = (bins ? hist2->GetXaxis()->GetXmin():-1);
	Double_t xmax = (bins ? hist2->GetXaxis()->GetXmax():-1);
	TH1* scalehisto = ScaleHisto(hist1, scalefunc, 0, nx, -1, xmin, xmax, -1.0, -1.0, "width");
	if(norm) scalehisto->Scale(hist2->Integral("width")/scalehisto->Integral("width"));
	delete scalefunc;
	return scalehisto;
}
//###############################################################################################################"
//-------------------------------------------------
void KVHistoManipulator::RescaleX(TH1* hist1, TH1* hist2, TF1* scale_func, Int_t npoints, Double_t eps)
{
	// Find the transformation of the X-axis of 1-D histogram hist1
	// so that the distribution ressembles that of histogram hist2.
	// The user provides a function f(x) (TF1* scale_func) which is supposed to
	// transform the abscissa X1 of 'hist1' in such a way that P1(f(X1)) = P2(X2).
	// We fit 'npoints' comparison points (see below), npoints>=2.
	//
	// METHOD
	// ======
	// 'hist1' contains the distribution P1 of variable X1
	// 'hist2' contains the distribution P2 of variable X2
	// Supposing that we can write X2=f(X1), we compare the abscissa of different
	// points of the two cumulative distributions, C1(X1) and C2(X2).
	// For npoints=2 we compare the 2 X1 & X2 values for which C1=C2=0.1, 0.9
	// For npoints=3 we compare the 3 X1 & X2 values for which C1=C2=0.1, 0.5, 0.9
	// For npoints=4 we compare the 4 X1 & X2 values for which C1=C2=0.1, 0.366..., 0.633..., 0.9
	// etc. etc.
	// In each case we fit the 'npoints' couples (X1,X2) with the TF1 pointed to by 'scale_func'
	
	register int i;
	npoints = TMath::Max(2,npoints);
	Info("RescaleX","Calculating transformation of histo %s using reference histo %s, %d points of comparison",
			hist1->GetName(), hist2->GetName(), npoints);
	// get cumulated histos
	TH1* cum1 = CumulatedHisto(hist1,"C",-1,-1,"max");
	TH1* cum2 = CumulatedHisto(hist2,"C",-1,-1,"max");
	// calculate comparison points
	Double_t *quantiles = new Double_t[npoints];
	Double_t delta_q = 8.0/(10.0*(npoints-1));
	for(i=0;i<npoints;i++) quantiles[i]=0.1+i*delta_q;
	// get X1 and X2 values corresponding to quantiles
	Double_t *X1 = new Double_t[npoints];
	Double_t *X2 = new Double_t[npoints];
	for(i=0;i<npoints;i++){
		X1[i]=GetX(cum1, quantiles[i], eps);
		X2[i]=GetX(cum2, quantiles[i], eps);
	}
	for(i=0;i<npoints;i++){
		printf("i=%d  quantile=%f  X1=%f  X2=%f\n",
				i, quantiles[i], X1[i], X2[i]);
	}
	// fill TGraph with points to fit
	TGraph* fitgraph=new TGraph(npoints,X1,X2);
	if(fitgraph->Fit(scale_func,"0N")!=0){
		Error("RescaleX","Fitting with function %s failed to converge",
				scale_func->GetName());
	}
	delete cum1;
	delete cum2;
	delete fitgraph;
	delete [] quantiles;
	delete [] X1;
	delete [] X2;
}
//###############################################################################################################"
//-------------------------------------------------
TH1* KVHistoManipulator::MakeHistoRescaleX(TH1* hist1, TH1* hist2, TF1* scale_func, Int_t npoints,
		Option_t* opt, Double_t eps)
{
	// Uses RescaleX(TH1* hist1, TH1* hist2, TF1* scale_func, Int_t npoints, Double_t eps)
	// to transform 'hist1' abscissa using TF1 'scale_func' so that its
	// distribution resembles that of 'hist2', then generates a new rescaled version of 'hist1'.
	//
	// npoints = number of points of comparison between the two histograms.
	//           Make sure this is sufficient for the TF1 used in the transformation.
	//           i.e. for a polynomial of degree 1 (a+bx), 2 points are enough,
	//           3 will give a meaningful Chi^2 value.
	// eps = relative precision used to find comparison points of histos (default = 1.e-07)
	//
	// OPTIONS
	// =======
	//  opt = "norm" : rescaled histogram normalised to have same integral as hist2
	//  opt = "bins" : rescaled histogram will have same number of bins & same limits as hist2
	//  opt = "normbins" |
	//  opt = "binsnorm" |--> rescaled histogram can be superposed and added to hist2
	
	RescaleX(hist1,hist2,scale_func,npoints,eps);
	TString options(opt);
	options.ToUpper();
	Bool_t norm = options.Contains("NORM");
	Bool_t bins = options.Contains("BINS");
	Int_t nx = (bins ? hist2->GetNbinsX():-1);
	Double_t xmin = (bins ? hist2->GetXaxis()->GetXmin():-1);
	Double_t xmax = (bins ? hist2->GetXaxis()->GetXmax():-1);
	TH1* scalehisto = ScaleHisto(hist1, scale_func, 0, nx, -1, xmin, xmax, -1.0, -1.0, "width");
	if(norm) scalehisto->Scale(hist2->Integral("width")/scalehisto->Integral("width"));
	return scalehisto;
}

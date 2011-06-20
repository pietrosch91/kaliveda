/*
$Id: KVCalorimetry.cpp,v 1.4 2009/01/23 15:25:52 franklan Exp $
$Revision: 1.4 $
$Date: 2009/01/23 15:25:52 $
*/

//Created by KVClassFactory on Mon Apr 14 15:01:51 2008
//Author: eric bonnet,,,

#include "KVCalorimetry.h"

ClassImp(KVCalorimetry)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVCalorimetry</h2>
<h4>compute calorimetry</h4>
//Several parameters has to be set in order to allow the calculation
//they correspond to hypotheses of the procedure
//	 SetAsurZ(Double_t fact)								-> Correpond to the ratio between mass and charge of the source					  
//  SetMinimumChargeForFragment(Double_t fact)  	-> Correspond to an eventuel different treatment
//																	between "fragments" and "particles"
//  SetFactorForNeutronsMeanEnergy(Double_t fact)	-> coefficient/factor for the mean neutron energy
//																	used values are mostly 1 or 1/2
//	 SetLevelDensityParameter(Double_t fact)    		-> Mostly used values 8,10, 12
//	 SetFactorForParticles(Double_t fact)  	  		-> In the case there is a distinction between
//																	"fragments" and "particles" and not the whole angular
//																	range of particles are considered (ex 1/2 4pi -> fact=2)
//
//Methode : - resolution du polynome de degré 2 en température
//				 - utilisation de la formule d'un gaz de Fermi (E* = aT2)
//	E* = aT2 = Asource/alevel T2 
// E* + \Delta Qi  = Ek + Mn*T + \Delta Qf --> E* + \Delta Qi - Eksource - Mneutron*T - \Delta Qf
// -> aT2 + \DeltaQi - Eksource - Mneutron*T - \Delta Qf = 0 
//	Variables de sorties : 
//	- Differentes propriétés de la sources reconstruites :
//		-> Température, Energie d'excitation etc ...
//	- Différentes composantes associées aux fragments/particules/neutrons
//		-> Energie cinétique, defaut de masse etc ...	
//	Liste	complete accessible via void PrintParameters()
//
//Utilisation : Fill(KVNucleus* )
//
//Acces aux variables calculees : 	GetValue(Int_t idx) par index
//												GetValue(GetNameIndex("Apart")) par nom

<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

KVCalorimetry::KVCalorimetry(void):KVVarGlob()
	{
//
// Createur par default
//
	init_KVCalorimetry();
	SetName("KVCalorimetry");
	SetTitle("A KVCalorimetry");
	}
	
//_________________________________________________________________
KVCalorimetry::KVCalorimetry(Char_t *nom):KVVarGlob(nom)
	{
//
// Constructeur avec un nom
//
	init_KVCalorimetry();
	}

//_________________________________________________________________
KVCalorimetry::KVCalorimetry(const KVCalorimetry &a):KVVarGlob()
	{
// 
// Contructeur par Copy
//
	init_KVCalorimetry();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVCalorimetry&)a).Copy(*this);
#endif
	}

//_________________________________________________________________
KVCalorimetry::~KVCalorimetry(void)
       {
// 
// Destructeur
//
 
 		delete ingredients;
 
       }

//_________________________________________________________________
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVCalorimetry::Copy(TObject&a) const
#else
void KVCalorimetry::Copy(TObject&a)
#endif
	{
//
// Methode de Copy
//
	KVVarGlob::Copy(a);
	}
	
//_________________________________________________________________
KVCalorimetry& KVCalorimetry::operator = (const KVCalorimetry &a)
	{
//
// Operateur =
//
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
	a.Copy(*this);
#else
	((KVCalorimetry&)a).Copy(*this);
#endif
	return *this;
	}

//_________________________________________________________________
void KVCalorimetry::Reset(void)		
{
// Remise a zero avant le
// traitement d'un evenement
 	for (Int_t nn=0;nn<nbre_ingredients;nn+=1) ingredients[nn]=0;
 	kIsModified=kTRUE;
	kParameterChanged=kTRUE;
}		

//_________________________________________________________________
Double_t KVCalorimetry::getvalue_int(Int_t i)
 {
	// On retourne la ieme valeur du tableau
	// si i est superieur au nbre de variables definies dans ingredient_list
	// retourne la valeur par defaut (ie 0)
	 
	if (i<nbre_ingredients){
		Calculate(-1.,-1.);
		return ingredients[i];
	}
	else return 0;
 }     
				
//________________________________________________________________
void KVCalorimetry::init_KVCalorimetry () 
{
   // PROTECTED method
   // Private initialisation method called by all constructors.
   // All member initialisations should be done here.

   fType = KVVarGlob::kOneBody; // this is a 1-body variable
	
	// Definition de la liste des ingredients rentrant en compte dans 
	// la calorimetrie, les energies et defauts de masse sont exprimees en MeV
	ingredient_list="";
	ingredient_list+="Exci Temp Zsource Asource Eksource Msource Qinitial Qfinal ";
	ingredient_list+="Zfrag Afrag Ekfrag Mfrag Qfrag ";
	ingredient_list+="Zpart Apart Ekpart Mpart Qpart ";
	ingredient_list+="Zneu Aneu Ekneu Mneu Qneu ";
	ingredient_list+="Tmin IsKnown";
	
	// Affiliation d'un index a chaque nom d'ingredients
	nbre_ingredients=0;
	ingredient_list.Begin(" ");
	while (!ingredient_list.End())
		SetNameIndex(ingredient_list.Next(),nbre_ingredients++);
	
	// Mise a Zero du tableau
	ingredients=new Double_t[nbre_ingredients];
	for (Int_t nn=0;nn<nbre_ingredients;nn+=1) ingredients[nn]=0;
	
	kIsModified=kTRUE;
	kParameterChanged=kTRUE;
	parameter_list="";
}

//________________________________________________________________
void KVCalorimetry::Fill (KVNucleus* n) 
{
   // Remplissage des energies, masse,charge et defaut de masse
	// En fonction du critere de la definition des fragments
	// Les contributions des particules sont eventuellements doubles dans SumUp()
	// Suivant le parametre "particle_factor"
	
	kIsModified=kTRUE;
	if (n->GetZ()>=GetParameter("zmin_frag")){
		AddIngValue("Zfrag",n->GetZ());
		AddIngValue("Afrag",n->GetA());
		AddIngValue("Ekfrag",n->GetFrame(fFrame.Data())->GetKE());
		AddIngValue("Qfrag",n->GetMassExcess());
		AddIngValue("Mfrag",1);
	}
	else {
		AddIngValue("Zpart",n->GetZ());
		AddIngValue("Apart",n->GetA());
		AddIngValue("Ekpart",n->GetFrame(fFrame.Data())->GetKE());
		AddIngValue("Qpart",n->GetMassExcess());
		AddIngValue("Mpart",1);
	}

}

//________________________________________________________________
void KVCalorimetry::SumUp() 
{
   // Sum all properties of reconstructed source
	
	if (kIsModified==kTRUE){	
		//printf("reconstruction de la source\n");
		
		// premier calcul depuis le dernier remplissage par Fill
		// Les proprietes de la source sont calculees
		
		// somme des contributions fragments et particules
		SetIngValue("Zsource",GetIngValue("Zfrag")+ GetParameter("particle_factor")*GetIngValue("Zpart"));
		SetIngValue("Asource",GetIngValue("Afrag")+ GetParameter("particle_factor")*GetIngValue("Apart"));
		SetIngValue("Eksource",GetIngValue("Ekfrag")+ GetParameter("particle_factor")*GetIngValue("Ekpart"));
		SetIngValue("Qfinal",GetIngValue("Qfrag")+ GetParameter("particle_factor")*GetIngValue("Qpart"));
		SetIngValue("Msource",GetIngValue("Mfrag")+ GetParameter("particle_factor")*GetIngValue("Mpart"));
		
		// conservation du AsurZ du systeme --> multiplicite moyenne des neutrons
		KVNucleus neutron(0,1);
		Double_t Mneutron = Double_t(TMath::Nint(GetParameter("AsurZ")*GetIngValue("Zsource") - GetIngValue("Asource"))); 	
		SetIngValue("Mneu",Mneutron);
		SetIngValue("Zneu",0);
		SetIngValue("Aneu",GetIngValue("Mneu"));
		SetIngValue("Qneu",GetIngValue("Mneu")*neutron.GetMassExcess());
		
		// prise en compte des neutrons dans la source
		AddIngValue("Asource",GetIngValue("Mneu"));
		AddIngValue("Msource",GetIngValue("Mneu"));
		AddIngValue("Qfinal",GetIngValue("Qneu"));
		
		// defaut de masse de la source reconstruite
		KVNucleus source((Int_t)(GetIngValue("Zsource")),(Int_t)(GetIngValue("Asource")));
		SetIngValue("Qinitial",source.GetMassExcess());
		//parametre additionnel
		//l'exces de masse est t'il tabule (1) ou extrapole (0)
		SetIngValue("IsKnown",source.IsKnown()); 
		
		kIsModified=kFALSE;
		//printf("fin de reconstruction\n");
	}
}

//________________________________________________________________
void 	KVCalorimetry::Calculate(Double_t neutron_factor,Double_t level_parameter)
{
	//Resout le polynome d'ordre 2 en T et remplie les derniers champs
	//Exci, Temp, Eksource ...
	
	SumUp();
	
	// Affilie les parametres donnes en argument
	if (neutron_factor!=-1) 	{SetParameter("neutron_factor",neutron_factor); }
	if (level_parameter!=-1) 	{SetParameter("level_parameter",level_parameter);}
	
	// test si les parametres de la calorimetrie (facteur pour l'energie moyenne des neutrons  et densite de niveaux)
	// ont ete modifies depuis le dernier calcul
	if (kParameterChanged){	
	
	// determination de la temperature
	//	E* = aT2 = Asource/alevel T2 
	// E* + \Delta Qi  = Ek + Mn*T + \Delta Qf --> E* + \Delta Qi - Eksource - Mneutron*T - \Delta Qf
	// -> aT2 + \DeltaQi - Eksource - Mneutron*T - \Delta Qf = 0 
	
		Double_t coefA = GetIngValue("Asource")/GetParameter("level_parameter");
		Double_t coefB = GetParameter("neutron_factor") * GetIngValue("Mneu");
		Double_t coefC = GetIngValue("Qinitial") - GetIngValue("Qfinal") - GetIngValue("Eksource");
		
	// Resolution du polynome de degre 2 
	// Les champs ne sont remplis que si une solution reelle est trouvee	
		if ( RootSquare(coefA,-1.*coefB,coefC) ){
			// la solution max donne la temperature
			SetIngValue("Temp",kracine_max);
			SetIngValue("Exci",coefA*pow(GetIngValue("Temp"),2.));
	
			// ajout de l'energie des neutrons a l energie totale de la source
			SetIngValue("Ekneu",coefB*GetIngValue("Temp"));
			SetIngValue("Eksource",GetIngValue("Eksource")+GetIngValue("Ekneu"));
			
			//parametre additionnel
			SetIngValue("Tmin",kracine_min); // la deuxieme solution de l'eq en T2
			
			kParameterChanged=kFALSE;
		}
		
		//printf("fin du calcul du polynome d'ordre 2 pour la temperature\n");
	}

}

//________________________________________________________________
Bool_t 	KVCalorimetry::RootSquare(Double_t aa,Double_t bb,Double_t cc)
{
	// calcul les racines du polynome d'ordre 2
	// les racines sont accessibles par les variables kracine_min et kracine_max
	//
	// kroot_status>=0 -> tout c'est bien passe	 la fonction retourne kTRUE
	//		=0 2 racines reelles distinctes
	//		=1 2 racines reelles egales (aa==0)
	//			 
	// kroot<0 les deux racines sont mises a zero la fonction retourne kFALSE
	//		=-1 2 racines imaginaires (Delta<0)
	//		=-2 aa=bb=0 
	kracine_max=0,kracine_min=0;
	Double_t x1,x2;
	kroot_status=0;
	if (aa!=0) {
		Double_t Delta=pow(bb,2.)-4.*aa*cc;
		if (Delta<0) {
			//printf("solutions imaginaires dans Fonctions::Racines() %lf %lf %lf\n",aa,bb,cc);
			//cout << "Delta<0 - Solutions imaginaires" << endl; min=-666; max=-666; 
			//printf("%lf %lf %lf\n",a,b,c);
			kroot_status=-1;
		}
		else {
			x1=(-bb+TMath::Sqrt(Delta))/(2.*aa);
			x2=(-bb-TMath::Sqrt(Delta))/(2.*aa);
			kroot_status=0;
			if (x1>x2) 	{ kracine_max=x1; kracine_min=x2; }
			else 			{ kracine_max=x2; kracine_min=x1; }
		}
	}
	else {
		if (bb!=0) { 
			kroot_status=1;
			kracine_max = kracine_min = -cc/bb;
		}
		else {
			kroot_status=-2;
			kracine_max = kracine_min = 0;
		}
	}
	return (kroot_status>=0);

}

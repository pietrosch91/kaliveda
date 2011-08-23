//Created by KVClassFactory on Wed Jun 30 15:10:22 2010
//Author: bonnet

#include "KVSimEvent.h"

ClassImp(KVSimEvent)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVSimEvent</h2>
<h4>Classe dérivée de KVEvent pour la gestion d'événements issus de simulations</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////
//La classe dérive également de KVNameValueList pour pouvoir ajouter autant
// de parametres que possible afin d'enregistrer des propriétés de l 'événement
// propre à chaque simulation
//
//Cette classe est couplée à KVSimNucleus

//___________________________
KVSimEvent::KVSimEvent(Int_t mult, const char*classname):KVEvent(mult,classname)
{
   // Default constructor
	init();
	//CustomStreamer(); 
}

//___________________________
void KVSimEvent::Streamer(TBuffer & R__b)
{
   //Stream an object of class KVSimEvent.
   //When reading an event, first Clear() is called (this calls the Clear()
   //method of all the particles in the previous event).

   if (R__b.IsReading()) {
      Clear();
      R__b.ReadClassBuffer(KVSimEvent::Class(), this);
   } else {
      R__b.WriteClassBuffer(KVSimEvent::Class(), this);
   }
}

//___________________________
KVSimEvent::~KVSimEvent()
{
	//destructeur
}

//___________________________
void KVSimEvent::init()
{
	//protected method
	//to initialize variables
	
}

//___________________________
void KVSimEvent::Copy(TObject& obj) const
{
	//Copy l'object "this" vers obj

	KVEvent::Copy(obj);
	
	//Appel direct de KVNameValueList::Copy()
	//provoquait un segmentation fault ...
	//on recopie peu ou prou
	//cette methode, pour que ca marche
	
	khl->Copy(*((KVSimEvent &)obj).GetList());
	((KVSimEvent &)obj).GetList()->SetName(khl->GetName());
}

//___________________________
void KVSimEvent::Clear(Option_t * opt)
{
	//Clear des variables héritées de KVEvent
	//La liste des parametres est effacées
	KVEvent::Clear(opt);
	Clear_NVL(opt);
}

//___________________________
void KVSimEvent::Print(Option_t* opt) const
{
	//print des informations relatives à l objet

	KVEvent::Print(opt);
	Print_NVL(opt);

}

// $Id: GTScalers.cpp,v 1.2 2007/09/17 12:33:27 franklan Exp $
// Author: $Author: franklan $
/***************************************************************************
//                        GTGanilData.cpp  -  Main Header to ROOTGAnilTape
//                             -------------------
//    begin                : Thu Jun 14 2001
//    copyright            : (C) 2001 by Garp
//    email                : patois@ganil.fr
//////////////////////////////////////////////////////////////////////////
//
// Part of ROOTGanilTape 
//
// GTScalers
//
// Scaler class for scaler events.
//
//////////////////////////////////////////////////////////////////////////
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// CVS Log:
// ---------------------------------------------------------------------------

#include "Riostream.h"

//extern "C"
//{
#include <GEN_TYPE.H>
#include <gan_acq_buf.h>

// Problem (for me?) in gan_acq_buf :
// What the use of defining struct inside other structs ?
// Then, theyr definition remain local and I have to do it again here:
  typedef struct SCALE
  {
    UNSINT32 Length; /* Nb bytes utils suivant ce mot */
    UNSINT32 Nb_channel; 
    UNSINT32 Acq_status;
    UNSINT32 Reserve[2];
    union JBUS_SCALE
    {
      scale_struct UnScale[NB_MAX_CHANNEL];
      UNSINT16 Info_jbus  [NB_MAX_JBUS];
    } jbus_scale;
  } scale;
//}

#include <TMath.h>
#include "GTScalers.H"


ClassImp(GTScalers);

//______________________________________________________________________________
GTScalers::GTScalers(void)
{
  fNbChannel=-1; // Initialisation Marker
  fScalerArray=NULL;  
}

//______________________________________________________________________________
GTScalers::~GTScalers(void)
{
  // Delete 
  delete[] fScalerArray;
}


//______________________________________________________________________________
void GTScalers::Fill(scale *s)
{
  cout<<"GOT A SCALER EVENT !!!"<<endl;

  cout <<s->Length<<endl;
  cout <<s->Nb_channel<<endl; // This is number of actual scalers
  cout <<s->Acq_status<<endl;
  
  if (fNbChannel==-1) {// First filling
    fNbChannel   = s->Nb_channel;
    fScalerArray = new GTOneScaler[fNbChannel];
  } 
  else if (fNbChannel!=s->Nb_channel) {
    cout <<"Dont know how to deal with variable number of scalers!"<<endl;
    cout <<fNbChannel<<" versus "<<s->Nb_channel<<endl;
    return;
  }
 
  for (Int_t i=0;i<fNbChannel;i++)
    fScalerArray[i].Set(&(s->jbus_scale.UnScale[i]));
}

//______________________________________________________________________________
void GTScalers::DumpScalers(void)
{
  // Dump scalers value on cout

  cout <<"Dumping "<<fNbChannel<<" scalers"<<endl;
  for (Int_t i=0;i<fNbChannel;i++)
  {
    cout <<"--------- "<<i<<" -------------"<<endl;
    fScalerArray[i].Dump();
  }
}

//______________________________________________________________________________
const GTOneScaler* GTScalers::GetScalerPtr (Int_t index) const
{
  // Return a constant pointer to the Scaler Index
  if (index<0||index>=fNbChannel) {
    cout << "Scaler index out of range. Got: "<<index
	 <<" Limits are: ["<<0<<","<<fNbChannel-1<<"]"<<endl;
    return(NULL);
  }
  return(&(fScalerArray[index]));
}

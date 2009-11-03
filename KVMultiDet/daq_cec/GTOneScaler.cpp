// $Id: GTOneScaler.cpp,v 1.2 2007/09/17 12:33:27 franklan Exp $
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
// GTOneScaler
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

//extern "C"
//{
#include <GEN_TYPE.H>
#include <gan_acq_buf.h>
//}
#include "GTOneScaler.H"

ClassImp(GTOneScaler);

//______________________________________________________________________________
GTOneScaler::GTOneScaler(void)
{
  // Default constructor. dont do anything
}


//______________________________________________________________________________
GTOneScaler::GTOneScaler(scale_struct *s)
{
  Set(s);
}

//______________________________________________________________________________
void GTOneScaler::Set(scale_struct *s)
{
  fLabel  =s->Label;
  fStatus =s->Status;
  fCount  =s->Count;
  fFreq   =s->Freq;
  fTics   =s->Tics;
  for (int i=0;i<3;i++) fReserve[i]=s->Reserve[i];
}


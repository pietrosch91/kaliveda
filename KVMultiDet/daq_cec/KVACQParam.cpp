/***************************************************************************
                          kvacqparam.cpp  -  description
                             -------------------
    begin                : Wed Nov 20 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVACQParam.cpp,v 1.23 2007/12/06 15:12:54 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Riostream.h"
#include "KVACQParam.h"
#include "KVDetector.h"
#include "TRandom.h"

ClassImp(KVACQParam)
///////////////////////////////////////////////////////////////////////////////
//16-bit data acquisition parameters read from raw data.
//
//This class provides a link between the values of the parameters for each event as read
//from a raw data file, and the detectors. Each detector has a KVACQParam object for each
//parameter concerning it written in the raw data file (KVDetector::SetACQParams is
//responsible for setting up a list of the relevant KVACQParams for each instance).
//
//GetCoderData() returns the 16-bit integer value of the acquisition parameter
//as read from the file. "-1" means that the parameter was not present in the event.
//
//GetData() takes the value of GetCoderData() and adds a random number in the range
//[-0.5,+0.5] to give continuous floating-point values."-1" means that the parameter was
//not present in the event.
//
//Fired() returns true or false depending on if the parameter was 'fired' (i.e. if GetCoderData is > -1)
//
//GetPedestal() returns the current pedestal value associated to this parameter
///////////////////////////////////////////////////////////////////////////////

void KVACQParam::init()
{
//Default initialisations
   fChannel = 0;
   fData = (Short_t) - 1;
   fDet = 0;
   fPied = 0.;
   fWorks = kTRUE;
}

//_________________________________________________________________________
KVACQParam::KVACQParam()
{
   //default constructor
   init();
}

//_________________________________________________________________________
KVACQParam::KVACQParam(const Char_t * parname)
{
   //constructor with name
   //'parname' = parameter name associated with this object
   init();
   SetName(parname);
}

//
KVACQParam::KVACQParam(const KVACQParam & obj)
{
   //Copy ctor
   init();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   obj.Copy(*this);
#else
   ((KVACQParam &) obj).Copy(*this);
#endif
}

////////////////////////////////////////////////////////////////////////////
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
void KVACQParam::Copy(TObject & obj) const
#else
void KVACQParam::Copy(TObject & obj)
#endif
{
   //
   //Copy this to obj
   //
   KVBase::Copy(obj);
   ((KVACQParam &) obj).SetData(GetCoderData());
   if (GetDetector())
      ((KVACQParam &) obj).SetDetector(GetDetector());
}

void KVACQParam::Print(Option_t * opt) const
{
   cout << "_________________________________________" << endl;
   cout << " KVACQParam: " << GetName() << " " << GetType() << endl;
   if (GetDetector())
      cout << " Detector: " << GetDetector()->GetName() << endl;
   cout << " Data = " << GetCoderData() << endl;
   cout << "_________________________________________" << endl;
}

void KVACQParam::ls(Option_t * option) const
{
   //Dump name of parameter, raw coder value, and "randomised" value
   cout << ClassName() << " : " << GetName() << " raw=" << GetCoderData()
       << " randomised=" << GetData() << endl;
}

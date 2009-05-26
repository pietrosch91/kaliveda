/***************************************************************************
$Id: KVCalibrationMaker5.h,v 1.4 2003/11/08 09:56:11 franklan Exp $
                          kvcalibrationmaker5.h  -  description
                             -------------------
    begin                : mer mar 26 2003
    copyright            : (C) 2003 by Alexis Mignon
    email                : mignon@ganil.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef KV_CALIBRATION_MAKER5_H
#define KV_CALIBRATION_MAKER5_H

#define KVCM5_THORON_1 6.06
#define KVCM5_THORON_2 8.78

#include "KVINDRADB5.h"
#include "KVINDRA5.h"

class KVCalibrationMaker5 : public TNamed {
  protected :
  KVDataBase5* fDataBase;
  KVINDRA5 * fIndra;

  Bool_t fDBBuild;
  
  public:
  KVCalibrationMaker();
  KVCalibrationMaker(KVINDRA5* indra);
  virtual KVCalibrationMaker();
  
  void MakeAlphaCalibrations();

  ClassDef(KVCalibrationMaker5,0) //To make calibrations
};


#endif

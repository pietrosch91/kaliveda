/***************************************************************************
                          KVIDZGrid.h  -  description
                             -------------------
    begin                : Nov 24 2004
    copyright            : (C) 2004 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVIDZGrid.h,v 1.12 2009/03/02 15:43:13 franklan Exp $
***************************************************************************/

#ifndef KVIDZGrid_H
#define KVIDZGrid_H

#include "KVIDGrid.h"

class KVIDZLine;

class KVIDZGrid:public KVIDGrid {

 public:

   KVIDZGrid();
   KVIDZGrid(const KVIDZGrid &);
   virtual ~ KVIDZGrid();
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,4,0)
   virtual void Copy(TObject &) const;
#else
   virtual void Copy(TObject &);
#endif

   ClassDef(KVIDZGrid, 2)       //Base class for 2D Z identification grids (E - dE method)
};

#endif

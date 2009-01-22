/***************************************************************************
                          KVTGIDZ.cpp  -  description
                             -------------------
    begin                : 5 July 2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVTGIDZ.cpp,v 1.6 2006/10/19 14:32:43 franklan Exp $
***************************************************************************/
#include "KVTGIDZ.h"
#include "KVIDZGrid.h"
#include "KVIDZLine.h"

ClassImp(KVTGIDZ)
//____________________________________________________________________________//
//KVTGIDZ
//
//Abstract base class for charged particle Z identfication using functionals
//developed by L. Tassan-Got (IPN Orsay)
//
//A KVIDZGrid identification grid can be generated from the functional in order
//to visualise the corresponding identification lines.
//
//The following methods are defined in this class:
//
//      SetIdent() -- sets the Z for each KVIDZLine
//      AddLine()  -- adds a KVIDZLine to the grid
//      NewGrid()  -- generates a new KVIDZGrid
//
///////////////////////////////////////////////////////////////////////////////
    KVTGIDZ::KVTGIDZ(const Char_t * name,
                     const Char_t * function,
                     Double_t xmin, Double_t xmax, Int_t npar, Int_t x_par,
                     Int_t y_par)
:KVTGID(name, function, xmin, xmax, npar, x_par, y_par)
{
   //Create Z identification with given name,
   //using a KVTGIDFunctions namespace function (e.g. "tassangot_Z"),
   //for 'x' values from xmin to xmax, npar parameters, and defining
   //the parameter indices corresponding to 'x' and 'y' coordinates.
}

//___________________________________________________________________________//

void KVTGIDZ::SetIdent(KVIDLine * line, Double_t Z)
{
   //Set atomic number Z of identification line (KVIDZLine)
   dynamic_cast < KVIDZLine * >(line)->SetZ((Int_t) Z);
}

//___________________________________________________________________________//

KVIDLine *KVTGIDZ::AddLine(KVIDGrid * g)
{
   //Add a new KVIDZLine to the grid
   return g->AddLine("ID", "KVIDZLine");
}

//___________________________________________________________________________//

KVIDGrid *KVTGIDZ::NewGrid()
{
   //Create new KVIDZGrid
   return new KVIDZGrid;
}

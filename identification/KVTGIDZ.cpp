/***************************************************************************
                          KVTGIDZ.cpp  -  description
                             -------------------
    begin                : 5 July 2005
    copyright            : (C) 2005 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVTGIDZ.cpp,v 1.7 2009/03/03 13:36:00 franklan Exp $
***************************************************************************/
#include "KVTGIDZ.h"
#include "KVIDZAGrid.h"
#include "KVIDZALine.h"

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
   //Set atomic number Z of identification line
   dynamic_cast < KVIDZALine * >(line)->SetZ((Int_t) Z);
}

//___________________________________________________________________________//

KVIDLine *KVTGIDZ::AddLine(KVIDGrid * g)
{
   //Add a new KVIDZALine to the grid
   return (KVIDLine*)g->Add("ID", "KVIDZALine");
}

//___________________________________________________________________________//

KVIDGrid *KVTGIDZ::NewGrid()
{
   //Create new KVIDZAGrid which is only to be used for Z identification
   KVIDZAGrid*gr = new KVIDZAGrid;
   gr->SetOnlyZId();
   return gr;
}

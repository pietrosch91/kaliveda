// $Id: KVParameterList.cpp,v 1.4 2007/10/24 10:30:40 franklan Exp $

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Manages a list of named parameters, either integers (KVParameterList<int>), floating point
//(KVParameterList<double>) or strings (KVParameterList<TString>).
//
//To add a parameter to the list, use:
//
//      SetParameter(name, value)
//
//If a parameter with the given name already exists, this will change its value.
//
//To retrieve the value of a parameter, use:
//
//      GetParameter(name)
//
//To test if a parameter with a given name exists, use
//
//      HasParameter(name)
//
//To remove a parameter from the list, use
//
//      RemoveParameter(name)
//
//To remove all parameters from the list, use
//
//      Clear()
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "KVParameterList.h"

templateClassImp(KVParameterList)

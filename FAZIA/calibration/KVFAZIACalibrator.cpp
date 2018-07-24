//Created by KVClassFactory on Thu Dec  3 15:59:09 2015
//Author: bonnet,,,

#include "KVFAZIACalibrator.h"
using namespace std;

ClassImp(KVFAZIACalibrator)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVFAZIACalibrator</h2>
<h4>Basic class for FAZIA calibrators</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

UInt_t KVFAZIACalibrator::fNCalibFunc = 0;

//___________________________________________________________________________
KVFAZIACalibrator::KVFAZIACalibrator()
{
   // Default constructor
   fFunc = 0;
}

//___________________________________________________________________________
KVFAZIACalibrator::KVFAZIACalibrator(const Char_t* name, const Char_t* type) : KVCalibrator(name, type, 1)
{
   // Default constructor
   fFunc = 0;
}

//___________________________________________________________________________
KVFAZIACalibrator::~KVFAZIACalibrator()
{
   // Destructor
   if (fFunc)
      delete fFunc;
}

void KVFAZIACalibrator::SetParameter(UShort_t i, Float_t par_val)
{
   fFunc->SetParameter(i, par_val);
}

//___________________________________________________________________________
void KVFAZIACalibrator::SetFunction(TF1* f1)
{
   if (fFunc)
      delete fFunc;
   fFunc = (TF1*)f1->Clone(Form("calibfunc_%d", fNCalibFunc++));
   SetNumberParams(fFunc->GetNpar());
}

//___________________________________________________________________________
void KVFAZIACalibrator::SetFunction(TString formula)
{
   if (fFunc)
      delete fFunc;
   fFunc = new TF1(Form("calibfunc_%d", fNCalibFunc++), formula.Data(), 0, 1);
   SetNumberParams(fFunc->GetNpar());
}

//___________________________________________________________________________
void KVFAZIACalibrator::ChangeParameters(Double_t* val)
{
   if (!fFunc)
      return;
   for (Int_t nn = 0; nn < fFunc->GetNpar(); nn += 1) {
      fFunc->SetParameter(nn, val[nn]);
   }
   SetStatus(kTRUE);

}
//___________________________________________________________________________
Double_t KVFAZIACalibrator::Compute(Double_t chan) const
{
   //Calculate the calibrated energy.
   //Returns 0 if calibrator is not ready (parameters not set)

   if (GetStatus()) {
      return fFunc->Eval(chan);
   }
   return 0;
}


//___________________________________________________________________________
Double_t KVFAZIACalibrator::operator()(Double_t chan)
{
   //Overloading of "()" to allow syntax such as:
   //
   //        KVFAZIACalibrator calibrator;
   //           ....
   //        Float_t calibrated_energy = calibrator(channel);
   //
   //equivalently to:
   //
   //        Float_t calibrated_energy = calibrator.Compute(channel);
   return Compute(chan);
}

//___________________________________________________________________________
Double_t KVFAZIACalibrator::Invert(Double_t energy)
{
   //Given the calibrated (or simulated) energy in MeV,
   //calculate the corresponding channel number according to the
   //calibration parameters (useful for filtering simulations).

   if (GetStatus()) {
      return fFunc->GetX(energy);
   }
   return 0;
}

//___________________________________________________________________________
void KVFAZIACalibrator::Print(Option_t*) const
{
   //Print a description of the calibration object, including a list of its parameters
   std::cout << "_________________________________________________" << std::endl
             << "KVCalibrator :" << std::endl
             << "  Name : " << GetName() << std::endl
             << "  Type : " << GetType() << std::endl
             << "  Formula : " << GetFunction()->GetExpFormula() << std::endl
             << "  Number of Parameters : " << GetFunction()->GetNpar() << std::endl
             << "  Parameters :" << std::endl;
   for (UShort_t i = 0; i < GetFunction()->GetNpar(); i++) {
      std::cout << "    " << GetFunction()->GetParName(i) << ": " << GetFunction()->GetParameter(i) << std::endl;
   }
   if (GetStatus())
      std::cout << "  Status : ready" << std::endl;
   else
      std::cout << "  Status : not ready" << std::endl;
}

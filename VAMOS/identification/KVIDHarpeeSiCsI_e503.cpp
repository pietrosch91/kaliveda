#include "KVIDHarpeeSiCsI_e503.h"

ClassImp(KVIDHarpeeSiCsI_e503)

KVIDHarpeeSiCsI_e503::KVIDHarpeeSiCsI_e503() :
#if __cplusplus < 201103L
   records_(NULL),
   base_id_result_(NULL),
   minimiser_(NULL),
#else
   records_(nullptr),
   base_id_result_(nullptr),
   minimiser_(nullptr),
#endif
   kInitialised_(kFALSE)
{
   // Constructor (Public Interface Method)
   //
   // It is good practice to keep the constructor clear of initialisation code
   // as there is no easy way for constructors to handle errors. Instead you
   // should call Init() immediately after you construct this object.

   fIDCode = kIDCode3;
}

KVIDHarpeeSiCsI_e503::~KVIDHarpeeSiCsI_e503()
{
#if __cplusplus < 201103L
   if (minimiser_) {
      delete minimiser_;
      minimiser_ = NULL;
   }

   if (base_id_result_) {
      delete base_id_result_;
      base_id_result_ = NULL;
   }

   if (records_) {
      delete records_;
      records_ = NULL;
   }
#endif
}

void KVIDHarpeeSiCsI_e503::Copy(TObject& obj) const
{
   // Call parent class copy method (takes care of the inherited stuff)
   KVIDHarpeeSiCsI::Copy(obj);

   // Then copy any desired member variables (not copied by the parent method)
   //KVIDHarpeeSiCsI_e503 *casted_obj = static_cast<KVIDHarpeeSiCsI_e503*>(&obj);
}

Bool_t KVIDHarpeeSiCsI_e503::Init()
{
   // Init (Public Interface Method)
   //
   // You should call this method immediately after you have constructed the
   // KVIDHarpeeSiCsI_e503 object. If the telescope has already been
   // initialised this method will do nothing.
   //

   if (kInitialised_) return kTRUE;

#if __cplusplus < 201103L
   base_id_result_ = new KVIdentificationResult();
   minimiser_ = new SiliconEnergyMinimiser();
#else
   base_id_result_.reset(new KVIdentificationResult());
   minimiser_.reset(new SiliconEnergyMinimiser());
#endif

   kInitialised_ = kTRUE;

   return kTRUE;
}

Bool_t KVIDHarpeeSiCsI_e503::Identify(KVIdentificationResult* idr, Double_t x,
                                      Double_t y)
{
   // Identify : Public Interface Method
   //
   // This method relies on the base class identification routine
   // (KVIDHarpeeSiCsI::Identify) to perform the Z identification, using a
   // KVIDZAGrid. It then estimates the A value using the ThreadedMassEstimator
   // class (by minimising the difference between simulated and real silicon
   // energies).
   //

   assert(idr);

   // Check to make sure the KVIDHarpeeSiCsI_e503 object has been properly
   // initialised. If not, we attempt to initialise it.

   if (!kInitialised_) Init();

   if (x < 0.) x = GetIDMapX();
   if (y < 0.) y = GetIDMapY();

   if (x == 0.) return kFALSE;
   if (y == 0.) return kFALSE;

   idr->IDOK = kFALSE;
   idr->IDattempted = kTRUE;

   // Base class performs the preliminary identification routine. We do this
   // after the initialisation check as we require base_id_result_.
   assert(base_id_result_);
#if __cplusplus < 201103L
   KVIDHarpeeSiCsI::Identify(base_id_result_, x, y);
#else
   KVIDHarpeeSiCsI::Identify(base_id_result_.get(), x, y);
#endif

   // Set the idcode and type for this telescope
   idr->IDcode = fIDCode;
   idr->SetIDType(GetType());

   if (!base_id_result_->Zident) {
      idr->IDquality = kBaseIdentZFailed;
      return kFALSE;
   }

   idr->Z = base_id_result_->Z;
   idr->PID = base_id_result_->PID;

   assert((idr->Z > 0) && (idr->Z < 120));
   assert(idr->PID > 0.);

   // At this point Z should be well identified by the base class

   minimiser_->SetIDTelescope(GetName());
   idr->A = minimiser_->Minimise(idr->Z, y, x);

   if (idr->A > 0) {
      idr->Zident = kTRUE;
      idr->Aident = kTRUE;

      idr->IDOK = kTRUE;
      idr->IDquality = kIdentified;
      return kTRUE;
   }

   return kFALSE;

}

Bool_t KVIDHarpeeSiCsI_e503::SetIDCorrectionParameters(
   const KVRList* const records
)
{
   // SetIDCorrectionParameters (Public Interface Method)
   //
   // This method adds all the identification correction parameter records
   // specified in 'records' to the private member KVRList 'records_';
   //

   if (!records) {
      Error("SetIDCorrectionParameters",
            "Supplied record list is a null pointer");
      return kFALSE;
   }

#if __cplusplus < 201103L
   if (records_) {
      delete records_;
      records_ = NULL;
   }

   records_ = new KVList(kFALSE);
#else
   records_.reset(new KVList(kFALSE));
#endif

   records_->AddAll(records);
   return kTRUE;
}

const KVList* KVIDHarpeeSiCsI_e503::GetIDCorrectionParameters() const
{
#if __cplusplus < 201103L
   return records_;
#else
   return records_.get();
#endif
}

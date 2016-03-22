#include "KV3DGeoTrack.h"

#include "TPad.h"
#include "TView.h"
#include "KVNucleus.h"
#include "TPolyMarker3D.h"

ClassImp(KV3DGeoTrack)

KV3DGeoTrack::KV3DGeoTrack():
   TPolyLine3D(),
   fIndex(0),
   fTrack(nullptr),
   fNuc()
{
}

KV3DGeoTrack::KV3DGeoTrack(TVirtualGeoTrack* tr):
   TPolyLine3D(tr->GetNpoints()),
   fIndex(0),
   fTrack(tr),
   fNuc(tr->GetName())
{
   SetLineColor(Color());
   for (int n = 0; n < tr->GetNpoints(); n++) {
      const Double_t* point = tr->GetPoint(n);
      SetPoint(n, point[0], point[1], point[2]);
   }
}

KV3DGeoTrack::~KV3DGeoTrack()
{
}

Int_t KV3DGeoTrack::DistancetoPrimitive(Int_t px, Int_t py)
{
   return TPolyLine3D::DistancetoPrimitive(px, py);
}

void KV3DGeoTrack::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{

   gPad->SetCursor(kCross);
   fTrack->ExecuteEvent(event, px, py);
}

const Text_t* KV3DGeoTrack::GetName() const
{
   return fTrack->GetName();
}

Text_t* KV3DGeoTrack::GetObjectInfo(Int_t px, Int_t py) const
{
   return fTrack->GetObjectInfo(px, py);
}

void KV3DGeoTrack::Draw(Option_t* option)
{
   // Override Draw to add a TPolyMarker3D at the end of the track
   TPolyMarker3D* pm = new TPolyMarker3D(1, 4);
   Float_t* coords = &(GetP()[3 * GetLastPoint()]);
   pm->SetPoint(0, coords[0], coords[1], coords[2]);
   pm->SetMarkerColor(GetLineColor());
   pm->SetMarkerSize(0.5);
   pm->Draw(option);
   pm->SetName(GetName());
   TPolyLine3D::Draw(option);
}


Int_t KV3DGeoTrack::Color()
{
   SetLineWidth(2);
   Int_t fZ = GetZ();
   if (fZ == 0) return kBlue;
   else if (fZ == 1) return kRed;
   if (fZ == 2) return kGreen;
   SetLineWidth(4);
   if (fZ < 10) return kMagenta;
   if (fZ < 20) return kCyan;
   if (fZ < 30) return kOrange;
   SetLineWidth(6);
   return kGray;
}


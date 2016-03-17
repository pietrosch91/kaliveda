#ifndef KV3DGeoTrack_H
#define KV3DGeoTrack_H

#include "TPolyLine3D.h"
class TVirtualGeoTrack;

class KV3DGeoTrack : public TPolyLine3D {

protected:

   Int_t Color(TObject*);
   Int_t          fIndex;
   TVirtualGeoTrack* fTrack;

public:
   KV3DGeoTrack();
   KV3DGeoTrack(TVirtualGeoTrack*);
   virtual ~KV3DGeoTrack();
   virtual Int_t         DistancetoPrimitive(Int_t px, Int_t py);
   virtual void          ExecuteEvent(Int_t event, Int_t px, Int_t py);
   virtual const Text_t* GetName() const;
   virtual Text_t*       GetObjectInfo(Int_t px, Int_t py) const;
   void Draw(Option_t* option = "");

   ClassDef(KV3DGeoTrack, 1) //Class to visualize tracks in OpenGL viewer
};
#endif






/***************************************************************************
                          KVASMultiDetArray.h  -  description
                             -------------------
    begin                : Thu May 16 2002
    copyright            : (C) 2002 by J.D. Frankland
    email                : frankland@ganil.fr

$Id: KVASMultiDetArray.h,v 1.55 2009/03/03 14:27:15 franklan Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KVASMultiDetArray_H
#define KVASMultiDetArray_H

#include "KVMultiDetArray.h"
#include "KVLayer.h"
class KVRing;

class KVASMultiDetArray : public KVMultiDetArray {

protected:
    UInt_t fCurrentLayerNumber; //! used to number layers

    KVSeqCollection *fLayers;             //-> list of layers in array sorted by layer number

    void SetGroups(KVLayer *, KVLayer *);
    void UpdateGroupsInRings(KVRing * r1, KVRing * r2);
    void MakeListOfDetectors();
public:
    KVASMultiDetArray();
    virtual ~ KVASMultiDetArray();
    void init();

    void AddLayer();
    void AddLayer(KVLayer * kvl);
    void CalculateGroupsFromGeometry();
    KVSeqCollection *GetLayers() const {
        return fLayers;
    };
    KVLayer *GetLayer(const Char_t * name) const {
        //find layer with name
        return (KVLayer *) fLayers->FindObject(name);
    }
    KVLayer *GetLayer(UInt_t num) const {
        //find layer with number
        return (KVLayer *) fLayers->FindObjectByNumber(num);
    }
    KVTelescope *GetTelescope(const Char_t *name) const;
    KVRing *GetRing(const Char_t * layer, const Char_t * ring_name) const;
    KVRing *GetRing(const Char_t * layer, UInt_t ring_number) const;
    KVRing *GetRing(UInt_t layer, const Char_t * ring_name) const;
    KVRing *GetRing(UInt_t layer, UInt_t ring_number) const;
    void RemoveRing(const Char_t * layer, const Char_t * ring_name);
    void RemoveRing(const Char_t * layer, UInt_t ring_number);
    void RemoveRing(UInt_t layer, const Char_t * ring_name);
    void RemoveRing(UInt_t layer, UInt_t ring_number);
    void RemoveRing(KVRing * ring);
    void RemoveLayer(KVLayer *, Bool_t kDeleteLay = kTRUE);
    void RemoveLayer(const Char_t *);
    void RemoveLayer(UInt_t);
    void ReplaceTelescope(const Char_t *name, KVTelescope *new_kvt);

    virtual Double_t GetSolidAngleByLayerAndRing(const Char_t* layer,UInt_t ring_number) {
        // return the solid angle (msr) for a given KVRing conditioned by the chosen KVLayer
        return GetRing(layer,ring_number)->GetSolidAngle();
    }
    virtual Double_t GetSolidAngleByLayer(const Char_t* layer) {
        // return the solid angle (msr) of all the KVRing conditioned by the chosen KVLayer
        Double_t sol_ang=0;
        KVRing *cou; TIter nxtcou(GetLayer(layer)->GetRings()); while ((cou = (KVRing *) nxtcou())) sol_ang+=cou->GetSolidAngle();
        return sol_ang;
    }

    virtual void Print(Option_t * opt = "") const;
    TGeoManager *CreateGeoManager(Double_t dx, Double_t dy, Double_t dz);

    ClassDef(KVASMultiDetArray, 1) //Azimuthally-symmetric multidetector arrays
};

#endif

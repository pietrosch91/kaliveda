//Created by KVClassFactory on Fri Apr 26 12:45:15 2013
//Author: John Frankland,,,

#include "KVDetectorNode.h"
#include "KVDetector.h"
#include "KVUniqueNameList.h"

ClassImp(KVDetectorNode)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVDetectorNode</h2>
<h4>Stores lists of detectors in front and behind this node</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVDetectorNode::init()
{
    fInFront=0;
    fBehind=0;
}

KVDetectorNode::KVDetectorNode()
{
   // Default constructor
    init();
}

//________________________________________________________________

KVDetectorNode::KVDetectorNode(const Char_t* name) : KVBase(name, "Detector node")
{
   // Write your code here
    init();
}

KVDetectorNode::~KVDetectorNode()
{
   // Destructor
    SafeDelete(fInFront);
    SafeDelete(fBehind);
}

void KVDetectorNode::ls(Option_t *option) const
{
    std::cout << "Detector Node " << GetName() << std::endl;
    if(fInFront){
        std::cout << "In front:" << std::endl;
        fInFront->ls();
    }
    if(fBehind){
        std::cout << "Behind:" << std::endl;
        fBehind->ls();
    }
}

void KVDetectorNode::AddInFront(KVDetector* d)
{
    if(!fInFront) fInFront = new KVUniqueNameList;
    fInFront->Add(d);
}

void KVDetectorNode::AddBehind(KVDetector* d)
{
    if(!fBehind) fBehind = new KVUniqueNameList;
    fBehind->Add(d);
}
Bool_t KVDetectorNode::IsInFrontOf(KVDetector* d)
{
    // return true if this node is directly in front of the detector
    return (fBehind->FindObject(d)!=0);
}
Bool_t KVDetectorNode::IsBehind(KVDetector* d)
{
    // return true if this node is directly behind the detector
    return (fInFront->FindObject(d)!=0);
}

//Created by KVClassFactory on Wed Jun  9 11:30:26 2010
//Author: John Frankland,,,

#include "KVIdentificationResult.h"

ClassImp(KVIdentificationResult)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVIdentificationResult</h2>
<h4>Full result of one attempted particle identification</h4>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

void KVIdentificationResult::Copy(TObject& obj) const
{
	// Copy this to obj
	KVBase::Copy(obj);
	KVIdentificationResult& id = (KVIdentificationResult&)obj;
	id.IDattempted = IDattempted;
	id.IDOK = IDOK;
	id.IDcode = IDcode;
	id.Zident = Zident;
	id.Aident = Aident;
	id.IDquality = IDquality;
	id.Z = Z;
	id.A = A;
	id.PID = PID;
}

void KVIdentificationResult::Reset()
{
	// Reset to initial values
	SetIDType("");
	SetComment("");
	IDattempted = kFALSE;
	IDOK = kFALSE;
	IDcode = -1;
	Zident = kFALSE;
	Aident = kFALSE;
	IDquality = -1;
	Z = -1;
	A = -1;
	PID = -1.0;
}

void KVIdentificationResult::Print(Option_t* opt) const
{
	printf("Identification #%d  -  Type:%s ", GetNumber(), GetIDType());
	if(!IDattempted) {
		printf("   => not attempted\n\n");
		return;
	}
	if(IDOK) printf("   => SUCCESS\n");
	else printf("   => FAILURE\n");
	printf("  Quality code = %d (%s)\n", IDquality, GetLabel());
	if(Zident) printf("  Z identified = %d", Z);
	if(Aident) printf("    A identified = %d", A);
	if(Zident||Aident) printf("    PID = %f\n", PID);
}

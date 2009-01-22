/*
BuildINDRAE475S.C
*/

#include "KVMultiDetArray.h"

void BuildINDRAE475S()
{
	//Modify INDRA_camp4 multidetector so that it corresponds to E475S experiment:
	//	- no 1st ring
	//	- no ChIo after ring 12
	//	- all silicons on rings 6&7 are 150um thick
	
	KVMultiDetArray::MakeMultiDetector("INDRA_camp4");
	
	gMultiDetArray->RemoveRing("SI-CSI", 1);
	gMultiDetArray->RemoveRing("CHIO", 13);
	gMultiDetArray->RemoveRing("CHIO", 14);
	gMultiDetArray->RemoveRing("CHIO", 16);
	for(int ring=6; ring<8; ring++){
		gMultiDetArray->GetRing("SI-CSI", ring)->GetTelescopes()->ForEach(KVTelescope, SetDetectorTypeThickness)("SI", 150.);
	}
	gMultiDetArray->UpdateArray();
	gMultiDetArray->SetPedestals("/net/ganp737/space/eindra/e475s/analyse/piedestaux/piedestaux_chiosi.dat");
	gMultiDetArray->SetPedestals("/net/ganp737/space/eindra/e475s/analyse/piedestaux/piedestaux_csi.dat");
}

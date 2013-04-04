//Created by KVClassFactory on Fri Mar 29 08:56:52 2013
//Author: Guilain ADEMARD

#include "KVVAMOSTransferMatrix.h"
#include "KVUnits.h"
#include "KVVAMOS.h"

ClassImp(KVVAMOSTransferMatrix)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>KVVAMOSTransferMatrix</h2>
<h4>VAMOS's transformation matrix to map the measured coordinates at the focal plane back to the target</h4>
<!-- */
// --> END_HTML
//
// For each transmitted trajectory the parameters that are to be reconstructed
// are expressed as a set of four independent functions:
//
//     Brho  = F1( Xf, Yf, Thetaf, Phif)
//     Theta = F2( Xf, Yf, Thetaf, Phif)
//     Phi   = F3( Xf, Yf, Thetaf, Phif)
//     path  = F4( Xf, Yf, Thetaf, Phif)
//
// The four functions "F" are a polynomial of four variables (Xf, Yf, Thetaf, Phif)
// which have to be stored in a KVVAMOSReconTrajectory object.
// As a example, the expression for Brho is:
//Begin_Latex( fontsize=22 )
//    B_{#rho} = #sum_{i,j,k,l=0}^{i+j+k+l=n} C_{ijkl} X_{f}^{i} Y_{f}^{j} #theta_{f}^{k} #phi_{f}^{l}
//
//End_Latex
// where n is the order of the polynomial and Begin_Latex( fontsize=22 ) C_{ijkl} End_Latex are
// the coefficients related to the properties of the experimental system.
////////////////////////////////////////////////////////////////////////////////

void KVVAMOSTransferMatrix::init(){
	//Initializing of data members
		fOrder = 0;
		fCoef  = NULL;
}
//________________________________________________________________

KVVAMOSTransferMatrix::KVVAMOSTransferMatrix( Bool_t scan_ds ) : KVBase("VAMOSTransferMatrix","Transfer Matrix of VAMOS"){
   // Default constructor
   init();
   if( scan_ds ) ReadCoefInDataSet();
}
//________________________________________________________________

KVVAMOSTransferMatrix::KVVAMOSTransferMatrix(UChar_t order) : KVBase("VAMOSTransferMatrix","Transfer Matrix of VAMOS"){
   // Default constructor
   init();
   fOrder = order;
   NewMatrix();
}
//________________________________________________________________

KVVAMOSTransferMatrix::KVVAMOSTransferMatrix (const KVVAMOSTransferMatrix& obj)  : KVBase("VAMOSTransferMatrix","Transfer Matrix of VAMOS"){
   	// Copy constructor
   	// This ctor is used to make a copy of an existing object 
   	init();
   	obj.Copy(*this);
}
//________________________________________________________________

KVVAMOSTransferMatrix::~KVVAMOSTransferMatrix(){
   	// Destructor
   	if( fCoef ) {
		DeleteMatrix();
	}
}
//________________________________________________________________

void KVVAMOSTransferMatrix::Copy (TObject& obj) const{
   	// This method copies the current state of 'this' KVVAMOSTransferMatrix 
   	// object into 'obj'.

    KVBase::Copy(obj);
   	KVVAMOSTransferMatrix& CastedObj = (KVVAMOSTransferMatrix&)obj;
   	CastedObj.fOrder = fOrder;
	CastedObj.NewMatrix();
	if( fOrder ){
		for(Int_t p=0; p<4; p++){//loop over parameters (Brho, theta, phi, path)
			for(Int_t i=0; i<=fOrder; i++){// loop over the power of Xf
				for(Int_t j=0; j<=fOrder; j++){// loop over the power of Yf
					for(Int_t k=0; k<=fOrder; k++){// loop over the power of Thetaf
						for(Int_t l=0; l<=fOrder; l++){// loop over the power of Phif
							CastedObj.fCoef[p][i][j][k][l] = fCoef[p][i][j][k][l];
 						}
					}
				}
			}
		}
	}
}
//________________________________________________________________

void KVVAMOSTransferMatrix::NewMatrix(){
	//Assignes dynamic memory to the array used to store the coefficients
	// of this matrix. The size of this array depends on the ordrer of the
	// matrix.
	//
	if(  fCoef  ) DeleteMatrix();
	if( !fOrder ) return;

	fCoef = new Double_t****[4];
	for(Int_t p=0; p<4; p++){//loop over parameters (Brho, theta, phi, path)
		fCoef[p] = new Double_t***[ fOrder+1 ];

		for(Int_t i=0; i<=fOrder; i++){// loop over the power of Xf
			fCoef[p][i] = new Double_t**[ fOrder+1 ];

			for(Int_t j=0; j<=fOrder; j++){// loop over the power of Yf
				fCoef[p][i][j] = new Double_t*[ fOrder+1 ];

				for(Int_t k=0; k<=fOrder; k++){// loop over the power of Thetaf
					fCoef[p][i][j][k] = new Double_t[ fOrder+1 ];

					// loop over the power of Phif
					for(Int_t l=0; l<=fOrder; l++) fCoef[p][i][j][k][l] = 0;
				}
			}
		}
	}
}
//________________________________________________________________

void KVVAMOSTransferMatrix::DeleteMatrix(){
	//Deletes memory allocated for the array used to store the coefficients
	//of this matrix.

	for(Int_t p=0; p<4; p++){//loop over parameters (Brho, theta, phi, path)
		fCoef[p] = new Double_t***[ fOrder+1 ];

		for(Int_t i=0; i<=fOrder; i++){// loop over the power of Xf
			fCoef[p][i] = new Double_t**[ fOrder+1 ];

			for(Int_t j=0; j<=fOrder; j++){// loop over the power of Yf
				fCoef[p][i][j] = new Double_t*[ fOrder+1 ];

				for(Int_t k=0; k<=fOrder; k++){// loop over the power of Thetaf
					delete[] fCoef[p][i][j][k];
				}
				delete[] fCoef[p][i][j];
			}
			delete[] fCoef[p][i];
		}
		delete[] fCoef[p];
	}
	delete[] fCoef;
	fCoef = NULL;
}
//________________________________________________________________

void KVVAMOSTransferMatrix::ReadCoefInDataSet(){
	//Read the coefficients in files listed in another file set in the 
	//dataset directory:
	//$KVROOT/KVFiles/<name_of_dataset>
	// 
	// This last file is found and open from its name given by the following
	// environment variable defined in $KVROOT/KVFiles/.kvrootrc:
	//
	// KVVAMOSTransferMatrix.CoefficientFile:     VAMOStransMatrixCoef.list
	//
	// A file name specific to a given dataset may also be defined:
	//
	// INDRA_e494s.KVVAMOSTransferMatrix.CoefficientFile:   E494StransMatrixCoef.list
	// 
	// where 'INDRA_e494s' is the name of the dataset in question.
	//
	// In this file, the order has to be given by:
	//
	// ORDER: 7
	//
	// To write the name of the file associated to a given parameter PARAM =  DELTA (Brho), THETA, PHI, PATH
	// use the line:
	//
	// PARAM: filename.dat
	//
	// The method ReadCoefficients(...) is called for each listed file.
	
	TEnv infos;
	TString path( gDataSet->GetDataSetDir() );
	path += "/";
	TString envname = Form("%s.CoefficientFile",ClassName());
	path += gDataSet->GetDataSetEnv( envname.Data(), "VAMOStransMatrixCoef.list");
//	Info("ReadCoefInDataSet"," reading file %s", path.Data() );

	infos.ReadFile(path.Data(),kEnvAll);

	// by default the order is seven
	SetOrder( infos.GetValue("ORDER", 7 ) );

	ifstream ifile; 
	TString fname;

	fname = infos.GetValue("DELTA","BrhoReconCoef.cal");
	if( gDataSet->OpenDataSetFile( fname.Data(), ifile ) ){ 
		Info("ReadCoefInDataSet","Reading DELTA Coefficients in %s",fname.Data());
		ReadCoefficients( kBrho, ifile );
		ifile.close();
	}

	fname = infos.GetValue("THETA","ThetaReconCoef.cal");
	if( gDataSet->OpenDataSetFile( fname.Data(), ifile ) ){ 
		Info("ReadCoefInDataSet","Reading THETA Coefficients in %s",fname.Data());
		ReadCoefficients( kTheta, ifile );
		ifile.close();
	}

	fname = infos.GetValue("PHI","PhiReconCoef.cal");
	if( gDataSet->OpenDataSetFile( fname.Data(), ifile ) ){ 
		Info("ReadCoefInDataSet","Reading PHI Coefficients in %s",fname.Data());
		ReadCoefficients( kPhi, ifile );
		ifile.close();
	}

	fname = infos.GetValue("PATH","PhiReconCoef.cal");
	if( gDataSet->OpenDataSetFile( fname.Data(), ifile ) ){ 
		Info("ReadCoefInDataSet","Reading PATH Coefficients in %s",fname.Data());
		ReadCoefficients( kPath, ifile );
		ifile.close();
	}
}
//________________________________________________________________

void KVVAMOSTransferMatrix::ReadCoefficients( EReconParam par, ifstream &file ){
	// Reads the value of coefficients Begin_Latex( fontsize=22 ) C_{ijkl} End_Latex used to reconstruct the parameter 'par' in the file 'file'.
	// This method read the files given by Maurycy Rejmund obtained by using
	// the ZGOUBI code (F. Meot, Nucl. Instr. and Meth. A 427 (1999) 353. 

	UChar_t nn[]={0,0,0,0,0};
	Int_t coef_num = 0;
	//skip the first line of the file which does not contain any coef.
	file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	RecursiveReadOfCoef( par, file, fOrder, nn, coef_num );
}
//________________________________________________________________

void KVVAMOSTransferMatrix::RecursiveReadOfCoef( EReconParam par, ifstream &file, UChar_t order, UChar_t *n, Int_t &coef_num, Int_t prev_i ){
	// Method used by ReadCoefficients(...) to read coefficients in the file
	// having a specific format which is different of the one used to store the
	// coefficients in the array fCoef. 
	//
	// Inputs:  par   - parameter associated to the coefficents to be read
	//                (kBrho, kTheta, kPhi, kPath)
	//          file  - file to be read
	//          order - function order in the recursive process 
	//          n     - array of counters [5] to count the power of each
	//                  parameter (Xf, Yf, ThetaF, PhiF) i.e. to find the
	//                  index i,j,k,l for the coefficient Begin_Latex( fontsize=22 ) C_{ijkl} End_Latex 
	//                  For the first call of this method, each element of 'n'
	//                  has to be equal zero.
	//       coef_num - used to count the number of coefficients read. 
	//                  To be equal to 0 for the first call. 
	//       prev_i   - indice of the parameter at the previous order in the
	//                  recursive process. To be equal to 0 for the first call. 

	order--;

	// loop over : 1, Xf, Yf, ThetaF, PhiF
	for(Int_t i=prev_i; i<5; i++){

		n[i]++;
		if( order ) RecursiveReadOfCoef( par, file, order, n, coef_num, i );
		else{

			file >> fCoef[ par ][ n[1] ][ n[2] ][ n[3] ][ n[4] ];
//			Info("RecursiveReadOfCoef","%d: Coef(%d,%d,%d,%d) = %f",coef_num,n[1],n[2],n[3],n[4], fCoef[ par ][ n[1] ][ n[2] ][ n[3] ][ n[4] ]);

			coef_num++;
		}
		n[i]--;
	}
}
//________________________________________________________________

void KVVAMOSTransferMatrix::Reset(){
	// Resets the coefficients (set to zero)

	for(Int_t p=0; p<4; p++){//loop over parameters (Brho, theta, phi, path)
		for(Int_t i=0; i<=fOrder; i++){// loop over the power of Xf
			for(Int_t j=0; j<=fOrder; j++){// loop over the power of Yf
				for(Int_t k=0; k<=fOrder; k++){// loop over the power of Thetaf
					// loop over the power of Phif
					for(Int_t l=0; l<=fOrder; l++) fCoef[p][i][j][k][l] = 0;
				}
			}
		}
	}
}
//________________________________________________________________

Bool_t KVVAMOSTransferMatrix::ReconstructFPtoLab( KVVAMOSReconTrajectory *traj){
	// Reconstruction of the trajectory at the target point, in the reference
	// frame of the laboratory, from the trajectory at the focal plane.
	// Then in the object 'traj' the focal plane parameters have to be given
	// i.e. traj->FPparamsAreReady() has to return true.
	//
	// the result is store in the 'traj' object and the method returns true
	// if the attempt is a success.

	if( !traj->FPparamsAreReady() ){
		Error("ReconstructFPtoLab","Focal plane position parameters are not ready to reconstruct the trajectory");
		return kFALSE;
	}
	if( !IsReady() ){
		Error("ReconstructFPtoLab","Not ready, the coefficients are missing");
		return kFALSE;
	}
	Double_t Xf, Yf, Tf, Pf, Xfp, Yfp, Tfp, Pfp;

	Double_t prod, prodP;
	Double_t Brhot,Thetat,Phit,Patht;

	Brhot=Thetat=Phit = Patht = 0.;
		
	Xf = -1. *traj->pointFP[0]/KVUnits::m; // in m
	Yf =  0.;
	Tf = -1. * traj->GetThetaF()*TMath::DegToRad(); // in rad
	Pf =  0.;

	Xfp = Xf;
	Yfp = -1. *traj->pointFP[1]/KVUnits::m; // in m
	Tfp = Tf;
	Pfp = -1. * TMath::ATan( TMath::Tan( traj->GetPhiF()*TMath::DegToRad() )*TMath::Cos( Tfp )); // in rad

	for(Int_t i=0; i<=fOrder; i++) // loop over power of Xf
		for(Int_t j=0; j<=fOrder-i; j++) // loop over power of Yf
			for(Int_t k=0; k<=fOrder-i-j; k++) // loop over power of ThetaF 
				for(Int_t l=0; l<=fOrder-i-j-k; l++){ // loop over power ofPhiF 

					prod = TMath::Power( Xf, i )
						* TMath::Power( Yf, j )
						* TMath::Power( Tf, k )
						* TMath::Power( Pf, k );
					
					Brhot  += fCoef[kBrho ][i][j][k][l] * prod; // in Tm
					Thetat += fCoef[kTheta][i][j][k][l] * prod; // in mrad
					Patht  += fCoef[kPath ][i][j][k][l] * prod; // in cm

					prodP = TMath::Power( Xfp, i )
						* TMath::Power( Yfp, j )
						* TMath::Power( Tfp, k )
						* TMath::Power( Pfp, k );

					Phit   += fCoef[kPhi][i][j][k][l] * prodP; // in mrad
				}

	traj->path = Patht;
	traj->Brho  = Brhot * gVamos->GetBrhoRef();

	Double_t theta = - Thetat/1000;  // in rad
	Double_t phi   = - Phit  /1000;  // in rad
	Double_t X     = TMath::Sin( theta ) * TMath::Cos( phi );
	Double_t Y     = TMath::Sin( phi   );
	Double_t Z     = TMath::Cos( theta ) * TMath::Cos( phi );

	traj->dirLab.SetXYZ( X, Y, Z );
	traj->dirLab.RotateY( gVamos->GetAngle()*TMath::DegToRad() );

	traj->SetFPtoLabAttempted();

	return kTRUE;
}
//________________________________________________________________

Bool_t KVVAMOSTransferMatrix::ReconstructLabToFP( KVVAMOSReconTrajectory *traj){
	// Reconstruction of the trajectory at the focal plan from the trajectory
	// at the target point in the reference frame of the laboratory.
	// Then in the object 'traj' the laboratory parameters have to be given
	// i.e. traj->LabParamsAreReady() has to return true.
	//
	// the result is store in the 'traj' object and the method returns true
	// if the attempt is a success.
	//
	// METHOD TO BE IMPLEMENTED 

	traj = traj;
	Warning("ReconstructLabToFP","To be implemented");
	return kFALSE;
}
//________________________________________________________________

void KVVAMOSTransferMatrix::SetCoef( EReconParam par, Int_t i, Int_t j, Int_t k, Int_t l, Double_t coef){
	// Sets the value of the coefficient Begin_Latex( fontsize=22 ) C_{ijkl} End_Latex used to reconstruct the parameter 'par'.
	//
	// par = kBrho, kTheta, kPhi, kPath

if( fCoef && i<=fOrder && j<=fOrder&& k<=fOrder&& l<=fOrder )
	fCoef[ par ][ i ][ j ][ k ][ l ] = coef;
}
//________________________________________________________________

void KVVAMOSTransferMatrix::Print(Option_t * option) const{
	//Prints informations about this object.
	//Use the option="coef" if you want to print the coefficients for each
	//parameters
	//
	cout <<ClassName()<<" object: Name=" << GetName() << " Title=" << GetTitle();
   	if ( strcmp(GetLabel(), "") )
      	cout << " Label=" << GetLabel();
   	if (GetNumber() != 0)
      	cout << " Number=" << GetNumber();
   	cout<<" Order= "<<(Int_t)GetOrder();
   	cout << endl;

   	if( strcmp( option, "coef" ) || !fOrder ) return;


   	TString par;
	Short_t Nnull, Nnonull;

   	for(Int_t p=0; p<4; p++){
	   	switch (p){
		   	case kBrho:
			   	par= "Brho";
			   	break;
		   	case kTheta:
			   	par= "Theta";
			   	break;
		   	case kPhi:
			   	par= "Phi";
			   	break;
		   	case kPath:
			   	par= "Path";
			   	break;

	   	}
	   	cout<<endl<<setw(8)<<par.Data()<<": "<<endl;
		Nnull = Nnonull = 0;

   	   	for(Int_t i=0; i<=fOrder; i++){
		   	for(Int_t j=0; j<=fOrder-i; j++){
			   	for(Int_t k=0; k<=fOrder-i-j; k++){
	   			   	for(Int_t l=0; l<=fOrder-i-j-k; l++){
						if( fCoef[p][i][j][k][l] == 0 ) Nnull++;
						else{
					   		cout << setw(9)<<" "<<Form("Coef(%d,%d,%d,%d) = %f", i,j,k,l, fCoef[p][i][j][k][l])<<endl;
							Nnonull++;
						}
   	   	   	   	   	}
   	   	   	   	}
   	   	   	}
   	   	}
		cout << setw(9)<<" "<<"with "<<Nnull<<" coefficients are null and "<<Nnonull<<" are not null"<<endl; 
		cout<<endl;
   	}
}

//Created by KVClassFactory on Thu Jun 24 14:22:17 2010
//Author: John Frankland,,,

#include "RKex1.h"
#include "TMath.h"
#include "Riostream.h"
#include "TROOT.h"
#include "TVector3.h"
#include "TRandom.h"

ClassImp(RKex1)

////////////////////////////////////////////////////////////////////////////////
// BEGIN_HTML <!--
/* -->
<h2>RKex1</h2>
<h4>Simulation of microcanonical equilibrium using class KVRungeKutta</h4>
<p>
This class uses <a href="../KVRungeKutta.html">KVRungeKutta</a> in order to
simulate the dynamical evolution of N identical particles of fixed total
kinetic energy enclosed in a sphere.
</p>
<p>
Example of use:<br>
RKex1 gas(5, 100, 20, 1);<br>
gas.Run(0,10000,0.1,1);<br>
</p>
<p>
This will generate a ROOT file RKex1_N5_B20.000000_S1.000000_E100.00000.root containing
a TTree "Events" containing the time evolution of the total energy and
of the kinetic energy, speed, and distance from the origin of each particle.
</p>
<!-- */
// --> END_HTML
////////////////////////////////////////////////////////////////////////////////

RKex1::RKex1(Int_t N, Double_t e0, Double_t b, Double_t s)
	: KVRungeKutta(6*N)
{
   // Default constructor
   // N = number of particles
   // e0 = total energy of particles
   // b = radius of spherical container for the 'gas'
   // s = radius of each spherical particle
   
   Nparts = N;
   coords = new Double_t [6*N];
   boxradius = b;
   sphereradius = s;
	xsec = 4.*pow(sphereradius,2.);
   Time = 0;
   TheFile = new TFile(Form("RKex1_N%d_B%f_S%f_E%f.root",N,b,s,e0), "recreate");
   TheTree = new TTree("Events", Form("RKex1 events N=%d E=%f box=%f sphere=%f",N,e0,b,s));
   TheTree->Branch("Time", &Time);
   TheTree->Branch("Etot", &Etot);
   TheTree->Branch("CollisionRate", &collRate);
   TheEnergies = new Double_t [N];
   TheDistances = new Double_t [N];
   TheVelocities = new Double_t [N];
   TheTree->Branch("Energies", TheEnergies, Form("Energies[%d]/D",N));
   TheTree->Branch("Velocities", TheVelocities, Form("Velocities[%d]/D",N));
   TheTree->Branch("Distances", TheDistances, Form("Distances[%d]/D",N));
   for(int i=0;i<N;i++) TheTree->SetAlias(Form("E%d",i+1),Form("Energies[%d]",i));      
   SetRandomCoords(e0);
}

RKex1::~RKex1()
{
   // Destructor
   delete [] coords;
   delete [] TheEnergies;
   delete [] TheVelocities;
   delete [] TheDistances;
}

Double_t RKex1::GetEKin() const
{
   // Total kinetic energy of gas
	Double_t ekin = 0.0;
	for(register int i=0; i<Nparts; i++){
		ekin += GetE(i);
	}
	return ekin;
}

Double_t RKex1::GetE(Int_t i) const
{
   // Kinetic energy of particle i(=0,...,N-1)
	Double_t ekin = 0.0;
	for(register int j=3*i; j<3*i+3; j++){
		ekin += 0.5*coords[j]*coords[j];
	}
	return ekin;
}

Double_t RKex1::GetR(Int_t i) const
{
   // Distance from origin of particle i(=0,...,N-1)
	Double_t ray = 0.0;
	for(register int j=3*i; j<3*i+3; j++){
		ray += pow(coords[3*Nparts+j],2.);
	}
	return sqrt(ray);
}

Double_t RKex1::GetV(Int_t i) const
{
   // Speed of particle i(=0,...,N-1)
	Double_t ray = 0.0;
	for(register int j=3*i; j<3*i+3; j++){
		ray += pow(coords[j],2.);
	}
	return sqrt(ray);
}

void RKex1::CalcDerivs(Double_t /*xx*/, Double_t* yy, Double_t* dyy)
{
   // Calculation of derivatives required by KVRungeKutta
	for(register int i=0; i<3*Nparts; i++){
		dyy[i] = 0.0; //--> no forces = no acceleration
		dyy[3*Nparts+i] = yy[i]; //--> constant velocities
	}
}

Int_t RKex1::CollisionDetection()
{
   // Detect & perform collisions between particles of gas.
   // Two particles undergo a collision when their separation
   // becomes smaller than the particle radius.
   // Returns the number of collisions which occured.
   
	Double_t x12, X[3];
   Int_t coll=0;
	for(register int i=0; i<3*Nparts-3; i+=3){
		for(register int j=i+3; j<3*Nparts; j+=3){			
			x12 = 0.;
			for(register int k=0;k<3;k++){
				X[k] =  coords[3*Nparts+j+k]-coords[3*Nparts+i+k];
				x12 += pow(X[k],2.);
			}
			if( x12 < xsec ) {coll+=Collision(i,j,X);}
		}
	}
   return coll;
}

void RKex1::CheckBoundaries()
{
   // Check for particles hitting the container walls.
   // Any such particles are reflected back into the bulk.
   
	for(register int i=0; i<Nparts; i++){
		if(GetR(i)>=boxradius-sphereradius){
			TVector3 r(&coords[3*Nparts+3*i]);
			TVector3 v(&coords[3*i]);
			TVector3 Unorm = r.Unit();
         // calculate velocity component along radial vector
			Double_t proj = v*Unorm;
         if(proj>0.){
            // velocity is outbound i.e. will increase R
            // we reverse this component (= reflection)
			   TVector3 vp = v - (2.*proj)*Unorm;
			   for(register int j=0;j<3;j++){
				   coords[3*i+j] = vp(j);
			   }
         }
		}
	}
}
Int_t RKex1::Collision(Int_t xi, Int_t xj, Double_t *_dx)
{
	// Particles xi & xj are close enough to be in collision (see CollisionDetection).
	// _dx[3] is relative displacement vector
   // If their relative velocity is reducing their displacement, we invert
   // the component of this velocity along the relative displacement vector,
   // which corresponds to an elastic collision between the two spheres.
   // If the collision takes place, we return 1, otherwise 0.
	
	TVector3 r_ij(_dx);
	TVector3 vi(&coords[xi]);
	TVector3 vj(&coords[xj]);
	TVector3 v_ij = vj - vi;
   TVector3 unit_r_ij = r_ij.Unit();
   Double_t dot_product = unit_r_ij*v_ij;
   if(dot_product<0.){
      v_ij = v_ij - 2.*dot_product*unit_r_ij;
      TVector3 vcm = 0.5*(vi+vj);
      TVector3 Vi = vcm-0.5*v_ij;
      TVector3 Vj = vcm+0.5*v_ij;
	   for(register int i=0;i<3;i++){
		   coords[xi+i] = Vi(i);
		   coords[xj+i] = Vj(i);
	   }
      return 1;
   }
   return 0;
}

void RKex1::SetRandomCoords(Double_t E0)
{
	// Initialise particles with random positions inside the spherical
   // container and random velocities with a total kinetic energy = E0
	
	Etot0 = E0;
	Double_t SR3 = pow(0.9*(boxradius-sphereradius),3.);
	for(register int i=0; i<3*Nparts; i+=3){
		Double_t r = pow(gRandom->Uniform(SR3), 1./3.);
		gRandom->Sphere(coords[i+3*Nparts],coords[i+3*Nparts+1],coords[i+3*Nparts+2],r);
		gRandom->Sphere(coords[i],coords[i+1],coords[i+2],1.);
	}
	Double_t norm = sqrt(Etot0/GetEKin());
	for(register int i=0; i<3*Nparts; i+=3){
		for(register int j=0;j<3;j++){
			coords[i+j]*=norm;
		}
	}
	Print();
   
}

void RKex1::Run(Double_t tmin, Double_t tmax, Double_t dt_coll, Double_t dt_write)
{
   // Perform the simulation from time 'tmin' to 'tmax'.
   // dt_coll = interval between collision & boundary condition checks
   // dt_write = interval for storing results in TTree
   //
   // After the end of the simulation, the ROOT file containing the TTree
   // is written to disk and closed.
   
	Int_t nsteps = (tmax-tmin)/dt_coll+1;
	Time = tmin;
   Double_t nextWrite = dt_write;
	
   collRate = 0.0;
   
   WriteResults();
   
	for(register int i=0; i<nsteps; i++){
      
      // take Runge-Kutta step from Time to Time+dt_coll
		Integrate(coords, Time, Time+dt_coll, dt_coll);
      
		Time+=dt_coll;
      
		collRate += CollisionDetection();
		CheckBoundaries();
      
      if(Time>nextWrite){
         collRate/=dt_write;
         WriteResults();
         nextWrite+=dt_write;
         collRate=0.0;
      }
	}
   
   TheFile->Write();
   TheFile->Close();
}
	
void RKex1::Print(Option_t* /*opt*/) const
{
	cout << "Configuration with " << Nparts << " particles at TIME = " << Time << endl;
	Double_t ekin = GetEKin();
	cout << "  EKIN = " << ekin << "  ETOT = " << ekin << endl;
}

void RKex1::WriteResults()
{
   // Store current state of gas particles in the TTree.
   
   Etot = 0;
	for(register int i=0;i<Nparts;i++){
		TheEnergies[i] = GetE(i);
		TheVelocities[i] = GetV(i);
		TheDistances[i] = GetR(i);
		Etot += TheEnergies[i];
	}
   TheTree->Fill();
}

//
//
//	James General Functions version 1.0
// 	27/02/09		09/02/2015
//	james.smallcombe@outlook.com
//
//
#ifndef jamesfunc_h
#define jamesfunc_h

#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

#include <TObjArray.h>
#include <TMath.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TRotation.h>
#include <TH2.h>
#include <TH1.h>
#include <TGraph.h>
#include <TF2.h>

#include <TRandom3.h>

using namespace std;


//Returns mass excess in MeV or zero if undefined [Inputs(Z,N)]
double get_mass_excess(int,int);

  //////////////////////////////////////
 ////////// Useful Constants ////////// 
//////////////////////////////////////

const double jam_phys_amu=931.494061; // MeV/c^2   /A
const double jam_phys_amu_kg=1.66053892e-27; // kg   /A
const double jam_phys_speed_c=299792458; // m/s
const double jam_phys_speed_c_mm_ns=299.792458; // mm/ns
const double jam_phys_e4pieps=1.4399644895; // MeV fm 
const double jam_phys_charge=1.60217657e-19; // C

void swap_jd(double &,double &);

/////////////////////////////////////////////////////
/////////// Trig functions for lazy typing   ////////
/////////////////////////////////////////////////////

// ALL ANGLES IN RADIANS

double Acos(double);
double cos2(double);

double Asin(double);
double sin2(double);

double Atan(double);
double tan2(double);

const double pi=TMath::Pi();

  /////////////////////////////////////////////
 /////////// Angle Difference calcs   ////////
/////////////////////////////////////////////

// Give the difference between two angles in a consistent way
// Avoids +- problems and going over 2PI
// [Inputs (Angle 1(radians),Angle 2(radians))]

// Return abs angle magnitude
double angledifference(double,double);
// Return signed angle
double angledifference_signed(double,double);

double happy_phi(double);
double happy_phi(TVector3&);

///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/////////// Functions for converting detector data to hit info   /////////// 
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
/////////// MWPC functions   /////////// 
////////////////////////////////////////

// Returns hit vector relative to detector centre. Rejects doubles
// [Inputs(Valid hit return (empty bool&), Detector edge array, X1, X2, Y)]
// Array = [X1_min,X1_max,X2_min,X2_max,Y_min,Y_max]
TVector3 make_det_position_new(bool&,int*,Int_t,Int_t,Int_t);

// Returns hit vector relative to detector centre. Rejects doubles
// [Inputs(Valid hit return (empty bool&), Detector last_wire array, X, Y)]
// Array = [X_min,X_max,Y_min,Y_max]
TVector3 make_short_position_new(bool&,int*,Int_t,Int_t);

// Returns TRUE if data is within valid range (which is marginally larger than limits)
// [Inputs(Data, min, max)]
bool make_limit_position(int,int,int);


/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// "silicon_hit" An object to construct and hold data on dE/E detector hit combos   ////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
class silicon_hit
{
    public:
	    
	////////////////////////////////////////
	/////////// Data members    ///////////
	///////////////////////////////////////

	int mult;	//multiplicity of the combinations

	// Vectors are "mult" long containing hits constructed from all valid dE-E combinations 
	
	vector< TVector3 > localvector; // Pixel centre in detector frame
	vector< TVector3 > worldhit;    // Pixel centre in world frame must call "setworld" fn
	vector< double > phi,r; 	// Pixel centre phi and r
	// Note: ROOT uses a stupid definition of phi this class ignores, but the vectors are fine
	// This Phi runs from 0 at 12 O'clock clockwise to 2*PI
	
	// These are randomised across pixed for graphical use
	vector< TVector3 > locald;
	vector< TVector3 > worldd;
	vector< double > phid,rd;

	vector< double > timeE,timedE;  //DAQ time E and dE
	vector< double > E,dE;		//DAQ data E and dE
	vector< int > pixr,pixphi;	//pixel numbers
	vector< double > EdEt;		//dE adn E detector timestamp difference

	///////////////////////////////////////////////////
	/////////// Constructor & Destructor    ///////////
	//////////////////////////////////////////////////	

	// constructor
	// [Inputs(DAQ_data <vec>,DAQ_timestamp <vec>)]
	// <vec>={EA_1,EA_2....EA_16,EB_1....EB_16,dE1...dE12}
	silicon_hit(vector< double >& ,vector< double > &);
	silicon_hit();
	// destructor
	~silicon_hit();   
	 
	//////////////////////////////////
	/////////// Methods    ///////////
	//////////////////////////////////

	// Given the rotation and vector to place the detector will correct worldd and worldhit and theta
	// [Inputs(Detector_zero_world, Local_Rotations)]
	void setworld(TVector3,TRotation = TRotation());

    private:

};

/////////////////////////////////////////////
//////// PHYSICS
////////////////////////////////////////////

  /////////////////////////////////////////
 ////////// Functionallity Tools ///////// 
/////////////////////////////////////////

// Returns a shinney new TLorentzVector Ep-4vec [Inputs(P_vector (MeV/c), A_0 (MeV))]
TLorentzVector make_lorentzvec(TVector3,double);

// Frame rotation to frame where vec_1=(0,0,1) [Inputs(Master_vector,Vector_to_change)]
// Leaves vec_1 unchanged
// (Change beam LAST and only IF needed)
void alight_to_Z(TVector3,TVector3&);

  ///////////////////////////////////////////////////////////////
 ////////// Single Body Relativistic E & P calculators   ///////
///////////////////////////////////////////////////////////////

// Returns total E relativistic [Inputs(P_frame (MeV/c), Mass_0 (amu))]
double get_relE_mom(double,double);

// Returns total E relativistic [Inputs(KE (MeV), Mass_0 (amu))]
double get_relE_KE(double,double);

// Returns P relativistic in current frame [Inputs(KE_frame (MeV), Mass_0 (amu), Print (TRUE/FALSE))]
double get_rel_mom(double,double,bool=false);

// Returns KE in current frame [Inputs(P_frame (MeV/c), Mass_0 (amu), Print (TRUE/FALSE))]
double get_KE(double,double,bool=false);

// Extract KE in current frame from TLorentzVector [Inputs(TLorentzVector*)]    Ep 4-vec in MeV
double get_KE(TLorentzVector*);


   //////////////////////////////////////////////////////
  ////////// Beta and Gamma calculators          ///////
 ////////// single particle in frame velocities /////// 
////////////////////////////////////////////////////// 

// Returns Gamma of single particle in frame [Inputs(KE_frame (MeV), Mass_0 (amu))]
double get_gamma_KE(double,double);

// Returns Gamma of single particle in frame [Inputs(P_frame (MeV/c), Mass_0 (amu))]
double get_gamma_mom(double,double);

// Returns Beta of single particle in frame [Inputs(KE_frame (MeV), Mass_0 (amu))]
double get_beta_KE(double,double);

// Returns Beta of single particle in frame [Inputs(P_frame (MeV/c), Mass_0 (amu))]
double get_beta_mom(double,double);


   ////////////////////////////////////
  ///////// Velocity of CoM    ///////
 /////////   (Boost beta)     ///////
//////////////////////////////////// 

// Returns Beta OF the CoM for beam + target [Inputs(P_beam (MeV/c), Mass_beam (amu), Mass_target (amu))]
double get_com_beta(double,double,double);

// Returns Beta OF the CoM from P vectors [Inputs(P_vector_1 (MeV/c), Mass_1 (amu), P_vector_2 (MeV/c), Mass_2 (amu))]
double get_com_beta(TVector3,double,TVector3,double);

    ///////////////////////////////////////////
   ///////// Product Momentum Split    ///////
  /////////   known trajectories      ///////
 /////////   Momentum conservation   ///////
///////////////////////////////////////////


// Returns P given trajectories using P conservation [Inputs(P_vec_beam (MeV/c), vec_1, vec_2)]
// Assumes a perfect 3 vector plane.
vector< double > mom_conserve_split(TVector3,TVector3,TVector3 );
// Returns fractions of P_0
vector< double > mom_conserve_unit(TVector3,TVector3,TVector3 );

// Returns P given trajectories using P conservation [Inputs(P_vec_beam (MeV/c), vec_1, vec_2, vec_3)]
// Directions must be accurate for good result as assumes perfect
vector< double > mom_conserve_split(TVector3,TVector3,TVector3,TVector3 );
// Returns fractions of P_0
vector< double > mom_conserve_unit(TVector3,TVector3,TVector3,TVector3 );

// For reactions of the type A + B -> Compound* + D -> FF_1 + FF_2 + D
// Good when phi is poorly known, but unreliable.
// [Inputs(P_vec_compound (MeV/c), Vec_FF1, Vec_FF2, vec_beam)]
vector< double > fission_compound_split(TVector3,TVector3,TVector3,TVector3=TVector3(0,0,1));
// Returns fractions of P_0
vector< double > fission_compound_unit(TVector3,TVector3,TVector3,TVector3=TVector3(0,0,1));


#endif


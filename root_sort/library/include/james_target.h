
//
//
//	James Target Object and related functions 2.2
// 	27/02/09		09/02/2015
//	james.smallcombe@outlook.com
//
//

#ifndef jamestar_h
#define jamestar_h

#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>

#include <TMath.h>
#include <TH2.h>
#include <TVector3.h>
#include <TRotation.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TFile.h>
#include <TRandom2.h>
#include <TLorentzVector.h>

#include "james_general.h"
#include "range.h"

using namespace std;


///////////////////////////////////////////////
/////////// Conversion functions    ///////////
///////////////////////////////////////////////

// Returns effective thickness for arbitrary trajectory
// Beam direction = (0,0,1)
// Axis of rotation = (0,1,0), clockwise from "above"
// [Inputs(Trajectory (Tvector3),Rotation (degrees),Nominal thickness (arb.))]
double target_effective(TVector3,double,double);

// Returns effective thickness for arbitrary trajectory
// Beam direction = (0,0,1)
// [Inputs(Trajectory (Tvector3),Target Normal (Tvector3),Nominal thickness (arb.))]
double target_effective(TVector3,TVector3,double);

// Returns thickness in (um)
// [Inputs(Density (g/cm3),Arial density (ug/cm2))]
double giveme_um(double,double,bool=true);

// Returns arial density in (ug/cm2)
// [Inputs(Density (g/cm3),Thickness (um))]
double giveme_areal(double,double,bool=true);

//mostly called internally by target class
void set_global_compound(vector<double>);

/////////////////////////////////////////////////////////////////////////////////////
/////////// "target" An object for holding and transfering target info    ///////////
///////////           handles energy loss with use of range.h             ///////////  
/////////////////////////////////////////////////////////////////////////////////////
class target
{ 
private:
    
    public:
	    
	////////////////////////////////////////
	/////////// Data members    ///////////
	///////////////////////////////////////

	int targ_Z;
	int targ_A;
	double target_thickness;//ug/cm^2
	TVector3 targ_norm; //should point away from backing (either beam or -beam)
	TVector3 targ_fornmal; //either norm or anti norm, point beamward
	int targ_compound;//-1 defined, 0 no, >0 fromlist

	double backing_thickness;//ug/cm^2
	int backing_Z;
	int backing_A;
	int backing_compound;//-1 defined, 0 no, 1 fromlist
	
	bool downstream_back;
	
	
// 	vector<double> compound_data;
	
	vector< TGraph* > particle;

	TH2D eloss_frag_graph_targ; //fission fragment energy graphs
	TH2D eloss_frag_graph_back;

	
	///////////////////////////////////////////////////
	/////////// Constructor & Destructor    ///////////
	//////////////////////////////////////////////////	
	
	// Backing is defined as UPstream
	// Compound_switch=0 none, =-1 user defined >0 from list
	// ONLY ONE user defined compound currently supported
	// Compound vec = (Z1,A1,weight1,Z2,A2,weight2 .... );


	//[Inputs(Targ_Z, Targ_A, Thickness/Arial density (ug/cm^2), Target Normal (TVector3), Target_compound_switch
	//        Backing_thickness, Backing_Z, Backing_A, Backing_compound_switch,
	//        Compound definition)]	
	// Beam direction = (0,0,1)
	// For a downstream "backing" set Target Normal = (0,0,-1)
	target(int=6, int=12, double=100, TVector3=TVector3(0.0,0.0,1.0), int=0, double=0, int=0, int=0, int=0);
	//Target Normal (TVector3) -> Rotation (degrees)
	// Axis of rotation = (0,1,0), clockwise from "above"
	target(int, int, double, double, int=0, double=0, int=0, int=0, int=0);

// 	//short cuts for backinless compound
// 	target(int a, int b, double c, double d, vector<double> e):target(a,b,c,d,-1,0.0,0,0,0,e){}
// 	target(int a, int b, double c, TVector3 d, vector<double> e):target(a,b,c,d,-1,0.0,0,0,0,e){}
	
	//WOULD ALSO NEED AN ASSIGNMENT OPPERATOR
	target( const target &obj);//copy constructor
	
	//assignment operator
	target& operator=(const target&);

	// destructor
	~target(); 
	
	//returns another target with backing and target material definitions flipped
	//geometrical orientation remains the same
	target inverse();
	
	//////////////////////////////////
	/////////// Methods    ///////////
	//////////////////////////////////
	
	// Returns particles / mb
	// based on target compound and A. (Does not work for "from list" compounds)
	double number_density();

	
	// Always assumes reaction at centre of target layer
	// to simulate otherwise try faking a target fronting etc
	
	// Returns energy in MeV of beam AT centre of target
	// [Inputs(beam_Z, beam_A, E_beam (MeV), Beam_vec)]	
	double beam_e_centre(int,int,double,TVector3=TVector3(0.0,0.0,1.0),double=0.5);
	
	// Returns energy in MeV of beam BEFORE target
	// [Inputs(beam_Z, beam_A, E_beam_CENTRE (MeV), Beam_vec)]	
	double beam_e_centre_reverse(int,int,double,TVector3=TVector3(0.0,0.0,1.0),double=0.5);	
	
	// Returns energy in MeV of paricle AFTER exit from centre
	// [Inputs(particle_Z, particle_A, E_BEFORE (MeV), Trajctory)]	
	double particle_e_exit(int,int,double,TVector3,double&);
	
	// Returns energy in MeV of paricle BEFORE target exit
	// [Inputs(particle_Z, particle_A, E_AFTER (MeV), Trajctory)]		
	double particle_e_reverse(int,int,double,TVector3,double=0.5);
	
	// Create a fission fragment energy lookup table in the target object
	// Slow initial process, attempts to load and save a local file.
	// [Inputs(Compound_Z, Compound_A)]	
	void fragment_set(double,double);
	
	// Lookup fission fragment energy loss if table has been generated
	// [Inputs(Fragment_mass (amu), Fragment Energy (MeV), Trajctory)]		
	double fragment_e_exit(double,double,TVector3,double=0.5);//Z,a,MeV,vector
	
	// Calculate energy change in a target
	// Start and end fraction should match trajectort orientation
	// If reverse, E is the energy AFTER and you will get energy BEFORE
	// [Inputs(particle_Z, particle_A, E (MeV), Trajctory, start frac, end frac, reverse?)]
	double traverse(int,int,double,TVector3,double,double,bool=false);


	void set_particle(int,int);
	double get_range_particle(bool,double);
	double passage_particle(bool,double,double,bool=false);
	double traverse_p(double,TVector3,double=0.5,double=2,bool=false);
	double traverse_p(double,double=1.0,double=0.5,double=2,bool=false);
		
// 	double lifetime_track(int,int,double&,TLorentzVector*,double&);

// 	ClassDef(target,0);// needed for "true" ROOT integration
};

	///////////////////////////////////////////////////
	/////////// range.h info for compounds    /////////
	//////////////////////////////////////////////////

// pred defined compounds
// 	   1: Mylar (C10-H8-O4)			1397 mg/cm3
// 	   2: Polyethylene (C2-H4)		 930 mg/cm3
// 	   3: Polypropylene (C3-H6)		 900 mg/cm3
// 	   4: Kapton (H10-C22-N2-O5)		1430 mg/cm3
// 	   5: Cesium Iodine (CsI)		4510 mg/cm3
// 	   6: Sodium Iodine (NaI)		3670 mg/cm3
// 	   7: Aluminum Oxide (Al2O3)		3980 mg/cm3
// 	   8: Tin-Lead (Sn60/Pb40)		8420 mg/cm3
// 	   9: Nickel (Ni-nat)			8902 mg/cm3
// 
// 	Gases:
// 	      Gas				Density (T=298 K)
// 	 100: Carbon Tetrafluoride (CF4)	P(Torr) x 4.73538e-3 mg/cm3
// 	 101: Propane (C3-H8)			P(Torr) x 2.37276e-3 mg/cm3
// 	 102: Butane (C4-H10)			P(Torr) x 3.12752e-3 mg/cm3
// 	 103: Octane (C8-H18)			P(Torr) x 6.14657e-3 mg/cm3

#endif // #ifdef z90selector_cxx

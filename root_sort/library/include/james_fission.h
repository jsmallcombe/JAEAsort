//
//
//	james_fission version 1.3
// 	27/02/09		09/02/2015
//	james.smallcombe@outlook.com
//
//

#ifndef jamesfission_h
#define jamesfission_h

#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>

#include <TObjArray.h>
#include <TMath.h>
#include <TGraph.h>
#include <TH2.h>
#include <TVector3.h>

#include "james_general.h"
#include "james_target.h"
#include "range.h"
using namespace std;



//////////////////////////////////////////////////////////
/////////// Fission Fragment mass calculations /////////// 
//////////////////////////////////////////////////////////

// Returns 2 masses in amu
// [Inputs(Worldhit1 (mm), Worldhit2 (mm), P1 (MeV/c2), P2 (MeV/c2), dT (ns), target, compound mass, reps, mass graph)]
// If reps <1 simple calc without E loss or relativity ELSE target should have run "fragment_set" fn
// Will break before reps once consitent 
double* mass_split_calc(TVector3,TVector3,double,double,double,target*,double,int=0,TGraph=TGraph());
// [Inputs(Worldhit1 (mm), Worldhit2 (mm), P_vec_beam(MeV/c2), dT (ns), target, compound mass, reps, mass graph)]
double* mass_split_calc(TVector3,TVector3,TVector3,double,target*,double,int=0,TGraph=TGraph());

// Return a graph for mass lookup during fission fragment calc
// Corrected for mass excess
// x-axis=one fragment mass (amu) y-axis=fragment mass sum (amu)
// [Inputs(Compound Z,Compound N,nuclear_data_ob)]
TGraph mass_convert(int,int);


//////////////////////////////////////////////////////////////////////////////////
/////////// Mostly obsolete functions to calc energy loss from a graph /////////// 
//////////////////////////////////////////////////////////////////////////////////

TH2D* eloss_mass_grapher(double,double, double, double,int,int=1,int=1,int=1,
	int=1,int=1,double=1,int=1,int=1,double=1,int=1,int=1,double=1,int=1,int=1,double=1);

#endif

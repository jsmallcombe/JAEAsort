//
//
//	james_fission_master_selector 2.0
// 	27/02/09		09/02/2015
//	james.smallcombe@outlook.com
//
//
#ifndef master_selector_h
#define master_selector_h

#include <iostream>
#include <fstream>
#include <vector>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TStyle.h>
#include <TTree.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TMath.h>
#include <TTreePlayer.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TRotation.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TCutG.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TCanvas.h>

#include <range.h>
#include "james_general.h"
#include "james_fission.h"
#include "james_target.h"

class master_selector : public TSelector {
public :
	
    TTree *fChain;   //!pointer to the analyzed TTree or TChain
	
  // Declaration of leaf types of input ttree
   Int_t           MWPC1_cat,MWPC2_cat,MWPC3_cat,MWPC4_cat;
   Int_t           MWPC1_cat_time,MWPC2_cat_time,MWPC3_cat_time,MWPC4_cat_time;
   Int_t           MWPC1_x1,MWPC1_x2,MWPC1_y;
   Int_t           MWPC1_x1_time,MWPC1_x2_time,MWPC1_y_time;
   Int_t           MWPC2_x1,MWPC2_x2,MWPC2_y;
   Int_t           MWPC2_x1_time,MWPC2_x2_time,MWPC2_y_time;
   Int_t           MWPC3_x1,MWPC3_y;
   Int_t           MWPC3_x1_time,MWPC3_y_time;
   Int_t           MWPC4_x1,MWPC4_y;
   Int_t           MWPC4_x1_time,MWPC4_y_time;
   Int_t           F_C_,F_C__time;
   Int_t           dT_A_B,dT_A_B_time,dT_B_C,dT_B_C_time,dT_A_D,dT_A_D_time;
   Int_t           ruth_mon,ruth_mon_time;

   // List of branches
   TBranch           *b_MWPC1_cat,*b_MWPC2_cat,*b_MWPC3_cat,*b_MWPC4_cat;
   TBranch           *b_MWPC1_cat_time,*b_MWPC2_cat_time,*b_MWPC3_cat_time,*b_MWPC4_cat_time;
   TBranch           *b_MWPC1_x1,*b_MWPC1_x2,*b_MWPC1_y;
   TBranch           *b_MWPC1_x1_time,*b_MWPC1_x2_time,*b_MWPC1_y_time;
   TBranch           *b_MWPC2_x1,*b_MWPC2_x2,*b_MWPC2_y;
   TBranch           *b_MWPC2_x1_time,*b_MWPC2_x2_time,*b_MWPC2_y_time;
   TBranch           *b_MWPC3_x1,*b_MWPC3_y;
   TBranch           *b_MWPC3_x1_time,*b_MWPC3_y_time;
   TBranch           *b_MWPC4_x1,*b_MWPC4_y;
   TBranch           *b_MWPC4_x1_time,*b_MWPC4_y_time;
   TBranch           *b_F_C_,*b_F_C__time;
   TBranch           *b_dT_A_B,*b_dT_A_B_time,*b_dT_B_C,*b_dT_B_C_time,*b_dT_A_D,*b_dT_A_D_time;
   TBranch           *b_ruth_mon,*b_ruth_mon_time;

   
TBranch        *b_siA_1,*b_siA_2,*b_siA_3,*b_siA_4,*b_siA_5,*b_siA_6,*b_siA_7,*b_siA_8,*b_siA_9,*b_siA_10,*b_siA_11,*b_siA_12,*b_siA_13,*b_siA_14,*b_siA_15,*b_siA_16;   
TBranch        *b_siA_1_time,*b_siA_2_time,*b_siA_3_time,*b_siA_4_time,*b_siA_5_time,*b_siA_6_time,*b_siA_7_time,*b_siA_8_time,*b_siA_9_time,*b_siA_10_time,*b_siA_11_time,*b_siA_12_time,*b_siA_13_time,*b_siA_14_time,*b_siA_15_time,*b_siA_16_time;   
TBranch        *b_siB_1,*b_siB_2,*b_siB_3,*b_siB_4,*b_siB_5,*b_siB_6,*b_siB_7,*b_siB_8,*b_siB_9,*b_siB_10,*b_siB_11,*b_siB_12,*b_siB_13,*b_siB_14,*b_siB_15,*b_siB_16;   
TBranch        *b_siB_1_time,*b_siB_2_time,*b_siB_3_time,*b_siB_4_time,*b_siB_5_time,*b_siB_6_time,*b_siB_7_time,*b_siB_8_time,*b_siB_9_time,*b_siB_10_time,*b_siB_11_time,*b_siB_12_time,*b_siB_13_time,*b_siB_14_time,*b_siB_15_time,*b_siB_16_time;   

TBranch        *b_dE_1,*b_dE_2,*b_dE_3,*b_dE_4,*b_dE_5,*b_dE_6,*b_dE_7,*b_dE_8,*b_dE_9,*b_dE_10,*b_dE_11,*b_dE_12; 
TBranch        *b_dE_1_time,*b_dE_2_time,*b_dE_3_time,*b_dE_4_time,*b_dE_5_time,*b_dE_6_time,*b_dE_7_time,*b_dE_8_time,*b_dE_9_time,*b_dE_10_time,*b_dE_11_time,*b_dE_12_time;

	
	
	///////////////////////////////////
	/////// Variable declaration //////
	///////////////////////////////////

	////// OUTPUT FILES /////
	string output;
	TFile* output_file;
	TTree *outtree;
	
	///// BRANCHES OF OUTPUT TREE /////
	double theta1,theta2,phi1,phi2,d_theta,d_phi,massA,massB;
	double sumKE_com,x_diff,MWPC_dT_raw;
	int AD_BC_swit;
	TVector3  worldhit1,worldhit2,worldhit3;
	
	///// Silicon and dE extra things /////

	//Output Branches
	double siliconenergy,silicondelta,si_dt,MWPC_si_dt;
		
	double silicon_array_distance;
	string si_cal_file;
	
	silicon_hit silicon;	
	//holds the silicon calibration values
	vector< double > quad_cal,grad_cal,intercept_cal,deangle_cal;
	
	//easier to hold data for silicon detecor as a single object
	vector< Int_t* > silicon_time_p,silicon_data_p;
	vector< double > silicon_time,silicon_data;

	//// MWPC DETECTORS ////
	TVector3  det1pos,det2pos,det3pos,det4pos;
	TVector3  dethit1,dethit2,dethit3,dethit4;
	TVector3  worldhitA,worldhitB,worldhitC,worldhitD;
	TRotation  det1rot,det2rot,det3rot,det4rot;
	int mwpc1chan[6],mwpc2chan[6],mwpc3chan[6],mwpc4chan[6];
	//arrays of spacial/daq calibration
	
	////// Input Variables ///////
	target* targ;
	double ebeam, beam_Z, beam_A;
	double tzeroAB,tzeroBC,tzeroAD;
	bool tcal;
	int calculation_version;
	int detconfig;
	
	////// Calculation Variables ///////
	double time_max;
	TGraph mass_adj_graph;
	double pbeam,ebeam_slow,pbeam_slow,beta_cm,masszero,newT;
	TGraph *TimegraphAB,*TimegraphBC,*TimegraphAD;
	Int_t nentries;
	
	double* tzero[3];
	int cn;
	string ABCDstring[3];

	
	////// Histograms ///////
	TH2D *hit_patern_1,*hit_patern_2,*hit_patern_3,*hit_patern_4;
	TH2D *silicon_view,*silicon_dE_E;
	TH2D* dangles,*massQ;
	TH1D* dthetagr,*massca;	
	TH1D *massAB,*massBC,*massAD,*mass_delta;
	TH1D *massABCD[3];
	TH3D *threedee;
	
	TH2D *ABCD_heavy[3],*ABCD_light[3],*ABCD_sum[3],*ABCD_delta[3];
	
	////// Ternary fission ///////
	int ternary;
	TVector3 triA,triB,triC;
	double tri_dt_AB,tri_dt_BC,tri_dt_CA;
	TH3D *ternary_mom;

//////////////////////////////////////////////////////////////////////	
        //////////// Selector Creation Method  ////////////////
//////////////////////////////////////////////////////////////////////	
	
master_selector(double inebeam,
		double inZbeam,
		double inAbeam,
		target* intarg,
		string outpath_in,
		int detconfigin,
		int mwpc_limts[],
		double tzeroinA=0,
		double tzeroinB=0,
		double tzeroinC=0,
		int calcvin=0,
		double silicondin=0,
		string si_cal_in="t.root",
		TTree * /*tree*/ =0) :
	// list of input assignments
	fChain(0),  				// Fundamental pointer of to tree/chain of a selector class
	output(outpath_in),			// Output filename
	silicon_array_distance(silicondin),	// Distance of the si dE/E array in mm
	si_cal_file(si_cal_in),			// .root file with the silicon dE/E calibration numbers in it
	targ(intarg),				// target class object containing all target information
	ebeam(inebeam),				// beam energy (lab frame) MeV
	beam_Z(inZbeam),			// Beam Z
	beam_A(inAbeam),			// Beam A
	tzeroAB(tzeroinA),			// t_0 calibration value for AB detector pair
	tzeroBC(tzeroinB),			// t_0 calibration value for BC detector pair
	tzeroAD(tzeroinC),			// t_0 calibration value for AD detector pair
	calculation_version(calcvin),		// control int for level of interative mass calculation
	detconfig(detconfigin),			// control int specifying the detector configuration 
	time_max(10){				// Placeholder for maximum time difference, used for reseting loop variables
		
		///////////////////////////////////
		// Begining of creation function
		///////////////////////////////////
	
		// If no t_0 values are given then set this to be a "time calibration" sort
		tcal=true;
		if(abs(tzeroinA)+abs(tzeroinB)+abs(tzeroinC)>0)	tcal=false;	
		
		// tzero point
		tzero[0]=&tzeroAB;tzero[1]=&tzeroBC;tzero[2]=&tzeroAD;
		ABCDstring[0]="AB";ABCDstring[1]="BC";ABCDstring[2]="AD";
		
		cn=1;
		if(detconfig==0)cn=3;
			
		/////////////////////////////////
		/// System physics  
		/////////////////////////////////
		
		double compound_Z=targ->targ_Z+beam_Z;
		double compound_A=targ->targ_A+beam_A;	

		masszero=compound_A+(get_mass_excess(compound_Z,compound_A-compound_Z)/jam_phys_amu);
		mass_adj_graph = mass_convert(compound_Z,compound_A);

		double targ_mass =  targ->targ_A+(get_mass_excess(targ->targ_Z,targ->targ_A-targ->targ_Z)/jam_phys_amu);
		double beam_mass = beam_A+(get_mass_excess(beam_Z,beam_A-beam_Z)/jam_phys_amu);

		pbeam=get_rel_mom(ebeam,beam_mass,false);
		ebeam_slow=targ->beam_e_centre(beam_Z,beam_A,ebeam);//z,a,MeV
		pbeam_slow=get_rel_mom(ebeam_slow,beam_mass,false);
		beta_cm=get_com_beta(pbeam_slow,targ_mass,beam_mass);	
		
		/////////////////////////////////////////////////////////
		// This is data from the last time the TAC channels used for the MWPCs were caibrated
		// These are used for a more accurate conversion between DAQ channel and ns of flight time
		/////////////////////////////////////////////////////////

		double t[]={80,160,240,320,400,480,560,640,720,800};
		double ta[]={784,1567,2352,3135.3,3915.6,4692.7,5465.1,6233.6,7001.2,7768.7};
		double tb[]={797.8,1606,2415,3223.7,4028.5,4829.7,5625.6,6418,7209.7,8001};
		double tc[]={779,1561.5,2345.8,3129.3,3909.8,4686.5,5458.6,6226.4,6993.2,7760.1};
		
		TimegraphAB= new TGraph(10,ta,t);
		TimegraphAD= new TGraph(10,tb,t);
		TimegraphBC= new TGraph(10,tc,t);

		/////////////////////////////////////////////////////////		
		/////////////////////////////////////////////////////////		
		// Set MWPC data, positions and rotations of detectors
		/////////////////////////////////////////////////////////		
		/////////////////////////////////////////////////////////
		switch ( detconfig ){
			case 1:
				det1pos=TVector3(249,0,0);
				det2pos=TVector3(-249,0,0);
				det1rot.RotateZ(pi/2);
				det1rot.RotateY(-pi/2);
				det2rot.RotateZ(pi/2);
				det2rot.RotateY(pi/2);
				for(int i=0;i<4;i++){
					mwpc1chan[i]=mwpc_limts[i];
					mwpc2chan[i]=mwpc_limts[i+4];
				}
				break;
			case 2:
				det1pos=TVector3(215,0,27);
				det2pos=TVector3(-215,0,27);
				det1rot.RotateZ(pi/2);
				det1rot.RotateY(-pi/2);
				det2rot.RotateZ(pi/2);
				det2rot.RotateY(pi/2);
				for(int i=0;i<4;i++){
					mwpc1chan[i]=mwpc_limts[i];
					mwpc2chan[i]=mwpc_limts[i+4];
				}
				break;
			case 3:
				det1pos=TVector3(304.9,0,304.9);
				det1rot.RotateY(TMath::Pi()/4);
				det2pos=TVector3(-304.9,0,304.9);
				det2rot.RotateY(-TMath::Pi()/4);
				for(int i=0;i<6;i++){
					mwpc1chan[i]=mwpc_limts[i];
					mwpc2chan[i]=mwpc_limts[i+6];
				}
				break;
			case 4:
				det1pos=TVector3(170.46,0,113.77);
				det1rot.RotateX(TMath::Pi()/2);                
				det1rot.RotateY(-TMath::Pi()/4);
				det2pos=TVector3(-170.46,0,113.77);
				det2rot.RotateX(TMath::Pi()/2);
				det2rot.RotateY(TMath::Pi()/4);
				for(int i=0;i<6;i++){
					mwpc1chan[i]=mwpc_limts[i];
					mwpc2chan[i]=mwpc_limts[i+6];
				}
				break;
			default:
				det1pos=TVector3(cos(0.873)*224,0,sin(0.873)*224);
				det1rot.RotateZ(pi/2);
				det1rot.RotateY(0.873);
				det2pos=TVector3(-cos(0.873)*224,0,sin(0.873)*224);
				det2rot.RotateZ(pi/2);
				det2rot.RotateY(-0.873);
				det3pos=TVector3(cos(0.873)*224,0,-sin(0.873)*224);
				det3rot.RotateZ(pi/2);
				det3rot.RotateY(2.269);
				det4pos=TVector3(-cos(0.873)*224,0,-sin(0.873)*224);
				det4rot.RotateZ(pi/2);
				det4rot.RotateY(-2.269);
				for(int i=0;i<4;i++){
					mwpc1chan[i]=mwpc_limts[i];
					mwpc2chan[i]=mwpc_limts[i+4];
					mwpc3chan[i]=mwpc_limts[i+8];
					mwpc4chan[i]=mwpc_limts[i+12];
				}
		}
		
		////////////
		//easier to hold data for silicon detecor as a single object
		///////////
		for(int i=0;i<44;i++){
			silicon_time_p.push_back(new Int_t);
			silicon_data_p.push_back(new Int_t);
			silicon_time.push_back(0.0);
			silicon_data.push_back(0.0);
		}
	}

	///////////////////////////////////
	// End of creation function
	///////////////////////////////////
	

   		
	///////////////////////////////////
	// Other functions of selector ////
	///////////////////////////////////
	
	
   virtual ~master_selector() { delete TimegraphAB;delete TimegraphAD;delete TimegraphBC;}
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
   void  cal_check(TTree*);

   ClassDef(master_selector,1);
};
#endif


#ifdef master_selector_cxx
void master_selector::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

	// IF YOU USE DIFFERENT NAMES DURING EVENT CONSTRUCTION WITH NIKIROOT OR OTHER MEANS
	// YOU WILL NEED TO CHANGE THE NAMES HERE
   
   fChain->SetBranchAddress("MWPC1_cat", &MWPC1_cat, &b_MWPC1_cat);
   fChain->SetBranchAddress("MWPC1_cat_time", &MWPC1_cat_time, &b_MWPC1_cat_time);
   fChain->SetBranchAddress("MWPC2_cat", &MWPC2_cat, &b_MWPC2_cat);
   fChain->SetBranchAddress("MWPC2_cat_time", &MWPC2_cat_time, &b_MWPC2_cat_time);
   fChain->SetBranchAddress("MWPC1_x1", &MWPC1_x1, &b_MWPC1_x1);
   fChain->SetBranchAddress("MWPC1_x1_time", &MWPC1_x1_time, &b_MWPC1_x1_time);
   fChain->SetBranchAddress("MWPC1_y", &MWPC1_y, &b_MWPC1_y);
   fChain->SetBranchAddress("MWPC1_y_time", &MWPC1_y_time, &b_MWPC1_y_time);
   fChain->SetBranchAddress("MWPC2_x1", &MWPC2_x1, &b_MWPC2_x1);
   fChain->SetBranchAddress("MWPC2_x1_time", &MWPC2_x1_time, &b_MWPC2_x1_time);
   fChain->SetBranchAddress("MWPC2_y", &MWPC2_y, &b_MWPC2_y);
   fChain->SetBranchAddress("MWPC2_y_time", &MWPC2_y_time, &b_MWPC2_y_time);   
   fChain->SetBranchAddress("dT_A_B", &dT_A_B, &b_dT_A_B);
   fChain->SetBranchAddress("dT_A_B_time", &dT_A_B_time, &b_dT_A_B_time);
   
   if(detconfig==3){ //IF USING LONG MWPCS
	fChain->SetBranchAddress("MWPC1_x2", &MWPC1_x2, &b_MWPC1_x2);
	fChain->SetBranchAddress("MWPC1_x2_time", &MWPC1_x2_time, &b_MWPC1_x2_time);
	fChain->SetBranchAddress("MWPC2_x2", &MWPC2_x2, &b_MWPC2_x2);
	fChain->SetBranchAddress("MWPC2_x2_time", &MWPC2_x2_time, &b_MWPC2_x2_time);
   }
   
   if(detconfig==0){ //IF USING 4 MWPCS
	fChain->SetBranchAddress("MWPC3_cat", &MWPC3_cat, &b_MWPC3_cat);
	fChain->SetBranchAddress("MWPC3_cat_time", &MWPC3_cat_time, &b_MWPC3_cat_time);
	fChain->SetBranchAddress("MWPC4_cat", &MWPC4_cat, &b_MWPC4_cat);
	fChain->SetBranchAddress("MWPC4_cat_time", &MWPC4_cat_time, &b_MWPC4_cat_time);
	fChain->SetBranchAddress("MWPC3_x1", &MWPC3_x1, &b_MWPC3_x1);
	fChain->SetBranchAddress("MWPC3_x1_time", &MWPC3_x1_time, &b_MWPC3_x1_time);
	fChain->SetBranchAddress("MWPC3_y", &MWPC3_y, &b_MWPC3_y);
	fChain->SetBranchAddress("MWPC3_y_time", &MWPC3_y_time, &b_MWPC3_y_time);
	fChain->SetBranchAddress("MWPC4_x1", &MWPC4_x1, &b_MWPC4_x1);
	fChain->SetBranchAddress("MWPC4_x1_time", &MWPC4_x1_time, &b_MWPC4_x1_time);
	fChain->SetBranchAddress("MWPC4_y", &MWPC4_y, &b_MWPC4_y);
	fChain->SetBranchAddress("MWPC4_y_time", &MWPC4_y_time, &b_MWPC4_y_time);
	fChain->SetBranchAddress("dT_B_C", &dT_B_C, &b_dT_B_C);
	fChain->SetBranchAddress("dT_B_C_time", &dT_B_C_time, &b_dT_B_C_time);
	fChain->SetBranchAddress("dT_A_D", &dT_A_D, &b_dT_A_D);
	fChain->SetBranchAddress("dT_A_D_time", &dT_A_D_time, &b_dT_A_D_time);
   }
   
   fChain->SetBranchAddress("F.C.", &F_C_, &b_F_C_);
   fChain->SetBranchAddress("F.C._time", &F_C__time, &b_F_C__time);   
   fChain->SetBranchAddress("ruth_mon", &ruth_mon, &b_ruth_mon);
   fChain->SetBranchAddress("ruth_mon_time", &ruth_mon_time, &b_ruth_mon_time);
   
   if(silicon_array_distance>0){
	fChain->SetBranchAddress("siA_1",silicon_data_p[0], &b_siA_1);
	fChain->SetBranchAddress("siA_2",silicon_data_p[1], &b_siA_2);
	fChain->SetBranchAddress("siA_3",silicon_data_p[2], &b_siA_3);
	fChain->SetBranchAddress("siA_4",silicon_data_p[3], &b_siA_4);
	fChain->SetBranchAddress("siA_5",silicon_data_p[4], &b_siA_5);
	fChain->SetBranchAddress("siA_6",silicon_data_p[5], &b_siA_6);
	fChain->SetBranchAddress("siA_7",silicon_data_p[6], &b_siA_7);
	fChain->SetBranchAddress("siA_8",silicon_data_p[7], &b_siA_8);
	fChain->SetBranchAddress("siA_9",silicon_data_p[8], &b_siA_9);
	fChain->SetBranchAddress("siA_10",silicon_data_p[9], &b_siA_10);
	fChain->SetBranchAddress("siA_11",silicon_data_p[10], &b_siA_11);
	fChain->SetBranchAddress("siA_12",silicon_data_p[11], &b_siA_12);
	fChain->SetBranchAddress("siA_13",silicon_data_p[12], &b_siA_13);
	fChain->SetBranchAddress("siA_14",silicon_data_p[13], &b_siA_14);
	fChain->SetBranchAddress("siA_15",silicon_data_p[14], &b_siA_15);
	fChain->SetBranchAddress("siA_16",silicon_data_p[15], &b_siA_16);
	fChain->SetBranchAddress("siB_1",silicon_data_p[16], &b_siB_1);
	fChain->SetBranchAddress("siB_2",silicon_data_p[17], &b_siB_2);
	fChain->SetBranchAddress("siB_3",silicon_data_p[18], &b_siB_3);
	fChain->SetBranchAddress("siB_4",silicon_data_p[19], &b_siB_4);
	fChain->SetBranchAddress("siB_5",silicon_data_p[20], &b_siB_5);
	fChain->SetBranchAddress("siB_6",silicon_data_p[21], &b_siB_6);
	fChain->SetBranchAddress("siB_7",silicon_data_p[22], &b_siB_7);
	fChain->SetBranchAddress("siB_8",silicon_data_p[23], &b_siB_8);
	fChain->SetBranchAddress("siB_9",silicon_data_p[24], &b_siB_9);
	fChain->SetBranchAddress("siB_10",silicon_data_p[25], &b_siB_10);
	fChain->SetBranchAddress("siB_11",silicon_data_p[26], &b_siB_11);
	fChain->SetBranchAddress("siB_12",silicon_data_p[27], &b_siB_12);
	fChain->SetBranchAddress("siB_13",silicon_data_p[28], &b_siB_13);
	fChain->SetBranchAddress("siB_14",silicon_data_p[29], &b_siB_14);
	fChain->SetBranchAddress("siB_15",silicon_data_p[30], &b_siB_15);
	fChain->SetBranchAddress("siB_16",silicon_data_p[31], &b_siB_16);
	fChain->SetBranchAddress("dE_1",silicon_data_p[32], &b_dE_1);
	fChain->SetBranchAddress("dE_2",silicon_data_p[33], &b_dE_2);
	fChain->SetBranchAddress("dE_3",silicon_data_p[34], &b_dE_3);
	fChain->SetBranchAddress("dE_4",silicon_data_p[35], &b_dE_4);
	fChain->SetBranchAddress("dE_5",silicon_data_p[36], &b_dE_5);
	fChain->SetBranchAddress("dE_6",silicon_data_p[37], &b_dE_6);
	fChain->SetBranchAddress("dE_7",silicon_data_p[38], &b_dE_7);
	fChain->SetBranchAddress("dE_8",silicon_data_p[39], &b_dE_8);
	fChain->SetBranchAddress("dE_9",silicon_data_p[40], &b_dE_9);
	fChain->SetBranchAddress("dE_10",silicon_data_p[41], &b_dE_10);
	fChain->SetBranchAddress("dE_11",silicon_data_p[42], &b_dE_11);
	fChain->SetBranchAddress("dE_12",silicon_data_p[43], &b_dE_12);

	fChain->SetBranchAddress("siA_1_time",silicon_time_p[0], &b_siA_1_time);
	fChain->SetBranchAddress("siA_2_time",silicon_time_p[1], &b_siA_2_time);
	fChain->SetBranchAddress("siA_3_time",silicon_time_p[2], &b_siA_3_time);
	fChain->SetBranchAddress("siA_4_time",silicon_time_p[3], &b_siA_4_time);
	fChain->SetBranchAddress("siA_5_time",silicon_time_p[4], &b_siA_5_time);
	fChain->SetBranchAddress("siA_6_time",silicon_time_p[5], &b_siA_6_time);
	fChain->SetBranchAddress("siA_7_time",silicon_time_p[6], &b_siA_7_time);
	fChain->SetBranchAddress("siA_8_time",silicon_time_p[7], &b_siA_8_time);
	fChain->SetBranchAddress("siA_9_time",silicon_time_p[8], &b_siA_9_time);
	fChain->SetBranchAddress("siA_10_time",silicon_time_p[9], &b_siA_10_time);
	fChain->SetBranchAddress("siA_11_time",silicon_time_p[10], &b_siA_11_time);
	fChain->SetBranchAddress("siA_12_time",silicon_time_p[11], &b_siA_12_time);
	fChain->SetBranchAddress("siA_13_time",silicon_time_p[12], &b_siA_13_time);
	fChain->SetBranchAddress("siA_14_time",silicon_time_p[13], &b_siA_14_time);
	fChain->SetBranchAddress("siA_15_time",silicon_time_p[14], &b_siA_15_time);
	fChain->SetBranchAddress("siA_16_time",silicon_time_p[15], &b_siA_16_time);
	fChain->SetBranchAddress("siB_1_time",silicon_time_p[16], &b_siB_1_time);
	fChain->SetBranchAddress("siB_2_time",silicon_time_p[17], &b_siB_2_time);
	fChain->SetBranchAddress("siB_3_time",silicon_time_p[18], &b_siB_3_time);
	fChain->SetBranchAddress("siB_4_time",silicon_time_p[19], &b_siB_4_time);
	fChain->SetBranchAddress("siB_5_time",silicon_time_p[20], &b_siB_5_time);
	fChain->SetBranchAddress("siB_6_time",silicon_time_p[21], &b_siB_6_time);
	fChain->SetBranchAddress("siB_7_time",silicon_time_p[22], &b_siB_7_time);
	fChain->SetBranchAddress("siB_8_time",silicon_time_p[23], &b_siB_8_time);
	fChain->SetBranchAddress("siB_9_time",silicon_time_p[24], &b_siB_9_time);
	fChain->SetBranchAddress("siB_10_time",silicon_time_p[25], &b_siB_10_time);
	fChain->SetBranchAddress("siB_11_time",silicon_time_p[26], &b_siB_11_time);
	fChain->SetBranchAddress("siB_12_time",silicon_time_p[27], &b_siB_12_time);
	fChain->SetBranchAddress("siB_13_time",silicon_time_p[28], &b_siB_13_time);
	fChain->SetBranchAddress("siB_14_time",silicon_time_p[29], &b_siB_14_time);
	fChain->SetBranchAddress("siB_15_time",silicon_time_p[30], &b_siB_15_time);
	fChain->SetBranchAddress("siB_16_time",silicon_time_p[31], &b_siB_16_time);
	fChain->SetBranchAddress("dE_1_time",silicon_time_p[32], &b_dE_1_time);
	fChain->SetBranchAddress("dE_2_time",silicon_time_p[33], &b_dE_2_time);
	fChain->SetBranchAddress("dE_3_time",silicon_time_p[34], &b_dE_3_time);
	fChain->SetBranchAddress("dE_4_time",silicon_time_p[35], &b_dE_4_time);
	fChain->SetBranchAddress("dE_5_time",silicon_time_p[36], &b_dE_5_time);
	fChain->SetBranchAddress("dE_6_time",silicon_time_p[37], &b_dE_6_time);
	fChain->SetBranchAddress("dE_7_time",silicon_time_p[38], &b_dE_7_time);
	fChain->SetBranchAddress("dE_8_time",silicon_time_p[39], &b_dE_8_time);
	fChain->SetBranchAddress("dE_9_time",silicon_time_p[40], &b_dE_9_time);
	fChain->SetBranchAddress("dE_10_time",silicon_time_p[41], &b_dE_10_time);
	fChain->SetBranchAddress("dE_11_time",silicon_time_p[42], &b_dE_11_time);
	fChain->SetBranchAddress("dE_12_time",silicon_time_p[43], &b_dE_12_time);
   }
}

Bool_t master_selector::Notify(){
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.
	if(fChain->GetTree()){
		cout<<endl<<"Opening to process : "<<fChain->GetCurrentFile()->GetName();
		cout<<endl<<"TTree : "<<fChain->GetTree()->GetName()<<" "<<fChain->GetTree()->GetEntries()<<" entries."<<endl;
		nentries = fChain->GetTree()->GetEntries(); 
	}

   return kTRUE;
}

#endif // #ifdef master_selector_cxx
 

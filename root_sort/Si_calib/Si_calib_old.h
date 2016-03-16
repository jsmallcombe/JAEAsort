//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Jul 30 18:00:09 2014 by ROOT version 5.34/18
// from TTree run005/run005
// found on file: run005.root
//////////////////////////////////////////////////////////

#ifndef Si_calib_h
#define Si_calib_h

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TTree.h>
#include <TSystem.h>
#include <TObjArray.h>
#include <TMath.h>
#include <TTreePlayer.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TRotation.h>
#include <TH2.h>
#include <TH1.h>
#include <TH3.h>
#include <TCutG.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TCanvas.h>
#include <range.h>

#include "james_detector_func.h"
#include "james_physics.h"
#include "james_target.h"
#include "james_root_maths.h"


#include "nuclear_data_ob.h"

// Header file for the classes stored in the TTree if any.
// Fixed size dimensions of array or collections stored in the TTree if any.

class Si_calib : public TSelector {
public :
	vector< Int_t* > silicon_data_p;
	
	vector< double > silicon_data;

	vector< TH1D* > E_graphs;

	vector< TH1D* > Ed_graphs;
	
	vector< vector< TH1D*  >   > dE_graphs;
	
	vector< TH1D*  > dE_sums;
	
	TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // List of branches
   TBranch        *b_siA_1;   //!
   TBranch        *b_siA_2;   //!
   TBranch        *b_siA_3;   //!
   TBranch        *b_siA_4;   //!
   TBranch        *b_siA_5;   //!
   TBranch        *b_siA_6;   //!
   TBranch        *b_siA_7;   //!
   TBranch        *b_siA_8;   //!
   TBranch        *b_siA_9;   //!
   TBranch        *b_siA_10;   //!
   TBranch        *b_siA_11;   //!
   TBranch        *b_siA_12;   //!
   TBranch        *b_siA_13;   //!
   TBranch        *b_siA_14;   //!
   TBranch        *b_siA_15;   //!
   TBranch        *b_siA_16;   //!
   TBranch        *b_siB_1;   //!
   TBranch        *b_siB_2;   //!
   TBranch        *b_siB_3;   //!
   TBranch        *b_siB_4;   //!
   TBranch        *b_siB_5;   //!
   TBranch        *b_siB_6;   //!
   TBranch        *b_siB_7;   //!
   TBranch        *b_siB_8;   //!
   TBranch        *b_siB_9;   //!
   TBranch        *b_siB_10;   //!
   TBranch        *b_siB_11;   //!
   TBranch        *b_siB_12;   //!
   TBranch        *b_siB_13;   //!
   TBranch        *b_siB_14;   //!
   TBranch        *b_siB_15;   //!
   TBranch        *b_siB_16;   //!
   TBranch        *b_dE_1;   //!
   TBranch        *b_dE_2;   //!
   TBranch        *b_dE_3;   //!
   TBranch        *b_dE_4;   //!
   TBranch        *b_dE_5;   //!
   TBranch        *b_dE_6;   //!
   TBranch        *b_dE_7;   //!
   TBranch        *b_dE_8;   //!
   TBranch        *b_dE_9;   //!
   TBranch        *b_dE_10;   //!
   TBranch        *b_dE_11;   //!
   TBranch        *b_dE_12;   //!
   
   target targ;
   double ebeam;
   double beam_Z;
   double beam_A;
   double distance;
   double dethick;//in microns
   double ethick;//in microns
   double deangle;//angle from upstream beam axis
   double deextradist;
   double silicondensity;//g/cm3


   Si_calib(double inebeam,double inZbeam,double inAbeam,double indistance,target intarg, TTree * /*tree*/ =0) :
		fChain(0),
		ebeam(inebeam),
		beam_Z(inZbeam),
		beam_A(inAbeam),
		distance(indistance),
		dethick(75),
		ethick(300),
		deangle(0.8342674),
		deextradist(62.5),
		silicondensity(2.3290)
		{		
			targ = intarg;
			
			for(int i=0;i<44;i++){
				silicon_data_p.push_back(new Int_t);
				silicon_data.push_back(0);
			}
			
			for(int i=0;i<16;i++){
				stringstream ii;
				ii<<(i+1);
				
				string titleA="EA_"+ii.str();
				string titleB="EA_"+ii.str()+"_with_dE";
				
				E_graphs.push_back(new TH1D(titleA.c_str(),titleA.c_str(),8000,0,8000));	
				Ed_graphs.push_back(new TH1D(titleB.c_str(),titleB.c_str(),8000,0,8000));	
			}
			for(int i=16;i<32;i++){
				stringstream ii;
				ii<<(i-15);
				
				string titleA="EB_"+ii.str();
				string titleB="EB_"+ii.str()+"_with_dE";
				
				E_graphs.push_back(new TH1D(titleA.c_str(),titleA.c_str(),8000,0,8000));	
				Ed_graphs.push_back(new TH1D(titleB.c_str(),titleB.c_str(),8000,0,8000));	
			}			
			
					
			for(int j=0;j<12;j++){
				stringstream jj;
				jj<<(j+1);
				string titleC="dE"+jj.str()+"_sum";
				dE_sums.push_back(new TH1D(titleC.c_str(),titleC.c_str(),8000,0,8000));

				dE_graphs.push_back(vector< TH1D* >(0) );
				for(int i=0;i<16;i++){
					stringstream ii;
					ii<<(i+1);
					string titleD="dE"+jj.str()+"_with_E"+ii.str();
					dE_graphs[j].push_back(new TH1D(titleD.c_str(),titleD.c_str(),8000,0,8000));
				}
			}			
	}
	
   virtual ~Si_calib() { }
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

   ClassDef(Si_calib,0);
};

#endif

#ifdef Si_calib_cxx
void Si_calib::Init(TTree *tree)
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
}

Bool_t Si_calib::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.
	if(fChain->GetTree())cout<<endl<<"Opening to process : "<<fChain->GetCurrentFile()->GetName();
	if(fChain->GetTree())cout<<endl<<"TTree : "<<fChain->GetTree()->GetName()<<" "<<fChain->GetTree()->GetEntries()<<" entries."<<endl;

   return kTRUE;
}

#endif // #ifdef Si_calib_cxx

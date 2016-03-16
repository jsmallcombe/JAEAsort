//
//
//	james_fission_master_selector 2.0
// 	27/02/09		09/02/2015
//	james.smallcombe@outlook.com
//
//


// The class definition in master_selector.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("master_selector.C")
// Root > T->Process("master_selector.C","some options")
// Root > T->Process("master_selector.C+")
//


#define master_selector_cxx
#include "james_selector.h"


void master_selector::cal_check(TTree *tree){
		
	if(tcal){
		cout<<endl<<"Raw tzero mean:"<<endl;
		TCanvas *c1=new TCanvas();
		TH1D* h1 = new TH1D("h1","h1",8000,0,8000);
		tree->Draw("dT_A_B>>+h1","dT_A_B>0","");
		tzeroAB=TimegraphAB->Eval(h1->GetMean());
		cout<<"tzeroAB "<<tzeroAB<<endl;
		if(detconfig==0){
			h1->Reset();
			tree->Draw("dT_B_C>>+h1","dT_B_C>0","");
			tzeroBC=TimegraphBC->Eval(h1->GetMean());
			cout<<"tzeroBC "<<tzeroBC<<endl;
			h1->Reset();
			tree->Draw("dT_A_D>>+h1","dT_A_D>0","");
			tzeroAD=TimegraphAD->Eval(h1->GetMean());
			cout<<"tzeroAD "<<tzeroAD<<endl;
		}
		delete h1;
		delete c1;
	}
	
}


void master_selector::Begin(TTree *tree /*tree*/)
{
	////////////////////////////////////////////////////
	// Create the output file and root tree
	// (want to be in an open file when creating ttree, for autosave and memory)
	////////////////////////////////////////////////////
	
	string tmp= tree->GetName();
	output_file= new TFile((output+"_"+tmp+".root").c_str(),"RECREATE");
	cout<<endl<<"Output file : "<< output+"_"+tmp+".root"<<endl;
	if(!tcal){
		outtree = new TTree("outtree","outtree");
		
		// Set the address for branches in output ttree
		outtree->Branch("MWPC_dT_raw",&MWPC_dT_raw);
		outtree->Branch("MWPC_dT_cal",&newT);
		outtree->Branch("MWPC1_cat",&MWPC1_cat);	
		outtree->Branch("MWPC2_cat",&MWPC2_cat);
		outtree->Branch("MWPC1_theta", &theta1);
		outtree->Branch("MWPC1_phi", &phi1);
		outtree->Branch("MWPC2_theta", &theta2);
		outtree->Branch("MWPC2_phi", &phi2);
		outtree->Branch("d_theta", &d_theta);
		outtree->Branch("d_phi", &d_phi);
		
		outtree->Branch("x_diff", &x_diff);
		outtree->Branch("massA", &massA);
		outtree->Branch("massB", &massB);
		outtree->Branch("sumKE_com", &sumKE_com);
		outtree->Branch("worldhit1", &worldhit1);	
		outtree->Branch("worldhit2", &worldhit2);
		if(detconfig==0)outtree->Branch("AD_BC_swit", &AD_BC_swit);
	}
	
	
	////////////////////////////////////////////////////		
	// Next create the histograms
	////////////////////////////////////////////////////
	output_file->mkdir("Hits");
	output_file->cd("Hits");
		hit_patern_1= new TH2D("MWPC_1_hits","MWPC_1_hits",500,-250,250,300,-150,150);
		hit_patern_2= new TH2D("MWPC_2_hits","MWPC_2_hits",500,-250,250,300,-150,150);
		if(detconfig==0){ 	hit_patern_3= new TH2D("MWPC_3_hits","MWPC_3_hits",500,-250,250,300,-150,150);
					hit_patern_4= new TH2D("MWPC_4_hits","MWPC_4_hits",500,-250,250,300,-150,150);}
		threedee= new TH3D("singles_3D","singles_3D",200,-300,300,200,-300,300,200,-200,200);
		threedee->GetYaxis()->SetTitle("Beam Axis");
	output_file->cd();
	
	if(!tcal){
		dangles= new TH2D("FF_folding_angles","FF_folding_angles",100,2.0,4.0,100,-0.5,0.5);
		massQ= new TH2D("FF_mass_vs_TKE","FF_mass_vs_TKE",100,0,200,100,0,500);
		dthetagr= new TH1D("FF_theta_folding","FF_theta_folding",100,2.0,4.0);
		massca= new TH1D("FF_mass","FF_mass",100,50,150);
		
		if(detconfig==0){ 
			massAB= new TH1D("FF_mass_AB","FF_mass_AB",100,0,200);	
			massAD= new TH1D("FF_mass_AD","FF_mass_AD",100,0,200);
			massBC= new TH1D("FF_mass_BC","FF_mass_BC",100,0,200);
			massABCD[0]=massAB;massABCD[1]=massAD;massABCD[2]=massBC;
		}
		mass_delta= new TH1D("FF_detector_asymetry","FF_detector_asymetry",100,0,200);
		
		
		ternary_mom= new TH3D("ternary_mom","ternary_mom",200,0,5000,200,0,5000,200,0,5000);
	}else{
		
		//If time calibrating

		for(int i=0;i<cn;i++){
			double up=(*tzero[i])+20;
			double down=(*tzero[i])-20;
			ABCD_heavy[i]= new TH2D((ABCDstring[i]+"_heavy").c_str(),(ABCDstring[i]+"_heavy").c_str(),200,5,masszero,500,down,up);
			ABCD_light[i]= new TH2D((ABCDstring[i]+"_light").c_str(),(ABCDstring[i]+"_heavy").c_str(),200,5,masszero,500,down,up);
			ABCD_sum[i]= new TH2D((ABCDstring[i]+"_sum").c_str(),(ABCDstring[i]+"_sum").c_str(),200,5,masszero,500,down,up);
			ABCD_delta[i]= new TH2D((ABCDstring[i]+"_delta").c_str(),(ABCDstring[i]+"_heavy").c_str(),200,5,masszero,500,down,up);
		}

	}
	
	////////////////////////////////////////////////////
	//	If using dE E silicon load the silicon calibration data	and add branches
	////////////////////////////////////////////////////
	if(silicon_array_distance>0&&!tcal){
		
		outtree->Branch("si_E_tot",&siliconenergy);
		outtree->Branch("si_dE_dX",&silicondelta);
		outtree->Branch("worldhit3", &worldhit3);
		outtree->Branch("si_dt", &si_dt);
		outtree->Branch("MWPC_si_dt", &MWPC_si_dt);
		
		silicon_view = new TH2D("silicon_view","silicon_view",1000,-100,100,1000,-100,100);
		silicon_dE_E= new TH2D("silicon_dEdx_E","silicon_dEdx_E",600,0,60,500,0,25);
		
		TFile *f = new TFile(si_cal_file.c_str());
		TTree *t = (TTree*)f->Get("si_calib");
		double qu=0,gr=0,ince=0,inanf=0.8342674;
		TBranch *aa = t->GetBranch("quad");
		if(aa)aa->SetAddress(&qu);
		TBranch *bb = t->GetBranch("grad");
		bb->SetAddress(&gr);
		TBranch *cc = t->GetBranch("intercept");
		cc->SetAddress(&ince);
		TBranch *dd = t->GetBranch("deangle");
		if(dd)dd->SetAddress(&inanf);
		
		quad_cal.resize(0);
		grad_cal.resize(0);
		intercept_cal.resize(0);
		deangle_cal.resize(0);
		
		Int_t nEnt = t->GetEntries();
		for (Int_t i=0;i<nEnt;++i) {
			t->GetEntry(i);
			if(aa)quad_cal.push_back(qu);else quad_cal.push_back(0);//can use old cal files without quad term
			grad_cal.push_back(gr);
			intercept_cal.push_back(ince);
			if(dd)deangle_cal.push_back(inanf);else deangle_cal.push_back(0.8342674);
		}	
		f->Close();
	}
	
}

void master_selector::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t master_selector::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either master_selector::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.

	//USEFUL LINE WHEN PROCESSING LARGE DATA SETS TO HAVE AN OUTPUT	
	if(entry%1000 == 0|| entry==nentries-1) 
	cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)entry/nentries << " % complete" << "\r" << flush;  
	
	GetEntry(entry);//core line to get the data!
	
	/////////////////////////////////
	//////// GENERAL SETUP   ////////
	/////////////////////////////////	
	
	//set the actual time max from event construction pre-sort.
	if(MWPC1_y_time>time_max-2)time_max=MWPC1_y_time+2;

	// Manually reset some branches & variables
	bool det1=false,det2=false,det3=false,det4=false;
	AD_BC_swit=-1;
	newT=0;
	double MWPC_time=time_max;
	
	if(silicon_array_distance>0){
		siliconenergy=0;
		silicondelta=0;
		worldhit3=TVector3(0,0,0);
		si_dt=time_max*2;
		MWPC_si_dt=time_max*2;
	}
	
	/////////////////////////////////
	////////DO MWPC SINGLES/////////
	/////////////////////////////////
		
	if(MWPC1_cat>1){//makes a hit and checks validity
		if(detconfig>=3)dethit1=make_det_position_new(det1,mwpc1chan,MWPC1_x1,MWPC1_x2,MWPC1_y);
		else dethit1=make_short_position_new(det1,mwpc1chan,MWPC1_x1,MWPC1_y);
		if(det1){
			hit_patern_1->Fill(dethit1.X(),dethit1.Z()); //fill detector hit
			worldhitA = ( det1rot * dethit1 ) + det1pos; //rotate and position in world
			threedee->Fill(worldhitA.X(),worldhitA.Z(),worldhitA.Y()); //fill 3D
		}
	}
	if(MWPC2_cat>1){//makes a hit and checks validity
		if(detconfig>=3)dethit2=make_det_position_new(det2,mwpc2chan,MWPC2_x1,MWPC2_x2,MWPC2_y);
		else dethit2=make_short_position_new(det2,mwpc2chan,MWPC2_x1,MWPC2_y);
		if(det2){
			hit_patern_2->Fill(dethit2.X(),dethit2.Z());
			worldhitB = ( det2rot * dethit2 ) + det2pos;
			threedee->Fill(worldhitB.X(),worldhitB.Z(),worldhitB.Y());	
		}    
	}
	if(MWPC3_cat>1&&detconfig==0){
		dethit3=make_short_position_new(det3,mwpc3chan,MWPC3_x1,MWPC3_y);
		if(det3){
			hit_patern_3->Fill(dethit3.X(),dethit3.Z());
			worldhitC = ( det3rot * dethit3 ) + det3pos;
			threedee->Fill(worldhitC.X(),worldhitC.Z(),worldhitC.Y());	
		}    
	}

	if(MWPC4_cat>1&&detconfig==0){
		dethit4=make_short_position_new(det4,mwpc4chan,MWPC4_x1,MWPC4_y);
		if(det4){
			hit_patern_4->Fill(dethit4.X(),dethit4.Z());
			worldhitD = ( det4rot * dethit4 ) + det4pos;
			threedee->Fill(worldhitD.X(),worldhitD.Z(),worldhitD.Y());
		}    
	}

	
	///////////////////////////////
	//////DO SILICON DET /////////
	///////////////////////////////
	
	if(silicon_array_distance>0&&!tcal){
		
		//
		//	CALIBRATE SILICON DATA
		//
		
		for(int i=0;(unsigned)i<silicon_time_p.size();i++){
			silicon_time[i]=*silicon_time_p[i];
			silicon_data[i]=*silicon_data_p[i];//tranfer tree pointers to local variables
			if(silicon_data[i]>5)silicon_data[i]=(silicon_data[i]*quad_cal[i])+(silicon_data[i]*grad_cal[i])+intercept_cal[i];//calibrate only nonzeroes
			if(silicon_data[i]>50)silicon_data[i]=0; //cut off for giberish
		}
		silicon_data[42]=0;// turn off missing dE detectors
		silicon_data[36]=0;
		
		// This is the silicon event constructor, very basic class which holds all the dE E combinations
		silicon=silicon_hit(silicon_data,silicon_time);
		silicon.setworld(TVector3(0,0,silicon_array_distance));
		
		//
		//	Do silicon singles
		//
		
		for(int k=0;k<silicon.mult;k++){
			worldhit3=silicon.worldhit[k];
			double thickness_dE=(1/sin(pi-(deangle_cal[silicon.pixphi[k]-1]+worldhit3.Theta())));
				
			siliconenergy=silicon.E[k]+silicon.dE[k];
			silicondelta=silicon.dE[k]/thickness_dE;
			si_dt=silicon.EdEt[k];

			silicon_dE_E->Fill(siliconenergy,silicondelta);
			silicon_view->Fill(silicon.locald[k].X(),silicon.locald[k].Y());
			threedee->Fill(silicon.worldd[k].X(),silicon.worldd[k].Z(),silicon.worldd[k].Y());

			//outtree->Fill();
		}
		
		
	}
	
	
	//////////////////////////////////////
	//////////////////////////////////////
	//////// CHECK FISSION PAIRS /////////
	//////////////////////////////////////
	//////////////////////////////////////
	
	if(det1&&det2&&dT_A_B>5){// If there is and AB fission pair
		worldhit1=worldhitA; // Make them the two fragments
		worldhit2=worldhitB;
		MWPC_dT_raw=dT_A_B; // Set FF ToF raw
		AD_BC_swit=0;// Set the switch to know the detector pair
		MWPC_time=MWPC1_cat_time;
	}
	
	if(det2&&det3&&dT_B_C>5){
		worldhit1=worldhitB;
		worldhit2=worldhitC;	
		MWPC_dT_raw=dT_B_C;
		AD_BC_swit=1;
		MWPC_time=MWPC2_cat_time;
	}
	
	
	if(det1&&det4&&dT_A_D>5){
		worldhit1=worldhitA;
		worldhit2=worldhitD;
		MWPC_dT_raw=dT_A_D;
		AD_BC_swit=2;
		MWPC_time=MWPC1_cat_time;
	}
	
	// Currently only one of the three combinations can be valid at once, that is not currently a physical limitation
	
	////////////////////////////////////////////////
	//////// SORT BINARY FISSION AND TREE //////////
	////////////////////////////////////////////////
	if(!tcal){
		if(AD_BC_swit>=0){// If there was a good fission event in any detector combo	
	
			///////////////////////////////////
			// First Calc and fill angles   ///
			///////////////////////////////////
			
			theta1=worldhit1.Theta();theta2=worldhit2.Theta();
			phi1=happy_phi(worldhit1);phi2=happy_phi(worldhit2);// I dont like the phi range ROOT uses  
			d_theta=theta1+theta2;
			d_phi=angledifference_signed(phi1,phi2-TMath::Pi());
			dangles->Fill(d_theta,d_phi);
			dthetagr->Fill(d_theta);
			x_diff=worldhit1.Mag()-worldhit2.Mag();	// mm difference in flight path
			
			////////////////////////////////
			//////  MASS CALCULATION   ////
			///////////////////////////////
			
			newT=TimegraphAD->Eval(MWPC_dT_raw)-*tzero[AD_BC_swit];// Convert DAQ ToF and zero adjust	
			newT*=-1E-9;//convert to seconds
			
			// 3 modes
			// Calculate mass in an interative way to account for energy loss and mass excess
			// Calculate mass in an interative way to account for energy loss
			// Calculate mass in a quick simplified way
			
			// Currently only does beam-target fusion-fission 
			// This can be easily changed by changing the inputs to mass_split_calc() 
			
			double* masstrans;
			if(calculation_version>1)masstrans= mass_split_calc(worldhit1,worldhit2,TVector3(0,0,pbeam_slow),newT,targ,masszero,15,mass_adj_graph);
			else if(calculation_version>0)masstrans= mass_split_calc(worldhit1,worldhit2,TVector3(0,0,pbeam_slow),newT,targ,masszero,15);
			else masstrans= mass_split_calc(worldhit1,worldhit2,TVector3(0,0,pbeam_slow),newT,targ,masszero,0);
			massA=masstrans[0];
			massB=masstrans[1];
			delete masstrans;
			massca->Fill(massA);
			massca->Fill(massB);
			mass_delta->Fill(massA);
			mass_delta->Fill(massB,-1);			

			if(detconfig==0){ 
				massABCD[AD_BC_swit]->Fill(massA);
				massABCD[AD_BC_swit]->Fill(massB);
			}

			///////////////////////////////////////////////////
			//////  With masses calculated, calculated Q   ////
			///////////////////////////////////////////////////
			
			double mom1=pbeam_slow*((1/(1+(TMath::Tan(theta1)/TMath::Tan(theta2))))/TMath::Cos(theta2));// Momentum split
			double mom2=pbeam_slow*((1/(1+(TMath::Tan(theta2)/TMath::Tan(theta1))))/TMath::Cos(theta2));
			TLorentzVector lorve1= make_lorentzvec(worldhit1.Unit()*mom1,massA);// Boost to reaction frame
			TLorentzVector lorve2= make_lorentzvec(worldhit2.Unit()*mom2,massB);// Boost to reaction frame
			lorve1.Boost(0,0,-beta_cm);
			lorve2.Boost(0,0,-beta_cm);
			sumKE_com=get_KE(&lorve1)+get_KE(&lorve2);//TKE calc
			
			if(sumKE_com>500)sumKE_com=0;//Spurious resuls make drawing difficult so I kill them
			else {massQ->Fill(massA,sumKE_com);massQ->Fill(massB,sumKE_com);}

			//////////////////////////////////////////////
			// Do the fission coincident silicon events //
			//////////////////////////////////////////////
			
			// There is some repeated code in here from silicon singles
			// I havent modified the "silicon_hit" class since adding the thickness_dE adjustment
			
			if(silicon_array_distance>0){			
				if(silicon.mult>0){
					for(int k=0;k<silicon.mult;k++){
						worldhit3=silicon.worldhit[k];
						double thickness_dE=(1/sin(pi-(deangle_cal[silicon.pixphi[k]-1]+worldhit3.Theta())));
							
						siliconenergy=silicon.E[k]+silicon.dE[k];
						silicondelta=silicon.dE[k]/thickness_dE;
						si_dt=silicon.EdEt[k];

						silicon_dE_E->Fill(siliconenergy,silicondelta);
						silicon_view->Fill(silicon.locald[k].X(),silicon.locald[k].Y());
						threedee->Fill(silicon.worldd[k].X(),silicon.worldd[k].Z(),silicon.worldd[k].Y());

						MWPC_si_dt=silicon.timeE[k]-MWPC_time;
						outtree->Fill();
					}
				}else	outtree->Fill();
			}else	outtree->Fill();
		}
			
		// In this version the tree is only filled for fission pairs
		// If you want a raw tree or silicon tree you must RESET THE BRANCH VARIABLES manually on each loop
		// outtree->Fill();	
		
		//////////////////////////////////////////
		//////// LOOK AT ternary EVENTS //////////
		//////////////////////////////////////////
		ternary=0;	
		
		//look at the 4 possible combinations
		if(det1&&det2&&det3&&!det4&&dT_B_C>5&&dT_A_B>5){
			ternary=1;
			triA=worldhitA;	triB=worldhitB;	triC=worldhitC;
			tri_dt_AB=TimegraphAB->Eval(dT_A_B)-tzeroAB;
			tri_dt_BC=TimegraphBC->Eval(dT_B_C)-tzeroBC;
			tri_dt_CA=-tri_dt_AB-tri_dt_BC;
		}
		
		if(det1&&det2&&!det3&&det4&&dT_A_D>5&&dT_A_B>5){
			ternary=2;
			triA=worldhitA;	triB=worldhitB;	triC=worldhitD;
			tri_dt_AB=TimegraphAB->Eval(dT_A_B)-tzeroAB;
			tri_dt_CA=-TimegraphAD->Eval(dT_A_D)-tzeroAD;
			tri_dt_BC=-tri_dt_AB-tri_dt_CA;
		}
		
		// THESE TWO ARE NOT POSSIBLE BECAUSE OF NO TIMING DATA BETWEEN D and C
	// 	if(det1&&!det2&&det3&&det4&&dT_A_D>5){
	// 		ternary=3;
	// 	}	
	// 	if(!det1&&det2&&det3&&det4&&dT_B_C>5){
	// 		ternary=4;
	// 	}
		
		//if any of the combinations are valid we can analyse them
		if(ternary>0){	
			//This momentum split function assumes 3 perfect vectors that ARE from a ternary reaction
			//results may easily be unphysical so be careful
			vector< double > mom_tri = mom_conserve_split(TVector3(0,0,pbeam_slow),triA,triB,triC);
			ternary_mom->Fill(mom_tri[0],mom_tri[1],mom_tri[2]);
			
			// Here one could use the 3 time differences to calculate the masses (double check the above timing calcs)
			// A second TTree for ternary events could be filled here in parrallel with the binary tree
		}
	}else{

		//time calibration
		if(AD_BC_swit>=0){
			double TT=TimegraphAD->Eval(MWPC_dT_raw);
	
			for(int i=1;i<ABCD_heavy[AD_BC_swit]->GetYaxis()->GetNbins()+1;i++){
				double tz=ABCD_heavy[AD_BC_swit]->GetYaxis()->GetBinCenter(i);
				newT=TT-tz;
				newT*=-1E-9;//convert to seconds
				
				double* masstrans;
				if(calculation_version>1)masstrans= mass_split_calc(worldhit1,worldhit2,TVector3(0,0,pbeam_slow),newT,targ,masszero,15,mass_adj_graph);
				else if(calculation_version>0)masstrans= mass_split_calc(worldhit1,worldhit2,TVector3(0,0,pbeam_slow),newT,targ,masszero,15);
				else masstrans= mass_split_calc(worldhit1,worldhit2,TVector3(0,0,pbeam_slow),newT,targ,masszero,0);
				massA=masstrans[0];
				massB=masstrans[1];
				delete masstrans;				
				
				if(massA>massB){
					ABCD_heavy[AD_BC_swit]->Fill(massA,tz,-1);
					ABCD_heavy[AD_BC_swit]->Fill(massB,tz,-1);
					ABCD_delta[AD_BC_swit]->Fill(massA,tz,-1);
					ABCD_delta[AD_BC_swit]->Fill(massB,tz,-1);
				}else{
					ABCD_light[AD_BC_swit]->Fill(massA,tz);
					ABCD_light[AD_BC_swit]->Fill(massB,tz);
					ABCD_delta[AD_BC_swit]->Fill(massA,tz);
					ABCD_delta[AD_BC_swit]->Fill(massB,tz);
				}
				ABCD_sum[AD_BC_swit]->Fill(massA,tz);
				ABCD_sum[AD_BC_swit]->Fill(massB,tz);
			}	
		}
	}
	
	return kTRUE;
}
//////////////////////////////////////
//// end of main event process loop
/////////////////////////////////

void master_selector::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void master_selector::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.
	
	//////////////////////////////////
	//////// TCAL FITTING   //////////
	//////////////////////////////////	
	
	// For each detector pair we have seperated out events in which the heavy fragment goes into the "first detector"
	// We then determine the T_zero value for which these have the maximum overlap
	
	if(tcal){
		for(int i=0;i<cn;i++){
			double best_sum=-1;
			double besttzero=0;		

			for(int j=1;j<ABCD_delta[i]->GetYaxis()->GetNbins()+1;j++){
	
				double row_sum=0;
				for(int k=1;k<ABCD_delta[i]->GetYaxis()->GetNbins()+1;k++){
					row_sum+=abs(ABCD_delta[i]->GetBinContent(k,j));
				}
				
				if(row_sum<best_sum||best_sum<0){
					best_sum=row_sum;
					besttzero=ABCD_delta[i]->GetYaxis()->GetBinCenter(j);
				}
			}	
			cout<<endl<<"Best tzero"<<ABCDstring[i]<<" = "<<besttzero<<flush;
		}
	}
	
	/////////////////////////////////////////
	//////// OUTPUT FILE WRITING   //////////
	/////////////////////////////////////////

	output_file->Write();
	output_file->Close();
}

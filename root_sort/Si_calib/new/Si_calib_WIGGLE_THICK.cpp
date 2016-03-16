#define Si_calib_cxx
#include "Si_calib.h"


void Si_calib::Begin(TTree * tree/*tree*/)
{  TString option = GetOption();
     // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).
	
	
		
	string in_file=tree->GetCurrentFile()->GetName();
	
	string path=in_file.substr(0,in_file.find("si/"));
	
	string filename=in_file.substr(in_file.find("extra"),in_file.length());
	filename=filename.substr(0,filename.find("."));
	
	output = path+"si_cal/"+filename+"_sical.root";
	
	cout<<endl<<"Output file : "<<output<<endl;
	
	
	
	
	output_file = new TFile(output.c_str(),"RECREATE");
	
	
	E_graphs.resize(0);
	Ed_graphs.resize(0);
	
	dE_sums.resize(0);	
	dE_graphs.resize(0);
	dE_2D_graphs.resize(0);	
	


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
		dE_2D_graphs.push_back(vector< TH2D* >(0) );
		for(int i=0;i<16;i++){
			stringstream ii;
			ii<<(i+1);
			string titleD="dE"+jj.str()+"_with_E"+ii.str();
			dE_graphs[j].push_back(new TH1D(titleD.c_str(),titleD.c_str(),8000,0,8000));
			
			string titleE="dE2D"+jj.str()+"_with_E"+ii.str();
			dE_2D_graphs[j].push_back(new TH2D(titleE.c_str(),titleE.c_str(),800,0,8000,1600,0,8000));

		}
		
		
		
	}

	
		
   
}

void Si_calib::SlaveBegin(TTree * /*tree*/)
{ TString option = GetOption();}

Bool_t Si_calib::Process(Long64_t entry)
{
	//USEFUL LINE WHEN PROCESSING LARGE DATA SETS TO HAVE AN OUTPUT
	if(((long)entry/1000000.0)==(int)(entry/1000000))cout<<"."<<flush;
	
	GetEntry(entry);//core line to get the data!

	//FETCH THE DATA
	for(int i=0;(unsigned)i<silicon_data_p.size();i++){
				silicon_data[i]=*silicon_data_p[i];
	}
 
	bool dEA=false,dEB=false;
 
	//Go through dE data holders
	for(int i=32;i<44;i++){
		int j=i-32;
		if(silicon_data[i]>5){//is there data?
			dE_sums[j]->Fill(silicon_data[i]);//add to dE sum hist
			if(j<6)dEA=true;//set flag for E detector
			else dEB=true;
		}
	}

	int A=0,B=0;
	int AA=0,BB=0;
	
	//Go through E detector data holders
	for(int i=0;i<32;i++){
		if(silicon_data[i]>5){//is there data?
			if(i<16){//A side
				if(dEA)Ed_graphs[i]->Fill(silicon_data[i]);//Either fill a dE coincidence or NONcoinc hist
				else E_graphs[i]->Fill(silicon_data[i]);
				A++;
				AA=i;
			}else{//B side
				if(dEB)Ed_graphs[i]->Fill(silicon_data[i]);
				else E_graphs[i]->Fill(silicon_data[i]);
				B++;
				BB=i-16;
			}
		}
	}	
	
 	//Go through dE data holders AGAIN now we have checked E
	for(int i=32;i<44;i++){
		int j=i-32;
		if(silicon_data[i]>5){//is there data?
			if(j<6&&A==1){//has there been EXACTLY one E hit
				dE_graphs[j][AA]->Fill(silicon_data[i]);//fill dE E hists
				dE_2D_graphs[j][AA]->Fill(silicon_data[AA],silicon_data[i]);
			}else if(j>5&&B==1){
				dE_graphs[j][BB]->Fill(silicon_data[i]);
				dE_2D_graphs[j][BB]->Fill(silicon_data[BB+16],silicon_data[i]);
			}	
		}
	}
	
   return kTRUE;
}

void Si_calib::SlaveTerminate()
{}

void Si_calib::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

	//
	//calculate the actual energies
	//
	
	vector< double > angle;
	vector< double > elastic_energy;
	vector< double > dE_thickness;
	vector< double > E_thickness;
	vector< double > dE_loss;
	vector< double > E_loss_with;
	vector< double > E_loss_without;
	
	vector< double > alpha_dE;	
	vector< double > alpha_E;
	vector< double > alpha_EB;
	double alpha_max;


	double* xxx;
	double * tempd= new double(0);
	double centrebeam=targ.beam_e_centre(beam_Z,beam_A,ebeam);
	
	dethick=giveme_areal(silicondensity,dethick);//input density in g/cm3 and thickness in um
	ethick=giveme_areal(silicondensity,ethick);	
	
	// SET CONSTANTS FOR THE REACTION AND ENERGIES	
	
	//GO THROUGH THE E RADIAL ELEMENTS
	for(int i=0;i<16;i++){
		//BEAM THETA ANGLE
		angle.push_back(Atan((23.25+(((double)i+1)*1.5))/distance));
		//BEAM KINEMATICS CALC
		xxx = kinetic_lab_calcs_elastic_E(centrebeam,beam_A,targ.targ_A,angle[i]);
		//ELASTIC SCATTER ENERGY AT THAT ANGLE
		elastic_energy.push_back(targ.particle_e_exit(beam_Z,beam_A,xxx[8],TVector3(tan(angle[i]),0,1)));
		
		//NOMINAL THICKNESS OF dE detector for that angle of beam
		dE_thickness.push_back(dethick/sin(pi-(deangle+angle[i])));
		//NOMINAL THICKNESS OF E detector for that angle of beam
		E_thickness.push_back(ethick/cos(angle[i]));
		dE_thickness[i]*=1.02;//you get a bit more thickness when averaging over phi aswell
		E_thickness[i]*=1.02;
	
		//CALCULATE E LOSS FOR THAT ANGLE OF dE
		dE_loss.push_back(elastic_energy[i]-passage(0,beam_Z,beam_A,0,14,28,elastic_energy[i],dE_thickness[i]/1000,tempd));
		//MAX ENERGY IN E DETECTOR BEFORE PENETRATION
		double E_max=egassap(0,beam_Z,beam_A,0,14,28,E_thickness[i]/1000, 0, tempd);
		double post_dE=elastic_energy[i]-dE_loss[i];
		
		//ENERGY EXPECTED IN E AT THIS ANGLE AFTER dE, EITHER ALL ENERGY OR THE PENETRATION
		if(post_dE<E_max) E_loss_with.push_back(post_dE);
		else E_loss_with.push_back(post_dE-passage(0,beam_Z,beam_A,0,14,28,post_dE,E_thickness[i]/1000,tempd));
		
		//ENERGY EXPECTED IN E AT THIS ANGLE WITHOUT dE, EITHER ALL ENERGY OR THE PENETRATION
		if((elastic_energy[i])<E_max) E_loss_without.push_back(elastic_energy[i]);
		else E_loss_without.push_back(elastic_energy[i]-passage(0,beam_Z,beam_A,0,14,28,post_dE,E_thickness[i]/1000,tempd));
		
		//ENERGY AT WHICH AN ALPHA PENTRATES BOTH DETECTORS
		alpha_max=egassap(0,2,4,0,14,28,(E_thickness[i]+dE_thickness[i])/1000, 0, tempd);
		
		//ENERGY SUCH AN ALPHA DEPOSITS
		alpha_dE.push_back(alpha_max-passage(0,2,4,0,14,28,alpha_max,dE_thickness[i]/1000,tempd));
		alpha_E.push_back(alpha_max-alpha_dE[i]);
		alpha_EB.push_back(alpha_E[i]);
		//cout<<endl<<i+1<<" "<<angle[i]*180/pi<<" "<<alpha_dE[i]<<" "<<alpha_E[i];

	}


	//function used for peak fitting throughout
	TF1* b_guas = new TF1("general_peak","gaus(0)");
	//const//mean//sigma
	b_guas->SetParameter(2,50);//set sigma estimate
	b_guas->SetParLimits(2,5,200);//set sigma limits
	

	///////////////////////////////////////////////////////////
	///////////// Fit basic the histograms
	///////////////////////////////////////////////////////////
	// Start with dE sums over all angles
	vector< double > dE_sumpeaks;
	for(int i=0;i<12;i++){
		//zero value if nothing good
		dE_sumpeaks.push_back(0);
		
		if(dE_sums[i]->GetEntries()>500){//dont bother for less than 100 counts	
				
			//start by finding what is probably the peak as it should be the max
			int maxbin=dE_sums[i]->GetMaximumBin();
			int counts=dE_sums[i]->GetBinContent(maxbin);
			
			//set parameters as expected
			b_guas->SetParameter(0,counts);
			b_guas->SetParLimits(0,counts*0.1,counts*10);
			b_guas->SetParameter(1,maxbin);
			b_guas->SetParLimits(1,maxbin-50,maxbin+50);
			b_guas->SetParameter(2,50);
			b_guas->SetRange(maxbin-200,maxbin+200);//only fit near the max, dont get confused
			
			//first rough fit
			dE_sums[i]->Fit(b_guas,"RNQWW");//RANGE Not_add Quiet 
			//set range just on the peak tip
			b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));
			//fit agiain for accuracy, this time add to hist
			dE_sums[i]->Fit(b_guas,"RQ");
			//integrals are in bin number, but here bin and rangeuser are 1to1
			double histfitarea=dE_sums[i]->Integral(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));
			if(histfitarea>400)dE_sumpeaks[i]=b_guas->GetParameter(1);//only use if enough data
		}
	}

	// Now fit E detectors.
	vector< double > E_withdEpeaks;
	vector< double > E_peaks;
	for(int i=0;i<32;i++){
		E_withdEpeaks.push_back(0);
		E_peaks.push_back(0);
		int j=i;
		if(i>15)j=i-16;//because the energy lists were made only for angle so reused for A and B sides
		
		
		// Now fit all E detectors where there WAS a dE penetration.
		if(Ed_graphs[i]->GetEntries()>500){
			int maxbin=Ed_graphs[i]->GetMaximumBin();
			int counts=Ed_graphs[i]->GetBinContent(maxbin);
			
			b_guas->SetParameter(0,counts);
			b_guas->SetParLimits(0,counts*0.1,counts*10);
			b_guas->SetParameter(1,maxbin);
			b_guas->SetParLimits(1,maxbin-50,maxbin+50);
			b_guas->SetParameter(2,50);
			b_guas->SetRange(maxbin-200,maxbin+200);	
			Ed_graphs[i]->Fit(b_guas,"RNQWW");
			b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));
			Ed_graphs[i]->Fit(b_guas,"RQ");
			
			double histfitarea=Ed_graphs[i]->Integral(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));
			if(histfitarea>400)E_withdEpeaks[i]=b_guas->GetParameter(1);//only use if enough data	
		}



		// Now fit all E detectors where there WAS NOT a dE penetration.
		if(E_graphs[i]->GetEntries()>500){
			//WE KNOW THE PEAK MUST RELATIVE POS TO THE PREVIOUS ONE, SO SET RANGE BEFORE FINDING MAX
			if(E_loss_without[j]>E_loss_with[j])E_graphs[i]->GetXaxis()->SetRange(Ed_graphs[i]->GetMaximumBin()+80,8000);
			else E_graphs[i]->GetXaxis()->SetRange(1,Ed_graphs[i]->GetMaximumBin()-80);
			
			if(E_graphs[i]->Integral()>500){

				int maxbin=E_graphs[i]->GetMaximumBin();
				int counts=E_graphs[i]->GetBinContent(maxbin);
							
				b_guas->SetParameter(0,counts);
				b_guas->SetParLimits(0,counts*0.1,counts*10);
				b_guas->SetParameter(1,maxbin);
				b_guas->SetParLimits(1,maxbin-50,maxbin+50);
				b_guas->SetParameter(2,50);
				b_guas->SetRange(maxbin-200,maxbin+200);
				E_graphs[i]->Fit(b_guas,"RNQWW");
				b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
				E_graphs[i]->Fit(b_guas,"RQ");	
				
				double histfitarea=E_graphs[i]->Integral(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));
				if(histfitarea>400)E_peaks[i]=b_guas->GetParameter(1);//only use if enough data	
			}
		}
	
	}		
	
	
	
	//FIT THE MANY dE E combinations
	vector< vector< double > > dE_peaks;
	for(int j=0;j<12;j++){

		dE_peaks.push_back( vector<double>(0) );
	
		vector< double > peakareacheck;
		for(int i=0;i<16;i++){
			dE_peaks[j].push_back(0);
			peakareacheck.push_back(0);
			
			if(dE_graphs[j][i]->GetEntries()>500){
				int maxbin=dE_graphs[j][i]->GetMaximumBin();
				int counts=dE_graphs[j][i]->GetBinContent(maxbin);
			
				b_guas->SetParameter(0,counts);
				b_guas->SetParLimits(0,counts*0.1,counts*10);
				b_guas->SetParameter(1,maxbin);
				b_guas->SetParLimits(1,maxbin-50,maxbin+50);
				b_guas->SetParameter(2,50);
				b_guas->SetRange(maxbin-200,maxbin+200);
				
				dE_graphs[j][i]->Fit(b_guas,"RNQWW");
				b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
				dE_graphs[j][i]->Fit(b_guas,"RQ");
				
				double histfitarea=dE_graphs[j][i]->Integral(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));
				peakareacheck[i]=histfitarea;
				
				if(histfitarea>400)dE_peaks[j][i]=b_guas->GetParameter(1);//only use if enough data
			}
		}
		
		for(int i=0;i<15;i++){//add this check for the occastions where you have shielding of rings and weird second peak effects
			if(peakareacheck[i+1]>peakareacheck[i]*2)dE_peaks[j][i]=0;
			if(peakareacheck[i+1]*2<peakareacheck[i]){dE_peaks[j][i+1]=0;peakareacheck[i+1]=peakareacheck[i]*0.8;}
		}	

	}	

	///////////////////////////////////////////////////////////
	///////////// SAVE HISTOGRAMS TO FILE
	///////////////////////////////////////////////////////////	
	
	output_file->cd();
	output_file->mkdir("E_no_dE");
	output_file->cd("E_no_dE");
	
		for(int i=0;i<32;i++){//rest axis we moves
			E_graphs[i]->GetXaxis()->SetRange(1,8000);
			E_graphs[i]->Write();	
		}
	
	output_file->cd();
	output_file->mkdir("E_with_dE");
	output_file->cd("E_with_dE");
	
		for(int i=0;i<32;i++){
			Ed_graphs[i]->Write();	
		}
		
	output_file->cd();
	output_file->mkdir("dE_sums");
	output_file->cd("dE_sums");		
	
		
		for(int i=0;i<12;i++){
			dE_sums[i]->Write();	
		}
		
	output_file->cd();
	output_file->mkdir("dE_individual");
	output_file->cd("dE_individual");					
	
	for(int j=0;j<12;j++){

		stringstream ii;
		ii<<(j+1);
		string titleD="dE_individual/dE"+ii.str();	
		
		output_file->mkdir(titleD.c_str());
		output_file->cd(titleD.c_str());
		
		for(int i=0;i<16;i++){
			dE_graphs[j][i]->Write();
		}
		
		output_file->cd("dE_individual");					
	}	



	///////////////////////////////////////////////////////////
	///////////// CALIBRATION FROM ABOVE PEAK FITS
	///////////////////////////////////////////////////////////	
	
	
	//create a TTree to hold the calib results
	output_file->cd();
	double grad, intercept, quad,bestangle;
	TTree *t = new TTree("si_calib","si_calib");
	t->Branch("quad",&quad);
	t->Branch("grad",&grad);
	t->Branch("intercept",&intercept);
	t->Branch("deangle",&bestangle);


	//basic fitting funcitons
	TF1* b_line = new TF1("general_line","pol1(0)");
	b_line->SetRange(1,8000);
	

	TF1* q_line = new TF1("quad_line","pol2(0)");
	q_line->SetRange(1,8000);
	q_line->SetLineColor(1);	
	
	
	///////////////////////////////////////////////////////////
	///////////// CALIB E DETECTOR
	///////////////////////////////////////////////////////////	
	for(int i=0;i<32;i++){
		int j=i;
		if(i>15)j=i-16;//because the energy lists were made only for angle so reused for A and B sides
		
		quad=0;//just linear
		grad=0;
		intercept=0;
		bestangle=deangle;
		
		//A VERY BASIC 2 POINT FIT WHICH ASSUMES EVERYTHING IS STRAIGHT AND TRUE
		if(E_peaks[i]>0&&E_withdEpeaks[i]>0){
			grad=(E_loss_without[j]-E_loss_with[j])/(E_peaks[i]-E_withdEpeaks[i]);
			intercept=E_withdEpeaks[i]*grad;
			intercept=E_loss_with[j]-intercept;
		}
// 		cout<<endl<<"E"<<i+1<<" "<<" y="<<grad<<"*x+"<<intercept;
		t->Fill();//store
		
		//USING THIS FRESH CALIBRATION THE ENERGY WHICH A DOUBLE PENETRATING ALPHA LEAVES IN THE E DETECTOR IS CONVERTED TO A CHANNEL NUMBER
		if(i>15)alpha_EB[j]=(alpha_EB[j]-intercept)/grad;
		else alpha_E[j]=(alpha_E[j]-intercept)/grad;
	}
	
	///////////////////////////////////////////////////////////
	///////////// Create alpha plots and peaks
	///////////////////////////////////////////////////////////	
	
	vector< vector< double > > alpha_peaks;
	for(int i=0;i<12;i++){//loop over 12 dE
		alpha_peaks.push_back(vector< double >(0));
		stringstream ii;
		ii<<(i+1);
		string titley="dE_individual/dE"+ii.str()+"_alpha";
		output_file->mkdir(titley.c_str());
		
		for(int j=0;j<16;j++){//WE ONLY LOOK AT THE MOST INNER FEW STRIPS
			alpha_peaks[i].push_back(0.0);
		
			//if an elastic is rubish no point even looking at that strips alphas
			if(dE_peaks[i][j]>0){
				//TAKE THE 2D dE - E CHANNEL SPECTRA AND CUT THE E AXIS TO ONLY THE ALPHA PENETRATION ENERGY REGION
				//USING OUR EARLIER E CALIBRATION
				if(i<6)dE_2D_graphs[i][j]->GetXaxis()->SetRangeUser(alpha_E[i]*0.9,alpha_E[i]*1.2);
				else dE_2D_graphs[i][j]->GetXaxis()->SetRangeUser(alpha_EB[i]*0.9,alpha_EB[i]*1.2);
				
				//SET THE dE AXIS TO AN APROXIMATE RANGE
				//double alpha_aprox=(alpha_dE[j]/dE_loss[j])*dE_peaks[i][j];
				double nagrad=(dE_loss[2])/(dE_sumpeaks[i]);
				double alpha_aprox=alpha_dE[j]/nagrad;
				dE_2D_graphs[i][j]->GetYaxis()->SetRangeUser(alpha_aprox*0.6,alpha_aprox*1.3);				
				//dE_2D_graphs[i][j]->GetYaxis()->SetRangeUser(dE_peaks[i][j]*0.2,dE_peaks[i][j]*0.7);				
				
				//create the histogram from projection
				stringstream jj;
				jj<<(j+1);
				string titlex="Alpha_dE"+ii.str()+"_E"+jj.str();
				output_file->cd(titley.c_str());
				TH1D* proj = dE_2D_graphs[i][j]->ProjectionY(titlex.c_str());
						
				if(proj->GetEntries()>500){

					//BASIC FIT LOOP
					int maxbin=proj->GetBinCenter(proj->GetMaximumBin());
					int counts=proj->GetBinContent(maxbin);
					b_guas->SetParameter(0,counts);
					b_guas->SetParLimits(0,counts*0.1,counts*10);
					b_guas->SetParameter(1,maxbin);
					b_guas->SetParLimits(1,maxbin-50,maxbin+50);
					b_guas->SetParameter(2,50);
					b_guas->SetRange(maxbin-30,maxbin+30);
					proj->Fit(b_guas,"RQN");
					b_guas->SetRange(b_guas->GetParameter(1)-3*b_guas->GetParameter(2),b_guas->GetParameter(1)+3*b_guas->GetParameter(2));		
					proj->Fit(b_guas,"RQ");

					double peak = b_guas->GetParameter(1);		
					double tester =peak*nagrad;
					int up=proj->FindBin(b_guas->GetParameter(1)+b_guas->GetParameter(2));
					int down=proj->FindBin(b_guas->GetParameter(1)-b_guas->GetParameter(2));
					double histfitarea=proj->Integral(down,up);
					
					if(tester>alpha_dE[j]*0.85&&tester*0.85<alpha_dE[j]&&histfitarea>300){
						alpha_peaks[i][j]=peak;
						//graph->SetPoint(graph->GetN(),peak,alpha_dE[j]);
					}
				}
				proj->Write();
			}
		}
	}	
	
	///////////////////////////////////////////////////////////
	///////////// CALIBRATION FROM ABOVE PEAK FITS
	/////////////  CALIB  dE DETECTOR
	///////////////////////////////////////////////////////////
	
	output_file->mkdir("dE_graphs");
	output_file->cd("dE_graphs");					

	for(int i=0;i<12;i++){//loop over 12 dE
		
		//a very rough gradient averaging over all angles (E) for a single dE element
		double nagrad=(dE_loss[2])/(dE_sumpeaks[i]);
		
		//make titles
		stringstream ii;
		ii<<(i+1);
		string title="dE"+ii.str()+"_graph";
		string titleD="dE_individual/dE"+ii.str();
		
		TGraph* graph=new TGraph();
		graph->SetName(title.c_str());
		graph->SetMarkerStyle(29);//SetMarkerSize(Size_t msize = 1)
		
		bool alpha_points_added=false;
		
		///////////////////////////////////////////////////////////
		/////////////  WIGGLE THE TARGET
		///////////////////////////////////////////////////////////

		//FIRST WE WIGGLE THE TARGET
		//wiggle the dE detector angle a bit
		bestangle=deangle;//best angle
		double best_chi=0;//checkvalue	
		for(int k=-100;k<=100;k++){
		
			b_line->SetParameters(0,nagrad);
			double tangle=deangle*(1+((double)k/400.0));
			TGraph temp_graph;
			
			//Go over the 16 E angle elastics
			for(int j=0;j<16;j++){
				//how much does this thickness vary from the stored energy loss
				//assuming a linear dE/dx change over the small dx difference here
				double factor=(sin(pi-(deangle+angle[j]))/sin(pi-(tangle+angle[j])));
				double adjusted_E=factor*dE_loss[j];
				
				
				//the simple gradient says peak should be at "tester"
				double tester =dE_peaks[i][j]*nagrad;
				//if it is near add it to the graph
				if(tester>adjusted_E*0.9&&tester*0.9<adjusted_E){
					temp_graph.SetPoint(temp_graph.GetN(),dE_peaks[i][j],adjusted_E);
				}
			}			
			
			//Go over the 12 inner angle E alphas
			for(int j=0;j<12;j++){
				double factor=(sin(pi-(deangle+angle[j]))/sin(pi-(tangle+angle[j])));
				double adjusted_alpha=factor*alpha_dE[j];
					
				double tester =alpha_peaks[i][j]*nagrad;
				//if it is near add it to the graph
				if(tester>adjusted_alpha*0.9&&tester*0.9<adjusted_alpha){
					alpha_points_added=true;
					temp_graph.SetPoint(temp_graph.GetN(),alpha_peaks[i][j],adjusted_alpha);
				}						
			}								
			
			//THE FIT
			if(temp_graph.GetN()>3){//if there were actually enough points
				
				if(!alpha_points_added)temp_graph.SetPoint(temp_graph.GetN(),1,0.01);//added to force graph positive
				
				temp_graph.Fit(b_line,"Q");//Quiet
				double chiget=b_line->GetChisquare()/(temp_graph.GetN()-3);

				if(best_chi==0||best_chi>chiget){//checkfit
					best_chi=chiget;
					bestangle=tangle;
				}	
			}
			

		}

		///////////////////////////////////////////////////////////
		/////////////  FINAL FILL
		///////////////////////////////////////////////////////////		
		
// 		//SET THE DECIDED ENERGIES
		for(int j=0;j<16;j++){
			double factor=(sin(pi-(deangle+angle[j]))/sin(pi-(bestangle+angle[j])));
			dE_loss[j]=factor*dE_loss[j];
			alpha_dE[j]=factor*alpha_dE[j];
		}
		
		//ACTUALLY FILL THE FINAL PRINT FIT GRAPG
		for(int j=0;j<16;j++){
			double tester =dE_peaks[i][j]*nagrad;
			if(tester>dE_loss[j]*0.9&&tester*0.9<dE_loss[j]){//if it is near add it to the graph
				graph->SetPoint(graph->GetN(),dE_peaks[i][j],dE_loss[j]);
			}
		}
		
		int xxyy=graph->GetN();	
		for(int j=0;j<10;j++){
			double tester =alpha_peaks[i][j]*nagrad;
			if(tester>alpha_dE[j]*0.9&&tester*0.9<alpha_dE[j]){//if it is near add it to the graph
				graph->SetPoint(graph->GetN(),alpha_peaks[i][j],alpha_dE[j]);
			}					
		}	
		if(xxyy==graph->GetN())graph->SetPoint(graph->GetN(),1,0.01);//if no alpha points



		//Do linear fit to all those points we just added
		b_line->SetParameters(0,nagrad);
		if(graph->GetN()>1)graph->Fit(b_line,"Q");
		quad=0;
		grad=b_line->GetParameter(1);
		intercept=b_line->GetParameter(0);
		
		//PRINT dE screen
		//cout<<endl<<"dE"<<i+1<<" "<<" y="<<grad<<"*x+"<<intercept;
		
		//do a Quad fit
		q_line->SetParameters(0,grad,intercept);
		if(graph->GetN()>1)graph->Fit(q_line,"+Q");
		//COMMENT OUT THESE 3 LINES TO USE POL(1) NOT QUAD
// 		quad=q_line->GetParameter(2);
// 		grad=q_line->GetParameter(1);
// 		intercept=q_line->GetParameter(0);		

		
		//Sets graph title to display the optimised angle
		stringstream gg;
		gg<<"dE "<<(i+1)<<" bestangle = "<<bestangle;
		string titlezz=gg.str();
		graph->SetTitle(titlezz.c_str());
		
		//fill tree with quad grad intercept
		//and write graph
		t->Fill();
		output_file->cd("dE_graphs");					
		graph->Write();
		output_file->cd();

	}	
	

	output_file->cd();
	t->AutoSave();
	//output_file->Write();
	output_file->Close();

}

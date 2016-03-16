#define Si_calib_cxx
#include "Si_calib.h"
#include <TH2.h>
#include <TStyle.h>


void Si_calib::Begin(TTree * /*tree*/)
{  TString option = GetOption();
     // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).
		
   
}

void Si_calib::SlaveBegin(TTree * /*tree*/)
{ TString option = GetOption();}

Bool_t Si_calib::Process(Long64_t entry)
{
	if(((long)entry/1000000.0)==(int)(entry/1000000))cout<<"."<<flush;
	
	GetEntry(entry);//core line to get the data!

	for(int i=0;(unsigned)i<silicon_data_p.size();i++){
				silicon_data[i]=*silicon_data_p[i];
	}
 
	bool dEA=false,dEB=false;
 
	for(int i=32;i<44;i++){
		int j=i-32;
		if(silicon_data[i]>5){
			dE_sums[j]->Fill(silicon_data[i]);
			if(j<6)dEA=true;
			else dEB=true;
		}
	}

	int A=0,B=0;
	int AA=0,BB=0;
	
	for(int i=0;i<32;i++){
		if(silicon_data[i]>5){
			if(i<16){
				if(dEA)Ed_graphs[i]->Fill(silicon_data[i]);
				else E_graphs[i]->Fill(silicon_data[i]);
				A++;
				AA=i;
			}else{
				if(dEB)Ed_graphs[i]->Fill(silicon_data[i]);
				else E_graphs[i]->Fill(silicon_data[i]);
				B++;
				BB=i-16;
			}
		}
	}	
	
 
	for(int i=32;i<44;i++){
		int j=i-32;
		if(silicon_data[i]>5){
			if(j<6&&A==1)dE_graphs[j][AA]->Fill(silicon_data[i]);
			else if(B==1)dE_graphs[j][BB]->Fill(silicon_data[i]);
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

	double E_max;
	double* xxx;
	double * tempd= new double(0);
	double centrebeam=targ.beam_e_centre(beam_Z,beam_A,ebeam);
	
	dethick=giveme_areal(silicondensity,dethick);//input density in g/cm3 and thickness in um
	ethick=giveme_areal(silicondensity,ethick);	
	
	
	for(int i=0;i<16;i++){
		angle.push_back(Atan((23.25+(((double)i+1)*1.5))/distance));
		xxx = kinetic_lab_calcs_elastic_E(centrebeam,beam_A,targ.targ_A,angle[i]);
		elastic_energy.push_back(targ.particle_e_exit(beam_Z,beam_A,xxx[8],TVector3(tan(angle[i]),0,1)));
		
		dE_thickness.push_back(dethick/sin(pi-(deangle+angle[i])));
		E_thickness.push_back(ethick/cos(angle[i]));
		dE_thickness[i]*=1.02;//you get a bit more thickness when averaging over phi aswell
		E_thickness[i]*=1.02;
	
		dE_loss.push_back(elastic_energy[i]-passage(0,beam_Z,beam_A,0,14,28,elastic_energy[i],dE_thickness[i]/1000,tempd));
		E_max=egassap(0,beam_Z,beam_A,0,14,28,E_thickness[i]/1000, 0, tempd);
		
		if((elastic_energy[i]-dE_loss[i])<E_max) E_loss_with.push_back(elastic_energy[i]-dE_loss[i]);
		else E_loss_with.push_back(E_max);
		
		if((elastic_energy[i])<E_max) E_loss_without.push_back(elastic_energy[i]);
		else E_loss_without.push_back(E_max);
		
		cout<<endl<<i+1<<" "<<angle[i]*180/pi<<" "<<dE_loss[i]<<" "<<E_loss_with[i]<<" "<<E_loss_without[i];
	}


	
	TF1* b_guas = new TF1("general_peak","gaus(0)");
	//const//mean/sigma
	b_guas->SetParameter(2,50);
	b_guas->SetParLimits(2,5,200);//set sigma limits
	

	//
	// Fit the graphs
	//

	vector< double > dE_sumpeaks;
	for(int i=0;i<12;i++){
		int maxbin=dE_sums[i]->GetMaximumBin();
		int counts=dE_sums[i]->GetBinContent(maxbin);
		
		
		b_guas->SetParameter(0,counts);
		b_guas->SetParLimits(0,counts*0.1,counts*10);
		b_guas->SetParameter(1,maxbin);
		b_guas->SetParLimits(1,maxbin-50,maxbin+50);
		b_guas->SetParameter(2,50);
		b_guas->SetRange(maxbin-200,maxbin+200);
		
		dE_sums[i]->Fit(b_guas,"RN");
		b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
		dE_sums[i]->Fit(b_guas,"R");
		
		dE_sumpeaks.push_back(b_guas->GetParameter(1));
	}
	
	vector< double > E_withdEpeaks;
	for(int i=0;i<32;i++){
		int maxbin=Ed_graphs[i]->GetMaximumBin();
		int counts=Ed_graphs[i]->GetBinContent(maxbin);
		
		
		b_guas->SetParameter(0,counts);
		b_guas->SetParLimits(0,counts*0.1,counts*10);
		b_guas->SetParameter(1,maxbin);
		b_guas->SetParLimits(1,maxbin-50,maxbin+50);
		b_guas->SetParameter(2,50);
		b_guas->SetRange(maxbin-200,maxbin+200);
		
		Ed_graphs[i]->Fit(b_guas,"RN");
		b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
		Ed_graphs[i]->Fit(b_guas,"R");
		
		E_withdEpeaks.push_back(b_guas->GetParameter(1));		
	}	
	
	
	vector< double > E_peaks;
	for(int i=0;i<32;i++){
		E_graphs[i]->GetXaxis()->SetRange(Ed_graphs[i]->GetMaximumBin()+80,8000);

		int maxbin=E_graphs[i]->GetMaximumBin();
		int counts=E_graphs[i]->GetBinContent(maxbin);
		
		
		b_guas->SetParameter(0,counts);
		b_guas->SetParLimits(0,counts*0.1,counts*10);
		b_guas->SetParameter(1,maxbin);
		b_guas->SetParLimits(1,maxbin-50,maxbin+50);
		b_guas->SetParameter(2,50);
		b_guas->SetRange(maxbin-200,maxbin+200);
		
		E_graphs[i]->Fit(b_guas,"RN");
		b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
		E_graphs[i]->Fit(b_guas,"R");
		
		E_peaks.push_back(b_guas->GetParameter(1));		
	}		
	
	vector< vector< double > > dE_peaks;
	
	for(int j=0;j<12;j++){

		dE_peaks.push_back( vector<double>(0) );
	
		for(int i=0;i<16;i++){
			int maxbin=dE_graphs[j][i]->GetMaximumBin();
			int counts=dE_graphs[j][i]->GetBinContent(maxbin);
			
			
			b_guas->SetParameter(0,counts);
			b_guas->SetParLimits(0,counts*0.1,counts*10);
			b_guas->SetParameter(1,maxbin);
			b_guas->SetParLimits(1,maxbin-50,maxbin+50);
			b_guas->SetParameter(2,50);
			b_guas->SetRange(maxbin-200,maxbin+200);
			
			dE_graphs[j][i]->Fit(b_guas,"RN");
			b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
			dE_graphs[j][i]->Fit(b_guas,"R");
			
			dE_peaks[j].push_back(b_guas->GetParameter(1));				
		}
				
	}	

	
	//
	//	save the histograms... just because
	//
	
	
	
	string outtitle=fChain->GetTree()->GetName();
	outtitle=outtitle+"_sical.root";
	
	TFile* output_file = new TFile(outtitle.c_str(),"RECREATE");
	
	output_file->cd();
	output_file->mkdir("E_no_dE");
	output_file->cd("E_no_dE");
	
		for(int i=0;i<32;i++){
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

	
	
	double grad, intercept;
	TTree *t = new TTree("si_calib","si_calib");
	t->Branch("grad",&grad);
	t->Branch("intercept",&intercept);
	
	
	for(int i=0;i<32;i++){
		int j=i;
		if(i>15)j=i-16;
		
		grad=(E_loss_without[j]-E_loss_with[j])/(E_peaks[i]-E_withdEpeaks[i]);
		intercept=E_withdEpeaks[i]*grad;
		intercept=E_loss_with[j]-intercept;	
		t->Fill();
		
		cout<<endl<<"E"<<i+1<<" "<<" y="<<grad<<"*x+"<<intercept;
	}
	
	

	
	output_file->mkdir("dE_graphs");
	output_file->cd("dE_graphs");					
	
	
	
// 	vector< TGraph*  >graph;
	for(int i=0;i<12;i++){
		
		double nagrad=(dE_loss[2])/(dE_sumpeaks[i]);
		//t->Fill();
		
		cout<<endl<<"dE"<<i+1<<" "<<" y="<<nagrad<<"*x+"<<0<<endl;
		
		stringstream ii;
		ii<<(i+1);
		string title="dE"+ii.str()+"_graph";	
		
		TGraph* graph=new TGraph();
		TF1* b_line = new TF1("general_line","pol1(0)");
		b_line->SetParameters(nagrad,0);
		b_line->SetRange(1,8000);
		graph->SetName(title.c_str());
		graph->SetMarkerStyle(29);//SetMarkerSize(Size_t msize = 1)

		for(int j=0;j<16;j++){
			
			double tester =dE_peaks[i][j]*nagrad;
			
			if(tester>dE_loss[j]*0.98&&tester*0.98<dE_loss[j]){cout<<j<<", ";
				for(int k=0;k<10;k++)graph->SetPoint(graph->GetN(),dE_peaks[i][j],dE_loss[j]);
			}	
		}
		//graph->SetPoint(graph->GetN(),0,0);

		graph->Fit(b_line,"Q");
		grad=b_line->GetParameter(0);
		intercept=b_line->GetParameter(1);
		graph->SetPoint(graph->GetN(),0,0);
		cout<<endl<<"dE"<<i+1<<" "<<" y="<<grad<<"*x+"<<intercept;
		
		graph->Fit(b_line,"Q+");
		grad=b_line->GetParameter(0);
		intercept=b_line->GetParameter(1);
		cout<<endl<<"dE"<<i+1<<" "<<" y="<<grad<<"*x+"<<intercept;		
		
		t->Fill();		
		graph->Write();
	}	
	
	output_file->cd();					
	output_file->Write();
	output_file->Close();

// 
// 
// 	if(front_thick>0)ebeam_slow=passage(0,beam_Z,beam_A,backing_compound,backing_Z,backing_A,ebeam,(front_thick)/1000,tempd);
// 	else ebeam_slow=ebeam;
// 	
// 	ebeam_slow=passage(0,beam_Z,beam_A,targ_compound,targ_Z,targ_A,ebeam_slow,(targ_thick/2)/1000,tempd);
// 	
// 	if(back_thick>0)ebeam_slow=passage(0,beam_Z,beam_A,backing_compound,backing_Z,backing_A,ebeam_slow,(back_thick)/1000,tempd);
// 
// 	
// 	double angleE[16];
// 	double EbeamE[16];
// 	double thicknessE[16];
// 	double dEpenetrateE[16];
// 	double dEnenetrateE[16];		
// 	double* xxx;	
// 	for(int i=0;i<16;i++){
// 		angleE[i]=TMath::ATan((23.25+((double)i*1.5))/45);
// 		xxx = kinetic_lab_calcs_elastic_E(ebeam_slow,beam_A,targ_A,angleE[i]);
// 		EbeamE[i]=xxx[8];
// 		thicknessE[i]=69.87/TMath::Cos(angleE[i]);
// 
// 		double Einterim=passage(0,3,7,0,14,28,EbeamE[i],18.632,tempd);
// 		
// 		dEpenetrateE[i]=Einterim-passage(0,3,7,0,14,28,Einterim,thicknessE[i],tempd);
// 		dEnenetrateE[i]=EbeamE[i]-passage(0,3,7,0,14,28,EbeamE[i],thicknessE[i],tempd);
// 	}		
// 	double de_MeV=EbeamE[7]-passage(0,3,7,0,14,28,EbeamE[7],18.632,tempd);
// 
// 

// 
// 	int j=0;
// 	for(int i=0;i<44;i++){
// 		if(i>=32){j=i-32;
// 			intercept=0;
// 			grad=de_MeV/dE_graphs[j]->GetMaximumBin();
// 		}else{
// 			if(i>15)j=i-16;
// 			E_graphs[i]->GetXaxis()->GetXaxis()->SetRange(Ed_graphs[i]->GetMaximumBin()+51,8000);
// 			E_graphs[i]->Smooth(5);
// 			grad=(dEnenetrateE[j]-dEpenetrateE[j])/(E_graphs[i]->GetMaximumBin()-Ed_graphs[i]->GetMaximumBin());
// 			intercept=Ed_graphs[i]->GetMaximumBin()*grad;
// 			intercept=dEpenetrateE[j]-intercept;
// 		}
// 		
// 		if(i==12){grad=0;intercept=0;}
// 		if(i==42){grad=0;intercept=0;}
// 		if(i==36){grad=0;intercept=0;}
// 		
// // 		t->Fill();
// 	}
// 	
// 	output_file->Write();
// 	output_file->Close();
}

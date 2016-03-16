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
			else if(j>5&&B==1)dE_graphs[j][BB]->Fill(silicon_data[i]);
			
			if(j<6&&A==1)dE_2D_graphs[j][AA]->Fill(silicon_data[AA],silicon_data[i]);
			else if(j>5&&B==1)dE_2D_graphs[j][BB]->Fill(silicon_data[BB+16],silicon_data[i]);			
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
		
		//cout<<endl<<i+1<<" "<<angle[i]*180/pi<<" "<<dE_loss[i]<<" "<<E_loss_with[i]<<" "<<E_loss_without[i];
		
		alpha_max=egassap(0,2,4,0,14,28,(E_thickness[i]+dE_thickness[i])/1000, 0, tempd);
		
		alpha_dE.push_back(alpha_max-passage(0,2,4,0,14,28,alpha_max,dE_thickness[i]/1000,tempd));
		alpha_E.push_back(alpha_max-alpha_dE[i]);
		alpha_EB.push_back(alpha_E[i]);
		//cout<<endl<<i+1<<" "<<angle[i]*180/pi<<" "<<alpha_dE[i]<<" "<<alpha_E[i];

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
		
		if(dE_sums[i]->GetEntries()>0){	
			dE_sums[i]->Fit(b_guas,"RNQ");
			b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
			dE_sums[i]->Fit(b_guas,"RQ");
		}
		
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
		
		if(Ed_graphs[i]->GetEntries()>0){	
			Ed_graphs[i]->Fit(b_guas,"RNQ");
			b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
			Ed_graphs[i]->Fit(b_guas,"RQ");
		}
		
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
			
		if(E_graphs[i]->GetEntries()>0){
			E_graphs[i]->Fit(b_guas,"RNQ");
			b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
			E_graphs[i]->Fit(b_guas,"RQ");		
		}

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
			
			if(dE_graphs[j][i]->GetEntries()>0){
				dE_graphs[j][i]->Fit(b_guas,"RNQ");
				b_guas->SetRange(b_guas->GetParameter(1)-b_guas->GetParameter(2),b_guas->GetParameter(1)+b_guas->GetParameter(2));		
				dE_graphs[j][i]->Fit(b_guas,"RQ");
			}
			dE_peaks[j].push_back(b_guas->GetParameter(1));				
		}
				
	}	

	
	//
	//	save the histograms... just because
	//
	
	
	
	
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

	
	
	//
	//	calib E
	//
	
	output_file->cd();
	double grad, intercept, quad;
	TTree *t = new TTree("si_calib","si_calib");
	
	t->Branch("quad",&quad);
	t->Branch("grad",&grad);
	t->Branch("intercept",&intercept);
	
	
	for(int i=0;i<32;i++){
		int j=i;
		if(i>15)j=i-16;
		
		quad=0;
		grad=(E_loss_without[j]-E_loss_with[j])/(E_peaks[i]-E_withdEpeaks[i]);
		intercept=E_withdEpeaks[i]*grad;
		intercept=E_loss_with[j]-intercept;	
		t->Fill();
		
		//cout<<endl<<"E"<<i+1<<" "<<" y="<<grad<<"*x+"<<intercept;
		
		if(i>15)alpha_EB[j]=(alpha_EB[j]-intercept)/grad;
		else alpha_E[j]=(alpha_E[j]-intercept)/grad;
	}
	
	

//
//	calib dE
//
	
	
	
	
	output_file->mkdir("dE_graphs");
	output_file->cd("dE_graphs");					
	
	
	
	for(int i=0;i<12;i++){
		
		double nagrad=(dE_loss[2])/(dE_sumpeaks[i]);
		//t->Fill();
		
		//cout<<endl<<"dE"<<i+1<<" "<<" y="<<nagrad<<"*x+"<<0<<endl;
		
		stringstream ii;
		ii<<(i+1);
		string title="dE"+ii.str()+"_graph";
		string titleD="dE_individual/dE"+ii.str();
		
		TGraph* graph=new TGraph();
		TF1* b_line = new TF1("general_line","pol1(0)");
		b_line->SetParameters(0,nagrad);
		b_line->SetRange(1,8000);
		

		TF1* q_line = new TF1("quaf_line","pol2(0)");
		q_line->SetParameters(0,nagrad,0);
		q_line->SetRange(1,8000);
		q_line->SetLineColor(1);
		
		graph->SetName(title.c_str());
		graph->SetMarkerStyle(29);//SetMarkerSize(Size_t msize = 1)

		for(int j=0;j<16;j++){
			
			double tester =dE_peaks[i][j]*nagrad;
			
			if(tester>dE_loss[j]*0.98&&tester*0.98<dE_loss[j]){//cout<<j<<", ";
				for(int k=0;k<10;k++)graph->SetPoint(graph->GetN(),dE_peaks[i][j],dE_loss[j]);
			}
		}
		
		
		int zeroadd=0;
		for(int j=0;j<4;j++){
			if(i<6)dE_2D_graphs[i][j]->GetXaxis()->SetRangeUser(alpha_E[i]*0.9,alpha_E[i]*1.2);
			else dE_2D_graphs[i][j]->GetXaxis()->SetRangeUser(alpha_EB[i]*0.9,alpha_EB[i]*1.2);
			dE_2D_graphs[i][j]->GetYaxis()->SetRangeUser(dE_peaks[i][j]*0.2,dE_peaks[i][j]*0.7);				
			
			stringstream jj;
			jj<<(j+1);
			string titlex="Alpha_dE"+ii.str()+"_E"+jj.str();
			output_file->cd(titleD.c_str());
			TH1D* proj = dE_2D_graphs[i][j]->ProjectionY(titlex.c_str());
						
			//BASIC FIT LOOP
			int maxbin=proj->GetBinCenter(proj->GetMaximumBin());
			int counts=proj->GetBinContent(maxbin);
			//cout<<endl<<i<<" "<<j<<" "<<maxbin<<" "<<counts;
			b_guas->SetParameter(0,counts);
			b_guas->SetParLimits(0,counts*0.1,counts*10);
			b_guas->SetParameter(1,maxbin);
			b_guas->SetParLimits(1,maxbin-50,maxbin+50);
			b_guas->SetParameter(2,50);
			b_guas->SetRange(maxbin-30,maxbin+30);
			if(proj->GetEntries()>0){
				proj->Fit(b_guas,"RQ");
				b_guas->SetRange(b_guas->GetParameter(1)-3*b_guas->GetParameter(2),b_guas->GetParameter(1)+3*b_guas->GetParameter(2));		
				proj->Fit(b_guas,"RQ");
			}
			double peak = b_guas->GetParameter(1);		
			//BASIC FIT LOOP
			
			double tester =peak*nagrad;

			
			if(tester>alpha_dE[j]*0.9&&tester*0.9<alpha_dE[j]){//cout<<j<<", ";
				zeroadd++;
				for(int k=0;k<10;k++)graph->SetPoint(graph->GetN(),peak,alpha_dE[j]);
			}
			//proj->Write();
		}
		if(zeroadd==0)graph->SetPoint(graph->GetN(),0,0);

		
		if(graph->GetN()>1)graph->Fit(b_line,"Q");
		quad=0;
		grad=b_line->GetParameter(1);
		intercept=b_line->GetParameter(0);
		
		//cout<<endl<<"dE"<<i+1<<" "<<" y="<<grad<<"*x+"<<intercept;
		
		if(graph->GetN()>1)graph->Fit(q_line,"+Q");
// 		quad=q_line->GetParameter(2);
// 		grad=q_line->GetParameter(1);
// 		intercept=q_line->GetParameter(0);		

		
		
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

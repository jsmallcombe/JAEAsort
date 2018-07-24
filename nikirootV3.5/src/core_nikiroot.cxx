// Nikiroot 3.5
// James Smallcombe 22/12/2017
// james.smallcombe@outlook.com

#include "core_nikiroot.h"

//	START OF MAIN FUNCTIONAL LOOP
//
//
using namespace std;
//using std::vector;
//using std::queue;
//using std::deque;

void core(int readoutdetail,long long triggerlength,long long pretrigger, bool timeonoroff,string addresses,string loadpath,string savepath,int readinbuffersize,vector<int> triggers,bool overwrite, int daqsafe){
	
	//
	// Address maps and buffer size to be set once DAQ channel usage is established
	//
	
	vector<int> addressmap;
	vector<string> buffername;
	vector<long long> timeoffset;

	gSystem->mkdir(savepath.c_str(),true);
	
	//generate file list variable from input file or directory
	if(readoutdetail>0) cout<<endl<<"Building file list.";
	list<string> filelist = make_file_list(loadpath);

	  
	if(readoutdetail>0)cout<<endl<<"Reading address map.";	  
	if(!read_addressmap(addressmap,buffername,timeoffset,addresses)){//if no manual address map given
		if(readoutdetail>0)cout<<endl<<"Read failed, building address map from data files.";
		if(readoutdetail>2)addressmap=generate_addressmap(filelist,buffername,true);
		else addressmap=generate_addressmap(filelist,buffername);
		timeoffset.resize(addressmap.size(),1);
	}
	
	//
	// "Global" variables for data processing
	//
	
	nikibuffer peakdatabuffer;
	
	if(readoutdetail>0) cout<<endl<<"Building buffers.";
	peakdatabuffer.SetChannels(addressmap,timeoffset,triggers);
	peakdatabuffer.SetWindow(triggerlength,pretrigger);
	
	peakdatabuffer.SetDAQSafe(daqsafe);
	
	int numberofchans=peakdatabuffer.size();
	vector<int> event(numberofchans,0);     
	vector<int> eventtimes(numberofchans,0); 	
	
	ifstream openfile;
	//TString line;

	TFile* output_file = new TFile();
	TH1F *mult = new TH1F();
	TTree *mytree= new TTree();


//DO NOT CHANGE
	bool datafilesremain=true;
	bool newrun=true;
	
	string currentfilerun="X";	
//DO NOT CHANGE
	
	//
	// MAIN PROCESS
	//

struct timeval tp;
gettimeofday(&tp, NULL);
cout<<endl<<endl<<"Start time "<<tp.tv_sec * 1000 + tp.tv_usec / 1000<<endl<<endl;

	cout<<endl;
	
	while(datafilesremain){
		
		//
		// DATA FILE LOADING
		//

		if(!openfile.is_open()){
			bool filereject=true;
			bool overwrite_clear=true;
			while(filereject){
				if(filelist.size()==0){
					if(readoutdetail>0)cout<<endl<<endl<<"That was the last file, new file not opened, nothing new buffered."<<endl;
					datafilesremain=false;
					filereject=false;
					break;
				}else{
					newrun=open_check_run(currentfilerun,openfile,filelist,readoutdetail);
					if(openfile.is_open()){
						filereject=false;
						if(!overwrite_clear&&!newrun){
							filereject=true;
							openfile.close();
						}
					}
					
					if(newrun){
						if(!overwrite){
							FileStat_t fst;
							if(gSystem->GetPathInfo((savepath+"/run"+currentfilerun+".root").c_str(), fst)==0){
								cout<<endl<<"That run has already been processed. Run with overwrite or delete .root file."<<endl;	
								filereject=true;
								overwrite_clear=false;
								newrun=false;
								openfile.close();
							}
						}
					}	
				}				
			}
		}
 
// 			//
// 			// MAIN PROCESS AREA
// 			//
		//
		// WHEN A NEW RUN IS STARTED
		// Closes old file if its there and opens a new one
		//
		if(newrun){
			if(output_file->IsOpen()){
				if(readoutdetail>0)cout<<endl<<mult->GetEntries()<<" Events. Average multiplicity "<<mult->GetMean()<<flush;;
				output_file->cd();
					mytree->AutoSave();
					mult->Write();
				gROOT->cd();
				output_file->Close();
			}
			delete output_file;
			output_file = new TFile((savepath+"/run"+currentfilerun+".root").c_str(),"RECREATE");
			output_file->cd();
				mytree = new TTree(("run"+currentfilerun).c_str(),("run"+currentfilerun).c_str());
				mult = new TH1F("mult", "Event Mutiplicity", numberofchans+1, 0, numberofchans+1);		
				
				for(int i=0;(unsigned)i<event.size();i++){
					if(mytree->GetBranchStatus(buffername[i].c_str()))cout<<endl<<"BRANCH "<<buffername[i].c_str()<<" OVERWRITTEN."<<endl;
					mytree->Branch(buffername[i].c_str(), &(event[i]));
					if(timeonoroff)mytree->Branch((buffername[i]+"_time").c_str(), &(eventtimes[i]));
				}
			gROOT->cd();

			if(readoutdetail>0)cout<<endl<<endl<<"This is a New Run. Run "<<currentfilerun;
			if(readoutdetail>0)cout<<endl<<"Wiping buffers and creating new root file. Run "<<currentfilerun<<endl;
			peakdatabuffer.WipeBuffers();
			
			newrun=false;
		}

		
		//
		// FILL BUFFER FROM FILE DURING NORMAL OPPERATION
		//
		
		if(openfile.is_open()){
			if(readoutdetail>2)peakdatabuffer.Print();
			if(peakdatabuffer.FillBuffers(openfile,readinbuffersize)){
				if(readoutdetail>1)cout<<endl<<"Reading to buffer successful";
			}else{
				if(readoutdetail>1)cout<<endl<<"Reading to buffer reached file end";
				openfile.close();
			}
			if(readoutdetail>0)cout<<"."<<flush;
			if(readoutdetail>1)cout<<endl<<"NUMBER OF EVENTS THIS RUN : "<<mytree->GetEntries()<<flush;
		}

		//
		// WHEN A EVENTS ARE TO BE CONSTRUCTED
		// Care is taken to maintain time across reads and files.
		//
		
		int multiplicity=0;
		if(openfile.is_open()||!datafilesremain){
			while(peakdatabuffer.MakeEvent(event,eventtimes,multiplicity)){
				if(multiplicity>0){
					mytree->Fill();
					mult->Fill(multiplicity);
				}
			}
		}
 	}//while(datafilesremain)

	filelist.clear();

	if(readoutdetail>0)cout<<endl<<endl<<"End of all files. Tidy and exit.";
	cout<<endl<<endl;
	if(output_file->IsOpen()){
		if(readoutdetail>0)cout<<endl<<mult->GetEntries()<<" Events. Average multiplicity "<<mult->GetMean()<<flush;;
		output_file->cd();
			mytree->AutoSave();
			mult->Write();
		gROOT->cd();
	}
	delete output_file;

gettimeofday(&tp, NULL);
cout<<endl<<endl<<"Finish time "<<tp.tv_sec * 1000 + tp.tv_usec / 1000<<endl<<endl;

	std::cout << std::flush;
}

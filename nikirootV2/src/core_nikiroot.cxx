// Nikiroot 2.4
// James Smallcombe 22/02/2015 (27/02/22)
// james.smallcombe@outlook.com

#include "core_nikiroot.h"

//	START OF MAIN FUNCTIONAL LOOP
//
//
using namespace std;
//using std::vector;
//using std::queue;
//using std::deque;

void core(int readoutdetail,long long triggerlength,long long triggerdelay, bool timeonoroff,string addresses,string loadpath,string savepath,int readinbuffersize,vector<int> triggers,double lossfraction,vector<int> extra_triggers,bool overwrite, bool buffover, bool zerotolerance){

	//
	// Check if second buffer is to be used
	//
	
	bool second_buff=false;
	if(extra_triggers.size()>0)second_buff=true;
	long dropcounter=0;
	long chopcounter=0;
	bool chopit=false;
	if(lossfraction<=1){
		chopit=true;
		lossfraction=1/lossfraction;
	}
	
	//
	// Address maps and buffer size to be set once DAQ channel usage is established
	//
	
	vector<int> addressmap;
	vector<string> buffername;

	gSystem->mkdir(savepath.c_str(),true);

	
	//generate file list variable from input file or directory
	if(readoutdetail>0) cout<<endl<<"Building file list.";
	list<string> filelist = make_file_list(loadpath);

	  
	if(readoutdetail>0)cout<<endl<<"Reading address map.";	  
	if(!read_addressmap(addressmap,buffername,addresses)){//if no manual address map given
		if(readoutdetail>0)cout<<endl<<"Read failed, building address map from data files.";
		if(readoutdetail>2)addressmap=generate_addressmap(filelist,buffername,true);
		else addressmap=generate_addressmap(filelist,buffername);
	}
	
	//
	// "Global" variables for data processing
	//
	
	nikibuffer peakdatabuffer,extra_peakdatabuffer;
	peakdatabuffer.readoutdetail=readoutdetail;
	extra_peakdatabuffer.readoutdetail=readoutdetail;
	
	if(readoutdetail>0) cout<<endl<<"Building buffers.";
	peakdatabuffer.setchannels(addressmap,triggers);
	peakdatabuffer.setwindow(triggerlength,triggerdelay);
	peakdatabuffer.buffover=buffover;
	peakdatabuffer.zerotolerance=zerotolerance;

	int numberofchans=peakdatabuffer.size()-1;
	vector<int> event(numberofchans,0);     
	vector<int> eventtimes(numberofchans,0); 	
	
	if(second_buff){
		extra_peakdatabuffer.setchannels(addressmap,extra_triggers);
		extra_peakdatabuffer.setwindow(triggerlength,triggerdelay);
		extra_peakdatabuffer.buffover=buffover;
		extra_peakdatabuffer.zerotolerance=zerotolerance;
	}
	vector<int> extra_event(numberofchans,0);     
	vector<int> extra_eventtimes(numberofchans,0);  
	
	ifstream openfile;
	//TString line;

	TFile* output_file = new TFile();
	TH1F *mult = new TH1F();
	TH1F *pile = new TH1F();
	TTree *mytree= new TTree();

	TFile* extra_output_file = new TFile();
	TH1F* extra_mult = new TH1F();
	TH1F* extra_pile = new TH1F();
	TTree* extra_mytree= new TTree();	

//DO NOT CHANGE
	bool datafilesremain=true;
	bool newrun=true;
	bool processevents=true;
	bool drainbuffer=false;
	
	string currentfilerun="X";	
//DO NOT CHANGE
	
	//
	// MAIN PROCESS
	//

struct timeval tp;
gettimeofday(&tp, NULL);
cout<<endl<<endl<<"Start time "<<tp.tv_sec * 1000 + tp.tv_usec / 1000<<endl<<endl;

	cout<<endl;
	
	while(datafilesremain||!peakdatabuffer.isbufferdry(false)||(!extra_peakdatabuffer.isbufferdry(false)&&second_buff)){
		
		//
		// DATA FILE LOADING
		//

		if(!openfile.is_open()){
			bool filereject=true;
			bool overwrite_clear=true;
			while(filereject){
				if(filelist.size()==0){
					processevents=true;	
					if(readoutdetail>0)cout<<endl<<endl<<"That was the last file, new file not opened, nothing new buffered."<<endl;
					datafilesremain=false;
					drainbuffer=true;
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
						drainbuffer=true;
						processevents=true;					
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
		if(newrun&&!drainbuffer){
			if(output_file->IsOpen()){
				if(readoutdetail>0)cout<<endl<<mult->GetEntries()<<" Events. Average multiplicity "<<mult->GetMean()<<"  Average pileup "<<pile->GetMean()<<flush;;
				output_file->cd();
					mytree->AutoSave();
					mult->Write();
					pile->Write();
				gROOT->cd();
				output_file->Close();
			}
			delete output_file;
			output_file = new TFile((savepath+"/run"+currentfilerun+".root").c_str(),"RECREATE");
			output_file->cd();
				mytree = new TTree(("run"+currentfilerun).c_str(),("run"+currentfilerun).c_str());
				mult = new TH1F("mult", "Event Mutiplicity", numberofchans+1, 0, numberofchans+1);
				pile = new TH1F("pile", "Channel Pileup", numberofchans*3, 0, numberofchans*3);
			
				
				for(int i=0;(unsigned)i<event.size();i++){
					if(mytree->GetBranchStatus(buffername[i].c_str()))cout<<endl<<"BRANCH "<<buffername[i].c_str()<<" OVERWRITTEN."<<endl;
					mytree->Branch(buffername[i].c_str(), &(event[i]));
					if(timeonoroff)mytree->Branch((buffername[i]+"_time").c_str(), &(eventtimes[i]));
				}
			gROOT->cd();

			if(readoutdetail>0)cout<<endl<<endl<<"This is a New Run. Run "<<currentfilerun;
			if(readoutdetail>0)cout<<endl<<"Wiping buffers and creating new root file. Run "<<currentfilerun<<endl;

			if(readoutdetail>1)peakdatabuffer.clearbuffer(true); else peakdatabuffer.clearbuffer(false);

			if(second_buff){
				if(extra_output_file->IsOpen()){
					if(readoutdetail>0)cout<<endl<<extra_mult->GetEntries()<<" Events. Average multiplicity "<<extra_mult->GetMean()<<"  Average pileup "<<extra_pile->GetMean()<<flush;;
					extra_output_file->cd();
						extra_mytree->AutoSave();
						extra_mult->Write();
						extra_pile->Write();
					gROOT->cd();
					extra_output_file->Close();
				}
				delete extra_output_file;
				dropcounter=0;
				chopcounter=0;
				extra_output_file = new TFile((savepath+"/extra_run"+currentfilerun+".root").c_str(),"RECREATE");
				extra_output_file->cd();
					extra_mytree = new TTree(("extra_run"+currentfilerun).c_str(),("run"+currentfilerun).c_str());
					extra_mult = new TH1F("extra_mult", "Event Mutiplicity", numberofchans+1, 0, numberofchans+1);
					extra_pile = new TH1F("extra_pile", "Channel Pileup", numberofchans*3, 0, numberofchans*3);
					for(int i=0;(unsigned)i<event.size();i++){
						if(extra_mytree->GetBranchStatus(buffername[i].c_str()))cout<<endl<<"BRANCH "<<buffername[i].c_str()<<" OVERWRITTEN."<<endl;
						extra_mytree->Branch(buffername[i].c_str(), &(extra_event[i]));
						if(timeonoroff)extra_mytree->Branch((buffername[i]+"_time").c_str(), &(extra_eventtimes[i]));
					}
				gROOT->cd();
				if(readoutdetail>0)cout<<"Clearing extra_buffers and creating new root file. Run "<<currentfilerun<<endl;
				if(readoutdetail>1)extra_peakdatabuffer.clearbuffer(true); else extra_peakdatabuffer.clearbuffer();			
			}
			
			newrun=false;
		}

		
		//
		// FILL BUFFER FROM FILE DURING NORMAL OPPERATION
		//
		
		if(openfile.is_open() && !drainbuffer){
			if(!second_buff||dropcounter>=lossfraction){
				peakdatabuffer.print();
				if(peakdatabuffer.fill(openfile,readinbuffersize)){
					if(readoutdetail>1)cout<<endl<<"Reading to buffer successful";
					processevents=true;
				}else{
					if(readoutdetail>1)cout<<endl<<"Reading to buffer reached file end";
					processevents=false;
					openfile.close();
				}				
			}else{
				peakdatabuffer.print();extra_peakdatabuffer.print();
				if(peakdatabuffer.fill(extra_peakdatabuffer,openfile,readinbuffersize)){
					if(readoutdetail>1)cout<<endl<<"Reading to buffer successful";
					processevents=true;
				}else{
					if(readoutdetail>1)cout<<endl<<"Reading to buffer reached file end";
					processevents=false;
					openfile.close();
				}					
			}

			if(readoutdetail>0)cout<<"."<<flush;
			if(readoutdetail>2)cout<<endl<<"NUMBER OF EVENTS THIS RUN : "<<mytree->GetEntries();
		}

		
		//
		// WHEN A EVENTS ARE TO BE CONSTRUCTED
		// Care is taken to maintain time across reads and files.
		//

		int pileup=0,multiplicity=0;
		bool readrequest=false,reada=false,readb=false;		
		while(processevents||drainbuffer){
	
			if(!reada){
				reada=peakdatabuffer.event_make(event,eventtimes,pileup,multiplicity,timeonoroff,drainbuffer);
	
				if(multiplicity>0){
					mytree->Fill();
					mult->Fill(multiplicity);
					pile->Fill(pileup);				
				}
			}

			if(!readb&&second_buff){
				readb=extra_peakdatabuffer.event_make(extra_event,extra_eventtimes,pileup,multiplicity,timeonoroff,drainbuffer);
				
				if(chopit)chopcounter++;
				if(multiplicity>0&&((dropcounter<lossfraction&&!chopit)||(chopcounter>=lossfraction&&chopit))){
					chopcounter=0;
					extra_mytree->Fill();
					extra_mult->Fill(multiplicity);
					extra_pile->Fill(pileup);
					if(!chopit)dropcounter++;				
				}

			}

			if(reada&&(readb||!second_buff||dropcounter>=lossfraction)){
				reada=false;
				readb=false;
				readrequest=true;
			}
			if(readrequest){
				processevents=false;
				if(drainbuffer){
					processevents=true;
					if(peakdatabuffer.isbufferdry()&&extra_peakdatabuffer.isbufferdry()){
						drainbuffer=false;
						processevents=false;
						if(readoutdetail>1)cout<<endl<<endl<<"All buffers dry.";
					}else if(peakdatabuffer.isbufferdry(false)&&extra_peakdatabuffer.isbufferdry(false)){
						drainbuffer=false;
						processevents=false;
						if(readoutdetail>1)cout<<endl<<endl<<"All trigger buffers dry.";
					}
				}
			}

		}//while(processevents)

 	}//while(datafilesremain)

	filelist.clear();


	if(readoutdetail>0)cout<<endl<<endl<<"End of all files. Tidy and exit.";
	cout<<endl<<endl;
	if(output_file->IsOpen()){
		if(readoutdetail>0)cout<<endl<<mult->GetEntries()<<" Events. Average multiplicity "<<mult->GetMean()<<"  Average pileup "<<pile->GetMean()<<flush;;
		output_file->cd();
			mytree->AutoSave();
			mult->Write();
			pile->Write();
		gROOT->cd();
	}
	delete output_file;
	if(readoutdetail>1)peakdatabuffer.clearbuffer(true); else peakdatabuffer.clearbuffer();
	
	if(second_buff){
		if(extra_output_file->IsOpen()){
			if(readoutdetail>0)cout<<endl<<extra_mult->GetEntries()<<" Events. Average multiplicity "<<extra_mult->GetMean()<<"  Average pileup "<<extra_pile->GetMean()<<flush;;
			extra_output_file->cd();
				extra_mytree->AutoSave();
				extra_mult->Write();
				extra_pile->Write();
			gROOT->cd();
		}
		delete extra_output_file;
		if(readoutdetail>1)extra_peakdatabuffer.clearbuffer(true); else extra_peakdatabuffer.clearbuffer();			
	}


gettimeofday(&tp, NULL);
cout<<endl<<endl<<"Finish time "<<tp.tv_sec * 1000 + tp.tv_usec / 1000<<endl<<endl;

	std::cout << std::flush;
}

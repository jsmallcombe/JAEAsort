#include "Si_calib.h"
#include "james_target.h"
void Calib(string);

void Calib(string infile){
	target targ(74,186,71.0,-45.0);

	ifstream openfile;
	string instr;
	openfile.open(infile.c_str());
	
	if(openfile.is_open()){
		while(openfile>>instr){
			
			string manip=instr;
			while(manip.find("/")<manip.length())manip=manip.substr(manip.find("/")+1,manip.length());
			while(manip.find(".root")<manip.length())manip=manip.substr(0,manip.find(".root"));
			string path=instr.substr(0,instr.find("run"));

// 			string calibfile="/home/smallcombe/data/Ir_fiss_Nov2014/cal/"+manip+"_sical.root";
// 			cout<<endl<<endl<<endl<<calibfile<<endl<<endl<<endl;
			
			
// 			TFile *f = new TFile(calibfile.c_str(),"READ");
// 			if(f){cout<<endl<<"BEST"<<endl;f->Close();}
// 			else
// 			{cout<<endl<<"TEST"<<endl;
				TChain chain(("extra_"+manip).c_str());
				chain.AddFile((path+"extra_"+manip+".root").c_str());
				Si_calib * calib = new Si_calib(41.5,3,7,80,targ,"/home/smallcombe/data/Ir_fiss_Nov2014/cal/");
				chain.Process(calib);
				gROOT->Delete("*;*");					
// 			}
// 			delete f;

		}
	}
	openfile.close();


//	new TBrowser;
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
// 			//
// 		// Move in to the data subdirector in the system
// 		//
// 		void *dirp = gSystem->OpenDirectory(dir_or_list.c_str());
// 		if (!dirp){cout<<endl<<"FAILED TO OPEN SPECIFIED LOADPATH DIRECTORY: "<<dir_or_list;}
// 		char *direntry;
// 
// 		// Get the names of the entries in the directory sequentially 
// 		while ((direntry=(char*)gSystem->GetDirEntry(dirp))) {
// 
// 			string str(direntry);	// Convert to c++ string
// 
// 			// If ".dat" and "RUN" contained
// 			if ((str.find(".dat"))<str.length()&&(str.find("RUN"))<str.length()&&(str.find("_"))<str.length()){
// 				filelistret.push_back(dir_or_list+"/"+str);
// 			}
// 
// 		}// End loop over loadpath directory
// 		
// 	}
// 		
// 	filelistret.sort();
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}
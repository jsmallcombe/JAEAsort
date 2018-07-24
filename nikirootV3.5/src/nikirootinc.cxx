// Nikiroot 3.5
// James Smallcombe 22/12/2017
// james.smallcombe@outlook.com

#include "nikirootinc.h"
string runnumberfindercutter(string input){
	string manip =input;
	if(manip.find("RUN")<manip.length()&&manip.find("_")<manip.length()){
		while(manip.find("RUN")<manip.length())manip=manip.substr(manip.find("RUN")+3);
		while(manip.find("_")<manip.length())manip=manip.substr(0,manip.find("_"));		
		if(manip.empty())manip="NA";
	}else manip="NA";
	
	return manip;
}

list<string> make_file_list(string dir_or_list){
	list<string> filelistret;
	
	if(dir_or_list.find(".txt")<dir_or_list.length()){
		//manual
		string reader;
		ifstream listfile;
		listfile.open(dir_or_list.c_str());

		if(listfile.is_open()){
			while(listfile>>reader)filelistret.push_back(reader);
			listfile.close();
		}else cout<<endl<<"FAILED TO OPEN LISTFILE: "<<dir_or_list;
	
	}else{
		//auto
		
		//
		// Move in to the data subdirector in the system
		//
		void *dirp = gSystem->OpenDirectory(dir_or_list.c_str());
		if (!dirp){cout<<endl<<"FAILED TO OPEN SPECIFIED LOADPATH DIRECTORY: "<<dir_or_list;}
		char *direntry;

		// Get the names of the entries in the directory sequentially 
		while ((direntry=(char*)gSystem->GetDirEntry(dirp))) {

			string str(direntry);	// Convert to c++ string

			// If ".dat" and "RUN" contained
			if ((str.find(".dat"))<str.length()&&(str.find("RUN"))<str.length()&&(str.find("_"))<str.length()){
				filelistret.push_back(dir_or_list+"/"+str);
			}

		}// End loop over loadpath directory
		
	}
		
	filelistret.sort();
	return filelistret;
}
	

	


vector<int> generate_addressmap(list<string> files,vector<string>& nam,bool output,int buffersize){
	vector<int> addresstemp(1,0);
	ifstream testfile;
	TString line;
	list<string> borrowlist=files;	
	
	cout<<endl;
	for(int i=0;(unsigned)i<files.size();i++){
		string filen=borrowlist.front();
		testfile.open(filen.c_str());
		borrowlist.pop_front();

		if(testfile.is_open()){
			
			if(output)cout<<endl<<"Skimming file "<<filen;else cout<<"."<<flush;

			int j=0,a,b;
			while(line.ReadLine(testfile)&&j<buffersize){
				line.ReplaceAll(","," ");
			    	istringstream is(line.Data());
			    	is >> a >> b;
				
				while(addresstemp.size()<=(unsigned)a*16+b)addresstemp.push_back(0);
				addresstemp[a*16+b]=1;
								
				j++;
			}
			testfile.seekg(0, std::ios::beg);
			testfile.close();
		}else cout<<endl<<"FAILED TO OPEN FILE: "<<filen;
	}
	
	
	ofstream ofs;
	ofs.open ("channellist.txt", std::ofstream::out | std::ofstream::trunc);
	if(output)cout<<endl<<endl<<"Channels active : ";
	vector<int> ret_list;
	for(int i=0;(unsigned)i<addresstemp.size();i++){
		if(addresstemp[i]==1){
			ret_list.push_back(i);
			stringstream buffername;
			buffername<<i;
			nam.push_back("chan_"+buffername.str());
			if(output)cout<<i<<", ";
			ofs<<i<<" "<<"chan_"+buffername.str()<<" 0"<<endl;
		}
	}
		
	return ret_list;
}
		
		
bool read_addressmap(vector<int>& chan,vector<string>& nam,vector<long long>& time,string filen){

	if(filen.size()<1) return false; 

	ifstream testfile;
	testfile.open(filen.c_str());
	if(!testfile.is_open()) return false;
	
	string ina;
	int inb;
	long long inc;
	while(testfile>>inb && testfile>>ina && testfile>>inc){
		chan.push_back(inb);
		nam.push_back(ina);
		time.push_back(inc);
	}
	testfile.close();
	
	if(chan.size()>0)return true;
	else return false;
}	
		
		
		
		
		
bool open_check_run(string& currentfilerun,ifstream& openfile,list<string>& filelist,int readoutdetail){

	string previousfilerun=currentfilerun;

	while(!openfile.is_open() && filelist.size()>0){
		string currentfile=filelist.front();
		filelist.pop_front();
		openfile.open(currentfile.c_str());
		
		if (openfile.is_open()){
			currentfilerun=runnumberfindercutter(currentfile);	
			if(readoutdetail>0)cout<<endl<<"Opening next file : "<<currentfile<<endl;else cout<<"."<<flush;
			
			
		}else	cout<<endl<<"ERROR OPENING FILE : "<<currentfile;
	}
	
	if(currentfilerun!=previousfilerun){
		if(readoutdetail>0&&previousfilerun!="X")cout<<"End of Run "<<previousfilerun<<". Using remaining buffer data.";
		return true;
	}else 	return false;
	

}

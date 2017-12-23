// Nikiroot 3.5
// James Smallcombe 22/12/2017
// james.smallcombe@outlook.com

#include <TApplication.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include "core_nikiroot.h"

int main(int argc, char *argv[]){
	
//int argcb;
//char **argvb;	
//TApplication *app = new TApplication("app", &argc, argv);

	int readoutdetail=1;
	long long triggerlength=200;
	long long pretrigger=0;
	bool timeonoroff=false;
	string addresses="";
	string loadpath=".";
	string savepath=".";
	int readinbuffersize=1000;
	int daqsafe=5;
	vector<int> triggers;
	bool overwrite=false;
	
	vector<string> inputs;
	ifstream openfile;

	for(int i=1;i<argc;i++)inputs.push_back(argv[i]);
				
	for(int i=0;(unsigned)i<inputs.size();i++){
		string arg=inputs[i];
		stringstream strea;

		if(arg.size()>1 && arg[0]=='-'){
			char bc=arg[1];
			arg=arg.substr(2);
			strea<<arg;
			
			if(bc=='r') strea>>readoutdetail;
			if(bc=='t') strea>>triggerlength;
			if(bc=='p') strea>>pretrigger;
			if(bc=='T') timeonoroff=true;
			if(bc=='c') addresses=arg;
			if(bc=='l') loadpath=arg;
			if(bc=='s') savepath=arg;
			if(bc=='b') strea>>readinbuffersize;
			if(bc=='o') overwrite=true;
			if(bc=='B') strea>>daqsafe;
			if(bc=='-'){
				openfile.open(arg.c_str());
				if(openfile.is_open()){
					while(openfile>>arg)inputs.push_back(arg);
					openfile.close();
				}
			}	
			if(bc=='D'){
				int a;
				TString line=arg;
				line.ReplaceAll(","," ");
				istringstream is(line.Data());
				while(is >> a)
				triggers.push_back(a);
			}
		}
	}

	core(readoutdetail,triggerlength,pretrigger,timeonoroff,addresses,loadpath,savepath,readinbuffersize,triggers,overwrite,daqsafe);
	
// 	gSystem->ChangeDirectory(savepath.c_str());
// 	TBrowser* broone=new TBrowser();
// 	broone->Create();
// 	app->Run();

	return 0;
}

// Nikiroot 2.4
// James Smallcombe 22/02/2015 (27/02/22)
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
	long long triggerdelay=0;
	bool timeonoroff=false;
	string addresses="";
	string loadpath=".";
	string savepath=".";
	int readinbuffersize=1000000;
	vector<int> triggers;
	vector<int> extra_triggers;
	double lossfraction=1.0;
	bool overwrite=false;
	bool buffover=false;
	bool zerotolerance=false;
	
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
			if(bc=='d') strea>>triggerdelay;
			if(bc=='T') timeonoroff=true;
			if(bc=='c') addresses=arg;
			if(bc=='l') loadpath=arg;
			if(bc=='s') savepath=arg;
			if(bc=='b') strea>>readinbuffersize;
			if(bc=='o') overwrite=true;
			if(bc=='B') buffover=true;
			if(bc=='Z') zerotolerance=true;
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
			if(bc=='E'){
				int a;
				TString line=arg;
				line.ReplaceAll(","," ");
				istringstream is(line.Data());
				while(is >> a)
				extra_triggers.push_back(a);
			}
			if(bc=='X'){
				strea>>lossfraction;
			}
		}
	}

	core(readoutdetail,triggerlength,triggerdelay,timeonoroff,addresses,loadpath,savepath,readinbuffersize,triggers,lossfraction,extra_triggers,overwrite,buffover,zerotolerance);
	
// 	gSystem->ChangeDirectory(savepath.c_str());
// 	TBrowser* broone=new TBrowser();
// 	broone->Create();
// 	app->Run();

	return 0;
}

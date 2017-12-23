// Nikiroot 3.5
// James Smallcombe 22/12/2017
// james.smallcombe@outlook.com

#ifndef __NTROOTINC_H_INCLUDED__   // if x.h hasn't been included yet...
#define __NTROOTINC_H_INCLUDED__   //   #define this so the compiler knows it has been included

#include <TSystem.h>


#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <queue>
#include <list>

using namespace std;



string runnumberfindercutter(string);

list<string> make_file_list(string);

vector<int> generate_addressmap(list<string>,vector<string>&,bool=false,int=100000);

bool read_addressmap(vector<int>&,vector<string>&,vector<long long>&,string);


bool open_check_run(string&,ifstream&,list<string>&,int);



















#endif 

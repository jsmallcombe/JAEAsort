// Nikiroot 2.4
// James Smallcombe 22/02/2015 (27/02/22)
// james.smallcombe@outlook.com

#ifndef __CORE_H_INCLUDED__   // if x.h hasn't been included yet...
#define __CORE_H_INCLUDED__   //   #define this so the compiler knows it has been included

#include <TH1.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TPad.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TObjArray.h>
#include <TMath.h>
#include <TFile.h>
#include <TTree.h>
#include <TBrowser.h>

#include <sys/time.h>


#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <queue>
#include <list>
#include <deque>


#include "nikirootinc.h"
#include "buffertemplate.h"

using namespace std;

	
void core(int =1,long long=200,long long=0, bool =false,string ="",string =".",string =".",int =1000000,vector<int> =vector<int>(),double =1.0,vector<int> =vector<int>(),bool=false,bool=false,bool=true);


#endif 

// Nikiroot 2.4
// James Smallcombe 22/02/2015 (27/02/22)
// james.smallcombe@outlook.com

#ifndef __BUFFERTEMPLATE_H_INCLUDED__   // if x.h hasn't been included yet...
#define __BUFFERTEMPLATE_H_INCLUDED__   //   #define this so the compiler knows it has been included

#include <TSystem.h>


#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <queue>
#include <list>
#include <iomanip>
#include <bitset>

using namespace std;

class nikibuffer
{
    public:
	// constructor
	nikibuffer();
	// destructor
	~nikibuffer();   

	
	bool file_death;
	bool buffover;
	bool zerotolerance;

	bool badtime;
	bool timeuncertain;
	long long oldtime;
	long long newtime;
	long long trigzeroaverage;
	int failcount;
	int failcount_datasum;
	int readoutdetail;

	vector< queue<int> > databuffer;
	vector< queue<long long> > timebuffer;	

	
	// Methods
	int size();
	void addchannel(int,bool);
	void setchannels(vector<int>,vector<int>);
	bool isbufferdry(bool=true);
	void clearbuffer(bool=false);
	bool fill(ifstream&,int=1000000);
	bool fill(nikibuffer&,ifstream&,int=1000000);
	void setwindow(long long=200, long long=0);

	bool event_make(vector<int>&,vector<int>& ,int&,int&,bool=false,bool=false);
	long long minimumtimefrontbuffer();
	void print();
	long long timeifiy(long long);
	void maketrigzeroaverage();
	void timebufferprint();

	
    private:
// 	vector< queue<int> > databuffer;
// 	vector< queue<long> > timebuffer;
	int lengthofbuffer;
	int mapped;
	vector<int> addressmap;
	vector<bool> triggers;
	vector<long long> prev_time;
	
	int read_in_size;
	
	long long fulltime;
	long long bottomtime; 
	long long toptime;

	long long triggerlength;
	long long triggerdelay;
	long long timezero;
	long long trigpast;
	long long trigfuture;
	
	int currenttriggerchanel;
	long long triggertime;
	long long triggerstart;
	long long triggerend;
	long long reasoned_resetminium;
};






//template <class T>
//void clearbuffer(vector< queue< T > > &);

//template class TT>
//bool isbufferdry(vector< queue< TT > > &);
	

























#endif 

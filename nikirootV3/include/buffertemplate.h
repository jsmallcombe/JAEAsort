// Nikiroot 3.0
// James Smallcombe 16/12/2016
// james.smallcombe@outlook.com

#ifndef __BUFFER_H_INCLUDED__   // if x.h hasn't been included yet...
#define __BUFFER_H_INCLUDED__   //   #define this so the compiler knows it has been included

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

	int fDetail;
	
	// Methods
	void AddChannel(int,bool);
	void SetChannels(vector<int>,vector<int>);
	void SetWindow(long long length, long long pretrig){fTriggerLength=length;fPreTrigger=pretrig;}
	void SetDAQSafe(int safe){DAQReadsSafe=safe;}	
	
	void WipeBuffers();
	
	bool FillBuffers(ifstream&,int=100);
	bool MakeEvent(vector<int>&,vector<int>&,int&);
	
	int size();
	
	void Print();

    private:
	int fBufferLength; //Number of active channels that have data buffers
	int fNumberOfMappedChannels; //Number of channels on the DAQ
	vector<int> fDAQaddresses; //Mapping from daq channel number to buffer number

	vector<bool> fChannelIsTrigger; //Is this buffer channel [i] a trigger channel
	vector<long long> fPrevTime; //Timestamp of previous datum in buffer [i]	
	vector< int > fDAQRead;
	
	vector< queue<int> > fDataBuffer;
	vector< queue<long long> > fTimeBuffer;	
	
	long long fTriggerLength; // In ticks
	long long fPreTrigger;
	int fTotalDAQReads;
	int DAQReadsSafe;// The number of DAQ reads to request more reads or to fix the time
	int DAQReadsGood;// The DAQ reads number after which data is good again
	
	// Methods
	void ResetBufferLists();
	void IterateDAQreads();
	int EarliestDAQread();
	int LatesetDAQread();
	int WipeSingleBuffer(int);
	
	bool PopChan(int);
	bool GetChan(int,int&,long long&);
	bool GetTrigger(int&,long long&);
	bool PopAllTillGood();
	
};




#endif 

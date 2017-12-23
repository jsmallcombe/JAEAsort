// Nikiroot 3.5
// James Smallcombe 22/12/2017
// james.smallcombe@outlook.com

#include "buffertemplate.h"

// S
// Contructor
nikibuffer::nikibuffer() : fDetail(0),fBufferLength(0),fNumberOfMappedChannels(0),fTriggerLength(10),fPreTrigger(0),fTotalDAQReads(0),DAQReadsSafe(4),DAQReadsGood(0)
{	
	this->ResetBufferLists();
}

// Destructor
nikibuffer::~nikibuffer(){}

// Delete all the buffers except the 0 null buffer and reset things to 0
void nikibuffer::ResetBufferLists(){
	fDAQRead.resize(1,0);
	fDataBuffer.resize(1);
	fTimeBuffer.resize(1);
	fTimeOffset.resize(1);
	fDAQaddresses.resize(1,0);
	fChannelIsTrigger.resize(1,false);
	fPrevTime.resize(1,-1);
}

// Methods
// Return the number of buffers EXCLUDING the 0 null buffer
int nikibuffer::size(){	return fBufferLength;}

//Add everything needed for an additional channel
void nikibuffer::AddChannel(int daqchan,long long toffset,bool trig){
		fDataBuffer.push_back(queue<int>());
		fTimeBuffer.push_back(queue<long long>());
		fTimeOffset.push_back(toffset);
		fDAQRead.push_back(fTotalDAQReads);
		fPrevTime.push_back(-1);
		fChannelIsTrigger.push_back(trig);
		fBufferLength++;
		
		while(fDAQaddresses.size()<=(unsigned)daqchan)fDAQaddresses.push_back(0);
		fDAQaddresses[daqchan]=fBufferLength;
		fNumberOfMappedChannels=fDAQaddresses.size();
}

//Add a list of buffer channels rather than individually
void nikibuffer::SetChannels(vector<int> orderedlist,vector<long long> toffset,vector<int> triglist){	
	this->ResetBufferLists();
	
	for(int i=0;(unsigned)i<orderedlist.size();i++){
		bool trig=false;
		if(triglist.size()==0)trig=true;
		else for(int j=0;(unsigned)j<triglist.size();j++)if(triglist[j]==orderedlist[i])trig=true;
		this->AddChannel(orderedlist[i],toffset[i],trig);
	}
}



// Clear all datat from the specified buffer
int nikibuffer::WipeSingleBuffer(int i){
	if(i>fBufferLength)return 0;
	int count=0;
	while(fDataBuffer[i].size()>0){fDataBuffer[i].pop();fTimeBuffer[i].pop();count++;}
	
	fDAQRead[i]=0;
	fPrevTime[i]=0;
	
	return count;
}

void nikibuffer::WipeBuffers(){
	DAQReadsGood=0;
	for(int i=0;i<=fBufferLength;i++)WipeSingleBuffer(i);
}

//Fill the buffers with N DAQ reads of data from the provided file. Return false if hit EOF
bool nikibuffer::FillBuffers(ifstream& openfile,int readinbuffersize){
	int readcount=readinbuffersize;
	int Card,Chan,Data,Address;
	long long TimeStamp;
	TString line;
	
	//Wipe the 0th unused buffer
	this->WipeSingleBuffer(0);
	
	this->IterateDAQreads();
	
	int CurrentCard=0;

	while(line.ReadLine(openfile)&&readcount>0) {
		line.ReplaceAll(","," ");
		istringstream is(line.Data());
		is >> Card >> Chan >> Data >> TimeStamp;
		Address=Card*16+Chan;
		
		if(fNumberOfMappedChannels>Address && Address>=0){
			fDataBuffer[fDAQaddresses[Address]].push(Data);
			fTimeBuffer[fDAQaddresses[Address]].push(TimeStamp);
		}
		
		if(CurrentCard!=Card){
			if(Card<CurrentCard){
				readcount--;
				if(readcount>0)this->IterateDAQreads();
			}
			CurrentCard=Card;
		}
	}
	
	if(readcount==0)return true;
	else return false;
}//returns false if EOF

//Add the negative DAQread flag and iterate
void nikibuffer::IterateDAQreads(){
	fTotalDAQReads--;
	for(int i=1;i<=fBufferLength;i++){
			fDataBuffer[i].push(fTotalDAQReads);
			fTimeBuffer[i].push(fTotalDAQReads);
	}
}
	
int nikibuffer::LatesetDAQread(){
	int ret=0;
	for(int i=1;i<=fBufferLength;i++){
			int r=fDAQRead[i];
			if(r<ret)ret=r;
	}
	return ret;
}
int nikibuffer::EarliestDAQread(){
	int ret=fTotalDAQReads;
	for(int i=1;i<=fBufferLength;i++){
			int r=fDAQRead[i];
			if(r>ret)ret=r;
	}
	return ret;
}


//Wipes the front of the channel
bool nikibuffer::PopChan(int i){
	if(i>fBufferLength)return false;
	
	//pops until actual data or end of buffer and updates DAQread marker
	fDataBuffer[i].pop();
	fTimeBuffer[i].pop();
	while(fDataBuffer[i].size()>0&&fDataBuffer[i].front()<0){
		fDAQRead[i]=fDataBuffer[i].front();
		fDataBuffer[i].pop();
		fTimeBuffer[i].pop();
	}
	
	//Returns false if out of data
	if(fDataBuffer[i].size()==0)return false;
	
	if(fPrevTime[i]>fTimeBuffer[i].front()){
		fPrevTime[i]=0;
		DAQReadsGood=fDAQRead[i]+DAQReadsSafe;
	}else{
		fPrevTime[i]=fTimeBuffer[i].front();
	}

	return true;
}


//Returns false if out of data
bool nikibuffer::GetChan(int i,int& Value,long long& Time){
	Value=0;Time=0;
	if(i>fBufferLength)return false;
	if(fDataBuffer[i].size()==0)return false;
	if(fDataBuffer[i].front()<0){
		if(!this->PopChan(i))return false;
	}
	Value=fDataBuffer[i].front();
	Time=fTimeBuffer[i].front()+fTimeOffset[i];
	if(Time<0)Time=0;//Because negative values denote daq reads
	return true;
}


bool nikibuffer::GetTrigger(int& TrigDAQ,long long& TrigTime){
	//
	// This establishes the earliest trigger and also if any of the channels have run out of data too early relative to the trigger
	//
	int EarliestReadRequest=fTotalDAQReads;
	long long TriggerT=0;
	long long TriggerDAQRead=fTotalDAQReads;
	int V;long long T;
	for(int i=1;i<=fBufferLength;i++){
		if(this->GetChan(i,V,T)){//Has data
			if(fChannelIsTrigger[i]){
				if(fDAQRead[i]>=TriggerDAQRead){
					if(T<TriggerT||TriggerT==0){
						TriggerT=T;
						TriggerDAQRead=fDAQRead[i];
					}
				}
			}
		}else{
			if(EarliestReadRequest<fDAQRead[i])EarliestReadRequest=fDAQRead[i];
		}
	}
	
	TrigDAQ=TriggerDAQRead;
	TrigTime=TriggerT;
	
	if(EarliestReadRequest>TriggerDAQRead-DAQReadsSafe)return false;
	return true;
}


bool nikibuffer::PopAllTillGood(){
	if(DAQReadsGood<EarliestDAQread()){
		for(int i=1;i<=fBufferLength;i++){
			while(fDAQRead[i]>DAQReadsGood){
				if(!PopChan(i))return false;
			}
		}
	}
	return true;
}


bool nikibuffer::MakeEvent(vector<int>& event,vector<int>& eventtime,int& multiplicity){	
	multiplicity=0;
	for(int i=0;i<fBufferLength;i++){
		event[i]=0;
		eventtime[i]=0;
	}
		
	if(!PopAllTillGood())return false;

	int TrigDAQ;
	long long TrigTime,TrigStart,TrigEnd;
	
	if(GetTrigger(TrigDAQ,TrigTime)){// Establishes a trigger present and no channels require read
		TrigStart=TrigTime-fPreTrigger;
		TrigEnd=TrigStart+fTriggerLength;
		for(int i=1;i<=fBufferLength;i++){
			int V;long long T=0;
			//Deletes any data present before the trigger return if run out of data
			bool HasData=true;
			while(T<TrigStart){
				HasData=GetChan(i,V,T);
				if(T<TrigStart){
					HasData=PopChan(i);
				}
				if(!HasData){
					if(fDAQRead[i]>TrigDAQ-DAQReadsSafe)return false;
					else break;
				}
			}
		}
	}else{
		return false;
	}

	//If we still havent returned by this point we have:
	//Established a trigger
	//Established any channels needing reads are way after the trigger
	//Popped all data before the start of trigger

	//Now we check if during any of the poping we found a time stamp error (and fix it if possible)
	if(!PopAllTillGood())return false;

	for(int i=1;i<=fBufferLength;i++){
		int V;long long T=0;
		if(!GetChan(i,V,T))return false;
		if(T<TrigEnd){
			event[i-1]=V;
			eventtime[i-1]=T-TrigTime;
			multiplicity++;
			PopChan(i);
		}
	}
	
	return true;
}


void nikibuffer::Print(){
	cout<<endl<<endl<<"Chan DAQRead Size Time"<<endl;
	cout<<"null - "<<fDataBuffer[0].size()<<" - "<<endl;
	for(int i=1;i<=fBufferLength;i++){
		for(int j=0;j<fNumberOfMappedChannels;j++){
			if(fDAQaddresses[j]==i)cout<<j<<" ";
		}
		cout<<fDAQRead[i]<<" ";
		cout<<fDataBuffer[i].size()<<" ";
		cout<<fTimeBuffer[i].front()<<" ";
		cout<<endl;
	}
}

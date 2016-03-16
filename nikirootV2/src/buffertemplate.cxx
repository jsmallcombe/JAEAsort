// Nikiroot 2.4
// James Smallcombe 22/02/2015 (27/02/22)
// james.smallcombe@outlook.com

#include "buffertemplate.h"

// S
// Contructor
nikibuffer::nikibuffer() : failcount(0),failcount_datasum(0),readoutdetail(0),triggerlength(10),triggerdelay(0),timezero(0),trigpast(0),trigfuture(0)
{	
	std::bitset<40> foo;     // foo: 0000
	foo.set();              // foo: 1111

	fulltime=foo.to_ullong();
	bottomtime=0.33*fulltime; 
	toptime=0.66*fulltime;
	
	//cout << foo << " as an integer is: " << fulltime << '\n';
	
	triggertime=0;
	currenttriggerchanel=0;
	this->setwindow();
	triggerstart=0;
	triggerend=0;
	
	lengthofbuffer = 1;

	databuffer.resize(1);
	timebuffer.resize(1);
	
	addressmap.resize(1,0);
	triggers.resize(1,false);
	prev_time.resize(1,-1);
	
	file_death=false;
	buffover=false;
	badtime=false;
	timeuncertain=false;
	zerotolerance=false;
	oldtime=0;
	newtime=0;
	trigzeroaverage=0;
	reasoned_resetminium=0;
	
	mapped=0;
}

// Destructor
nikibuffer::~nikibuffer()
{
	databuffer.clear();
	timebuffer.clear();
}


// Methods
int nikibuffer::size()
{
	return lengthofbuffer;
}

// Methods
void nikibuffer::addchannel(int daqchan,bool trig)
{
		databuffer.push_back(queue<int>());
		timebuffer.push_back(queue<long long>());
		prev_time.push_back(-1);
		while(addressmap.size()<=(unsigned)daqchan)addressmap.push_back(0);
		addressmap[daqchan]=lengthofbuffer;
		mapped=addressmap.size();
		lengthofbuffer++;
		triggers.push_back(trig);

}



// Methods
void nikibuffer::setchannels(vector<int> orderedlist,vector<int> triglist)
{	//basically resetbector
	lengthofbuffer = 1;
	databuffer.resize(1);
	timebuffer.resize(1);
	addressmap.resize(1);
	triggers.resize(1);
	prev_time.resize(1);
	
	for(int i=0;(unsigned)i<orderedlist.size();i++){
		bool trig=false;
		if(triglist.size()==0)trig=true;

		databuffer.push_back(queue<int>());
		timebuffer.push_back(queue<long long>());
		prev_time.push_back(-1);
		while(addressmap.size()<=(unsigned)orderedlist[i])addressmap.push_back(0);
		addressmap[orderedlist[i]]=lengthofbuffer;
		lengthofbuffer++;
		for(int j=0;(unsigned)j<triglist.size();j++)if(triglist[j]==orderedlist[i])trig=true;
		triggers.push_back(trig);
	}
	mapped=addressmap.size();
}



bool nikibuffer::isbufferdry(bool all){
	if(badtime) return true;//time going bad is one way of going dry
		
	for(int i=1;i<lengthofbuffer;i++){
		if(databuffer[i].size()>0&&(triggers[i]||all))return false;
	}

	return true;
}

void nikibuffer::clearbuffer(bool printt){
	if(badtime&&readoutdetail>1){
		cout<<endl<<endl<<" TIME ERROR NOT FIXED. "<<endl<<" Data waste >"<<0.00005722*double(failcount_datasum)<<" Mb"<<endl<<endl;					
		this->timebufferprint();
	}
	if(timeuncertain&&readoutdetail>1){
		cout<<endl<<endl<<" TIME FIX NOT CONFIRMED. "<<endl<<endl;							
		this->timebufferprint();
	}
	for(int i=0;i<lengthofbuffer;i++){
		if(databuffer[i].size()>0 && printt)cout<<endl<<"Clearing Buffer "<<i<<", should be size 0 but is "<<databuffer[i].size()<<flush;
		while(databuffer[i].size()>0){databuffer[i].pop();timebuffer[i].pop();}
		prev_time[i]=-1;
		
	}
	badtime=false;
	timeuncertain=false;
	failcount=0;
	failcount_datasum=0;
	oldtime=0;
	newtime=0;
	trigzeroaverage=0;
}

bool nikibuffer::fill(ifstream& openfile,int readinbuffersize){
	int readcount=readinbuffersize;
	int a,b,c,m;
	long long d;
	TString line;
	
	while(databuffer[0].size()>0){databuffer[0].pop();timebuffer[0].pop();}

	
	while(line.ReadLine(openfile)&&readcount>0) {
		line.ReplaceAll(","," ");
		istringstream is(line.Data());
		is >> a >> b >> c >> d;
		m=a*16+b;
		
		if(mapped>m && m>=0){
			databuffer[addressmap[m]].push(c);
			timebuffer[addressmap[m]].push(d);
		}
		readcount--;
		
		if(readcount==0&&buffover){ //new overide bit
			for(int i=1;i<lengthofbuffer;i++)if(databuffer[i].size()==0) readcount=readinbuffersize;
		}
	}
	
	this->maketrigzeroaverage();
	
	if(openfile.good()&&!file_death) return true;
	else {file_death=false; return false;}
}

bool nikibuffer::fill(nikibuffer& other,ifstream& openfile,int readinbuffersize){
	int readcount=readinbuffersize;
	int a,b,c,m;
	long long d;
	TString line;
	
	while(databuffer[0].size()>0){databuffer[0].pop();timebuffer[0].pop();}
	while(other.databuffer[0].size()>0){other.databuffer[0].pop();other.timebuffer[0].pop();}
	
	while(line.ReadLine(openfile)&&readcount>0) {
		line.ReplaceAll(","," ");
		istringstream is(line.Data());
		is >> a >> b >> c >> d;
		m=a*16+b;
		
		if(mapped>m && m>=0){
			databuffer[addressmap[m]].push(c);
			timebuffer[addressmap[m]].push(d);
			
			other.databuffer[addressmap[m]].push(c);
			other.timebuffer[addressmap[m]].push(d);
		}
		readcount--;
	}
	
	this->maketrigzeroaverage();
	other.maketrigzeroaverage();

	if(openfile.good()&&!file_death) return true;
	else {file_death=false; return false;}
}

//
//	loop to make events if possible
//	picks out next trigger
//	wont give events while there is a time error
//	but will repair time errors in the buffer
//
bool nikibuffer::event_make(vector<int>& event,vector<int>& eventtime,int& pileup,int& multiplicity,bool timeor,bool drainbuffer){
	long long nexttriggertime=0;
	pileup=0;
	multiplicity=0;
	bool readrequest=false;
	int newtrig=currenttriggerchanel;

	////////////////////////////////////////////
	///////TIME BROKEN If currently there is a bad timeflag we are clearing buffer segments to fix it
	///////////////////////////////////////////
	
	if(badtime){
		int count=0;//count the buffer channels currently time_good
		for(int i=1;i<lengthofbuffer;i++){
			bool valid=false;
			
			//Channels that have been fixed on previous loops
			//& empty channels (ones which have never containted data), which start with prev_time[i]=-1,
			//are all good by default			

			
			if(databuffer[i].size()==0){
				if(prev_time[i]==-1||failcount>5||drainbuffer){count++;valid=true;}//if empty channel move on
			}//if one buffer is empty after several reads OR has never been used OR it is EOF, then its probably fine
			
			while((databuffer[i].size()>0)&&!valid){//else clear buffer until empty or fixed
				//comparing this current buffer to "oldtime" and "newtime" which were stored when the current TIME BROKEN was recorded
				if(abs(oldtime-timebuffer[i].front())>abs(newtime-timebuffer[i].front())){
					count++;valid=true;
				}else{
					prev_time[i]=timebuffer[i].front();//standard pop procedure 
					timebuffer[i].pop();
					databuffer[i].pop();
					failcount_datasum++;
				}	
			}
			
			if(!valid&&drainbuffer){count++;valid=true;}//if drainbuffer there is no more data so fuck it

		}
		//now have checked all channels
		
		if(count==(lengthofbuffer-1)){//IF fixed
			failcount=0;
			badtime=false;
			timeuncertain=false;
			nexttriggertime=this->minimumtimefrontbuffer();//reset trigger time
			if(readoutdetail>0){cout<<endl<<endl<<" TIME STAMP FIXED : "<<nexttriggertime;
			cout<<endl<<" Data waste >"<<0.00005722*double(failcount_datasum)<<" Mb"<<endl<<endl;}
			failcount_datasum=0;
			
			if(readoutdetail>1)this->timebufferprint();
			
			for(int i=1;i<lengthofbuffer;i++)if(databuffer[i].size()>0)prev_time[i]=timebuffer[i].front();else prev_time[i]=-1;
			//Reset the prevtime or it will complain time still broken.
			//Because the prevtime of a fixed chanel is on the wrong side of the time break.
			//Empty channels have the -1 prevtime flag.
				
		}else{//if its not fixed yet, we need to read more data to some buffers to reach the break
			readrequest=true;
			failcount++;
			if(failcount>10){//if too much failing abandon, may indicate buffer read size too small
				badtime=false;
				cout<<endl<<endl<<" FAILED TO FIX TIME STAMP. DUMP DATA.";
				cout<<endl<<" Data waste >"<<0.00005722*double(failcount_datasum)<<" Mb"<<flush;
				cout<<endl<<" Smaller buffer = reduce waste."<<endl<<" Larger buffer = May fix errors."<<endl<<endl;
							
				if(readoutdetail>1)this->timebufferprint();
				
				this->clearbuffer(false);//we reset the buffer, if any channels are left behind they will be caught by the empty channel routines (as now all are empty channels)
				failcount=0;
				failcount_datasum=0;
				if(readoutdetail>1)timeuncertain=true;
			}
		}
	}else{
	
		////////////////////////////////////////////
		///////EVENT MAKE When time fine make an event
		///////////////////////////////////////////
		
		int all_channels_good=0;

		//LOOP OVER BUFFERS
		for(int i=1;i<lengthofbuffer;i++){
			
			//DEAL WITH GETTING TIMES
			int chanpile=0;
			event[i-1]=0;			//because the zero buffer is waste, buffer [1] -> event [0]
			eventtime[i-1]=timezero;	//timezero is the "if empty" value, leaves a 1 bin gap between event window of null bin
			
			long long timef;//looped time
			long long baset;//base time
			if(databuffer[i].size()>0){
				baset=timebuffer[i].front();
				timef=this->timeifiy(baset);//Clever new thing cycles time based on trigger position past the max time
			}
			
			//LOOP OVER THIS BUFFER TO GET DATA FOR THIS EVENT
			while(		(databuffer[i].size()>0)
				&&	(timef<=triggerend)	){//while there is data before event-end.				

				//CHECK IF THERE IS A TIME ISSUE
				if(baset<prev_time[i]||(prev_time[i]==-1&&baset<trigzeroaverage)){

					//EITHER time is lower than prev time (channel time reset/loop)
					//OR the first value reported in a previously empty channel is significantly lower than average
					//indicating it may actually be AHEAD in real time but over a time reset/loop, so we check to be safe
					
					if(baset<bottomtime&&(toptime<prev_time[i]||(prev_time[i]==0&&toptime<trigzeroaverage))){//normal loop
						//normal clock loop at TMAX nothing special continue with event making
						if(readoutdetail>0) cout<<endl<<"Time reset buffer "<<i<<" : "<<prev_time[i]<<" > "<<baset<<flush;
						if(readoutdetail>1)this->timebufferprint();
			
					} else if(reasoned_resetminium>prev_time[i]&&prev_time[i]!=-1){
						//If reseting while too close to zero the its one of the rare double reset errors and cn just be ignored
						//a few events will be garbage but nothing worse.
						if(readoutdetail>0) cout<<endl<<endl<<"Danger reset skipped, buffer "<<i<<" : "<<prev_time[i]<<" > "<<baset<<" Min = "<<reasoned_resetminium<<endl<<endl;
						if(readoutdetail>1)this->timebufferprint();
						
					} else {//something worse
						if(zerotolerance){//off by default, abandons entire data file
							cout<<endl<<endl<<"TIME STAMP ERROR, buffer purge and file close. Buffer "<<i<<" : "<<prev_time[i]<<" > "<<baset<<endl<<endl;
							file_death=true;
							this->clearbuffer(false);
							readrequest=true;							
						}else{
							if(readoutdetail>0) {//first we print the problem
								if(prev_time[i]!=-1)cout<<endl<<endl<<"TIME STAMP ERROR. Buffer "<<i<<" : "<<prev_time[i]<<" > "<<baset<<" "<<endl<<endl;
								else cout<<endl<<endl<<"TIME STAMP ERROR. Untried Buffer "<<i<<" : "<<trigzeroaverage<<" > "<<baset<<" "<<endl<<endl;
							}else{cout<<" T_E ";}
											
							if(readoutdetail>1)this->timebufferprint();
							
							badtime=true;
							failcount=0;
							failcount_datasum=0;
							
							//set the "oldtime" & "newtime" that are used to adjust all channels over the time reset/loop
							if(prev_time[i]==-1)
								oldtime=trigzeroaverage;
							else
								oldtime=prev_time[i];

							newtime=baset;
						}
						
						//set event to zero
						for(int j=1;j<lengthofbuffer;j++){
							event[j-1]=0;
							eventtime[j-1]=timezero;
						}
						pileup=0;
						multiplicity=0;

						//break from even make loops
						i=lengthofbuffer;
						triggertime=0;
						break;
					}
				}
				
				//IF NO TIME ISSUE, MAKE THE EVENT				

				if(timef>=triggerstart){ //if data after event start
					chanpile++;
					if(event[i-1]==0){//If no data yet
						event[i-1]=databuffer[i].front();//Make it the event data
						if(timeor)eventtime[i-1]=(int)(timef-triggertime);
					}
				}
				prev_time[i]=baset;				
				timebuffer[i].pop(); //it data now either been used or was too early, delete it
				databuffer[i].pop();
				
				if(databuffer[i].size()==0){//if we JUST emptied the buffer a read is requested
					readrequest=true;
					if(readoutdetail>1)cout<<endl<<"Buffer "<<setw(3)<<i<<" empty requesting read. "<<triggertime;
					if(readoutdetail>2)cout<<endl<<"Time:"<< triggertime<<endl;
				}else{//otherwise the next buffer time is loaded to repeat the loop
					baset=timebuffer[i].front();
					timef=this->timeifiy(baset);
				}
			}
			//HAVE CHANNEL DATA FOR THIS EVENT


			
			//Now whatever is left in this buffer channel is AFTER the end time of the event being constructed
			if(databuffer[i].size()>0)all_channels_good++;
			if((databuffer[i].size()>0)&&(triggers[i])){//if there is data AND it is a trigger channel
				if(	(timef>triggerend) //if it is AFTER the ENF of the current event
				&& ((baset<nexttriggertime)||(nexttriggertime==0))	){//If this trigger is far ENOUGH AFTER current
					nexttriggertime=baset;				//AND BEFORE any OTHER trigger, then this is the next trigger.
					newtrig=i;
				}
			}
			if(chanpile>0){multiplicity++;pileup+=chanpile;pileup--;}
		}
		//FINISHED CHANNEL LOOP
		
		//If, after good event construction, all channels have data after the trigger then it is likely any time error has been fixed
		if(timeuncertain&&multiplicity>0){
			if(all_channels_good>=lengthofbuffer-1){
				timeuncertain=false;
				cout<<endl<<endl<<" DATA DUMP APPEARS TO HAVE FIXED TIME ERROR. "<<endl<<endl;
			}
		}


		if(readoutdetail>3)cout<<endl<<" Multi:"<<setw(3)<<multiplicity<<" Pile:"<<setw(3)<<pileup<<" Trig:"<<setw(3)<<currenttriggerchanel<<" Time:"<< triggertime<<flush;

		if(multiplicity==0){//IF NO EVENT WAS MADE, possibily because new file
			nexttriggertime=this->minimumtimefrontbuffer(); //get next trigger or return 0
			if((readoutdetail>1)&&(triggertime!=0))cout<<endl<<"TIME Alighment Break "<<triggertime<<"-"<<nexttriggertime<<". Trig Channel "<<currenttriggerchanel<<flush;
			//if(nexttriggertime==0)this->clearbuffer(false);
			if(nexttriggertime==0)readrequest=true;//If still no trigger, no data request read.
		}
	}
	
	triggertime=nexttriggertime; //trigger info of NEXT event
	triggerstart=triggertime+trigpast;
	triggerend=triggertime+trigfuture;	
	
	currenttriggerchanel=newtrig;
	
	return readrequest;	
}

long long nikibuffer::minimumtimefrontbuffer(){ //return the next trigger times from the front of the buffers
		vector<long long> process;
		long long min;
		for(int i=1;i<lengthofbuffer;i++)if((timebuffer[i].size()>0)&&(triggers[i])) process.push_back(timebuffer[i].front());
		if(process.size()>0)min=process[0];else min=0;
 		for(int i=1;(unsigned)i<process.size();i++)if(process[i]<min)min=process[i];
		
//		for(int i=1;i<50;i++)cout<<" "<<min;
		return min;
}



void nikibuffer::print(){

	vector<int> process;
	int largets=1,sum=0,loopnum=0;
	double mega;

	for(int i=0;i<lengthofbuffer;i++){
		loopnum=databuffer[i].size();
		process.push_back(loopnum);
		if(loopnum>largets)largets=loopnum;
		sum+=loopnum;
	}	
	
	mega=0.00005722*double(sum);
	if(mega>1000||readoutdetail>1)cout<<endl<<"Buffer size >"<<mega<<" Mb";
	
	if(readoutdetail>2){
		for(int i=0;i<lengthofbuffer;i++){
			cout<<endl<<i<<" ";
	//		loopnum=databuffer[i].size()+1;
			if(databuffer[i].size()>0)cout<<"*";
			for(int j=0;double(j)<60*double(process[i])/double(largets);j++)cout<<"*";
		}
		cout<<endl;		
	}


	return;
}


long long nikibuffer::timeifiy(long long buftime){
	if (triggertime!=0){
		if(triggertime>toptime){
			if(buftime<bottomtime)buftime+=fulltime;
		}else if(triggertime<bottomtime){
			if(buftime>toptime)buftime-=fulltime;
		}
	}
	return buftime;
}

void nikibuffer::setwindow(long long triggerl,long long triggerd){
	triggerlength=triggerl;
	triggerdelay=triggerd;
	timezero=triggerlength-triggerdelay+2;	//leaves a 1 bin gap between event window of null bin
	trigpast=-triggerdelay;
	trigfuture=triggerlength-triggerdelay;
}

void nikibuffer::maketrigzeroaverage(){//take an average of the minimum time of all the most intense channels, to check for problems in the least intense
	int limit=0;
	for(int j=1;j<lengthofbuffer;j++)if(timebuffer[j].size()*0.2>(unsigned)limit)limit=timebuffer[j].size()*0.2;
	
	long long count= 0, sum =0, rmin=0, rmax=0,range=0;
	for(int j=1;j<lengthofbuffer;j++)if(timebuffer[j].size()>(unsigned)limit){
		count++;
		sum+=timebuffer[j].front();
		if(timebuffer[j].front()>rmax)rmax=timebuffer[j].front();
		if(timebuffer[j].front()<rmin||rmin==0)rmin=timebuffer[j].front();
	}
	range=rmax-rmin;

	trigzeroaverage=0.9*(sum/count);
	
	reasoned_resetminium=max(triggerlength*1000000,range*1000);
	reasoned_resetminium=min(reasoned_resetminium,(bottomtime/100));
	
	//if the value is too close to zero could be a mistake so dont use
	if(reasoned_resetminium>trigzeroaverage)trigzeroaverage=0; 
	
	return;
}

// Methods
void nikibuffer::timebufferprint()
{	
	cout<<endl;
	for(int j=1;j<lengthofbuffer;j++){
		cout<<endl<<setw(3)<<j<<setw(15)<<prev_time[j]<<" ";
		if(timebuffer[j].size()>0)cout<<setw(15)<<timebuffer[j].front()<<" ";
		else cout<<setw(15)<<"-"<<" ";
// 		if(timebuffer[j].size()>1){
// 			long long temp=timebuffer[j].front();
// 			timebuffer[j].pop();
// 			cout<<setw(15)<<timebuffer[j].front()<<" ";
// 			timebuffer[j].push_front(temp);
// 		}else cout<<setw(15)<<"-"<<" ";
	}
	cout<<endl;
}

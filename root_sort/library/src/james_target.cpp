//
//
//	James Target Object and related functions 2.2
// 	27/02/09		09/02/2015
//	james.smallcombe@outlook.com
//
//

#include "james_target.h"

///////////////////////////////////////////////
/////////// Conversion functions    ///////////
///////////////////////////////////////////////

//If at centre give half thickness
//rotation clockwise while looking down
//thickness is total
//(0,0,1) is beam direction
//(0,1,0) axis of rotation, clockwise from "above"
//direction from 0, rotation in degrees, nominal thickness
double target_effective(TVector3 world,double degees,double thickness){
	TVector3 shldntbndd=world;
	TRotation rot;
	rot.RotateY((-degees/180)*TMath::Pi());
	shldntbndd*=rot;
	double the=shldntbndd.Theta();
	return abs(thickness/(TMath::Cos(the)));
	
}
double target_effective(TVector3 world,TVector3 targ_norm,double thickness){
	double ang= world.Angle(targ_norm);
	if(ang==0)ang=0.00000000000001;
	return abs(thickness/(cos(abs(ang))));
}


//input density in g/cm3 and arial density in ug/cm2
double giveme_um(double p,double a, bool print){
	double t=(a/p)*0.01;
	if(print)cout<<endl<<"Thickness "<<t<<" um."<<flush;
	return t;
}

//input density in g/cm3 and thickness in um
double giveme_areal(double p,double t,bool print){
	double a=(t*p)*100;
	if(print)cout<<endl<<"Areal density "<<a<<" ug/cm^2."<<flush;
	return a;
}

void set_global_compound(vector<double> k){
	
	double n=floor((double)k.size()/3.0);
	//setting commpounds into range.h
	if(n>1){
		if(n>4)n=4;
		nelem = n;	
		for(int m=0;m<n;m++){
			absorb[m].z = round(k[m*3]);
			absorb[m].a = round(k[(m*3)+1]);
			absorb[m].w = k[(m*3)+2];
		}
	}
}


///////////////////////////////////////////////////
/////////// Constructor & Destructor    ///////////
//////////////////////////////////////////////////


target::target(int a, int b, double c, TVector3 d, int e, double g, int h, int i, int j): targ_Z(a), targ_A(b),target_thickness(c),targ_norm(d),targ_fornmal(d),targ_compound(e),backing_thickness(g),backing_Z(h),backing_A(i),backing_compound(j),downstream_back(0),eloss_frag_graph_targ(),eloss_frag_graph_back()
{
// 	stringstream ss;// 	rand.SetSeed();// 	ss << rand.Rndm();// 	randstr=ss.str();
// 	eloss_frag_graph_targ=TH2D(("eloss_frag_graph_targ"+randstr).c_str(),"eloss_frag_graph_targ", 10000,0,1000,2000,0.05,200.05);
// 	eloss_frag_graph_back=TH2D(("eloss_frag_graph_back"+randstr).c_str(),"eloss_frag_graph_back", 10000,0,1000,2000,0.05,200.05);

	if(TVector3(0.0,0.0,1.0).Angle(targ_norm)>(pi/2)){targ_fornmal=-targ_norm;downstream_back=true;} else targ_fornmal=targ_norm;
	for(int j=0;j<4;j++)particle.push_back(new TGraph());
}



// C++11CODE //target::target(int a, int b, double c, double d, int e, double g, int h, int i, int j): target(a,b,c,TVector3(0.0,0.0,1.0),e,g,h,i,j)
target::target(int a, int b, double c, double d, int e, double g, int h, int i, int j): targ_Z(a), targ_A(b),target_thickness(c),targ_norm(0.0,0.0,1.0),targ_fornmal(0.0,0.0,1.0),targ_compound(e),backing_thickness(g),backing_Z(h),backing_A(i),backing_compound(j),downstream_back(0),eloss_frag_graph_targ(),eloss_frag_graph_back()
{
	targ_norm.RotateY((-d/180)*pi);
	if(TVector3(0.0,0.0,1.0).Angle(targ_norm)>(pi/2)){targ_fornmal=-targ_norm;downstream_back=true;} else targ_fornmal=targ_norm;
	for(int j=0;j<4;j++)particle.push_back(new TGraph());
}


target::target( const target &obj){
	targ_Z=obj.targ_Z;
	targ_A=obj.targ_A;
	target_thickness=obj.target_thickness;
	targ_norm=obj.targ_norm;
	targ_fornmal=obj.targ_fornmal;
	targ_compound=obj.targ_compound;
	backing_thickness=obj.backing_thickness;
	backing_Z=obj.backing_Z;
	backing_A=obj.backing_A;
	backing_compound=obj.backing_compound;
	downstream_back=obj.downstream_back;
	
	particle.resize(4);
	for(int j=0;j<4;j++)particle[j] = (TGraph*)obj.particle[j]->Clone();	
//	for(int j=0;j<4;j++)particle.push_back(new TGraph(*(obj.particle[j])));	
	
	eloss_frag_graph_targ=obj.eloss_frag_graph_targ;
	eloss_frag_graph_back=obj.eloss_frag_graph_back;
}

target& target::operator=( const target &obj){
	if(this!=&obj){//to prevent self-assignment errors
		for(int j=0;j<4;j++)delete particle[j];	
		
		targ_Z=obj.targ_Z;targ_A=obj.targ_A;
		target_thickness=obj.target_thickness;	targ_norm=obj.targ_norm;
		targ_fornmal=obj.targ_fornmal;	targ_compound=obj.targ_compound;
		backing_thickness=obj.backing_thickness;backing_Z=obj.backing_Z;
		backing_A=obj.backing_A;backing_compound=obj.backing_compound;
		downstream_back=obj.downstream_back;
		
		particle.resize(4);
		for(int j=0;j<4;j++)particle[j] = (TGraph*)obj.particle[j]->Clone();	
	//	for(int j=0;j<4;j++)particle.push_back(new TGraph(*(obj.particle[j])));	
		
		eloss_frag_graph_targ=obj.eloss_frag_graph_targ;
		eloss_frag_graph_back=obj.eloss_frag_graph_back;
	}
	return (*this); // for cascading assignment
}

target::~target(){
	for(int j=0;j<4;j++)delete particle[j];	
}

target target::inverse(){
	if(backing_thickness>0){
		target rettarg(backing_Z,backing_A,backing_thickness,-targ_norm,backing_compound,target_thickness,targ_Z,targ_A,targ_compound);
	for(int j=0;j<1;j++)rettarg.particle[j] = (TGraph*)this->particle[j+2]->Clone();	
	for(int j=0;j<1;j++)rettarg.particle[j+2] = (TGraph*)this->particle[j]->Clone();		
		rettarg.eloss_frag_graph_targ=this->eloss_frag_graph_back;
		rettarg.eloss_frag_graph_back=this->eloss_frag_graph_targ;
		return rettarg;
	}
	return *this;
}

//////////////////////////////////
/////////// Methods    ///////////
//////////////////////////////////

double target::number_density(){//return partiles/mb
	double fraction=1.0;
	if(targ_compound<0){
		fraction=0;
		double tar=0,sum=0;
		for(int m=0;m<nelem;m++){
			if(targ_Z==absorb[m].z&&targ_A==absorb[m].a)tar=absorb[m].w;
			sum+=absorb[m].w;
		}
		if(tar>0)fraction=tar/sum;
	}
	return target_effective(TVector3(0.0,0.0,1.0),targ_norm,1.0)*target_thickness*fraction*6.02214129E-10/double(targ_A);
}


double target::beam_e_centre(int z,int a,double mev,TVector3 beamv,double fraction_in){
	double enterance=2;
	if(abs(targ_fornmal.Angle(beamv))<pi/2)enterance=-1;
// 	cout<<endl<<startfr<<" "<<fraction_in<<endl;
	return traverse(z,a,mev,beamv,enterance,fraction_in,false);
}

double target::beam_e_centre_reverse(int z,int a,double mev,TVector3 beamv,double fraction_in){
	double enterance=2;
	if(abs(targ_fornmal.Angle(beamv))<pi/2)enterance=-1;	
	return traverse(z,a,mev,beamv,enterance,fraction_in,true);
}

double target::particle_e_exit(int z,int a,double mev,TVector3 traj,double& fraction_in){
	double exit=-1;
	if(abs(targ_fornmal.Angle(traj))<pi/2)exit=2;	
	return traverse(z,a,mev,traj,fraction_in,exit,false);
}

double target::particle_e_reverse(int z,int a,double mev,TVector3 traj,double fraction_in){
	double exit=-1;
	if(abs(targ_fornmal.Angle(traj))<pi/2)exit=2;	
	return traverse(z,a,mev,traj,fraction_in,exit,true);
}

double target::traverse(int z,int a,double mev_in,TVector3 traj,double startfr,double endfr,bool reverse){
	
	double mev=abs(mev_in);	
	if(mev<0.001&&!reverse)return 0.0;//avoid some problems

	double * tempd= new double(0);
	double eff_thick=target_effective(targ_norm,traj,1.0);
	double pfrac=1;
			
	if(reverse){double a=startfr;startfr=endfr;endfr=a;traj-=traj;}

	bool flip=false;
	if(abs(targ_fornmal.Angle(traj))>pi/2){//going out the back (upstream). Counts through 3 layers, what order they are in depends on direction relative to target orinations
		flip=true;
		startfr=1-startfr;
		endfr=1-endfr;
	}
	if(startfr>endfr){double a=startfr;startfr=endfr;endfr=a;}
	
	if(!(downstream_back^flip)&&startfr<0&&backing_thickness>0){//backing layer 1
			pfrac=-startfr;
			if(endfr<0)pfrac+=endfr;
			if(reverse) mev=egassap(0,z,a,backing_compound,backing_Z,backing_A,eff_thick*(backing_thickness/1000)*pfrac,mev,tempd);
			else mev=passage(0,z,a,backing_compound,backing_Z,backing_A,mev,eff_thick*(backing_thickness/1000)*pfrac,tempd);
	}
	if(startfr<1&&endfr>0){//target, layer 2
			if(startfr<0)startfr=0;
			if(endfr<1)pfrac=endfr-startfr;
			else pfrac=1-startfr;	
			if(reverse) mev=egassap(0,z,a,targ_compound,targ_Z,targ_A,eff_thick*(target_thickness/1000)*pfrac,mev,tempd);
			else mev=passage(0,z,a,targ_compound,targ_Z,targ_A,mev,eff_thick*(target_thickness/1000)*pfrac,tempd);
	}		
	if((downstream_back^flip)&&backing_thickness>0&&endfr>1){//layer 3
			if(startfr<1)startfr=1;
			if(endfr<2)pfrac=endfr-startfr;
			else pfrac=2-startfr;
			if(reverse) mev=egassap(0,z,a,backing_compound,backing_Z,backing_A,eff_thick*(backing_thickness/1000)*pfrac,mev,tempd);
			else mev=passage(0,z,a,backing_compound,backing_Z,backing_A,mev,eff_thick*(backing_thickness/1000)*pfrac,tempd);
	}
	
	if(mev<0.001)return 0.0;//avoid some problems
	if(!(reverse^(mev_in>mev))){return mev_in;}
	delete tempd;
	return mev;	
}

void target::set_particle(int Z,int A){
	//reset
	for(int j=0;j<4;j++) *particle[j]= TGraph();
	
	double E=500.0;
	double xt=E,yt=1E10;
	double xb=E,yb=1E10;
	bool t=false,b=false;
	double R=0;
	while(E>0.050){
		if(target_thickness>0){
			if(!t)R=rangen(0,Z,A,targ_compound,targ_Z,targ_A,E)*1000;
			if(!(R<yt&&R>0)||t){R=E*(yt/xt);t=true;}//if stupid value produced, track linearly to zero
			particle[0]->SetPoint(particle[0]->GetN(),R,E);
			particle[1]->SetPoint(particle[1]->GetN(),E,R);
			xt=E;yt=R;
		}
		
		if(backing_thickness>0){
			if(!b)R=rangen(0,Z,A,backing_compound,backing_Z,backing_A,E)*1000;
			if(!(R<yb&&R>0)||b){R=E*(yb/xb);b=true;}
			particle[2]->SetPoint(particle[2]->GetN(),R,E);
			particle[3]->SetPoint(particle[3]->GetN(),E,R);
			xb=E;yb=R;
		}
		E*=0.95;
	}
	
	//add the very important zerpoint
	for(int j=0;j<4;j++)particle[j]->SetPoint(particle[j]->GetN(),0,0);
}

double target::get_range_particle(bool back,double mev){
	if(mev>0){
		if(back)return particle[3]->Eval(mev);
		else return particle[1]->Eval(mev);
	}
	return 0.0;
}

double target::passage_particle(bool back,double mev,double ugcm2,bool reverse){
	if(mev>0){
		double R=this->get_range_particle(back,mev);
		if(R>ugcm2){
			double i=-1; if(reverse)i=1;
			if(back)return particle[2]->Eval(R+i*ugcm2);
			else return particle[0]->Eval(R+i*ugcm2);
		}
	}
	return 0.0;
}

double target::traverse_p(double mev_in,TVector3 traj,double startfr,double endfr,bool reverse){
	return this->traverse_p(mev_in,target_effective(targ_norm,traj,1.0),startfr,endfr,reverse);
}

double target::traverse_p(double mev_in,double eff,double startfr,double endfr,bool reverse){
	double mev=abs(mev_in);
	if(mev<0.001&&!reverse)return 0.0;//avoid some problems

	double pfrac=1;
			
	if(reverse){double a=startfr;startfr=endfr;endfr=a;}

	bool flip=false;
	if(endfr<startfr){//going out the back (upstream). Counts through 3 layers, what order they are in depends on direction relative to target orinations
		flip=true;
		startfr=1-startfr;
		endfr=1-endfr;
	}
	if(startfr>endfr){double a=startfr;startfr=endfr;endfr=a;}
	
	if(!(downstream_back^flip)&&startfr<0&&backing_thickness>0){//backing layer 1
			pfrac=-startfr;
			if(endfr<0)pfrac+=endfr;
			mev=this->passage_particle(true,mev,eff*(backing_thickness/1000)*pfrac,reverse);
	}
	if(startfr<1&&endfr>0){//target, layer 2
			if(startfr<0)startfr=0;
			if(endfr<1)pfrac=endfr-startfr;
			else pfrac=1-startfr;	
			mev=this->passage_particle(false,mev,eff*(backing_thickness/1000)*pfrac,reverse);
	}		
	if((downstream_back^flip)&&backing_thickness>0&&endfr>1){//layer 3
			if(startfr<1)startfr=1;
			if(endfr<2)pfrac=endfr-startfr;
			else pfrac=2-startfr;
			mev=this->passage_particle(true,mev,eff*(backing_thickness/1000)*pfrac,reverse);
	}
	
	if(mev<0.001)return 0.0;//avoid some problems
	if(!(reverse^(mev_in>mev))){return mev_in;}
	return mev;	
}


//Use TH2D not graph2D because interpolate on graph is too slow
void target::fragment_set(double Z,double A){
	stringstream ss;
	ss << targ_Z << "_" <<targ_A<<"_"<<(int)target_thickness<<"_"<<(int)Z<<"_"<<(int)A;
	string extrastr=ss.str();	
	
	
	//If its already been made it'll be stored in the run directory
	TFile* qfile = new TFile(("eloss_store"+extrastr+".root").c_str(),"READ");
	if(qfile->IsOpen()){
		TH2D *h2a=0,*h2b=0;
		h2a = (TH2D*)qfile->Get("eloss_frag_graph_targ");
		h2b = (TH2D*)qfile->Get("eloss_frag_graph_back");
		
		if(h2a&&h2b){
			this->eloss_frag_graph_targ= *h2a;
			this->eloss_frag_graph_targ.SetName(("eloss_frag_graph_targ"+extrastr).c_str());
			this->eloss_frag_graph_back= *h2b;
			this->eloss_frag_graph_back.SetName(("eloss_frag_graph_back"+extrastr).c_str());
			cout<<endl<<endl<<"READ FRAGMENT E LOSS TO TARGET. DELETE eloss_store.root IF UNDESIRED."<<endl<<endl;	
			qfile->Close();
			delete qfile;
			return;
		}
		
		qfile->Close();
	}
	delete qfile;
	
	//otherwise create new ones
	

	eloss_frag_graph_targ=TH2D(("eloss_frag_graph_targ"+extrastr).c_str(),"eloss_frag_graph_targ", 10000,0,1000,2000,0.05,200.05);
	eloss_frag_graph_back=TH2D(("eloss_frag_graph_back"+extrastr).c_str(),"eloss_frag_graph_back", 10000,0,1000,2000,0.05,200.05);
	
	double ratio=Z/A;
	
	//create the var hist
	//82
	//double xbins[]={0.05,0.15,0.25,0.35,0.45,0.55,0.65,0.75,0.85,0.95,1.25,1.75,2.25,2.75,3.25,3.75,4.25,4.75,5.25,6.5,7.5,8.5,9.5,10.5,11.5,12.5,13.5,14.5,15.5,16.7,17.5,18.5,19.5,21,23,25,27,29,31,33,35,37,39,41,46,50,54,58,62,66,70,74,78,82,95,105,115,125,135,145,155,165,175,185,195,210,230,250,270,290,310,330,350,370,390,425,475,550,650,750,850,950,1050};
	double xbins[347];
	for(int z=0;z<=199;z++)xbins[z]=0.1*(double)z;
	for(int z=0;z<=60;z++)xbins[z+200]=(0.5*(double)z)+20;
	for(int z=0;z<=50;z++)xbins[z+260]=((double)z)+50;
	for(int z=0;z<=40;z++)xbins[z+300]=(10*(double)z)+100;
	for(int z=0;z<=5;z++)xbins[z+340]=(100*(double)z)+500;
	xbins[346]=1001;
	TH2D varhista("varhista","varhista", 346,xbins,200,0.5,200.5);	
	TH2D varhistb("varhistb","varhistb", 346,xbins,200,0.5,200.5);
	
	//For targ material
	//calc the z averages E loss	

	for(int a=9;a<=200;a++){
		double* tempd=new double(0);
		double idealZ=(double)a*ratio;
		int z=round(idealZ);
		double weight_sum=0;
		double ZZ[5];
		for(int k=-2;k<=2;k++){
			ZZ[k+2]=TMath::Gaus(z+k,idealZ);
			weight_sum+=ZZ[k+2];
		}
		int f=0,l=4;
		if(abs(idealZ-(double)z)>0.25){weight_sum-=ZZ[0];f=1;}
		if(abs(idealZ-(double)z)<-0.25){weight_sum-=ZZ[4];l=3;}

		for(int i=1;i<=346;i++){//346
			double Emev=varhista.GetXaxis()->GetBinCenter(i);
			double en=0;
			for(int k=f;k<=l;k++){
				en+=(Emev-passage(0,z+k-2,a,targ_compound,targ_Z,targ_A,Emev,1.1*(target_thickness/1000),tempd))*ZZ[k];//*(3-abs(k));
			}
			en/=weight_sum;			
			if(en>Emev)en=Emev;
			if(en<0)en=0;
			varhista.SetBinContent(i,a,en);
		}	
		delete tempd;
		// 	was split into 2 loops because it was really slow otherwise for some reason
		tempd=new double(0);
		for(int i=1;i<=346;i++){//346
			double Emev=varhistb.GetXaxis()->GetBinCenter(i);
			double en=0;
			for(int k=f;k<=l;k++){
				en+=(Emev-passage(0,z+k-2,a,backing_compound,backing_Z,backing_A,Emev,1.1*(backing_thickness/1000),tempd))*ZZ[k];//*(3-abs(k));
			}
			en/=weight_sum;			
			if(en>Emev)en=Emev;
			if(en<0)en=0;
			varhistb.SetBinContent(i,a,en);	
		}
		delete tempd;	
	}

	//copy into a more fleshed out even histogram
	for(int i=1;i<=10000;i++){
		double Emev=(0.05+(0.1*i));
		for(int j=90;j<=2000;j++){
			double mass=(0.1+(0.1*j));
			double ena=varhista.Interpolate(Emev,mass);
			eloss_frag_graph_targ.SetBinContent(i,j,ena);			
			double enb=varhistb.Interpolate(Emev,mass);
			eloss_frag_graph_back.SetBinContent(i,j,enb);
		}
	}
	
	eloss_frag_graph_targ.Smooth();	
	eloss_frag_graph_targ.Smooth();	
	eloss_frag_graph_targ.Smooth();			
				
	eloss_frag_graph_back.Smooth();
	eloss_frag_graph_back.Smooth();
	eloss_frag_graph_back.Smooth();
	
	//write it for saves
	TFile* rfile = new TFile(("eloss_store"+extrastr+".root").c_str(),"RECREATE");
		eloss_frag_graph_targ.Write("eloss_frag_graph_targ");
		eloss_frag_graph_back.Write("eloss_frag_graph_back");
	rfile->Close();
	delete rfile;
}

double target::fragment_e_exit(double mass,double energy,TVector3 traj,double fraction_in){
	if(fraction_in<=0)fraction_in=0.5;
	if(fraction_in!=0.5)if(abs(targ_fornmal.Angle(traj))<pi/2)fraction_in=1-fraction_in;//going out the front
	
	if(mass>=5&&mass<200&&energy>0&&energy<1000){
		bool backing_traversed=false;
		if(backing_thickness>0 && (abs(targ_norm.Angle(traj))>pi/2)){backing_traversed=true;}
		
		double ebeam_slow=energy;
		double eff_thick=target_effective(traj,targ_norm,(1.0/1.1));
		
		ebeam_slow=ebeam_slow-(eloss_frag_graph_targ.Interpolate(ebeam_slow,mass)*eff_thick*fraction_in);		
		if(ebeam_slow<=0)return 0;
		if(backing_traversed)ebeam_slow=ebeam_slow-(eloss_frag_graph_back.Interpolate(ebeam_slow,mass)*eff_thick);
		if(ebeam_slow<=0)return 0;
		return ebeam_slow;	
	}
	return 0;
} 
 
 
//target W_186_straight(74,186,140.0,180.0,0,60.2,6,12);
//
//
//	James General Functions version 1.0
// 	27/02/09		09/02/2015
//	james.smallcombe@outlook.com
//
//

#include "james_general.h"


void swap_jd(double &a,double &b){double x=a;a=b;b=x;}

/////////////////////////////////////////////////////
/////////// Trig functions for lazy typing   ////////
/////////////////////////////////////////////////////

double Acos(double in){
	return acos(in);
}
double cos2(double in){
	return cos(in)*cos(in);
}

double Asin(double in){
	return asin(in);
}
double sin2(double in){
	return sin(in)*sin(in);
}

double Atan(double in){
	return atan(in);
}
double tan2(double in){
	return tan(in)*tan(in);
}


  /////////////////////////////////////////////
 /////////// Angle Difference calcs   ////////
/////////////////////////////////////////////

double angledifference(double A,double B){ //Angles in Radians
	while(A<0)A+=(2*TMath::Pi());
	while(B<0)B+=(2*TMath::Pi());
	while(A>(2*TMath::Pi()))A-=(2*TMath::Pi());
	while(B>(2*TMath::Pi()))B-=(2*TMath::Pi());
	
	double dphi=TMath::Abs(A-B);
	if(dphi>TMath::Pi())dphi=(2*TMath::Pi())-dphi;
	
	return dphi;
}

double angledifference_signed(double A,double B){ //Angles in Radians
	while(A<0)A+=(2*TMath::Pi());
	while(B<0)B+=(2*TMath::Pi());
	while(A>(2*TMath::Pi()))A-=(2*TMath::Pi());
	while(B>(2*TMath::Pi()))B-=(2*TMath::Pi());

	double dphi=A-B;
	if(dphi>TMath::Pi())dphi=(2*TMath::Pi())-dphi;
	if(dphi<-TMath::Pi())dphi=-((2*TMath::Pi())+dphi);
	
	return dphi;
}


double happy_phi(double phi)
{
	phi+=pi/2;
	while(phi<0)phi+=2*pi;
	return phi;
}

double happy_phi(TVector3 &vec)
{
	return happy_phi(vec.Phi());
}


 int mass_loaded=0;
 double mass_table[128][256];

double get_mass_excess(int z,int n){
	
	if(mass_loaded==0){
		#include <mass_table.hh>
		mass_loaded=1;
	}

	if(z>=0&&z<128&&n>=0&&n<256){
		if(n>0 && z>0)		return mass_table[z][n];
		else	return (mass_table[0][1]*n)+(mass_table[1][0]*z);
	}	
	return 0;
}



///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/////////// Functions for converting detector data to hit info   /////////// 
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


////////////////////////////////////////
/////////// MWPC functions   /////////// 
////////////////////////////////////////



bool make_limit_position(int val,int down,int up){
	int range=up-down;
	int upper=up+(range*0.04);
	int downer=down-(range*0.04);
	
	if(val>downer&&val<upper)return true;
	return false;
}

TVector3 make_det_position_new(bool& constructed,int* limits,Int_t x1,Int_t x2,Int_t y)
{
	constructed=false;
	TVector3 vec(0,0,0);
	
	if(
		(  make_limit_position(x1,limits[0],limits[1]) ^  make_limit_position(x2,limits[2],limits[3])  ) 
		&& make_limit_position(y,limits[4],limits[5])
	){		
		
		constructed=true;
		double XX=0,YY=0;
		
		if(make_limit_position(x2,limits[2],limits[3]))
			XX=(((((double)(x2-limits[2])/(double)(limits[3]-limits[2])))-1)*(196))-2;
		else
			XX=(((double)(x1-limits[0])/(double)(limits[1]-limits[0]))*(196))+2;
		
		YY=(((double)(y-limits[4])/(double)(limits[5]-limits[4]))*(196))-98;
		vec=TVector3(0,YY,XX);
	}	

	return vec;
}

TVector3 make_short_position_new(bool& constructed,int* limits,Int_t x1,Int_t y)
{
	constructed=false;
	TVector3 vec(0,0,0);
	
	if(
		make_limit_position(x1,limits[0],limits[1]) &&  make_limit_position(y,limits[2],limits[3])
	){		
		constructed=true;
		double XX=0,YY=0;
		
		XX=(((double)(x1-limits[0])/(double)(limits[1]-limits[0]))*(196))-98;
		
		YY=(((double)(y-limits[2])/(double)(limits[3]-limits[2]))*(196))-98;
		vec=TVector3(XX,YY,0);
	}	
	
	return vec;
}


silicon_hit::silicon_hit(vector< double > & data,vector< double > & times )	:	mult(0),localvector(),worldhit(),locald(),worldd(){	
	
	//Make all pairs	
	for(int i=0;i<16;i++){
		if(data[i]>0)for(int j=32;j<38;j++){
			if(data[j]>0){
				timeE.push_back(times[i]);
				timedE.push_back(times[j]);
				E.push_back(data[i]);
				dE.push_back(data[j]);
				pixr.push_back(i+1);
				pixphi.push_back(j-31);
			}			
		}
	}	
	for(int i=16;i<32;i++){
		if(data[i]>0)for(int j=38;j<44;j++){
			if(data[j]>0){
				timeE.push_back(times[i]);
				timedE.push_back(times[j]);
				E.push_back(data[i]);
				dE.push_back(data[j]);
				pixr.push_back(i-15);
				pixphi.push_back(j-31);
			}
		}
	}		
	
	//Make vectors
	for(int i=0;(unsigned)i<E.size();i++){
		mult++;
		
		phi.push_back(((TMath::Pi()/6)*pixphi[i])-(TMath::Pi()/12));
		r.push_back(23.25+(pixr[i]*1.5));
		
		double upr=r[i]+0.6;
		double dwnr=r[i]-0.6;
		TRandom3 rnd(0);

		
		rd.push_back(sqrt(((upr*upr-dwnr*dwnr)*rnd.Rndm())+(dwnr*dwnr)));
		phid.push_back(phi[i]+((rnd.Rndm()-0.5)*(((TMath::Pi()*rd[i]/6)-1)/rd[i])));

		localvector.push_back(TVector3(TMath::Sin(phi[i])*r[i],TMath::Cos(phi[i])*r[i],0.0));
		locald.push_back(TVector3(TMath::Sin(phid[i])*rd[i],TMath::Cos(phid[i])*rd[i],0.0));
		
		worldhit.push_back(localvector[i]);
		worldd.push_back(locald[i]);
		
		EdEt.push_back(timedE[i]-timeE[i]);
	}	

}

silicon_hit::silicon_hit(){
	mult=0;
}

silicon_hit::~silicon_hit(){};   

   
void silicon_hit::setworld(TVector3 pos,TRotation rot){
	for(int i=0;(unsigned)i<localvector.size();i++){
		worldhit[i]=(rot*localvector[i])+pos;
		worldd[i]=(rot*locald[i])+pos;		
	}


}



/////////////////////////////////////////////
//////// PHYSICS
////////////////////////////////////////////


  /////////////////////////////////////////
 ////////// Functionallity Tools ///////// 
/////////////////////////////////////////

TLorentzVector make_lorentzvec(TVector3 mom,double mass){
	return TLorentzVector(mom,get_relE_mom(mom.Mag(),mass));
}	

void alight_to_Z(TVector3 beam,TVector3& toadjust){
	if(beam.Theta()>0.000000000000000001){
		TRotation rot;
		rot.RotateZ(-beam.Phi());
		rot.RotateY(-beam.Theta());
		toadjust*=rot;	
	}
	return;
}


  ///////////////////////////////////////////////////////////////
 ////////// Single Body Relativistic  E & P calculators  ///////
///////////////////////////////////////////////////////////////

double get_relE_mom(double mom,double Amass){
	double mass=(jam_phys_amu*Amass);
	return sqrt((mom*mom)+(mass*mass));
}

double get_relE_KE(double KE,double Amass){
	double mass=(jam_phys_amu*Amass);
	return KE+mass;
}

double get_rel_mom(double emev,double Amass,bool overkill){//input particle KE in MeV and mass in amu
	double mass=(jam_phys_amu*Amass);
	double mom=emev+mass;
	mom*=mom;
	mom-=(mass*mass);
	mom=sqrt(mom);
	if(overkill) cout<<endl<<"Relativistic momentum = "<<mom<<" MeV/c. Classical = "<<sqrt(emev*2*mass);
	return mom;//momentum in MeV/c
}
	
double get_KE(double mom,double Amass,bool overkill){//input particle momentum in MeV/c and mass in amu
	double mass=(jam_phys_amu*Amass);
	double emev=sqrt(mom*mom+mass*mass)-mass;
	if(overkill) cout<<endl<<"KE = "<<emev<<" MeV. Classical = "<<(mom*mom)/(2*mass);
	return emev;//KE in MeV
}


double get_KE(TLorentzVector* four){//input Ep 4vec in MeV
	return four->E()-four->M();//KE in MeV
}


   //////////////////////////////////////////////////////
  ////////// Beta and Gamma calculators          ///////
 ////////// single particle in frame velocities /////// 
////////////////////////////////////////////////////// 

double get_gamma_KE(double emev,double Amass){//input particle KE in MeV and mass in amu
	double mass=(jam_phys_amu*Amass);
	double gamma=(emev/mass)+1;
	return gamma;
}

double get_gamma_mom(double mom,double Amass){//input particle KE in MeV and mass in amu
	double beta=get_beta_mom(mom,Amass);
	double gamma=1/sqrt(1-(beta*beta));
	
	return gamma;
}
double get_beta_KE(double emev,double Amass){//input particle KE in MeV and mass in amu
	double gamma=get_gamma_KE(emev,Amass);
	double beta=1/gamma;
	beta*=beta;
	beta=sqrt(1-beta);
	return beta;
}

double get_beta_mom(double mom,double Amass){//input particle momentum in MeV/c and mass in amu
	double mass=(jam_phys_amu*Amass);
	double beta=1/sqrt(((mass*mass)/(mom*mom))+1);
	return beta;//in frame velocity in C
}


   ////////////////////////////////////
  ///////// Velocity of CoM    ///////
 /////////   (Boost beta)     ///////
//////////////////////////////////// 

double get_com_beta(double mom,double Amass,double Bmass){//input MeV/c and mass in amu
	return mom/(get_relE_mom(mom,Amass)+get_relE_mom(0,Bmass));
}

double get_com_beta(TVector3 momA,double massA,TVector3 momB,double massB){
	return (momA+momB).Mag()/(get_relE_mom(momA.Mag(),massA)+get_relE_mom(momB.Mag(),massB));
}


    ///////////////////////////////////////////
   ///////// Product Momentum Split    ///////
  /////////   known trajectories      ///////
 /////////   Momentum conservation   ///////
///////////////////////////////////////////


vector< double > mom_conserve_unit(TVector3 vecZ,TVector3 vecA,TVector3 vecB){
	vector< double > ret(2);
	
	//assumes a perfect plane
	double theta1 = vecZ.Angle(vecA);
	double theta2 = vecZ.Angle(vecB);
	
	//these new lines of code average a bit for imperfect planes
	double tot=abs(vecB.Angle(vecA));
	double fract=theta1/(theta1+theta2);
	theta1=0.5*(theta1+fract*tot);
	theta2=0.5*(theta2+(1-fract)*tot);
	
	ret[0]=1/(TMath::Cos(theta1)+(TMath::Sin(theta1)/TMath::Tan(theta2)));		
	ret[1]=1/(TMath::Cos(theta2)+(TMath::Sin(theta2)/TMath::Tan(theta1)));
	return ret;
}

vector< double > mom_conserve_split(TVector3 vecZ,TVector3 vecA,TVector3 vecB){
	vector< double > ret= mom_conserve_unit(vecZ,vecA,vecB);
	
	double mom=vecZ.Mag();
	ret[0]=ret[0]*mom;
	ret[1]=ret[1]*mom;
	
	return ret;
}

vector< double > mom_conserve_unit(TVector3 vecZ,TVector3 vecA,TVector3 vecB,TVector3 vecC){
	vector< double > ret(3);
	
	double thetaA = vecZ.Angle(vecA);
	double thetaB = vecZ.Angle(vecB);
	double thetaC = vecZ.Angle(vecC);
	TVector3 flatA = vecA-(TMath::Cos(thetaA)*vecA.Mag())*vecZ.Unit();
	TVector3 flatB = vecB-(TMath::Cos(thetaB)*vecB.Mag())*vecZ.Unit();
	TVector3 flatC = vecC-(TMath::Cos(thetaC)*vecC.Mag())*vecZ.Unit();
	double phiA = flatC.Angle(flatA);
	double phiB = flatC.Angle(flatB);

	//length MA=1
	double MB=(TMath::Sin(phiA)*flatA.Mag())/(TMath::Sin(phiB)*flatB.Mag());
	double MC=((flatA+(flatB*MB)).Mag())/(flatC.Mag());
		
	double A=TMath::Cos(thetaA)*vecA.Mag();
	double B=TMath::Cos(thetaB)*vecB.Mag()*MB;
	double C=TMath::Cos(thetaC)*vecC.Mag()*MC;
	
	ret[0]=vecA.Mag()/(A+B+C);
	ret[1]=(vecB.Mag()*MB)/(A+B+C);
	ret[2]=(vecC.Mag()*MC)/(A+B+C);

 	if(ret[0]<0||ret[1]<0||ret[2]<0){ret[0]=0;ret[1]=0;ret[2]=0;}
	
	return ret;
}

vector< double > mom_conserve_split(TVector3 vecZ,TVector3 vecA,TVector3 vecB,TVector3 vecC){
	vector< double > ret= mom_conserve_unit(vecZ,vecA,vecB,vecC);
	
	double mom=vecZ.Mag();
	ret[0]=ret[0]*mom;
	ret[1]=ret[1]*mom;
	ret[2]=ret[2]*mom;
	
	return ret;
}


vector< double > fission_compound_unit(TVector3 compound,TVector3 A,TVector3 B,TVector3 beam){
	vector< double > ret(2);
	alight_to_Z(beam,compound);
	alight_to_Z(beam,A);
	alight_to_Z(beam,B);
	double theta1=A.Theta();
	double theta2=B.Theta();
	double theta5=compound.Theta();
	double phinaught=angledifference(A.Phi(),B.Phi());
	
	double pA1=0;
	double pA2=0;
	double pB1=0;
	double pB2=0;

	double temp_par=2*sin(theta1)*cos(phinaught)*tan(theta2);
	double AAA=1-cos(theta1)*temp_par+(cos2(theta1)*(tan2(theta2)-1));
	double BBB=temp_par-(2*cos(theta1)*tan2(theta2));
	BBB*=cos(theta5);
	double CCC=cos2(theta5)*tan2(theta2)-sin2(theta5);

	if(BBB*BBB>=4*AAA*CCC){
		pA1=(sqrt((BBB*BBB)-(4*AAA*CCC))-BBB)/(2*AAA);
		pA2=(-sqrt((BBB*BBB)-(4*AAA*CCC))-BBB)/(2*AAA);
		pB1=((cos(theta5)/cos(theta2)))-pA1*(cos(theta1)/cos(theta2));
		pB2=((cos(theta5)/cos(theta2)))-pA2*(cos(theta1)/cos(theta2));
		
	}
	
	TVector3 ma=(A.Unit()*pA1+B.Unit()*pB1);
	TVector3 mb=(A.Unit()*pA2+B.Unit()*pB2);
	
	double ca=ma.Unit().Dot(compound.Unit());
	double cb=mb.Unit().Dot(compound.Unit());
	
	if((pA2<=0||pB2<=0||ca>=cb)&&pA1>0&&pB1>0){
		ret[0]=pA1;
		ret[1]=pB1;
	}
		
	if((pA1<=0||pB1<=0||ca<cb)&&pA2>0&&pB2>0){
		ret[0]=pA2;
		ret[1]=pB2;
	}

	return ret;
}

vector< double > fission_compound_split(TVector3 compound,TVector3 A,TVector3 B,TVector3 beam){
	vector< double > ret= fission_compound_unit(compound,A,B,beam);

	double mom=compound.Mag();
	ret[0]=ret[0]*mom;
	ret[1]=ret[1]*mom;
	
	return ret;
}
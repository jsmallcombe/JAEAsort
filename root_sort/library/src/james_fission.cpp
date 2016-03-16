//
//
//	james_fission version 1.3
// 	27/02/09		09/02/2015
//	james.smallcombe@outlook.com
//
//

#include "james_fission.h"



//////////////////////////////////////////////////////////
/////////// Fission Fragment mass calculations /////////// 
//////////////////////////////////////////////////////////


double* mass_split_calc(TVector3 worldhit1,TVector3 worldhit2,TVector3 beamP,double newT,target *targ,double massBero,int reps,TGraph massadj){
	vector< double > pass = mom_conserve_split(beamP,worldhit1,worldhit2);
	return mass_split_calc(worldhit1,worldhit2,pass[0],pass[1],newT,targ,massBero,reps,massadj);
}

//targ_thick[3] angle, clockwise from above in degrees
//ug/cm2 degrees front = upstream
double* mass_split_calc(TVector3 worldhit1,TVector3 worldhit2,double mom1,double mom2,double newT,target *targ,double massBero,int reps,TGraph massadj){
	double* mass= new double[2];

	double distA=(worldhit1.Mag()/1000);
	double distB=(worldhit2.Mag()/1000);
	double E_post_targ1,E_post_targ2,P_post_targ1,P_post_targ2;
	
	//SIMPLE MASS CALCULATION MOSTLY IGNORING TARGET AND SOME OF RELATIVITY
	double s1=distA/(mom1);// s/p
	double s2=distB/(mom2);
	mass[0]=(((massBero*jam_phys_amu*s2/jam_phys_speed_c)+newT)/((s1+s2)/jam_phys_speed_c))/jam_phys_amu;//calc mass
	if(mass[0]<0)mass[0]=0;
	if(mass[0]>massBero)mass[0]=massBero;	
	mass[1]=massBero-mass[0];

	for(int i=1;i<=reps;i++){
		double mca=mass[0];
		double mcb=mass[1];		

		if(massadj.GetN()>0)massBero=massadj.Eval(mass[0]);
		
		E_post_targ1=get_KE(mom1,mass[0]);
		E_post_targ2=get_KE(mom2,mass[1]);
		
		E_post_targ1=targ->fragment_e_exit(mass[0],E_post_targ1,worldhit1);
		E_post_targ2=targ->fragment_e_exit(mass[1],E_post_targ2,worldhit2);
					
		//Convert those energiEs back to momenta
		P_post_targ1=get_rel_mom(E_post_targ1,mass[0]);
		P_post_targ2=get_rel_mom(E_post_targ2,mass[1]);
		
		//re_calc
		s1=(distA/P_post_targ1)*get_gamma_KE(E_post_targ1,mass[0]);// s*gamma/p
		s2=(distB/P_post_targ2)*get_gamma_KE(E_post_targ2,mass[1]);
		mass[0]=(((massBero*jam_phys_amu*s2/jam_phys_speed_c)+newT)/((s1+s2)/jam_phys_speed_c))/jam_phys_amu;//calc mass		
		mass[1]=massBero-mass[0];
						
		if((int)(mass[0]*100)==(int)(mca*100)&&(int)(mass[1]*100)==(int)(mcb*100)){break;}
		
	}
	///cout<<endl<<endl<<endl;
	return mass;
}
	
TGraph mass_convert(int inZ,int inA){
	
	double Efull=(double)inA*jam_phys_amu;
	
	double ratio=(double)inZ/(double)inA;
	TGraph graph;
	for(int j=9;j<=inA-9;j++){
		double dsum=0;
		double ssum=0;
		double ta=(double)j*jam_phys_amu;
		double zideal=(double)j*ratio;
		int za=round(zideal);
		int na=j-za;		
		int zb=inZ-za;
		int nb=inA-za-zb-na;
		
		double aa=0;
		for(int i=-2;i<3;i++){
			double a=TMath::Gaus(za+i,zideal);
			aa+=a;
			double x=get_mass_excess(za+i,na-i)*a;
			dsum+=x;
			ssum+=x;
			dsum+=get_mass_excess(zb-i,nb+i)*a;
		}
		ssum/=aa;
		dsum/=aa;
	
		graph.SetPoint(graph.GetN(),(ta+ssum)/jam_phys_amu,(Efull+dsum)/jam_phys_amu);		
	}
	
	
	//actually the new Z averageing makes this smoothing almost un-needed
	for(int i=0;i<2;i++){
		TGraph Rgraph;
		double x,y;
		int NN=graph.GetN();
		
		for(int j=0;j<NN;j++){
			double averagedpoint=0;
			
			if(NN-1!=j&&j!=0){
				graph.GetPoint(j-1,x,y);		
				averagedpoint+=y;
				graph.GetPoint(j+1,x,y);		
				averagedpoint+=y;
			}
			graph.GetPoint(j,x,y);		
			averagedpoint+=y;		
			if(NN-1!=j&&j!=0)averagedpoint/=3;

			Rgraph.SetPoint(Rgraph.GetN(),x,averagedpoint);		
		}
		graph=Rgraph;
	}
	
	return graph;
}



//////////////////////////////////////////////////////////////////////////////////
/////////// Mostly obsolete functions to calc energy loss from a graph /////////// 
//////////////////////////////////////////////////////////////////////////////////


TH2D* eloss_mass_grapher(double emax,double compound_Z, double compound_A, double nomthick,int targ_type,
	int targ_Z,int targ_A,int nelm,
	int Z1,int A1,double W1,
	int Z2,int A2,double W2,
	int Z3,int A3,double W3,
	int Z4,int A4,double W4){

	if(targ_type<0){
		nelem = nelm;
		absorb[0].z = Z1; absorb[0].a = A1; absorb[0].w = W1;
		absorb[1].z = Z2; absorb[1].a = A2; absorb[1].w = W2;
		absorb[2].z = Z3; absorb[2].a = A3; absorb[2].w = W3;
		absorb[3].z = Z4; absorb[3].a = A4; absorb[3].w = W4;
	}
	
	string titl="range_2D_";
	ostringstream convert;
	convert << nomthick;
	titl+=convert.str();

	double* tempd=new double(0);
	double ratio=compound_Z/compound_A;
	
	
	TH2D* graph = new TH2D(titl.c_str(),titl.c_str(),(int)compound_A,0.5,compound_A+0.5,800,(emax/1600),emax+(emax/1600));
	
	for(int i=1;i<compound_A+0.5;i++){
		for(double j=emax;j>(emax/800);j-=emax/800){	
			double en=j-passage(0,ratio*i,i,targ_type,targ_Z,targ_A,j,(nomthick)/1000,tempd);
			if(!(en>0))en=0;
			graph->SetBinContent(graph->FindBin(i,j),en);
		}
	}
	delete tempd;
	
	return graph;
}


{
	//
	// BEAM DATA
	//
	
	double ebeam=29;//MeV
	double zbeam=5.0;//Z
	double abeam=10.0;//A	
	
	//
	// TARGETS
	//
	
	// We define the targets that are used in this experiment
	// See target.h for explanation of target class
	//target(Z,A,ug/cm^2,degrees,compound=0,B ug/cm^2=0,B Z=0,B A=0,B compound=0);
	
	target targ(74,186,140.0,180.0,0,60.2,6,12);
	
	// In an attempt to save processing speed, when fission fragment energy losses are to be calculated, "fragment_set" is called.
	// This creates a .root file in the current working directory with a big histogram in it.
	// target class will attempt to load this eloss_store.root file if present to save time.
	targ.fragment_set(targ.targ_Z+zbeam,targ.targ_A+abeam);
	
	//
	// DETECTORS
	//
	
	int detconfig=0;
	// det config 0 = 4 MWPCs
	// det config 1 = 2 MWPCs
	// det config 2 = 2 MWPCs off-centre
	// det config 3 = 2 large mwpcs
	
	// 0 = no silicon detectors
	double silicon_array_distance=0; //mm 
	
	// Here we enter the x y mwpc limits
	// x_min,x_max,y_min,y_max (for small type)
	// MWPC A,B,C,D
	int mwpc_limts[16];
	
	//Feb 2014
	mwpc_limts[0]=476;
	mwpc_limts[1]=3386;
	mwpc_limts[2]=523;
	mwpc_limts[3]=3521;
	
	mwpc_limts[4]=480;
	mwpc_limts[5]=3410;
	mwpc_limts[6]=475;
	mwpc_limts[7]=3469;
	
	mwpc_limts[8]=2008;
	mwpc_limts[9]=4020;
	mwpc_limts[10]=2682;
	mwpc_limts[11]=4600;
	
	mwpc_limts[12]=1960;
	mwpc_limts[13]=5030;
	mwpc_limts[14]=1835;
	mwpc_limts[15]=4783;
	
	// Time zero values for MWPC pairs
	// set = 0 for calibration
	double AB=0,BC=0,AD=0;	
	
	/////////////////////////////////////////
	////////     Main run section     ///////
	/////////////////////////////////////////
	
	// Pointers for the main operation
	string si_cal="/home/smallcombe/data/tmp.root";
	string out_dir="/home/smallcombe/data/";	
	// Create chains of the individual run files as needed
	TChain* data_chain = new TChain("186W_B10_71.8");
	data_chain->AddFile("/home/smallcombe/data/2015_feb_B10_Li6_186W/feb_10Be_data/y1y2y3y4/run001.root",0,"run001");
	data_chain->AddFile("/home/smallcombe/data/2015_feb_B10_Li6_186W/feb_10Be_data/y1y2y3y4/run002.root",0,"run002");
	data_chain->AddFile("/home/smallcombe/data/2015_feb_B10_Li6_186W/feb_10Be_data/y1y2y3y4/run007.root",0,"run007");

	// PROCESSING LINES
	cout<<endl<<"///// Start /////"<<endl;	
	
	master_selector *proc = new master_selector(ebeam,zbeam,abeam,&targ,out_dir,detconfig,mwpc_limts,AB,BC,AD,2,silicon_array_distance,si_cal);
	proc->cal_check(data_chain);
	data_chain->Process(proc);

	cout<<endl<<"///// Finished /////"<<endl;

	gSystem->ChangeDirectory(out_dir.c_str());
	
	new TBrowser
}

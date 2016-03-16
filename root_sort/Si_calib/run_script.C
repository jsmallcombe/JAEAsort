{
// 	gSystem->Load("/usr/local/lib/librange");
// 	gROOT->ProcessLine(".L nuclear_data_ob.cxx+"); 
// 	gROOT->ProcessLine(".L james_detector_func.cpp+");
// 	gROOT->ProcessLine(".L james_physics.cpp+"); 
// 	gROOT->ProcessLine(".L james_target.cpp+");
	
	gROOT->ProcessLine(".L Si_calib.cpp+");
	gROOT->ProcessLine(".L Calib.cpp+");
	
	Calib("files.txt");

	new TBrowser;
}
void macro_nikiroot(){
	gROOT->ProcessLine(".include include");
	gROOT->ProcessLine(".L src/nikirootinc.cxx+");
	gROOT->ProcessLine(".L src/buffertemplate.cxx+");
	gROOT->ProcessLine(".L src/core_nikiroot.cxx+"); 

	//gROOT->ProcessLine("core()");
	gROOT->ProcessLine("vector<int> triggers;");
	gROOT->ProcessLine("triggers.push_back(5);");
	gROOT->ProcessLine("triggers.push_back(3);");
	gROOT->ProcessLine("triggers.push_back(4);");
	gROOT->ProcessLine("core(0,100,0,0,\"\",\".\",\".\",1000000,triggers);");

// 	1 Report Level : int 0-4
// 	2 Trigger Length : long
// 	3 Trigger Delay : long
// 	4 Timing Data on/off : int 0-1
// 	5 Channel file : string
// 	6 Load path or list : string
// 	7 Save path : string
// 	8 Buffer size : int
// 	9 DAQ triggers : vector<int>

	//gSystem->ChangeDirectory("");
	//new TBrowser();
}
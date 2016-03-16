{
	gSystem->Load("./library/bin/james_root.so");
	gROOT->ProcessLine(".include ./library/include");
	gSystem->Load("/usr/local/lib/librange");
	gROOT->ProcessLine(".x experiment.C");
}

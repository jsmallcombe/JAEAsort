void scan(string file=""){
	
	if(file.size()<1)cin>>file;
	
	int count=0;
	int Bcount=0;
	int module=0;
	ifstream openfile(file.c_str());
	
	if(!openfile.is_open())return;
	
	std::bitset<40> foo;     // foo: 0000
	foo.set();              // foo: 1111
	long long fulltime=foo.to_ullong();
	
	int a,b,c,m;
	long long d,max=0,min=fulltime;
	TString line;
	
	while(line.ReadLine(openfile)) {
		line.ReplaceAll(","," ");
		istringstream is(line.Data());
		is >> a >> b >> c >> d;
		
		if(a!=module){
			cout<<endl<<module<<" "<<count<<flush;
			module=a;
			count=0;
			if(a==0){
				cout<<endl<<endl<<Bcount<<" entries "<<max-min<<" ticks"<<endl;
				Bcount=0;
				max=0;
				min=fulltime;
			}
		}
		count++;
		Bcount++;
		if(d<min)min=d;
		if(d>max)max=d;
	}
}


	

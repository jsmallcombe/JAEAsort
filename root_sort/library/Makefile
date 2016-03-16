# 
# 
# 	james_library 1.15
#  	27/7/20		20/7/2015
# 	james.smallcombe@outlook.com
# 
# 

JAMES_LIB = $(shell pwd)
JAMES_INCLUDE = $(shell pwd)/include

CC = g++
#CFLAGS = -std=c++11 -fPIC -Wall `root-config --cflags` -I$(JAMES_INCLUDE)
CFLAGS = -fPIC -Wall `root-config --cflags` -I$(JAMES_INCLUDE)

LIBRS = -L$(JAMES_INCLUDE) -lrange `root-config --glibs`

SYSHEAD = $(wildcard include/james*.h)
OBJECTS = $(patsubst include/%.h,bin/%.o,$(SYSHEAD))
HEAD = $(patsubst %.h,$(shell pwd)/%.h,$(SYSHEAD))





TARG = bin/james_root.so
	
$(TARG): $(OBJECTS) bin/DictOutput.cxx
	$(CC) $(CFLAGS) -o $@ -shared bin/DictOutput.cxx $(OBJECTS) -I. $(LIBRS)	
	
bin/DictOutput.cxx: $(SYSHEAD)
	bash 	bin/link.sh
	rootcint -f $@ -c -I$(JAMES_INCLUDE) $(HEAD) bin/Linkdef.h

bin/%.o: src/%.cpp include/%.h
	$(CC) $(CFLAGS) -o $@ -c $< $(LIBRS)

clean:
	rm $(JAMES_LIB)/bin/*.o
	rm $(JAMES_LIB)/bin/Linkdef.h
	rm $(JAMES_LIB)/bin/DictOutput*
	rm $(TARG)


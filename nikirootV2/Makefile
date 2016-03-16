CC = g++ -std=c++0x
CFLAGS = -fPIC -Wall `root-config --cflags` -I./include
INCLUDE = include
SOURCEDIR = src
TARG = nikiwrapped
WRAP = $(SOURCEDIR)/wrapper.cpp
LIBRS =-L$(INCLUDE) `root-config --glibs` 
_OBJ = core_nikiroot.o nikirootinc.o buffertemplate.o
OBJ = $(patsubst %,$(SOURCEDIR)/%,$(_OBJ))

# dont actually need to make libraries just wanted to test that I could
$(TARG): $(OBJ) $(WRAP)
	$(CC) -o $@ $(CFLAGS) $(WRAP) $(OBJ) $(LIBRS) -lstdc++

$(SOURCEDIR)/%.o: $(SOURCEDIR)/%.cxx $(INCLUDE)/%.h
	$(CC) $(CFLAGS) -o $@ -c $< $(LIBRS)

clean:
	rm $(SOURCEDIR)/*.o

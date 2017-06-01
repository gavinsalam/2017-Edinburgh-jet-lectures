
# Makefile generated automatically by /Users/gsalam/scripts/mkcxx.pl '-i' '-I ../../tutorial-1/pythia8226/include' '-l' '-L ../../tutorial-1/pythia8226/lib -lpythia8 -ldl' '-g' 'c++'
# run 'make make' to update it if you add new files

CXX = c++
CXXFLAGS = -Wall -g -O2

# also arrange for fortran support
FC = gfortran
FFLAGS = -Wall -O2
INCLUDE += $(LCLINCLUDE)

COMMONSRC = CmdLine.cc FlavourHolder.cc helpers.cc
F77SRC = 
COMMONOBJ = CmdLine.o FlavourHolder.o helpers.o

PROGSRC = main01.cc
PROGOBJ = main01.o

INCLUDE += -I ../../tutorial-1/pythia8226/include
LIBRARIES += -L ../../tutorial-1/pythia8226/lib -lpythia8 -ldl


all:  main01 


main01: main01.o  $(COMMONOBJ)
	$(CXX) $(LDFLAGS) -o $@ $@.o $(COMMONOBJ) $(LIBRARIES)


make:
	/Users/gsalam/scripts/mkcxx.pl '-i' '-I ../../tutorial-1/pythia8226/include' '-l' '-L ../../tutorial-1/pythia8226/lib -lpythia8 -ldl' '-g' 'c++'

clean:
	rm -vf $(COMMONOBJ) $(PROGOBJ)

realclean: clean
	rm -vf  main01 

.cc.o:         $<
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
.cpp.o:         $<
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
.C.o:         $<
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@
.f.o:         $<
	$(FC) $(FFLAGS) -c $< -o $@
.f90.o:         $<
	$(FC) $(FFLAGS) -c $< -o $@


depend:
	makedepend  $(LCLINCLUDE) -Y --   -- $(COMMONSRC) $(PROGSRC)
# DO NOT DELETE

CmdLine.o: CmdLine.hh
FlavourHolder.o: FlavourHolder.hh
helpers.o: helpers.hh
main01.o: helpers.hh AverageAndError.hh SimpleHist.hh CmdLine.hh
main01.o: FJCorePythia.hh

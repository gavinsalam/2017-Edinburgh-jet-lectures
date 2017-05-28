
# Makefile generated automatically by /Users/gsalam/scripts/mkcxx.pl '-i' '-I ../pythia8226/include' '-l' '-L ../pythia8226/lib -lpythia8' '-g' 'c++'
# run 'make make' to update it if you add new files

CXX = c++
CXXFLAGS = -Wall -g -O2

# also arrange for fortran support
FC = gfortran
FFLAGS = -Wall -O2
INCLUDE += $(LCLINCLUDE)

COMMONSRC = 
F77SRC = 
COMMONOBJ = 

PROGSRC = main01.cc
PROGOBJ = main01.o

INCLUDE += -I ../pythia8226/include
LIBRARIES += -L ../pythia8226/lib -lpythia8


all:  main01 


main01: main01.o  $(COMMONOBJ)
	$(CXX) $(LDFLAGS) -o $@ $@.o $(COMMONOBJ) $(LIBRARIES)


make:
	/Users/gsalam/scripts/mkcxx.pl '-i' '-I ../pythia8226/include' '-l' '-L ../pythia8226/lib -lpythia8' '-g' 'c++'

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

main01.o: SimpleHist.hh

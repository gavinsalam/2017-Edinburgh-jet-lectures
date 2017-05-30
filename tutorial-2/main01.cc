// main01.cc is a part of the PYTHIA event generator.
// Copyright (C) 2017 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is adapted from Pythia 8's main01.cc
// 

#include "Pythia8/Pythia.h"
#include <cmath>
#include <stream>

using namespace Pythia8;
using namespace std;


int main() {
  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;
  pythia.readString("Beams:eCM = 91.2");  
  pythia.readString("Beams:idA =  11"); // electron
  pythia.readString("Beams:idB = -11"); // positron

  // produce Z bosons and interference with photon
  pythia.readString("WeakSingleBoson:ffbar2gmZ = on");

  // prevent ISR radiation of photons
  pythia.readString("SpaceShower:QEDshowerByL = off");
  pythia.readString("SpaceShower:MEcorrections = off");
  pythia.readString("PartonLevel:ISR = off");
  
  // consider only Z (PDGID=23) decays to light quarks 
  pythia.readString("23:onMode = off");
  pythia.readString("23:onIfAny = 1 2 3 4 5");

  int nEvents = 1;
  
  pythia.init();
  
  ofstream file_particles("main01.particles");
  ofstream file_jets     ("main01.jets");

  // Begin event loop. Generate event. Skip if error. List first one.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    
    if (!pythia.next()) continue;
    
    // Add an entry to the rapidity histogram for each particle
    for (int i = 0; i < pythia.event.size(); ++i) {

      // consider only final-state particles
      if (!pythia.event[i].isFinal()) continue;

      // print out each momentum
      file << "0 0 0" << endl;
      file << pythia.event[i].px() << " "
           << pythia.event[i].px() << " "
           << pythia.event[i].pz() << endl << endl << endl;
    }
  // End of event loop. Statistics. Histogram. Done.
  }
  pythia.stat();

  return 0;
}

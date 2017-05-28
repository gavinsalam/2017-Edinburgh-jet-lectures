// main01.cc is a part of the PYTHIA event generator.
// Copyright (C) 2017 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is adapted from Pythia 8's main01.cc
// 

#include "Pythia8/Pythia.h"
#include "SimpleHist.hh"

using namespace Pythia8;
using namespace std;

int main() {
  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;
  pythia.readString("Beams:eCM = 13000.");

  // produce Z bosons
  pythia.readString("WeakSingleBoson:ffbar2gmZ = on");
  pythia.readString("WeakZ0:gmZmode = 2");
  
  // consider only Z (PDGID=23) decays to muons (PDG ID 13)
  pythia.readString("23:onMode = off");
  pythia.readString("23:onIfAny = 13");

  int nEvents = 100;
  
  pythia.init();
  SimpleHist particleRap(-20.0, 20.0, 0.5);
  
  // Begin event loop. Generate event. Skip if error. List first one.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    
    if (!pythia.next()) continue;
    
    // Add an entry to the rapidity histogram for each particle
    for (int i = 0; i < pythia.event.size(); ++i) {
      if (!pythia.event[i].isFinal()) continue;
      particleRap.add_entry(pythia.event[i].p().rap());
    }
  // End of event loop. Statistics. Histogram. Done.
  }
  pythia.stat();

  // normalise the histogram to the number of events
  particleRap /= nEvents;
  ofstream file("main01.out");
  file << "# Rapidity: cols1,2,3 = bin lo,mid,hi; col4 = nparticles/event" << endl;
  file << particleRap;
  return 0;
}

// main01.cc is a part of the PYTHIA event generator.
// Copyright (C) 2017 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// This is adapted from Pythia 8's main01.cc
// 

#include "Pythia8/Pythia.h"
#include <cmath>
#include "Pythia8/FJcore.h"  // subset of FastJet clustering
#include "helpers.hh"

using namespace Pythia8;
using namespace std;
using namespace fjcore;


int main() {
  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;
  pythia.readString("Beams:eCM = 91.2");  
  pythia.readString("Beams:idA =  11"); // electron
  pythia.readString("Beams:idB = -11"); // positron

  // produce Z bosons and interference with photon
  pythia.readString("WeakSingleBoson:ffbar2gmZ = on");

  // consider only Z (PDGID=23) decays to light quarks 
  pythia.readString("23:onMode = off");
  pythia.readString("23:onIfAny = 1 2 3 4 5");

  // by changing the seed you can get different events
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed    = 20");

  // we want to display the event, so generate only one
  int nEvents = 1;
  
  pythia.init();
  
  ofstream file_particles("main01.particles");
  ofstream file_jets     ("main01.jets");

  JetDefinition jet_def(ee_kt_algorithm);
  
  // Begin event loop. Generate event. Skip if error. List first one.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    
    if (!pythia.next()) continue;

    vector<PseudoJet> particles;
    
    // Add an entry to the rapidity histogram for each particle
    for (int i = 0; i < pythia.event.size(); ++i) {

      // consider only final-state particles
      if (!pythia.event[i].isFinal()) continue;

      particles.push_back(PseudoJet(pythia.event[i]));
      
      // print out each momentum (starting from 0 to enable
      // gnuplot to easily draw a line)
      file_particles << "0 0 0 0" << endl;
      file_particles << particles.back() << endl << endl << endl;
    }

    ClusterSequence cs(particles, jet_def);
    vector<PseudoJet> jets = cs.exclusive_jets_ycut(0.02);
    for (unsigned i = 0; i < jets.size(); i++) {
      file_jets << "0 0 0 0" << endl << jets[i] << endl << endl << endl;
    }
      
  // End of event loop. Statistics. Histogram. Done.
  }
  pythia.stat();

  return 0;
}

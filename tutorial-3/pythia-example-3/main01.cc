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
#include "AverageAndError.hh"
#include "SimpleHist.hh"
#include "CmdLine.hh"

using namespace Pythia8;
using namespace std;
using namespace fjcore;


int main(int argc, char ** argv) {
  // A simple command-line processor
  CmdLine cmdline(argc,argv);

  // set a few variables based on the command line
  double Q    = cmdline.value("-Q", 100.0);
  double ycut = cmdline.value("-ycut", 0.03);
  int nEvents = int(cmdline.value("-nev", 1000.0));
  cmdline.assert_all_options_used();
  
  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;
  pythia.settings.parm("Beams:eCM", Q);  
  //pythia.readString("Beams:eCM = 91000.2");  
  pythia.readString("Beams:idA =  11"); // electron
  pythia.readString("Beams:idB = -11"); // positron

  // produce Z bosons and interference with photon
  pythia.readString("WeakSingleBoson:ffbar2gmZ = on");

  // consider only Z (PDGID=23) decays to light quarks 
  pythia.readString("23:onMode = off");
  pythia.readString("23:onIfAny = 1 2 3 4 5");

  // prevents electrons from radiating photons and losing energy prior
  // to producing the Z (issue of "radiative return" at high
  // energies).
  pythia.readString("PDF:lepton = off");
  
  // by changing the seed you can get different events
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed    = 20");

  
  pythia.init();

  // for getting the average across events
  AverageAndError multiplicity;
  // for storing the different jet rates
  SimpleHist jet_rates(1.5, 5.5, 1.0);
  
  
  // Begin event loop. Generate event. Skip if error. List first one.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    
    if (!pythia.next()) continue;

    vector<PseudoJet> particles;
    
    // Add an entry to the rapidity histogram for each particle
    for (int i = 0; i < pythia.event.size(); ++i) {

      // consider only final-state particles
      if (!pythia.event[i].isFinal()) continue;

      particles.push_back(PseudoJet(pythia.event[i]));
    }

    // this keeps track of the multiplicity (of the special AverageAndError type)
    multiplicity += particles.size();
    
    // Cluster particle into jets
    // First generate a whole "clustering sequence" with the e+e- kt algorithm
    JetDefinition jet_def(ee_kt_algorithm);
    ClusterSequence cs(particles, jet_def);
    
    // then select the jets that come out of clustering with the following
    // value of ycut
    vector<PseudoJet> jets = cs.exclusive_jets_ycut(ycut);

    // record the number of jets
    jet_rates.add_entry(jets.size());
      
  // End of event loop. Statistics. Histogram. Done.
  }
  pythia.stat();


  // now write the output
  ostringstream filename_stream;
  filename_stream << "main01_ycut" <<ycut << "_Q" << Q << ".out";
  cout << "Sending output to " << filename_stream.str() << endl;
  ofstream file(filename_stream.str());
  file << "# " << cmdline.command_line() << endl;
  file << "# Q = " << Q << endl;
  file << "# <multiplicity> = " << multiplicity.average() << " +- " << multiplicity.error() << endl;
  file << "# <multiplicity^2> - <multiplicity>^2 = " << multiplicity.sd() << endl;
  file << endl;

  file << "# Histogram of fraction of events with n jets (col4) v. n (col2)" << endl;
  jet_rates /= nEvents;
  file << jet_rates << endl;
  
  
  return 0;
}

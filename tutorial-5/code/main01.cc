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
#include "SimpleHist.hh"
#include "CmdLine.hh"
// slightly extended version of Pythia8Plugins/FastJet3.h, adapted to fjcore
// rather than FastJet
#include "FJCorePythia.hh" 

using namespace Pythia8;
using namespace std;
using namespace fjcore;


int main(int argc, char ** argv) {
  // A simple command-line processor
  CmdLine cmdline(argc,argv);

  // set a few variables based on the command line
  int nEvents = int(cmdline.value("-nev", 1000.0));
  // the parameters for the jet finding
  double R     = cmdline.value("-R", 1.0);
  string MPI   = cmdline.value<string>("-MPI", "on");
  string ISR   = cmdline.value<string>("-ISR", "on");
  double ptmin = cmdline.value("-ptmin", 500.0);
  double mmin  = cmdline.value("-mmin", 1000.0);

  cmdline.assert_all_options_used();
  
  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;
  pythia.settings.parm("Beams:eCM", 13000.0);  
  pythia.readString("Beams:idA =  2212"); // proton
  pythia.readString("Beams:idB =  2212"); // proton

  // generate WW events
  pythia.readString("WeakDoubleBoson:ffbar2WW = on   ");
  // alternatively, try dijet events
  //pythia.readString("HardQCD:all = on   ");

  // Tell Pythia to generate only a portion of phasespace
  pythia.settings.parm("PhaseSpace:pTHatMin", ptmin);
  pythia.settings.parm("PhaseSpace:mHatMin", mmin);
  
  // let W's decay only to light quarks 
  pythia.readString("24:onMode = off");
  pythia.readString("24:onIfAny = 1 2 3 4");

  pythia.readString("PartonLevel:ISR = "+ISR);
  pythia.readString("PartonLevel:MPI = "+MPI);
  
  // by changing the seed you can get different events
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed    = 20");

  pythia.init();
  
  // sets up the default jet-finding parameters
  JetDefinition jet_def(antikt_algorithm, R);
  // set up the algorithm for declustering (with a large radius, R=1.5), to
  // capture everything
  JetDefinition jet_def_CA(cambridge_algorithm, 1.5);
  
  // take the two highest-pt jets, regardless of where they are in pt and rapidity
  Selector jet_selector = SelectorNHardest(2);

  // histograms for later
  SimpleHist jet_mass(0.0, 150.0, 2.0);
  SimpleHist mmdt_jet_mass(0.0, 150.0, 2.0);
  
  // Begin event loop. Generate event. Skip if error. List first one.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    if (iEvent%100 == 0) cout << iEvent << endl;
    
    if (!pythia.next()) continue;

    vector<PseudoJet> particles;
    
    // collect all final state particles
    for (int i = 0; i < pythia.event.size(); ++i) {
      // consider only final-state particles
      if (!pythia.event[i].isFinal()) continue;
      particles.push_back(PseudoJet(pythia.event[i]));
    }

    // Cluster particle into jets; 
    vector<PseudoJet> jets = jet_def(particles);

    // then loop over the two hardest jets and bin their mass
    for (unsigned i = 0; i < 2; i++) {
      jet_mass.add_entry(jets[i].m());

      // // try the "mMDT(mu=1) - SoftDrop(beta=0) procedure"
      // double zcut = 0.1;
      // // First recluster the jet's constituents with C/A and take the
      // // highest-pt jet ([0])
      // PseudoJet CA_jet = jet_def_CA(jets[i].constituents())[0];
      // 
      // // recursively undo the clustering until we find hard substructure
      // PseudoJet parent1, parent2;
      // PseudoJet mmdt_jet = CA_jet;
      // while (mmdt_jet.has_parents(parent1, parent2)) {
      //   // parent1 and parent2 are the two pieces that come from undoing the
      //   // last stage of the clustering of mmdt_jet;
      //   // Make sure that parent1 is the hardest
      //   if (parent1.pt() < parent2.pt()) swap(parent1, parent2);
      //   
      //   // if parent2 is too soft, discard it, redefine mmdt_jet to be
      //   // parent1 and continue declustering
      //   if (parent2.pt() < zcut * mmdt_jet.pt()) {
      //     mmdt_jet = parent1;
      //     continue;
      //   }
      //   
      //   // Otherwise, if we get here, then last stage of declustering
      //   // of mmdt_jet showed hard substructure, so this is right stage
      //   // to exit the declustering
      //   break;
      // }
      // mmdt_jet_mass.add_entry(mmdt_jet.m());
    }
    
  }
  pythia.stat();


  // now write the output
  ostringstream filename_stream;
  filename_stream << "main01.out";
  cout << "Sending output to " << filename_stream.str() << endl;
  ofstream file(filename_stream.str());
  file << "# " << cmdline.command_line() << endl;
  file << "# jet_definition = " << jet_def.description() << endl;
  
  file << "# jet mass" << endl;
  file << jet_mass << endl << endl;

  file << "# mMDT jet mass" << endl;
  file << mmdt_jet_mass << endl << endl;

  return 0;
}

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
  double R     = cmdline.value("-R", 0.4);
  double ptmin = cmdline.value("-ptmin", 20.0);
  double ymax  = cmdline.value("-ymax", 2.5);

  cmdline.assert_all_options_used();
  
  // Generator. Process selection. LHC initialization. Histogram.
  Pythia pythia;
  pythia.settings.parm("Beams:eCM", 13000.0);  
  //pythia.readString("Beams:eCM = 91000.2");  
  pythia.readString("Beams:idA =  2212"); // proton
  pythia.readString("Beams:idB =  2212"); // proton

  // generate ttbar events
  pythia.readString("Top:gg2ttbar = on   ");
  pythia.readString("Top:qqbar2ttbar = on");

  // top quarks decay to b W and we choose a "semileptonic" W decay pattern
  pythia.readString("24:onMode = off");
  // W+ -> light quarks (no b's, even though occasionally this is possible)
  pythia.readString("24:onPosIfAny = 1 2 3 4");
  // W- -> mu- nu_mu
  pythia.readString("24:onNegIfAny = 13");

  // this routines (in helpers.hh and .cc) makes all b-hadrons stable,
  // which will help us with b-tagging
  set_bflavour_stable(pythia);
  
  pythia.readString("PartonLevel:FSR = off      ");
  pythia.readString("PartonLevel:ISR = off      ");
  pythia.readString("HadronLevel:Hadronize = off");
  pythia.readString("PartonLevel:MPI = off      ");
    
  
  // by changing the seed you can get different events
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed    = 20");

  pythia.init();
  
  // sets up the use of the jet-finding parameters
  JetDefinition jet_def(antikt_algorithm, R);
  Selector jet_selector = SelectorPtMin(ptmin) && SelectorAbsRapMax(ymax);

  // these will help us separate out different kinds of particles;
  // We will ignore all neutrinos
  Selector neutrino_selector = SelectorIsLepton() && SelectorIsNeutral();

  // and pull out the hardest muon within acceptance (assume the same
  // acceptance as for the jets)
  Selector hardest_muon = SelectorNHardest(1) * SelectorId(13) * jet_selector;

  // histograms for later
  SimpleHist jet_multiplicity  (-0.5, 12.5, 1.0);
  SimpleHist bjet_multiplicity (-0.5, 12.5, 1.0);
  SimpleHist W_candidate_mass  (0.0, 150.0, 2.0);
  SimpleHist top_candidate_mass(0.0, 300.0, 4.0);
  
  // Begin event loop. Generate event. Skip if error. List first one.
  for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
    if (iEvent%100 == 0) cout << iEvent << endl;
    
    if (!pythia.next()) continue;

    vector<PseudoJet> particles;
    
    // Add an entry to the rapidity histogram for each particle
    for (int i = 0; i < pythia.event.size(); ++i) {
      // consider only final-state particles
      if (!pythia.event[i].isFinal()) continue;
      particles.push_back(PseudoJet(pythia.event[i]));
    }

    // having engineered Pythia top decays to be semi-leptonic, we
    // will now attempt to selector out the neutrinos and the hardest
    // muon
    vector<PseudoJet> hadrons, muons, neutrinos, not_neutrinos;
    PseudoJet muon;

    // FastJet selectors' "sift" function separates out particles
    // into the ones that do and don't satisfy the condition
    neutrino_selector.sift(particles, neutrinos, not_neutrinos);
    hardest_muon.sift(not_neutrinos, muons, hadrons);

    // if we don't have a muon, then skip this event
    if (muons.size() < 1) continue;
    muon = muons[0];

    // Cluster particle into jets; for hadron collider algorithms it's easiest
    // to use the jet def operator(), which automatically applies the "inclusive"
    // algorithm and returns jets sorted by pt (highest-pt first)
    //for (auto h: hadrons) cout << h << "; "; cout << endl; // NB C++11
    vector<PseudoJet> jets = jet_def(hadrons);
    
    // then apply the selector to narrow down the jets we consider
    // (i.e. pt and rapidity cuts, see above); the pt ordering
    // is maintained by all selector operations
    jets = jet_selector(jets);

    // record the number of jets that are left
    jet_multiplicity.add_entry(jets.size());

    // identify b-jets as being any jet that contains a b-hadron
    vector<PseudoJet> bjets, non_bjets;
    for (unsigned i = 0; i < jets.size(); i++) {
      // we will look through the constituents of each jet
      vector<PseudoJet> constituents = jets[i].constituents();
      // and count the number that have non-zero b-flavour
      int nb = 0;
      for (unsigned j = 0; j < constituents.size(); j++) {
        if (constituents[j].user_info<Py8Particle>().flavour()[5] != 0) nb++;
      }
      if (nb != 0) {
        bjets.push_back(jets[i]);
      } else {
        non_bjets.push_back(jets[i]);
      }
    }
    bjet_multiplicity.add_entry(bjets.size());


    // We expect a b from the top and two non-bjets from the W.
    // So skip the rest of this event if we can't find them
    if (bjets.size() < 1 || non_bjets.size() < 2) continue;

    // now try to reconstruct a W candidate
    PseudoJet W_candidate = non_bjets[0] + non_bjets[1];
    W_candidate_mass.add_entry(W_candidate.m());
    // then a top candidate, by combining the W with each of the b jets
    // in sequence
    for (unsigned ib = 0; ib < bjets.size(); ib++) {
      PseudoJet top_candidate = W_candidate + bjets[ib];
      top_candidate_mass.add_entry(top_candidate.m());
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
  file << "# jet_selector   = " << jet_selector.description() << endl;
  
  file << "# jet multiplicity (col2 = njets, col4 = nevents)" << endl;
  file << jet_multiplicity << endl << endl;

  file << "# bjet multiplicity (col2 = njets, col4 = nevents)" << endl;
  file << bjet_multiplicity << endl << endl;

  file << "# W candidate mass" << endl;
  file << W_candidate_mass << endl << endl;

  file << "# top candidate mass" << endl;
  file << top_candidate_mass << endl << endl;

  return 0;
}

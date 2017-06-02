#ifndef __HELPERS_HH__
#define __HELPERS_HH__

//#include <stream>

#include "Pythia8/Event.h"              // this is what we need from Pythia8
#include "Pythia8/FJcore.h"
#include "Pythia8/Pythia.h"
#include "FJCorePythia.hh"
#include <sstream>

namespace Pythia8 {
namespace fjcore {

inline ostream & operator<<(ostream & ostr, PseudoJet & p) {
  ostr << p.pt() << " "
       << p.rap() << " "
       << p.phi() << " "
       << p.m()  << " ";
  if (p.has_user_info<Py8Particle>()) ostr << p.user_info<Py8Particle>().id() << " ";
  return ostr;
}
  
}
}


/// make particle flavour id stable
inline void set_stable(Pythia8::Pythia & pythia, unsigned int id) {
  std::ostringstream oss;
  oss << id  << ":mayDecay = off"; //  better than onMode (harder to check later)
  pythia.readString(oss.str());
}

/// make all long-lived B-hadrons stable
void set_bflavour_stable(Pythia8::Pythia & pythia);
#endif //  __HELPERS_HH__

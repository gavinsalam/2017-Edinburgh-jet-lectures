#ifndef __HELPERS_HH__
#define __HELPERS_HH__

//#include <stream>

#include "Pythia8/Event.h"              // this is what we need from Pythia8
#include "Pythia8/FJcore.h"

namespace Pythia8 {
namespace fjcore {

/// specialization of the PseudoJet constructor so that it can take a
/// pythia8 particle (and makes a copy of it as user info);
template<>
inline PseudoJet::PseudoJet(const Pythia8::Particle & particle) {
  reset(particle.px(),particle.py(),particle.pz(), particle.e());
}

/// specialization of the PseudoJet constructor so that it can take a
/// pythia8 Vec4. There is then no particular user info available.
template<>
inline PseudoJet::PseudoJet(const Pythia8::Vec4 & particle) {
  reset(particle.px(),particle.py(),particle.pz(), particle.e());
}

ostream & operator<<(ostream & ostr, PseudoJet & p) {
  ostr << p.px() << " "
       << p.py() << " "
       << p.pz() << " "
       << p.E()  << " ";
  return ostr;
}
  
}
}

#endif //  __HELPERS_HH__

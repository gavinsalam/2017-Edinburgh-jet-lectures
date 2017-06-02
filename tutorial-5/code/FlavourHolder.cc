//STARTHEADER
// $Id: FlavourHolder.cc 3883 2013-06-14 20:01:07Z salam $
//
// Copyright (c) 2006-2013 Gavin Salam
//
//----------------------------------------------------------------------
// This file provides a simple class for extracting information from
// standard-model PDG codes.
//
//  FlavourHolder is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  FlavourHolder is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with FastJet; if not, write to the Free Software
//  Foundation, Inc.:
//      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//----------------------------------------------------------------------
//ENDHEADER


#include<valarray>
#include<iostream>
#include "FlavourHolder.hh"

using namespace std;

//----------------------------------------------------------------------
/// set the FlavourHolder such that FlavourHolder[iflv] corresponds to
/// the net number of quarks of that flavour present in the particle
/// labelled by idhep.
///
/// Particle numbering is given at 
///   http://www-cpd.fnal.gov/psm/stdhep/
///
/// This routine is closely based on the f90 routine convert_hadron_codes
/// written for the jet-flavour work with Andrea Banfi and Giulia
/// Zanderighi.
///
FlavourHolder::FlavourHolder(int idhep): _flav_content(7,0), _idhep(idhep) {

  // [NB: the following are now done in the member initialiser list]
  // make _flav_content of size 7 to allow easy access to entries 1..6 
  //_flav_content.resize(7);
  //_idhep = idhep;

  // for particles with illicit (zero) idhep, no work to be done
  if (_idhep == 0) return;

  int netsign = (idhep >= 0 ? +1 : -1);
  idhep = abs(idhep);

  // extract digits of the idhep, since these contain information 
  // on flavour of component quarks
  valarray<int> digit(4);
  int           ndigits = 0;
  for (int i = 0; i < 4; i++) {
    digit[i] = idhep % 10;
    if (digit[i] != 0) ndigits = i+1;
    idhep /= 10; // "shift" things along
  }
  
  // // sanity check...
  // if (idhep != 0) {
  //   cerr <<"idhep ("<<_idhep
  //        <<") contained more digits than are understood."<<endl;
  //   exit(-1);}
  
  // start this part with _flav_content already initialised to zero
  // in constructor
  if (ndigits == 1) { // a lone quark
    if (digit[0] > 6 || digit[0] == 0) {
      cerr << "FlavourHolder failed to understand idhep = "<<_idhep<<endl; exit(-1);}
    _flav_content[digit[0]] = netsign;

  } else if (ndigits == 2) { // a lepton, photon or cluster [flav lost...]
    // do nothing...

  } else { // must be a meson, cluster or baryon
    // check sanity of codes
    for (int i=1; i < ndigits; i++) {
      if (digit[i] > 6) {cerr << "FlavourHolder failed to understand idhep = "
			       <<_idhep<<endl; exit(-1);}}
    
    // now deal with different cases
    if (ndigits == 4) { // diquark [nm0x] or baryon [nmpx]
      for (int i=1; i < ndigits; i++) {
	if (digit[i] > 0) _flav_content[digit[i]] += netsign;}
    } else if (ndigits == 3) { // meson [nmx]
      // Beware of PDG convention that says that a K+ or B+ are a
      // particle and so have positive idhep (i.e. flavcodes > 1). So
      if (digit[2] == 3 || digit[2] == 5) netsign = -netsign;
      _flav_content[digit[2]] += netsign;
      _flav_content[digit[1]] -= netsign;
    } else {
      cerr << "FlavourHolder failed to understand idhep = " <<_idhep<<endl; exit(-1);}
  }

}

// return the particle chage
double FlavourHolder::charge() const {
  // with leptons it's easy
  if (is_charged_lepton()) {
    return idhep() > 0 ? -1.0 : +1.0;
  }
  // with hadrons, work hadrer
  const double dchg = -1.0;
  const double uchg =  2.0;
  double chg = 0.0;
  for (int iflv = 1; iflv <=6; iflv++) {
    chg += _flav_content[iflv] * (iflv%2==0? uchg : dchg);
  }
  return chg/3.0;
}

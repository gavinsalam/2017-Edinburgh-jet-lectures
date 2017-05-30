#ifndef __SIMPLEHIST_HH__
#define __SIMPLEHIST_HH__

//STARTHEADER
// $Id: SimpleHist.hh 3392 2012-08-01 10:42:51Z salam $
//
// Copyright (c) 2007-2011, Matteo Cacciari, Gavin Salam and Gregory Soyez
//
//----------------------------------------------------------------------
//
//  SimpleHist is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  SimpleHist is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with FastJet; if not, write to the Free Software
//  Foundation, Inc.:
//      59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//----------------------------------------------------------------------
//ENDHEADER


#include<valarray>
#include<string>
#include<cmath>
#include<iostream>
#include<cassert>
#include<vector>

class SimpleHist {
public:
  SimpleHist() {};
  SimpleHist(double minv, double maxv, int n): 
    _minv(minv), _maxv(maxv), _dv((maxv-minv)/n), _weights(n+1) {
    _weights = 0.0;
    _weight_v = 0.0;
    _weight_vsq = 0.0;
    _have_total = false;
    _n_entries = 0.0;
  }

  SimpleHist(double minv, double maxv, unsigned int n) {
    declare(minv, maxv, n);
  }

  SimpleHist(double minv, double maxv, double bin_size) {
    declare(minv, maxv, int(0.5+(maxv-minv)/bin_size));
  }

  // declare (or redeclare) the histogram
  void declare(double minv, double maxv, double bin_size) {
    declare(minv, maxv, int(0.5+(maxv-minv)/bin_size));
  }

  void declare(double minv, double maxv, int n) {
    declare(minv, maxv, unsigned(n));
  }

  // declare (or redeclare) the histogram
  void declare(double minv, double maxv, unsigned int n) {
    _minv = minv; _maxv = maxv; _dv = (maxv-minv)/n; 
    _weights.resize(n+1);
    reset();
  }

  /// reset the contents of the histogram to zero (does not
  /// modify the histogram bounds)
  void reset() {
    _weights = 0.0;
    _weight_v = 0.0;
    _weight_vsq = 0.0;
    _total_weight = 0.0;
    _have_total = false;
    _n_entries = 0.0;
  }

  double min() const {return _minv;};
  double max() const {return _maxv;};
  /// returns the size of the histogram proper
  unsigned int size() const {
    unsigned outflow_sz = _weights.size();
    assert(outflow_sz > 0); // help capture uninitialised histogram bugs
    return outflow_sz-1;
  }
  /// returns the size of the histogram plus outflow bin
  unsigned int outflow_size() const {return _weights.size();};

  double & operator[](int i) {_have_total = false; return _weights[i];};
  const double & operator[](int i) const {return _weights[i];};
  
  /// returns the outflow bin
  double & outflow() {return _weights[size()];};
  const double & outflow() const {return _weights[size()];};

  double binlo (int i) const {return i*_dv + _minv;};
  double binhi (int i) const {return (i+1)*_dv + _minv;};
  double binmid(int i) const {return (i+0.5)*_dv + _minv;};
  double binsize()     const {return _dv;};

  unsigned int bin(double v) const {
    if (v >= _minv && v < _maxv) {
      int i = int((v-_minv)/_dv); 
      if (i >= 0) {
        // do unsigned comparisons to avoid issues with huge
        // histograms
        unsigned unsigned_i = unsigned(i);
        if (unsigned_i < size()) {return unsigned_i;} 
      }
    }
    // otherwise...
    return size();
  }

  /// return the mean value of all events given to histogram
  /// including those that were outside the histogram edges
  double mean() const {return _weight_v / total_weight();}

  /// return the total weight in the histogram (inefficient)...
  double total_weight() const {
    if (!_have_total) {
      _total_weight = 0.0;
      for (unsigned i = 0; i < _weights.size(); i++) {
        _total_weight += _weights[i];}
      _have_total = true;
    }
    return _total_weight;
  }

  double n_entries() const {return _n_entries;}

  void add_entry(double v, double weight = 1.0) {
    //if (v >= _minv && v < _maxv) {
    //  int i = int((v-_minv)/_dv); 
    //  if (i >= 0 && i < int(_weights.size())) _weights[i] += weight;
    //}
    _have_total = false;
    _weights[bin(v)] += weight;
    _weight_v += weight * v;
    _weight_vsq += weight * v * v;
    _n_entries += 1.0;
  };

  // Operations with constants ---------------------------------------
  SimpleHist & operator*=(double fact) {
    for (unsigned i = 0; i < outflow_size(); i++) (*this)[i] *= fact;
    _weight_v *= fact;
    _weight_vsq *= fact;
    _total_weight *= fact;
    return *this;
  };
  SimpleHist & operator/=(double fact) {
    *this *= 1.0/fact;
    return *this;
  };

  // Operations with another histogram -------------------------------
  SimpleHist & operator*=(const SimpleHist & other) {
    assert(other.outflow_size() == outflow_size());
    for (unsigned i = 0; i < outflow_size(); i++) (*this)[i] *= other[i];
    return *this;
  };

  SimpleHist & operator/=(const SimpleHist & other) {
    assert(other.outflow_size() == outflow_size());
    for (unsigned i = 0; i < outflow_size(); i++) (*this)[i] /= other[i];
    return *this;
  };

  SimpleHist & operator+=(const SimpleHist & other) {
    assert(other.outflow_size() == outflow_size());
    for (unsigned i = 0; i < outflow_size(); i++) (*this)[i] += other[i];
    _weight_v += other._weight_v;
    _weight_vsq += other._weight_vsq;
    if (_have_total && other._have_total) {
      _total_weight += other._total_weight;
    } else {_have_total = false;}
    return *this;
  };

  SimpleHist & operator-=(const SimpleHist & other) {
    assert(other.outflow_size() == outflow_size());
    for (unsigned i = 0; i < outflow_size(); i++) (*this)[i] -= other[i];
    return *this;
  };


  friend SimpleHist operator*(const SimpleHist & hist, double fact);
  friend SimpleHist operator/(const SimpleHist & hist, double fact);

private:
  double _minv, _maxv, _dv;
  std::valarray<double> _weights;
  std::string _name;
  double _weight_v, _weight_vsq;
  mutable double _total_weight;
  mutable bool   _have_total;
  double _n_entries;
};



// Binary operations with constants -----------------------------
inline SimpleHist operator*(const SimpleHist & hist, double fact) {
  SimpleHist result(hist.min(), hist.max(), hist.outflow_size());
  for (unsigned i = 0; i < hist.outflow_size(); i++) result[i] = hist[i] * fact;
  result._weight_v = hist._weight_v * fact;
  result._weight_vsq = hist._weight_vsq * fact;
  return result;
}

inline SimpleHist operator/(const SimpleHist & hist, double fact) {
  SimpleHist result(hist.min(), hist.max(), hist.outflow_size());
  for (unsigned i = 0; i < hist.outflow_size(); i++) result[i] = hist[i] / fact;
  result._weight_v = hist._weight_v / fact;
  result._weight_vsq = hist._weight_vsq / fact;
  return result;
}

inline SimpleHist operator*(double fact, const SimpleHist & hist) {
  return hist*fact;
}

inline SimpleHist operator/(double fact, const SimpleHist & hist) {
  return hist/fact;
}


// Binary operations with other histograms ------------------------
inline SimpleHist operator*(const SimpleHist & hista, const SimpleHist & histb) {
  assert(hista.outflow_size() == histb.outflow_size());
  SimpleHist result(hista.min(), hista.max(), hista.size());
  for (unsigned i = 0; i < hista.outflow_size(); i++) result[i] = hista[i] * histb[i];
  return result;
}
inline SimpleHist operator/(const SimpleHist & hista, const SimpleHist & histb) {
  assert(hista.outflow_size() == histb.outflow_size());
  SimpleHist result(hista.min(), hista.max(), hista.size());
  for (unsigned i = 0; i < hista.outflow_size(); i++) result[i] = hista[i] / histb[i];
  return result;
}
inline SimpleHist operator+(const SimpleHist & hista, const SimpleHist & histb) {
  assert(hista.outflow_size() == histb.outflow_size());
  SimpleHist result(hista.min(), hista.max(), hista.size());
  for (unsigned i = 0; i < hista.outflow_size(); i++) result[i] = hista[i] + histb[i];
  return result;
}
inline SimpleHist operator-(const SimpleHist & hista, const SimpleHist & histb) {
  assert(hista.outflow_size() == histb.outflow_size());
  SimpleHist result(hista.min(), hista.max(), hista.size());
  for (unsigned i = 0; i < hista.outflow_size(); i++) result[i] = hista[i] - histb[i];
  return result;
}


// Unary mathematical functions
inline SimpleHist sqrt(const SimpleHist & hist) {
  SimpleHist result(hist.min(), hist.max(), hist.size());
  for (unsigned i = 0; i < hist.outflow_size(); i++) result[i] = sqrt(hist[i]);
  return result;
}

// Unary mathematical functions
inline SimpleHist pow2(const SimpleHist & hist) {
  SimpleHist result(hist.min(), hist.max(), hist.size());
  for (unsigned i = 0; i < hist.outflow_size(); i++) result[i] = hist[i]*hist[i];
  return result;
}

/// output the histogram to standard output -- an operator<< might
/// have seemed nice, but less easy to generalize to multiple
/// histograms; the output is multipled by the factor norm.
inline void output(const SimpleHist & hist0, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  for (unsigned i = 0; i < hist0.size(); i++) {
    *ostr << hist0.binlo(i)  << " " 
          << hist0.binmid(i) << " "
          << hist0.binhi(i) << " "
          << hist0[i]*norm << std::endl;
  }
}


std::ostream & operator<<(std::ostream & ostr, const SimpleHist & hist) {
  output(hist, &ostr);
  return ostr;
}

inline void output(const SimpleHist & hist0, 
                   const SimpleHist & hist1, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  assert(hist0.size() == hist1.size() && 
         hist0.min()  == hist1.min() &&
         hist0.max()  == hist1.max());
  for (unsigned i = 0; i < hist0.size(); i++) {
    *ostr << hist0.binlo(i)  << " " 
          << hist0.binmid(i) << " "
          << hist0.binhi(i) << " "
          << hist0[i] * norm << " "
          << hist1[i] * norm << " "
          << std::endl;
  }
}

inline void output(const SimpleHist & hist0, 
                   const SimpleHist & hist1, 
                   const SimpleHist & hist2, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  assert(hist0.size() == hist1.size() && 
         hist0.min()  == hist1.min() &&
         hist0.max()  == hist1.max());
  assert(hist0.size() == hist2.size() && 
         hist0.min()  == hist2.min() &&
         hist0.max()  == hist2.max());
  for (unsigned i = 0; i < hist0.size(); i++) {
    *ostr << hist0.binlo(i)  << " " 
          << hist0.binmid(i) << " "
          << hist0.binhi(i) << " "
          << hist0[i] * norm << " "
          << hist1[i] * norm << " "
          << hist2[i] * norm << " "
          << std::endl;
  }
}


inline void output(const SimpleHist & hist0, 
                   const SimpleHist & hist1, 
                   const SimpleHist & hist2, 
                   const SimpleHist & hist3, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  assert(hist0.size() == hist1.size() && 
         hist0.min()  == hist1.min() &&
         hist0.max()  == hist1.max());
  assert(hist0.size() == hist2.size() && 
         hist0.min()  == hist2.min() &&
         hist0.max()  == hist2.max());
  assert(hist0.size() == hist3.size() && 
         hist0.min()  == hist3.min() &&
         hist0.max()  == hist3.max());
  for (unsigned i = 0; i < hist0.size(); i++) {
    *ostr << hist0.binlo(i)  << " " 
          << hist0.binmid(i) << " "
          << hist0.binhi(i) << " "
          << hist0[i] * norm << " "
          << hist1[i] * norm << " "
          << hist2[i] * norm << " "
          << hist3[i] * norm << " "
          << std::endl;
  }
}

inline void output(const SimpleHist & hist0, 
                   const SimpleHist & hist1, 
                   const SimpleHist & hist2, 
                   const SimpleHist & hist3, 
                   const SimpleHist & hist4, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  assert(hist0.size() == hist1.size() && 
         hist0.min()  == hist1.min() &&
         hist0.max()  == hist1.max());
  assert(hist0.size() == hist2.size() && 
         hist0.min()  == hist2.min() &&
         hist0.max()  == hist2.max());
  assert(hist0.size() == hist3.size() && 
         hist0.min()  == hist3.min() &&
         hist0.max()  == hist3.max());
  assert(hist0.size() == hist4.size() && 
         hist0.min()  == hist4.min() &&
         hist0.max()  == hist4.max());
  for (unsigned i = 0; i < hist0.size(); i++) {
    *ostr << hist0.binlo(i)  << " " 
          << hist0.binmid(i) << " "
          << hist0.binhi(i) << " "
          << hist0[i] * norm << " "
          << hist1[i] * norm << " "
          << hist2[i] * norm << " "
          << hist3[i] * norm << " "
          << hist4[i] * norm << " "
          << std::endl;
  }
}

inline void output(const SimpleHist & hist0, 
                   const SimpleHist & hist1, 
                   const SimpleHist & hist2, 
                   const SimpleHist & hist3, 
                   const SimpleHist & hist4, 
                   const SimpleHist & hist5, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  assert(hist0.size() == hist1.size() && 
         hist0.min()  == hist1.min() &&
         hist0.max()  == hist1.max());
  assert(hist0.size() == hist2.size() && 
         hist0.min()  == hist2.min() &&
         hist0.max()  == hist2.max());
  assert(hist0.size() == hist3.size() && 
         hist0.min()  == hist3.min() &&
         hist0.max()  == hist3.max());
  assert(hist0.size() == hist4.size() && 
         hist0.min()  == hist4.min() &&
         hist0.max()  == hist4.max());
  assert(hist0.size() == hist5.size() && 
         hist0.min()  == hist5.min() &&
         hist0.max()  == hist5.max());
  for (unsigned i = 0; i < hist0.size(); i++) {
    *ostr << hist0.binlo(i)  << " " 
          << hist0.binmid(i) << " "
          << hist0.binhi(i)  << " "
          << hist0[i] * norm << " "
          << hist1[i] * norm << " "
          << hist2[i] * norm << " "
          << hist3[i] * norm << " "
          << hist4[i] * norm << " "
          << hist5[i] * norm << " "
          << std::endl;
  }
}

inline void output(const SimpleHist *hists, 
                   const unsigned int nb_hist, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  unsigned int ih;
  for (ih=1;ih<nb_hist;ih++)
    assert(hists[0].size() == hists[ih].size() && 
	   hists[0].min()  == hists[ih].min() &&
	   hists[0].max()  == hists[ih].max());

  for (unsigned i = 0; i < hists[0].size(); i++) {
    *ostr << hists[0].binlo(i)  << " " 
          << hists[0].binmid(i) << " "
          << hists[0].binhi(i)  ;
    for (ih=0;ih<nb_hist;ih++)
      *ostr << " " << hists[ih][i] * norm;
    *ostr << std::endl;
  }
}

inline void output(const std::vector<SimpleHist*> &hists, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  unsigned int ih;
  unsigned int nb_hist = hists.size();
  for (ih=1;ih<nb_hist;ih++)
    assert(hists[0]->size() == hists[ih]->size() && 
	   hists[0]->min()  == hists[ih]->min() &&
	   hists[0]->max()  == hists[ih]->max());

  for (unsigned i = 0; i < hists[0]->size(); i++) {
    *ostr << hists[0]->binlo(i)  << " " 
          << hists[0]->binmid(i) << " "
          << hists[0]->binhi(i)  ;
    for (ih=0;ih<nb_hist;ih++)
      *ostr << " " << (*hists[ih])[i] * norm;
    *ostr << std::endl;
  }
}

inline void output(const std::vector<SimpleHist> &hists, 
                   std::ostream * ostr = (&std::cout),
                   double norm = 1.0) {
  unsigned int ih;
  unsigned int nb_hist = hists.size();
  for (ih=1;ih<nb_hist;ih++)
    assert(hists[0].size() == hists[ih].size() && 
	   hists[0].min()  == hists[ih].min() &&
	   hists[0].max()  == hists[ih].max());

  for (unsigned i = 0; i < hists[0].size(); i++) {
    *ostr << hists[0].binlo(i)  << " " 
          << hists[0].binmid(i) << " "
          << hists[0].binhi(i)  ;
    for (ih=0;ih<nb_hist;ih++)
      *ostr << " " << hists[ih][i] * norm;
    *ostr << std::endl;
  }
}

#endif // __SIMPLEHIST_HH__

///////////////////////////////////////////////////////////////////////////////
// File: CmdLine.hh                                                          //
// Part of the CmdLine library
//                                                                           //
// Copyright (c) 2007 Gavin Salam                                            //
//                                                                           //
// This program is free software; you can redistribute it and/or modify      //
// it under the terms of the GNU General Public License as published by      //
// the Free Software Foundation; either version 2 of the License, or         //
// (at your option) any later version.                                       //
//                                                                           //
// This program is distributed in the hope that it will be useful,           //
// but WITHOUT ANY WARRANTY; without even the implied warranty of            //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             //
// GNU General Public License for more details.                              //
//                                                                           //
// You should have received a copy of the GNU General Public License         //
// along with this program; if not, write to the Free Software               //
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA //
//                                                                           //
// $Revision:: 139                                                          $//
// $Date:: 2007-01-23 16:09:23 +0100 (Tue, 23 Jan 2007)                     $//
///////////////////////////////////////////////////////////////////////////////


#ifndef __CMDLINE__
#define __CMDLINE__

#include<string>
#include<sstream>
#include<map>
#include<vector>
#include<ctime>
using namespace std;

/// Class designed to deal with command-line arguments in a fashion similar
/// to what was done in f90 iolib.
///
/// Note that functionality might be slightly different? 
/// Currently do not implement access to arguments by index
/// though data structure would in principle allow this quite easily.
///
/// GPS 03/01/05
/// [NB: wonder if some of this might be more efficiently written 
/// with templates for different type that can be read in...]
///
/// Other question: dealing with list of options is rather common
/// occurrence -- command-line arguments, but also card files; maybe one
/// could somehow use base/derived classes to share common functionality? 
///
class CmdLine {
  mutable map<string,int> __options;
  vector<string> __arguments;
  mutable map<string,bool> __options_used;
  //string __progname;
  string __command_line;
  std::time_t __time_at_start;

 public :
  CmdLine() {};
  /// initialise a CmdLine from a C-style array of command-line arguments
  CmdLine(const int argc, char** argv);
  /// initialise a CmdLine from a C++ vector of arguments 
  CmdLine(const vector<string> & args);

  /// true if the option is present
  bool    present(const string & opt) const;
  /// true if the option is present and corresponds to a value
  bool    present_and_set(const string & opt) const;

  /// return a reference to the vector of command-line arguments (0 is
  /// command).
  inline const vector<string> & arguments() const {return __arguments;}

  /// returns the value of the argument converted to type T
  template<class T> T value(const string & opt) const;
  /// returns the value of the argument, prefixed with prefix (NB: 
  /// require different function name to avoid confusion with 
  /// 2-arg template).
  template<class T> T value_prefix(const string & opt, const string & prefix) const;
  template<class T> T value(const string & opt, const T & defval) const;
  template<class T> T value(const string & opt, const T & defval, 
                            const string & prefix) const;


  /// return the integer value corresponding to the given option
  int     int_val(const string & opt) const;
  /// return the integer value corresponding to the given option or default if option is absent
  int     int_val(const string & opt, const int & defval) const;

  /// return the double value corresponding to the given option
  double  double_val(const string & opt) const;
  /// return the double value corresponding to the given option or default if option is absent
  double  double_val(const string & opt, const double & defval) const;

  /// return the string value corresponding to the given option
  string  string_val(const string & opt) const;
  /// return the string value corresponding to the given option or default if option is absent
  string  string_val(const string & opt, const string & defval) const;

  /// return the full command line
  string  command_line() const;

  /// return true if all options have been asked for at some point or other
  bool all_options_used() const;

  /// gives an error if there are unused  options
  void assert_all_options_used() const;

  /// return a time stamp (UTC) corresponding to now
  string time_stamp(bool utc = false) const;

  /// return a time stamp (UTC) corresponding to time of object construction
  string time_stamp_at_start(bool utc = false) const;

  /// return output similar to that from uname -a on unix
  string unix_uname() const;

  class Error;

 private:
  /// builds the internal structures needed to keep track of arguments and options
  void init();

  /// report failure of conversion
  void _report_conversion_failure(const string & opt, 
                                  const string & optstring) const;

  /// convert the time into a string (local by default -- utc if 
  /// utc=true).
  std::string _string_time(const time_t & time, bool utc) const;
};



//----------------------------------------------------------------------
/// class that deals with errors
class CmdLine::Error {
public:
  Error(const std::ostringstream & ostr);
  const std::string & message() {return _message;}
  static void set_print_message(bool doprint) {_do_printout = doprint;}
private:
  std::string _message;
  static bool        _do_printout;
};


/// returns the value of the argument converted to type T
template<class T> T CmdLine::value(const string & opt) const {
  return value_prefix<T>(opt,""); // just give it a null prefix
}

/// returns the value of the argument converted to type T
template<class T> T CmdLine::value_prefix(const string & opt, const string & prefix) const {
  T result;
  string optstring = prefix+string_val(opt);
  istringstream optstream(optstring);
  optstream >> result;
  if (optstream.fail()) _report_conversion_failure(opt, optstring);
  return result;
}

/// for the string case, just copy the string...
template<> inline string CmdLine::value<string>(const string & opt) const {
  return string_val(opt);}



template<class T> T CmdLine::value(const string & opt, const T & defval) const {
  if (this->present_and_set(opt)) {return value<T>(opt);} 
  else {return defval;}
}

template<class T> T CmdLine::value(const string & opt, const T & defval, 
                                   const string & prefix) const {
  if (this->present_and_set(opt)) {return value_prefix<T>(opt, prefix);} 
  else {return defval;}
}

#endif

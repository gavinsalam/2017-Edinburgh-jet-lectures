///////////////////////////////////////////////////////////////////////////////
// File: CmdLine.cc                                                          //
// Part of the CmdLine library                                               //
//                                                                           //
// Copyright (c) 2007-2012 Gavin Salam                                       //
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



#include "CmdLine.hh"
#include<string>
#include<sstream>
#include<iostream> // testing
#include<vector>
#include<cstddef> // for size_t
#include <sys/utsname.h> // for getting uname
using namespace std;

// initialise the various structures that we shall
// use to access the command-line options;
//
// If an option appears several times, it is its LAST value
// that will be used in searching for option values (opposite of f90)
CmdLine::CmdLine (const int argc, char** argv) {
  __arguments.resize(argc);
  for(int iarg = 0; iarg < argc; iarg++){
    __arguments[iarg] = argv[iarg];
  }
  this->init();
}

/// constructor from a vector of strings, one argument per string
CmdLine::CmdLine (const vector<string> & args) {
  __arguments = args;
  this->init();
}

//----------------------------------------------------------------------
void CmdLine::init (){
  // record time at start
  time(&__time_at_start);

  // record whole command line so that it can be easily reused
  __command_line = "";
  for(size_t iarg = 0; iarg < __arguments.size(); iarg++){
    const string & arg = __arguments[iarg];
    // if an argument contains special characters, enclose it in
    // single quotes [NB: does not work if it contains a single quote
    // itself]
    if (arg.find(' ') != string::npos ||
        arg.find('|') != string::npos ||
        arg.find('<') != string::npos || 
        arg.find('>') != string::npos || 
        arg.find('"') != string::npos || 
        arg.find('#') != string::npos) {
      __command_line += "'"+arg+"'";
    } else {
      __command_line += __arguments[iarg];
    }
    __command_line += " ";
  }
  
  // group things into options
  bool next_may_be_val = false;
  string currentopt;
  for(size_t iarg = 1; iarg < __arguments.size(); iarg++){
    // if expecting an option value, then take it (even if
    // it is actually next option...)
    if (next_may_be_val) {__options[currentopt] = iarg;}
    // now see if it might be an option itself
    string arg = __arguments[iarg];
    bool thisisopt = (arg.compare(0,1,"-") == 0);
    if (thisisopt) {
      // set option to a standard undefined value and say that 
      // we expect (possibly) a value on next round
      currentopt = arg;
      __options[currentopt] = -1;
      __options_used[currentopt] = false;
      next_may_be_val = true;}
    else {
      // otherwise throw away the argument for now...
      next_may_be_val = false;
      currentopt = "";
    }
  }
}

// indicates whether an option is present
bool CmdLine::present(const string & opt) const {
  bool result = (__options.find(opt) != __options.end());
  if (result) __options_used[opt] = true;
  return result;
}

// indicates whether an option is present and has a value associated
bool CmdLine::present_and_set(const string & opt) const {
  bool result = present(opt) && __options[opt] > 0;
  return result;
}


// return the string value corresponding to the specified option
string CmdLine::string_val(const string & opt) const {
  if (!this->present_and_set(opt)) {
    ostringstream ostr;
    ostr << "Option "<<opt
	 <<" is needed but is not present_and_set"<<endl;
    throw Error(ostr);
  }
  string arg = __arguments[__options[opt]];
  // this may itself look like an option -- if that is the case
  // declare the option to have been used
  if (arg.compare(0,1,"-") == 0) {__options_used[arg] = true;}
  return arg;
}

// as above, but if opt is not present_and_set, return default
string CmdLine::string_val(const string & opt, const string & defval) const {
  if (this->present_and_set(opt)) {return string_val(opt);} 
  else {return defval;}
}

// Return the integer value corresponding to the specified option;
// Not too sure what happens if option is present_and_set but does not
// have string value...
int CmdLine::int_val(const string & opt) const {
  int result;
  string optstring = string_val(opt);
  istringstream optstream(optstring);
  optstream >> result;
  if (optstream.fail()) {
    ostringstream ostr;
    ostr << "could not convert option ("<<opt<<") value ("
	 <<optstring<<") to int"<<endl; 
    throw Error(ostr);
  }
  return result;
}

// as above, but if opt is not present_and_set, return default
int CmdLine::int_val(const string & opt, const int & defval) const {
  if (this->present_and_set(opt)) {return int_val(opt);} 
  else {return defval;}
}


// Return the integer value corresponding to the specified option;
// Not too sure what happens if option is present_and_set but does not
// have string value...
double CmdLine::double_val(const string & opt) const {
  double result;
  string optstring = string_val(opt);
  istringstream optstream(optstring);
  optstream >> result;
  if (optstream.fail()) {
    ostringstream ostr;

    ostr << "could not convert option ("<<opt<<") value ("
	 <<optstring<<") to double"<<endl; 
    throw Error(ostr);
  }
  return result;
}

// as above, but if opt is not present_and_set, return default
double CmdLine::double_val(const string & opt, const double & defval) const {
  if (this->present_and_set(opt)) {return double_val(opt);} 
  else {return defval;}
}


// return the full command line including the command itself
string CmdLine::command_line() const {
  return __command_line;
}


// return true if all options have been asked for at some point or other
bool CmdLine::all_options_used() const {
  bool result = true;
  for(map<string,bool>::const_iterator opt = __options_used.begin();
      opt != __options_used.end(); opt++) {
    bool this_one = opt->second;
    if (! this_one) {cerr << "Option "<<opt->first<<" unused/unrecognized"<<endl;}
    result = result && this_one;
  }
  return result;
}

/// return a time stamp corresponding to now
string CmdLine::time_stamp(bool utc) const {
  time_t timenow;
  time(&timenow);
  return _string_time(timenow, utc);
}

/// return a time stamp corresponding to start time
string CmdLine::time_stamp_at_start(bool utc) const {
  return _string_time(__time_at_start, utc);
}


/// convert the time into a string (local by default -- utc if 
/// utc=true).
string CmdLine::_string_time(const time_t & time, bool utc) const {
  struct tm * timeinfo;
  if (utc) {
    timeinfo = gmtime(&time);
  } else {
    timeinfo = localtime(&time);
  }
  char timecstr[100];
  strftime (timecstr,100,"%Y-%m-%d %H:%M:%S (%Z)",timeinfo);
  //sprintf(timecstr,"%04d-%02d-%02d %02d:%02d:%02d",
  //        timeinfo->tm_year+1900,
  //        timeinfo->tm_mon+1,
  //        timeinfo->tm_mday,
  //        timeinfo->tm_hour,
  //        timeinfo->tm_min,
  //        timeinfo->tm_sec);
  //string timestr = timecstr;
  //if (utc) {
  //  timestr .= " (UTC)";
  //} else {
  //  timestr .= " (local)";
  //}
  return timecstr;
}

/// return a unix-style uname
string CmdLine::unix_uname() const {
  utsname utsbuf;
  int utsret = uname(&utsbuf);
  if (utsret != 0) {return "Error establishing uname";}
  ostringstream uname_result;
  uname_result << utsbuf.sysname << " " 
               << utsbuf.nodename << " "
               << utsbuf.release << " "
               << utsbuf.version << " "
               << utsbuf.machine;
  return uname_result.str();
}

/// report failure of conversion
void CmdLine::_report_conversion_failure(const string & opt, 
                                         const string & optstring) const {
  ostringstream ostr;
  ostr << "could not convert option ("<<opt<<") value ("
       <<optstring<<") to requested type"<<endl; 
  throw Error(ostr);
}

void CmdLine::assert_all_options_used() const {
  if (! all_options_used()) {
    ostringstream ostr;
    ostr <<"Unrecognised options on the command line" << endl;
    throw Error(ostr);
  }
}


bool CmdLine::Error::_do_printout = true;
CmdLine::Error::Error(const std::ostringstream & ostr) 
  : _message(ostr.str()) {
  if (_do_printout) cerr << "CmdLine Error: " << _message;
}

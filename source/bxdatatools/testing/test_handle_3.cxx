// -*- mode: c++; -*-
// test_handle_3.cxx
// Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>

#include <datatools/handle.h>

using namespace std;

#define DATATOOLS_FOREACH_MUTABLE_REF_HANDLE_BEGIN(TYPE, VAR, HCOL)     \
  BOOST_FOREACH(handle<TYPE> & __mutable_handle, HCOL)                  \
  {                                                                     \
  if (! __mutable_handle.has_data ()) continue;                         \
  TYPE & VAR = __mutable_handle.grab ();                                \
  /**/

#define DATATOOLS_FOREACH_CONST_REF_HANDLE_BEGIN(TYPE, VAR, HCOL)       \
  BOOST_FOREACH(const handle<TYPE> & __const_handle, HCOL)              \
  {                                                                     \
  if (! __const_handle.has_data ()) continue;                           \
  const TYPE & VAR = __const_handle.get ();                             \
  /**/

#define DATATOOLS_FOREACH_REF_HANDLE_END        \
  }                                             \
  /**/


int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test of the 'handle<>' template class..." << endl;
    bool debug = false;

    int iarg =  1;
    while (iarg < argc_) {
      string arg = argv_[iarg];
      if ((arg == "-d") || (arg == "--debug")) debug = true;
      iarg++;
    }

    using namespace datatools;

    typedef handle<int>             int_handle_type;
    typedef vector<int_handle_type> int_handle_collection_type;

    int_handle_collection_type ints;
    for (int i = 0; i < 5; i++){
      ints.push_back (int_handle_type (new int (i)));
    }

    cout << "Print the list of ints : " << endl;
    BOOST_FOREACH(const handle<int> & hi, ints){
      if (! hi.has_data ()) continue;
      const int & i = hi.get ();
      {
        cout << "i=" << i << endl;
      }
    }
    cout << endl;

    cout << "Print the list of ints : " << endl;
    DATATOOLS_FOREACH_CONST_REF_HANDLE_BEGIN(int,i,ints) {
      cout << "i=" << i << endl;
    }
    DATATOOLS_FOREACH_REF_HANDLE_END
      cout << endl;

    cout << "Modify the ints in the list... " << endl;
    DATATOOLS_FOREACH_MUTABLE_REF_HANDLE_BEGIN(int,i,ints) {
      i = 666;
    }
    DATATOOLS_FOREACH_REF_HANDLE_END
      cout << endl;

    cout << "Print the list of ints : " << endl;
    DATATOOLS_FOREACH_CONST_REF_HANDLE_BEGIN(int,i,ints) {
      cout << "i=" << i << endl;
    }
    DATATOOLS_FOREACH_REF_HANDLE_END
      cout << endl;

  }
  catch (exception & x) {
    clog << "error: " << x.what () << endl;
    error_code =  EXIT_FAILURE;
  }
  catch (...) {
    clog << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_handle_3.cxx


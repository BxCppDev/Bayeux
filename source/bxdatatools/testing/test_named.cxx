// -*- mode: c++; -*-
// test_named.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <datatools/i_named.h>

using namespace std;

struct foo : public datatools::i_named
{
private:
  string __prefix;
  int    __id;
public:
  foo (const string & prefix_ , int id_) :
    __prefix (prefix_), __id (id_)
  {
  }
  virtual string get_name ()
  {
    ostringstream name_ss;
    name_ss << __prefix << __id;
    return name_ss.str ();
  }
};

int main (// int argc_ , char ** argv_
          )
{
  int error_code = EXIT_SUCCESS;
  try
    {

      clog << "Test of the 'i_named' interface..." << endl;
      // bool debug = false;

      // int iarg =  1;
      // while (iarg < argc_)
      //   {
      //     string arg = argv_[iarg];

      //     if ((arg == "-d") || (arg == "--debug")) debug = true;

      //     iarg++;
      //   }

      foo t1 ("t",1);
      foo t2 ("t",2);
      foo t3 ("t",666);

      clog << "t1 is named '" << t1.get_name () << "'" <<  endl;
      clog << "t2 is named '" << t2.get_name () << "'" <<  endl;
      clog << "t3 is named '" << t3.get_name () << "'" <<  endl;

    }
  catch (exception & x)
    {
      clog << "error: " << x.what () << endl;
      error_code =  EXIT_FAILURE;
    }
  catch (...)
    {
      clog << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_named.cxx

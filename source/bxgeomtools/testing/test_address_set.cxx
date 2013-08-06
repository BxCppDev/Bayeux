// -*- mode: c++; -*-
// test_address_set.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>

#include <geomtools/address_set.h>

using namespace std;
using namespace geomtools;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Hello, this is a sample program for class 'address_set'!" << endl;

      bool debug = false;
      bool interactive = false;
      int iarg = 1;
      while (iarg < argc_)
        {

          string arg = argv_[iarg];

          if (arg == "-d" || arg == "--debug") debug = true;
          if (arg == "-i" || arg == "--interactive") interactive = true;

          iarg++;
        }

      address_set as1;
      as1.set_mode_all ();

      uint32_t val = 234;
      if (as1.match (val))
        {
          clog << "Value " << val << " matches the '" << as1 << "' address_set !" << endl;
        }
      else
        {
          clog << "Value " << val << " does not match the '" << as1 << "' address_set !" << endl;
        }

      clog << endl << "Switch to reverse mode..." << endl;
      as1.set_reverse (true);

      if (as1.match (val))
        {
          clog << "Value " << val << " matches the '" << as1 << "' address set !" << endl;
        }
      else
        {
          clog << "Value " << val << " does not match the '" << as1 << "' address set anymore !" << endl;
        }

      clog << endl << "Reset mode..." << endl;
      as1.reset ();
      if (as1.match (val))
        {
          clog << "Value " << val << " matches the '" << as1 << "' address set !" << endl;
        }
      else
        {
          clog << "Value " << val << " does not match the '" << as1 << "' address set !" << endl;
        }

      clog << endl << "Switch to range mode..." << endl;
      as1.set_range (10, 340);
      if (as1.match (val))
        {
          clog << "Value " << val << " matches the '" << as1 << "' address set !" << endl;
        }
      else
        {
          clog << "Value " << val << " does not match the '" << as1 << "' address set !" << endl;
        }

      clog << endl << "Switch to list mode..." << endl;
      as1.set_mode_list ();
      for (uint32_t v = 34; v < 300; v += 10)
        {
          as1.add_to_list (v);
        }
      if (as1.match (val))
        {
          clog << "Value " << val << " matches the '" << as1 << "' address set !" << endl;
        }
      else
        {
          clog << "Value " << val << " does not match the '" << as1 << "' address set !" << endl;
        }

      if (interactive) {
        while (true)
          {
            address_set as2;
            string line;
            clog << "Enter an address set : ";
            getline (cin, line);
            istringstream iss (line);
            iss >> ws;
            if (iss.eof ()) break;
            iss >> as2;
            if (! iss)
              {
                ostringstream message;
                message << "Invalid format for address set !";
                throw runtime_error (message.str ());
              }
            cout << "Address set is : " << as2 << endl;
          }
      }

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_address_set.cxx

// -*- mode: c++; -*-
// test_things_macros.cxx
// Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/utils/things.h>
#include <datatools/utils/things_macros.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/multi_properties.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test of the 'things_macros'..." << endl;
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];
          if ((arg == "-d") || (arg == "--debug")) debug = true;
          iarg++;
        }

      datatools::utils::things bag;

      {
        DATATOOLS_UTILS_THINGS_ADD_BANK(bag, "Prop", datatools::utils::properties, p);
        p.store ("pi", 3.14159);
        p.store ("animal", "lion");
      }

      {
        DATATOOLS_UTILS_THINGS_ADD_BANK(bag, "Test", datatools::utils::properties, t);
        t.store_flag ("test");
        t.store ("answer", 42);
      }

      {
        DATATOOLS_UTILS_THINGS_CONST_BANK(bag, "Prop", datatools::utils::properties, p);
        p.tree_dump (std::clog, "Prop (with pi and lion)");
      }
 
      bag.tree_dump (std::clog, "bag");

      {
        DATATOOLS_UTILS_THINGS_MUTABLE_BANK(bag, "Prop", datatools::utils::properties, p);
        p.erase ("pi");
        p.tree_dump (std::clog, "Prop (pi removed)");
      }

      if (! DATATOOLS_UTILS_THINGS_CHECK_BANK (bag, "Bar", datatools::utils::properties))
        {
          clog << "Ok ! We cannot find a bank named '" << "Bar" << "' as expected !" << endl;   
        }

      try
        {
          DATATOOLS_UTILS_THINGS_CONST_BANK(bag, "Foo", 
                                            datatools::utils::multi_properties, m);
          m.tree_dump (std::clog, "Foo");
        }
      catch (exception & x)
        {
          clog << "Ok ! We have a missing '" << "Foo" << "' bank as expected !" << endl;
        }
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

// end of test_things_macros.cxx


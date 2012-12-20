// -*- mode: c++; -*- 
// test_library_loader_1.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <datatools/datatools_config.h>
#include <datatools/detail/DynamicLoader.h>

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    { 
      std::clog << "Test of the 'datatools::library_loader' class." << std::endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg <  argc_) 
        {
          std::string arg = argv_[iarg];
          if (boost::algorithm::starts_with (arg, "-"))
            {
              if (arg == "-d" || arg == "--debug") 
                {
                  debug =  true;
                }
            }
          iarg++;
        }
      
      std::vector<datatools::detail::DynamicLoader::LibraryHandle> h;

      std::clog << "************* Loading 'ssl'..." << std::endl;
      h.push_back (datatools::detail::DynamicLoader::OpenLibrary ("libssl.so"));
 
      std::clog << "************* Loading 'mygsl'..." << std::endl;
      h.push_back (datatools::detail::DynamicLoader::OpenLibrary ("libmygsl.so"));
 
      std::clog << "************* Loading 'geomtools'..." << std::endl;
      h.push_back (datatools::detail::DynamicLoader::OpenLibrary ("libgeomtools.so"));

      std::clog << "************* Loading 'crypto'..." << std::endl;
      h.push_back (datatools::detail::DynamicLoader::OpenLibrary ("libcrypto.so"));

      std::clog << "************* Load is done." << std::endl;

      for (int i = (h.size () - 1) ; i >= 0 ; --i)
        {
          std::clog << "************* Closing library i=" << i << std::endl;
          datatools::detail::DynamicLoader::CloseLibrary (h[i]);
        }

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_library_loader_0.cxx

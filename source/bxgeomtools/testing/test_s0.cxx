/* test_s0.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>


//#include <datatools/serialization/io_factory.h>

// a serializable test class :
//#include <datatools_test_my_data.cc>
//#include <datatools_test_my_data.ipp>

// Some pre-processor guard about Boost I/O usage and linkage :
//#include <datatools/serialization/bio_guard.h>

// Special Boost serialization export/registration code :
// BOOST_CLASS_EXPORT_KEY2(datatools::test::data_t, "datatools::test::data_t")
// BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::data_t)
// BOOST_CLASS_EXPORT_KEY2(datatools::test::more_data_t, "datatools::test::more_data_t")
// BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::more_data_t)

//#include <datatools/serialization/check.h>

using namespace std;

int main (/*int argc_, char ** argv_*/)
{
  clog << endl << "**************** Starting program..." << endl;
  try
    {

    }
  catch (exception & x)
    {
      clog << "test_s0: ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  clog << endl << "**************** Stopping program." << endl;

  return (EXIT_SUCCESS);
}

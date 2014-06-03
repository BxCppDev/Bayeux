/* test_data_serialization.cxx */

// Standard C++ file streams:
#include <fstream>

// The text (ASCII) archives from Boost:
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

// The user's class:
#include <datatools_test_data.h>

using namespace std;

int main (int /*argc_*/, char ** /*argv_*/)
{
  /** Serialize a single instance of class 'data' in a
   *  text (ASCII) archive associated to an output file
   */
  {
    ofstream output_stream ("test_data_serialization.txt");
    boost::archive::text_oarchive oa (output_stream);

    datatools::test::data a_data;
    // oa << a_data;
    oa & a_data;
    output_stream.flush ();
  }

  /** Deserialize a single instance of class 'data' in a
   *  text (ASCII) archive associated to an output file
   */
  {
    ifstream input_stream ("test_data_serialization.txt");
    boost::archive::text_iarchive ia (input_stream);

    datatools::test::data a_data;
    ia >> a_data;
  }

  return 0;
}

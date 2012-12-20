/* test_binary_serialization.cxx */

// Standard C++ file streams:
#include <iostream>
#include <fstream>
#include <limits>

// The quasi portable binary archives :
#include <datatools/eos/portable_iarchive.hpp>
#include <datatools/eos/portable_oarchive.hpp>

#include <boost/cstdint.hpp>
#include <boost/serialization/string.hpp>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>

using namespace std;

int main (int argc_, char ** argv_) 
{
  /** Serialize **/ 
  {
    clog << "Storing..." << endl;
    ofstream output_stream ("test_binary_serialization.data");
    eos::portable_oarchive oa (output_stream);
    clog << "Output archive is setup !" << endl;
    uint32_t i0 = 0; 
    uint32_t i1 = 42;  
    uint32_t i2 = 666; 
    uint32_t i3 = 100000000; 
    uint32_t i4 = 0xFFFFFFFF; 
    int32_t  i5 = -1; 
    clog << "i0 = '" << i0 << "'" << endl;
    clog << "i1 = '" << i1 << "'" << endl;
    clog << "i2 = '" << i2 << "'" << endl;
    clog << "i3 = '" << i3 << "'" << endl;
    clog << "i4 = '" << i4 << "'" << endl;
    clog << "i5 = '" << i5 << "'" << endl;
    oa << i0 << i1 << i2 << i3 << i4 << i5;
    string toto = "toto";
    clog << "Toto = '" << toto << "'" << endl;
    oa << toto;
    double x1 = 3.14159;
    clog << "x1 = '" << x1 << "'" << endl;
    oa << x1;
    double x2 = std::numeric_limits<double>::quiet_NaN ();
    clog << "x2 = '" << x2 << "'" << endl;
    oa << x2;
    double x3 = std::numeric_limits<double>::infinity();
    clog << "x3 = '" << x3 << "'" << endl;
    oa << x3;
    double x4 = -std::numeric_limits<double>::infinity();
    clog << "x4 = '" << x4 << "'" << endl;
    oa << x4;
  }

  /** Deserialize **/
  {
    clog << endl << endl << "Loading..." << endl;
    ifstream input_stream ("test_binary_serialization.data");
    eos::portable_iarchive ia (input_stream);
    clog << "Input archive is setup !" << endl;
    uint32_t i0; 
    uint32_t i1;  
    uint32_t i2; 
    uint32_t i3; 
    uint32_t i4; 
    int32_t  i5; 

    ia >> i0;
    clog << "i0 = '" << i0 << "'" << endl;
    ia >> i1;
    clog << "i1 = '" << i1 << "'" << endl;
    ia >> i2;
    clog << "i2 = '" << i2 << "'" << endl;
    ia >> i3;
    clog << "i3 = '" << i3 << "'" << endl;
    ia >> i4;
    clog << "i4 = '" << i4 << "'" << endl;
    ia >> i5;
    clog << "i5 = '" << i5 << "'" << endl;

    string toto;
    ia >> toto;
    clog << "Toto = '" << toto << "'" << endl;

    double x1;
    ia >> x1;
    clog << "x1 = '" << x1 << "'" << endl;
    double x2;
    ia >> x2;
    clog << "x2 = '" << x2 << "'" << endl;
    double x3;
    ia >> x3;
    clog << "x3 = '" << x3 << "'" << endl;
    double x4;
    ia >> x4;
    clog << "x4 = '" << x4 << "'" << endl;
  }

  return 0;
}

/* end of test_binary_serialization.cxx */

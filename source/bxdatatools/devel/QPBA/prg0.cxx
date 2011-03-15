//

#include <iostream>
#include <fstream>
#include <exception>

#include <boost/version.hpp>
#include <boost/cstdint.hpp>
#include <boost/archive/text_oarchive.hpp>

using namespace std;

int main (void)
{
  try
    {
      cout << "BOOST version = " << BOOST_VERSION << endl;

    }
  catch (std::exception &x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}

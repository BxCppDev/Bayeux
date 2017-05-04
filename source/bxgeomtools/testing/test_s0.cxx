/* test_s0.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>


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

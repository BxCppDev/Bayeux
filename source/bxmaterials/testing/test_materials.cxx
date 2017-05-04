// test_materials.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <materials/materials_config.h>
#include <materials/version.h>

int main (/*int argc_, char ** argv_*/)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'materials' program library." << endl;

      clog << "materials version : " << MATERIALS_LIB_VERSION << endl;

      clog << "The end." << endl;
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error !" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

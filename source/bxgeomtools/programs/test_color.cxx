// -*- mode: c++; -*- 
// test_color.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/color.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
  {
    cerr << "Hello, this is a sample program for class 'color'!" 
	      << endl; 
  
    bool debug = false;

    int iarg = 1;
    while (iarg < argc_)
    {
    
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;

      iarg++;
    }
    
    geomtools::color color;
    color.code = 1;
    color.name = "red";
    color.r = 1;
    color.g = 0;
    color.b = 0;

    string color_name = "green";
    int color_index = geomtools::color::get_color (color_name);
    cout << "Color '" << color_name << "' index is : " << color_index << endl; 

  }
  catch (exception & x)
  {
    cerr << "error: " << x.what() << endl; 
    error_code = EXIT_FAILURE;
  }
  catch (...)
  {
    cerr << "error: " << "unexpected error!" << endl; 
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_dummy_geomtools.cxx

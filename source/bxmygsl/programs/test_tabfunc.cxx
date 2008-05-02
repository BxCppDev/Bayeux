// -*- mode: c++; -*- 
// test_tabfunc.cxx
/*
 * Test program for the 'tabulated_function' class
 *
 * shell$ test_tabfunc > test_tabfunc.data
 *
 * gnuplot> plot [0:2][0:1.2] \
 *   'test_tabfunc.data' index 0 w dots, \
 *   'test_tabfunc.data' index 1 w points ps 1 pt 6 lt 3, \
 *   cos(x) w lines lt 4
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <vector>

#include <mygsl/tabfunc.h>

int main(int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      std::cerr << "Hello, this is a sample program for package 'mygsl'!" 
		<< std::endl; 
  
      mygsl::tabulated_function tf("cspline"); // also available "linear";
    
      std::vector<double> xs;
      xs.reserve(7);
      xs.push_back(0.0);
      xs.push_back(0.1);
      xs.push_back(0.3);
      xs.push_back(0.7);
      xs.push_back(1.1);
      xs.push_back(1.3);
      xs.push_back(1.4);

      for (int i = 0; i < xs.size(); i++) 
	{
	  tf.add_point(xs[i],std::cos(xs[i]),i==xs.size()-1);
	}
      tf.lock_table("linear"); // change interpolation scheme

      std::cerr.precision(10);
      std::cerr << "DEBUG: min=" << tf.x_min() << std::endl; 
      std::cerr << "DEBUG: max=" << tf.x_max() << std::endl; 
      double dx = 0.01;
      for (double x = tf.x_min(); x <= tf.x_max() + 0.001 * dx; x += dx) 
	{
	  if (tf.in_range(x)) 
	    {
	      std::cout << x << ' ' << tf(x) << std::endl;
	    }
	}
      std::cout << std::endl << std::endl;

      tf.print_points(std::cout);
      std::cout << std::endl << std::endl;

    }
  catch(std::exception & x)
    {
      std::cerr << "ERROR: " << x.what() << std::endl; 
      error_code=EXIT_FAILURE;
    }
  catch(...)
    {
      std::cerr << "ERROR: " << "unexpected error!" << std::endl; 
      error_code=EXIT_FAILURE;
    }
  return error_code;
}

// end of test_tabfunc.cxx

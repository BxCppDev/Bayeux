// -*- mode: c++; -*- 
// test_sphere.cxx
// gnuplot macro: ./tests/test_sphere.gpl

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/sphere.h>
#include <geomtools/gnuplot_draw.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      long seed  = 314159;
      bool test1 = true;
      bool test2 = true;
      bool test3 = true;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;
	  if (arg == "-t1" || arg == "--test1") test1 = false;
	  if (arg == "-t2" || arg == "--test2") test2 = false;
	  if (arg == "-t3" || arg == "--test3") test3 = false;

	  iarg++;
	}
    
      srand48 (seed);
    
      // test 1:
      if (test1) 
	{
	  geomtools::sphere my_sphere (1000 * CLHEP::mm);

	  clog << "test 1: Sphere #1 = " << my_sphere << " " << endl;

	  clog << "test 1: Volume   = " << my_sphere.get_volume () / CLHEP::m3 << " m3" << endl;

	  clog << "test 1: Side surface = " 
		    << my_sphere.get_surface (geomtools::sphere::FACE_SIDE) / CLHEP::m2 
		    << " m2" << endl;

	  clog << "test 1: Full surface = " 
		    << my_sphere.get_surface (geomtools::sphere::FACE_ALL) / CLHEP::m2 
		    << " m2" << endl;

	  {
	    bool test_in = false;
	    if (test_in)
	      {
		clog << "test 1: Enter a sphere (example: '{sphere 1000}'): ";
		cin >> ws >> my_sphere;
		if (cin) 
		  {
		    clog << "test 1: Sphere = " << my_sphere << " " << endl;
		  }
		else 
		  {
		    throw runtime_error ("Invalid input for sphere!");
		  }
	      }
	  }
	}

      // test 2:
      if (test2) 
	{
	  geomtools::sphere my_sphere (3.0 * CLHEP::mm);
	  clog << "test 2: Sphere #2   = " << my_sphere << " " << endl;
      
	  geomtools::vector_3d pos (4.0 * CLHEP::mm, 
				    0.0 * CLHEP::mm, 
				    0.0 * CLHEP::mm);
	  geomtools::vector_3d dir (-1, 0, 0);
	  geomtools::intercept_t intercept;
	  int intercept_face;
	  if (my_sphere.find_intercept (pos, dir, 
					intercept))
	    {
	      clog << "test 2: Intercept face=" << intercept.get_face ()
			<< " at impact=" << intercept.get_impact ()
			<< endl;
	    }
	  else
	    {
	      clog << "test 2: No intercept." << endl;
	    }

	  geomtools::vector_3d sphere_pos;
	  geomtools::rotation sphere_rot;
	  geomtools::create_rotation (sphere_rot, 0.0, 0.0, 0.0);
	  geomtools::gnuplot_draw::draw_sphere (cout, 
						sphere_pos, 
						sphere_rot, 
						my_sphere);
	  cout << endl << endl;

	  geomtools::gnuplot_draw::basic_draw_point (cout, pos);
	  cout << endl << endl;

	  geomtools::gnuplot_draw::basic_draw_point (cout, 
						     intercept.get_impact ());
	  cout << endl << endl;

	  geomtools::gnuplot_draw::basic_draw_point (cout, pos);
	  geomtools::gnuplot_draw::basic_draw_point (cout, pos + dir);
	  cout << endl << endl;
	}
 
      // test 3:
      if (test3) 
	{
	  geomtools::sphere my_sphere (2.0 * CLHEP::mm);
	  clog << "test 3: Sphere #3   = " << my_sphere 
		    << " " << endl;
	  geomtools::vector_3d sphere_pos;
	  geomtools::rotation sphere_rot;
	  geomtools::create_rotation (sphere_rot, 0.0, 0.0, 0.0);
	  geomtools::gnuplot_draw::draw_sphere (cout, 
						sphere_pos, 
						sphere_rot, 
						my_sphere);
	  cout << endl << endl;
	
	  size_t nshoots = 100000;
	  for (int i = 0; i < (int) nshoots; i++)
	    {
	      double dim = 4. * CLHEP::mm;
	      //dim = 0. * CLHEP::mm;
	      geomtools::vector_3d pos (dim * drand48 (),
					dim * drand48 (),
					dim * drand48 ());
	      //pos.setZ (3. * CLHEP::mm);
	      geomtools::vector_3d dir;
	      geomtools::randomize_direction (drand48, dir);

	      geomtools::intercept_t intercept;
	      if (my_sphere.find_intercept (pos, dir, 
					    intercept))
		{
		  if (debug) clog << "test 3: Intercept face=" 
				       << intercept.get_face ()
				       << " at impact=" << intercept.get_impact ()
				       << endl;
		  geomtools::gnuplot_draw::basic_draw_point (cout, 
							     intercept.get_impact ());
		}
	      else
		{
		  if (debug) clog << "test 3: No intercept." << endl;
		}
	    }
	}
	
	
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...) 
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_sphere.cxx

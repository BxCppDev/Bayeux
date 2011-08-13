// -*- mode: c++ ; -*- 
// test_disk.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/disk.h>
#include <geomtools/gnuplot_draw.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'disk'!" << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token; 
               if ((option == "-d") || (option == "--debug")) 
                 {
                   debug = true;
                 }
               else 
                 { 
                    clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    
      geomtools::disk my_disk;
      double radius = 1.0;
      my_disk.set_r (radius);
      my_disk.tree_dump (clog, "my_disk");

      {
	geomtools::vector_3d disk_pos;
	geomtools::rotation_3d disk_rot;
	geomtools::create_rotation (disk_rot, 0.0, 0.0, 0.0);
	geomtools::gnuplot_draw::draw_disk (cout, 
					    disk_pos,
					    disk_rot, 
					    my_disk);
	cout << endl << endl;
	size_t nshoots = 10000;
	for (int i = 0; i < (int) nshoots; i++)
	  {
	    geomtools::vector_3d pos (2.* drand48 () * CLHEP::mm, 
				      2.* drand48 () * CLHEP::mm, 
				      -2 + 4.* drand48 () * CLHEP::mm);
	    geomtools::vector_3d dir;
	    geomtools::randomize_direction (drand48, dir);
	    geomtools::intercept_t intercept;
	    int intercept_face;
	    if (my_disk.find_intercept (pos, dir, 
					intercept))
	      {
		geomtools::gnuplot_draw::basic_draw_point (cout, 
							   intercept.get_impact ());
	      }
	  }
 	cout << endl << endl;
 
	// not working yet:
	nshoots = 1000;
 	for (int i = 0; i < (int) nshoots; i++)
	  {
	    geomtools::vector_3d pos (2.* drand48 () * CLHEP::mm, 
				      2.* drand48 () * CLHEP::mm, 
				      0.0);
	    geomtools::vector_3d dir;
	    geomtools::randomize_direction (drand48, dir);
	    dir.setZ (0.0);
	    geomtools::intercept_t intercept;
 	    int intercept_face;
	    if (my_disk.find_intercept (pos, dir, 
					intercept))
	      {
		geomtools::gnuplot_draw::basic_draw_point (cout, 
							   intercept.get_impact ());
	      }
	  }
 	cout << endl << endl;
  
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
  return (error_code);
}

// end of test_disk.cxx

// -*- mode: c++; -*- 
// test_regular_linear_placement.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unistd.h>

#include <geomtools/regular_linear_placement.h>
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/drawer.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      geomtools::vector_3d  basic_position (1., 2., 3.);
      cout << "Basic position : " << basic_position << endl;

      double phi   = -30.0 * CLHEP::degree;
      double theta = 75.0 * CLHEP::degree;
      double delta = 0.0 * CLHEP::degree;
 
      geomtools::placement basic_placement (basic_position, phi, theta, delta);

      geomtools::vector_3d step (+1, +0.5, 0.0);


      geomtools::regular_linear_placement rlp (basic_placement, step, 4);
      rlp.tree_dump (clog, "regular_linear_placement", ">>> ");

      for (int i = 0; i < rlp.get_number_of_items (); i++)
	{
	  ostringstream title;
	  title << "Item #" << i << " placement:";
	  geomtools::placement pi = rlp.i_placement::get_placement (i);
	  pi.tree_dump (clog, title.str (), ">>> ");
	}

      {
	char tmp[256];
	sprintf (tmp, "%s",".tmp_drawer_XXXXXX");
	char * c = mktemp (tmp);
	string tmp_filename = tmp;
	ofstream tmp_file (tmp_filename.c_str ());
	
	geomtools::box b (3.0, 1.0, 0.3);
	for (int i = 0; i < rlp.get_number_of_items (); i++)
	  {
	    geomtools::gnuplot_draw::draw (tmp_file,
					   rlp.i_placement::get_placement (i),
					   b);
	  }
	Gnuplot g1 ("lines");
	g1.set_title ("test_regular_linear_placement");
	g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
	g1.plotfile_xyz (tmp_filename, 1, 2, 3, "3D view");
	g1.showonscreen (); 
	geomtools::drawer::wait_for_key ();
	tmp_file.flush ();
	tmp_file.close ();
	usleep (200);
	unlink (tmp_filename.c_str ());
      }

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

// end of test_regular_linear_placement.cxx

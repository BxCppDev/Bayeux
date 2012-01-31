// -*- mode: c++; -*- 
// test_utils_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/box.h>

#include <datatools/utils/temporary_files.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool draw = false;
      long seed = 314159;
      srand48 (seed);

      using namespace geomtools;

      int rotation_axis = ROTATION_AXIS_INVALID;
      clog << "Enter rotation axis (x, y, z): ";
      string word;
      {
	string s;
	getline (cin, s);
	istringstream iss (s);
	iss >> word;
	if (! iss)
	  {
	    throw runtime_error ("Invalid format for rotation axis!");
	  }
      }
      if (word == "x") rotation_axis = ROTATION_AXIS_X;
      else if (word == "y") rotation_axis = ROTATION_AXIS_Y;
      else if (word == "z") rotation_axis = ROTATION_AXIS_Z;
      else
	{
	  throw runtime_error ("Invalid rotation axis !");
	}

      double rotation_angle = 0.0;
      clog << "Enter rotation angle (degree): ";
      {
	string s;
	getline (cin, s);
	istringstream iss (s);
	iss >> rotation_angle;
	if (! iss)
	  {
	    throw runtime_error ("Invalid format for rotation angle !");
	  }
      }
      rotation_angle *= CLHEP::degree;

      vector_3d pos;
      pos.set (0., 0., 0.);
      clog << "Vector: " << pos << endl;

      rotation rot;
      create_rotation (rot, rotation_axis, rotation_angle);
      tree_dump (rot, clog, "Rotation matrix:");

      box b (8.0, 4.0, 1.5);

      datatools::utils::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", ".test_utils_2_");
 
      gnuplot_draw::draw_box (tmp_file.out (), pos, rot, b);
      tmp_file.close ();
      usleep (200);
 
      if (draw)
	{
	  Gnuplot g1 ("lines");	
	  g1.set_xrange (-10, +10).set_yrange (-10, +10).set_zrange (-10, +10);
	  g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
	  g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
	  g1.showonscreen (); // window output
	  gnuplot_drawer::wait_for_key ();
	  usleep (200);
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

// end of test_utils_2.cxx

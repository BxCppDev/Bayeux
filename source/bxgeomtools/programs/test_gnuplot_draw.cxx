// -*- mode: c++; -*- 
// test_gnuplot_draw.cxx 

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/placement.h>

#include <CLHEP/Random/RanluxEngine.h>
#include <CLHEP/Random/RandFlat.h>

using namespace std;

class test_app
{
protected:
  long        _seed;
  string _filename;
  
public:

  test_app (long seed_, const string & filename_)
  {
    _seed     = seed_;
    _filename = filename_;
  }

  void run ()
  {
    CLHEP::RanluxEngine engine (_seed, 3);
    CLHEP::RandFlat randflat (engine);

    ostream * output_ptr = &cout;
    ofstream foutput;
    if (! _filename.empty ()) 
      {
	cerr << "test_app::run: info: file name='" 
		  << _filename << "'" << endl;
	foutput.open (_filename.c_str ());
	if (! foutput) 
	  {
	    throw runtime_error ("test_app::run: Cannot open file!");
	  }
	output_ptr = &foutput;
      }
    ostream & output = *output_ptr;

    geomtools::vector_3d pos;
    geomtools::rotation rot;
    geomtools::create_rotation (rot, 0.0, 90. * CLHEP::degree, 0.0);

    // Head:
    pos.set (0., 0., 7.0);
    geomtools::gnuplot_draw::draw_circle (output, pos, rot, 1.);

    // Body:
    pos.set (0., 0., 4.5);
    geomtools::gnuplot_draw::draw_rectangle (output, pos, rot, 3., 2.);

    // Legs:
    pos.set (0., -0.75, 1.5);
    geomtools::gnuplot_draw::draw_rectangle (output, pos, rot, 3., 0.5);
    pos.set (0., +0.75, 1.5);
    geomtools::gnuplot_draw::draw_rectangle (output, pos, rot, 3., 0.5);

    // Arms:
    pos.set (0.,-2.0,5.75);
    geomtools::gnuplot_draw::draw_rectangle (output, pos, rot, 0.5, 2.0);
    pos.set (0.,+2.0,5.75);
    geomtools::gnuplot_draw::draw_rectangle (output, pos, rot, 0.5, 2.0);

    output << endl;

    // Holy hat:
    pos.set (0.,0.,9.0);
    geomtools::create_rotation (rot, 0.0, 150. * CLHEP::degree, 0.0);
    geomtools::gnuplot_draw::draw_circle (output, pos, rot, 0.75);

    output << endl;

    // Saint's wand:
    pos.set (0., 0., 0.0);
    geomtools::create_rotation (rot, 0.0, 0. * CLHEP::degree, 0.0);
    geomtools::vector_3d start (0., 2., 0.);
    geomtools::vector_3d stop (0., 3.5, 7.);
    geomtools::gnuplot_draw::draw_segment (output, pos, rot, start, stop);  

    output << endl;

    // Saint's box:
    double x0 = 3.0;
    double y0 = 0.0;
    double z0 = 0.5;
    pos.set (x0, y0, z0);
    geomtools::create_rotation (rot, 0.0, 0. * CLHEP::degree, 0.0);
    geomtools::gnuplot_draw::draw_box (output, pos, rot, 2., 4., 1.);  
    pos.set (x0 + 1.707, y0, 1.707);
    geomtools::create_rotation (rot, 0.0, -45. * CLHEP::degree, 0.0);
    geomtools::gnuplot_draw::draw_rectangle (output, pos, rot, 2., 4.);

    output << endl;

    // Coins:
    int nb_coins = randflat.shootInt (10, 30);
    for (int i = 0; i < nb_coins; i++) 
      {
	pos.set (x0 + randflat.shoot (-0.5, +0.5),
		 y0 + randflat.shoot (-0.5, +0.5),
		 randflat.shoot (2,6));
	geomtools::create_rotation (rot, 
				     360. * CLHEP::degree * randflat.shoot (0.,1.),
				     180. * CLHEP::degree * randflat.shoot (0., 1.),
				     0.0);
	geomtools::gnuplot_draw::draw_cylinder (output, pos, rot, 0.1, 0.02);
      }

    // Holy tube: 
    pos.set (1., -3., 2.0);
    geomtools::create_rotation (rot, 0.0, 10. * CLHEP::degree, 0.0);
    geomtools::gnuplot_draw::draw_tube (output, pos, rot, 1.0, 1.3, 3.0);

    output << endl;

    geomtools::placement placement1 (1.5, 3, 2.5, 
				     30. * CLHEP::degree, 
				     25. * CLHEP::degree,
				     50. * CLHEP::degree);
    geomtools::box box1 (2.0, 3.0, 0.5);
    geomtools::gnuplot_draw::draw (output, placement1, box1);

    geomtools::placement placement2 (+1.5, -4, 4.5, 
				     10. * CLHEP::degree, 
				     65. * CLHEP::degree,
				     0. * CLHEP::degree);

    int mode = geomtools::gnuplot_draw::MODE_NULL;
    mode = geomtools::gnuplot_draw::MODE_WIRED_CYLINDER;
    geomtools::cylinder cyl1 (0.5, 3.0);
    geomtools::gnuplot_draw::draw (output, placement2, cyl1, mode);

  }

};

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      long seed = 12345;
      string filename = "";

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg = argv_[iarg];
	  cerr << "Argument=" << arg << endl;
	  if (arg == "-d" || arg == "--debug") debug = true;
	  if (arg == "-o" || arg == "--output") 
	    {
	      iarg++;
	      if (iarg == argc_) 
		{
		  throw runtime_error ("Missing output filename!");
		}
	      filename = argv_[iarg];
	    }
	  if (arg == "-s" || arg == "--seed") 
	    {
	      iarg++;
	      if (iarg == argc_) 
		{
		  throw runtime_error ("Missing seed value!");
		}
	      istringstream seed_ss (argv_[iarg]);
	      seed_ss >> seed;
	      if (! seed_ss) 
		{
		  throw runtime_error ("Invalid format for seed value!");
		}
	      if (seed <= 0) 
		{
		  throw runtime_error ("Invalid seed value!");
		}
	    }
	  iarg++;
	}
    
      test_app my_app (seed, filename);
      my_app.run ();

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

// end of test_gnuplot_draw.cxx

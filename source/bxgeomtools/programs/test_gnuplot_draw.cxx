// -*- mode: c++; -*- 
// test_gnuplot_draw.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>

#include <CLHEP/Random/RanluxEngine.h>
#include <CLHEP/Random/RandFlat.h>

class test_app
{
protected:
  long        _seed;
  std::string _filename;
  
public:

  test_app (long seed_, const std::string & filename_)
  {
    _seed     = seed_;
    _filename = filename_;
  }

  void 
  run ()
  {
    CLHEP::RanluxEngine engine (_seed, 3);
    CLHEP::RandFlat randflat (engine);

    std::ostream * output_ptr = &std::cout;
    std::ofstream foutput;
    if (! _filename.empty ()) 
      {
	std::cerr << "test_app::run: info: file name='" 
		  << _filename << "'" << std::endl;
	foutput.open (_filename.c_str ());
	if (! foutput) 
	  {
	    throw std::runtime_error ("test_app::run: Cannot open file!");
	  }
	output_ptr = &foutput;
      }
    std::ostream & output = *output_ptr;

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

    output << std::endl;

    // Holy hat:
    pos.set (0.,0.,9.0);
    geomtools::create_rotation (rot, 0.0, 150. * CLHEP::degree, 0.0);
    geomtools::gnuplot_draw::draw_circle (output, pos, rot, 0.75);

    output << std::endl;

    // Saint's wand:
    pos.set (0., 0., 0.0);
    geomtools::create_rotation (rot, 0.0, 0. * CLHEP::degree, 0.0);
    geomtools::vector_3d start (0., 2., 0.);
    geomtools::vector_3d stop (0., 3.5, 7.);
    geomtools::gnuplot_draw::draw_segment (output, pos, rot, start, stop);  

    output << std::endl;

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

    output << std::endl;

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
    geomtools::gnuplot_draw::draw_tube (output, pos, rot, 1.2, 1.3, 3.0);

    output << std::endl;
  }

};

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      long seed = 12345;
      std::string filename = "";

      int iarg = 1;
      while (iarg < argc_) 
	{
	  std::string arg = argv_[iarg];
	  std::cerr << "Argument=" << arg << std::endl;
	  if (arg == "-d" || arg == "--debug") debug = true;
	  if (arg == "-o" || arg == "--output") 
	    {
	      iarg++;
	      if (iarg == argc_) 
		{
		  throw std::runtime_error ("Missing output filename!");
		}
	      filename = argv_[iarg];
	    }
	  if (arg == "-s" || arg == "--seed") 
	    {
	      iarg++;
	      if (iarg == argc_) 
		{
		  throw std::runtime_error ("Missing seed value!");
		}
	      std::istringstream seed_ss (argv_[iarg]);
	      seed_ss >> seed;
	      if (! seed_ss) 
		{
		  throw std::runtime_error ("Invalid format for seed value!");
		}
	      if (seed <= 0) 
		{
		  throw std::runtime_error ("Invalid seed value!");
		}
	    }
	  iarg++;
	}
    
      test_app my_app (seed, filename);
      my_app.run ();

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_gnuplot_draw.cxx

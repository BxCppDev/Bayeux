// -*- mode: c++ ; -*- 
// test_box_vg.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/gnuplot_draw.h>
#include <genvtx/box_vg.h>
#include <mygsl/rng.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'box_vg'!" << endl; 
  
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

      std::string       rng_id = "mt19937";
      unsigned long int rng_seed = 12345;
      mygsl::rng random (rng_id, rng_seed);
         
      geomtools::box b (4., 2., 3.);

      genvtx::box_vg::g_debug = debug;
      genvtx::box_vg vg;
      vg.set_box (b);

      {
	vg.set_mode (genvtx::box_vg::MODE_SURFACE);
	int surface_mask = 0;
	//surface_mask |= geomtools::box::FACE_BACK ;
	surface_mask |= geomtools::box::FACE_FRONT;
	//surface_mask |= geomtools::box::FACE_LEFT;
	//surface_mask |= geomtools::box::FACE_RIGHT;
	//surface_mask |= geomtools::box::FACE_BOTTOM;
	//surface_mask |= geomtools::box::FACE_TOP;
	vg.set_surface_mask (surface_mask);
	vg.set_skin_skip (0.10);
	vg.set_skin_thickness (0.20);
	vg.dump ();
	size_t nshoots = 2000;
	geomtools::vector_3d vertex;     
	for (int i = 0; i < nshoots; i++)
	  {
	    vg.shoot_vertex (random, vertex);
	    geomtools::gnuplot_draw::basic_draw_point (cout, vertex, true);
	  }
	cout << endl << endl;
      }

      {
	vg.reset ();
	vg.set_mode (genvtx::box_vg::MODE_BULK);
	vg.set_skin_thickness (0.30);
	vg.dump ();
	size_t nshoots = 5000;
	geomtools::vector_3d vertex;     
	for (int i = 0; i < nshoots; i++)
	  {
	    vg.shoot_vertex (random, vertex);
	    geomtools::gnuplot_draw::basic_draw_point (cout, vertex, true);
	  }
	cout << endl << endl;
      }

      {
	geomtools::vector_3d pos; 
	geomtools::rotation_3d rot;
	geomtools::gnuplot_draw::draw_box (cout, pos, rot, vg.get_box ());
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

// end of test_box_vg.cxx

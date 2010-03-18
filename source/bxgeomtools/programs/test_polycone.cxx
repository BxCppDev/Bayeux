// -*- mode: c++ ; -*- 
// test_polycone.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/polycone.h>
#include <geomtools/gnuplot_draw.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'polycone'!" << endl; 
  
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
    

      double pmt_radius  = 100. * CLHEP::mm;
      double pmt_front_a = 74. * CLHEP::mm;
      double pmt_back_a  = 80. * CLHEP::mm;
      double pmt_base_radius = 50. * CLHEP::mm;
      double pmt_length  = 240. * CLHEP::mm;
	
      geomtools::polycone my_polycone;
      
      double a = pmt_front_a;
      double b = pmt_radius;
      double dtheta = M_PI / 50.;
      my_polycone.add (a / CLHEP::cm, 0.0 / CLHEP::cm);
      for (double theta = dtheta; 
	   theta < 0.5 * M_PI + 0.1 * dtheta; 
	   theta += dtheta)
	{
	  double z = b / sqrt (pow (tan (theta), 2) + pow (b / a, 2));
	  double r = b * sqrt (1.0 - pow (z / a, 2));
	  my_polycone.add (z / CLHEP::cm, r / CLHEP::cm);
	  
	}
      double a2 = pmt_back_a;
      for (double theta = 0.5 * M_PI; 
	   theta > 0.2 * M_PI; 
	   theta -= dtheta)
	{
	  double z = b / sqrt (pow (tan (theta), 2) + pow (b / a2, 2));
	  double r = b * sqrt (1.0 - pow (z / a2, 2));
	  my_polycone.add (- z / CLHEP::cm, r / CLHEP::cm);
	}
      my_polycone.add (-pmt_front_a / CLHEP::cm, pmt_base_radius / CLHEP::cm);
      my_polycone.add ((pmt_front_a - pmt_length)  / CLHEP::cm, 
		       pmt_base_radius / CLHEP::cm);

      my_polycone.tree_dump (clog, "Polycone");
      clog << "Polycone: " << my_polycone << endl;

      {
	geomtools::vector_3d polycone_pos;
	geomtools::rotation polycone_rot;
	geomtools::create_rotation (polycone_rot, 0.0, 0.0, 0.0);
	geomtools::gnuplot_draw::draw_polycone (cout, 
						polycone_pos, 
						polycone_rot, 
						my_polycone);
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

// end of test_polycone.cxx

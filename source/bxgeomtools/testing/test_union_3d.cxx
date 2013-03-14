// -*- mode: c++; -*- 
// test_union_3d.cxx
// gnuplot macro: ./tests/test_union_3d.gpl

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/box.h>
#include <geomtools/union_3d.h>
#include <geomtools/gnuplot_draw.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      long seed = 314159;

      int iarg = 1;
      while (iarg < argc_) 
        {
          string arg = argv_[iarg];

          if (arg == "-d" || arg == "--debug") debug = true;

          iarg++;
        }
    
      srand48 (seed);

      using namespace geomtools;

      // test 1:
      {
        box b1 (4.0 * CLHEP::mm, 
                4.0 * CLHEP::mm, 
                4.0 * CLHEP::mm);
        placement p1 (vector_3d (0, 0, 0), 
                      0, 0, 0);

        box b2 (2.0 * CLHEP::mm, 
                2.0 * CLHEP::mm, 
                2.0 * CLHEP::mm);
        placement p2 (vector_3d (2, 2, 2), 
                      M_PI / 3., M_PI / 3., M_PI / 6.);

        union_3d u1;
        u1.set_shape1 (b1, p1);
        u1.set_shape2 (b2, p2);
        u1.tree_dump (clog, "Union:");

        char c;
        cin >> c;

        geomtools::vector_3d pos (4.0 * CLHEP::mm, 
                                  3.0 * CLHEP::mm, 
                                  6.0 * CLHEP::mm);
        geomtools::vector_3d dir (-1., -1., -1.);
        geomtools::intercept_t intercept;
        
        if (u1.find_intercept (pos, dir, 
                               intercept))
          {
            clog << "test 1: Intercept face=" << intercept.get_face ()
                      << " at impact=" << intercept.get_impact ()
                      << endl;
          }
        else
          {
            clog << "test 1: No intercept." << endl;
          }

        geomtools::gnuplot_draw::draw_box (cout, 
                                           p1.get_translation (), 
                                           p1.get_rotation (), 
                                           b1.get_x (), 
                                           b1.get_y (), 
                                           b1.get_z ());
        geomtools::gnuplot_draw::draw_box (cout, 
                                           p2.get_translation (), 
                                           p2.get_rotation (), 
                                           b2.get_x (), 
                                           b2.get_y (), 
                                           b2.get_z ());
        cout << endl << endl;

        geomtools::gnuplot_draw::basic_draw_point (cout, pos);
        cout << endl << endl;

        geomtools::gnuplot_draw::basic_draw_point (cout, intercept.get_impact ());
        cout << endl << endl;

        geomtools::gnuplot_draw::basic_draw_point (cout, pos);
        geomtools::gnuplot_draw::basic_draw_point (cout, intercept.get_impact ());
        cout << endl << endl;

        clog << "test 1: End." << endl;

        size_t nshoots = 10000;
        for (int i = 0; i < (int) nshoots; i++)
          {
            if ((i%1000) == 0) clog << "Loop #" << i << endl;
            clog << "DEVEL: Loop #" << i << endl;

            // special debug activation:
            int idevel = -1;
            geomtools::union_3d::g_devel = false;
            idevel = 1817;
            if (i == idevel)
              {
                geomtools::union_3d::g_devel = true;
              }

            double dim = 6. * CLHEP::mm;
            geomtools::vector_3d pos (dim * drand48 (), 
                                      dim * drand48 (), 
                                      dim * drand48 ());
            geomtools::vector_3d dir;
            geomtools::randomize_direction (drand48, dir);
            
            geomtools::intercept_t intercept;
            if (u1.find_intercept (pos, dir, intercept))
              {
                if (debug) clog << "test 1: Intercept face=" 
                                     << intercept.get_face ()
                                     << " at impact=" 
                                     << intercept.get_impact ()
                                     << endl;
                geomtools::gnuplot_draw::basic_draw_point (cout, 
                                                           intercept.get_impact (),
                                                           false);
                int face = intercept.get_face ();
                cout << ' ' << face;
                cout << endl;
              }
            else
              {
                if (debug) clog << "test 1: No intercept." << endl;
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

// end of test_box.cxx

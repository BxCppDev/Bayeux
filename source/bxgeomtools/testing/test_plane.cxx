// -*- mode: c++ ; -*- 
// test_plane.cxx
 
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/plane.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

using namespace std;


void test0 ( bool draw_ )
{
  geomtools::plane p;
  geomtools::vector_3d A (1, 2, 0.5);
  //A.set(0,0,0);
  geomtools::vector_3d pN(0.0, 2.0, 1.0);
  //pN.set(0,1,1);
  p.set (A,pN);

  geomtools::vector_3d B (3, 5, 2.5);
  //B.set(0,6,6);
  //B = A + 0.2 * pN;
  //B = A;
  double dist_pB = p.distance (B);
  std::clog << "Distance from B to p is : " 
            << dist_pB << std::endl;
  geomtools::vector_3d H = p.orthogonal_projection (B);
  geomtools::vector_3d shoot_dir (0.0, 1.2, -0.5);
  geomtools::vector_3d I = p.projection (B, shoot_dir);
  std::clog << "Distance from B to H is : " 
            << (H - B).mag () << std::endl;
  std::clog << "Distance from B to I is : " 
            << (I - B).mag () << std::endl;

  geomtools::intercept_t impact;
  geomtools::vector_3d shoot_dir2 (0.0, -1.2, +0.25);
  if (p.find_intercept (B, shoot_dir2, impact))
    {
      std::clog << "Distance from B to impact is : " 
                << (impact.get_impact () - B).mag () << std::endl;      
    }
  else
    {
      std::clog << "No impact was found !" << std::endl;      
    }

  if (draw_)
    {
      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", "test_plane_");
      p.print_grid (tmp_file.out(), 1.0, 5, 5);
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), A);
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), B);
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), H);
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), I);
      if (impact.is_valid ())
        {
          geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), impact.get_impact ());
        }
      tmp_file.out() << '\n' << '\n';
      geomtools::gnuplot_draw::draw_line (tmp_file.out(), B, H, true);
      geomtools::gnuplot_draw::draw_line (tmp_file.out(), B, I);
      if (impact.is_valid ())
        {
          geomtools::gnuplot_draw::draw_line (tmp_file.out(), B, impact.get_impact ());
        }
      tmp_file.out() << '\n' << '\n';
      tmp_file.close ();
      usleep (200);


      {
        Gnuplot g1 ("lines"); 
        g1.cmd ("set title 'Test geomtools::plane -- test0' ");
        g1.cmd ("set grid");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        g1.cmd ("set xyplane at -10");
        g1.set_xrange (-10, +10).set_yrange (-10, +10).set_zrange (-10, +10);
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Plane p' with lines ,";
        plot_cmd << " '' index 1 notitle with points lt 3 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 2 notitle with lines lt 2 ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        Gnuplot g1 ("lines"); 
        g1.cmd ("set title 'Test geomtools::plane -- test0' ");
        g1.cmd ("set grid");
        g1.cmd ("set size ratio -1");
        g1.set_xlabel ("y").set_ylabel ("z");
        g1.set_xrange (-10, +10).set_yrange (-10, +10).set_zrange (-10, +10);
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << tmp_file.get_filename () << "' index 0 using 2:3 title 'Plane p' with lines ,";
        plot_cmd << " '' index 1 using 2:3 notitle with points lt 3 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 2 using 2:3 notitle with lines lt 2 ";
        g1.cmd (plot_cmd.str ());
        //g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

    } 
  return;
}


int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'geomtools::plane' class." << endl; 
  
      bool debug = false;
      bool verbose = false;
      bool draw = false;

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
               else if ((option == "-v") || (option == "--verbose")) 
                 {
                   verbose = true;
                 }
               else if ((option == "-D") || (option == "--draw")) 
                 {
                   draw = true;
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

      test0 (draw);

      clog << "The end." << endl;
    }
  catch (exception & x)
    { 
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error !" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_geomtools.cxx

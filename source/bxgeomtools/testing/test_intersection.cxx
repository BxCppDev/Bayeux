// test_intersection.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Ourselves:
#include <geomtools/intersection.h>

// This project:
#include <geomtools/gnuplot_draw.h>
#include <geomtools/circle.h>
#include <geomtools/line_3d.h>


int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for 'intersection' utilities !" << endl;

      // bool debug = false;
      bool interactive = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token;
               // if ((option == "-d") || (option == "--debug"))
               //   {
               //     debug = true;
               //   }
               // else
               if ((option == "-i") || (option == "--interactive"))
                 {
                   interactive = true;
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


      using namespace geomtools;

      double b = 0.0;
      double e = 10.0;
      clog << "Enter pedestal 'b' : ";
      clog << " (hint: 2.6225, )" << endl;
      if (interactive) {
        cin >> b;
      } else {
        std::istringstream iss(" 2.6225");
        iss >> b;
      }
      clog << " b == " << b << endl;
      clog << "Enter range 'e' : ";
      clog << " (hint: 1.0, )" << endl;
      if (interactive) {
        cin >> e;
      } else {
        std::istringstream iss("1.0");
        iss >> e;
      }
      clog << " e == " << e << endl;

      vector_2d C (2, 3);
      double R = 4.;

      vector_2d P1ref (0., 5. + b);
      vector_2d P2ref (8., 3. + b);
      vector_2d u = (P2ref - P1ref).unit ();
      vector_2d P1 = P1ref - e * u;
      vector_2d P2 = P2ref + e * u;

      circle my_circle;
      my_circle.set_r (R);
      my_circle.tree_dump (clog, "my_circle");

      line_3d my_line;
      my_line.set_first (P1 - 15. * u);
      my_line.set_last (P2 + 15. * u);
      my_line.tree_dump (clog, "my_line");



      {
        vector_3d p1 = P1;
        vector_3d p2 = P2;
        geomtools::gnuplot_draw::basic_draw_point (cout, p1);
        geomtools::gnuplot_draw::basic_draw_point (cout, p2);
        cout << endl;
        cout << endl;

        geomtools::vector_3d circle_pos = C;
        geomtools::gnuplot_draw::basic_draw_point (cout, circle_pos);
        cout << endl;
        cout << endl;
        geomtools::rotation_3d circle_rot;
        geomtools::create_rotation (circle_rot, 0.0, 0.0, 0.0);
        geomtools::gnuplot_draw::draw_circle (cout,
                                              circle_pos,
                                              circle_rot,
                                              my_circle);
        cout << endl;
        cout << endl;

        geomtools::vector_3d line_pos;
        geomtools::rotation_3d line_rot;
        geomtools::create_rotation (line_rot, 0.0, 0.0, 0.0);
        geomtools::gnuplot_draw::draw_line (cout,
                                            line_pos,
                                            line_rot,
                                            my_line);
        cout << endl;
        cout << endl;

      }

      size_t nsols;
      vector_2d I1, I2;
      intersection::find_intersection_line_circle_2d (P1, u, C, R, nsols, I1, I2);

      {
        vector_3d i1;
        invalidate (i1);
        if (nsols >= 1)
          {
            i1 = I1;
          }
        geomtools::gnuplot_draw::basic_draw_point (cout, i1);

        vector_3d i2;
        invalidate (i2);
        if (nsols == 2)
          {
            i2 = I2;
          }
        geomtools::gnuplot_draw::basic_draw_point (cout, i2);

        cout << endl;
        cout << endl;
      }


      vector_2d S1, S2;
      if (intersection::find_intersection_segment_disk_2d (P1, P2, C, R, S1, S2))
        {
          vector_3d s1;
          invalidate (s1);
          s1 = S1;
          geomtools::gnuplot_draw::basic_draw_point (cout, s1);
          vector_3d s2;
          invalidate (s2);
          s2 = S2;
          geomtools::gnuplot_draw::basic_draw_point (cout, s2);

          cout << endl;
          cout << endl;
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

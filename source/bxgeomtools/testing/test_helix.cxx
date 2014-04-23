// -*- mode: c++; -*-
// test_helix.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/geomtools_config.h>
#include <geomtools/helix_3d.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

void
wait_for_key ()
{
  std::cout << std::endl
            << "Press ENTER to continue..."
            << std::endl;
  std::cin.clear();
  std::cin.ignore(std::cin.rdbuf()->in_avail());
  std::cin.get();
  return;
}


void test0 ()
{
  std::clog << "\n********** test0: Check 'get_length' value \n";
  geomtools::vector_3d O ( 0., 0., 0. );

  geomtools::helix_3d H;
  H.set_radius ( 1. * CLHEP::m  );
  H.set_center ( O );
  H.set_step   ( 1. * CLHEP::m );
  H.set_t1     ( 0.0 );
  H.set_t2     ( 0.1 );

  double l = H.get_length ();

  std::clog << "H length = " << l /CLHEP::mm  << " mm" << std::endl;

  double sum = 0.0;
  double dt =  1.e-4;
  for (double t = H.get_t1 (); t <= H.get_t2 (); t += dt)
    {
      double tp = t + dt;
      geomtools::vector_3d p = H.get_point (t);
      geomtools::vector_3d pp = H.get_point (tp);
      sum += (pp - p).mag ();
    }
  double S = H.get_curvilinear_position (H.get_t2 ())
    - H.get_curvilinear_position (H.get_t1 ());
  std::clog << "H sum-length = " << sum / CLHEP::mm  << " mm" << std::endl;
  std::clog << "H S-length = " << S / CLHEP::mm  << " mm" << std::endl;

  return;
}

int
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      // bool debug   = false;
      bool gnuplot = false;
      bool only_test_0 = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          std::string arg = argv_[iarg];

          // if (arg == "-d" || arg == "--debug")
          //   debug = true;
          if (arg == "-g" || arg == "--gnuplot")
            gnuplot = true;
          if (arg == "-0" || arg == "--only-test-0")
            only_test_0 = true;

          iarg++;
        }

      test0 ();
      if (only_test_0)
        {
          return error_code;
        }

      geomtools::helix_3d my_helix_1;
      geomtools::helix_3d my_helix_2;

      geomtools::vector_3d helix_center_1 ( 0., 0., 0. );

      my_helix_1.set_radius ( 1. * CLHEP::m  );
      my_helix_1.set_center ( helix_center_1 );
      my_helix_1.set_step   ( 10. * CLHEP::mm  );
      my_helix_1.set_t1     ( 0.0 );
      my_helix_1.set_t2     ( 2.0 );

      geomtools::vector_3d helix_center_2 ( 0., 0., 0. );

      my_helix_2.set_radius ( 0.5 * CLHEP::m );
      my_helix_2.set_center ( helix_center_2 );
      my_helix_2.set_step   ( -1. * CLHEP::mm );
      my_helix_2.set_t1     ( 0.0 );
      my_helix_2.set_t2     ( 2.0 );

      my_helix_1.tree_dump  ( std::cerr, "helix #1:" );
      std::cerr << std::endl;

      my_helix_2.tree_dump  ( std::cerr, "helix #2:" );
      std::cerr << std::endl;

      if ( gnuplot )
        {
          std::vector<double> x1, y1, z1;
          for ( double t = my_helix_1.get_t1();
                t <= my_helix_1.get_t2();
                t += ( ( my_helix_1.get_t2() - my_helix_1.get_t1() ) / 1000. ) )
            {
              x1.push_back ( my_helix_1.get_point ( t ).x() );
              y1.push_back ( my_helix_1.get_point ( t ).y() );
              z1.push_back ( my_helix_1.get_point ( t ).z() );
            }

          std::vector<double> x2, y2, z2;
          for ( double t = my_helix_2.get_t1();
                t <= my_helix_2.get_t2();
                t += ( ( my_helix_2.get_t2() - my_helix_2.get_t1() ) / 1000. ) )
            {
              x2.push_back ( my_helix_2.get_point ( t ).x() );
              y2.push_back ( my_helix_2.get_point ( t ).y() );
              z2.push_back ( my_helix_2.get_point ( t ).z() );
            }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
          Gnuplot g1("lines");
          g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
          g1.set_grid();
          g1.plot_xyz ( x1, y1, z1, "Helix1" );
          g1.plot_xyz ( x2, y2, z2, "Helix2" );
          wait_for_key();
#endif
        }
      else
        {
          my_helix_1.print_xyz  ( std::cout, my_helix_1, 0.1, 0 );

          std::cout << std::endl;
          std::cout << std::endl;

          for ( double t = my_helix_1.get_t1();
                t <= my_helix_1.get_t2();
                t += ( ( my_helix_1.get_t2() - my_helix_1.get_t1() ) / 2. ) )
            {
              geomtools::vector_3d position ( my_helix_1.get_point ( t ).x(),
                                              my_helix_1.get_point ( t ).y(),
                                              my_helix_1.get_point ( t ).z() );
              geomtools::vector_3d dir_vec = my_helix_1.get_direction_on_curve (position);

              std::cout << position.x() << " "
                        << position.y() << " "
                        << position.z() << std::endl;

              geomtools::vector_3d end_point = position + dir_vec;

              std::cout << end_point.x() << " "
                        << end_point.y() << " "
                        << end_point.z() << std::endl;

              std::cout << std::endl;
              std::cout << std::endl;
            }

          my_helix_2.print_xyz  ( std::cout, my_helix_2, 0.1, 0 );
        }

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

// end of test_helix.cxx

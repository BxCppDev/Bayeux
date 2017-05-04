// test_elliptical_tube.cxx
//
// bash$ test_elliptical_tube [-D ] [-B] [-i] [-o] [-b] [-t]

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/geomtools_config.h>
#include <geomtools/elliptical_tube.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <datatools/temporary_files.h>
#include <datatools/utils.h>


int
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool interactive = false;
    // bool debug = false;
    bool side   = false;
    bool top    = false;
    bool bottom = false;
    bool bulk   = false;
    bool draw   = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-I" || arg == "--interactive") interactive = true;
      // if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-s" || arg == "--side") side = false;
      if (arg == "-b" || arg == "--bottom") bottom = true;
      if (arg == "-t" || arg == "--top") top = true;
      if (arg == "-B" || arg == "--bulk") bulk = true;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_elliptical_tube_");

    geomtools::elliptical_tube my_elliptical_tube (300 * CLHEP::mm,
						   200 * CLHEP::mm,
						   500 * CLHEP::mm);
    std::clog << "Elliptical_Tube     = " << my_elliptical_tube << " " << std::endl;

    std::clog << "Volume   = " << my_elliptical_tube.get_volume () / CLHEP::m3 << " m3" << std::endl;

    std::clog << "Side surface = "
              << my_elliptical_tube.get_surface (geomtools::elliptical_tube::FACE_SIDE) / CLHEP::m2
              << " m2" << std::endl;

    std::clog << "Top surface = "
              << my_elliptical_tube.get_surface (geomtools::elliptical_tube::FACE_TOP) / CLHEP::m2
              << " m2" << std::endl;

    std::clog << "Bottom surface = " << my_elliptical_tube.get_surface (geomtools::elliptical_tube::FACE_BOTTOM) / CLHEP::m2
              << " m2" << std::endl;

    std::clog << "Full surface = "
              << my_elliptical_tube.get_surface (geomtools::elliptical_tube::FACE_ALL) / CLHEP::m2
              << " m2" << std::endl;

    geomtools::vector_3d elliptical_tube_pos;
    geomtools::rotation elliptical_tube_rot;
    geomtools::create_rotation (elliptical_tube_rot, 0.0, 0.0, 0.0);
    geomtools::gnuplot_draw::draw_elliptical_tube (tmp_file.out(), elliptical_tube_pos, elliptical_tube_rot, my_elliptical_tube);
    tmp_file.out() << std::endl << std::endl;

    size_t nshoots = 500000;
    for (int i = 0; i < (int) nshoots; i++) {
      double dim = 1000. * CLHEP::mm;
      geomtools::vector_3d position (dim * ( -1 + 2 * drand48 ()),
                                     dim * ( -1 + 2 * drand48 ()),
                                     dim * ( -1 + 2 * drand48 ()));
      double skin = 20.0 * CLHEP::mm;
      if (side && my_elliptical_tube.is_on_surface(position,
						   geomtools::elliptical_tube::FACE_SIDE, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 3
                       << std::endl;
      } else if (top && my_elliptical_tube.is_on_surface(position,
							 geomtools::elliptical_tube::FACE_TOP, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 4
                       << std::endl;
      } else if (bottom && my_elliptical_tube.is_on_surface(position,
							    geomtools::elliptical_tube::FACE_BOTTOM, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 5
                       << std::endl;
      } else if (bulk && my_elliptical_tube.is_inside(position, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 2
                       << std::endl;
      }
    }

    if (interactive) {
      std::clog << "Enter a elliptical_tube (example: '{elliptical_tube 900 1000 1000}'): ";
      std::cin >> std::ws >> my_elliptical_tube;
      if (std::cin)
        {
          std::clog << "Elliptical_Tube = " << my_elliptical_tube << " " << std::endl;
        }
      else
        {
          throw std::runtime_error ("Invalid input for elliptical_tube!");
        }
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::elliptical_tube' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      //g1.cmd ("set xyplane at -10");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 notitle with lines ,";
        plot_cmd << " '' index 1 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (2);
      }
#endif
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

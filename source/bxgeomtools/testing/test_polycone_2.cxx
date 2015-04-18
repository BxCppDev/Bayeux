// test_polycone_2.cxx

// Ourselves:
#include <geomtools/polycone.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'polycone'!" << std::endl;

    std::string datafile;
    double zmin = std::numeric_limits<double>::quiet_NaN ();
    double zmax = std::numeric_limits<double>::quiet_NaN ();
    bool draw          = false; // Interactive plot

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw")      draw = true;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_polycone_2_");

    geomtools::polycone PC1;
    if (datafile.empty()) {
      datafile = "${GEOMTOOLS_TESTING_DIR}/data/test_hamamatsu_R5912MOD_1.data";
    }
    datatools::fetch_path_with_env(datafile);
    zmin = -100 * CLHEP::mm;
    //zmin = -54 * CLHEP::mm;
    PC1.initialize (datafile, zmin, zmax);
    std::clog << "Polycone: " << PC1 << std::endl;

    geomtools::polycone PC2;
    zmax = zmin;
    datatools::invalidate(zmin);
    PC2.initialize (datafile, zmin, zmax);
    std::clog << "Polycone: " << PC2 << std::endl;

    {
      tmp_file.out() << "# Draw solid (index 0) " << std::endl;
      geomtools::gnuplot_draw::draw_polycone(tmp_file.out(), PC1);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      tmp_file.out() << "# Draw solid (index 1) " << std::endl;
      geomtools::gnuplot_draw::draw_polycone(tmp_file.out(), PC2);
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1

      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::" + PC1.get_shape_name() + "' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd ("set view equal xyz");
      // g1.cmd("set xrange [-4:+4]");
      // g1.cmd("set yrange [-4:+4]");
      // g1.cmd("set zrange [-4:+4]");
      // g1.cmd("set xyplane at -4");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Solid' with lines ";
        // plot_cmd << ", '' index 1 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(2);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 1 title 'Solid' with lines ";
        // plot_cmd << ", '' index 1 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(2);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);

}

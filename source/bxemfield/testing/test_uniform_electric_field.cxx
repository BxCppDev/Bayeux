// test_uniform_electric_field.cxx

// Ourselves:
#include <emfield/uniform_electric_field.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
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
    std::clog << "Test program for the 'emfield::uniform_electric_field' class." << std::endl;

    bool draw = false;

    {
      int iarg = 1;
      while (iarg < argc_) {
        std::string arg = argv_[iarg];
        if (arg == "-D" || arg == "--draw") draw = true;
        iarg++;
      }
    }

    emfield::base_electromagnetic_field::handle_type uef_h(new emfield::uniform_electric_field);
    emfield::uniform_electric_field & uef
      = dynamic_cast<emfield::uniform_electric_field &>(uef_h.grab());
    geomtools::vector_3d E0(0.0, 0.0, 2.0 * CLHEP::volt/CLHEP::mm);
    uef.set_uniform_electric_field(E0);
    uef.initialize_simple();

    // emfield::uniform_electric_field oef;
    // oef.set_sine_cosine_mode(emfield::uniform_electric_field::SC_MODE_COSINE);
    // oef.set_frequency(1.0 * CLHEP::megahertz);
    // oef.set_phase(20.0 * CLHEP::degree);
    // oef.set_scale(-1.0);
    // oef.set_pedestal(1.0);
    // oef.set_field(uef_h);
    // oef.initialize_simple();

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_emfield_uniform_electric_field_");

    double time = 0.0 * CLHEP::nanosecond;
    geomtools::vector_3d position;
    geomtools::vector_3d E;
    for (double x = -3 * CLHEP::mm; x <= +3 * CLHEP::mm; x += 0.5 * CLHEP::mm) {
      for (double y = -3 * CLHEP::mm; y <= +3 * CLHEP::mm; y += 0.5 * CLHEP::mm) {
        position.set(x, y, 0.0);
        uef.compute_electric_field(position, time, E);
        tmp_file.out() << position.x() / CLHEP::mm << ' '
                       << position.y() / CLHEP::mm << ' '
                       << position.z() / CLHEP::mm << ' '
                       << E.x() / (CLHEP::volt/CLHEP::mm) << ' '
                       << E.y() / (CLHEP::volt/CLHEP::mm) << ' '
                       << E.z() / (CLHEP::volt/CLHEP::mm) << ' '
                       << std::endl;
      }
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test emfield::uniform_electric_field' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-4:+4]");
      g1.cmd("set yrange [-4:+4]");
      g1.cmd("set zrange [-4:+4]");
      g1.set_xlabel("x (mm)").set_ylabel("y (mm)").set_zlabel("E (V/mm)");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' u 1:2:3:4:5:6 notitle 'Uniform electric field' with vectors";
        g1.cmd (plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
    }

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

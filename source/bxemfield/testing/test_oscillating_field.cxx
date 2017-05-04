// test_oscillating_field.cxx

// Ourselves:
#include <emfield/oscillating_field.h>

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

// This project:
#include <emfield/uniform_electric_field.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'emfield::oscillating_field' class." << std::endl;

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
    geomtools::vector_3d E0(0.0, 0.0, 3.0 * CLHEP::volt/CLHEP::mm);
    uef.set_uniform_electric_field(E0);
    uef.initialize_simple();

    emfield::oscillating_field oef;
    oef.set_sine_cosine_mode(emfield::oscillating_field::SC_MODE_COSINE);
    oef.set_frequency(1.0 * CLHEP::megahertz);
    oef.set_phase(20.0 * CLHEP::degree);
    oef.set_scale(-1.0);
    oef.set_pedestal(1.0);
    oef.set_field(uef_h);
    oef.initialize_simple();

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_emfield_oscillating_field_");

    geomtools::vector_3d position;
    for (double time = 0.0; time < 2 * CLHEP::microsecond; time += 10 * CLHEP::nanosecond) {
      geomtools::vector_3d E;
      oef.compute_electric_field(position, time, E);
      tmp_file.out() << time / CLHEP::nanosecond << ' ';
      tmp_file.out() << E.z() / (CLHEP::volt/CLHEP::mm);
      tmp_file.out() << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test emfield::oscillating_field' ");
      g1.cmd("set grid");
      g1.cmd("set xrange [0:2000]");
      g1.cmd("set yrange [-10:+10]");
      g1.set_xlabel("t (ns)").set_ylabel("E (V/mm)");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << tmp_file.get_filename() << "' u 1:2 notitle 'Box' with lines";
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

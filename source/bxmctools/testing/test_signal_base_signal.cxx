// test_signal_base_signal.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
// - Bayeux/mygsl:
#include <mygsl/parameter_store.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This project:
#include <mctools/signal/base_signal.h>

void test_base_signal_1(bool draw_ = false);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::signal::base_signal'!" << std::endl;

    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw") draw = true;
      iarg++;
    }

    test_base_signal_1(draw);

    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void test_base_signal_1(bool draw_)
{
  static const double nVs = CLHEP::nanosecond * CLHEP::volt;
  static const double mV  = 1e-3 * CLHEP::volt;

  std::clog << "nanosecond  = " << CLHEP::nanosecond << std::endl;
  std::clog << "Volt        = " << CLHEP::volt << std::endl;
  std::clog << "millivolt   = " << mV << std::endl;
  std::clog << "nV.s        = " << nVs << std::endl;

  {
    mctools::signal::base_signal hitSig;
    hitSig.set_hit_id(12);
    geomtools::geom_id hitGid(1000, 1, 2);
    hitSig.set_geom_id(hitGid);
    hitSig.grab_auxiliaries().store("test", 42);
    hitSig.set_shape_type_id("mctools::signal::triangle_signal_shape");
    hitSig.set_shape_string_parameter("polarity", "-");
    hitSig.set_shape_real_parameter_with_explicit_unit("rise_time",
                                                       8.0 * CLHEP::nanosecond,
                                                       "ns");
    hitSig.set_shape_real_parameter_with_explicit_unit("fall_time",
                                                       16.0 * CLHEP::nanosecond,
                                                       "ns");
    hitSig.set_shape_real_parameter_with_explicit_unit("t0",
                                                       10.0 * CLHEP::nanosecond,
                                                       "ns");
    hitSig.set_shape_real_parameter_with_explicit_unit("q",
                                                       60.0 * nVs,
                                                       "nV.s");
    hitSig.initialize_simple();
    hitSig.tree_dump(std::clog, "Hit signal: ");

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_signal_base_signal_");

    tmp_file.out() << "#signal_1:\n";
    hitSig.grab_shape().write_ascii_file_with_units(tmp_file.get_filename(),
                                                    -10.0 * CLHEP::nanosecond,
                                                    +60.0 * CLHEP::nanosecond,
                                                    3000,
                                                    "ns",
                                                    "volt",
                                                    16, 16,
                                                    mygsl::i_unary_function::wo_append
                                                    );

    if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test mygsl::signal::base_signal' ");
      g1.cmd("set key out");
      g1.cmd("set grid");
      g1.cmd("set xrange [0:100]");
      {
        std::ostringstream cmd1;
        cmd1 << "volt=" << CLHEP::volt;
        g1.cmd(cmd1.str());
        std::ostringstream cmd2;
        cmd2 << "nanosecond=" << CLHEP::nanosecond;
        g1.cmd(cmd2.str());
        // std::ostringstream cmd3;
        // cmd3 << "nVs=nanosecond*volt";
        // g1.cmd(cmd3.str());
      }
      g1.cmd("set yrange [-5:+0.5]");
      g1.set_xlabel("time (ns)").set_ylabel("Signal (V)");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "plot '" << tmp_file.get_filename() << "' "
                 << " index 0 using (column(1)"
                 << "):(column(2)"<< ')'
                 << " title 'Signal shape 1' with lines lw 4";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

    {
      // Serialization:
      datatools::data_writer writer("test_signal_base_signal.xml",
                                    datatools::using_multi_archives);
      writer.store(hitSig);
    }

  }

  {
    mctools::signal::base_signal hitSig;

    {
      // Deserialization:
      datatools::data_reader reader("test_signal_base_signal.xml",
                                    datatools::using_multi_archives);
      reader.load(hitSig);
    }

    hitSig.tree_dump(std::clog, "Hit signal: ");
    for (double t = 0.0 * CLHEP::ns;
         t <= 40.0 * CLHEP::ns;
         t += 1.00 * CLHEP::ns) {
      std::cout << t / CLHEP::ns << ' '
                << hitSig.grab_shape().eval(t) / CLHEP::volt << std::endl;
    }
  }

  return;
}

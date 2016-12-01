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
    hitSig.set_logging(datatools::logger::PRIO_DEBUG);
    hitSig.set_hit_id(12);
    geomtools::geom_id hitGid(1000, 1, 2);
    hitSig.set_geom_id(hitGid);
    hitSig.grab_auxiliaries().store("test", 42);
    hitSig.set_shape_type_id("mctools::signal::triangle_signal_shape");
    hitSig.set_shape_string_parameter("polarity", "-");
    hitSig.set_shape_real_parameter_with_explicit_unit("t0",
                                                       10.0 * CLHEP::nanosecond,
                                                       "ns");
    hitSig.set_shape_real_parameter_with_explicit_unit("t1",
                                                       16.0 * CLHEP::nanosecond,
                                                       "ns");
    hitSig.set_shape_real_parameter_with_explicit_unit("t2",
                                                       28.0 * CLHEP::nanosecond,
                                                       "ns");
    hitSig.set_shape_real_parameter_with_explicit_unit("amplitude",
                                                       600.0 * mV,
                                                       "mV");
    hitSig.initialize_simple();
    hitSig.tree_dump(std::clog, "Hit signal: ");

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    // tmp_file.set_remove_at_destroy(false);
    tmp_file.create("/tmp", "test_signal_base_signal_");

    tmp_file.out() << "#signal_1:\n";
    hitSig.get_shape().write_ascii_with_units(tmp_file.out(),
                                              -10.0 * CLHEP::nanosecond,
                                              +50.0 * CLHEP::nanosecond,
                                              600,
                                              CLHEP::ns,
                                              CLHEP::volt,
                                              16, 16
                                              );
    tmp_file.out() << "\n\n";
    hitSig.tree_dump(std::clog, "Hit signal (with instantiated shape): ");

    mctools::signal::base_signal hitSig2(hitSig);
    hitSig2.set_hit_id(13);
    geomtools::geom_id hitGid2(1000, 1, 4);
    hitSig2.set_geom_id(hitGid2);
    if (!hitSig2.is_shape_instantiated()) {
      hitSig2.unlock();
      hitSig2.tree_dump(std::clog, "Hit signal 2 (unlock): ");
      hitSig2.set_shape_real_parameter_with_explicit_unit("amplitude",
                                                          400.0 * mV,
                                                          "mV");
      hitSig2.set_shape_real_parameter_with_explicit_unit("t0",
                                                          12.0 * CLHEP::nanosecond,
                                                          "ns");
      hitSig2.set_shape_real_parameter_with_explicit_unit("t2",
                                                          35.0 * CLHEP::nanosecond,
                                                          "ns");
      hitSig2.relock();
    }
    hitSig2.tree_dump(std::clog, "Hit signal 2: ");
    tmp_file.out() << "#signal_2:\n";
    hitSig2.get_shape().write_ascii_with_units(tmp_file.out(),
                                               -10.0 * CLHEP::nanosecond,
                                               +50.0 * CLHEP::nanosecond,
                                               600,
                                               CLHEP::ns,
                                               CLHEP::volt,
                                               16, 16
                                               );
    hitSig2.tree_dump(std::clog, "Hit signal 2 (with instantiated shape): ");
    tmp_file.out() << "\n\n" << std::flush;


    if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test mygsl::signal::base_signal' ");
      g1.cmd("set key out");
      g1.cmd("set grid");
      g1.cmd("set xrange [-20:100]");
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
      g1.cmd("set yrange [-1:+0.1]");
      g1.set_xlabel("time (ns)").set_ylabel("Signal (V)");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "plot "
                 << "  '" << tmp_file.get_filename() << "' "
                 << " index 0 using (column(1)"
                 << "):(column(2)"<< ')'
                 << " title 'Signal shape 1' with lines lw 6"
                 << ", '" << tmp_file.get_filename() << "' "
                 << " index 1 using (column(1)"
                 << "):(column(2)"<< ')'
                 << " title 'Signal shape 2' with lines lw 2"
          ;
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
                << hitSig.get_shape().eval(t) / CLHEP::volt << std::endl;
    }
  }

  return;
}

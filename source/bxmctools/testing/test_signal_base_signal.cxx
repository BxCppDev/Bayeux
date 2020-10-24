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
#include <mctools/signal/signal_shape_builder.h>

void test_base_signal_1(bool draw_ = false);

void test_base_signal_2(bool draw_ = false);

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

    test_base_signal_2(draw);

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
  std::clog << "\ntest_base_signal_1: Entering...\n";

  static const double nVs = CLHEP::nanosecond * CLHEP::volt;
  static const double mV  = 1e-3 * CLHEP::volt;

  std::clog << "nanosecond  = " << CLHEP::nanosecond << std::endl;
  std::clog << "Volt        = " << CLHEP::volt << std::endl;
  std::clog << "millivolt   = " << mV << std::endl;
  std::clog << "nV.s        = " << nVs << std::endl;

  {
    mctools::signal::base_signal hitSig;
    hitSig.set_allow_no_shape_builder(true);
    hitSig.set_logging(datatools::logger::PRIO_DEBUG);
    hitSig.set_hit_id(12);
    geomtools::geom_id hitGid(1000, 1, 2);
    hitSig.set_geom_id(hitGid);
    hitSig.set_category("calo");
    hitSig.set_time_ref(0.3 * CLHEP::ns);
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
    hitSig2.set_category("calo");
    hitSig2.set_time_ref(0.2 * CLHEP::ns);
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

void test_base_signal_2(bool draw_)
{
  std::clog << "\ntest_base_signal_2: Entering...\n";
  static const double nVs = CLHEP::nanosecond * CLHEP::volt;
  static const double mV  = 1e-3 * CLHEP::volt;
  std::clog << "nanosecond  = " << CLHEP::nanosecond << std::endl;
  std::clog << "Volt        = " << CLHEP::volt << std::endl;
  std::clog << "millivolt   = " << mV << std::endl;
  std::clog << "nV.s        = " << nVs << std::endl;

  mctools::signal::signal_shape_builder calo_ssb;
  std::string calo_ssb_config_file = "${MCTOOLS_TESTING_DIR}/config/signal/calo_ssb.conf";
  datatools::fetch_path_with_env(calo_ssb_config_file);
  datatools::properties calo_ssb_config;
  datatools::read_config(calo_ssb_config_file, calo_ssb_config);
  calo_ssb.initialize(calo_ssb_config);

  {
    mctools::signal::base_signal hitSig;
    hitSig.set_logging(datatools::logger::PRIO_DEBUG);
    hitSig.set_hit_id(12);
    geomtools::geom_id hitGid(1000, 1, 2);
    hitSig.set_geom_id(hitGid);
    hitSig.set_category("calo");
    hitSig.set_time_ref(0.2 * CLHEP::ns);
    hitSig.grab_auxiliaries().store("test", 42);

    // Private shape parameters:

    // Shape associated to hit #0:
    datatools::properties s0_params("Hit #0 signal shape parameters");
    s0_params.store_string("polarity", "-");
    s0_params.store_real_with_explicit_unit("t0",
                                            0.0 * CLHEP::nanosecond,
                                            "ns");
    s0_params.store_real_with_explicit_unit("t1",
                                            6.0 * CLHEP::nanosecond,
                                            "ns");
    s0_params.store_real_with_explicit_unit("t2",
                                            18.0 * CLHEP::nanosecond,
                                            "ns");
    s0_params.store_real_with_explicit_unit("amplitude",
                                            300.0 * mV,
                                            "mV");
    std::string s0_key = hitSig.add_auto_private_shape("mctools::signal::triangle_signal_shape", s0_params);

    // Shape associated to hit #1:
    datatools::properties s1_params("Hit #1 signal shape parameters");
    s1_params.store_string("polarity", "-");
    s1_params.store_real_with_explicit_unit("t0",
                                            0.0 * CLHEP::nanosecond,
                                            "ns");
    s1_params.store_real_with_explicit_unit("t1",
                                            5.0 * CLHEP::nanosecond,
                                            "ns");
    s1_params.store_real_with_explicit_unit("t2",
                                            12.0 * CLHEP::nanosecond,
                                            "ns");
    s1_params.store_real_with_explicit_unit("t3",
                                            17.0 * CLHEP::nanosecond,
                                            "ns");
    s1_params.store_real_with_explicit_unit("amplitude",
                                            300.0 * mV,
                                            "mV");
    std::string s1_key = hitSig.add_auto_private_shape("mctools::signal::triangle_gate_signal_shape", s1_params);

    // The signal shape is a combinaison of both private shapes above:
    hitSig.set_shape_type_id("mctools::signal::multi_signal_shape");
    datatools::properties shape_params("Signal shape parameters");
    std::vector<std::string> comp_labels({"hit0","hit1"});
    shape_params.store("components", comp_labels);

    shape_params.store("components.hit0.key", s0_key);
    shape_params.store_real_with_explicit_unit("components.hit0.time_shift", 0.0 * CLHEP::ns);
    shape_params.set_unit_symbol("components.hit0.time_shift", "ns");
    shape_params.store_real("components.hit0.scaling", 0.5);

    shape_params.store("components.hit1.key", s1_key);
    shape_params.store_real_with_explicit_unit("components.hit1.time_shift", 1.0 * CLHEP::ns);
    shape_params.set_unit_symbol("components.hit1.time_shift", "ns");
    shape_params.store_real("components.hit1.scaling", 1.5);

    hitSig.set_shape_parameters(shape_params);
    hitSig.initialize_simple();
    hitSig.set_shape_builder(calo_ssb);
    hitSig.tree_dump(std::clog, "Hit signal: ");

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_signal_base_signal_");

    tmp_file.out() << "#signal:\n";
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
      }
      g1.cmd("set yrange [-1:+0.1]");
      g1.set_xlabel("time (ns)").set_ylabel("Signal (V)");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "plot "
                 << "  '" << tmp_file.get_filename() << "' "
                 << " index 0 using (column(1)"
                 << "):(column(2)"<< ')'
                 << " title 'Signal shape' with lines lw 6"
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
      std::clog << "Serialization..." << std::endl;
      datatools::data_writer writer("test_signal_base_signal2.xml",
                                    datatools::using_multi_archives);
      writer.store(hitSig);
    }

  }

  calo_ssb.clear_functors();

  {
    mctools::signal::base_signal hitSig;
    hitSig.set_logging(datatools::logger::PRIO_DEBUG);
    {
      // Deserialization:
      std::clog << "Deserialization..." << std::endl;
      datatools::data_reader reader("test_signal_base_signal2.xml",
                                    datatools::using_multi_archives);
      reader.load(hitSig);
    }

    // Connect a shape builder to use the shape functor:
    hitSig.set_shape_builder(calo_ssb);
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

// test_signal_shape_builder.cxx

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
#include <mctools/signal/signal_shape_builder.h>

void test_ssb_1(bool draw_ = false);

void test_ssb_2(bool draw_ = false);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::signal::signal_shape_builder'!" << std::endl;

    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];
      if (arg == "-D" || arg == "--draw") draw = true;
      iarg++;
    }

    test_ssb_1(draw);
    test_ssb_2(draw);

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

void test_ssb_1(bool draw_)
{
  std::clog << "\ntest_ssb_1: Entering...\n";
  static const double mV  = 1e-3 * CLHEP::volt;

  mctools::signal::signal_shape_builder ssb;
  ssb.set_logging_priority(datatools::logger::PRIO_DEBUG);
  ssb.set_category("calo");
  ssb.add_registered_shape_type_id("mctools::signal::triangle_signal_shape");
  ssb.add_registered_shape_type_id("mctools::signal::triangle_gate_signal_shape");
  ssb.add_registered_shape_type_id("mygsl::zero_function");
  ssb.add_registered_shape_type_id("mygsl::gaussian_function");
  ssb.add_registered_shape_type_id("mygsl::linear_combination_function");
  ssb.add_registered_shape_type_id("mctools::signal::multi_signal_shape");
  ssb.initialize_simple();

  datatools::properties s1_cfg;
  s1_cfg.store_string("polarity", "-");
  s1_cfg.store_real_with_explicit_unit("t0",
                                       10.0 * CLHEP::nanosecond,
                                       "ns");
  s1_cfg.store_real_with_explicit_unit("t1",
                                       16.0 * CLHEP::nanosecond,
                                       "ns");
  s1_cfg.store_real_with_explicit_unit("t2",
                                       28.0 * CLHEP::nanosecond,
                                       "ns");
  s1_cfg.store_real_with_explicit_unit("amplitude",
                                       600.0 * mV,
                                       "mV");

  datatools::properties s2_cfg;
  s2_cfg.store_string("polarity", "-");
  s2_cfg.store_real_with_explicit_unit("t0",
                                       10.0 * CLHEP::nanosecond,
                                       "ns");
  s2_cfg.store_real_with_explicit_unit("t1",
                                       16.0 * CLHEP::nanosecond,
                                       "ns");
  s2_cfg.store_real_with_explicit_unit("t2",
                                       28.0 * CLHEP::nanosecond,
                                       "ns");
  s2_cfg.store_real_with_explicit_unit("t3",
                                       35.0 * CLHEP::nanosecond,
                                       "ns");
  s2_cfg.store_real_with_explicit_unit("amplitude",
                                       450.0 * mV,
                                       "mV");

  datatools::properties s3_cfg;

  datatools::properties s4_cfg;
  s4_cfg.store_real_with_explicit_unit("mu",
                                       16.0 * CLHEP::nanosecond,
                                       "ns");
  s4_cfg.store_real_with_explicit_unit("sigma",
                                       3.0 * CLHEP::nanosecond,
                                       "ns");
  s4_cfg.store_real_with_explicit_unit("amplitude",
                                       -350.0 * mV,
                                       "mV");

  datatools::properties s5_cfg;
  std::vector<std::string> comp_labels({"hit0","hit1","hit2"});
  s5_cfg.store("components", comp_labels);

  s5_cfg.store("components.hit0.key", "s1");
  s5_cfg.store_real_with_explicit_unit("components.hit0.time_shift", 3.0 * CLHEP::ns);
  s5_cfg.set_unit_symbol("components.hit0.time_shift", "ns");
  s5_cfg.store_real("components.hit0.scaling", 0.5);

  s5_cfg.store("components.hit1.key", "s2");
  s5_cfg.store_real_with_explicit_unit("components.hit1.time_shift", -2.0 * CLHEP::ns);
  s5_cfg.set_unit_symbol("components.hit1.time_shift", "ns");
  s5_cfg.store_real("components.hit1.scaling", 1.0);

  s5_cfg.store("components.hit2.key", "s4");
  s5_cfg.store_real_with_explicit_unit("components.hit2.time_shift", 1.5 * CLHEP::ns);
  s5_cfg.set_unit_symbol("components.hit2.time_shift", "ns");
  s5_cfg.store_real("components.hit2.scaling", 1.3);

  ssb.create_signal_shape("s1",    "mctools::signal::triangle_signal_shape", s1_cfg);
  ssb.create_signal_shape("s2",    "mctools::signal::triangle_gate_signal_shape", s2_cfg);
  ssb.create_signal_shape("s3",    "mygsl::zero_function", s3_cfg);
  ssb.create_signal_shape("s3bis", "mygsl::zero_function", s3_cfg);
  ssb.create_signal_shape("s3ter", "mygsl::zero_function", s3_cfg);
  ssb.create_signal_shape("s4",    "mygsl::gaussian_function", s4_cfg);
  ssb.create_signal_shape("s5",    "mctools::signal::multi_signal_shape", s5_cfg);
  ssb.tree_dump(std::clog, "Signal shape builder:");

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_signal_shape_buider_");

  std::set<std::string> fkeys;
  ssb.build_list_of_functors(fkeys);
  for (const auto & fkey : fkeys) {
    tmp_file.out() << "#" << fkey << ":\n";
    ssb.get_functor(fkey).write_ascii_with_units(tmp_file.out(),
                                                 -10.0 * CLHEP::nanosecond,
                                                 +50.0 * CLHEP::nanosecond,
                                                 600,
                                                 CLHEP::ns,
                                                 CLHEP::volt,
                                                 16, 16
                                                 );
    tmp_file.out() << "\n\n" << std::flush;
  }

    if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test mygsl::signal::signal_shape_builder' ");
      g1.cmd("set key out");
      g1.cmd("set grid");
      g1.cmd("set xrange [-10:50]");
      {
        std::ostringstream cmd1;
        cmd1 << "volt=" << CLHEP::volt;
        g1.cmd(cmd1.str());
        std::ostringstream cmd2;
        cmd2 << "nanosecond=" << CLHEP::nanosecond;
        g1.cmd(cmd2.str());
      }
      g1.cmd("set yrange [-1.5:+0.5]");
      g1.set_xlabel("time (ns)").set_ylabel("Signal (V)");

      {
        std::size_t fcount = 0;
        std::ostringstream plot_cmd;
        plot_cmd << "plot ";
        for (const auto & fkey : fkeys) {
          if (fcount > 0) {
            plot_cmd << ',';
          }
          plot_cmd << "  '" << tmp_file.get_filename() << "' "
                   << " index " << fcount << " using (column(1)"
                   << "):(column(2)"<< ')'
                   << " title 'Signal shape " << fkey << "' with lines lw 3"
            ;
          fcount++;
        }
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

  return;
}

void test_ssb_2(bool draw_)
{
  std::clog << "\ntest_ssb_2: Entering...\n";
  static const double mV  = 1e-3 * CLHEP::volt;

  mctools::signal::signal_shape_builder calo_ssb;
  std::string calo_ssb_config_file = "${MCTOOLS_TESTING_DIR}/config/signal/calo_ssb.conf";
  datatools::fetch_path_with_env(calo_ssb_config_file);
  datatools::properties calo_ssb_config;
  datatools::read_config(calo_ssb_config_file, calo_ssb_config);
  // calo_ssb.add_registered_shape_type_id("mctools::signal::triangle_signal_shape");
  calo_ssb.initialize(calo_ssb_config);

  /*
  ssb.set_logging_priority(datatools::logger::PRIO_DEBUG);
  ssb.set_category("calo2");
  std::clog << "\ntest_ssb_2: Loading reference functors...\n";
  ssb.load_reference_functors_from_file("${MCTOOLS_TESTING_DIR}/config/signal/calo_ref_functors.def");
  std::clog << "\ntest_ssb_2: Done.\n";
  ssb.add_registered_shape_type_id("mctools::signal::triangle_signal_shape");
  ssb.add_registered_shape_type_id("mctools::signal::triangle_gate_signal_shape");
  ssb.add_registered_shape_type_id("mygsl::gaussian_function");
  ssb.add_registered_shape_type_id("mctools::signal::multi_signal_shape");
  ssb.initialize_simple();
  */
  calo_ssb.tree_dump(std::clog, "Calo SSB: ");

  datatools::properties s1_cfg;
  s1_cfg.store_string("polarity", "-");
  s1_cfg.store_real_with_explicit_unit("t0",
                                       10.0 * CLHEP::nanosecond,
                                       "ns");
  s1_cfg.store_real_with_explicit_unit("t1",
                                       16.0 * CLHEP::nanosecond,
                                       "ns");
  s1_cfg.store_real_with_explicit_unit("t2",
                                       28.0 * CLHEP::nanosecond,
                                       "ns");
  s1_cfg.store_real_with_explicit_unit("amplitude",
                                       600.0 * mV,
                                       "mV");
  calo_ssb.create_signal_shape("s1", "mctools::signal::triangle_signal_shape", s1_cfg);

  datatools::properties s2_cfg;
  s2_cfg.store_string("polarity", "-");
  s2_cfg.store_real_with_explicit_unit("t0",
                                       15.0 * CLHEP::nanosecond,
                                       "ns");
  s2_cfg.store_real_with_explicit_unit("t1",
                                       21.0 * CLHEP::nanosecond,
                                       "ns");
  s2_cfg.store_real_with_explicit_unit("t2",
                                       33.0 * CLHEP::nanosecond,
                                       "ns");
  s2_cfg.store_real_with_explicit_unit("t3",
                                       40.0 * CLHEP::nanosecond,
                                       "ns");
  s2_cfg.store_real_with_explicit_unit("amplitude",
                                       450.0 * mV,
                                       "mV");
  calo_ssb.create_signal_shape("s2", "mctools::signal::triangle_gate_signal_shape", s2_cfg);

  datatools::properties s5_cfg;
  std::vector<std::string> comp_labels({"hit0","hit1","hit2"});
  s5_cfg.store("components", comp_labels);
  s5_cfg.store("components.hit0.key", "s1");
  s5_cfg.store_real_with_explicit_unit("components.hit0.time_shift", 3.0 * CLHEP::ns);
  s5_cfg.set_unit_symbol("components.hit0.time_shift", "ns");
  s5_cfg.store_real("components.hit0.scaling", 0.5);
  s5_cfg.store("components.hit1.key", "s2");
  s5_cfg.store_real_with_explicit_unit("components.hit1.time_shift", -2.0 * CLHEP::ns);
  s5_cfg.set_unit_symbol("components.hit1.time_shift", "ns");
  s5_cfg.store_real("components.hit1.scaling", 1.0);
  s5_cfg.store("components.hit2.key", "@ref:single_photo_electron_surface");
  s5_cfg.store_real_with_explicit_unit("components.hit2.time_shift", 1.5 * CLHEP::ns);
  s5_cfg.set_unit_symbol("components.hit2.time_shift", "ns");
  s5_cfg.store_real("components.hit2.scaling", 1.3);
  calo_ssb.create_signal_shape("s5", "mctools::signal::multi_signal_shape", s5_cfg);

  calo_ssb.tree_dump(std::clog, "Calo SSB: ");

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_signal_shape_buider_2_");

  std::set<std::string> fkeys;
  calo_ssb.build_list_of_functors(fkeys);
  for (const auto & fkey : fkeys) {
    tmp_file.out() << "#" << fkey << ":\n";
    calo_ssb.get_functor(fkey).write_ascii_with_units(tmp_file.out(),
                                                      -10.0 * CLHEP::nanosecond,
                                                      +50.0 * CLHEP::nanosecond,
                                                      600,
                                                      CLHEP::ns,
                                                      CLHEP::volt,
                                                      16, 16
                                                      );
    tmp_file.out() << "\n\n" << std::flush;
  }

  if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    Gnuplot g1;
    g1.cmd("set title 'Test mygsl::signal::signal_shape_builder' ");
    g1.cmd("set key out");
    g1.cmd("set grid");
    g1.cmd("set xrange [-10:50]");
    {
      std::ostringstream cmd1;
      cmd1 << "volt=" << CLHEP::volt;
      g1.cmd(cmd1.str());
      std::ostringstream cmd2;
      cmd2 << "nanosecond=" << CLHEP::nanosecond;
      g1.cmd(cmd2.str());
    }
    g1.cmd("set yrange [-1.5:+0.5]");
    g1.set_xlabel("time (ns)").set_ylabel("Signal (V)");

    {
      std::size_t fcount = 0;
      std::ostringstream plot_cmd;
      plot_cmd << "plot ";
      for (const auto & fkey : fkeys) {
        if (fcount > 0) {
          plot_cmd << ',';
        }
        plot_cmd << "  '" << tmp_file.get_filename() << "' "
                 << " index " << fcount << " using (column(1)"
                 << "):(column(2)"<< ')'
                 << " title 'Signal shape " << fkey << "' with lines lw 3"
          ;
        fcount++;
      }
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  }

  calo_ssb.clear_functors();
  calo_ssb.tree_dump(std::clog, "Calo SSB: ");

  return;
}

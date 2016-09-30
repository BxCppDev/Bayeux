// test_signal_signal_data.cxx

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
#include <mctools/signal/signal_data.h>
#include "snemo_signal_geiger_anode_signal_shape.h"

void test_signal_data_1(bool draw_ = false);

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

    test_signal_data_1(draw);

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

void test_signal_data_1(bool draw_)
{
  mctools::signal::signal_data signals;
  signals.grab_auxiliaries().store("Generator", "flsiggen");
  signals.grab_auxiliaries().store("Author", "Bevedere");
  signals.grab_auxiliaries().store("Date", "2016-09-27");

  const double effective_tracker_cell_length = 2.70 * CLHEP::meter;
  const int tracker_cell_category = 1023;
  const int tracker_cell_anode_category = 1024;
  // const int tracker_cell_cathode_category = 1025;
  const double cm_per_us = CLHEP::cm/CLHEP::microsecond;
  const double millivolt = 1e-3 * CLHEP::volt;
  const double mV = millivolt;
  const int cathode_bottom = 0;
  const int cathode_top = 1;
  // const int cathode_index = 4;

  // Loop on Geiger hits:
  for (int icell = 0; icell < 5; icell++) {

    // The hit cell:
    int module_id = 0; // SuperNEMO demonstrator module identifier
    int side_id = 1;   // French half-tracking chamber
    int row_id = 36 + (icell / 2); // Geiger row [0:112]
    int layer_id = icell; // Geiger layer [0:8]
    geomtools::geom_id cell_gid(tracker_cell_category, module_id, side_id, row_id, layer_id);
    double t_hit = 2.0 * CLHEP::microsecond;
    double z_hit = (34.3 + 2.3 * icell) * CLHEP::cm;
    double drift_radius_hit = (drand48() * 22.0 + 0.1) * CLHEP::mm;

    // Simple algorithm to compute Geiger signals charateristics:
    const double drift_velocity  = 0.5 * cm_per_us;
    const double plasma_velocity = 5.0 * cm_per_us;
    const double plasma_propagation_prob = 1.00;
    bool has_cathodes[2] = { true, true };
    if (drand48() > plasma_propagation_prob) {
      has_cathodes[cathode_bottom] = false;
    }
    if (drand48() > plasma_propagation_prob) {
      has_cathodes[cathode_top] = false;
    }
    double anode_drift_time = drift_radius_hit / drift_velocity;
    double t0 = t_hit + anode_drift_time;
    double t_rise = 200.0 * CLHEP::nanosecond;
    double amplitude = (400.0 + drand48() * 200.0) * mV;
    double t1 = datatools::invalid_real();
    double t2 = datatools::invalid_real();
    if (has_cathodes[cathode_bottom]) {
      t1 = t0 + (0.5 * effective_tracker_cell_length - z_hit) / plasma_velocity;
    }
    if (has_cathodes[cathode_top]) {
      t2 = t0 + (0.5 * effective_tracker_cell_length + z_hit) / plasma_velocity;
    }

    int anode_sig_counter = 0;
    {
      // Anode signal:
      mctools::signal::base_signal & sig = signals.add_signal("gg.anode");
      geomtools::geom_id anode_cell_gid;
      anode_cell_gid.set_type(tracker_cell_anode_category);
      anode_cell_gid.set_depth(4);
      anode_cell_gid.inherits_from(cell_gid);
      sig.set_hit_id(anode_sig_counter);
      sig.set_geom_id(anode_cell_gid);
      sig.set_shape_type_id("snemo::signal::geiger_anode_signal_shape");
      sig.set_shape_real_parameter_with_explicit_unit("t0", t0, "us");
      sig.set_shape_real_parameter_with_explicit_unit("t_rise", t_rise, "us");
      sig.set_shape_real_parameter_with_explicit_unit("t1", t1, "us");
      sig.set_shape_real_parameter_with_explicit_unit("t2", t2, "us");
      sig.set_shape_real_parameter_with_explicit_unit("amplitude", amplitude, "mV");
      sig.initialize_simple();
    }

    // // Cathode signals:
    // for (int icath = cathode_bottom; icath <= cathode_top; icath++) {
    //   if (!has_cathodes[icath]) {
    //     continue;
    //   }
    //   mctools::signal::base_signal & sig = signals.add_signal("gg.cathode");
    //   geomtools::geom_id cathode_cell_gid;
    //   cathode_cell_gid.set_type(tracker_cell_cathode_category);
    //   cathode_cell_gid.set_depth(4);
    //   cathode_cell_gid.inherits_from(cell_gid);
    //   cathode_cell_gid.set(cathode_index, icath);
    //   sig.grab_auxiliaries().store("parent_signal.category", "gg.anode");
    //   sig.grab_auxiliaries().store("parent_signal.index", anode_sig_counter);
    //   sig.set_hit_id(cathode_sig_counter);
    //   sig.set_geom_id(cathode_cell_gid);
    //   sig.set_shape_real_parameter_with_explicit_unit("t0", t0, "us");
    //   sig.set_shape_real_parameter_with_explicit_unit("t1", t1, "us");
    //   sig.set_shape_real_parameter_with_explicit_unit("t2", t2, "us");
    //   sig.set_shape_type_id("mctools::signal::triangle_signal_shape");
    //   sig.initialize_simple();
    //   cathode_sig_counter++;
    // }

    anode_sig_counter++;

  }

  signals.tree_dump(std::clog, "Signals:");

  datatools::temp_file tmp_file;
  tmp_file.set_remove_at_destroy(true);
  tmp_file.create("/tmp", "test_signal_signal_data_");

  for (size_t isig = 0; isig < signals.get_number_of_signals("gg.anode"); isig++) {
    const mctools::signal::base_signal & hsig = signals.get_signal("gg.anode", isig);
    hsig.get_shape().write_ascii_file_with_units(tmp_file.get_filename(),
                                                 -10.0 * CLHEP::microsecond,
                                                 +50.0 * CLHEP::microsecond,
                                                 600,
                                                 "us",
                                                 "volt",
                                                 16, 16,
                                                 mygsl::i_unary_function::wo_append | mygsl::i_unary_function::wo_data_index
                                                 );
  }

  if (draw_) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    Gnuplot g1;
    g1.cmd("set title 'Test mygsl::signal::signal_data' ");
    g1.cmd("set key out");
    g1.cmd("set grid");
    // g1.cmd("set xrange [0:100]");
    {
      std::ostringstream cmd1;
      cmd1 << "volt=" << CLHEP::volt;
      g1.cmd(cmd1.str());
      std::ostringstream cmd2;
      cmd2 << "nanosecond=" << CLHEP::nanosecond;
      g1.cmd(cmd2.str());
    }
    g1.cmd("set yrange [-1:+0.1]");
    g1.set_xlabel("time (us)").set_ylabel("Signal (V)");

    {
      std::ostringstream plot_cmd;
      plot_cmd << "plot ";
      for (size_t isig = 0; isig < signals.get_number_of_signals("gg.anode"); isig++) {
        if (isig != 0) plot_cmd << ", ";
        plot_cmd << "'" << tmp_file.get_filename() << "' "
                 << " index " << isig << " using (column(1)"
                 << "):(column(2)"<< ')'
                 << " title 'Signal shape " << isig << "' with lines lw 2";
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

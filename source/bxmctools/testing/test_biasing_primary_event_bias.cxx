// test_biasing_primary_event_bias.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/mygsl:
// #include <mygsl/rng.h>
// - Bayeux/genbb_help:
#include <genbb_help/single_particle_generator.h>
#include <genbb_help/primary_event.h>

// This project:
#include <mctools/biasing/primary_event_bias.h>
#include <mctools/biasing/point_of_interest.h>

int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::biasing::primary_event_bias'!" << std::endl;

    bool debug = false;
    bool draw = false;
    size_t max_count = 10;
    bool a_pois = false;
    bool r_pois = false;
    int iarg = 1;
    while(iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else if ((option == "-m") || (option == "--more")) {
          max_count *= 10;
        } else if ((option == "-a") || (option == "--attractive-pois")) {
          a_pois = true;
        } else if ((option == "-r") || (option == "--repulsive-pois")) {
          r_pois = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    // int seed = 123456;
    // mygsl::rng random;
    // random.init(type, seed);

    mctools::biasing::primary_event_bias peb;
    peb.set_logging(debug? datatools::logger::PRIO_TRACE : datatools::logger::PRIO_FATAL);
    peb.set_bias_mode(mctools::biasing::primary_event_bias::BIAS_ONLY_MASTER_PARTICLE);
    peb.add_particle_by_label("gamma");
    peb.add_particle_by_label("electron");
    peb.set_particle_minimum_energy(800.0 * CLHEP::keV);
    peb.set_master_particle_rank(0);
    peb.set_track_only_master_particle(true);

    if (a_pois) {
      mctools::biasing::point_of_interest target0;
      target0.set_name("target0");
      target0.grab_position().set(0.0, 0.0, 1.5 * CLHEP::meter);
      target0.set_radius(0.45 * CLHEP::meter);
      target0.set_attractivity(+1.0);
      target0.tree_dump(std::clog, target0.get_name() + " :");
      peb.add_point_of_interest(target0.get_name(), target0);
    }

    if (a_pois) {
      mctools::biasing::point_of_interest target1;
      target1.set_name("target1");
      target1.grab_position().set(0.0, 1.0 * CLHEP::meter, 0.0);
      target1.set_radius(0.25 * CLHEP::meter);
      target1.set_attractivity(+1.0);
      target1.tree_dump(std::clog, target1.get_name() + " :");
      peb.add_point_of_interest(target1.get_name(), target1);
    }

    if (r_pois) {
      mctools::biasing::point_of_interest veto0;
      veto0.set_name("veto0");
      veto0.grab_position().set(0.0, -1.0 * CLHEP::meter, 0.0);
      veto0.set_radius(0.35 * CLHEP::meter);
      veto0.set_attractivity(-1.0);
      veto0.tree_dump(std::clog, veto0.get_name() + " :");
      peb.add_point_of_interest(veto0.get_name(), veto0);
    }

    peb.initialize_simple();
    peb.tree_dump(std::clog, "Primary event biaising: ");

    genbb::single_particle_generator spg;
    datatools::properties spg_config;
    if (debug) {
      spg_config.store("logging.priority", "trace");
    }
    spg_config.store("seed", 314159);
    spg_config.store("emission_direction", "randomized");
    spg_config.store("particle_name", "gamma");
    spg_config.store("mode", "energy_range");
    spg_config.store("energy_unit", "keV");
    spg_config.store_with_explicit_unit("min_energy", 900.0 * CLHEP::keV);
    spg_config.store_with_explicit_unit("max_energy",1100.0 * CLHEP::keV);
    // spg_config.store_with_explicit_unit("mean_energy", 1000.0 * CLHEP::keV);
    // spg_config.store_with_explicit_unit("sigma_energy",  50.0 * CLHEP::keV);
    spg.initialize_standalone(spg_config);

    size_t normal_count = 0;
    size_t truncated_count = 0;
    size_t killed_count = 0;
    for (int i = 0; i < (int) max_count; i++) {
      geomtools::vector_3d vertex(0.0, 0.0, 0.0);
      genbb::primary_event pe;
      if (debug || (i % 1000 == 0) || (i == (int) max_count - 1)) std::clog << "Count : " << i << std::endl;
      spg.load_next(pe);
      mctools::biasing::primary_event_bias::biasing_info binfo;
      peb.process(vertex, pe, binfo);
      if (binfo.is_normal()) {
        normal_count++;
      } else if (binfo.is_truncated()) {
        truncated_count++;
      } else if (binfo.is_killed()) {
        killed_count++;
      }
      if (draw) {
        peb.draw(vertex, pe);
      }
      if (debug) {
        pe.tree_dump(std::clog, "Biased primary event: ", "[trace]: ");
      }
    }
    std::clog << "Statistic report:" << std::endl;
    std::clog << "  Total count     = " << max_count << std::endl;
    std::clog << "  Normal count    = " << normal_count << std::endl;
    std::clog << "  Truncated count = " << truncated_count << std::endl;
    std::clog << "  Killed count    = " << killed_count << std::endl;

  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

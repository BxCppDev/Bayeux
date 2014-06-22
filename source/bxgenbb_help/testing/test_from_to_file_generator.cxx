// test_from_to_file_generator.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>

// This project:
#include <genbb_help/single_particle_generator.h>
#include <genbb_help/save_to_file_wrapper.h>
#include <genbb_help/from_file_generator.h>
#include <genbb_help/primary_event.h>

void test_out()
{
  std::clog << "\nTest output: " << std::endl;
  // Build a configuration properties container:
  datatools::properties config;
  config.store("seed", 314159);
  config.store("angle_unit", "degree");
  config.store("cone.min_angle",  0.0);
  config.store("cone.max_angle", 35.0);
  config.store("cone.axis", "z");
  config.store("particle_name", "electron");
  config.store("mode", "gaussian_energy");
  config.store("energy_unit", "keV");
  config.store("mean_energy", 1000.0);
  config.store("sigma_energy", 50.0);
  config.tree_dump(std::clog, "Event generator configuration : ");

  // Configure the event generator:
  genbb::single_particle_generator SPG;
  SPG.initialize_standalone(config);

  genbb::save_to_file_wrapper STFW;
  STFW.set_primary_event_bank_label("PE");
  STFW.set_generator(SPG);
  datatools::properties out_config;
  out_config.store("max_files", 3);
  out_config.store("max_record_total", 36);
  out_config.store("max_record_per_file", 12);
  out_config.store("files.mode", "incremental");
  out_config.store("files.incremental.path", ".");
  out_config.store("files.incremental.prefix", "test_genbb_io_");
  out_config.store("files.incremental.extension", "xml");
  out_config.store("files.incremental.start", 0);
  out_config.store("files.incremental.stop", 10);
  STFW.initialize_standalone(out_config);

  // Shoot events:
  size_t max_count = 40;
  genbb::primary_event pe; // working primary event
  int count = 0;
  for (; count < (int) max_count; count++) {
    std::clog << "Count : " << count << std::endl;
    STFW.load_next(pe);
    // Print the kinetic energy from the only particle in each event:
    std::cout << pe.get_particles().front().get_kinetic_energy()
              << ' ' << pe.get_particles().front().get_momentum().x()
              << ' ' << pe.get_particles().front().get_momentum().y()
              << ' ' << pe.get_particles().front().get_momentum().z()
              << std::endl;
  }
  std::clog << "Number of stored primary events : " << count << std::endl;
  return;
}

void test_in()
{
  std::clog << "\nTest input: " << std::endl;
  datatools::properties in_config;
  in_config.store("max_files", 3);
  in_config.store("max_record_total", 36);
  in_config.store("max_record_per_file", 12);
  in_config.store("files.mode", "incremental");
  in_config.store("files.incremental.path", ".");
  in_config.store("files.incremental.prefix", "test_genbb_io_");
  in_config.store("files.incremental.extension", "xml");
  in_config.store("files.incremental.start", 0);
  in_config.store("files.incremental.stop", 10);

  genbb::from_file_generator FFG;
  FFG.set_primary_event_bank_label("PE");
  FFG.initialize_standalone(in_config);

  // Load event:
  size_t count = 0;
  genbb::primary_event pe; // working primary event
  while (FFG.has_next()) {
    FFG.load_next(pe);
    std::clog << "Count : " << count++ << std::endl;
    std::cout << pe.get_particles().front().get_kinetic_energy()
              << ' ' << pe.get_particles().front().get_momentum().x()
              << ' ' << pe.get_particles().front().get_momentum().y()
              << ' ' << pe.get_particles().front().get_momentum().z()
              << std::endl;
  }
  std::clog << "Number of loaded primary events : " << count << std::endl;

  return;
}

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {

    std::clog << "Test program for class 'genbb::single_particle_generator'!" << std::endl;

    test_out();

    test_in();

    std::clog << "The end." << std::endl;

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

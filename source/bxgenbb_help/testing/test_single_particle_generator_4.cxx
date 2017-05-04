// test_single_particle_generator_4.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// This project:
#include <genbb_help/single_particle_generator.h>
#include <genbb_help/primary_event.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'single_particle_generator'!" << std::endl;

    bool debug = false;
    int  many = 0;
    bool randomized_direction = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-m") || (option == "--many")) {
          many = 1;
        } else if ((option == "-M") || (option == "--many-many")) {
          many = 2;
        } else if ((option == "-S") || (option == "--so-many")) {
          many = 3;
        } else if ((option == "-H") || (option == "--huge-many")) {
          many = 4;
        } else if ((option == "-r") || (option == "--randomized-direction")) {
          randomized_direction = true;
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

    // The generator:
    genbb::single_particle_generator SPG;

    // Build a configuration properties container:
    datatools::properties config;
    config.store("seed", 314159);
    config.store("randomized_direction", randomized_direction);

    std::string Li6_label = genbb::primary_particle::nucleus_to_label(3, 6);
    std::clog << "Using Li-6 nucleus' label = '" << Li6_label << "'" << std::endl;
    config.store("particle_name", Li6_label);
    config.store("mode", "monokinetic");
    config.store("energy_unit", "MeV");
    config.store("energy", 1.4 * CLHEP::keV);
    config.set_explicit_unit("energy", true);
    config.tree_dump(std::clog, "Configuration: ", "debug: ");

    // Configure the event generator:
    SPG.initialize_standalone(config);

    // Shoot events:
    size_t max_count = 10;
    if (many == 1) max_count = 100;
    if (many == 2) max_count = 1000;
    if (many == 3) max_count = 10000;
    if (many == 4) max_count = 100000;
    genbb::primary_event pe; // working primary event
    for (int i = 0; i < (int) max_count; i++) {
      std::clog << "Count : " << i << std::endl;
      SPG.load_next(pe, false);
      if (debug) pe.tree_dump(std::clog, "Primary particle: ");
      // Print the kinetic energy from the only particle in each event:
      double ke = pe.get_particles().front().get_kinetic_energy();
      std::cout << "Ke=" << ke / CLHEP::keV << " keV" << std::endl;
    }

    std::clog << "debug: " << "The end." << std::endl;
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

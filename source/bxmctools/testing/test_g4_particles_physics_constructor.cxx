// test_g4_particles_physics_constructor.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <mygsl/rng.h>

#include <mctools/g4/particles_physics_constructor.h>

int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::g4::particles_physics_constructor'!" << std::endl;

    bool debug = false;
    std::string type = "taus2";

    int iarg = 1;
    while(iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") ||(option == "--debug")) {
          debug = true;
        }
        else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      }
      else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    mctools::g4::particles_physics_constructor PPC;

    datatools::properties PPC_config;
    PPC_config.set_description("Geant4 particles physics constructor parameters");
    if (debug) PPC_config.store("logging.priority", "debug");
    PPC_config.store_flag("use_muon_leptons");
    PPC_config.store_flag("use_light_mesons");
    PPC_config.store_flag("use_nucleons");
    PPC_config.store_flag("use_strange_baryons");
    PPC_config.store_flag("use_light_nuclei");
    PPC_config.store_flag("use_generic_ion");

    PPC.initialize_standalone(PPC_config);
    PPC.tree_dump(std::cout, "Geant4 particles definition : ");

    std::cerr << "INFO: Invoke the Geant4 interface..." << std::endl;
    PPC.ConstructParticle();
    std::cerr << "INFO: Done." << std::endl;

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

// end of test_g4_particles_physics_constructor.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

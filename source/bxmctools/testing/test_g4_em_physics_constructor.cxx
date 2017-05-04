// test_g4_em_physics_constructor.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <mygsl/rng.h>

#include <mctools/g4/em_physics_constructor.h>

int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::g4::em_physics_constructor'!" << std::endl;

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

    mctools::g4::em_physics_constructor EmPC;
    EmPC.set_name("electromagnetic");
    EmPC.set_class_id("mctools::g4::em_physics_constructor");

    datatools::properties EmPC_config;
    EmPC_config.set_description("Geant4 electro-magnetism physics constructor parameters");
    if (debug) EmPC_config.store("logging.priority", "debug");
    EmPC_config.store("em.model", "penelope");
    //EmPC_config.store_flag("em.gamma.rayleigh_scattering");
    EmPC_config.store_flag("em.gamma.photo_electric");
    EmPC_config.store_flag("em.gamma.compton_scattering");
    EmPC_config.store_flag("em.gamma.conversion");
    //EmPC_config.store_flag("em.gamma.conversion_to_muons");
    EmPC_config.store_flag("em.electron.ionisation");
    EmPC_config.store_flag("em.electron.bremsstrahlung");
    EmPC_config.store_flag("em.positron.annihilation");
    EmPC_config.store_flag("em.electron.multiple_scattering");
    EmPC_config.store_flag("em.electron.multiple_scattering.use_distance_to_boundary");
    EmPC_config.store("em.electron.multiple_scattering.range_factor", 0.005);
    //EmPC_config.store_flag("em.electron.step_limiter");
    EmPC_config.store_flag("em.deexcitation.fluorescence");
    EmPC_config.store_flag("em.deexcitation.auger");
    EmPC_config.store_flag("em.deexcitation.pixe");
    std::vector<std::string> deexcitation_regions;
    deexcitation_regions.push_back("A");
    deexcitation_regions.push_back("B");
    EmPC_config.store("em.deexcitation.regions", deexcitation_regions);
    EmPC_config.store_flag("em.deexcitation.regions.A.fluorescence");
    EmPC_config.store_flag("em.deexcitation.regions.A.auger");
    EmPC_config.store_flag("em.deexcitation.regions.A.pixe");
    EmPC_config.store_flag("em.deexcitation.regions.B.fluorescence");
    //EmPC_config.store_flag("em.deexcitation.regions.B.auger");
    //EmPC_config.store_flag("em.deexcitation.regions.B.pixe");
    EmPC_config.store_flag("em.ion.ionisation");
    EmPC_config.store_flag("em.ion.multiple_scattering");
    //EmPC_config.store_flag("em.ion.step_limiter");
    EmPC_config.store_flag("em.muon.ionisation");
    EmPC_config.store_flag("em.muon.bremsstrahlung");
    EmPC_config.store_flag("em.muon.pair_production");
    EmPC_config.store_flag("em.muon.multiple_scattering");
    //EmPC_config.store_flag("em.muon.step_limiter");

    EmPC.initialize_standalone(EmPC_config);
    EmPC.tree_dump(std::cout, "Geant4 EM processes : ");

    std::cerr << "INFO: Invoke the Geant4 interface..." << std::endl;
    EmPC.ConstructParticle();
    //EmPC.ConstructProcess();
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

// Standard Library
#include <iostream>

// Third Party
// - Bayeux
#include <bayeux/version.h>
#include <mctools/g4/manager.h>
#include <mctools/g4/manager_parameters.h>

int main(int argc, const char *argv[])
{
  std::cout << "[bxmctoolsg4] Bayeux version string = "
            << bayeux::version::get_version() << std::endl;
  mctools::g4::manager_parameters simParams;
  simParams.set_defaults();

  //mctools::g4::manager simManager;
  //simManager.dump_base(std::cout, "The Geant4 manager: ");
  return 0;
}

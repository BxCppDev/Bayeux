// test_g4_processes_em_model_factory.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Geant4:
#include <G4VEmModel.hh>

// This project:
#include <mctools/g4/processes/em_model_factory.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::g4::processes::em_model_factory'!" << std::endl;

    // bool debug = false;

    int iarg = 1;
    while(iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") ||(option == "--debug")) {
          // debug = true;
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

    mctools::g4::processes::em_model_factory EmModFact;
    EmModFact.initialize();
    boost::scoped_ptr<G4VEmModel> pEmMod;
    pEmMod.reset(EmModFact.create_model("BraggIon", "myBraggIonModel"));
    if (pEmMod) {
      std::clog << "EM model name: '" << pEmMod->GetName() << "'" << std::endl;
    } else {
      DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                   "No EM model of type : '" << "BraggIon" << "' has been created!");
    }
    EmModFact.reset();

    std::cerr << "Done." << std::endl;

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

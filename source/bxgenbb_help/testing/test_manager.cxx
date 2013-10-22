// -*- mode: c++ ; -*-
// test_manager.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <datatools/units.h>

#include <mygsl/rng.h>
#include <mygsl/histogram.h>

#include <genbb_help/genbb_help_config.h>
#include <genbb_help/manager.h>
#include <genbb_help/primary_event.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'genbb::manager' class." << std::endl;

    bool debug = false;
    bool verbose = false;
    std::string pg_name;
    int nevents = 10000;
    bool use_fortran_lib = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug"))  {
          debug = true;
        } else if ((option == "-v") || (option == "--verbose"))  {
          verbose = true;
        } else if ((option == "-g") || (option == "--particle-generator"))  {
          pg_name = argv_[++iarg];
        } else if ((option == "-f") || (option == "--use-fortran"))  {
          use_fortran_lib = true;
        } else  {
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

    // External PRNG :
    mygsl::rng PRNG;
    PRNG.initialize("taus2", 314159);

    // Particle generator manager configuration:
    datatools::properties config;
    std::string config_filename;
    config_filename = "${GENBB_HELP_TESTING_DIR}/config/test-2.0/manager.conf";
#if GENBB_HELP_WITH_FORTRAN == 1
    if (use_fortran_lib) {
      config_filename = "${GENBB_HELP_TESTING_DIR}/config/test-1.0/manager.conf";
    }
#endif // GENBB_HELP_WITH_FORTRAN == 1
    datatools::fetch_path_with_env(config_filename);
    datatools::properties::read_config(config_filename, config);

    // Particle generator manager :
    genbb::manager PGMgr;
    PGMgr.set_debug(debug);
    PGMgr.set_external_prng(PRNG); // Attach the PRNG to the manager
    PGMgr.initialize(config);
    PGMgr.dump_particle_generators(std::clog,
                                   "List of particle generators: ",
                                   "NOTICE: ");

    if (pg_name.empty()) {
      pg_name = "bkg_Co60";
    }
    if (PGMgr.has(pg_name)) {
      mygsl::histogram hEc(70, 0.0 * CLHEP::keV, 3500.0 * CLHEP::keV);
      std::clog << "NOTICE: " << "Found particle generator named '"
                << pg_name << "'" << std::endl;
      genbb::i_genbb & PG = PGMgr.grab(pg_name);

      int count = 0;
      while (PG.has_next()) {
        genbb::primary_event decay_event;
        PG.load_next(decay_event, false);
        decay_event.set_label (pg_name);
        if (count < 1) decay_event.tree_dump (std::clog,
                                              "Generated primary event : ",
                                              "NOTICE: ");
        hEc.fill (decay_event.get_particles().front().get_kinetic_energy ());
        count++;
        if (count >= nevents) break;
      }
      PGMgr.reset(pg_name);
      std::string sname = "electron.hist";
      std::ofstream ofhist(sname.c_str());
      hEc.print(ofhist);
      ofhist.close();
    } else {
      std::clog << "NOTICE: " << "Cannot find particle generator named '"
                << pg_name << "'" << std::endl;
    }

    pg_name = "electron_pdf";
    if (PGMgr.has(pg_name)) {
      mygsl::histogram hEc(100, 0.0 * CLHEP::keV, 3500.0 * CLHEP::keV);
      std::clog << "NOTICE: " << "Found particle generator named '"
                << pg_name << "'" << std::endl;
      genbb::i_genbb & PG = PGMgr.grab(pg_name);
      nevents = 100000;
      int count = 0;
      while (PG.has_next())  {
        genbb::primary_event decay_event;
        PG.load_next(decay_event, false);
        decay_event.set_label (pg_name);
        hEc.fill (decay_event.get_particles().front().get_kinetic_energy ());
        count++;
        if (count >= nevents) break;
      }
      PGMgr.reset(pg_name);
      std::string sname = "electron_pdf.hist";
      std::ofstream ofhist(sname.c_str());
      hEc.print(ofhist);
      ofhist.close();
    } else {
      std::clog << "NOTICE: " << "Cannot find particle generator named '"
                << pg_name << "'" << std::endl;
    }

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "ERROR: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_manager.cxx

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

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for the 'genbb::manager' class." << endl; 
  
      bool debug = false;
      bool verbose = false;
      string pg_name;
      int nevents = 10000;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-') 
            {
               string option = token; 
               if ((option == "-d") || (option == "--debug")) 
                 {
                   debug = true;
                 }
               else if ((option == "-v") || (option == "--verbose")) 
                 {
                   verbose = true;
                 }
                else if ((option == "-g") || (option == "--particle-generator")) 
                 {
                   pg_name = argv_[++iarg];
                 }
               else 
                 { 
                   clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            } 
          iarg++; 
      }

      // PRNG :
      mygsl::rng PRNG;
      PRNG.initialize("taus", 314159);

      // Particle generator manager configuration:
      datatools::properties config;
      config.store_flag("debug");
      std::vector<std::string> pg_conf_files;
      pg_conf_files.push_back("${GENBB_HELP_DATA_DIR}/testing/config/dbd.conf");
      pg_conf_files.push_back("${GENBB_HELP_DATA_DIR}/testing/config/backgrounds.conf");
      pg_conf_files.push_back("${GENBB_HELP_DATA_DIR}/testing/config/calibrations.conf");
      pg_conf_files.push_back("${GENBB_HELP_DATA_DIR}/testing/config/misc.conf");
      config.store("generators.configuration_files", pg_conf_files);

      // Particle generator manager :
      genbb::manager PGMgr;
      PGMgr.set_debug(debug);
      PGMgr.set_external_prng(PRNG);
      PGMgr.initialize(config);
      PGMgr.dump_particle_generators(std::clog, 
                                     "List of particle generators: ",
                                     "NOTICE: ");

      if (pg_name.empty())
        {
          pg_name = "bkg_Co60";
        }
      if (PGMgr.has(pg_name))
        {
          mygsl::histogram hEc(70, 0.0 * CLHEP::keV, 3500.0 * CLHEP::keV);
          std::clog << "NOTICE: " << "Found particle generator named '"
                    << pg_name << "'" << std::endl;
          genbb::i_genbb & PG = PGMgr.grab(pg_name);

          int count = 0;
          while (PG.has_next())
            {
              genbb::primary_event decay_event;
              PG.load_next(decay_event, false);
              decay_event.set_label (pg_name);
              if (count < 1) decay_event.tree_dump (std::clog, "Generated primary event : ",
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
        }
      else
        {
          std::clog << "NOTICE: " << "Cannot find particle generator named '"
                    << pg_name << "'" << std::endl;
        }

      pg_name = "electron_pdf";
      if (PGMgr.has(pg_name))
        {
          mygsl::histogram hEc(100, 0.0 * CLHEP::keV, 3500.0 * CLHEP::keV);
          std::clog << "NOTICE: " << "Found particle generator named '"
                      << pg_name << "'" << std::endl;
          genbb::i_genbb & PG = PGMgr.grab(pg_name);
          nevents = 100000;
          int count = 0;
          while (PG.has_next())
            {
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
        }
      else
        {
          std::clog << "NOTICE: " << "Cannot find particle generator named '"
                    << pg_name << "'" << std::endl;
        }

      clog << "The end." << endl;
    }
  catch (exception & x)
    { 
      cerr << "ERROR: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "ERROR: " << "unexpected error !" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_manager.cxx

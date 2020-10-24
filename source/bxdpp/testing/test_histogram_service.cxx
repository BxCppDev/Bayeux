/* test_histogram_service.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-03-08
 * Last modified :
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrate how to manipulate
 *  a histrogram service through a service manager.
 *
 * Usage:
 *
 *  test_histogram_service --debug
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/base_service.h>
#include <datatools/service_manager.h>

// This project:
#include <dpp/histogram_service.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  int seed = 314159;
  try
    {
      clog << "Test program for class 'dpp::histogram_service' !" << endl;

      bool debug = false;
      bool export_root = false;

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
              else if ((option == "-r") || (option == "--export-root"))
                {
                  export_root = true;
                }
              else
                {
                  clog << datatools::io::warning << "ignoring option '" << option << "'!" << endl;
                }
            }
          else
            {
              string argument = token;
              {
                clog << datatools::io::warning << "ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
        }

      srand48(seed);

      DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (datatools::base_service).print (clog);

      {
        uint32_t SM_flags = datatools::service_manager::BLANK;
        datatools::service_manager SM ("SM",
                                       "A service manager",
                                       SM_flags);
        if (debug) SM.set_logging_priority(datatools::logger::PRIO_DEBUG);
        SM.tree_dump (clog, "Service manager : ");

        // Load another multi_property container stored in a file :
        datatools::properties services_setup;
        string services_setup_file = "${DPP_TESTING_DIR}/config/test_service_manager.conf";
        datatools::fetch_path_with_env (services_setup_file);
        datatools::read_config (services_setup_file, services_setup);
        SM.initialize (services_setup);

        if (SM.has ("Histos") && SM.is_a<dpp::histogram_service> ("Histos"))
          {
            // Access to the histogram service by reference
            // through its name and class :
            dpp::histogram_service & Histos
              = SM.grab<dpp::histogram_service> ("Histos");

            Histos.tree_dump(std::clog, "Histos service : ");

            mygsl::histogram_pool & HP = Histos.grab_pool ();

            mygsl::histogram_1d & hngg     = HP.grab_1d("MC::ngghits");
            mygsl::histogram_1d & hscin    = HP.grab_1d("MC::ncalohits");
            mygsl::histogram_2d & hnggscin = HP.grab_2d("SD::ngghits_vs_ncalohits");
            mygsl::histogram_1d & he1   = HP.grab_1d("calib::e1");
            mygsl::histogram_1d & he2   = HP.grab_1d("calib::e2");
            mygsl::histogram_2d & he1e2 = HP.grab_2d("CD::e1_vs_e2");
            for (int i = 0; i < 1000; i++)
              {
                int ngg = (int) (20 * drand48());
                int nscin = (int) (4 *  drand48());
                double e1 = 50. + 2000. *  drand48();
                double e2 = 50. + 1600. *  drand48();
                e1 *= CLHEP::keV;
                e2 *= CLHEP::keV;
                hngg.fill(ngg);
                hscin.fill(nscin);
                he1.fill(e1);
                he2.fill(e2);
                hnggscin.fill(ngg, nscin);
                he1e2.fill(e1, e2);
              }

            if (export_root) Histos.store_as_root_file("${DPP_TMP_TEST_DIR}/test_histogram_service.root");

          }
        clog << datatools::io::debug << "Terminating service manager..." << endl;
        SM.reset ();
        SM.tree_dump (clog, "Service manager (terminated) : ", "");
      }

      {
        datatools::object_configuration_description OCD;
        datatools::load_ocd<dpp::histogram_service>(OCD);
        OCD.dump(std::clog, "OCD: ");
        std::ofstream fscf ("test_OCD_histogram_service.sample.conf");
        OCD.generate_sample_configuration(fscf, "the configuration of a 'dpp::histogram_service' object");

        // unlink("test_OCD_histogram_service.sample.conf");
      }
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

/* test_service_manager.cxx
 * Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-09
 * Last modified : 2015-09-22
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A test sample program that illustrate how to create a service manager
 *  that manages a geometry service.
 *
 * Usage:
 *
 *  test_service_manager --debug
 *
 */

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include <stdexcept>

// Third Party:
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/base_service.h>
#include <datatools/service_manager.h>

// This project:
#include <geomtools/geometry_service.h>
#include <geomtools/manager.h>
#include <geomtools/placement.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;

  try
    {
      clog << "Test program for class 'geomtool::geometry_service'." << endl;

      bool debug = false;
      bool visu = false;

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
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
              else if ((option == "-v") || (option == "--visu"))
                {
                  visu = true;
                }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
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

      DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (datatools::base_service).print (clog);

      {
        uint32_t SM_flags = datatools::service_manager::BLANK;
        datatools::service_manager SM ("SM", "A test service manager", SM_flags);
        if (debug) SM.set_logging_priority(datatools::logger::PRIO_DEBUG);
        SM.tree_dump (clog, "Service manager : ");

        datatools::properties SM_setup;
        string SM_setup_file = "${GEOMTOOLS_TESTING_DIR}/config/test_service_manager.conf";
        if (debug) clog << datatools::io::debug << "Setup file = "
                        << SM_setup_file << endl;
        datatools::fetch_path_with_env (SM_setup_file);
        datatools::read_config(SM_setup_file, SM_setup);
        SM_setup.tree_dump(std::cerr, "The service manager setup : ");
        if (debug) clog << datatools::io::debug << "Initializing the service manager..." << endl;
        SM.initialize (SM_setup);

        if (SM.has ("Geo") && SM.is_a<geomtools::geometry_service> ("Geo"))
          {
            // Access to a service by (const) reference through its name and class :
            const geomtools::geometry_service & Geo = SM.get<geomtools::geometry_service> ("Geo");
            clog << "Geometry service 'Geo' is known !" << endl;

            // Access to the embeded geometry manager :
            const geomtools::manager & geom_mgr = Geo.get_geom_manager();
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
            if (visu)
              {
                std::string visu_drawer_view = geomtools::gnuplot_drawer::view_3d();
                bool visu_drawer_labels = true;
                std::string visu_model_name = geom_mgr.get_world_name ();
                geomtools::placement p;
                p.set (0, 0, 0, 0 * CLHEP::degree, 0 * CLHEP::degree, 0);
                geomtools::gnuplot_drawer GPD;
                GPD.set_mode (geomtools::gnuplot_drawer::mode_wired());
                GPD.set_view (visu_drawer_view);
                GPD.set_labels (visu_drawer_labels);
                GPD.draw_model (geom_mgr.get_factory(),
                                visu_model_name,
                                p,
                                geomtools::gnuplot_drawer::display_level_no_limit());
              }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
          }
        else
          {
            clog << "Geometry service 'Geo' is NOT known !" << endl;
          }

        SM.tree_dump (clog, "Service manager (used): ", "");
        SM.reset ();
        SM.tree_dump (clog, "Service manager (terminated) : ", "");
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

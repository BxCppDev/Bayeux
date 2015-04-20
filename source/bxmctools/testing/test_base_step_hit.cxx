// test_base_step_hit.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/io_factory.h>
// - Bayeux/geomtools:
#include <geomtools/utils.h>

// This project:
#include <mctools/base_step_hit.h>
#include <mctools/base_step_hit.ipp>

// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>
#include <mctools/bio_guard.h>

int main (int /* argc_ */, char ** /* argv_ */)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'base_step_hit'!" << endl;

      // bool debug = false;

      // int iarg = 1;
      // while (iarg < argc_)
      //   {
      //     string token = argv_[iarg];

      //     if (token[0] == '-')
      //       {
      //         string option = token;
      //         if ((option == "-d") || (option == "--debug"))
      //           {
      //              debug = true;
      //           }
      //         else
      //           {
      //             clog << "warning: ignoring option '" << option << "'!" << endl;
      //           }
      //       }
      //     else
      //       {
      //         string argument = token;
      //         {
      //           clog << "warning: ignoring argument '" << argument << "'!" << endl;
      //         }
      //       }
      //     iarg++;
      //   }

      geomtools::vector_3d position (0., 0., 0.);
      geomtools::vector_3d direction (0.1, 0., 0.);
      double time = 0.0;
      double speed = 1.0;
      double mass = 1.0;

      // Serialization:
      datatools::data_writer DW ("test_base_step_hit.xml",
                                 datatools::using_multi_archives);

      mctools::base_step_hit BSH;
      BSH.tree_dump (clog, "Void step hit: ", "TEST: ");
      for (int i = 0; i < 30; i++)
        {
          // begin of step hit:
          BSH.set_hit_id (i);
          BSH.set_geom_id (geomtools::geom_id (1000, 3, 4, 5 + i));
          BSH.set_particle_name ("geantino");
          BSH.set_position_start (position);
          BSH.set_time_start (time);

          geomtools::vector_3d step;
          geomtools::randomize_direction (drand48, step);
          step += geomtools::vector_3d (1. + 1. * drand48 ());
          BSH.set_momentum_start (step.unit () * speed * mass);

          position += step;
          time += step.mag () / speed;

          BSH.set_position_stop (position);
          BSH.set_time_stop (time);
          BSH.set_energy_deposit (drand48 () * 10. * CLHEP::eV * step.mag ());
          if (drand48 () < 0.3)
            {
              BSH.grab_auxiliaries ().store ("sensitive", "drift_chamber");
              BSH.grab_auxiliaries ().store ("medium", "tracking_gas");
              BSH.grab_auxiliaries ().store_flag ("simulated");
            }
          // end of step hit:
          BSH.tree_dump (clog, "Step hit: ");
          DW.store (BSH);
          BSH.reset ();
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

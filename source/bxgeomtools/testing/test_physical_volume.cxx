// test_physical_volume.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <cmath>

#include <geomtools/box.h>
#include <geomtools/physical_volume.h>
#include <geomtools/logical_volume.h>
#include <geomtools/placement.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'geom_id'!" << endl;

      // bool debug = false;

      bool step1 = false;
      bool step2 = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];
          // if ((arg == "-d") || (arg == "--debug"))
          //   {
          //     debug = true;
          //   }
          if (arg == "-1")
           {
             step1 = true;
           }
         if (arg == "-2")
           {
             step2 = true;
           }
          iarg++;
        }

      if (step1)
        {
          geomtools::box world_box (1.0, 1.0, 1.0);
          geomtools::logical_volume world_log ("world", world_box);
          world_log.grab_parameters ().store ("material", "vacuum");
          world_log.grab_parameters ().store ("visualization.visible", false);
          world_log.tree_dump (cout, "World");

          geomtools::box vessel_box (0.9, 0.9, 0.9);
          geomtools::logical_volume vessel_log ("vessel_log", vessel_box);
          vessel_log.grab_parameters ().store ("material", "copper");
          vessel_log.grab_parameters ().store ("visualization.visible", true);
          vessel_log.grab_parameters ().store ("visualization.color", "red");
          vessel_log.tree_dump (cout, "Vessel");

          geomtools::placement vessel_pl (-0.25, 0., 0.,
                                          0., 0.5 * M_PI, 0.);
          geomtools::physical_volume vessel_phys ("vessel_phys",
                                                  vessel_log,
                                                  world_log,
                                                  vessel_pl);
          vessel_phys.grab_parameters ().store ("author", "anonymous");

          vessel_phys.tree_dump (cout, "vessel_phys", ">>> ");
          world_log.tree_dump (cout, "world_log", ">>> ");
        }

      if (step2)
        {
          geomtools::box world_box (1.0, 1.0, 1.0);
          geomtools::logical_volume world_log ("world", world_box);
          world_log.grab_parameters ().store ("material", "vacuum");
          world_log.grab_parameters ().store ("visualization.visible", false);
          world_log.tree_dump (cout, "World");
          geomtools::box vessel_box (0.9, 0.9, 0.9);
          geomtools::logical_volume vessel_log ("vessel", vessel_box);
          vessel_log.grab_parameters ().store ("material", "copper");
          vessel_log.grab_parameters ().store ("visualization.visible", true);
          vessel_log.grab_parameters ().store ("visualization.color", "red");
          vessel_log.tree_dump (cout, "Vessel");

          geomtools::box gas_box (0.85, 0.85, 0.85);
          geomtools::logical_volume gas_log ("gas", gas_box);
          gas_log.grab_parameters ().store ("material", "air");
          gas_log.grab_parameters ().store ("material.state", "gas");
          gas_log.grab_parameters ().store ("material.pressure", 1.0 * CLHEP::atmosphere);
          gas_log.grab_parameters ().store ("material.temperature", 300. * CLHEP::kelvin);
          gas_log.grab_parameters ().store ("visualization.visible", true);
          gas_log.grab_parameters ().store ("visualization.color", "blue");
          gas_log.tree_dump (cout, "Gas");

          geomtools::box electrode_box (0.2, 0.2, 0.01);
          geomtools::logical_volume electrode_log ("electrode", electrode_box);
          electrode_log.grab_parameters ().store ("material", "iron");
          electrode_log.grab_parameters ().store ("material.state", "solid");
          electrode_log.grab_parameters ().store ("visualization.visible", true);
          electrode_log.grab_parameters ().store ("visualization.color", "red");
          electrode_log.tree_dump (cout, "electrode");

          geomtools::placement gas_pl (0., 0., 0., 0., 0., 0.);
          geomtools::physical_volume gas_phys ("gas", gas_log, vessel_log, gas_pl);
          gas_phys.tree_dump (cout, "Gas_phys");

          geomtools::placement electrode_0_pl (-0.25, 0., 0., 0., M_PI / 2, 0.);
          geomtools::placement electrode_1_pl (+0.25, 0., 0., M_PI, M_PI / 2, 0.);
          geomtools::physical_volume electrode_0_phys ("electrode_0",
                                                       electrode_log,
                                                       gas_log,
                                                       electrode_0_pl);
          electrode_0_phys.grab_parameters ().store ("potential", +1.0 * CLHEP::volt);

          geomtools::physical_volume electrode_1_phys ("electrode_1",
                                                       electrode_log,
                                                       gas_log,
                                                       electrode_1_pl);
          electrode_1_phys.grab_parameters ().store ("potential", -1.0 * CLHEP::volt);

          electrode_0_phys.tree_dump (cout, "electrode_0", ">>> ");
          electrode_1_phys.tree_dump (cout, "electrode_1", ">>> ");
          gas_log.tree_dump (cout, "Gas");
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
  return error_code;
}

#include <datatools/properties.h>
#include <datatools/clhep_units.h>

using namespace std;

int main (void)
{
  try 
    {
      {
        datatools::properties setup;

        std::string setup_file = "${DATATOOLS_DATA_DIR}/testing/config/test_properties_sample.conf";
        datatools::fetch_path_with_env(setup_file);        
        datatools::properties::read_config (setup_file, setup);
        
        setup.tree_dump (cout, "Setup: ");
        
        if (setup.has_key("Mass"))
          {
            double mass = setup.fetch_real("Mass");
            if (! setup.has_explicit_unit("Mass"))
              {
                mass *= CLHEP::kg;
                std::clog << "Forcing unit for Mass..." << std::endl;
             }
            else
              {
                std::clog << "Mass has its own explicit unit..." << std::endl;
              }
            std::clog << "Mass = " << mass / CLHEP::g << " g" << std::endl;
          }
       if (setup.has_key("Value"))
          {
            double value = setup.fetch_real("Value");
            if (! setup.has_explicit_unit("Value"))
              {
                value *= CLHEP::m;
                std::clog << "Forcing unit for Value..." << std::endl;
              }
            else
              {
                std::clog << "Value has its own explicit unit..." << std::endl;
              }
            std::clog << "Value = " << value / CLHEP::mm << " mm" << std::endl;
          }

      }
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
    }

  return 0;
}


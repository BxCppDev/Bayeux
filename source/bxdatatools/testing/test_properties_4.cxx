// This Project:
#include <datatools/properties.h>
#include <datatools/clhep_units.h>

int main (void)
{
  using namespace std;
  try {
    {
      datatools::properties setup;
      
      std::string setup_file = "${DATATOOLS_TESTING_DIR}/config/test_properties_sample.conf";
      datatools::fetch_path_with_env(setup_file);
      datatools::properties::read_config (setup_file, setup);

      setup.tree_dump (cout, "Setup: ");

      if (setup.has_key("Mass")){
        double mass = setup.fetch_real("Mass");
        if (! setup.has_explicit_unit("Mass")) {
          mass *= CLHEP::kg;
          std::clog << "Forcing unit for Mass..." << std::endl;
        } else {
          std::clog << "Mass has its own explicit unit..." << std::endl;
        }
        std::clog << "Mass = " << mass / CLHEP::g << " g" << std::endl;
      }
      if (setup.has_key("Value")) {
        double value = setup.fetch_real("Value");
        if (! setup.has_explicit_unit("Value")) {
          value *= CLHEP::m;
          std::clog << "Forcing unit for Value..." << std::endl;
        } else {
          std::clog << "Value has its own explicit unit..." << std::endl;
        }
        std::clog << "Value = " << value / CLHEP::mm << " mm" << std::endl;
      }
      
      std::clog << "\nAdd more properties..." << std::endl;

      setup.store("test", true, "Test flag");
      setup.store("nitems", 42, "Number of items");
      setup.store("favorite_color", "Blue", "my favorite color");
      setup.store("ratio", 3.14159, "The Pi ratio");
      setup.store_with_explicit_unit("width", 12.34 * CLHEP::meter, "The width of the table");
      setup.set_unit_symbol("width", "mm");
      std::vector<double> pos({ 1.2 * CLHEP::km, 3.4 * CLHEP::km});
      setup.store("position", pos, "The position on the map");
      setup.set_unit_symbol("position", "km");
      setup.tree_dump (cout, "Setup: ");

    }
  } catch (exception & x) {
    cerr << "error: " << x.what () << endl;
  }

  return 0;
}

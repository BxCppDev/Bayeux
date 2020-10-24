// Ourselves:
#include <datatools/properties.h>

// Standard library:
#include <iostream>
#include <exception>

// This Project:
#include <datatools/clhep_units.h>

void tpm();

int main(void)
{
  try {
    tpm();
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    return 1;
  }
  return 0;
}

void tpm()
{
  bool allow_override = false;
  allow_override = true;
  bool run_override = true;
  bool run_errors = false;
  // run_errors = true;
  
  try {
    datatools::properties baseConfig("Base configuration");
    baseConfig.store_flag("debug", "The debug status"); 
    baseConfig.store("nevent", 10000, "The number of events"); 
    baseConfig.store("pi", 3.14159, "The PI constant"); 
    baseConfig.lock("pi");                             
    baseConfig.store_real_with_explicit_unit("value", 1.2345 * CLHEP::mm, "A physical quantity"); 
    baseConfig.set_unit_symbol("value", "mm");
    baseConfig.store("James_Bond", "007", "A famous spy !");
    std::clog << "\nbaseConfig: " << std::endl;
    baseConfig.print_tree(std::clog);

    datatools::properties addonConfig("Add-on configuration");
    addonConfig.store_flag("foo");
    addonConfig.store("magick", 42);
    if (run_override) {
      addonConfig.store("pi", 3.14, "The PI constant");
      if (run_errors) {
        addonConfig.store_path("James_Bond", "007", "A famous spy !");
      } else {
        addonConfig.store("James_Bond", "007++", "A famous spy (version 2.0)!");
      }
      if (run_errors) {
        addonConfig.store_real_with_explicit_unit("value",
                                                 5.4321 * CLHEP::kg,
                                                 "A physical quantity");
        addonConfig.set_unit_symbol("value", "kg");
      } else  {
        addonConfig.store_real_with_explicit_unit("value",
                                                  5.4321 * CLHEP::mm,
                                                  "A physical quantity");
        addonConfig.set_unit_symbol("value", "mm");
      }
    }
    std::clog << "\naddonConfig: " << std::endl;
    addonConfig.print_tree(std::clog);

    std::clog << "\nbaseConfig after merging: " << std::endl;
    baseConfig.merge_with(addonConfig, allow_override);
    baseConfig.print_tree(std::clog);
   
    datatools::write_config("base_config.conf", baseConfig);

    {
      datatools::properties baseConfig2;
      datatools::read_config("base_config.conf", baseConfig2);
      std::clog << "\nloaded: " << std::endl;
      baseConfig2.dump(std::clog);
    }
 
  } catch (std::exception & x) {
    std::cerr << "error: as expected: " << x.what() << std::endl;
  }

  return;
}

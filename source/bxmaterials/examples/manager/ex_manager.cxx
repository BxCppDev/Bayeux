// materials example: ex_manager.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools;
#include <datatools/utils.h>
#include <datatools/properties.h>

// This project:
#include <materials/materials_config.h>
#include <materials/manager.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "materials example program : ex_manager" << std::endl;

    bool debug = false;
    std::string config_filename = "";
    std::string material_name = "";

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else {
          std::clog << "warning: ignoring option '"
                    << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        if (config_filename.empty()) {
          config_filename = argument;
        } else if (material_name.empty()) {
          material_name = argument;
        } else {
          std::clog << "warning: ignoring argument '"
                    << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    // Declare a set of configuration parameters :
    datatools::properties material_manager_config;
    if (config_filename.empty()) {
      config_filename = "config/manager.conf";
    }
    datatools::fetch_path_with_env(config_filename);
    // Load configuration parameters from a file :
    datatools::read_config(config_filename,
                                       material_manager_config);

    // Declare a material manager :
    materials::manager my_material_manager;
    my_material_manager.set_debug(debug);

    // Initialization from the set of parameters extracted from the configuration file :
    my_material_manager.initialize(material_manager_config);

    my_material_manager.tree_dump(std::clog, "The material manager : ");

    // Pickup a specific material by name :
    const materials::material_dict_type & materials_map = my_material_manager.get_materials();
    if (material_name.empty()) {
      material_name = "my::water";
    }
    if (materials_map.find(material_name) != materials_map.end()) {
      const materials::material & my_material = materials_map.find(material_name)->second.get_ref ();
      my_material.tree_dump(std::clog, "The material is : ");
    } else {
      std::ostringstream message;
      message << "No material with name '" << material_name << "' !";
      throw std::logic_error(message.str());
    }
    std::clog << std::endl;

    // Pickup a specific material through an alias :
    if (my_material_manager.has_material("metal")) {
      const materials::material & my_metal = my_material_manager.get_material("metal");
      my_metal.tree_dump(std::clog, "The metal alias is : ");
      // const materials::material & my_metal2 = my_material_manager.get_unaliased_material("metal");
      // my_metal2.tree_dump(std::clog, "The unaliased metal alias is : ");
    }
    std::clog << std::endl;

    // Pickup a specific material through an alias of alias :
    if (my_material_manager.has_material("soft_metal")) {
      const materials::material & my_soft_metal = my_material_manager.get_material("soft_metal");
      my_soft_metal.tree_dump(std::clog, "The soft metal alias is : ");
      // const materials::material & my_soft_metal2 = my_material_manager.get_unaliased_material("soft_metal");
      // my_soft_metal2.tree_dump(std::clog, "The unaliased soft metal alias is : ");
    }
    std::clog << std::endl;

    // Pickup a specific material through an alias of alias :
    if (my_material_manager.has_material("dummy_metal")) {
      const materials::material & my_dummy_metal = my_material_manager.get_material("dummy_metal");
      my_dummy_metal.tree_dump(std::clog, "The dummy metal alias is : ");
      // const materials::material & my_dummy_metal2 = my_material_manager.get_unaliased_material("dummy_metal");
      // my_dummy_metal2.tree_dump(std::clog, "The unaliased dummy metal alias is : ");
    }
    std::clog << std::endl;

  } catch(std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch(...) {
    std::cerr << "ERROR: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return(error_code);
}

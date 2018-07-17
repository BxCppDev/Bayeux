// -*- mode: c++; -*-
// test_factory.cxx

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

// Third Party:
// - Boost:
#include <boost/shared_ptr.hpp>

// This Project:
#include <datatools/factory_macros.h>

/**
 *
 *                     base_type  -> _g_datatools_factory_system_factory_register_
 *                         |
 *            +------------+-------------------+
 *            |                                |
 *  derived_1_type                       derived_2_type
 *   -> _g_system_factory_registration_        | -> _g_system_factory_registration_
 *                                             |
 *                                             |
 *                                     derived_2_1_type
 *
 */
using namespace std;

namespace my
{
  /// Base class
  class base_type
  {
  public:

    base_type();

    virtual ~base_type();

    virtual std::string to_string() const = 0;

    // Factory stuff :
    DATATOOLS_FACTORY_SYSTEM_REGISTER_INTERFACE(base_type)

  };

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_type, "my::base_type/__system__")

  base_type::base_type()
  {
    std::cout << "base_type: construction..." << std::endl;
    return;
  }

  base_type::~base_type()
  {
    std::cout << "base_type: destruction." << std::endl;
    return;
  }

  /***************************************************************************/

  /// Derived class #1
  struct derived_1_type : public base_type
  {
    derived_1_type()
    {
      std::cout << "derived_1_type: construction..." << std::endl;
    }
    virtual ~derived_1_type()
    {
      std::cout << "derived_1_type: destruction." << std::endl;
    }
    virtual std::string to_string() const
    {
      return "derived_1_type";
    }

    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_type, derived_1_type)
  };

  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_type, derived_1_type, "my::derived_1_type")

  /***************************************************************************/

  /// Derived class #2
  struct derived_2_type : public base_type
  {
    derived_2_type()
    {
      std::cout << "derived_2_type: construction..." << std::endl;
    }
    virtual ~derived_2_type()
    {
      std::cout << "derived_2_type: destruction." << std::endl;
    }
    virtual std::string to_string() const
    {
      return "derived_2_type";
    }
    DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(base_type, derived_2_type)
  };

  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(base_type, derived_2_type, "my::derived_2_type")

  /***************************************************************************/

  /// Derived class #2/#1
  struct derived_2_1_type : public base_type
  {
    derived_2_1_type()
    {
      std::cout << "derived_2_1_type: construction..." << std::endl;
    }
    virtual ~derived_2_1_type()
    {
      std::cout << "derived_2_1_type: destruction." << std::endl;
    }
    virtual std::string to_string() const
    {
      return "derived_2_1_type";
    }
  };

} // end of namespace my;


int main(int /*argc_*/ , char ** /*argv_*/)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'factory' template class." << std::endl;
    // bool verbose = false;

    // int iarg =  1;
    // while (iarg < argc_)
    //   {
    //     std::string arg = argv_[iarg];

    //     if ((arg == "-v") || (arg == "--verbose")) verbose = true;

    //     iarg++;
    //   }

    // Print the contents of the system factory register :
    DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(my::base_type).print(std::clog, "NOTICE: ");
    //my::base_type::get_system_factory_register ().print (std::clog, "NOTICE: ");

    // Declare a factory register :
    my::base_type::factory_register_type the_factory_register("my::base_type/test",
                                                              my::base_type::factory_register_type::verbose);
    the_factory_register.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(my::base_type));
    //the_factory_register.import (my::base_type::get_system_factory_register ());

    // Clear the system factory register :
    DATATOOLS_FACTORY_GRAB_SYSTEM_REGISTER(my::base_type).clear();

    the_factory_register.registration("my::derived_2_1_type",
                                      boost::factory<my::derived_2_1_type*>(),
                                      typeid(my::derived_2_1_type));
    the_factory_register.print(std::clog, "NOTICE: ");

    typedef std::map<std::string, std::string> directives_list_type;
    directives_list_type factory_directives;
    factory_directives["obj1"] = "my::derived_1_type";
    factory_directives["obj2"] = "my::derived_1_type";
    factory_directives["obj3"] = "my::derived_2_type";
    factory_directives["obj4"] = "my::derived_2_1_type";
    factory_directives["obj5"] = "my::derived_1_type";
    factory_directives["obj6"] = "my::derived_2_type";

    typedef boost::shared_ptr<my::base_type> object_smart_ptr_type;
    typedef std::map<std::string, object_smart_ptr_type> object_map_type;
    object_map_type objects;

    std::clog << "Creating objects..." << std::endl;
    for (directives_list_type::const_iterator i
           = factory_directives.begin();
         i != factory_directives.end();
         i++) {
      const std::string & obj_name = i->first;
      const std::string & obj_type_id = i->second;
      if (! the_factory_register.has(obj_type_id)) {
        std::cerr << "No registered class with ID '"
                  << obj_type_id << "' !" << std::endl;
      } else {
        const my::base_type::factory_register_type::factory_type & the_factory
          = the_factory_register.get(obj_type_id);
        object_smart_ptr_type tmp_ptr(the_factory());
        objects[obj_name] = tmp_ptr;
      }
    }

    std::clog << "Done." << std::endl;

    for (object_map_type::const_iterator i
           = objects.begin();
         i != objects.end();
         i++) {
      const std::string & obj_name = i->first;
      const my::base_type * obj_ptr = i->second.get();
      std::clog << "'" << obj_name << "' is a '" << obj_ptr->to_string() << "'" << std::endl;
    }

    std::clog << "End." << std::endl;
  } catch (std::exception & x) {
    std::clog << "error: " << x.what() << std::endl;
    error_code =  EXIT_FAILURE;
  } catch (...) {
    std::clog << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  std::clog << "End." << std::endl;
  return error_code;
}

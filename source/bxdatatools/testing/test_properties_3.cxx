// This Project:
#include <datatools/properties.h>

// Standard library:
#include <iostream>
#include <exception>

int main(void)
{
  try {
    {
      datatools::properties my_bag("A list of properties");
      // Populate the *bag* of properties:
      my_bag.store_flag("debug", "The debug status"); // add a boolean value and set it at *true*
      my_bag.store("nevent", 10000, "The number of events"); // add an integer value
      my_bag.store("pi", 3.14159, "The PI constant"); // add a double precision floatting value
      my_bag.lock("pi");                              // make it non-mutable
      my_bag.store("James_Bond", "007", "A famous spy !");
      datatools::write_config("my_bag.conf", my_bag);
    }

    {
      datatools::properties my_bag;
      datatools::read_config("my_bag.conf", my_bag);
      my_bag.dump(std::clog);
    }
    
  } catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
  }
  return 0;
}

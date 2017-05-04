#include <iostream>
#include <vector>
#include <datatools/properties.h>

int main (void) {
  {
    datatools::properties config;

    // Default value is set at 'true' for this boolean property :
    config.store_flag ("debug", "Activation of debugging feature");

    // Set explicitely the values of some boolean properties :
    config.store ("test", false);
    config.store_boolean ("another", true);

    if (config.is_boolean ("test")) {
      std::cout << "Boolean 'test' exists !" << std::endl;
    }

    if (config.has_flag ("debug")) {
      std::cout << "Flag 'debug' is set !" << std::endl;
    }

    if (config.has_flag ("another")) {
      std::cout << "Flag 'another' is also set !" << std::endl;
    }

    if (! config.has_flag ("test")) {
      std::cout << "Flag 'test' is not set !" << std::endl;
    }

    if (config.has_key ("debug")) {
      bool debug = config.fetch_boolean ("debug");
      std::cout << "Fetched boolean value for 'debug' is : " << debug << std::endl;
    }

    {
      std::vector<bool> bits;
      bits.assign (4, true);
      bits[1] = false;
      config.store ("bits", bits, "An array of bits");
    }

    {
      std::vector<bool> bits;
      if (config.has_key ("bits") && config.is_vector ("bits")) {
        config.fetch ("bits", bits);
      }
      for (int i = 0; i < bits.size (); ++i) {
        std::clog << bits[i] << ' ';
      }
      std::clog << std::endl;
    }

    config.tree_dump (std::cout, "The properties container :");
  }

  return 0;
}

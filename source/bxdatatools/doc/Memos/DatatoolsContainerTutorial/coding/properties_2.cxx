#include <iostream>
#include <datatools/properties.h>
int main (void) {
  datatools::properties config ("A test properties container");
  config.store_flag ("test");      // store a boolean/flag property (true)
  config.store ("name", "Monty");  // store a string property
  config.tree_dump (std::cout, "Original container :"); // ASCII print
  std::cout << std::endl;

  datatools::properties  config2;
  config2 = config;
  config2.tree_dump (std::cout, "Properties container copied through assignment :");
  std::cout << std::endl;

  datatools::properties config3 (config);
  config3.tree_dump (std::cout, "Properties container copied at construction :");

  return 0;
}

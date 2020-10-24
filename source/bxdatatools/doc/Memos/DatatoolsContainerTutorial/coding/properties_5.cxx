#include <iostream>
#include <datatools/properties.h>

int main (void) {
  datatools::properties config;

  // read the container from an ASCII configuration file :
  datatools::read_config ("properties_5.conf",
                                      config);

  config.tree_dump (std::cout,
                    "The properties container built from a file :");
  return 0;
}

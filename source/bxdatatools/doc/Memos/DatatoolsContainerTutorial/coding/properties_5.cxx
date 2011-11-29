#include <iostream>

#include <datatools/utils/properties.h>

int main (void)
{
  datatools::utils::properties config;

  // read the container from an ASCII configuration file :
  datatools::utils::properties::read_config ("properties_5.conf",
					     config);

  config.tree_dump (std::cout, 
		      "The transcient properties container built from a file :"); 
  return 0;
}

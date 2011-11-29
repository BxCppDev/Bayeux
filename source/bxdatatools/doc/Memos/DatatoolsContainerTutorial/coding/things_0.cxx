#include <iostream>

#include <datatools/utils/things.h>
#include <datatools/utils/properties.h>

int main (void)
{
  datatools::utils::things bag;

  // Give it a description string :
  bag.set_description ("A test things container");

  // Add two properties objects (serializable) :
  bag.add<datatools::utils::properties> ("foo");
  bag.add<datatools::utils::properties> ("bar");

  bag.tree_dump (std::cout,
		 "A things container with 2 banks :");
  return 0;
}

#include <iostream>
#include <datatools/things.h>
#include <datatools/properties.h>

int main (void) {
  datatools::things bag;
  // Give it a description string :
  bag.set_description ("A test things container");
  // Add two properties objects (serializable) :
  bag.add<datatools::properties> ("foo");
  bag.add<datatools::properties> ("bar");
  // Print :
  bag.tree_dump (std::cout, "A things container with 2 banks :");
  return 0;
}

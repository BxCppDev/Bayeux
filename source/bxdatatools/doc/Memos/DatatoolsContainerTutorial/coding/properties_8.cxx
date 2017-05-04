#include <iostream>
#include <vector>
#include <datatools/properties.h>

int main (void) {
  datatools::properties config;
  // Set explicitely the values of some integer properties :
  config.store ("name", "King Arthur");
  config.store_string ("author", "Sir Robin's minstrel");
  try {
    config.store_string ("title", "Sir Robin \"The Brave\""); // this fails
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
  }
  config.tree_dump (std::cout, "The properties container :");
  return 0;
}

#include <iostream>
#include <datatools/multi_properties.h>

int main (void) {
  datatools::multi_properties mconfig;
  mconfig.set_description ("A test multi-properties container");
  mconfig.set_key_label ("name");
  mconfig.set_meta_label ("type");
  // Add new sections with unique key and associated meta strings :
  mconfig.add ("test", "foo");
  mconfig.add ("debug", "gnus");
  mconfig.add ("log", "gnats");
  mconfig.tree_dump (std::cout,
                     "A multi-properties container with 3 empty sections :");
  return 0;
}

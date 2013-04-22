#include <iostream>
#include <datatools/multi_properties.h>

int main (void) {
  datatools::multi_properties mconfig;
  mconfig.set_description ("A test multi-properties container");
  mconfig.set_key_label ("name");
  mconfig.set_meta_label ("type");
  return 0;
}

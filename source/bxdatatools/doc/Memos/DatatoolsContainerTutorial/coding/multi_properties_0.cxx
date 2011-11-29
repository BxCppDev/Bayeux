#include <iostream>

#include <datatools/utils/multi_properties.h>

int main (void)
{
  datatools::utils::multi_properties mconfig;

  mconfig.set_description ("A test multi-properties container");
  mconfig.set_key_label ("name");
  mconfig.set_meta_label ("type");

  return 0;
}

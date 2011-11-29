#include <iostream>

#include <datatools/utils/multi_properties.h>

int main (void)
{
  datatools::utils::multi_properties mconfig;

  mconfig.set_description ("A test multi-properties container");
  mconfig.set_key_label ("name");
  mconfig.set_meta_label ("type");

  // Add new sections with unique key and associated meta strings :
  mconfig.add ("test", "foo");
  mconfig.add ("debug", "gnus");
  mconfig.add ("log", "gnats");

  datatools::utils::properties config;
  config.store_flag ("dummy");
  config.store ("tmpdir", "/tmp");
  mconfig.add ("config", "blah", config);
  
  mconfig.tree_dump (std::cout, 
		     "A multi-properties container with 4 sections :");
  return 0;
}

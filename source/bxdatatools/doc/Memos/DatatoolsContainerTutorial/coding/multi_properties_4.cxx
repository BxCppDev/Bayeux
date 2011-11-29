#include <iostream>

#include <datatools/utils/multi_properties.h>

int main (void)
{
  {
    datatools::utils::multi_properties mconfig;

    mconfig.set_description ("A test multi-properties container");
    mconfig.set_key_label ("name");
    mconfig.set_meta_label ("type");

    // fill the multi-properties container : 
    mconfig.add ("test", "test::config");
    datatools::utils::properties & test_config 
      = mconfig.get ("test").get_properties ();
    test_config.store_flag ("using_excalibur");
    test_config.store ("number_of_witches", 1);
    test_config.update_string ("bird", "African swallow");
    test_config.update_string ("color", "blue");
    test_config.update_string ("quest", "The search for the Holly Grail");

    mconfig.add ("debug", "debug::config");
    datatools::utils::properties & debug_config 
      = mconfig.get ("debug").get_properties ();
    debug_config.store_flag ("active");
    debug_config.store ("level",3);
    debug_config.update_string ("filename", "/tmp/debug.out");

    mconfig.add ("math", "math::config");
    datatools::utils::properties & math_config 
      = mconfig.get ("math").get_properties ();
    math_config.store_real ("pi", 3.14159);
    math_config.store_real ("big", 1.e300);
    math_config.store_integer ("digit", 12);

    mconfig.write ("multi_properties_4.conf");
  }

  {
    datatools::utils::multi_properties mconfig;
    mconfig.read ("multi_properties_4.conf");
    mconfig.tree_dump (std::cout, 
		       "A multi-properties container restored from a file :");
  }
  return 0;
}

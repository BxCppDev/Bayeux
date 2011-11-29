#include <iostream>

#include <datatools/utils/multi_properties.h>

int main (void)
{
  datatools::utils::multi_properties mconfig;

  mconfig.set_description ("A test multi-properties container");
  mconfig.set_key_label ("name");
  mconfig.set_meta_label ("type");

  // Add a new section with unique key and associated meta strings :
  mconfig.add ("test", "foo");

  if (mconfig.has_key ("test"))
    {
      std::cout << "The multi-properties container has a section "
		<< "with key 'test'." << std::endl;
      // Update a string property in properties section 'test' :
      mconfig.get ("test").get_properties ().update_string ("bird", 
							    "African swallow");
      if (! mconfig.get ("test").get_properties ().has_key ("fruit"))
	{
	  // Store a new string property in properties section 'test' :
	  mconfig.get ("test").get_properties ().store_string ("fruit", 
							       "coconut");
	}
      // Finally, clear all properties in this section :
      mconfig.get ("test").get_properties ().clear ();
    }
 
  if (mconfig.has_key ("test"))
    {
      datatools::utils::properties & test_config 
	= mconfig.get ("test").get_properties ();

      test_config.store_flag ("using_excalibur");
      test_config.store ("number_of_witches", 1);
      test_config.update_string ("color", "blue");
      test_config.update_string ("quest", "The search for the Holly Grail");

      const datatools::utils::properties & const_test_config 
	= mconfig.get ("test").get_properties ();

      std::cout << "Number of properties in section 'test' : " 
		<< const_test_config.size () << std::endl;
    }

  mconfig.tree_dump (std::cout, 
		     "A multi-properties container with 1 section :");
  return 0;
}

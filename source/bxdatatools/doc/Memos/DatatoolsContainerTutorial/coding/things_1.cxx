#include <iostream>

#include <datatools/utils/things.h>
#include <datatools/utils/properties.h>

int main (void)
{
  datatools::utils::things bag;

  // Give it a description string :
  bag.set_description ("A test things container");

  // Add two properties objects (serializable) and
  // use the mutable reference returned by the 'add' method :

  // 'On the fly' usage of the returned reference :
  bag.add<datatools::utils::properties> ("foo").store_flag ("debug");

  // 'Off-line' usage of the returned reference :
  datatools::utils::properties & bar_ref = 
    bag.add<datatools::utils::properties> ("bar");
  // Now we are free to use the 'properties' class interface :
  bar_ref.store_flag ("test");
  bar_ref.store ("number_of_gate", 9);
  bar_ref.store ("pi", 3.14159);
  bar_ref.store ("name", "John Doe");

  // Check the availability of a 'properties' object
  // in the 'things' container and get :
  //   a) a non-mutable reference to it
  //   b) a mutable reference to it
  if (bag.has ("foo") && bag.is_a<datatools::utils::properties> ("foo"))
    {
      const datatools::utils::properties & const_foo_ref = 
	bag.get<datatools::utils::properties> ("foo");
      if (! const_foo_ref.has_key ("Devil"))
	{
	  datatools::utils::properties & foo_ref = 
	    bag.grab<datatools::utils::properties> ("foo");
	  foo_ref.store ("Devil", 666);
	}
   }

  bag.get<datatools::utils::properties> ("foo").tree_dump (std::cout,
							   "foo :");

  return 0;
}

#include <iostream>
#include <datatools/things.h>
#include <datatools/properties.h>

int main (void) {
  datatools::things bag;

  // Give it a description string :
  bag.set_description ("A test things container");

  // Add two properties objects (serializable) and
  // use the mutable reference returned by the 'add' method :

  // 'On the fly' usage of the returned reference :
  bag.add<datatools::properties> ("foo").store_flag ("debug");

  // 'Off-line' usage of the returned reference :
  datatools::properties & bar_ref =
    bag.add<datatools::properties> ("bar");
  // Now we are free to use the 'properties' class interface :
  bar_ref.store_flag ("test");
  bar_ref.store ("number_of_gate", 9);
  bar_ref.store ("pi", 3.14159);
  bar_ref.store ("name", "John Doe");

  // Check the availability of a 'properties' object
  // in the 'things' container and get :
  //   a) a non-mutable reference to it
  //   b) a mutable reference to it
  if (bag.has ("foo") && bag.is_a<datatools::properties> ("foo"))
    {
      const datatools::properties & const_foo_ref =
        bag.get<datatools::properties> ("foo");
      if (! const_foo_ref.has_key ("Devil"))
        {
          datatools::properties & foo_ref =
            bag.grab<datatools::properties> ("foo");
          foo_ref.store ("Devil", 666);
        }
      const_foo_ref.tree_dump (std::cout, "foo :");
   }

   return 0;
}

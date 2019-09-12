// This Project:
#include <datatools/properties.h>

// Standard library:
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>

int main (void)
{
  // Declare a *properties* object:
  datatools::properties my_bag("A list of properties");

  // Populate the *bag* of properties:
  my_bag.store_flag("debug");    // add a boolean value and set it at *true*
  my_bag.store("nevent", 10000); // add an integer value
  my_bag.store("pi", 3.14159, "The PI constant"); // add a double precision floatting value
  my_bag.lock("pi");                              // make it non-mutable
  std::vector<std::string> str_vec;
  str_vec.push_back("a");
  str_vec.push_back("b");
  my_bag.store("words", str_vec); // add an array of strings

  // A basic data object
  datatools::properties::data d;
  d.set_description("bare data object");
  d.set_value(1);
  my_bag.store("baredata", d);

  /* These are forbidden for the keys are not valid:
   * my_bag.store("#number", 666);        // starts with '#'
   * my_bag.store("", "Miss Moneypenny"); // empty key
   * my_bag.store("007", "James Bond");   // starts with a number
   */
  my_bag.store("James_Bond", "007");

  std::vector<double> position;
  position.push_back(1.0);
  position.push_back(2.0);
  position.push_back(-1.0);
  my_bag.store("position", position); // add an array of doubles

  // Manipulate properties within the bag:
  my_bag.change("position", -3.0, 1); // change the second value in the array
  my_bag.update("nevent", 100001);    // update an existing property or add it if missing
  my_bag.erase("words"); // remove a property
  /* This is forbidden for the 'pi' property is locked:
   * my_bag.change("pi", 3.14);
   */
  // Fetch property's values from the bag:
  if (my_bag.has_flag("debug")) {
    std::cout << "Debug mode !" << std::endl;
  }
  int nevent = my_bag.fetch_integer("nevent");
  std::cout << "nevent = " << nevent << std::endl;
  double pi = my_bag.fetch_real("pi");
  std::cout << "pi = " << pi << std::endl;
  if (! my_bag.has_key("words")) {
    std::cout << "Bag has no 'words' anymore !" << std::endl;
  }
  std::vector<double> translation;
  my_bag.fetch("position", translation);
  std::cout << "translation = "
            << translation[0] << ' '
            << translation[1] << ' '
            << translation[2] << std::endl;

  // Check that the baredata object exists and is an integer
  datatools::properties::data rd = my_bag.get("baredata");
  if (!rd.is_integer()) {
    throw std::runtime_error("baredata item is present but not integer type");
  }

  return 0;
}

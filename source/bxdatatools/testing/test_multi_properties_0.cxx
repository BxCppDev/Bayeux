// -*- mode: c++; -*-
// test_multi_properties_0.cxx

// This Project:
#include <datatools/multi_properties.h>
#include <datatools/multi_properties_config.h>

int main (int /* argc_ */, char ** /* argv_ */)
{
  using namespace std;
  {
    // Declare a *bag of bags*:
    datatools::multi_properties my_bag_of_bags ("name", "type", "A sample list of setups");

    // Add a new bag (*properties*) named 'display' within the bag of bags:
    my_bag_of_bags.add ("display", "display_type");
    // Retrieve a hook to the bag hosted by our *bag of bags*:
    datatools::multi_properties::entry & e1 = my_bag_of_bags.grab("display");
    // Add properties within this bag:
    e1.grab_properties ().store_flag ("debug");
    e1.grab_properties ().store ("colour", "blue");
    e1.grab_properties ().store ("depth", 1.234);

    if (my_bag_of_bags.get_section ("display").has_key ("colour")) {
      std::clog << "Display section has colour : "
           << my_bag_of_bags.get_section ("display").fetch_string ("colour")
           << endl;
    }

    // Add a new 'water' bag;
    my_bag_of_bags.add ("water", "material_type");
    datatools::multi_properties::entry & e2 = my_bag_of_bags.grab("water");
    e2.grab_properties ().store_flag ("liquid");
    e2.grab_properties ().store ("density", 1.0);
    e2.grab_properties ().store ("temperature", 300.0);
    e2.grab_properties ().store ("pressure", 1.0);
    e2.grab_properties ().store ("temperature.unit", "kelvin");
    e2.grab_properties ().store ("pressure.unit", "bar");

    // Add a new 'particle' bag;
    my_bag_of_bags.add ("shape", "shape_type");
    datatools::multi_properties::entry & e3 = my_bag_of_bags.grab("shape");
    e3.grab_properties ().store_flag ("visible");
    e3.grab_properties ().store ("type", "box");
    e3.grab_properties ().store ("material", "water");
    e3.grab_properties ().store ("length", 0.12);
    e3.grab_properties ().store ("width", 0.04);
    e3.grab_properties ().store ("depth", 0.06);

    // Remove a bag from the bag of bags;
    std::clog << "Remove section 'display'" << endl;
    my_bag_of_bags.remove ("display");

    my_bag_of_bags.grab_section ("shape").store ("author", "Pablo Picasso");

    // Save in ASCII file:
    my_bag_of_bags.write ("my_bag_of_bags.conf",
                          datatools::multi_properties_config::HEADER_FOOTER);
  }

  datatools::multi_properties copy_bag;
  {
    // Load from ASCII file:
    datatools::multi_properties my_bag_of_bags ("name", "type");
    my_bag_of_bags.read ("my_bag_of_bags.conf",
                         datatools::multi_properties_config::SKIP_PRIVATE_SECTIONS);

    // Print:

    std::clog << endl << "my_bag_of_bags :" << endl;
    my_bag_of_bags.print_tree(clog);
    copy_bag = my_bag_of_bags;
    datatools::multi_properties clone_bag = my_bag_of_bags;
    std::clog << endl << "Clone bag:" << endl;
    clone_bag.print_tree(std::clog);
  }
  std::clog << endl << "Copy bag:" << endl;
  copy_bag.print_tree(std::clog);
  return 0;
}

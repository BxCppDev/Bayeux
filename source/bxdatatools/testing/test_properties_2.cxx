// -*- mode: c++; -*-
// test_properties_2.cxx

// Standard Library:
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <map>

// This Project:
#include <datatools/properties.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test of the 'properties' class..." << std::endl;

    long seed = 12345;

    int iarg = 1;
    while(iarg < argc_) {
      std::string arg = argv_[iarg];
      iarg++;
    }

    srand48(seed);

    datatools::properties my_dict("a list of user properties");
    my_dict.dump(std::clog);
    my_dict.store("name","my name");
    my_dict.store("firstname","my firstname");
    my_dict.store(datatools::properties::make_private_key("private"),
                  "a private data", "Confidential stuff");
    my_dict.store("visibility.shown", true);
    my_dict.store("visibility.color", "red");
    my_dict.store("visibility.rendering", "shaded");
    my_dict.store("visibility.texture", "marble");
    my_dict.store("age", 24, "the age of the captain");
    std::vector<int> vi;
    vi.push_back(1);
    vi.push_back(2);
    vi.push_back(3);
    my_dict.store("vi", vi, "a vector of integers");
    std::vector<double> vd;
    vd.push_back(1.6e-19);
    vd.push_back(M_PI);
    vd.push_back(6.022e23);
    my_dict.store("vd", vd, "a vector of reals");
    std::vector<std::string> vs;
    vs.push_back("Hello");
    vs.push_back("world");
    vs.push_back("!");
    my_dict.store("vs", vs, "a vector of strings");
    my_dict.dump(std::clog);

    datatools::properties::keys_col_type all_keys;
    my_dict.keys(all_keys);

    datatools::properties::keys_col_type visibility_keys;
    my_dict.keys_starting_with(visibility_keys, "visibility.");
    datatools::properties::keys_col_type other_keys;
    my_dict.keys_not_starting_with(other_keys, "visibility.");

    std::clog << "All keys: " << std::endl;
    for (datatools::properties::keys_col_type::const_iterator i
          = all_keys.begin();
        i != all_keys.end();
        i++) {
      std::clog << " " << '"' << *i << '"' << std::endl;
    }
    std::clog << std::endl;

    std::clog << "Visibility keys: " << std::endl;
    for (datatools::properties::keys_col_type::const_iterator i
          = visibility_keys.begin();
        i != visibility_keys.end();
        i++) {
      std::clog << " " << '"' << *i << '"' << std::endl;
    }
    std::clog << std::endl;

    std::clog << "Other keys: " << std::endl;
    for (datatools::properties::keys_col_type::const_iterator i
          = other_keys.begin();
        i != other_keys.end();
        i++) {
      std::clog << " " << '"' << *i << '"' << std::endl;
    }

    datatools::properties my_dict2 = my_dict;
    my_dict2.erase_all_not_starting_with("visibility.");
    my_dict2.dump(std::clog);

    datatools::properties my_dict3 = my_dict;
    my_dict3.erase_all_starting_with("visibility.");
    my_dict3.dump(std::clog);

    datatools::properties my_dict4;
    my_dict.export_starting_with(my_dict4, "visibility.");
    my_dict4.dump(std::clog);

    datatools::properties my_dict5;
    my_dict.export_not_starting_with(my_dict5, "visibility.");
    my_dict5.dump(std::clog);

    //datatools::properties my_dict;
    //my_dict.export_starting_with(my_dict, "visibility.");

    std::map<std::string, std::string> string_based_dict;
    my_dict.export_to_string_based_dictionary(string_based_dict);

    std::clog << std::endl << "Exported string-based dictionary:" << std::endl;
    for (std::map<std::string, std::string>::const_iterator i = string_based_dict.begin();
        i != string_based_dict.end();
        i++) {
      std::clog << i->first << " = " << i->second << std::endl;
    }
    
  } catch (std::exception & x) {
    std::clog << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

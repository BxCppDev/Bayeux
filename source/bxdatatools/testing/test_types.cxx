// datatools test_types.cxx program

// Standard Library:
#include <iostream>
#include <string>

// This project:
#include <datatools/types.h>

int main (void)
{
  std::string bool_lbl = datatools::get_label_from_type(datatools::TYPE_BOOLEAN);
  std::clog << "Boolean label = '" << bool_lbl << "' \n";

  std::string str_lbl = datatools::get_label_from_type(datatools::TYPE_STRING);
  std::clog << "String label  = '" << str_lbl << "' \n";

  int str_id = datatools::get_type_from_label("string");
  std::clog << "String ID     = " << str_id << " \n";

  return 0;
}

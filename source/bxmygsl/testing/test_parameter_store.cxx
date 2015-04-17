// test_parameter_store.cxx

// Ourselves:
#include <mygsl/parameter_store.h>

// Standard library:
#include <cstdlib>
#include <iostream>

int main(/* int argc_ , char ** argv_ */)
{
  try {

    mygsl::parameter_store ps;
    int a_idx = ps.add("a", datatools::TYPE_REAL, "A");
    int b_idx = ps.add("b", datatools::TYPE_REAL, "B");
    int n_idx = ps.add("n", datatools::TYPE_INTEGER, "N");
    ps.tree_dump(std::clog, "Parameter store: ");

    ps.grab("a").set_real_value(2.31);
    ps.grab("b").set_real_value(0.53);
    ps.grab("n").set_integer_value(2);
    ps.tree_dump(std::clog, "Parameter store: ");

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

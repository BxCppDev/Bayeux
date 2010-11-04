#include <datatools/utils/properties.h>

using namespace std;

int main (void)
{
  // Declare a *properties* object:
  datatools::utils::properties my_bag;

  datatools::utils::properties::read_config ("my_bag.conf", my_bag);

  my_bag.dump (cout);

  return 0;
}


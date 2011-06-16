//

#include <iostream>
#include <exception>

#include <Ac1.hpp>
#include <Ac2.hpp>
#include <Ad1.hpp>
#include <Bc3.hpp>
#include <Bdd1.hpp>

/* If we include one or both of these files (despite we don't need any
serialization code here), it executes without crashing : */

//#include <Aio.cpp> 
//#include <Bio.cpp>

int main (void)
{
  try
    {
      A::c1 o1 (666);
      std::cout << "o1 : " << o1 << std::endl;

      A::c2 o2 ("test");
      std::cout << "o2 : " << o2 << std::endl;

      A::d1 w1 (9, 1000);
      std::cout << "w1 : " << w1 << std::endl;

      B::c3 o3 (7);
      std::cout << "o3 : " << o3 << std::endl;

      B::dd1 v3 (7, 3);
      std::cout << "v3 : " << v3 << std::endl;

    }
  catch (std::exception &x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}

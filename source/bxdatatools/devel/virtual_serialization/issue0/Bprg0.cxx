//

#include <iostream>
#include <exception>

#include <Ac1.hpp>
#include <Ac2.hpp>
#include <Bc3.hpp>

int main (void)
{
  try
    {
      A::c1 o1 (666);
      std::cout << "o1 is " << o1 << std::endl;

      A::c2 o2 ("test");
      std::cout << "o2 is " << o2 << std::endl;

      B::c3 o3 (7);
      std::cout << "o3 is " << o3 << std::endl;

    }
  catch (std::exception & x)
    {
      std::cerr << "Bprg0: error: " << x.what () << std::endl;
      return 1;
    }
  return 0;
}

// test_constants.cxx

#include <cstdlib>
#include <iostream>
#include <cmath>

#include <mygsl/constants.h>

int main(int argc_ , char ** argv_)
{
  try {
    double me_keV = 
      mygsl::constants::mksa::mass_electron 
      * std::pow (mygsl::constants::mksa::speed_of_light, 2) 
      / (mygsl::constants::num::kilo * mygsl::constants::mksa::electron_volt);
    
    std::cout.precision (10);
    std::cout << "me.c^2 = "  << me_keV << " keV" << std::endl; 
    
  }
  catch (std::exception & x){
    std::cerr << "CONSTANTS: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

// end of test_constants.cxx

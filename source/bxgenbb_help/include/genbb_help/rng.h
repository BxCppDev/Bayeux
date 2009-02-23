// -*- mode: c++; -*- 
/* rng.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2009-01-19
 * 
 * License: 
 * 
 * Description: 
 *   Manager for GENBB input data
 * 
 * History: 
 * 
 */

#ifndef __genbb_help__rng_h

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <mygsl/rng.h>

namespace genbb {

  struct rng
  {
    static mygsl::rng g__ran;
  };

} // end of namespace genbb

extern "C"
{
  int rng_shoot_reset (int seed_);
  float rng_shoot_flat ();
}

#endif // __genbb_help__rng_h

// end of genbb_mgr.h

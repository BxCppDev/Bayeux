// -*- mode: c++; -*- 
/* genbb_mgr.cc
 */

#include <genbb_help/rng.h>

namespace genbb {

  mygsl::rng rng::g__ran ("mt19937", 0);
 
} // end of namespace genbb

int rng_shoot_reset (int seed_)
{
  genbb::rng::g__ran.init ("mt19937", seed_);
  return seed_;
}

float rng_shoot_flat ()
{
  return (float) genbb::rng::g__ran.uniform ();
}

// end of genbb_mgr.cc

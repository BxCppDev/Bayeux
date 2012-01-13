// -*- mode: c++; -*- 
// rng.cc
/*
 * Copyright 2007-2011 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Publi * License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Publi * License for more details.
 * 
 * You should have received a copy of the GNU General Publi * License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 */

#include <genbb_help/rng.h>
 
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>

namespace genbb {
  
  // static :
  const std::string rng::DEFAULT_PRNG_ID = "taus2";

  mygsl::rng   rng::g_ran (rng::DEFAULT_PRNG_ID, 0);
  mygsl::rng * rng::g_ran_ptr = 0;

  // static :
  void rng::set_genbb_external_prng (mygsl::rng & prng_)
  {
    rng::g_ran_ptr = &prng_;
    return;
  }
  
} // end of namespace genbb

int32_t rng_shoot_reset (int32_t * seed_)
{
  if (genbb::rng::g_ran_ptr != 0)
    {
      std::clog << "WARNING: " 
		<< "genbb::rng_shoot_reset: " 
		<< "External PRNG does not use this local seed :" << std::endl;
      return -1;
    }
  std::clog << "NOTICE: " 
	    << "genbb::rng_shoot_reset: " 
	    << "Initialize the PRNG with seed '" << *seed_ << "'" <<  std::endl;
  genbb::rng::g_ran.init ("taus2", *seed_);
  return *seed_;
}

float rng_shoot_flat ()
{
  if (genbb::rng::g_ran_ptr != 0)
    {
      if (! genbb::rng::g_ran_ptr->is_initialized ())
	{
	  std::ostringstream message;
	  message << "genbb::rng_shoot_flat: "
		  << "External PRNG is not initialized !";
	  throw std::runtime_error (message.str ());
	}
      return (float) genbb::rng::g_ran_ptr->uniform ();
    }
  return (float) genbb::rng::g_ran.uniform ();
}

// end of rng.cc

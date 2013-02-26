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

#include <mygsl/rng.h>

namespace genbb {
  
  // static :
  const std::string rng::DEFAULT_PRNG_ID = "taus2";

  mygsl::rng * rng::g_ran_ptr = 0;

  mygsl::rng & rng::grab_genbb_external_prng ()
  {
    static boost::scoped_ptr<mygsl::rng> _g_prng_ (0);
    if (_g_prng_.get () == 0)
      {
        mygsl::rng * dummy_prng = new mygsl::rng; 
        {
          const char * c = getenv ("GENBB_PRNG_SEED");
          if (c != 0)
            {
              std::string s = c;
              std::istringstream iss (s);
              int seed = -1;
              iss >> seed;
              if (! iss)
                {
                  std::ostringstream message;
                  message << "genbb::rng::grab_genbb_external_prng: "
                          << "Invalid seed format from 'GENBB_PRNG_SEED' environment variable !";
                  throw std::logic_error (message.str ());
                }
              if (seed < 0)
                {
                  std::ostringstream message;
                  message << "genbb::rng::grab_genbb_external_prng: "
                          << "Invalid seed value (" << seed << ") from 'GENBB_PRNG_SEED' environment variable !";
                  throw std::logic_error (message.str ());
                }
              dummy_prng->initialize (rng::DEFAULT_PRNG_ID, seed);
            }
        }
        _g_prng_.reset (dummy_prng);
      }
    mygsl::rng & prng_ref = *_g_prng_.get ();
    return prng_ref;
  }

  const mygsl::rng & rng::get_genbb_external_prng ()
  {
    mygsl::rng& mutable_prng = rng::grab_genbb_external_prng ();
    return const_cast<mygsl::rng& > (mutable_prng);
  }

  void rng::init_genbb_external_prng (int seed_)
  {
    mygsl::rng & the_prng = grab_genbb_external_prng ();
    if (the_prng.is_initialized ())
      {
        the_prng.reset ();
      }
    the_prng.init (rng::DEFAULT_PRNG_ID, seed_);
    return;
  }

  void rng::reset_genbb_external_prng (int seed_)
  {
    mygsl::rng & the_prng = grab_genbb_external_prng ();
    if (the_prng.is_initialized ())
      {
        the_prng.reset ();
      }
    return;
  }

  // static :
  void rng::set_genbb_external_prng (mygsl::rng & prng_)
  {
    rng::g_ran_ptr = &prng_;
    return;
  }
  
} // end of namespace genbb


/* The following function is invoked from Fortran
 * using special mangling name.
 */

extern "C"
{
  int32_t rng_shoot_reset (int32_t * seed_)
  {
    if (genbb::rng::g_ran_ptr != 0)
      {
        std::clog << "WARNING: " 
                  << "genbb::rng_shoot_reset: " 
                  << "External PRNG does not use this local seed." << std::endl;
        return -1;
      }
    std::clog << "NOTICE: " 
              << "genbb::rng_shoot_reset: " 
              << "Initializing the static PRNG with seed '" << *seed_ << "'" <<  std::endl;
    genbb::rng::init_genbb_external_prng (*seed_);
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
            throw std::logic_error (message.str ());
          }
        return (float) genbb::rng::g_ran_ptr->uniform ();
      }
    return (float) genbb::rng::grab_genbb_external_prng ().uniform ();
  }
}

// end of rng.cc

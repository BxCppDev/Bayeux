// -*- mode: c++; -*-
/* rng.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2012-06-26
 *
 * License:
 * Copyright 2007-2012 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *   Manager for GENBB input data
 *
 * History:
 *
 */

#ifndef GENBB_HELP_DECAY0_FORTRAN_RNG_H_
#define GENBB_HELP_DECAY0_FORTRAN_RNG_H_

#include <string>

#include <boost/cstdint.hpp>

namespace mygsl {
  class rng;
}

namespace genbb {

  struct rng
  {
    static const std::string DEFAULT_PRNG_ID;

    static void set_genbb_external_prng (mygsl::rng & prng_);
    static mygsl::rng & grab_genbb_external_prng ();
    static const mygsl::rng & get_genbb_external_prng ();
    static void init_genbb_external_prng (int seed_);
    static void reset_genbb_external_prng (int seed_);

    static mygsl::rng * g_ran_ptr;

  };

} // end of namespace genbb

#include <genbb_help/decay0_fortran/__genbb_help_FC.h>

extern "C"
{
  int32_t rng_shoot_reset (int32_t * seed_);
  float rng_shoot_flat ();
}

#endif // GENBB_HELP_DECAY0_FORTRAN_RNG_H_

// end of genbb_mgr.h

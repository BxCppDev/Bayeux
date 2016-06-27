// -*- mode: c++ ; -*-
/* random_utils.cc
 *
 * Copyright 2007-2016 F. Mauger
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
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <mygsl/random_utils.h>

namespace mygsl {

  // static
  const int32_t random_utils::SEED_INVALID;
  const int32_t random_utils::SEED_AUTO;
  const int32_t random_utils::SEED_TIME;
  const int32_t random_utils::SEED_MAX;

  // static
  bool random_utils::is_valid_seed(int32_t seed_)
  {
    if (seed_ < SEED_TIME) return false;
    if (seed_ > SEED_MAX) return false;
    return true;
  }

} // end of namespace mygsl

// end of random_utils.cc

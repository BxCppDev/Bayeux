// -*- mode: c++; -*- 
/* rng.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-19
 * Last modified: 2009-01-19
 * 
 * License: 
 * Copyright 2007-2011 F. Mauger
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
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
  int32_t rng_shoot_reset (int32_t * seed_);
  float rng_shoot_flat ();
}

#endif // __genbb_help__rng_h

// end of genbb_mgr.h

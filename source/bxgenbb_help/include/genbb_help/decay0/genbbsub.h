// -*- mode: c++; -*-
/// \file genbb_help/decay0/genbbsub.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-05-05
 * Last modified: 2013-05-05
 *
 * License:
 * Copyright 2013 F. Mauger
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
 *
 *   Interface classes with GENBB/Decay0 generator
 *
 */

#ifndef GENBB_HELP_GENBBSUB_H_
#define GENBB_HELP_GENBBSUB_H_ 1

// Standard library:
#include <string>
#include <cmath>

namespace mygsl {
  class rng;
}
namespace genbb {
  class primary_event;
  namespace decay0 {

    // struct bkgpars {
    //   bkgpars();
    //   void reset();
    // };

    struct bbpars;

    enum genbbsub_i2bbs_type {
      GENBBSUB_I2BBS_DBD = 1, /// Double beta decay mode
      GENBBSUB_I2BBS_BACKGROUND = 2, /// Background/calibration decay mode
    };

    enum genbbsub_start_type {
      GENBBSUB_ISTART_INIT = -1, /// Initialize only
      GENBBSUB_ISTART_INIT_GENERATE_ONE = 0, /// Initialize and generate one event
      GENBBSUB_ISTART_GENERATE = 1 /// Generate one event
    };

    /// Main decay0 C++ port interface function
    void genbbsub(mygsl::rng & prng_,
                  primary_event & event_,
                  int i2bbs_,
                  const std::string & chnuclide_,
                  int ilevel_,
                  int modebb_,
                  int istart_,
                  int & ier_,
                  bbpars & bb_params_);

  } // end of namespace decay0
} // end of namespace genbb

#endif // GENBB_HELP_GENBBSUB_H_

// Local Variables: --
// mode: c++ --
// End: --

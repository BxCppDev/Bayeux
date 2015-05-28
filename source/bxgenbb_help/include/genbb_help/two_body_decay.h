/// \file genbb_help/two_body_decay.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-06-05
 * Last modified: 2014-06-05
 *
 * License:
 * Copyright 2014 F. Mauger
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
 *
 *   Utilities related to two body decays.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_TWO_BODY_DECAY_H
#define GENBB_HELP_TWO_BODY_DECAY_H 1

// This project:
#include <genbb_help/kinematics.h>

namespace genbb {

  /// \brief Description of a two body decay
  class two_body_decay
  {
  public:

    /// Default constructor
    two_body_decay();

    /// Constructor
    two_body_decay(double M_, double m1_, double m2_);

    /// Set the masses of the particles
    void set(double M_, double m1_, double m2_);

    /// Reset
    void reset();

    /// Check validity
    bool is_valid() const;

    /// Return the mass of the decaying particle
    double M() const;

    /// Return the mass of the first daughter particle
    double m1() const;

    /// Return the mass of the second daughter particle
    double m2() const;

    /// Return the kinematics data of the first daughter particle
    const kinematics & daughter1() const;

    /// Return the kinematics data of the second daughter particle
    const kinematics & daughter2() const;

  private:

    double     _M_;         //!< Mass of the parent particle
    kinematics _daughter1_; //!< Kinematics data of the first daughter particle
    kinematics _daughter2_; //!< Kinematics data of the second daughter particle

  };

} // end of namespace genbb

#endif // GENBB_HELP_TWO_BODY_DECAY_H

// Local Variables: --
// mode: c++ --
// End: --

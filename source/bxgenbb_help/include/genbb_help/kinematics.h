/// \file genbb_help/kinematics.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-06-06
 * Last modified: 2014-06-06
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
 *   Utilities related to kinematics.
 *
 * History:
 *
 */

#ifndef GENBB_HELP_KINEMATICS_H
#define GENBB_HELP_KINEMATICS_H 1

namespace genbb {

  /// \brief Relastivistic particle kinematics
  class kinematics
  {
  public:

    /// Default constructor
    kinematics();

    /// Return the mass
    double m() const;

    /// Return the total energy
    double w() const;

    /// Return the kinetic energy
    double k() const;

    /// Return the momentum
    double p() const;

    /// Return the beta=v/c=p/w factor
    double beta() const;

    /// Return the gamma=v/c factor
    double gamma() const;

    /// Return the velocity
    double v() const;

    /// Invalidate
    void reset();

    /// Initialize from mass and total energy
    void initialize_from_m_w(double m_, double w_);

    /// Initialize from mass and kinetic energy
    void initialize_from_m_k(double m_, double k_);

    /// Initialize from mass and momentum
    void initialize_from_m_p(double m_, double p_);

  private:

    double _m_; //!< Particle mass
    double _w_; //!< Total energy
    double _k_; //!< Kinetic energy
    double _p_; //!< Momentum

  };

} // end of namespace genbb

#endif // GENBB_HELP_KINEMATICS_H

// Local Variables: --
// mode: c++ --
// End: --

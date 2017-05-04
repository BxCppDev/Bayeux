//! \file   hpge/calorimeter_hit.h
//! \author Arnaud Chapon, François Mauger
//! \brief  Describe a calorimeter hit from a HPGe
//
// Copyright (c) 2015 by Arnaud Chapon <achapon@cerap.fr>
//                       and François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux/mctools.
//
// Bayeux/mctools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/mctools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/mctools. If not, see <http://www.gnu.org/licenses/>.

#ifndef HPGE_CALORIMETER_HIT_H
#define HPGE_CALORIMETER_HIT_H

// Third party:
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace hpge {

  //! \brief A HPGe calorimeter hit
  class calorimeter_hit : public geomtools::base_hit
  {
  public:

    //! Default constructor
    calorimeter_hit();

    //! Check the energy deposit
    bool has_energy_deposit() const;

    //! Set the energy deposit
    void set_energy_deposit(double);

    //! Return the energy deposit
    double get_energy_deposit();

    //! Increment the energy deposit
    void increment_energy_deposit(double);

    //! Reset the energy deposit
    void reset_energy_deposit();

    //! Check validity
    bool is_valid() const;

    //! Reset
    void reset();

  private:

    double _energy_deposit_; //!< Energy deposit

  };

} // namespace hpge

#endif // HPGE_CALORIMETER_HIT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

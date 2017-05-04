//  hpge/calorimeter_hit.cc
//! \author Arnaud Chapon, François Mauger
//! \brief  Describe the HPGe detector response
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

// Ourselves:
#include <hpge/calorimeter_hit.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

namespace hpge {

  calorimeter_hit::calorimeter_hit()
  {
    _energy_deposit_ = datatools::invalid_real();
    return;
  }

  bool calorimeter_hit::has_energy_deposit() const
  {
    return datatools::is_valid(_energy_deposit_);
  }

  void calorimeter_hit::set_energy_deposit(double ed_)
  {
    _energy_deposit_ = ed_;
    return;
  }

  void calorimeter_hit::reset_energy_deposit()
  {
    _energy_deposit_ = datatools::invalid_real();
    return;
  }

  double calorimeter_hit::get_energy_deposit()
  {
    return _energy_deposit_;
  }

  void calorimeter_hit::increment_energy_deposit(double de_)
  {
    DT_THROW_IF(de_ < 0.0, std::domain_error,
                "Invalid energy deposit increment!");
    if (! has_energy_deposit()) {
      _energy_deposit_ = 0.0;
    }
    _energy_deposit_ += de_;
    return;
  }

  bool calorimeter_hit::is_valid() const
  {
    return this->geomtools::base_hit::is_valid() && has_energy_deposit();
  }

  void calorimeter_hit::reset()
  {
    reset_energy_deposit();
    this->geomtools::base_hit::reset();
    return;
  }

} // namespace hpge

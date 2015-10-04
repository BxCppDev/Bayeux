// lahague/dosimetry/fluence_hit.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.
//

// Ourselves:
#include <lahague/dosimetry/fluence_hit.h>

// Third Party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/clhep_units.h>
#include <datatools/exception.h>

namespace lahague {

  namespace dosimetry {

    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(fluence_hit,
                                                      "lahague::dosimetry::fluence_hit")

    fluence_hit::fluence_hit()
    {
      fluence_hit::reset();
      return;
    }

    fluence_hit::~fluence_hit()
    {
      return;
    }

    void fluence_hit::reset ()
    {
      invalidate_particle_name();
      invalidate_entrance_energy();
      invalidate_h10_star();
      geomtools::base_hit::reset();
      return;
    }


    bool fluence_hit::is_valid() const
    {
      if (! geomtools::base_hit::is_valid ()) return false;
      if (_particle_name_.empty()) return false;
      if (!datatools::is_valid(_entrance_energy_)) return false;
      if (!datatools::is_valid(_h10_star_)) return false;
      return true;
    }

    void fluence_hit::invalidate()
    {
      fluence_hit::reset();
      return;
    }

    const std::string & fluence_hit::get_particle_name() const
    {
      return _particle_name_;
    }

    void fluence_hit::set_particle_name(const std::string & pname_)
    {
      _store |= STORE_PARTICLE_NAME;
      _particle_name_ = pname_;
      return;
    }

    void fluence_hit::invalidate_particle_name ()
    {
      _particle_name_ = "";
      _store &= ~STORE_PARTICLE_NAME;
      return;
    }

    double fluence_hit::get_entrance_energy() const
    {
      return _entrance_energy_;
    }

    void fluence_hit::set_entrance_energy(double ee_)
    {
      DT_THROW_IF(ee_ < 0.0, std::domain_error, "Invalid negative entrance energy!");
      _store |= STORE_ENTRANCE_ENERGY;
      _entrance_energy_ = ee_;
      return;
    }

    void fluence_hit::invalidate_entrance_energy ()
    {
      datatools::invalidate(_entrance_energy_);
      _store &= ~STORE_ENTRANCE_ENERGY;
      return;
    }

    double fluence_hit::get_h10_star() const
    {
      return _h10_star_;
    }

    void fluence_hit::set_h10_star(double h10_star_)
    {
      DT_THROW_IF(h10_star_ < 0.0, std::domain_error, "Invalid negative H*(10)!");
      _store |= STORE_H10_STAR;
      _h10_star_ = h10_star_;
      return;
    }

    void fluence_hit::invalidate_h10_star()
    {
      datatools::invalidate(_h10_star_);
      _store &= ~STORE_H10_STAR;
      return;
    }

    void fluence_hit::tree_dump(std::ostream & out_,
                                        const std::string & title_,
                                        const std::string & indent_,
                                        bool inherit_) const
    {
      this-> geomtools::base_hit::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Particle: '" << _particle_name_ << "'"
           << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Entrance energy: " << _entrance_energy_ / CLHEP::MeV << " MeV"
           << std::endl;

      const double picosievert = datatools::units::get_unit("picosievert");
      const double pSv_cm2 =  picosievert * CLHEP::cm2;
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "H*(10): " << _h10_star_ / pSv_cm2 << " pSv.cm2 "
           << std::endl;

      return;
    }

  } // namespace dosimetry

} // namespace lahague

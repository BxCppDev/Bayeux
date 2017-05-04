// mctools/mcnp/material/material.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <mctools/mcnp/material/material.h>

// Standard library:
#include <sstream>
#include <iomanip>
#include <list>

// Third party:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <materials/material.h>
#include <materials/element.h>
#include <materials/isotope.h>

// This project:
#include <mctools/mcnp/format.h>

namespace mctools {

  namespace mcnp {

    namespace material {

      nuclide_entry::nuclide_entry()
      {
        _z_ = INVALID_ZA;
        _a_ = INVALID_ZA;
        _fraction_mode_ = FRACTION_INVALID;
        _fraction_ = datatools::invalid_real();
        return;
      }

      nuclide_entry::nuclide_entry(int z_, int a_, double fraction_)
      {
        set_z(z_);
        set_a(a_);
        _a_ = a_;
        _fraction_mode_ = FRACTION_INVALID;
        _fraction_ = fraction_;
        return;
      }

      void nuclide_entry::reset()
      {
        _z_ = INVALID_ZA;
        _a_ = INVALID_ZA;
        _fraction_mode_ = FRACTION_INVALID;
        _fraction_ = datatools::invalid_real();
        return;
      }

      bool nuclide_entry::is_valid() const
      {
        if (_z_ < 1) return false;
        if (_a_ < 0) return false;
        if (_a_ > 0) {
          if (_fraction_mode_ == FRACTION_INVALID) return false;
          if (!datatools::is_valid(_fraction_)) return false;
        }
        return true;
      }

      bool nuclide_entry::is_natural() const
      {
        return _a_ == 0;
      }

      void nuclide_entry::set_z(int z_)
      {
        DT_THROW_IF(z_ < 1, std::domain_error, "Invalid nuclide Z=[" << z_ << "]!");
        _z_ = z_;
        return;
      }

      int nuclide_entry::get_z() const
      {
        return _z_;
      }

      void nuclide_entry::set_a(int a_)
      {
        DT_THROW_IF(a_ < 0, std::domain_error, "Invalid nuclide A=[" << a_ << "]!");
        _a_ = a_;
        return;
      }

      int nuclide_entry::get_a() const
      {
        return _a_;
      }

      bool nuclide_entry::is_mass_fraction() const
      {
        return _fraction_mode_ == FRACTION_MASS;
      }

      bool nuclide_entry::is_number_of_atoms_fraction() const
      {
        return _fraction_mode_ == FRACTION_NATOMS;
      }

      nuclide_entry::fraction_mode_type nuclide_entry::get_fraction_mode() const
      {
        return _fraction_mode_;
      }

      void nuclide_entry::set_fraction(double fraction_,
                                       fraction_mode_type fraction_mode_)
      {
        _fraction_mode_ = fraction_mode_;
        _fraction_ = fraction_;
        return;
      }

      double nuclide_entry::get_fraction() const
      {
        return _fraction_;
      }

      void nuclide_entry::print_card(std::ostream & out_, uint32_t flags_) const
      {
        DT_THROW_IF(!is_valid(), std::logic_error, "Invalid nuclide!");
        std::ostringstream outss;
        int z = get_z();
        int a = get_a();
        outss << std::right << std::setfill(' ') << std::setw(3) << z;
        outss << std::right << std::setfill('0') << std::setw(3) << a;
        outss << std::setfill(' ');
        outss << ' ';
        if (is_mass_fraction()) {
          outss << "-";
        } else {
          outss << " ";
        }
        outss << std::left << std::setfill(' ') << std::setw(20);
        outss.precision(12);
        outss << get_fraction();
        out_ << outss.str();
        if (flags_ & format::CARD_COMMENT) {
          out_ << " $ " << "Z=" << z;
          if (a != 0) {
            out_ << " " << "A=" << a;
          }
        }
        return;
      }

      // static
      const int material::INVALID_ID;
      const int material::MIN_ID;
      const int material::MAX_ID;

      material::material()
      {
        _locked_ = false;
        _id_ = INVALID_ID;
        return;
      }

      material::~material()
      {
        reset();
        return;
      }

      void material::reset()
      {
        _locked_ = false;
        _id_ = INVALID_ID;
        _name_.clear();
        _composition_.clear();
        return;
      }

      bool material::is_locked() const
      {
        return _locked_;
      }

      void material::lock()
      {
        _locked_ = true;
        return;
      }

      void material::unlock()
      {
        _locked_ = false;
        return;
      }

      void material::set_name(const std::string & name_)
      {
        _name_ = name_;
        return;
      }

      const std::string & material::get_name() const
      {
        return _name_;
      }

      void material::set_id(int id_)
      {
        DT_THROW_IF(id_ < MIN_ID || id_ > MAX_ID, std::range_error,
                    "Invalid material identifier [" << id_ << "]!");
        _id_ = id_;
        return;
      }

      int material::get_id() const
      {
        return _id_;
      }

      void material::make_natural_element(int z_)
      {
        DT_THROW_IF(is_locked(), std::logic_error, "Material is locked!");
        nuclide_entry ne;
        ne.set_z(z_);
        ne.set_a(0.0);
        ne.set_fraction(1.0, nuclide_entry::FRACTION_NATOMS);
        DT_THROW_IF(!ne.is_valid(), std::logic_error, "Invalid natural element entry!");
        DT_THROW_IF(_composition_.size(), std::logic_error, "Material already have a composition!");
        _composition_.push_back(ne);
        lock();
        return;
      }

      void material::add_nuclide(int z_,
                                 int a_,
                                 double fraction_,
                                 nuclide_entry::fraction_mode_type fraction_mode_,
                                 bool lock_)
      {
        std::cerr << "DEVEL: mcnp::material::add_nuclide: Entering..." << std::endl;
        DT_THROW_IF(is_locked(), std::logic_error, "Material is locked!");
        nuclide_entry ne;
        ne.set_z(z_);
        ne.set_a(a_);
        ne.set_fraction(fraction_, fraction_mode_);
        DT_THROW_IF(!ne.is_valid(), std::logic_error, "Invalid nuclide entry!");
        if (_composition_.size()) {
          const nuclide_entry & ne0 = _composition_[0];
          DT_THROW_IF(ne0.get_fraction_mode() != ne.get_fraction_mode(),
                      std::logic_error,
                      "Unmatching fraction mode!");
        }
        _composition_.push_back(ne);
        std::cerr << "DEVEL: mcnp::material::add_nuclide: Composition=" << _composition_.size() << std::endl;
        if (lock_) lock();
        std::cerr << "DEVEL: mcnp::material::add_nuclide: Exiting." << std::endl;
        return;
      }

      bool material::is_valid() const
      {
        if (_id_ == INVALID_ID) {
          std::cerr << "DEVEL: mcnp::material::is_valid: Invalid: id=" << _id_ << std::endl;
          return false;
        }
        if (_composition_.size() == 0) {
          std::cerr << "DEVEL: mcnp::material::is_valid: Invalid: composition=" << _composition_.size() << std::endl;
          return false;
        }
        return true;
      }

      void material::print_card(std::ostream & out_, uint32_t flags_) const
      {
        DT_THROW_IF(!is_valid(), std::logic_error, "Invalid material '" << _name_ << "'!");

        if (flags_ & format::CARD_COMMENT) {
          format::add_comment_line(out_, "Material: '" + get_name() + "'");
        }
        std::ostringstream outss;
        outss << "M"
              << std::left
              << std::setw(6)
              << std::setfill(' ') << _id_;
        for (int i = 0; i < (int) _composition_.size(); i++) {
          if (i == 0) {
            out_ << outss.str();
          } else {
            out_ << "       ";
          }
          const nuclide_entry & ne = _composition_[i];
          ne.print_card(out_, flags_);
          out_ << std::endl;
        }
        return;
      }

    } // namespace material

  } // namespace mcnp

} // namespace mctools

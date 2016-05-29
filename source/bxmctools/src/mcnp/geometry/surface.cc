//! \file  mctools/mcnp/geometry/surface.cc
//! \brief Description of MCNP surface
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
#include <mctools/mcnp/geometry/surface.h>

// Standard library:
#include <sstream>
#include <iomanip>

// Third party:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
#include <mctools/mcnp/format.h>
#include <mctools/mcnp/units.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      // static
      const int surface::INVALID_ID;
      const int surface::MIN_ID;
      const int surface::MAX_ID;

      surface::surface()
      {
        _id_ = INVALID_ID;
        _type_ = SURF_INVALID;
        _trait_ = TRAIT_NONE;
        return;
      }

      surface::~surface()
      {
        reset();
        return;
      }

      bool surface::is_valid() const
      {
        if (_id_ == INVALID_ID) {
          return false;
        }
        if (_type_ == SURF_INVALID) {
          return false;
        }
        if (_parameters_.size() == 0) {
          return false;
        }
        return true;
      }

      void surface::set_name(const std::string & name_)
      {
        _name_ = name_;
        return;
      }

      const std::string & surface::get_name() const
      {
        return _name_;
      }

      void surface::set_id(int id_)
      {
        DT_THROW_IF(id_ < MIN_ID || id_ > MAX_ID, std::range_error,
                    "Invalid surface identifier [" << id_ << "]!");
        _id_ = id_;
        return;
      }

      int surface::get_id() const
      {
        return _id_;
      }

      void surface::set_trait(trait_type t_)
      {
        _trait_ = t_;
        return;
      }

      surface::trait_type surface::get_trait() const
      {
        return _trait_;
      }

      bool surface::is_reflective() const
      {
        return _trait_ == TRAIT_REFLECTIVE;
      }

      void surface::set_reflective(bool r_)
      {
        if (r_) {
          set_trait(TRAIT_REFLECTIVE);
        } else {
          set_trait(TRAIT_NONE);
        }
        return;
      }

      bool surface::is_white_boundary() const
      {
        return _trait_ == TRAIT_WHITE_BOUNDARY;
      }

      void surface::set_white_boundary(bool wb_)
      {
        if (wb_) {
          set_trait(TRAIT_WHITE_BOUNDARY);
        } else {
          set_trait(TRAIT_NONE);
        }
        return;
      }

      surface_type surface::get_type() const
      {
        return _type_;
      }

      void surface::set_type(surface_type t_)
      {
        // std::cerr << "DEVEL: surface::set_type: type_ == [" << t_ << "] " << std::endl;
        _type_ = t_;
        return;
      }

      void surface::reset()
      {
        _parameters_.clear();
        _trait_ = TRAIT_NONE;
        _type_ = SURF_INVALID;
        _name_.clear();
        _id_ = INVALID_ID;
        return;
      }

      void surface::print_card(std::ostream & out_, uint32_t flags_) const
      {
        DT_THROW_IF(!is_valid(), std::logic_error, "Invalid surface '" << _name_ << "'!");
        // std::cerr << "DEVEL: surface::print_card: get_type() == [" << get_type() << "] " << std::endl;

        if (flags_ & ::mctools::mcnp::format::CARD_COMMENT) {
          ::mctools::mcnp::format::add_comment_line(out_, "Surface '" + get_name() + "' : " + surface_type_to_mcnp_label(get_type()) );
        }
        std::ostringstream outss;
        int hskip = 0;
        if (is_white_boundary()) {
          outss << '+';
          hskip = 1;
        } else if (is_reflective()) {
          outss << '*';
          hskip = 1;
        }
        outss << std::left
              << std::setw(7 - hskip)
              << std::setfill(' ')
              << _id_;
        out_ << outss.str();
        out_ << " ";
        out_ << surface_type_to_mcnp_label(get_type());
        if (flags_ & ::mctools::mcnp::format::CARD_COMMENT) {
          out_ << " $ " << surface_type_to_mcnp_description(get_type());
        }
        out_ << std::endl;
        for (int ipar = 0; ipar < (int) _parameters_.size(); ipar++) {
          const parameter_type & par = _parameters_[ipar];
          // if (ipar == 0) {
          //   out_ << outss.str();
          // } else {
            out_ << "      ";
          // }
          out_ << std::left << std::setfill(' ') << std::setw(20);
          out_.precision(12);
          if (par.value.type() == typeid(double)) {
            double unit = ::mctools::mcnp::units::get_default_unit_for_dimension(par.dimension);
            double value = boost::any_cast<double>(par.value) / unit;
            out_ << value;
          }
          if (flags_ & ::mctools::mcnp::format::CARD_COMMENT) {
            out_ << " $ " << par.name;
            if (!par.dimension.empty()) {
              out_ << " in MCNP " << par.dimension << " unit";
            }
          }
          out_ << std::endl;
        }
        return;
      }

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools

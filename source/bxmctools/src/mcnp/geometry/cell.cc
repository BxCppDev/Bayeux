//! \file  mctools/mcnp/geometry/cell.cc
//! \brief Description of MCNP cell
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
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
#include <mctools/mcnp/geometry/cell.h>

// Standard library:
#include <sstream>
#include <iomanip>

// Third party:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
#include <mctools/mcnp/format.h>
#include <mctools/mcnp/units.h>
#include <mctools/mcnp/utils.h>
#include <mctools/mcnp/geometry/surface.h>
#include <mctools/mcnp/material/material.h>
#include <mctools/mcnp/geometry/surface_type.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      // static
      const int cell::INVALID_ID;
      const int cell::MIN_ID;
      const int cell::MAX_ID;
      const int cell::MATERIAL_VOID;

      cell::surface_node_type::surface_node_type()
      {
        parent = 0;
        op = SURF_OP_INVALID;
        surface_id = surface::INVALID_ID;
        surface_ie = SURF_IE_INVALID;
        return;
      }

      bool cell::surface_node_type::is_valid() const
      {
        if (op != SURF_OP_INVALID) {
          if (op == SURF_OP_COMPLEMENT) {
            // A COMPLEMENT branch needs at least 1 children node:
            if (children.size() > 0) return true;
          } else {
            // abother branch needs at least 2 children node:
            if (children.size() > 1) return true;
          }
        } else if (surface_id != surface::INVALID_ID) {
          // A surface identifier is needed with a proper int/ext orientation:
          if (surface_ie != SURF_IE_INVALID) return true;
        }
        return false;
      }

      bool cell::surface_node_type::is_branch() const
      {
        return is_valid() && op != SURF_OP_INVALID;
      }

      bool cell::surface_node_type::is_leaf() const
      {
        return is_valid() && surface_id != surface::INVALID_ID;
      }

      void cell::surface_node_type::reset()
      {
        for (std::list<surface_node_type>::iterator inode = children.begin();
             inode != children.end();
             inode++) {
          inode->reset();
        }
        children.clear();
        parent = 0;
        op = SURF_OP_INVALID;
        surface_id = surface::INVALID_ID;
        surface_ie = SURF_IE_INVALID;
        return;
      }

      void cell::surface_node_type::set_leaf(int surface_id_, surface_int_ext_type surface_ie_)
      {
        surface_id = surface_id_;
        surface_ie = surface_ie_;
        op = SURF_OP_INVALID;
        children.clear();
        return;
      }

      void cell::surface_node_type::set_branch(surface_operator_type surface_op_)
      {
        surface_id = surface::INVALID_ID;
        surface_ie = SURF_IE_INVALID;
        op = surface_op_;
        return;
      }

      cell::surface_node_type &
      cell::surface_node_type::add_leaf(int surface_id_, surface_int_ext_type surface_ie_)
      {
        DT_THROW_IF(is_leaf(), std::logic_error, "Cannot add a daughter leaf in a leaf surface node!");
        DT_THROW_IF(is_branch() && op == SURF_OP_COMPLEMENT && children.size() > 0,
                    std::logic_error,
                    "Cannot add a daughter leaf in a branch complement surface node!");
        surface_node_type leaf;
        leaf.surface_id = surface_id_;
        leaf.surface_ie = surface_ie_;
        leaf.parent = this;
        children.push_back(leaf);
        return children.back();
      }

      cell::surface_node_type &
      cell::surface_node_type::add_branch(surface_operator_type op_)
      {
        DT_THROW_IF(is_leaf(), std::logic_error, "Cannot add a daughter branch in a leaf surface node!");
        surface_node_type branch;
        branch.op = op_;
        branch.parent = this;
        children.push_back(branch);
        return children.back();
      }

      void cell::surface_node_type::to_card(std::ostream & out_) const
      {
        if (is_leaf()) {
          if (surface_ie == SURF_IE_INTERIOR) {
            out_ << '-';
          }
          out_ << surface_id;
        } else {
          if (parent != 0) {
            out_ << '(';
          }
          char op_symbol = ' ';
          if (op == SURF_OP_UNION) {
            op_symbol = ':';
          } else if (op == SURF_OP_COMPLEMENT) {
            op_symbol = '#';
            out_ << op_symbol;
          }
          for (std::list<surface_node_type>::const_iterator ichild = children.begin();
               ichild != children.end();
               ichild++) {
            const surface_node_type & child_node = *ichild;
            child_node.to_card(out_);
            std::list<surface_node_type>::const_iterator last = ichild;
            last++;
            if (last != children.end()) {
              out_ << op_symbol;
            }
          }
          if (parent != 0) {
            out_ << ')';
          }
        }
        return;
      }

      void cell::surface_node_type::tree_dump(std::ostream & out_,
                                              int indent_level_) const
      {
        std::ostringstream indent_oss;
        for (int i = 0; i < indent_level_ - 1; i++) {
          indent_oss << "    ";
        }
        if (indent_level_ != 0) {
          indent_oss << " +- ";
        }
        std::string indent = indent_oss.str();
        out_ << indent;
        if (is_leaf()) {
          out_ << "Surface[" << surface_id << "]\n";
        } else if (is_branch()) {
          out_ << "Op";
          if (op == SURF_OP_INTERSECTION) {
            out_ << "*";
          } else if (op == SURF_OP_UNION) {
            out_ << "+";
          } else {
            out_ << "#";
          }
          out_ << "(" << children.size() << ")";
          out_ << "\n";
          for (std::list<surface_node_type>::const_iterator ichild = children.begin();
               ichild != children.end();
               ichild++) {
            const surface_node_type & child_node = *ichild;
            child_node.tree_dump(out_, indent_level_+1);
          }
        } else {
          out_ << "!\n";
        }

        return;
      }

      void cell::_set_defaults()
      {
        _id_ = INVALID_ID;
        _like_id_ = INVALID_ID;
        _material_id_ = MATERIAL_VOID;
        _material_density_ = datatools::invalid_real();
        return;
      }

      cell::cell()
      {
        _set_defaults();
        return;
      }

      cell::~cell()
      {
        reset();
        return;
      }

      void cell::set_name(const std::string & name_)
      {
        _name_ = name_;
        return;
      }

      const std::string & cell::get_name() const
      {
        return _name_;
      }

      void cell::set_id(int id_)
      {
        DT_THROW_IF(id_ < MIN_ID || id_ > MAX_ID, std::range_error,
                    "Invalid cell identifier [" << id_ << "]!");
        _id_ = id_;
        return;
      }

      int cell::get_id() const
      {
        return _id_;
      }

      bool cell::is_like() const
      {
        return _like_id_ != INVALID_ID;
      }

      void cell::set_like_id(int like_id_)
      {
        DT_THROW_IF(like_id_ < MIN_ID || like_id_ > MAX_ID, std::range_error,
                    "Invalid cell identifier [" << like_id_ << "]!");
        _like_id_ = like_id_;
        return;
      }

      int cell::get_like_id() const
      {
        return _like_id_;
      }

      void cell::set_material_id(int mid_)
      {
        _material_id_ = mid_;
        return;
      }

      int cell::get_material_id() const
      {
        return _material_id_;
      }

      void cell::set_material_density(double d_)
      {
        _material_density_ = d_;
        return;
      }

      double cell::get_material_density() const
      {
        return _material_density_;
      }

      bool cell::is_void() const
      {
        return _material_id_ == MATERIAL_VOID;
      }

      const cell::surface_node_type &
      cell::get_surface_node() const
      {
        return _surface_node_;
      }

      cell::surface_node_type &
      cell::grab_surface_node()
      {
        return _surface_node_;
      }

      void cell::reset()
      {
        _surface_node_.reset();
        _name_.clear();
        _set_defaults();
        return;
      }

      bool cell::is_valid() const
      {
        if (_id_ == INVALID_ID) {
          return false;
        }
        if (_material_id_ >= material::material::MIN_ID && _material_id_ <= material::material::MAX_ID) {
          if (!datatools::is_valid(_material_density_)) {
            return false;
          }
        }
        return true;
      }

      void cell::print_card(std::ostream & out_, uint32_t flags_) const
      {
        DT_THROW_IF(!is_valid(), std::logic_error, "Invalid cell '" << _name_ << "'!");

        if (flags_ & format::CARD_COMMENT) {
          format::add_comment_line(out_, "Cell '" + get_name() + "'");
        }
        std::ostringstream outss;
        int hskip = 0;
        outss << std::left
              << std::setw(7 - hskip)
              << std::setfill(' ')
              << _id_;
        out_ << outss.str();
        out_ << ' ';
        if (is_like()) {
          out_ << "LIKE " << _like_id_ << " BUT ";
        } else {
          out_ << _material_id_;
          out_ << ' ';
          if (_material_id_ != MATERIAL_VOID) {
            out_ << -1.0 * _material_density_ / mcnp::units::density_unit();
            out_ << ' ';
          }
          _surface_node_.to_card(out_);
        }
        out_ << std::endl;

        return;
      }

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools

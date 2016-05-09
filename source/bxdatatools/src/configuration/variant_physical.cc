// datatools/configuration/variant_physical.cc
/*
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

// Ourselves:
#include <datatools/configuration/variant_physical.h>

// This project:
#include <datatools/configuration/variant_model.h>

namespace datatools {

  namespace configuration {

    // virtual
    bool variant_physical::is_name_valid(const std::string & name_) const
    {
      // std::cerr << "DEVEL: variant_physical::is_name_valid: name = '" << name_ << "'" << std::endl;
      return ::datatools::configuration::validate_instance_name(name_);
    }

    variant_physical::variant_physical()
    {
      return;
    }

    variant_physical::~variant_physical()
    {
      return;
    }

    void variant_physical::set(const std::string & name_,
                               const std::string & desc_,
                               const vm_handle_type & variant_model_handle_)
    {
      set_name(name_);
      set_terse_description(desc_);
      _model_ = variant_model_handle_;
      return;
    }

    const variant_model & variant_physical::get_model() const
    {
      return _model_.get();
    }

    const vm_handle_type & variant_physical::get_model_handle() const
    {
      return _model_;
    }

    void variant_physical::tree_dump(std::ostream & out_,
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
    {
      this->enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_)
           << "Variant model : ";
      if (_model_) {
        out_ << get_model().get_name();
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;
      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools

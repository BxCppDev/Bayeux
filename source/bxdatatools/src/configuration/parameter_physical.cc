// datatools/configuration/parameter_physical.cc
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
#include <datatools/configuration/parameter_physical.h>

// This project:
#include <datatools/configuration/parameter_model.h>

namespace datatools {

  namespace configuration {

    parameter_physical::parameter_physical()
    {
      return;
    }

    parameter_physical::~parameter_physical()
    {
      return;
    }

    void parameter_physical::set(const std::string & name_,
                                 const std::string & desc_,
                                 const pm_handle_type & parameter_model_handle_)
    {
      set_name(name_);
      set_terse_description(desc_);
      _model_ = parameter_model_handle_;
      return;
    }

    const parameter_model & parameter_physical::get_model() const
    {
      return _model_.get();
    }

    const pm_handle_type & parameter_physical::get_model_handle() const
    {
      return _model_;
    }

    void parameter_physical::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
    {
      this->enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_)
           << "Parameter model : ";
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

/// \file datatools/configuration/variant_physical.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-05-11
 * Last modified : 2014-07-18
 *
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
 *
 * Description:
 *
 *   The physical instance of a configuration variant.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_PHYSICAL_H
#define DATATOOLS_CONFIGURATION_VARIANT_PHYSICAL_H

// Standard library:
#include <string>

// This project (Bayeux/datatools)
#include <datatools/i_tree_dump.h>
#include <datatools/configuration/utils.h>
#include <datatools/enriched_base.h>

namespace datatools {

  namespace configuration {

    // Forward declaration:
    class variant_model;

    /// \brief Physical variant physical
    class variant_physical : public datatools::enriched_base
    {
    public:

      /// Check if a name is valid
      bool is_name_valid(const std::string & name_) const override;

      /// Default constructeur
      variant_physical();

      /// Destructeur
      ~variant_physical() override;

      /// Set the physical
      void set(const std::string & name_,
               const std::string & desc_,
               const vm_handle_type & variant_model_handle_);

      /// Return the model of the variant
      const variant_model & get_model() const;

      /// Return the handle on the model of the variant
      const vm_handle_type & get_model_handle() const;

      /// Smart print
      void tree_dump(std::ostream& out_ = std::clog,
                             const std::string& title_  = "",
                             const std::string& indent_ = "",
                             bool inherit_ = false) const override;

    private:

      vm_handle_type _model_; //!< Model of the variant

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_VARIANT_PHYSICAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

/// \file datatools/configuration/parameter_physical.h
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
 * Description:
 *
 *   The physical instance of a configuration parameter.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_PARAMETER_PHYSICAL_H
#define DATATOOLS_CONFIGURATION_PARAMETER_PHYSICAL_H

// Standard library:
#include <string>
#include <memory>

// Third party:
// #include <boost/shared_ptr.hpp>

// This project (Bayeux/datatools)
#include <datatools/i_tree_dump.h>
#include <datatools/configuration/utils.h>
#include <datatools/enriched_base.h>

namespace datatools {

  namespace configuration {

    // Forward declaration:
    class parameter_model;
    class i_occurrence;

    /// \brief Physical parameter physical
    class parameter_physical : public datatools::enriched_base
    {
    public:

      /// Check if a name is valid
      bool is_name_valid(const std::string & name_) const override;

      /// Default constructeur
      parameter_physical();

      /// Destructeur
      ~parameter_physical() override;

      /// Reset
      void reset();

      /// Set the physical
      void set(const std::string & name_,
               const std::string & desc_,
               const pm_handle_type & parameter_model_handle_);

      /// Return the model of the parameter
      const parameter_model & get_model() const;

      /// Return the handle on the model of the parameter
      const pm_handle_type & get_model_handle() const;

      /// Smart print
      void tree_dump(std::ostream& out_ = std::clog,
                             const std::string& title_  = "",
                             const std::string& indent_ = "",
                             bool inherit_ = false) const override;

      /// Check occurrence
      bool has_occurrence() const;

      /// Return the occurrence of the parameter
      const i_occurrence & get_occurrence() const;

      /// Install an occurrence object
      void install_occurrence(const std::string & occurrence_def_);

    private:

      pm_handle_type _model_; //!< Model of the parameter
      std::shared_ptr<i_occurrence> _occurrence_; //!< Occurrence

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_PARAMETER_PHYSICAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

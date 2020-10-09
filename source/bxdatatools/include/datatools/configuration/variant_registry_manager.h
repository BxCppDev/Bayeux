/// \file datatools/configuration/variant_registry_manager.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-05-11
 * Last modified : 2014-10-27
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
 *   The configuration variant registry manager/factory.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_MANAGER_H
#define DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_MANAGER_H

// Standard library:
#include <string>
#include <set>
#include <vector>
#include <map>

// This project (Bayeux/datatools):
#include <datatools/i_tree_dump.h>
#include <datatools/enriched_base.h>
#include <datatools/multi_properties.h>
#include <datatools/configuration/utils.h>
#include <datatools/logger.h>
#include <datatools/object_configuration_description.h>

namespace datatools {

  namespace configuration {

    /// \brief A Factory/manager for configuration variants and associated parameters
    class variant_registry_manager : public datatools::enriched_base
    {
    public:

      /// Default constructor
      variant_registry_manager();

      /// Destructor
      ~variant_registry_manager() override;

      /// Check initialization status
      bool is_initialized() const;

      /// Configuration and initialization from properties
      void initialize(const properties & config_);

      /// Bare initialization
      void initialize_simple();

      /// Reset
      void reset();

      /// Set the name of the top variant
      void set_top_variant_name(const std::string &);

      /// Return the name of the top variant
      const std::string & get_top_variant_name() const;

      /// Add a property prefix to be preserved configuration items
      void add_property_prefix(const std::string & prefix_);

      /// Return the array of exported property prefixes
      const std::set<std::string> & get_property_prefixes() const;

      /// Loading a file that contains a list of configuration items definition filenames :
      void load_configuration_items_list(const std::string & item_list_file_);

      /// Load definitions of configuration items from a file with a stored multi-properties container
      void load(const std::string & items_config_file_);

      /// Get a non-mutable collection of configuration items
      const model_item_dict_type & get_configuration_items() const;

      /// Smart print
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

      /// OCD support
      static void init_ocd(datatools::object_configuration_description &);

    protected:

      /// Set default values
      void _set_default();

    private:

      /// Construct the virtual hierarchy of variants and parameters
      void _construct_();

    private:

      bool                   _initialized_;         //!< Initialization flag
      multi_properties       _mp_;                  //!< Multi-container for description of configuration model items
      std::set<std::string>  _property_prefixes_;   //!< List of prefixes for properties to be preserved in configuration items
      std::string            _top_variant_name_;    //!< The name of the top variant
      model_item_dict_type   _configuration_items_; //!< Dictionary of configuration model items

    };

  }  // end of namespace configuration

}  // end of namespace datatools

// OCD support:
#include <datatools/ocd_macros.h>

/// OCD registration
/// @arg datatools::configuration::variant_registry_manager the name of the class
DOCD_CLASS_DECLARATION(datatools::configuration::variant_registry_manager)

#endif // DATATOOLS_CONFIGURATION_VARIANT_REGISTRY_MANAGER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

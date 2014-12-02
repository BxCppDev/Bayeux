/// \file datatools/configuration/variant_model.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-05-11
 * Last modified : 2014-08-10
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
 *   The description of a configuration variant.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_VARIANT_MODEL_H
#define DATATOOLS_CONFIGURATION_VARIANT_MODEL_H

// Standard library:
#include <string>
#include <set>
#include <vector>
#include <map>

// This project (Bayeux/datatools):
#include <datatools/i_tree_dump.h>
#include <datatools/enriched_base.h>
#include <datatools/handle.h>
#include <datatools/types.h>
#include <datatools/configuration/utils.h>
#include <datatools/object_configuration_description.h>

namespace datatools {

  // Forward declaration:
  class multi_properties;

  namespace configuration {

    // Forward declaration:
    class parameter_model;
    class parameter_physical;

    /// \brief Model of a configuration variant
    class variant_model : public datatools::enriched_base
    {
    public:

      /// Dictionary of parameter physicals
      typedef std::map<std::string, parameter_physical> parameter_dict_type;
      typedef std::map<std::string, int> ranked_parameter_dict_type;

      /// Default constructor
      variant_model();

      /// Destructor
      virtual ~variant_model();

      /// Check if the documentation is empty
      bool has_documentation() const;

      /// Set the documentation
      void set_documentation(const std::string & documentation_);

      /// Return the documentation
      const std::string & get_documentation() const;

      /// Check if a parameter exists
      bool has_parameter(const std::string & name_) const;

      /// Add a new parameter
      void add_parameter(const std::string &    parameter_name_,
                         const pm_handle_type & parameter_model_handle_,
                         const std::string &    description_ = "",
                         const std::string &    occurence_def_ = "");

      /// Remove parameter
      void remove_parameter(const std::string & parameter_name_);

      // Check ranked parameter
      bool is_ranked_parameter(const std::string & parameter_name_) const;

      /// Return the rank of a parameter
      int get_parameter_rank(const std::string & parameter_name_) const;

      /// Set the rank of a parameter
      void set_parameter_rank(const std::string & parameter_name_, int rank_);

      /// Return a description string associated to a given parameter
      const std::string & get_parameter_description(const std::string & parameter_name_) const;

      /// Return a const reference to a parameter
      const parameter_model & get_parameter_model(const std::string & parameter_name_) const;

      /// Return a parameter model handle associated to a given parameter
      pm_handle_type get_parameter_model_handle(const std::string & parameter_name_) const;

      /// Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

      /// Print in ReST format
      void print_rst(std::ostream & out_, const std::string & indent_) const;

      /// Check initialization status
      bool is_initialized() const;

      /// Initialization from properties and a dictionary of configuration items
      void initialize(const properties & config_, item_dict_type & items_);

      /// Initialization from properties only
      void initialize_standalone(const properties & config_);

      /// Bare initialization
      void initialize_simple();

      /// Reset
      void reset();

      /// Return the dictionary of children parameters
      const parameter_dict_type & get_parameters() const;

      /// Return the dictionary of ranked parameters
      const ranked_parameter_dict_type & get_ranked_parameters() const;

      /// OCD support
      static void init_ocd(datatools::object_configuration_description &);

    protected:

      /// Set default values
      void _set_default();

    private:

      bool                _initialized_;   //!< Initialization flag
      std::string         _documentation_; //!< Documentation
      parameter_dict_type _parameters_;    //!< Dictionary of children parameters
      ranked_parameter_dict_type _ranked_parameters_; //!< Ranked dictionary of parameters

    };

  }  // end of namespace configuration

}  // end of namespace datatools

// OCD support:
#include <datatools/ocd_macros.h>

/// OCD registration
/// @arg datatools::configuration::variant the name of the class
DOCD_CLASS_DECLARATION(datatools::configuration::variant_model)

#endif // DATATOOLS_CONFIGURATION_VARIANT_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/

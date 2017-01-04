/// \file datatools/configuration/utils.h
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
 *   Some utilities for the description of configuration variants and parameters.
 *
 */

#ifndef DATATOOLS_CONFIGURATION_UTILS_H
#define DATATOOLS_CONFIGURATION_UTILS_H

// Standard library:
#include <string>
#include <map>

// Third party:
#include <boost/variant.hpp>

// This project (Bayeux/datatools):
#include <datatools/handle.h>
#include <datatools/properties.h>
#include <datatools/command_utils.h>

namespace datatools {

  /// \brief Nested namespace of the Bayeux/datatools module library
  namespace configuration {

    // Forward declarations:
    class parameter_model;
    class variant_repository;
    class variant_model;
    class model_item;

    /// Handle on variant model
    typedef handle<variant_model> vm_handle_type;

    /// Handle on parameter model
    typedef handle<parameter_model> pm_handle_type;

    /// Dictionary of items
    typedef std::map<std::string, model_item> model_item_dict_type;

    /// \deprecated
    /// \see validate_instance_name
    bool validate_item_name(const std::string &);

    /// Validate the name of an instance
    bool validate_instance_name(const std::string &);

    /// Validate the name of a model
    bool validate_model_name(const std::string &);

    /// \brief Configuration item with an embedded parameter or variant model handle through a smart pointer
    // class item {
    class model_item {
    public:

      /// \brief Type of item (parameter or variant)
      enum model_type {
        MODEL_UNDEFINED = 0, //!< Undefined model type
        MODEL_PARAMETER = 1, //!< Parameter model
        MODEL_VARIANT   = 2  //!< Variant model
      };

      /// Return the label associated to a item type
      static std::string label_from_model_type(model_type);

      /// Return the item type associated to label
      static model_type model_type_from_label(const std::string &);

      /// Return the conventional variant suffix
      static const std::string & variant_suffix();

      /// Return the conventional parameter suffix
      static const std::string & parameter_suffix();

      /// Default constructor
      model_item();

      /// Destructor
      virtual ~model_item();

      /// Reset the item
      void reset();

      /// Return the name
      const std::string & get_name() const;

      /// Set the name
      void set_name(const std::string & name_);

      /// Check if item is a parameter
      bool is_parameter() const;

      /// Check if item is a variant
      bool is_variant() const;

      /// Check if model is defined
      bool has_model() const;

      /// Set the type of the item
      void set_model(model_type);

      /// Return the type of the item
      model_type get_model() const;

      /// Return a non mutable reference to the list of configuration parameters
      const properties & get_config() const;

      /// Return a mutable reference to the list of configuration parameters
      properties & grab_config();

      /// Return a non mutable reference to the embedded parameter item
      const parameter_model & get_parameter_model() const;

      /// Return a mutable reference to the embedded parameter item
      parameter_model & grab_parameter_model();

      /// Return a non mutable reference to the embedded variant item
      const variant_model & get_variant_model() const;

      /// Return a mutable reference to the embedded variant item
      variant_model & grab_variant_model();

      /// Set the parameter item by handle
      void set_parameter_handle(const pm_handle_type &);

      /// Set the variant item by handle
      void set_variant_handle(const vm_handle_type &);

      /// Check if the item is instantiated
      bool is_instantiated() const;

      /// Instantiate the embedded item
      void instantiate(model_item_dict_type &);

      /// Destroy the embedded item
      void destroy();

      /// Return a non mutable handle to the embedded parameter
      const pm_handle_type & get_parameter_handle() const;

      /// Return a mutable handle to the embedded parameter
      pm_handle_type & grab_parameter_handle();

      /// Return a non mutable handle to the embedded variant
      const vm_handle_type & get_variant_handle() const;

      /// Return a mutable handle to the embedded variant
      vm_handle_type & grab_variant_handle();

      /// Create a new embedded parameter item
      parameter_model & make_parameter(const std::string & name_ = "");

      /// Create a new embedded variant item
      variant_model & make_variant(const std::string & name_ = "");

      /// Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

      // Methods to manipulate dictionaries of configuration items:

      /// Add a parameter item in a dictionary
      static parameter_model & add_parameter_item(model_item_dict_type & dict_,
                                                  const std::string & name_);

      /// Add a variant item in a dictionary
      static variant_model & add_variant_item(model_item_dict_type & dict_,
                                              const std::string & name_);

      /// Check if item with given name exists in a dictionary
      static bool has_item(const model_item_dict_type & dict_,
                           const std::string & name_,
                           model_item::model_type t_ = MODEL_UNDEFINED);

      /// Return a parameter model from a dictionary
      static const parameter_model & get_parameter_model(const model_item_dict_type & dict_,
                                                         const std::string & name_);

      /// Return a variant model from a dictionary
      static const variant_model & get_variant_model(const model_item_dict_type & dict_,
                                                     const std::string & name_);

      /// Remove an item from a dictionary
      static void remove_item(model_item_dict_type & dict_,
                              const std::string & name_);

      /// Smart print of a dictionary
      static void print_items(const model_item_dict_type & dict_,
                              std::ostream & out_ = std::clog,
                              const std::string & title_ = "",
                              const std::string & indent_ = "",
                              bool inherit_ = false);

    private:

      std::string    _name_;             //!< Name of the item
      properties     _config_;           //!< Configuration properties for the item
      model_type     _model_;            //!< Type of the model (parameter/variant)
      pm_handle_type _parameter_handle_; //!< Handle to an instantiated parameter model
      vm_handle_type _variant_handle_;   //!< Handle to an instantiated variant model

    };

    /// \brief A triplet of character strings representing a variant parameter set directive
    /** Typical format is:
     *  \code
     *   "{registry-name}:{path/to/the/variant/parameter}={value expression}"
     *  \endcode
     *
     * The datatools kernel accept special command line options that automatically set
     * the values of some variant parameters. Example:
     * \code
     * myapp \
     *   --datatools::variant-config="myapp/variance.conf" \
     *   --datatools::variant-set="myapp:foo=1" \
     *   --datatools::variant-set="myapp:foo/small_value/algo=rk4" \
     *   --datatools::variant-set="myapp:foo/small_value/epsilon=0.01 mm"
     *   ...
     * \endcode
     */
    struct variant_parameter_set_type {
      std::string registry_key;    //!< The registration key of the variant registry the parameter belongs to
      std::string param_key;       //!< The full path of the variant parameter of which the value should be modify
      std::string param_value_str; //!< The string to be parsed into the new value of the variant parameter
      variant_parameter_set_type();
      void reset();
      command::returned_info parse(const std::string & variant_set_);
    };

    /// \brief Comparator functor used to sort variant parameters by registry and path
    class variant_parameter_set_comparator {
    public:
      variant_parameter_set_comparator();
      variant_parameter_set_comparator(variant_repository &);
      bool operator()(const std::string & vs1_, const std::string & vs2_) const;
    private:
      variant_repository * _repository_ = nullptr;
    };

    typedef boost::variant<bool, int, double, std::string> parameter_value_type;

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

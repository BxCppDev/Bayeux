/// \file mctools/signal/signal_shape_builder.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Author(s)     : Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>
 * Creation date : 2016-11-29
 * Last modified : 2016-11-29
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Copyright (C) 2016 Guillaume Oliviéro <goliviero@lpccaen.in2p3.fr>
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
 *   A signal shape builder
 *
 */

#ifndef MCTOOLS_SIGNAL_SIGNAL_SHAPE_BUILDER_H
#define MCTOOLS_SIGNAL_SIGNAL_SHAPE_BUILDER_H

// Standard library:
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/noncopyable.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/factory.h>
#include <datatools/i_tree_dump.h>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace datatools {
  class service_manager;
} // end of namespace datatools

namespace mctools {

  namespace signal {

    //! \brief A signal shape builder/factory.
    class signal_shape_builder
      : private boost::noncopyable
      , public datatools::i_tree_dumpable
    {
    public:

      typedef datatools::factory_register<mygsl::i_unary_function> freg_type;

      // Return the prefix for reference functor keys
      static const std::string & reference_functor_prefix();

      //! Default constructor
      signal_shape_builder();

      //! Destructor
      ~signal_shape_builder() override;

      //! Return the logging priority
      datatools::logger::priority get_logging_priority() const;

      //! Set the logging priority
      void set_logging_priority(const datatools::logger::priority);

      //! Check if a category of signal is set
      bool has_category() const;

      //! Set the category of signals
      void set_category(const std::string &);

      //! Reset the category of signals
      void reset_category();

      //! Return the category of signals
      const std::string & get_category() const;

      //! Add registered shape type identifier
      void add_registered_shape_type_id(const std::string &);

      //! Check if a registered shape type identifier exists
      bool has_registered_shape_type_id(const std::string & )const;

      //! Check if the handle to an external service manager is set
      bool has_service_mgr() const;

      //! Set the handle to an external service manager
      void set_service_mgr(const datatools::service_manager &);

      //! Return the handle to an external service manager
      const datatools::service_manager & get_service_mgr() const;

      //! Check initialization
      bool is_initialized() const;

      //! Initialize the signal builder
      void initialize_simple();

      //! Initialize the signal builder from a set of configuration properties
      void initialize(const datatools::properties & config_);

      //! Reset the signal
      void reset();

      //! Instantiate a functor
      const mygsl::i_unary_function &
      create_signal_shape(const std::string & key_,
                          const std::string & shape_type_id_,
                          const datatools::properties & shape_params_);

      //! Check if a functor factory exists
      bool has_functor_factory(const std::string & shape_type_id_) const;

      //! Check if a reference functor with given key exists
      bool has_reference_functor(const std::string & key_) const;

      //! Return a const reference to a reference functor
      const mygsl::i_unary_function & get_reference_functor(const std::string & key_) const;

      //! Load definition file for reference functors
      void load_reference_functors_from_file(const std::string & filename_);

      //! Create a new reference functor
      void add_reference_functor(const std::string & key_,
                                 const std::string & shape_type_id_,
                                 const datatools::properties & shape_params_);

      //! Check if an user functor with given key exists
      bool has_functor(const std::string & key_) const;

      //! Return a mutable reference to an user functor
      mygsl::i_unary_function & grab_functor(const std::string & key_);

      //! Return a const reference to an user functor
      const mygsl::i_unary_function & get_functor(const std::string & key_) const;

      //! Return a handle to a const user functor
      mygsl::const_unary_function_handle_type get_functor_handle(const std::string & key_) const;

      //! Remove an user functor with given key
      void clear_functor(const std::string & key_);

      //! Remove all user functors
      void clear_functors();

      //! Return the dictionnary of functors
      const mygsl::unary_function_dict_type & get_functors() const;

      //! Build the list of reference functors
      void build_list_of_reference_functors(std::set<std::string> &) const;

      //! Build the list of user functors
      void build_list_of_functors(std::set<std::string> &) const;

      //! Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const override;

    private:

      void _init_(const datatools::properties * config_);

      void _init_reference_functors_();

      void _init_registration_();

      void _reset_();

      std::string _generate_functor_key();

      void _update_all_functors_();

    private:

      // Management:
      bool _initialized_ = false; //!< Initialization flag
      datatools::logger::priority _logging_; //!< Logging priority threshold

      // Configuration:
      std::string _category_; //!< The category of signals
      std::set<std::string> _registered_shape_type_ids_; //!< Set of shape type identifiers
      const datatools::service_manager * _service_mgr_ = nullptr; //!< Handle to an external service manager

      // Working data:
      datatools::factory_register<mygsl::i_unary_function> _freg_; //!< The factory register
      mygsl::unary_function_dict_type _reference_functors_; //!< Dictionary of persitent reference functors
      mygsl::unary_function_dict_type _functors_; //!< Dictionary of user created functors
      mygsl::unary_function_dict_type _all_functors_; //!< Dictionary of all (reference + user) created functors

      //! Reflection interface
      DR_CLASS_RTTI()

    };

  } // end of namespace signal

} // end of namespace mctools

// Activate reflection layer :
DR_CLASS_INIT(::mctools::signal::signal_shape_builder)

#endif // MCTOOLS_SIGNAL_SIGNAL_SHAPE_BUILDER_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

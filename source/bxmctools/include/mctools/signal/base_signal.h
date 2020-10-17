/// \file mctools/signal/base_signal.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-09-27
 * Last modified : 2016-09-27
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   A base class representing an analog signal associated to a hit detector.
 *
 */

#ifndef MCTOOLS_SIGNAL_BASE_SIGNAL_H
#define MCTOOLS_SIGNAL_BASE_SIGNAL_H

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace mctools {

  namespace signal {

    class signal_shape_builder;

    //! \brief Abstract base class representing an analog signal associated to a hit detector.
    //!
    //! The signal is modelled has a [voltage = f(time)] arbitrary function.
    //! It uses a functor to describe the shape of the electric signal as expected
    //! from an analog measurement. This object contains all informations needed
    //! to build/instantiate a signal shape object, with the help of an external
    //! signal shape builder object.
    class base_signal : public geomtools::base_hit
    {
    public:

      static const std::string & shape_parameter_prefix();
      static const std::string & allow_no_shape_builder_key();
      static std::string build_shape_key(const int32_t);
      static std::string build_private_shape_key(const int32_t, const std::size_t);

      //! Default constructor
      base_signal();

      //! Copy constructor
      base_signal(const base_signal &);

      //! Destructor
      ~base_signal() override;

      //! Assignment operator
      base_signal & operator=(const base_signal &);

      //! Check signal hit validity
      bool is_valid() const override;

      //! Allow no shape builder
      void set_allow_no_shape_builder(bool a_);

      //! Check is a shape builder is mandatory to build the signal shape object
      bool is_shape_builder_mandatory() const;

      //! Return the logging priority
      datatools::logger::priority get_logging() const;

      //! Set the logging priority
      void set_logging(const datatools::logger::priority);

      //! Check if time reference is set
      bool has_time_ref() const;

      //! Return the time reference
      double get_time_ref() const;

      //! Set the time reference
      void set_time_ref(const double);

      //! Reset the time reference
      void reset_time_ref();

      //! Check if a shape builder is set
      bool has_shape_builder() const;

      //! Return a handle to a shape builder
      const signal_shape_builder & get_shape_builder() const;

      //! Return a handle to a shape builder
      signal_shape_builder & grab_shape_builder();

      //! Set a handle to a shape builder
      void set_shape_builder(signal_shape_builder &);

      //! Check if the signal has category
      bool has_category() const;

      //! Set the signal category
      const std::string & get_category() const;

      //! Set the signal category
      void set_category(const std::string &);

      //! Reset the signal category
      void reset_category();

      //! Check the shape type identifier
      bool has_shape_type_id() const;

      //! Return the shape type identifier
      const std::string & get_shape_type_id() const;

      //! Set the shape type identifier
      void set_shape_type_id(const std::string &);

      //! Set the full set of parameters for the shape object
      void set_shape_parameters(const datatools::properties & params_);

      //! Erase the full set of parameters for the shape object
      void reset_shape_parameters();

      //! Erase one specific parameter for the shape object
      void reset_shape_parameter(const std::string & key_);

      //! Set a boolean parameter for the shape object
      void set_shape_boolean_parameter(const std::string & key_,
                                       const bool value_,
                                       const std::string & desc_ = "");

      //! Set a vector of boolean parameters for the shape object
      void set_shape_boolean_parameters(const std::string & key_,
                                        const std::vector<bool> & values_,
                                        const std::string & desc_ = "");

      //! Set a integer parameter for the shape object
      void set_shape_integer_parameter(const std::string & key_,
                                       const int value_,
                                       const std::string & desc_ = "");

      //! Set a vector of integer parameters for the shape object
      void set_shape_integer_parameters(const std::string & key_,
                                        const std::vector<int> & values_,
                                        const std::string & desc_ = "");

      //! Set a string parameter for the shape object
      void set_shape_string_parameter(const std::string & key_,
                                      const std::string & value_,
                                      const std::string & desc_ = "");

      //! Set a vector of string parameters for the shape object
      void set_shape_string_parameters(const std::string & key_,
                                       const std::vector<std::string> & values_,
                                       const std::string & desc_ = "");

      //! Set a real parameter for the shape object
      void set_shape_real_parameter(const std::string & key_,
                                    const double value_,
                                    const std::string & desc_ = "");

      //! Set a vector of real parameters for the shape object
      void set_shape_real_parameters(const std::string & key_,
                                     const std::vector<double> & values_,
                                     const std::string & desc_ = "");

      //! Set a real parameter for the shape object (with explicit unit)
      void set_shape_real_parameter_with_explicit_unit(const std::string & key_,
                                                       const double value_,
                                                       const std::string & unit_symbol_,
                                                       const std::string & desc_ = "");

      //! Set a vector of real parameters for the shape object (with explicit unit)
      void set_shape_real_parameters_with_explicit_unit(const std::string & key_,
                                                        const std::vector<double> & values_,
                                                        const std::string & unit_symbol_,
                                                        const std::string & desc_ = "");

      //! Add a private shape (only useful if a shape builder is associated to the base signal)
      void add_private_shape(const std::string & key_,
                             const std::string & type_id_,
                             const datatools::properties & parameters_);

      //! Add a private shape (only useful if a shape builder is associated to the base signal)
      std::string add_auto_private_shape(const std::string & type_id_,
                                         const datatools::properties & parameters_);

      //! Remove a private shape
      void remove_private_shape(const std::string & key_);

      //! Remove all private shapes
      void remove_private_shapes();

      //! Check if a shape is instantiated
      bool is_shape_instantiated() const;

      //! Return a const reference to the embedded signal shape
      const mygsl::i_unary_function & get_shape() const;

      //! Discard the embedded signal shape object
      void reset_shape();

      //! Set the embedded signal shape object without ownership
      void set_shape(const mygsl::i_unary_function & shape_);

      //! Set the embedded signal shape object with transfered ownership
      void set_shape_owned(const mygsl::i_unary_function * shape_ptr_);

      //! Unlock
      void unlock();

      //! Relock
      void relock();

      //! Check initialization
      bool is_initialized() const;

      //! Initialize the signal
      void initialize_simple();

      //! Initialize the signal
      void initialize(const datatools::properties & config_);

      //! Reset the signal
      void reset();

      //! Check if some private shapes are setup
      bool has_private_shapes_config() const;

      //! Return the configuration informations for private shapes
      const datatools::multi_properties & get_private_shapes_config() const;

      //! Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const override;

      //! Compute shape
      double compute_shape(double time_) const;

      //! Build the signal shape from a builder
      static bool build_signal_shape(signal_shape_builder & builder_,
                                     const std::string & key_,
                                     base_signal & signal_,
                                     const datatools::logger::priority logging_ = datatools::logger::PRIO_FATAL);

      //! Instantiate the signal shape from a builder
      bool instantiate_signal_shape(signal_shape_builder & builder_, const std::string & key_ = "");

    private:

      //! Factory method for the embedded shape object
      const mygsl::i_unary_function & _get_shape_();

      //! Copy from another signal hit
      void _copy_(const base_signal & sig_);

      //! Discard the embedded signal shape object
      void _reset_shape_();

      //! Set the embedded signal shape object
      void _set_shape_(const mygsl::i_unary_function &, bool owned_);

      //! Private initialization
      void _init_();

      //! Private reset
      void _reset_();

      //! Base construct
      void _base_construct_();

    private:

      // Management:
      bool                         _initialized_ = false;     //!< Initialization flag
      datatools::logger::priority  _logging_;                 //!< Logging priority threshold
      signal_shape_builder *       _shape_builder_ = nullptr; //!< Handle to an external shape builder

      // Configuration:
      std::string _category_;      //!< Signal category
      double      _time_ref_;      //!< Time reference
      std::string _shape_type_id_; //!< Shape type identifier
      datatools::multi_properties _private_shapes_config_; //!< Configuration for private shapes

      // Working data:
      bool _owned_shape_ = false; //!< Ownership flag for the embedded signal shape object
      const mygsl::i_unary_function * _shape_ = nullptr; //!< Handle to an embedded signal shape object

      DATATOOLS_SERIALIZATION_DECLARATION()

      //! Reflection interface
      DR_CLASS_RTTI()

    };

  } // end of namespace signal

} // end of namespace mctools

// Activate reflection layer :
DR_CLASS_INIT(::mctools::signal::base_signal)

#endif // MCTOOLS_SIGNAL_BASE_SIGNAL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

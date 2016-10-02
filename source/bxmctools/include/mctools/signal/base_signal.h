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
 * History:
 *
 */

#ifndef MCTOOLS_SIGNAL_BASE_SIGNAL_H
#define MCTOOLS_SIGNAL_BASE_SIGNAL_H

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>
// - Bayeux/geomtools:
#include <geomtools/base_hit.h>

namespace mctools {

  namespace signal {

    //! \brief Abstract base class representing an analog signal of a hit detector.
    //!
    //! The signal is modelled has a [voltage = f(time)] arbitrary function.
    //! It uses a functor to describe the shape of the electric signal as expected
    //! from an analog measurement.
    class base_signal : public geomtools::base_hit
    {
    public:

      static const std::string & shape_parameter_prefix();

      //! Default constructor
      base_signal();

      //! Copy constructor
      base_signal(const base_signal &);

      //! Destructor
      virtual ~base_signal();

      //! Assignment operator
      base_signal & operator=(const base_signal &);

      //! Check if object can be copied
      bool is_copyable() const;

      //! Check signal hit validity
      virtual bool is_valid() const;

      //! Check the shape type identifier
      bool has_shape_type_id() const;

      //! Return the shape type identifier
      const std::string & get_shape_type_id() const;

      //! Set the shape type identifier
      void set_shape_type_id(const std::string &);

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

      //! Check if a shape is instantiated
      bool is_shape_instantiated() const;

      //! Return a const reference to the embedded signal shape
      const mygsl::i_unary_function & get_shape() const;

      //! Return a mutable reference to the embedded signal shape
      mygsl::i_unary_function & grab_shape();

      //! Discard the embedded signal shape object
      void reset_shape();

      //! Set the embedded signal shape object
      void set_shape(mygsl::i_unary_function & shape_);

      //! Set the embedded signal shape object
      void set_shape(mygsl::i_unary_function * shape_ptr_);

      //! Check initialization
      bool is_initialized() const;

      //! Initialize the signal
      void initialize_simple();

      //! Initialize the signal
      void initialize(const datatools::properties & config_);

      //! Reset the signal
      void reset();

      //! Smart print
      virtual void tree_dump(std::ostream & out_         = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_               = false) const;

      //! Compute shape
      double compute_shape(double time_) const;

    protected:

      //! Factory method for the embedded shape object
      mygsl::i_unary_function & _grab_shape();

      //! Discard the embedded signal shape object
      void _reset_shape();

      //! Copy from another signal hit
      void _copy(const base_signal & sig_);

    private:

      // Management:
      bool _initialized_ = false; //!< Initialization flag

      // Configuration:
      std::string _shape_type_id_; //!< Shape type identifier

      // Working data:
      bool _owned_shape_ = false; //!< Ownership flag for the embedded signal shape object
      mygsl::i_unary_function * _shape_ = nullptr; //!< Handle to an embedded signal shape object

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

/// \file mctools/signal/multi_signal_shape.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2016-11-30
 * Last modified : 2016-11-30
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
 *   A class representing a signal shape made of the superposition of
 *   other signal shapes, arbitrarily scaled and shifted.
 *
 */

#ifndef MCTOOLS_SIGNAL_MULTI_SIGNAL_SHAPE_H
#define MCTOOLS_SIGNAL_MULTI_SIGNAL_SHAPE_H

// Standard library:
#include <vector>

// Third party:
// - Bayeux/datatools :
#include <datatools/i_cloneable.h>
// - Bayeux/mygsl:
#include <mygsl/i_unary_function.h>
#include <mygsl/triangle_function.h>

//  This project:
#include <mctools/signal/utils.h>

namespace mctools {

  namespace signal {

    //! \brief A signal shape made of the superposition of other signal shapes, arbitrarily scaled and shifted
    class multi_signal_shape
      : public mygsl::i_unary_function
      , public datatools::i_cloneable
    {
    public:

      //! Default constructor
      multi_signal_shape();

      //! Destructor
      ~multi_signal_shape() override;

      //! Check the validity
      bool is_valid() const;

      //! Check initialization status
      bool is_initialized() const override;

      //! Initialization
      void initialize(const datatools::properties & config_,
                              const mygsl::unary_function_dict_type & functors_) override;

      //! Reset
      void reset() override;

      //! Remove a component signal by index
      void remove(const std::size_t index_);

      //! Add a component signal
      std::size_t add(const std::string & key_,
                      const mygsl::const_unary_function_handle_type & shape_,
                      double time_shift_ = 0.0,
                      double scaling_ = 1.0);

      //! Add a component signal
      std::size_t add(const std::string & key_,
                      const mygsl::i_unary_function & shape_,
                      double time_shift_ = 0.0,
                      double scaling_ = 1.0);

      //! Return the number of components
      std::size_t get_number_of_components() const;

      //! Check if the function has an explicit domain of definition (default: false)
      bool has_explicit_domain_of_definition() const override;

      //! Check if a value is in the explicit domain of definition (default: true)
      bool is_in_domain_of_definition(double x_) const override;

      //! The minimum bound of the non-zero domain (default is minus infinity)
      double get_non_zero_domain_min() const override;

      //! The maximum bound of the non-zero domain (default is plus infinity)
      double get_non_zero_domain_max() const override;

      //! Smart printing
      void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const override;

    protected:

      //! Set default attributes
      void _set_defaults();

      //! Evaluation from parameters
      double _eval(double x_) const override;

    private:

      //! Private initialization
      void _init_();

      //! Private reset
      void _reset_();

      //! Automatically update the non zero domain
      void _recompute_();

    private:

      // Management:
      bool _initialized_ = false; //!< Initialization flag

      // Working data:
      bool _explicit_domain_of_definition_ = false;
      double _non_zero_domain_min_; //!< The minimum bound of the non-zero domain
      double _non_zero_domain_max_; //!< The maximum bound of the non-zero domain

      struct component_record;
      std::vector<component_record> _components_; //!< List of component signal shapes

      //! Registration of the functor class
      MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(multi_signal_shape)

      //! Cloneable interface
      DATATOOLS_CLONEABLE_DECLARATION(multi_signal_shape)

    };

  } // end of namespace signal

} // end of namespace mctools

#endif // MCTOOLS_SIGNAL_MULTI_SIGNAL_SHAPE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

//! \file  mygsl/composite_function.h
//! \brief A composite function from two unary functors
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MYGSL_COMPOSITE_FUNCTION_H
#define MYGSL_COMPOSITE_FUNCTION_H

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief Composite function
  /**
   *  h(x) = f(g(x))
   *
   * where f(y) is named the first function
   * and g(x) is named the second function.
   *
   *
   */
  class composite_function
    : public i_unary_function
  {
  public:

    //! Default constructor
    composite_function();

    //! Constructor
    composite_function(const i_unary_function & f_,
                       const i_unary_function & g_);

    //! Constructor
    composite_function(const const_unary_function_handle_type & hcf_,
                       const const_unary_function_handle_type & hcg_);

    //! Destructor
    ~composite_function() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the composite function
    void reset() override;

    //! Set the first functor
    void set_f(const i_unary_function &);

    //! Set the second functor
    void set_g(const i_unary_function &);

    //! Check the first functor
    bool has_f() const;

    //! Check the second functor
    bool has_g() const;

    //! Check if the function has an explicit domain of definition (default: false)
    /* virtual */ bool has_explicit_domain_of_definition() const override;

    /* virtual */ bool is_in_domain_of_definition(double x_) const override;

    /* virtual */ double get_non_zero_domain_min() const override;

    /* virtual */ double get_non_zero_domain_max() const override;

    //! Smart printing
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected :

    //! Evaluation
    double _eval(double x_) const override;

  private:

    //! Private initialization
    void _init_();

   private:

    unary_function_handle _f_; //!< First functor
    unary_function_handle _g_; //!< Second functor

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(composite_function)

  };

} // end of namespace mygsl

#endif // MYGSL_COMPOSITE_FUNCTION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --

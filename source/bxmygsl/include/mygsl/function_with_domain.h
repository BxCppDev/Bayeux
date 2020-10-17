//! \file  mygsl/function_with_domain.h
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

#ifndef MYGSL_FUNCTION_WITH_DOMAIN_H
#define MYGSL_FUNCTION_WITH_DOMAIN_H

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/interval.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace mygsl {

  //! \brief Function with interval/domain of definition
  class function_with_domain : public i_unary_function
  {
  public:

    //! Check if the functor is defined
    bool has_functor() const;

    //! Set the functor
    void set_functor(const i_unary_function & functor_);

    //! Check if the domain of definition is defined
    bool has_domain_of_definition() const;

    //! Set the domain of definition
    void set_domain_of_definition(const interval & domain_);

    //! Return the domain of definition
    const interval & get_domain_of_definition() const;

    //! Default constructor
    function_with_domain();

    //! Constructor
    function_with_domain(const i_unary_function & functor_);

    //! Constructor
    function_with_domain(const i_unary_function & functor_, const interval & domain_);

    //! Destructor
    ~function_with_domain() override;

    //! Check the domain of definition
    bool has_explicit_domain_of_definition() const override;

    //! Check is a value is in the domain of definition
    bool is_in_domain_of_definition(double x_) const override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the functor
    void reset() override;

  protected:

    //! Evaluation
    double _eval(double x_) const override;

  private:

    unary_function_handle _functor_; //!< Address of an external unary function
    interval              _domain_of_definition_; //!< Interval of definition

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(function_with_domain)

  };

} // end of namespace mygsl

#endif // MYGSL_FUNCTION_WITH_DOMAIN_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

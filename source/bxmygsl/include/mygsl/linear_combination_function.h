//! \file  mygsl/linear_combination_function.h
//! \brief A linear combination function from many unary functors
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

#ifndef MYGSL_LINEAR_COMBINATION_FUNCTION_H
#define MYGSL_LINEAR_COMBINATION_FUNCTION_H

// This project:
#include <mygsl/i_unary_function.h>

namespace mygsl {

  //! \brief A linear combination function
  /**
   *  h(x) = a * f(x) + b * g(x) + ... + c * j(x)
   *
   * where f(x), g(x)... j(x) are functors
   * and a, b... c are weighting factors.
   *
   */
  class linear_combination_function : public i_unary_function
  {
  public:

    //! Default constructor
    linear_combination_function();

    //! Constructor
    linear_combination_function(double w1_,
                                const const_unary_function_handle_type & f1_,
                                double w2_,
                                const const_unary_function_handle_type & f2_);

    //! Destructor
    ~linear_combination_function() override;

    //! Check initialization status
    bool is_initialized() const override;

    //! Initialization from a container of parameters and a dictionary of functors
    void initialize(const datatools::properties & config_,
                            const unary_function_dict_type & functors_) override;

    //! Reset the composite function
    void reset() override;

    //! Add a weighted term
    std::size_t add(double weight_, const i_unary_function &);

    //! Add a weighted term
    std::size_t add(double weight_, const const_unary_function_handle_type &);

    //! Change weight
    void change_weight(int term_index_, double weight_);

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

  protected :

    //! Evaluation
    double _eval(double x_) const override;

    //! Set default attributes values
    void _set_defaults();

  private:

    //! Recompute internals
    void _recompute_();

  private:

    // Working data:
    struct term_type;
    std::vector<term_type> _terms_; //!< Array of weighted functors
    bool _explicit_domain_of_definition_; //!< Flag for explicit domain of definition
    double _non_zero_domain_min_; //!< Non zero domain minimum bound
    double _non_zero_domain_max_; //!< Non zero domain maximum bound

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(linear_combination_function)

  };

} // end of namespace mygsl

#endif // MYGSL_LINEAR_COMBINATION_FUNCTION_H

// Local Variables:
// mode: c++
// coding: utf-8
// End:

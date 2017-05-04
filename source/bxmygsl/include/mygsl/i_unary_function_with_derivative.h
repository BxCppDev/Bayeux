/// \file mygsl/i_unary_function_with_derivative.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2007-12-07
 * Last modified: 2013-04-23
 *
 * License:
 *
 *
 * Description:
 *
 *   Abstract interface for R to R functor with
 *   support interface for associated derivativ function.
 *
 * History:
 *
 */

#ifndef MYGSL_I_UNARY_FUNCTION_WITH_DERIVATIVE_H
#define MYGSL_I_UNARY_FUNCTION_WITH_DERIVATIVE_H 1

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/interval.h>

namespace mygsl {

  //! \brief Abstract interface for unary functions with derivative
  class i_unary_function_with_derivative : public i_unary_function
  {

  public:

    //! Constructor
    i_unary_function_with_derivative(double epsilon_ = 0.0);

    //! Destructor
    virtual ~i_unary_function_with_derivative();

    virtual double eval_f(double x_) const;

    virtual double eval_df(double x_) const;

    virtual void eval_fdf(double x_, double & f_, double & df_) const;

  protected :

    virtual double _eval_df_numeric(double x_, double h_ = 0.0) const;

  };

  //! \brief Unary functions wrapper that adds numeric derivative
  class unary_function_promoted_with_numeric_derivative : public i_unary_function_with_derivative
  {
  public:

    //! Check if the functor is defined
    bool has_functor() const;

    //! Set the functor
    void set_functor(const i_unary_function & functor_);

    //! Default constructor
    unary_function_promoted_with_numeric_derivative();

    //! Constructor
    unary_function_promoted_with_numeric_derivative(const i_unary_function & functor_);

    virtual bool is_in_domain_of_definition(double x_) const;

    virtual bool has_explicit_domain_of_definition() const;

    virtual double get_non_zero_domain_min() const;

    virtual double get_non_zero_domain_max() const;

    //! Check initialization status
    virtual bool is_initialized() const;

    //! Reset the functor
    virtual void reset();

  protected:

    //! Evaluation
    virtual double _eval(double x_) const;

  private:

    const i_unary_function * _functor_; //!< Address of an external unary function

    //! Registration of the functor class
    MYGSL_UNARY_FUNCTOR_REGISTRATION_INTERFACE(unary_function_promoted_with_numeric_derivative)

  };

} // end of namespace mygsl

#endif // MYGSL_I_UNARY_FUNCTION_WITH_DERIVATIVE_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

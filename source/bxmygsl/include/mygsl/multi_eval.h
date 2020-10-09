/// \file mygsl/multi_eval.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-04
 * Last modified: 2013-04-11
 *
 * License:
 *
 *
 * Description:
 *
 * History:
 *
 */

#ifndef MYGSL_MULTI_EVAL_H
#define MYGSL_MULTI_EVAL_H 1

// Standard library:
#include <vector>

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/interval.h>

namespace mygsl {

  /// Abstract functor from a R^n space to R
  class multi_eval
  {
  public:

    typedef std::vector<interval> multi_domain;

  public:

    multi_eval (size_t dimension_);

    virtual ~multi_eval ();

  public:

    size_t get_dimension () const;

    interval & get_domain (int i_);

    const interval & get_domain (int i_) const;

    bool is_valid (const double * x_) const;

    virtual bool is_valid (int i_, double x_) const;

    double evaluate (double x_) const;

    double evaluate (double x_, double y_) const;

    double evaluate (double x_, double y_, double z_) const;

    double evaluate (const std::vector<double> & v_) const;

    double evaluate (const double * x_) const;

    double operator () (const double * x_) const;

    // to feed the GSL gsl_function interface:
    static double g_function (const double * x_, void * functor_);

  protected:

    virtual double _eval(const double * x_) const = 0;

  private:

    void _check_dimension_ (size_t dim_) const;

  private:

    multi_domain _domains_;

  };

  class unary_eval_from_multi : public i_unary_function
  {

  public:

    bool is_valid (double x_) const;

    const double & param (int i_) const;

    double & param (int i_);

    void init (const multi_eval & multi_eval_,
               int i_,
               const std::vector<double> & params_);

    void set_index (int i_);

    unary_eval_from_multi (const multi_eval & multi_eval_,
                           int i_,
                           const std::vector<double> & params_);

    unary_eval_from_multi (const multi_eval & multi_eval_,
                           int i_,
                           const double * params_);

    ~unary_eval_from_multi () override;

  protected:

   double _eval(double x_) const override;

  private:

    int                 _index_;
    const multi_eval *  _multi_eval_;
    std::vector<double> _params_;

  };

} // end of namespace mygsl

#endif // MYGSL_MULTI_EVAL_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

/// \file mygsl/one_dimensional_minimization.h

#ifndef MYGSL_ONE_DIMENSIONAL_MINIMIZATION_H
#define MYGSL_ONE_DIMENSIONAL_MINIMIZATION_H 1

// Standard library:
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

// Third party:
// - GSL:
#include <gsl/gsl_min.h>

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/best_value.h>

namespace mygsl {

  /// \brief One dimensional minimization algorithm
  class one_dimensional_minimization
  {
  public:
    static const std::string & goldensection_method_label();
    static const std::string & brent_method_label();
    static double        default_epsabs();
    static const size_t        DEFAULT_MAX_ITER = 1000;

    class at_step_action
    {
    public:

      virtual void action (int status_,
                           size_t iter_,
                           double a_,
                           double b_,
                           double c_) = 0;

      void operator () (int status_,
                        size_t iter_,
                        double a_,
                        double b_,
                        double c_);
    };

    struct default_step_action : public at_step_action
    {
      void action (int status_,
                           size_t iter_,
                           double a_,
                           double b_,
                           double c_) override;
    };

  public:

    void unset_step_action ();

    void set_default_step_action ();

    void set_step_action (at_step_action & action_);

    const best_value & get_minimum_value () const;

    std::string get_name () const;

    void set_debug (bool debug_ = true);

    bool is_debug () const;

    size_t get_iter () const;

    size_t get_max_iter () const;

    double get_epsabs () const;

    bool is_converged () const;

    one_dimensional_minimization (bool debug_ = false);

    virtual ~one_dimensional_minimization ();

    void init (const i_unary_function & functor_,
               const std::string & method_ = "");

    void initialize (const i_unary_function & functor_,
                     const std::string & method_ = "");

    void reset ();

  protected:

    void _at_step_hook (int status_,
                        size_t iter_,
                        double a_,
                        double b_,
                        double c_);

  public:

    int minimize (double a_,
                  double b_,
                  double m_,
                  double epsabs_);

    static double g_function (double x_, void * params_);

    static best_value minimize (const i_unary_function & sys_,
                                double a_,
                                double b_,
                                double m_,
                                double epsabs_,
                                const std::string & method_ = "");

  private:

    static default_step_action _default_step_action_;

  private:

    bool _debug_;
    const gsl_min_fminimizer_type * _fminimizer_type_;
    gsl_min_fminimizer            * _fminimizer_;
    int _status_;
    gsl_function _function_;
    best_value _minimum_value_;
    size_t _iter_;
    size_t _max_iter_;
    double _epsabs_;
    bool   _converged_;
    const i_unary_function * _functor_;

    // hook step function:
    at_step_action * _at_step_action_;

  };

} // namespace mygsl

#endif // MYGSL_ONE_DIMENSIONAL_MINIMIZATION_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

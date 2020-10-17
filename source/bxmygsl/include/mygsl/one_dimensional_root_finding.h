/// \file mygsl/one_dimensional_root_finding.h

#ifndef MYGSL_ONE_DIMENSIONAL_ROOT_FINDING_H
#define MYGSL_ONE_DIMENSIONAL_ROOT_FINDING_H 1


// Third party:
// - GSL:
#include <gsl/gsl_roots.h>

// This project:
#include <mygsl/i_unary_function.h>
#include <mygsl/i_unary_function_with_derivative.h>
#include <mygsl/best_value.h>

namespace mygsl {

  /// \brief One dimensional root solver algorithm
  class one_dimensional_root_solver
  {
  public:

    static const std::string BISECTION_METHOD_LABEL;
    static const std::string FALSEPOS_METHOD_LABEL;
    static const std::string BRENT_METHOD_LABEL;

    static const std::string NEWTON_METHOD_LABEL;
    static const std::string SECANT_METHOD_LABEL;
    static const std::string STEFFENSON_METHOD_LABEL;
    static const size_t DEFAULT_MAX_ITER;
    static const double DEFAULT_EPSABS;

    enum mode_t
      {
        MODE_NULL = 0,
        MODE_F    = 1,
        MODE_FDF  = 2
      };

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

    void unset_step_action ();

    void set_default_step_action ();

    void set_step_action (at_step_action & action_);

    bool is_fsolver () const;

    bool is_fdfsolver () const;

    bool is_initialized () const;

    const best_value & get_root () const;

    std::string get_name () const;

    void set_debug (bool debug_ = true);

    bool is_debug () const;

    size_t get_iter () const;

    size_t get_max_iter () const;

    double get_epsabs () const;

    bool is_converged () const;

    one_dimensional_root_solver (bool debug_ = false);

    virtual ~one_dimensional_root_solver ();

    void init (const i_unary_function & sys_, const std::string & method_ = BRENT_METHOD_LABEL);

    void init (const i_unary_function_with_derivative & sys_, const std::string & method_ = STEFFENSON_METHOD_LABEL);

    void reset ();

    int solve (double epsabs_, double a_, double b_);

    static double g_function (double x_, void * params_);

    static double g_dfunction (double x_, void * params_);

    static void   g_fdfunction (double x_, void * params_, double * y_, double * dy_);

    static best_value solve (const i_unary_function & sys_,
                             double epsabs_,
                             double a_,
                             double b_,
                             const std::string & method_ = BRENT_METHOD_LABEL);

    static best_value solve (const i_unary_function_with_derivative & sys_,
                             double epsabs_,
                             double a_,
                             double b_,
                             const std::string & method_ = STEFFENSON_METHOD_LABEL);

  protected:

    void _at_step_hook (int status_,
                        size_t iter_,
                        double a_,
                        double b_,
                        double c_);

  private:

    static default_step_action _default_step_action_;

  private:

    bool _debug_;
    const gsl_root_fsolver_type   * _fsolver_type_;
    const gsl_root_fdfsolver_type * _fdfsolver_type_;
    gsl_root_fsolver              * _fsolver_;
    gsl_root_fdfsolver            * _fdfsolver_;
    int    _mode_;
    int    _status_;
    size_t _iter_;
    size_t _max_iter_;
    double _epsabs_;
    bool   _converged_;
    const i_unary_function           * _functor_f_;
    const i_unary_function_with_derivative * _functor_fdf_;
    const unary_function_promoted_with_numeric_derivative * _functor_numeric_fdf_;
    best_value       _root_;
    gsl_function     _function_;
    gsl_function_fdf _fdfunction_;

    // hook step function:
    at_step_action *            _at_step_action_;

  };

} // namespace mygsl

#endif // MYGSL_ONE_DIMENSIONAL_ROOT_FINDING_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

// -*- mode : c++; -*- 
// mygsl::one_dimensional_root_finding.h

#ifndef __mygsl__one_dimensional_root_finding_h
#define __mygsl__one_dimensional_root_finding_h 1


#include <gsl/gsl_roots.h>

#include <mygsl/unary_eval.h>
#include <mygsl/unary_eval_with_derivative.h>
#include <mygsl/best_value.h>

using namespace std;

namespace mygsl {

  class one_dimensional_root_solver
  {
  public: 
    static const string BISECTION_METHOD_LABEL;
    static const string FALSEPOS_METHOD_LABEL;
    static const string BRENT_METHOD_LABEL;

    static const string NEWTON_METHOD_LABEL;
    static const string SECANT_METHOD_LABEL;
    static const string STEFFENSON_METHOD_LABEL;
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
      virtual void action (int status_,
                           size_t iter_, 
                           double a_, 
                           double b_,
                           double c_);
    };

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
    unary_eval                 * _eval_f_;
    unary_eval_with_derivative * _eval_fdf_;
    unary_eval_promoted_with_numeric_derivative * _eval_numeric_fdf_;
    best_value       _root_;
    gsl_function     _function_;
    gsl_function_fdf _fdfunction_;

    // hook step function:
    at_step_action *            _at_step_action_;
   

  public:

    void unset_step_action ();
      
    void set_default_step_action ();
      
    void set_step_action (at_step_action & action_);

    bool is_fsolver () const;

    bool is_fdfsolver () const;

    bool is_initialized () const;

    const best_value & get_root () const;

    string get_name () const;

    void set_debug (bool debug_ = true);
      
    bool is_debug () const;
   
    size_t get_iter () const;
   
    size_t get_max_iter () const;
   
    double get_epsabs () const;

    bool is_converged () const;

    one_dimensional_root_solver (bool debug_ = false);

    virtual ~one_dimensional_root_solver ();

    void init (unary_eval & sys_, const string & method_ = BRENT_METHOD_LABEL);

    void init (unary_eval_with_derivative & sys_, const string & method_ = STEFFENSON_METHOD_LABEL);

    void reset ();

  protected:

    void _at_step_hook (int status_,
                        size_t iter_, 
                        double a_, 
                        double b_,
                        double c_);

  public:

    int solve (double epsabs_, double a_, double b_);

    static double g_function (double x_, void * params_);

    static double g_dfunction (double x_, void * params_);

    static void   g_fdfunction (double x_, void * params_, double * y_, double * dy_);

    static best_value solve (unary_eval & sys_, 
                             double epsabs_, 
                             double a_, 
                             double b_, 
                             const string & method_ = BRENT_METHOD_LABEL);
      
    static best_value solve (unary_eval_with_derivative & sys_, 
                             double epsabs_, 
                             double a_, 
                             double b_,
                             const string & method_ = STEFFENSON_METHOD_LABEL);

  };
  
} // namespace mygsl

#endif // __mygsl__one_dimensional_root_finding_h

// end of mygsl::one_dimensional_root_finding.h

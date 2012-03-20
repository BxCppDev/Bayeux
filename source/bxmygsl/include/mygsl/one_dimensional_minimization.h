// -*- mode: c++; -*- 
// mygsl::one_dimensional_minimization.h

#ifndef __mygsl__one_dimensional_minimization_h
#define __mygsl__one_dimensional_minimization_h 1

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

#include <gsl/gsl_min.h>

#include <mygsl/unary_eval.h>
#include <mygsl/best_value.h>

// using  namespace std;

namespace mygsl {

  class one_dimensional_minimization
  {
  public: 
    static const std::string GOLDENSECTION_METHOD_LABEL;
    static const std::string BRENT_METHOD_LABEL;
    static const size_t DEFAULT_MAX_ITER;
    static const double DEFAULT_EPSABS;

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

    void init (unary_eval & eval_, const std::string & method_ = BRENT_METHOD_LABEL);

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

    static best_value minimize (unary_eval & sys_, 
                                double a_, 
                                double b_, 
                                double m_, 
                                double epsabs_, 
                                const std::string & method_ = BRENT_METHOD_LABEL);
      
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
    unary_eval * _eval_;

    // hook step function:
    at_step_action * _at_step_action_;

  };
  
} // namespace mygsl

#endif // __mygsl__one_dimensional_minimization_h

// end of mygsl::one_dimensional_minimization.h

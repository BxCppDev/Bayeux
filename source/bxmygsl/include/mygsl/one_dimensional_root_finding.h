// mygsl::one_dimensional_root_finding.h

#ifndef __mygsl__one_dimensional_root_finding_h
#define __mygsl__one_dimensional_root_finding_h 1


#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
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
			    double c_)
	    {
	      action (status_, iter_, a_, b_, c_);
	    }
	  
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
      
      static default_step_action __default_step_action;
 
    private:
      bool __debug;
      const gsl_root_fsolver_type   * __fsolver_type;
      const gsl_root_fdfsolver_type * __fdfsolver_type;
      gsl_root_fsolver              * __fsolver;
      gsl_root_fdfsolver            * __fdfsolver;
      int    __mode;
      int    __status;
      size_t __iter;
      size_t __max_iter;
      double __epsabs;
      bool   __converged;
      unary_eval                 * __eval_f;
      unary_eval_with_derivative * __eval_fdf;
      unary_eval_promoted_with_numeric_derivative * __eval_numeric_fdf;
      best_value       __root;
      gsl_function     __function;
      gsl_function_fdf __fdfunction;

       // hook step function:
      at_step_action *            __at_step_action;
   

    public:

      void unset_step_action ();
      
      void set_default_step_action ();
      
      void set_step_action (at_step_action & action_);

      bool is_fsolver () const
	{
	  return __fsolver != 0;
	}

      bool is_fdfsolver () const
	{
	  return __fdfsolver != 0;
	}

      bool is_initialized () const
	{
	  return is_fsolver () || is_fdfsolver ();
	}

      const best_value & get_root () const
	{
	  return __root;
	}

      string get_name () const
	{
	  if (__fsolver != 0)
	    {
	      return string (gsl_root_fsolver_name (__fsolver));
	    }
	  if (__fdfsolver != 0)
	    {
	      return string (gsl_root_fdfsolver_name (__fdfsolver));
	    }
	  return string ("");
	}

      void set_debug (bool debug_ = true)
	{
	  __debug = debug_;
	}
      
      bool is_debug () const
	{
	  return __debug;
	}
   
      size_t get_iter () const
	{
	  return __iter;
	}
   
      size_t get_max_iter () const
	{
	  return __max_iter;
	}
   
      double get_epsabs () const
	{
	  return __epsabs;
	}

      bool is_converged () const
	{
	  return __converged;
	}

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

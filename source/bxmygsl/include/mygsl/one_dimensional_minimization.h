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

using namespace std;

namespace mygsl {

  class one_dimensional_minimization
    {
    public: 
      static const string GOLDENSECTION_METHOD_LABEL;
      static const string BRENT_METHOD_LABEL;
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
      const gsl_min_fminimizer_type * __fminimizer_type;
      gsl_min_fminimizer            * __fminimizer;
      int __status;
      gsl_function __function;
      best_value __minimum_value;
      size_t __iter;
      size_t __max_iter;
      double __epsabs;
      bool   __converged;
      unary_eval * __eval;

       // hook step function:
      at_step_action *            __at_step_action;

    public:

      void unset_step_action ();
      
      void set_default_step_action ();
      
      void set_step_action (at_step_action & action_);

      const best_value & get_minimum_value () const
	{
	  return __minimum_value;
	}

      string get_name () const
	{
	  if (__fminimizer == 0)
	    {
	      return string ("");
	    }
	  return string (gsl_min_fminimizer_name (__fminimizer));
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

      one_dimensional_minimization (bool debug_ = false);

      virtual ~one_dimensional_minimization ();

      void init (unary_eval & eval_, const string & method_ = BRENT_METHOD_LABEL);

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
				  const string & method_ = BRENT_METHOD_LABEL);

    };
  
} // namespace mygsl

#endif // __mygsl__one_dimensional_minimization_h

// end of mygsl::one_dimensional_minimization.h

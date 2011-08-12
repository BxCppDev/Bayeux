// mygsl::multifit_nlin.h

#ifndef __mygsl__multifit_nlin_h
#define __mygsl__multifit_nlin_h 1

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>

#include <mygsl/unary_eval.h>
#include <mygsl/param_entry.h>
#include <mygsl/multiparameter_system.h>

namespace mygsl {

  class multifit_nlin_system : public multiparameter_system
    {

    private:
 
      void __update_free_dimension ();

      void __update_auto_dimension ();

      void __update_dimensions ();

    public:
 
      void lock_params ();

      void unlock_params ();

      bool is_lock_params () const;

      void print_params (std::ostream &) const;

      void print_params2 (std::ostream &) const;

      const param_entry & get_param (int i_) const;

      const param_entry & get_free_param (int i_) const;

      param_entry & get_free_param (int i_);

      void set_param_value (int i_, double val_);

      void set_param_value_no_check (int i_, double val_);

      bool is_param_fixed (int i_) const;

      bool is_param_free (int i_) const;

      bool is_param_auto (int i_) const;

      double get_param_value (int i_) const;

      double get_param_min (int i_) const;

      double get_param_max (int i_) const;

      double get_param_step (int i_) const;

      const std::string & get_param_name (int i_) const;

      void add_param (const param_entry & pe_);

      size_t get_dimension () const;

      size_t get_free_dimension () const;

      size_t get_auto_dimension () const;

      size_t get_number_of_data () const;

    protected:

      virtual int _prepare_values ();

      virtual int _auto_values ();

      /*
      virtual int _eval_f (double & f_) = 0;

      virtual int _eval_df (double * gradient_) = 0;
      */

    public:

      virtual int prepare_values ();

      virtual int auto_values ();

      /*
      virtual int eval_f (const double * x_ , double * f_);

      virtual int eval_df (const double * x_ , double * gradient_);

      virtual int eval_fdf (const double * x_ , 
			    double & f_ , 
			    double * gradient_);
      */

      void to_double_star (double * pars_ , 
			   size_t dimension_) const;

      virtual void from_double_star (const double * pars_ , 
				     size_t dimension_);

      multifit_nlin_system ();

      virtual ~multifit_nlin_system ();
      
      void dump (std::ostream & out_) const;

    };

  /**********************************************/

  class multifit_nlin
    {
    public:
      static size_t DEFAULT_MAX_ITER;
      static bool   DEFAULT_VERBOSE;

      static bool g_debug;

      static int multifit_nlin_f (const gsl_vector * v_, 
				  void * params_,
				  gsl_vector * f_);
      
      static int multifit_nlin_df (const gsl_vector * v_, 
				   void * params_ , 
				   gsl_matrix * j_);
      
      static int multifit_nlin_fdf (const gsl_vector * v_, 
				    void * params_,
				    gsl_vector * f_, 
				    gsl_matrix * j_);

      static bool name_is_valid (const std::string & name_);

      class at_step_action
	{
	  
	public:

	  virtual void action (int status_ ,
			       size_t iter_ , 
			       double * x_ , 
			       size_t   dim_ ,
			       double f_) = 0;

	  void operator () (int status_ ,
			    size_t iter_ , 
			    double * x_ , 
			    size_t   dim_ ,
			    double f_)
	    {
	      action (status_, iter_, x_, dim_, f_);
	    }
	  
	};
      
      struct default_step_action : public at_step_action
      {
	virtual void action (int status_ ,
			     size_t iter_ , 
			     double * x_ , 
			     size_t   dim_ , 
			     double f_);
      };
      
      static default_step_action __default_step_action;
      
    private:

      bool __verbose;

      multifit_nlin_system            * __sys;
      const gsl_multifit_fdfsolver_type * __T;
      gsl_multifit_fdfsolver          * __s;
      gsl_multifit_function_fdf         __fdf;
      gsl_vector                      * __x;
      gsl_matrix                      * __covar;
      size_t                            __max_iter;
      size_t                            __iter;
      size_t                            __n;
      size_t                            __p;
      double __dof;
      double __chi;
      int    __status;
      string __error_message;
                
      // hook step function:
      at_step_action                  * __at_step_action;

      void __init_algorithm (const std::string & name_);

    public:

      const string & get_error_message () const;

      void reset_error_message ();

      static void print_types (std::ostream &);

      size_t get_iter () const;

      size_t get_n () const;

      size_t get_p () const ;

      double get_dof () const ;

      double get_chi () const ;
      
      void unset_step_action ();
      
      void set_default_step_action ();
      
      void set_step_action (at_step_action & asd_);
      
      std::string get_name () const;

      void init (const std::string & name_ , 
		 multifit_nlin_system & ms_);

      void reset ();

      multifit_nlin ();

      virtual ~multifit_nlin ();

      /*
      void _at_step_hook (int status_ ,
			  size_t iter_ , 
			  double * x_ , 
			  size_t   dim_ , 
			  double f_);
      */

      int fit (double epsabs_);

      void devel_dump_x () const;
       
    };
  
} // namespace mygsl

#endif // __mygsl__multifit_nlin_h

// end of mygsl::multifit_nlin.h

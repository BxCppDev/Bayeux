// mygsl::multimin.h

#ifndef __mygsl__multimin_h
#define __mygsl__multimin_h 1

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

#include <gsl/gsl_multimin.h>
#include <gsl/gsl_vector.h>

#include <mygsl/unary_eval.h>

namespace mygsl {


  class multimin_system
    {
    public:

      class param_entry
      {
      public:
	enum limit_t
	  {
	    LIMIT_NO    = 0x0,
	    LIMIT_MIN   = 0x1,
	    LIMIT_MAX   = 0x2,
	    LIMIT_RANGE = LIMIT_MIN|LIMIT_MAX
	  };
	enum type_t
	  {
	    TYPE_FREE=0,
	    TYPE_FIXED=1,
	    TYPE_AUTO=2
	  };

      public:

	std::string name;
	double value;
	int    type;
	int    limit;
	double min;
	double max;
	double step;

      private:

	param_entry()
	  {
	    value=0.1;
	    type =TYPE_FREE;
	    limit=LIMIT_NO;
	    min=0.0;
	    max=1.0;
	    step=0.01;
	    name="parameter";
	  }

      public:

	bool is_fixed() const { return type==TYPE_FIXED; }

	bool is_free() const { return type==TYPE_FREE; }

	bool is_auto() const { return type==TYPE_AUTO; }

	bool has_limit() const {return limit!=LIMIT_NO; }

	bool has_no_limit() const {return limit==LIMIT_NO; }

	bool has_min() const {return limit&LIMIT_MIN; }

	bool has_max() const {return limit&LIMIT_MAX; }

	bool is_in_range() const;

	double get_dist_to_min() const;

	double get_dist_to_max() const;

	double get_dist_to_limit() const;

	double get_value() const;

	double get_step() const;

	double get_min() const;

	double get_max() const;

	void set_value( double value_ );

	void set_value_no_check( double value_ );

	static param_entry make_param_entry_auto(
						 const std::string & name_ );

	static param_entry make_param_entry_fixed(
					    const std::string & name_ ,
					    double value_ );

	static param_entry make_param_entry_ranged(
					    const std::string & name_ ,
					    double value_ ,
					    double min_ ,
					    double max_ ,
					    double step_ ,
					    bool fixed_ = false );	
      };

      // predicate:
      struct param_has_name : std::unary_function<bool,param_entry>
      {
	std::string __name;
	param_has_name( const std::string & name_ ) : __name(name_) 
	{
	}
	bool operator()( const param_entry & pe_ )
	{
	  return pe_.name == __name;
	}
      };

      // predicate:
      struct param_is_free : std::unary_function<bool,param_entry>
      {
	param_is_free()  
	{
	}
	bool operator()( const param_entry & pe_ )
	{
	  return pe_.is_free();
	}
      };

      // predicate:
      struct param_is_fixed : std::unary_function<bool,param_entry>
      {
	param_is_fixed()  
	{
	}
	bool operator()( const param_entry & pe_ )
	{
	  return pe_.is_fixed();
	}
      };

      // predicate:
      struct param_is_auto : std::unary_function<bool,param_entry>
      {
	param_is_auto()  
	{
	}
	bool operator()( const param_entry & pe_ )
	{
	  return pe_.is_auto();
	}
      };

    protected:

      bool                     __lock_params;
      std::vector<param_entry> __params;

    private:

      size_t __free_dimension;
      size_t __auto_dimension;
 
      void __update_free_dimension();

      void __update_auto_dimension();

      void __update_dimensions();

    public:
 
      void lock_params();

      void unlock_params();

      bool is_lock_params() const;

      void print_params( std::ostream & ) const;

      void print_params2( std::ostream & ) const;

      const param_entry & get_param( int i_ ) const;

      void set_param_value( int i_, double val_ );

      void set_param_value_no_check( int i_, double val_ );

       bool is_param_fixed( int i_ ) const;

      bool is_param_free( int i_ ) const;

      bool is_param_auto( int i_ ) const;

      double get_param_value( int i_ ) const;

      double get_param_min( int i_ ) const;

      double get_param_max( int i_ ) const;

      double get_param_step( int i_ ) const;

      const std::string & get_param_name( int i_ ) const;

      void add_param( const param_entry & pe_ );

      size_t get_dimension() const;

      size_t get_free_dimension() const;

      size_t get_auto_dimension() const;

    protected:

      virtual int _prepare_values();

      virtual int _auto_values();

      virtual int _eval_f( double & f_ ) = 0;

      virtual int _eval_df( double * gradient_ ) = 0;

    public:

      virtual int prepare_values();

      virtual int auto_values();

      virtual int eval_f( const double * x_ , double & f_ );

      virtual int eval_df( const double * x_ , double * gradient_ );

      virtual int eval_fdf( const double * x_ , 
			    double & f_ , 
			    double * gradient_ );

      void to_double_star( double * pars_ , 
			   size_t dimension_ ) const;

      virtual void from_double_star( const double * pars_ , 
				     size_t dimension_ );

      multimin_system();

      virtual ~multimin_system();
      
      void dump( std::ostream & out_ ) const;

    };

  class multimin
    {
    public:
      static size_t DEFAULT_MAX_ITER;
      static bool   DEFAULT_VERBOSE;

      static bool g_debug;

      static double multimin_f   ( const gsl_vector * v_ , void * params_ );

      static void   multimin_df  ( const gsl_vector * v_ , void * params_ , 
				   gsl_vector * gradient_ );

      static void   multimin_fdf ( const gsl_vector * v_ , void * params_ ,
				   double * f_ , gsl_vector * gradient_ );

      static bool name_is_valid( const std::string & name_ );

      enum mode_t
	{
	  MODE_FDF = 0,
	  MODE_F   = 1
	};
     
      enum stopping_t
	{
	  STOPPING_GRADIENT=0,
	  STOPPING_SIZE=1
	};

      class at_step_action
	{
	  
	public:
	  virtual void action( int status_ ,
			       size_t iter_ , 
			       double * x_ , 
			       size_t   dim_ ,
			       double f_ ) = 0;
	  void operator()( int status_ ,
			   size_t iter_ , 
			   double * x_ , 
			   size_t   dim_ ,
			   double f_ )
	    {
	      action(status_,iter_,x_,dim_,f_);
	    }
	  
	};
      
      struct default_step_action : public at_step_action
      {
	virtual void action( int status_ ,
			     size_t iter_ , 
			     double * x_ , 
			     size_t   dim_ , 
			     double f_ );
      };
      
      static default_step_action __default_step_action;
      
    private:

      bool __verbose;
      int  __mode;
      const gsl_multimin_fdfminimizer_type * __algo_fdf;
      const gsl_multimin_fminimizer_type   * __algo_f;
      gsl_multimin_fdfminimizer * __fdfmin;
      gsl_multimin_fminimizer   * __fmin;
      gsl_multimin_function       __f;
      gsl_multimin_function_fdf   __fdf;
      gsl_vector                * __x;
      gsl_vector                * __ss;
      multimin_system           * __sys;

      double                      __fdf_step_size;
      double                      __fdf_tol;
      size_t                      __max_iter;
      size_t                      __n_iter;

      double                      __fval;
      int                         __stopping;
      double                      __epsabs;

      // hook step function:
      at_step_action *            __at_step_action;

      void __init_algorithm( const std::string & name_ );

      void __set_mode( int mode_ );

    public:

      static void print_types( std::ostream & );

      size_t get_n_iter() const;
      
      double get_fval() const;
      
      void unset_step_action();
      
      void set_default_step_action();
      
      void set_step_action( at_step_action & asd_ );
      
      std::string get_name() const;

      void init( const std::string & name_ , 
		 multimin_system & ms_ );

      void reset();

      multimin();

      virtual ~multimin();

      void _at_step_hook( int status_ ,
			  size_t iter_ , 
			  double * x_ , 
			  size_t   dim_ , 
			  double f_ );

      int minimize( double epsabs_ );

      void devel_dump_x() const;
       
    };
  
} // namespace mygsl

#endif // __mygsl__multimin_h

// end of mygsl::multimin.h

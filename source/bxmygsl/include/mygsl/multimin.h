// mygsl::multimin.h

#ifndef __mygsl__multimin_h
#define __mygsl__multimin_h 1

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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
	static const bool FREE_PARAM  = false;
	static const bool FIXED_PARAM = true;

      public:

	double value;
	bool   fixed;
	double min;
	double max;
	double step;
	std::string name;

      private:

	param_entry()
	  {
	  }

      public:

	double get_value() const;

	void set_value( double value_ );

	void set_value_no_check( double value_ );

	static param_entry make_param_entry(
					    const std::string & name_ ,
					    double value_ ,
					    double min_ ,
					    double max_ ,
					    double step_ ,
					    bool fixed_ = param_entry::FREE_PARAM );	
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

    protected:

      std::vector<param_entry> __params;

    public:

      void print_params( std::ostream & ) const;

      const param_entry &  get_param( int i_ ) const;

      void set_param_value( int i_, double val_ );

      double get_param_value( int i_ ) const;

      double get_param_min( int i_ ) const;

      double get_param_max( int i_ ) const;

      double get_param_step( int i_ ) const;

      const std::string & get_param_name( int i_ ) const;

      void add_param( const param_entry & pe_ );

      size_t get_dimension() const;

      virtual int eval_f( const double * x_ , double & f_ ) = 0;

      virtual int eval_df( const double * x_ , double * gradient_ ) = 0;

      virtual int eval_fdf( const double * x_ , double & f_ , double * gradient_ );

      virtual void to_double_star( double * pars_ , size_t dimension_ ) const; // = 0;

      virtual void from_double_star( const double * pars_ , size_t dimension_ ); // = 0;

      multimin_system();

      virtual ~multimin_system();

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

      double                      __fval;
      int                         __stopping;
      double                      __epsabs;

      // hook step function:
      at_step_action *            __at_step_action;

      void __init_algorithm( const std::string & name_ );

      void __set_mode( int mode_ );

    public:

      static void print_types( std::ostream & );
      
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

    };
  

} // namespace mygsl

#endif // __mygsl__multimin_h

// end of mygsl::multimin.h

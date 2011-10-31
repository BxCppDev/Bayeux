// mygsl::multidimensional_minimization.h

#ifndef __mygsl__multidimensional_minimization_h
#define __mygsl__multidimensional_minimization_h 1

#include <string>

#include <gsl/gsl_multimin.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_deriv.h>

#include <mygsl/error.h>
#include <mygsl/unary_eval.h>
#include <mygsl/multiparameter_system.h>

namespace mygsl {

  class multidimensional_minimization_system : 
  public mygsl::multiparameter_system
    {
    public:
      static const double DEFAULT_OUT_OF_LIMIT_SLOPE;

    private:
      bool   _numeric_eval_df_;
      double _out_of_limit_slope_;

    private:
      int _eval_f_MR_ (double & f_);
      int _eval_df_MR_ (double * gradient_);
      int _numerical_eval_df_MR_  (double * gradient_);

    protected:

      // you must provide this method:
      virtual int _eval_f (double & f_) = 0;

      // you may provide this method;
      // if not, the '_numerical_eval_df_MR_' method will be used in place:
      virtual int _eval_df (double * gradient_);

    public:

      int init_params_values ();

      void set_numeric_eval_df (bool = true);

      bool is_numeric_eval_df () const;

      double get_out_of_limit_slope () const;

      int eval_f (const double * x_ , double & f_);

      int eval_df (const double * x_ , double * gradient_);

      int eval_fdf (const double * x_ , 
                    double & f_ , 
                    double * gradient_);

      void to_double_star (double * pars_ , 
                           size_t dimension_) const;

      void from_double_star (const double * pars_ , 
                             size_t dimension_);

      // ctor:
      multidimensional_minimization_system (double slope_ = DEFAULT_OUT_OF_LIMIT_SLOPE,
                                            bool use_numeric_eval_ = false);

      // dtor:
      virtual ~multidimensional_minimization_system ();

    public:

      struct func_eval_f_param : public mygsl::unary_eval
        {
          int free_param_index;
          multidimensional_minimization_system * sys;

          func_eval_f_param (int free_param_index_, 
                             multidimensional_minimization_system & sys_)
            {
              free_param_index = free_param_index_;
              sys = &sys_;
            }
        
          double eval (double x_) const 
          {
            return func_eval_f_MR (x_, const_cast<func_eval_f_param*> (this));
          }

        };

      static double func_eval_f_MR  (double x_, void * params_);

      void plot_f (const string & prefix_, int mode_ = 0) const;

    };

  /********************************************************/

  class multidimensional_minimization
  {
  public:
    static const size_t DEFAULT_MAX_ITER;
    static const size_t DEFAULT_MODULO_ITER;
    static const bool   DEFAULT_VERBOSE;
    static const double DEFAULT_EPSABS;

    static bool g_debug;

    static double f   (const gsl_vector * v_ , void * params_);

    static void   df  (const gsl_vector * v_ , void * params_ , 
                       gsl_vector * gradient_);

    static void   fdf (const gsl_vector * v_ , void * params_ ,
                       double * f_ , gsl_vector * gradient_);

    static bool name_is_valid (const std::string & name_);

    enum mode_t
    {
      MODE_FDF = 0,
      MODE_F   = 1
    };
     
    enum stopping_t
    {
      STOPPING_GRADIENT = 0,
      STOPPING_SIZE = 1
    };

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

    bool _verbose_;
    int  _mode_;
    const gsl_multimin_fdfminimizer_type * _algo_fdf_;
    const gsl_multimin_fminimizer_type   * _algo_f_;
    gsl_multimin_fdfminimizer * _fdfmin_;
    gsl_multimin_fminimizer   * _fmin_;
    gsl_multimin_function       _f_;
    gsl_multimin_function_fdf   _fdf_;
    gsl_vector                * _x_;
    gsl_vector                * _ss_;
    multidimensional_minimization_system * _sys_;

    double                      _fdf_step_size_;
    double                      _fdf_tol_;
    size_t                      _max_iter_;
    size_t                      _n_iter_;
    size_t                      _modulo_iter_;

    double                      _fval_;
    int                         _stopping_;
    double                      _epsabs_;

    // hook step function:
    at_step_action *            _at_step_action_;

    void _init_algorithm_ (const std::string & name_);

    void _set_mode_ (int mode_);

  public:

    static void print_types (std::ostream &);

    void set_max_iter (size_t max_);

    void set_modulo_iter (size_t modulo_);

    size_t get_n_iter () const;

    size_t get_modulo_iter () const;

    size_t get_max_iter () const;
       
    double get_epsabs () const;
      
    double get_fval () const;
      
    void unset_step_action ();
      
    void set_default_step_action ();
      
    void set_step_action (at_step_action & asd_);
      
    std::string get_name () const;

    void init (const std::string & name_ , 
               multidimensional_minimization_system & ms_);

    void reset ();

    multidimensional_minimization ();

    virtual ~multidimensional_minimization ();

  protected:

    void _at_step_hook (int      status_ ,
                        size_t   iter_ , 
                        double * x_ , 
                        size_t   dim_ , 
                        double   f_);

  public:

    int minimize (double epsabs_);

    void devel_dump_x () const;
       
  };
  
} // namespace mygsl

#endif // __mygsl__multidimensional_minimization_h

// end of mygsl::multidimensional_minimization.h

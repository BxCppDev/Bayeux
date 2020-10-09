/// \file mygsl/multimin.h

#ifndef MYGSL_MULTIMIN_H
#define MYGSL_MULTIMIN_H 1

// Standard library:
#include <iostream>
#include <string>
#include <vector>

// Third party:
// - GSL:
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_vector.h>

// This project:
#include <mygsl/i_unary_function.h>

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

      param_entry ()
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

      bool is_fixed () const { return type==TYPE_FIXED; }

      bool is_free () const { return type==TYPE_FREE; }

      bool is_auto () const { return type==TYPE_AUTO; }

      bool has_limit () const {return limit!=LIMIT_NO; }

      bool has_no_limit () const {return limit==LIMIT_NO; }

      bool has_min () const {return limit&LIMIT_MIN; }

      bool has_max () const {return limit&LIMIT_MAX; }

      bool is_in_range () const;

      double get_dist_to_min () const;

      double get_dist_to_max () const;

      double get_dist_to_limit () const;

      double get_value () const;

      double get_step () const;

      double get_min () const;

      double get_max () const;

      void set_value (double value_);

      void set_value_no_check (double value_);

      static param_entry make_param_entry_auto (
                                                const std::string & name_);

      static param_entry make_param_entry_fixed (
                                                 const std::string & name_ ,
                                                 double value_);

      static param_entry make_param_entry_ranged (
                                                  const std::string & name_ ,
                                                  double value_ ,
                                                  double min_ ,
                                                  double max_ ,
                                                  double step_ ,
                                                  bool fixed_ = false);
    };

    // predicate:
    struct param_has_name : std::unary_function<bool,param_entry>
    {
      std::string _name_;
      param_has_name (const std::string & name_) : _name_ (name_)
      {
        return;
      }
      bool operator () (const param_entry & pe_)
      {
        return pe_.name == _name_;
      }
    };

    // predicate:
    struct param_is_free : std::unary_function<bool,param_entry>
    {
      param_is_free ()
      {
        return;
      }
      bool operator () (const param_entry & pe_)
      {
        return pe_.is_free ();
      }
    };

    // predicate:
    struct param_is_fixed : std::unary_function<bool,param_entry>
    {
      param_is_fixed ()
      {
        return;
      }
      bool operator () (const param_entry & pe_)
      {
        return pe_.is_fixed ();
      }
    };

    // predicate:
    struct param_is_auto : std::unary_function<bool,param_entry>
    {
      param_is_auto ()
      {
        return;
      }
      bool operator () (const param_entry & pe_)
      {
        return pe_.is_auto ();
      }
    };

  protected:

    bool                     _lock_params;
    std::vector<param_entry> _params;

  private:

    size_t _free_dimension_;
    size_t _auto_dimension_;

    void _update_free_dimension_ ();

    void _update_auto_dimension_ ();

    void _update_dimensions_ ();

  public:

    void lock_params ();

    void unlock_params ();

    bool is_lock_params () const;

    void print_params (std::ostream &) const;

    void print_params2 (std::ostream &) const;

    const param_entry & get_param (int i_) const;

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

  protected:

    virtual int _prepare_values ();

    virtual int _auto_values ();

    virtual int _eval_f (double & f_) = 0;

    virtual int _eval_df (double * gradient_) = 0;

  public:

    virtual int prepare_values ();

    virtual int auto_values ();

    virtual int eval_f (const double * x_ , double & f_);

    virtual int eval_df (const double * x_ , double * gradient_);

    virtual int eval_fdf (const double * x_ ,
                          double & f_ ,
                          double * gradient_);

    void to_double_star (double * pars_ ,
                         size_t dimension_) const;

    virtual void from_double_star (const double * pars_ ,
                                   size_t dimension_);

    multimin_system ();

    virtual ~multimin_system ();

    void dump (std::ostream & out_) const;

  };

  class multimin
  {
  public:
    static const size_t DEFAULT_MAX_ITER;
    static const bool   DEFAULT_VERBOSE;

    static double multimin_f    (const gsl_vector * v_ , void * params_);

    static void   multimin_df   (const gsl_vector * v_ , void * params_ ,
                                 gsl_vector * gradient_);

    static void   multimin_fdf  (const gsl_vector * v_ , void * params_ ,
                                 double * f_ , gsl_vector * gradient_);

    static bool name_is_valid (const std::string & name_);

    enum mode_type
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
        action (status_,iter_,x_,dim_,f_);
      }

    };

    struct default_step_action : public at_step_action
    {
      void action (int status_ ,
                           size_t iter_ ,
                           double * x_ ,
                           size_t   dim_ ,
                           double f_) override;
    };

    static default_step_action _default_step_action_;

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
    multimin_system           * _sys_;

    double                      _fdf_step_size_;
    double                      _fdf_tol_;
    size_t                      _max_iter_;
    size_t                      _n_iter_;

    double                      _fval_;
    int                         _stopping_;
    double                      _epsabs_;

    // hook step function:
    at_step_action *            _at_step_action_;

    void _init_algorithm_ (const std::string & name_);

    void _set_mode_ (int mode_);

  public:

    static void print_types (std::ostream &);

    size_t get_n_iter () const;

    double get_fval () const;

    void unset_step_action ();

    void set_default_step_action ();

    void set_step_action (at_step_action & asd_);

    std::string get_name () const;

    void init (const std::string & name_ ,
               multimin_system & ms_);

    void reset ();

    multimin ();

    virtual ~multimin ();

  protected:

    void _at_step_hook (int status_ ,
                        size_t iter_ ,
                        double * x_ ,
                        size_t   dim_ ,
                        double f_);

  public:

    int minimize (double epsabs_);

    void devel_dump_x () const;

  };

} // namespace mygsl

#endif // MYGSL_MULTIMIN_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

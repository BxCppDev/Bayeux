/// \file mygsl/ode.h

#ifndef MYGSL_ODE_H
#define MYGSL_ODE_H 1

// Standard library:
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - GSL:
#include <gsl/gsl_odeiv.h>

namespace mygsl {

  /// \brief System for ODE solving
  struct ode_system
  {
  public:

    virtual bool has_jacobian () const;

    virtual size_t get_dimension () const = 0;

    virtual int compute_derivatives ( double         t_ ,
                                      const double * y_ ,
                                      double       * f_  ) = 0;

    virtual int compute_jacobian ( double         t_    ,
                                   const double * y_    ,
                                   double  *      dfdy_ ,
                                   double  *      dfdt_  );

    virtual void to_double_star ( double * y_ , size_t dimension_ ) const = 0;

    virtual void from_double_star ( const double * y_ , size_t dimension_ ) = 0;


    // GSL interface:
    static int gsl_ode_function ( double t_ ,
                                  const double * y_ ,
                                  double * f_ ,
                                  void * params_ );

    static int gsl_ode_jacobian ( double t_ ,
                                  const double * y_ ,
                                  double * dfdy_ ,
                                  double * dfdt_ ,
                                  void * params_ );
  };


  class ode_driver
  {
  public:

    static const std::string DEFAULT_TYPE;

    struct at_step_action
    {
    public:
      virtual void action ( double   t_ ,
                            double * y_ ,
                            size_t   dim_ ) = 0;

      void operator () ( double   t_ ,
                         double * y_ ,
                         size_t   dim_  )
      {
        action (t_,y_,dim_);
      }

    };

  public:

    struct default_step_action : public at_step_action
    {
      void action ( double t_ , double * y_ , size_t dim_ ) override;
    };

    static default_step_action _default_step_action_;

    bool is_regular () const;

    void regular ();

    void not_regular ();

    void unset_step_action ();

    void set_default_step_action ();

    void set_step_action ( at_step_action & asd_ );

    static bool type_is_valid ( const std::string & type_ );

    static bool type_requires_jacobian ( const std::string & type_ );

    static void print_types ( std::ostream & );

    const std::string & get_type () const;

    void set_type ( const std::string & type_ );

    size_t get_dimension () const;

    ode_driver ( ode_system & sys_ ,
                 const std::string & type_ = DEFAULT_TYPE ,
                 double epsabs_   = 1.e-6,
                 double epsrel_   = 1.e-6,
                 bool   regular_  = false );

    void reset ();

    virtual ~ode_driver ();

    void init ();

    int run ( double t_begin_ , double t_end_ , double h_ );

  protected:

    void _at_step_hook ();

  private:

    void _init_step_ ();

    int _inner_run_not_regular_ ();

    int _inner_run_regular_ ();

    void _init_run_ ();

    void _done_run_ ();

  private:
    ode_system *                _ode_sys_;

    // GSL stuff:
    std::string                 _type_;
    const gsl_odeiv_step_type * _step_type_;
    gsl_odeiv_step *            _step_;
    gsl_odeiv_control *         _control_;
    gsl_odeiv_evolve *          _evolve_;
    gsl_odeiv_system            _system_;
    bool                        _has_jacobian_;
    bool                        _regular_;
    double                      _epsabs_;
    double                      _epsrel_;

    // internal state:
    double                      _t_begin_;
    double                      _t_end_;
    double                      _h_;
    double                      _t_;
    double *                    _y_;

    // hook step function:
    at_step_action *            _at_step_action_;

  };

}

#endif //  MYGSL_ODE_H

/* Local Variables: */
/* mode: c++        */
/* coding: utf-8    */
/* End:             */

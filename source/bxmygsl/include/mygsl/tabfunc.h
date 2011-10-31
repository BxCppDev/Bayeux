// -*- mode: c++; -*- 
/* mygsl::tabfunc.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2007-12-07
 * Last modified: 2007-12-07
 * 
 * License: 
 * 
 * 
 * Description: 
 *   Tabulated function
 * History: 
 * 
 */

#ifndef __mygsl__tabfunc_h
#define __mygsl__tabfunc_h 1

#include <iostream>
#include <map>
#include <string>

#include <mygsl/unary_eval.h>

#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

namespace mygsl {

  class tabulated_function : public unary_eval
  {
  public:

    typedef std::map<double,double> points_map_t;

    static const std::string LINEAR_INTERP_NAME;
    static const std::string POLYNOMIAL_INTERP_NAME;
    static const std::string CSPLINE_INTERP_NAME;
    static const std::string CSPLINE_PERIODIC_INTERP_NAME;
    static const std::string AKIMA_INTERP_NAME;
    static const std::string AKIMA_PERIODIC_INTERP_NAME;
    static const std::string DEFAULT_INTERP_NAME;

    static bool g_debug;

  private:

    bool               _verbose_;
    bool               _table_locked_;
    std::string        _interpolator_name_;
    points_map_t       _points_;
    gsl_spline *       _gs_;
    gsl_interp_accel * _giacc_;
    double             _x_min_;
    double             _x_max_;

  public:

    bool is_verbose () const;

    void set_verbose (bool v_ = true);

    bool is_valid (double x_) const;

    const std::string & interpolator_name () const;

    const points_map_t & points () const;

    static const std::string & default_interpolator_name ();

    static bool interpolator_name_is_valid (const std::string & name_);

    void scale (double s_);

    size_t size () const;

    bool is_table_locked () const;

    void lock_table (const std::string & interp_name_ = "");

    void unlock_table ();

    void relock_table (const std::string & interp_name_ = "");

    void add_point (double x_ , double y_ , bool lock_after_ = false);

    tabulated_function (const std::string & interp_name_ = DEFAULT_INTERP_NAME);

    tabulated_function (const tabulated_function & tab_func_);

    tabulated_function & operator= (const tabulated_function & tab_func_);
    
    double x_min () const;

    double x_max () const;

    void reset ();

    virtual ~tabulated_function ();

    virtual double eval (double x_) const;

    virtual void tabfunc_load (std::istream & in_ , void * context_ = 0);

    virtual void tabfunc_store (std::ostream & out_ , void * context_ = 0) const;

    void print_points (std::ostream & out_ , 
                       const std::string & header_comment_ = "" ,
                       const std::string & footer_comment_ = "") const;

  };

  typedef tabulated_function tabfunc;

} // end of namespace mygsl

#endif // __mygsl__tabfunc_h

// end of mygsl::tabfunc.h

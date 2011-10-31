// -*- mode: c++; -*- 
/* mygsl::multi_eval.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-10-04
 * Last modified: 2009-10-04
 * 
 * License: 
 * 
 * 
 * Description: 
 *
 * History: 
 * 
 */

#ifndef __mygsl__multi_eval_h
#define __mygsl__multi_eval_h 1

#include <vector>

#include <mygsl/unary_eval.h>
#include <mygsl/interval.h>

using namespace std;

namespace mygsl {

  class multi_eval
  {
  public:

    typedef vector<interval> multi_domain;

  public:

    multi_eval (size_t dimension_);

    virtual ~multi_eval ();

  protected:

    virtual double eval (const double * x_) const = 0;

  private:
    
    void _check_dimension_ (size_t dim_) const;

  public:

    size_t get_dimension () const;
  
    interval & get_domain (int i_);
  
    const interval & get_domain (int i_) const;

    bool is_valid (const double * x_) const;

    virtual bool is_valid (int i_, double x_) const;

    double evaluate (double x_) const;

    double evaluate (double x_, double y_) const;

    double evaluate (double x_, double y_, double z_) const;

    double evaluate (const vector<double> & v_) const;

    double evaluate (const double * x_) const;

    double operator () (const double * x_) const;
    
    // to feed the GSL gsl_function interface:
    static double g_function (const double * x_, void * functor_);

  private:

    multi_domain _domains_;
    
  };

  class unary_eval_from_multi : public unary_eval
  {
    
  public:

    bool is_valid (double x_) const;

    const double & param (int i_) const;

    double & param (int i_);

    void init (const multi_eval & multi_eval_, 
	       int i_, 
	       const vector<double> & params_);

    void set_index (int i_);

    unary_eval_from_multi (const multi_eval & multi_eval_, 
			   int i_, 
			   const vector<double> & params_);

    unary_eval_from_multi (const multi_eval & multi_eval_, 
			   int i_, 
			   const double * params_);

    virtual ~unary_eval_from_multi ();

  protected:

   double eval (double x_) const;

  private:

    int                _index_;
    const multi_eval * _multi_eval_;
    vector<double>     _params_;

  };

} // end of namespace mygsl

#endif // __mygsl__multi_eval_h

// end of mygsl::multi_eval.h

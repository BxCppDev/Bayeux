// -*- mode: c++; -*- 
/* mygsl::unary_eval.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2007-12-07
 * Last modified: 2007-12-07
 * 
 * License: 
 * 
 * 
 * Description: 
 *   Abstract interface for R to R functor with a default 
 *   interface as the domain of validity
 *
 * History: 
 * 
 */

#ifndef __mygsl__unary_eval_h
#define __mygsl__unary_eval_h 1

#include <functional>

#include <mygsl/interval.h>

namespace mygsl {

  class unary_eval : public std::unary_function<double,double>
  {
  public:
    typedef double (*native_eval) (double);

  private:

    interval __domain; // domain of validity

  public:

    interval & get_domain ();

    const interval & get_domain ( ) const;

    virtual bool is_valid (double x_) const;

    unary_eval ();

    unary_eval (const interval & domain_);

    virtual ~unary_eval ();

    // mandatory:
    virtual double eval (double x_) const = 0;

    double evaluate (double x_) const;

    double operator () (double x_) const;
    
    // to feed the GSL gsl_function interface:
    static double g_function (double x_, void * functor_);

  };


  class unary_eval_from_native : public unary_eval
  {
  private:

    const native_eval * __native_eval;
    
  public:

    void set_native_eval (const native_eval &);

    unary_eval_from_native (const native_eval &);

    unary_eval_from_native (const native_eval &, const interval & domain_);

    virtual ~unary_eval_from_native ();

  protected:

    virtual double eval (double x_) const;

  };

} // end of namespace mygsl

#endif // __mygsl__unary_eval_h

// end of mygsl::unary_eval.h

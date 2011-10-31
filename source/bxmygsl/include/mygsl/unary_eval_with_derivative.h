// -*- mode: c++; -*- 
/* mygsl::unary_eval_with_derivative.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2007-12-07
 * Last modified: 2007-12-07
 * 
 * License: 
 * 
 * 
 * Description: 
 *   Abstract interface for R to R functor
 *
 * History: 
 * 
 */

#ifndef __mygsl__unary_eval_with_derivative_h
#define __mygsl__unary_eval_with_derivative_h 1


#include <mygsl/unary_eval.h>
#include <mygsl/interval.h>

using namespace std;

namespace mygsl {

  class unary_eval_with_derivative : public unary_eval
  {

  public:

    unary_eval_with_derivative ();

    unary_eval_with_derivative (const interval & domain_);
      
    virtual double eval_f (double x_) const = 0;

    virtual double eval_df_numeric (double x_) const;
      
    virtual double eval_df (double x_) const;

    virtual void eval_fdf (double x_, double & f_, double & df_) const;
      
    double eval (double x_) const;
      

  };
   
  class unary_eval_promoted_with_numeric_derivative : public unary_eval_with_derivative
  {
  private:

    const unary_eval * _eval_;
      
  public:

    bool is_valid (double x_) const;
      
    unary_eval_promoted_with_numeric_derivative (const unary_eval & eval_);
      
    double eval_f (double x_) const;
      
  };

} // end of namespace mygsl

#endif // __mygsl__unary_eval_with_derivative_h

// end of mygsl::unary_eval_with_derivative.h

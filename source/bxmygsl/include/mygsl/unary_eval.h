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
 *   Abstract interface for R to R functor
 *
 * History: 
 * 
 */

#ifndef __mygsl__unary_eval_h
#define __mygsl__unary_eval_h 1

#include <functional>

namespace mygsl {

  struct unary_eval : public std::unary_function<double,double>
  {
    virtual double eval (double x_) const = 0;

    inline double 
    operator () (double x_) const
    {
      return eval (x_);
    }

  };

} // end of namespace mygsl

#endif // __mygsl__unary_eval_h

// end of mygsl::unary_eval.h

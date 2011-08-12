// -*- mode: c++; -*- 
/* mygsl::eval_slot.h
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

#ifndef __mygsl__eval_slot_h
#define __mygsl__eval_slot_h 1

#include <functional>

#include <mygsl/unary_eval.h>
#include <mygsl/unary_eval_with_derivative.h>

namespace mygsl {

  class eval_slot : public unary_eval
  {
  private:
    bool  __delete;
    const unary_eval * __eval;

  public:
    eval_slot (const unary_eval & eval_);

    eval_slot (const unary_eval * eval_);

  protected:

    virtual double eval (double x_) const;
    
  };


} // end of namespace mygsl

#endif // __mygsl__eval_slot_h

// end of mygsl::eval_slot.h

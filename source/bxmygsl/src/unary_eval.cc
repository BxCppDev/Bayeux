// mygsl::unary_eval.cc

#include <mygsl/unary_eval.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <gsl/gsl_math.h>

namespace mygsl {
  
  unary_eval::unary_eval ()
  {
    __domain = interval::make_no_limit ();
    return;
  }
  
  unary_eval::unary_eval (const interval & domain_)
  {
    __domain = domain_;
    return;
  }
  
  unary_eval::~unary_eval ()
  {
    return;
  }

  bool unary_eval::is_valid (double x_) const
  {
    return __domain.is_in (x_);
  }
  
  interval & unary_eval::get_domain ()
  {
    return __domain;
  }  
  
  const interval & unary_eval::get_domain () const
  {
    return __domain;
  }  

  double unary_eval::operator () (double x_) const
  {
    return evaluate (x_);
  }
  
  double unary_eval::evaluate (double x_) const
  {
    if (! is_valid (x_))
      {
	ostringstream message;
	message << "unary_eval::evaluate: "
		<< "value '" << x_ << "' is out of the evaluation domain!";
	throw runtime_error (message.str ());
      }
    return eval (x_);
  }

  // static:
  double unary_eval::g_function (double x_, void * functor_)
  {
    const unary_eval * an_eval = static_cast<const unary_eval *>(functor_);
    return (*an_eval) (x_);
  };

  /***************************************/

  
  void unary_eval_from_native::set_native_eval (const native_eval & native_)
  {
    __native_eval = &native_;
    /*
     * clog << "DEVEL: addr(native) = " << __native_eval << endl;
     * clog << "DEVEL: addr(native) = " << (void *) (*__native_eval) << endl;
     */
    return;
  }
  
  unary_eval_from_native::unary_eval_from_native (const native_eval & native_)
    : unary_eval ()
  {
    set_native_eval (native_);
    return;
  }
  
  unary_eval_from_native::unary_eval_from_native (const native_eval & native_,
						  const interval & domain_)
    : unary_eval (domain_)
  {
    set_native_eval (native_);
    return;
  }
  
  unary_eval_from_native::~unary_eval_from_native ()
  {
    __native_eval = 0;
    return;
  }
  
  double unary_eval_from_native::eval (double x_) const 
  {
    return (*__native_eval) (x_);
  }
  
}

// end of mygsl::unary_eval.cc

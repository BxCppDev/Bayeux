/// zero_function.cc

// Ourselves:
#include <mygsl/zero_function.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(zero_function,
                                             "mygsl::zero_function")

  zero_function::zero_function()
  {
    return;
  }

  zero_function::~zero_function()
  {
    return;
  }

  double zero_function::_eval(double) const
  {
    return 0.0;
  }

} // namespace mygsl
